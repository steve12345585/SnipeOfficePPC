/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include "externalrefmgr.hxx"
#include "document.hxx"
#include "token.hxx"
#include "tokenarray.hxx"
#include "address.hxx"
#include "tablink.hxx"
#include "docsh.hxx"
#include "scextopt.hxx"
#include "rangenam.hxx"
#include "cell.hxx"
#include "viewdata.hxx"
#include "tabvwsh.hxx"
#include "sc.hrc"
#include "globstr.hrc"

#include "sfx2/app.hxx"
#include "sfx2/docfilt.hxx"
#include "sfx2/docfile.hxx"
#include "sfx2/fcontnr.hxx"
#include "sfx2/sfxsids.hrc"
#include "sfx2/objsh.hxx"
#include "svl/broadcast.hxx"
#include "svl/smplhint.hxx"
#include "svl/itemset.hxx"
#include "svl/stritem.hxx"
#include "svl/urihelper.hxx"
#include "svl/zformat.hxx"
#include "sfx2/linkmgr.hxx"
#include "tools/urlobj.hxx"
#include "unotools/ucbhelper.hxx"
#include "unotools/localfilehelper.hxx"
#include "vcl/msgbox.hxx"

#include <memory>
#include <algorithm>

#include <boost/scoped_ptr.hpp>

using ::std::auto_ptr;
using ::com::sun::star::uno::Any;
using ::rtl::OUString;
using ::std::vector;
using ::std::find;
using ::std::find_if;
using ::std::remove_if;
using ::std::distance;
using ::std::pair;
using ::std::list;
using ::std::unary_function;
using namespace formula;

#define SRCDOC_LIFE_SPAN     6000       // 1 minute (in 100th of a sec)
#define SRCDOC_SCAN_INTERVAL 1000*5     // every 5 seconds (in msec)

namespace {

class TabNameSearchPredicate : public unary_function<ScExternalRefCache::TableName, bool>
{
public:
    explicit TabNameSearchPredicate(const OUString& rSearchName) :
        maSearchName(ScGlobal::pCharClass->uppercase(rSearchName))
    {
    }

    bool operator()(const ScExternalRefCache::TableName& rTabNameSet) const
    {
        // Ok, I'm doing case insensitive search here.
        return rTabNameSet.maUpperName.equals(maSearchName);
    }

private:
    OUString maSearchName;
};

class FindSrcFileByName : public unary_function<ScExternalRefManager::SrcFileData, bool>
{
public:
    FindSrcFileByName(const OUString& rMatchName) :
        mrMatchName(rMatchName)
    {
    }

    bool operator()(const ScExternalRefManager::SrcFileData& rSrcData) const
    {
        return rSrcData.maFileName.equals(mrMatchName);
    }

private:
    const OUString& mrMatchName;
};

class NotifyLinkListener : public unary_function<ScExternalRefManager::LinkListener*,  void>
{
public:
    NotifyLinkListener(sal_uInt16 nFileId, ScExternalRefManager::LinkUpdateType eType) :
        mnFileId(nFileId), meType(eType) {}

    NotifyLinkListener(const NotifyLinkListener& r) :
        mnFileId(r.mnFileId), meType(r.meType) {}

    void operator() (ScExternalRefManager::LinkListener* p) const
    {
        p->notify(mnFileId, meType);
    }
private:
    sal_uInt16 mnFileId;
    ScExternalRefManager::LinkUpdateType meType;
};

struct UpdateFormulaCell : public unary_function<ScFormulaCell*, void>
{
    void operator() (ScFormulaCell* pCell) const
    {
        // Check to make sure the cell really contains ocExternalRef.
        // External names, external cell and range references all have a
        // ocExternalRef token.
        const ScTokenArray* pCode = pCell->GetCode();
        if (!pCode->HasExternalRef())
            return;

        ScTokenArray* pArray = pCell->GetCode();
        if (pArray)
            // Clear the error code, or a cell with error won't get re-compiled.
            pArray->SetCodeError(0);

        pCell->SetCompile(true);
        pCell->CompileTokenArray();
        pCell->SetDirty();
    }
};

class RemoveFormulaCell : public unary_function<pair<const sal_uInt16, ScExternalRefManager::RefCellSet>, void>
{
public:
    explicit RemoveFormulaCell(ScFormulaCell* p) : mpCell(p) {}
    void operator() (pair<const sal_uInt16, ScExternalRefManager::RefCellSet>& r) const
    {
        r.second.erase(mpCell);
    }
private:
    ScFormulaCell* mpCell;
};

class ConvertFormulaToStatic : public unary_function<ScFormulaCell*, void>
{
public:
    explicit ConvertFormulaToStatic(ScDocument* pDoc) : mpDoc(pDoc) {}
    void operator() (ScFormulaCell* pCell) const
    {
        ScAddress aPos = pCell->aPos;

        // We don't check for empty cells because empty external cells are
        // treated as having a value of 0.

        if (pCell->IsValue())
        {
            // Turn this into value cell.
            double fVal = pCell->GetValue();
            mpDoc->PutCell(aPos, new ScValueCell(fVal));
        }
        else
        {
            // string cell otherwise.
            rtl::OUString aVal = pCell->GetString();
            mpDoc->PutCell(aPos, new ScStringCell(aVal));
        }
    }
private:
    ScDocument* mpDoc;
};

/**
 * Check whether a named range contains an external reference to a
 * particular document.
 */
bool hasRefsToSrcDoc(ScRangeData& rData, sal_uInt16 nFileId)
{
    ScTokenArray* pArray = rData.GetCode();
    if (!pArray)
        return false;

    pArray->Reset();
    ScToken* p = static_cast<ScToken*>(pArray->GetNextReference());
    for (; p; p = static_cast<ScToken*>(pArray->GetNextReference()))
    {
        if (!p->IsExternalRef())
            continue;

        if (p->GetIndex() == nFileId)
            return true;
    }
    return false;
}

class EraseRangeByIterator : unary_function<ScRangeName::iterator, void>
{
    ScRangeName& mrRanges;
public:
    EraseRangeByIterator(ScRangeName& rRanges) : mrRanges(rRanges) {}
    void operator() (const ScRangeName::iterator& itr)
    {
        mrRanges.erase(itr);
    }
};

/**
 * Remove all named ranges that contain references to specified source
 * document.
 */
void removeRangeNamesBySrcDoc(ScRangeName& rRanges, sal_uInt16 nFileId)
{
    ScRangeName::iterator itr = rRanges.begin(), itrEnd = rRanges.end();
    vector<ScRangeName::iterator> v;
    for (; itr != itrEnd; ++itr)
    {
        if (hasRefsToSrcDoc(*itr->second, nFileId))
            v.push_back(itr);
    }
    for_each(v.begin(), v.end(), EraseRangeByIterator(rRanges));
}

}

// ============================================================================

ScExternalRefCache::Table::Table()
    : meReferenced( REFERENCED_MARKED )
      // Prevent accidental data loss due to lack of knowledge.
{
}

ScExternalRefCache::Table::~Table()
{
}

void ScExternalRefCache::Table::setReferencedFlag( ScExternalRefCache::Table::ReferencedFlag eFlag )
{
    meReferenced = eFlag;
}

void ScExternalRefCache::Table::setReferenced( bool bReferenced )
{
    if (meReferenced != REFERENCED_PERMANENT)
        meReferenced = (bReferenced ? REFERENCED_MARKED : UNREFERENCED);
}

ScExternalRefCache::Table::ReferencedFlag ScExternalRefCache::Table::getReferencedFlag() const
{
    return meReferenced;
}

bool ScExternalRefCache::Table::isReferenced() const
{
    return meReferenced != UNREFERENCED;
}

void ScExternalRefCache::Table::setCell(SCCOL nCol, SCROW nRow, TokenRef pToken, sal_uLong nFmtIndex, bool bSetCacheRange)
{
    using ::std::pair;
    RowsDataType::iterator itrRow = maRows.find(nRow);
    if (itrRow == maRows.end())
    {
        // This row does not exist yet.
        pair<RowsDataType::iterator, bool> res = maRows.insert(
            RowsDataType::value_type(nRow, RowDataType()));

        if (!res.second)
            return;

        itrRow = res.first;
    }

    // Insert this token into the specified column location.  I don't need to
    // check for existing data.  Just overwrite it.
    RowDataType& rRow = itrRow->second;
    ScExternalRefCache::Cell aCell;
    aCell.mxToken = pToken;
    aCell.mnFmtIndex = nFmtIndex;
    rRow.insert(RowDataType::value_type(nCol, aCell));
    if (bSetCacheRange)
        setCachedCell(nCol, nRow);
}

ScExternalRefCache::TokenRef ScExternalRefCache::Table::getCell(SCCOL nCol, SCROW nRow, sal_uInt32* pnFmtIndex) const
{
    RowsDataType::const_iterator itrTable = maRows.find(nRow);
    if (itrTable == maRows.end())
    {
        // this table doesn't have the specified row.
        return getEmptyOrNullToken(nCol, nRow);
    }

    const RowDataType& rRowData = itrTable->second;
    RowDataType::const_iterator itrRow = rRowData.find(nCol);
    if (itrRow == rRowData.end())
    {
        // this row doesn't have the specified column.
        return getEmptyOrNullToken(nCol, nRow);
    }

    const Cell& rCell = itrRow->second;
    if (pnFmtIndex)
        *pnFmtIndex = rCell.mnFmtIndex;

    return rCell.mxToken;
}

bool ScExternalRefCache::Table::hasRow( SCROW nRow ) const
{
    RowsDataType::const_iterator itrRow = maRows.find(nRow);
    return itrRow != maRows.end();
}

void ScExternalRefCache::Table::getAllRows(vector<SCROW>& rRows, SCROW nLow, SCROW nHigh) const
{
    vector<SCROW> aRows;
    aRows.reserve(maRows.size());
    RowsDataType::const_iterator itr = maRows.begin(), itrEnd = maRows.end();
    for (; itr != itrEnd; ++itr)
        if (nLow <= itr->first && itr->first <= nHigh)
            aRows.push_back(itr->first);

    // hash map is not ordered, so we need to explicitly sort it.
    ::std::sort(aRows.begin(), aRows.end());
    rRows.swap(aRows);
}

::std::pair< SCROW, SCROW > ScExternalRefCache::Table::getRowRange() const
{
    ::std::pair< SCROW, SCROW > aRange( 0, 0 );
    if( !maRows.empty() )
    {
        // iterate over entire container (hash map is not sorted by key)
        RowsDataType::const_iterator itr = maRows.begin(), itrEnd = maRows.end();
        aRange.first = itr->first;
        aRange.second = itr->first + 1;
        while( ++itr != itrEnd )
        {
            if( itr->first < aRange.first )
                aRange.first = itr->first;
            else if( itr->first >= aRange.second )
                aRange.second = itr->first + 1;
        }
    }
    return aRange;
}

void ScExternalRefCache::Table::getAllCols(SCROW nRow, vector<SCCOL>& rCols, SCCOL nLow, SCCOL nHigh) const
{
    RowsDataType::const_iterator itrRow = maRows.find(nRow);
    if (itrRow == maRows.end())
        // this table doesn't have the specified row.
        return;

    const RowDataType& rRowData = itrRow->second;
    vector<SCCOL> aCols;
    aCols.reserve(rRowData.size());
    RowDataType::const_iterator itrCol = rRowData.begin(), itrColEnd = rRowData.end();
    for (; itrCol != itrColEnd; ++itrCol)
        if (nLow <= itrCol->first && itrCol->first <= nHigh)
            aCols.push_back(itrCol->first);

    // hash map is not ordered, so we need to explicitly sort it.
    ::std::sort(aCols.begin(), aCols.end());
    rCols.swap(aCols);
}

::std::pair< SCCOL, SCCOL > ScExternalRefCache::Table::getColRange( SCROW nRow ) const
{
    ::std::pair< SCCOL, SCCOL > aRange( 0, 0 );

    RowsDataType::const_iterator itrRow = maRows.find( nRow );
    if (itrRow == maRows.end())
        // this table doesn't have the specified row.
        return aRange;

    const RowDataType& rRowData = itrRow->second;
    if( !rRowData.empty() )
    {
        // iterate over entire container (hash map is not sorted by key)
        RowDataType::const_iterator itr = rRowData.begin(), itrEnd = rRowData.end();
        aRange.first = itr->first;
        aRange.second = itr->first + 1;
        while( ++itr != itrEnd )
        {
            if( itr->first < aRange.first )
                aRange.first = itr->first;
            else if( itr->first >= aRange.second )
                aRange.second = itr->first + 1;
        }
    }
    return aRange;
}

void ScExternalRefCache::Table::getAllNumberFormats(vector<sal_uInt32>& rNumFmts) const
{
    RowsDataType::const_iterator itrRow = maRows.begin(), itrRowEnd = maRows.end();
    for (; itrRow != itrRowEnd; ++itrRow)
    {
        const RowDataType& rRowData = itrRow->second;
        RowDataType::const_iterator itrCol = rRowData.begin(), itrColEnd = rRowData.end();
        for (; itrCol != itrColEnd; ++itrCol)
        {
            const Cell& rCell = itrCol->second;
            rNumFmts.push_back(rCell.mnFmtIndex);
        }
    }
}

bool ScExternalRefCache::Table::isRangeCached(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2) const
{
    return maCachedRanges.In(ScRange(nCol1, nRow1, 0, nCol2, nRow2, 0));
}

void ScExternalRefCache::Table::setCachedCell(SCCOL nCol, SCROW nRow)
{
    setCachedCellRange(nCol, nRow, nCol, nRow);
}

void ScExternalRefCache::Table::setCachedCellRange(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2)
{
    ScRange aRange(nCol1, nRow1, 0, nCol2, nRow2, 0);
    if ( maCachedRanges.empty() )
        maCachedRanges.Append(aRange);
    else
        maCachedRanges.Join(aRange);

    String aStr;
    maCachedRanges.Format(aStr, SCA_VALID);
}

void ScExternalRefCache::Table::setWholeTableCached()
{
    setCachedCellRange(0, 0, MAXCOL, MAXROW);
}

bool ScExternalRefCache::Table::isInCachedRanges(SCCOL nCol, SCROW nRow) const
{
    return maCachedRanges.In(ScRange(nCol, nRow, 0, nCol, nRow, 0));
}

ScExternalRefCache::TokenRef ScExternalRefCache::Table::getEmptyOrNullToken(
    SCCOL nCol, SCROW nRow) const
{
    if (isInCachedRanges(nCol, nRow))
    {
        TokenRef p(new ScEmptyCellToken(false, false));
        return p;
    }
    return TokenRef();
}

// ----------------------------------------------------------------------------

ScExternalRefCache::TableName::TableName(const OUString& rUpper, const OUString& rReal) :
    maUpperName(rUpper), maRealName(rReal)
{
}

// ----------------------------------------------------------------------------

ScExternalRefCache::CellFormat::CellFormat() :
    mbIsSet(false), mnType(NUMBERFORMAT_ALL), mnIndex(0)
{
}

// ----------------------------------------------------------------------------

ScExternalRefCache::ScExternalRefCache()
{
}
ScExternalRefCache::~ScExternalRefCache()
{
}

const OUString* ScExternalRefCache::getRealTableName(sal_uInt16 nFileId, const OUString& rTabName) const
{
    DocDataType::const_iterator itrDoc = maDocs.find(nFileId);
    if (itrDoc == maDocs.end())
    {
        // specified document is not cached.
        return NULL;
    }

    const DocItem& rDoc = itrDoc->second;
    TableNameIndexMap::const_iterator itrTabId = rDoc.maTableNameIndex.find(
        ScGlobal::pCharClass->uppercase(rTabName));
    if (itrTabId == rDoc.maTableNameIndex.end())
    {
        // the specified table is not in cache.
        return NULL;
    }

    return &rDoc.maTableNames[itrTabId->second].maRealName;
}

const OUString* ScExternalRefCache::getRealRangeName(sal_uInt16 nFileId, const OUString& rRangeName) const
{
    DocDataType::const_iterator itrDoc = maDocs.find(nFileId);
    if (itrDoc == maDocs.end())
    {
        // specified document is not cached.
        return NULL;
    }

    const DocItem& rDoc = itrDoc->second;
    NamePairMap::const_iterator itr = rDoc.maRealRangeNameMap.find(
        ScGlobal::pCharClass->uppercase(rRangeName));
    if (itr == rDoc.maRealRangeNameMap.end())
        // range name not found.
        return NULL;

    return &itr->second;
}

ScExternalRefCache::TokenRef ScExternalRefCache::getCellData(
    sal_uInt16 nFileId, const OUString& rTabName, SCCOL nCol, SCROW nRow, sal_uInt32* pnFmtIndex)
{
    DocDataType::const_iterator itrDoc = maDocs.find(nFileId);
    if (itrDoc == maDocs.end())
    {
        // specified document is not cached.
        return TokenRef();
    }

    const DocItem& rDoc = itrDoc->second;
    TableNameIndexMap::const_iterator itrTabId = rDoc.maTableNameIndex.find(
        ScGlobal::pCharClass->uppercase(rTabName));
    if (itrTabId == rDoc.maTableNameIndex.end())
    {
        // the specified table is not in cache.
        return TokenRef();
    }

    const TableTypeRef& pTableData = rDoc.maTables[itrTabId->second];
    if (!pTableData.get())
    {
        // the table data is not instantiated yet.
        return TokenRef();
    }

    return pTableData->getCell(nCol, nRow, pnFmtIndex);
}

ScExternalRefCache::TokenArrayRef ScExternalRefCache::getCellRangeData(
    sal_uInt16 nFileId, const OUString& rTabName, const ScRange& rRange)
{
    DocDataType::iterator itrDoc = maDocs.find(nFileId);
    if (itrDoc == maDocs.end())
        // specified document is not cached.
        return TokenArrayRef();

    DocItem& rDoc = itrDoc->second;

    TableNameIndexMap::iterator itrTabId = rDoc.maTableNameIndex.find(
        ScGlobal::pCharClass->uppercase(rTabName));
    if (itrTabId == rDoc.maTableNameIndex.end())
        // the specified table is not in cache.
        return TokenArrayRef();

    const ScAddress& s = rRange.aStart;
    const ScAddress& e = rRange.aEnd;

    SCTAB nTab1 = s.Tab(), nTab2 = e.Tab();
    SCCOL nCol1 = s.Col(), nCol2 = e.Col();
    SCROW nRow1 = s.Row(), nRow2 = e.Row();

    // Make sure I have all the tables cached.
    size_t nTabFirstId = itrTabId->second;
    size_t nTabLastId  = nTabFirstId + nTab2 - nTab1;
    if (nTabLastId >= rDoc.maTables.size())
        // not all tables are cached.
        return TokenArrayRef();

    ScRange aCacheRange( nCol1, nRow1, static_cast<SCTAB>(nTabFirstId), nCol2, nRow2, static_cast<SCTAB>(nTabLastId));

    RangeArrayMap::const_iterator itrRange = rDoc.maRangeArrays.find( aCacheRange);
    if (itrRange != rDoc.maRangeArrays.end())
        // Cache hit!
        return itrRange->second;

    ::boost::scoped_ptr<ScRange> pNewRange;
    TokenArrayRef pArray;
    bool bFirstTab = true;
    for (size_t nTab = nTabFirstId; nTab <= nTabLastId; ++nTab)
    {
        TableTypeRef pTab = rDoc.maTables[nTab];
        if (!pTab.get())
            return TokenArrayRef();

        SCCOL nDataCol1 = nCol1, nDataCol2 = nCol2;
        SCROW nDataRow1 = nRow1, nDataRow2 = nRow2;

        if (!pTab->isRangeCached(nDataCol1, nDataRow1, nDataCol2, nDataRow2))
        {
            // specified range is not entirely within cached ranges.
            return TokenArrayRef();
        }

        ScMatrixRef xMat = new ScMatrix(
            static_cast<SCSIZE>(nDataCol2-nDataCol1+1), static_cast<SCSIZE>(nDataRow2-nDataRow1+1));

        // Only fill non-empty cells, for better performance.
        vector<SCROW> aRows;
        pTab->getAllRows(aRows, nDataRow1, nDataRow2);
        for (vector<SCROW>::const_iterator itr = aRows.begin(), itrEnd = aRows.end(); itr != itrEnd; ++itr)
        {
            SCROW nRow = *itr;
            vector<SCCOL> aCols;
            pTab->getAllCols(nRow, aCols, nDataCol1, nDataCol2);
            for (vector<SCCOL>::const_iterator itrCol = aCols.begin(), itrColEnd = aCols.end(); itrCol != itrColEnd; ++itrCol)
            {
                SCCOL nCol = *itrCol;
                TokenRef pToken = pTab->getCell(nCol, nRow);
                if (!pToken)
                    // This should never happen!
                    return TokenArrayRef();

                SCSIZE nC = nCol - nDataCol1, nR = nRow - nDataRow1;
                switch (pToken->GetType())
                {
                    case svDouble:
                        xMat->PutDouble(pToken->GetDouble(), nC, nR);
                    break;
                    case svString:
                        xMat->PutString(pToken->GetString(), nC, nR);
                    break;
                    default:
                        ;
                }
            }
        }

        if (!bFirstTab)
            pArray->AddOpCode(ocSep);

        ScMatrixToken aToken(xMat);
        if (!pArray)
            pArray.reset(new ScTokenArray);
        pArray->AddToken(aToken);

        bFirstTab = false;

        if (!pNewRange)
            pNewRange.reset(new ScRange(nDataCol1, nDataRow1, 0, nDataCol2, nDataRow2, 0));
        else
            pNewRange->ExtendTo(ScRange(nDataCol1, nDataRow1, 0, nDataCol2, nDataRow2, 0));
    }

    if (pNewRange)
        rDoc.maRangeArrays.insert( RangeArrayMap::value_type(*pNewRange, pArray));
    return pArray;
}

ScExternalRefCache::TokenArrayRef ScExternalRefCache::getRangeNameTokens(sal_uInt16 nFileId, const OUString& rName)
{
    DocItem* pDoc = getDocItem(nFileId);
    if (!pDoc)
        return TokenArrayRef();

    RangeNameMap& rMap = pDoc->maRangeNames;
    RangeNameMap::const_iterator itr = rMap.find(
        ScGlobal::pCharClass->uppercase(rName));
    if (itr == rMap.end())
        return TokenArrayRef();

    return itr->second;
}

void ScExternalRefCache::setRangeNameTokens(sal_uInt16 nFileId, const OUString& rName, TokenArrayRef pArray)
{
    DocItem* pDoc = getDocItem(nFileId);
    if (!pDoc)
        return;

    String aUpperName = ScGlobal::pCharClass->uppercase(rName);
    RangeNameMap& rMap = pDoc->maRangeNames;
    rMap.insert(RangeNameMap::value_type(aUpperName, pArray));
    pDoc->maRealRangeNameMap.insert(NamePairMap::value_type(aUpperName, rName));
}

void ScExternalRefCache::setCellData(sal_uInt16 nFileId, const OUString& rTabName, SCCOL nCol, SCROW nRow,
                                     TokenRef pToken, sal_uLong nFmtIndex)
{
    if (!isDocInitialized(nFileId))
        return;

    using ::std::pair;
    DocItem* pDocItem = getDocItem(nFileId);
    if (!pDocItem)
        return;

    DocItem& rDoc = *pDocItem;

    // See if the table by this name already exists.
    TableNameIndexMap::iterator itrTabName = rDoc.maTableNameIndex.find(
        ScGlobal::pCharClass->uppercase(rTabName));
    if (itrTabName == rDoc.maTableNameIndex.end())
        // Table not found.  Maybe the table name or the file id is wrong ???
        return;

    TableTypeRef& pTableData = rDoc.maTables[itrTabName->second];
    if (!pTableData.get())
        pTableData.reset(new Table);

    pTableData->setCell(nCol, nRow, pToken, nFmtIndex);
    pTableData->setCachedCell(nCol, nRow);
}

void ScExternalRefCache::setCellRangeData(sal_uInt16 nFileId, const ScRange& rRange, const vector<SingleRangeData>& rData,
                                          const TokenArrayRef& pArray)
{
    using ::std::pair;
    if (rData.empty() || !isDocInitialized(nFileId))
        // nothing to cache
        return;

    // First, get the document item for the given file ID.
    DocItem* pDocItem = getDocItem(nFileId);
    if (!pDocItem)
        return;

    DocItem& rDoc = *pDocItem;

    // Now, find the table position of the first table to cache.
    const String& rFirstTabName = rData.front().maTableName;
    TableNameIndexMap::iterator itrTabName = rDoc.maTableNameIndex.find(
        ScGlobal::pCharClass->uppercase(rFirstTabName));
    if (itrTabName == rDoc.maTableNameIndex.end())
    {
        // table index not found.
        return;
    }

    size_t nTabFirstId = itrTabName->second;
    SCROW nRow1 = rRange.aStart.Row(), nRow2 = rRange.aEnd.Row();
    SCCOL nCol1 = rRange.aStart.Col(), nCol2 = rRange.aEnd.Col();
    vector<SingleRangeData>::const_iterator itrDataBeg = rData.begin(), itrDataEnd = rData.end();
    for (vector<SingleRangeData>::const_iterator itrData = itrDataBeg; itrData != itrDataEnd; ++itrData)
    {
        size_t i = nTabFirstId + ::std::distance(itrDataBeg, itrData);
        TableTypeRef& pTabData = rDoc.maTables[i];
        if (!pTabData.get())
            pTabData.reset(new Table);

        for (SCROW nRow = nRow1; nRow <= nRow2; ++nRow)
        {
            for (SCCOL nCol = nCol1; nCol <= nCol2; ++nCol)
            {
                SCSIZE nC = nCol - nCol1, nR = nRow - nRow1;
                TokenRef pToken;
                const ScMatrixRef& pMat = itrData->mpRangeData;
                if (pMat->IsEmpty(nC, nR))
                    // Don't cache empty cells.
                    continue;

                if (pMat->IsValue(nC, nR))
                    pToken.reset(new formula::FormulaDoubleToken(pMat->GetDouble(nC, nR)));
                else if (pMat->IsString(nC, nR))
                    pToken.reset(new formula::FormulaStringToken(pMat->GetString(nC, nR)));

                if (pToken)
                    // Don't mark this cell 'cached' here, for better performance.
                    pTabData->setCell(nCol, nRow, pToken, 0, false);
            }
        }
        // Mark the whole range 'cached'.
        pTabData->setCachedCellRange(nCol1, nRow1, nCol2, nRow2);
    }

    size_t nTabLastId = nTabFirstId + rRange.aEnd.Tab() - rRange.aStart.Tab();
    ScRange aCacheRange( nCol1, nRow1, static_cast<SCTAB>(nTabFirstId), nCol2, nRow2, static_cast<SCTAB>(nTabLastId));

    rDoc.maRangeArrays.insert( RangeArrayMap::value_type( aCacheRange, pArray));
}

bool ScExternalRefCache::isDocInitialized(sal_uInt16 nFileId)
{
    DocItem* pDoc = getDocItem(nFileId);
    if (!pDoc)
        return false;

    return pDoc->mbInitFromSource;
}

static bool lcl_getTableDataIndex(const ScExternalRefCache::TableNameIndexMap& rMap, const String& rName, size_t& rIndex)
{
    ScExternalRefCache::TableNameIndexMap::const_iterator itr = rMap.find(rName);
    if (itr == rMap.end())
        return false;

    rIndex = itr->second;
    return true;
}

void ScExternalRefCache::initializeDoc(sal_uInt16 nFileId, const vector<OUString>& rTabNames)
{
    DocItem* pDoc = getDocItem(nFileId);
    if (!pDoc)
        return;

    size_t n = rTabNames.size();

    // table name list - the list must include all table names in the source
    // document and only to be populated when loading the source document, not
    // when loading cached data from, say, Excel XCT/CRN records.
    vector<TableName> aNewTabNames;
    aNewTabNames.reserve(n);
    for (vector<OUString>::const_iterator itr = rTabNames.begin(), itrEnd = rTabNames.end();
          itr != itrEnd; ++itr)
    {
        TableName aNameItem(ScGlobal::pCharClass->uppercase(*itr), *itr);
        aNewTabNames.push_back(aNameItem);
    }
    pDoc->maTableNames.swap(aNewTabNames);

    // data tables - preserve any existing data that may have been set during
    // file import.
    vector<TableTypeRef> aNewTables(n);
    for (size_t i = 0; i < n; ++i)
    {
        size_t nIndex;
        if (lcl_getTableDataIndex(pDoc->maTableNameIndex, pDoc->maTableNames[i].maUpperName, nIndex))
        {
            aNewTables[i] = pDoc->maTables[nIndex];
        }
    }
    pDoc->maTables.swap(aNewTables);

    // name index map
    TableNameIndexMap aNewNameIndex;
    for (size_t i = 0; i < n; ++i)
        aNewNameIndex.insert(TableNameIndexMap::value_type(pDoc->maTableNames[i].maUpperName, i));
    pDoc->maTableNameIndex.swap(aNewNameIndex);

    pDoc->mbInitFromSource = true;
}

String ScExternalRefCache::getTableName(sal_uInt16 nFileId, size_t nCacheId) const
{
    if( DocItem* pDoc = getDocItem( nFileId ) )
        if( nCacheId < pDoc->maTableNames.size() )
            return pDoc->maTableNames[ nCacheId ].maRealName;
    return EMPTY_STRING;
}

void ScExternalRefCache::getAllTableNames(sal_uInt16 nFileId, vector<OUString>& rTabNames) const
{
    rTabNames.clear();
    DocItem* pDoc = getDocItem(nFileId);
    if (!pDoc)
        return;

    size_t n = pDoc->maTableNames.size();
    rTabNames.reserve(n);
    for (vector<TableName>::const_iterator itr = pDoc->maTableNames.begin(), itrEnd = pDoc->maTableNames.end();
          itr != itrEnd; ++itr)
        rTabNames.push_back(itr->maRealName);
}

SCsTAB ScExternalRefCache::getTabSpan( sal_uInt16 nFileId, const OUString& rStartTabName, const OUString& rEndTabName ) const
{
    DocItem* pDoc = getDocItem(nFileId);
    if (!pDoc)
        return -1;

    vector<TableName>::const_iterator itrBeg = pDoc->maTableNames.begin();
    vector<TableName>::const_iterator itrEnd = pDoc->maTableNames.end();

    vector<TableName>::const_iterator itrStartTab = ::std::find_if( itrBeg, itrEnd,
            TabNameSearchPredicate( rStartTabName));
    if (itrStartTab == itrEnd)
        return -1;

    vector<TableName>::const_iterator itrEndTab = ::std::find_if( itrBeg, itrEnd,
            TabNameSearchPredicate( rEndTabName));
    if (itrEndTab == itrEnd)
        return 0;

    size_t nStartDist = ::std::distance( itrBeg, itrStartTab);
    size_t nEndDist = ::std::distance( itrBeg, itrEndTab);
    return nStartDist <= nEndDist ? static_cast<SCsTAB>(nEndDist - nStartDist + 1) : -static_cast<SCsTAB>(nStartDist - nEndDist + 1);
}

void ScExternalRefCache::getAllNumberFormats(vector<sal_uInt32>& rNumFmts) const
{
    using ::std::sort;
    using ::std::unique;

    vector<sal_uInt32> aNumFmts;
    for (DocDataType::const_iterator itrDoc = maDocs.begin(), itrDocEnd = maDocs.end();
          itrDoc != itrDocEnd; ++itrDoc)
    {
        const vector<TableTypeRef>& rTables = itrDoc->second.maTables;
        for (vector<TableTypeRef>::const_iterator itrTab = rTables.begin(), itrTabEnd = rTables.end();
              itrTab != itrTabEnd; ++itrTab)
        {
            TableTypeRef pTab = *itrTab;
            if (!pTab)
                continue;

            pTab->getAllNumberFormats(aNumFmts);
        }
    }

    // remove duplicates.
    sort(aNumFmts.begin(), aNumFmts.end());
    aNumFmts.erase(unique(aNumFmts.begin(), aNumFmts.end()), aNumFmts.end());
    rNumFmts.swap(aNumFmts);
}

bool ScExternalRefCache::setCacheDocReferenced( sal_uInt16 nFileId )
{
    DocItem* pDocItem = getDocItem(nFileId);
    if (!pDocItem)
        return areAllCacheTablesReferenced();

    for (::std::vector<TableTypeRef>::iterator itrTab = pDocItem->maTables.begin();
            itrTab != pDocItem->maTables.end(); ++itrTab)
    {
        if ((*itrTab).get())
            (*itrTab)->setReferenced( true);
    }
    addCacheDocToReferenced( nFileId);
    return areAllCacheTablesReferenced();
}

bool ScExternalRefCache::setCacheTableReferenced( sal_uInt16 nFileId, const OUString& rTabName, size_t nSheets, bool bPermanent )
{
    DocItem* pDoc = getDocItem(nFileId);
    if (pDoc)
    {
        size_t nIndex = 0;
        String aTabNameUpper = ScGlobal::pCharClass->uppercase( rTabName);
        if (lcl_getTableDataIndex( pDoc->maTableNameIndex, aTabNameUpper, nIndex))
        {
            size_t nStop = ::std::min( nIndex + nSheets, pDoc->maTables.size());
            for (size_t i = nIndex; i < nStop; ++i)
            {
                TableTypeRef pTab = pDoc->maTables[i];
                if (pTab.get())
                {
                    Table::ReferencedFlag eNewFlag = (bPermanent ?
                            Table::REFERENCED_PERMANENT :
                            Table::REFERENCED_MARKED);
                    Table::ReferencedFlag eOldFlag = pTab->getReferencedFlag();
                    if (eOldFlag != Table::REFERENCED_PERMANENT && eNewFlag != eOldFlag)
                    {
                        pTab->setReferencedFlag( eNewFlag);
                        addCacheTableToReferenced( nFileId, i);
                    }
                }
            }
        }
    }
    return areAllCacheTablesReferenced();
}

void ScExternalRefCache::setAllCacheTableReferencedStati( bool bReferenced )
{
    if (bReferenced)
    {
        maReferenced.reset(0);
        for (DocDataType::iterator itrDoc = maDocs.begin(); itrDoc != maDocs.end(); ++itrDoc)
        {
            ScExternalRefCache::DocItem& rDocItem = (*itrDoc).second;
            for (::std::vector<TableTypeRef>::iterator itrTab = rDocItem.maTables.begin();
                    itrTab != rDocItem.maTables.end(); ++itrTab)
            {
                if ((*itrTab).get())
                    (*itrTab)->setReferenced( true);
            }
        }
    }
    else
    {
        size_t nDocs = 0;
        for (DocDataType::const_iterator itrDoc = maDocs.begin(); itrDoc != maDocs.end(); ++itrDoc)
        {
            if (nDocs <= (*itrDoc).first)
                nDocs  = (*itrDoc).first + 1;
        }
        maReferenced.reset( nDocs);

        for (DocDataType::iterator itrDoc = maDocs.begin(); itrDoc != maDocs.end(); ++itrDoc)
        {
            ScExternalRefCache::DocItem& rDocItem = (*itrDoc).second;
            sal_uInt16 nFileId = (*itrDoc).first;
            size_t nTables = rDocItem.maTables.size();
            ReferencedStatus::DocReferenced & rDocReferenced = maReferenced.maDocs[nFileId];
            // All referenced => non-existing tables evaluate as completed.
            rDocReferenced.maTables.resize( nTables, true);
            for (size_t i=0; i < nTables; ++i)
            {
                TableTypeRef & xTab = rDocItem.maTables[i];
                if (xTab.get())
                {
                    if (xTab->getReferencedFlag() == Table::REFERENCED_PERMANENT)
                        addCacheTableToReferenced( nFileId, i);
                    else
                    {
                        xTab->setReferencedFlag( Table::UNREFERENCED);
                        rDocReferenced.maTables[i] = false;
                        rDocReferenced.mbAllTablesReferenced = false;
                        // An addCacheTableToReferenced() actually may have
                        // resulted in mbAllReferenced been set. Clear it.
                        maReferenced.mbAllReferenced = false;
                    }
                }
            }
        }
    }
}

void ScExternalRefCache::addCacheTableToReferenced( sal_uInt16 nFileId, size_t nIndex )
{
    if (nFileId >= maReferenced.maDocs.size())
        return;

    ::std::vector<bool> & rTables = maReferenced.maDocs[nFileId].maTables;
    size_t nTables = rTables.size();
    if (nIndex >= nTables)
        return;

    if (!rTables[nIndex])
    {
        rTables[nIndex] = true;
        size_t i = 0;
        while (i < nTables && rTables[i])
            ++i;
        if (i == nTables)
        {
            maReferenced.maDocs[nFileId].mbAllTablesReferenced = true;
            maReferenced.checkAllDocs();
        }
    }
}

void ScExternalRefCache::addCacheDocToReferenced( sal_uInt16 nFileId )
{
    if (nFileId >= maReferenced.maDocs.size())
        return;

    if (!maReferenced.maDocs[nFileId].mbAllTablesReferenced)
    {
        ::std::vector<bool> & rTables = maReferenced.maDocs[nFileId].maTables;
        size_t nSize = rTables.size();
        for (size_t i=0; i < nSize; ++i)
            rTables[i] = true;
        maReferenced.maDocs[nFileId].mbAllTablesReferenced = true;
        maReferenced.checkAllDocs();
    }
}

bool ScExternalRefCache::areAllCacheTablesReferenced() const
{
    return maReferenced.mbAllReferenced;
}

ScExternalRefCache::ReferencedStatus::ReferencedStatus() :
    mbAllReferenced(false)
{
    reset(0);
}

void ScExternalRefCache::ReferencedStatus::reset( size_t nDocs )
{
    if (nDocs)
    {
        mbAllReferenced = false;
        DocReferencedVec aRefs( nDocs);
        maDocs.swap( aRefs);
    }
    else
    {
        mbAllReferenced = true;
        DocReferencedVec aRefs;
        maDocs.swap( aRefs);
    }
}

void ScExternalRefCache::ReferencedStatus::checkAllDocs()
{
    for (DocReferencedVec::const_iterator itr = maDocs.begin(); itr != maDocs.end(); ++itr)
    {
        if (!(*itr).mbAllTablesReferenced)
            return;
    }
    mbAllReferenced = true;
}

ScExternalRefCache::TableTypeRef ScExternalRefCache::getCacheTable(sal_uInt16 nFileId, size_t nTabIndex) const
{
    DocItem* pDoc = getDocItem(nFileId);
    if (!pDoc || nTabIndex >= pDoc->maTables.size())
        return TableTypeRef();

    return pDoc->maTables[nTabIndex];
}

ScExternalRefCache::TableTypeRef ScExternalRefCache::getCacheTable(sal_uInt16 nFileId, const OUString& rTabName, bool bCreateNew, size_t* pnIndex)
{
    // In API, the index is transported as cached sheet ID of type sal_Int32 in
    // sheet::SingleReference.Sheet or sheet::ComplexReference.Reference1.Sheet
    // in a sheet::FormulaToken, choose a sensible value for N/A. Effectively
    // being 0xffffffff
    const size_t nNotAvailable = static_cast<size_t>( static_cast<sal_Int32>( -1));

    DocItem* pDoc = getDocItem(nFileId);
    if (!pDoc)
    {
        if (pnIndex) *pnIndex = nNotAvailable;
        return TableTypeRef();
    }

    DocItem& rDoc = *pDoc;

    size_t nIndex;
    String aTabNameUpper = ScGlobal::pCharClass->uppercase(rTabName);
    if (lcl_getTableDataIndex(rDoc.maTableNameIndex, aTabNameUpper, nIndex))
    {
        // specified table found.
        if( pnIndex ) *pnIndex = nIndex;
        if (bCreateNew && !rDoc.maTables[nIndex])
            rDoc.maTables[nIndex].reset(new Table);

        return rDoc.maTables[nIndex];
    }

    if (!bCreateNew)
    {
        if (pnIndex) *pnIndex = nNotAvailable;
        return TableTypeRef();
    }

    // Specified table doesn't exist yet.  Create one.
    nIndex = rDoc.maTables.size();
    if( pnIndex ) *pnIndex = nIndex;
    TableTypeRef pTab(new Table);
    rDoc.maTables.push_back(pTab);
    rDoc.maTableNames.push_back(TableName(aTabNameUpper, rTabName));
    rDoc.maTableNameIndex.insert(
        TableNameIndexMap::value_type(aTabNameUpper, nIndex));
    return pTab;
}

void ScExternalRefCache::clearCache(sal_uInt16 nFileId)
{
    maDocs.erase(nFileId);
}

ScExternalRefCache::DocItem* ScExternalRefCache::getDocItem(sal_uInt16 nFileId) const
{
    using ::std::pair;
    DocDataType::iterator itrDoc = maDocs.find(nFileId);
    if (itrDoc == maDocs.end())
    {
        // specified document is not cached.
        pair<DocDataType::iterator, bool> res = maDocs.insert(
                DocDataType::value_type(nFileId, DocItem()));

        if (!res.second)
            // insertion failed.
            return NULL;

        itrDoc = res.first;
    }

    return &itrDoc->second;
}

// ============================================================================

ScExternalRefLink::ScExternalRefLink(ScDocument* pDoc, sal_uInt16 nFileId, const String& rFilter) :
    ::sfx2::SvBaseLink(::sfx2::LINKUPDATE_ONCALL, FORMAT_FILE),
    mnFileId(nFileId),
    maFilterName(rFilter),
    mpDoc(pDoc),
    mbDoRefresh(true)
{
}

ScExternalRefLink::~ScExternalRefLink()
{
}

void ScExternalRefLink::Closed()
{
    ScExternalRefManager* pMgr = mpDoc->GetExternalRefManager();
    pMgr->breakLink(mnFileId);
}

::sfx2::SvBaseLink::UpdateResult ScExternalRefLink::DataChanged(const String& /*rMimeType*/, const Any& /*rValue*/)
{
    if (!mbDoRefresh)
        return SUCCESS;

    rtl::OUString aFile, aFilter;
    mpDoc->GetLinkManager()->GetDisplayNames(this, NULL, &aFile, NULL, &aFilter);
    ScExternalRefManager* pMgr = mpDoc->GetExternalRefManager();

    if (!pMgr->isFileLoadable(aFile))
        return ERROR_GENERAL;

    const OUString* pCurFile = pMgr->getExternalFileName(mnFileId);
    if (!pCurFile)
        return ERROR_GENERAL;

    if (pCurFile->equals(aFile))
    {
        // Refresh the current source document.
        pMgr->refreshNames(mnFileId);
    }
    else
    {
        // The source document has changed.
        ScDocShell* pDocShell = ScDocShell::GetViewData()->GetDocShell();
        ScDocShellModificator aMod(*pDocShell);
        pMgr->switchSrcFile(mnFileId, aFile, aFilter);
        maFilterName = aFilter;
        aMod.SetDocumentModified();
    }

    return SUCCESS;
}

void ScExternalRefLink::Edit(Window* pParent, const Link& /*rEndEditHdl*/)
{
    SvBaseLink::Edit(pParent, LINK(this, ScExternalRefLink, ExternalRefEndEditHdl));
}

void ScExternalRefLink::SetDoReferesh(bool b)
{
    mbDoRefresh = b;
}

IMPL_LINK_NOARG(ScExternalRefLink, ExternalRefEndEditHdl)
{
    return 0;
}

// ============================================================================

static FormulaToken* lcl_convertToToken(ScBaseCell* pCell)
{
    if (!pCell || pCell->HasEmptyData())
    {
        bool bInherited = (pCell && pCell->GetCellType() == CELLTYPE_FORMULA);
        return new ScEmptyCellToken( bInherited, false);
    }

    switch (pCell->GetCellType())
    {
        case CELLTYPE_EDIT:
        {
            rtl::OUString aStr = static_cast<ScEditCell*>(pCell)->GetString();
            return new formula::FormulaStringToken(aStr);
        }
        case CELLTYPE_STRING:
        {
            rtl::OUString aStr = static_cast<ScStringCell*>(pCell)->GetString();
            return new formula::FormulaStringToken(aStr);
        }
        case CELLTYPE_VALUE:
        {
            double fVal = static_cast<ScValueCell*>(pCell)->GetValue();
            return new formula::FormulaDoubleToken(fVal);
        }
        case CELLTYPE_FORMULA:
        {
            ScFormulaCell* pFCell = static_cast<ScFormulaCell*>(pCell);
            sal_uInt16 nError = pFCell->GetErrCode();
            if (nError)
                return new FormulaErrorToken( nError);
            else if (pFCell->IsValue())
            {
                double fVal = pFCell->GetValue();
                return new formula::FormulaDoubleToken(fVal);
            }
            else
            {
                rtl::OUString aStr = pFCell->GetString();
                return new formula::FormulaStringToken(aStr);
            }
        }
        default:
            OSL_FAIL("attempted to convert an unknown cell type.");
    }

    return NULL;
}

static ScTokenArray* lcl_convertToTokenArray(const ScDocument* pSrcDoc, ScRange& rRange,
                                             vector<ScExternalRefCache::SingleRangeData>& rCacheData)
{
    ScAddress& s = rRange.aStart;
    ScAddress& e = rRange.aEnd;

    SCTAB nTab1 = s.Tab(), nTab2 = e.Tab();
    SCCOL nCol1 = s.Col(), nCol2 = e.Col();
    SCROW nRow1 = s.Row(), nRow2 = e.Row();

    if (nTab2 != nTab1)
        // For now, we don't support multi-sheet ranges intentionally because
        // we don't have a way to express them in a single token.  In the
        // future we can introduce a new stack variable type svMatrixList with
        // a new token type that can store a 3D matrix value and convert a 3D
        // range to it.
        return NULL;

    ::boost::scoped_ptr<ScRange> pUsedRange;

    SAL_WNODEPRECATED_DECLARATIONS_PUSH
    auto_ptr<ScTokenArray> pArray(new ScTokenArray);
    SAL_WNODEPRECATED_DECLARATIONS_POP
    bool bFirstTab = true;
    vector<ScExternalRefCache::SingleRangeData>::iterator
        itrCache = rCacheData.begin(), itrCacheEnd = rCacheData.end();

    for (SCTAB nTab = nTab1; nTab <= nTab2 && itrCache != itrCacheEnd; ++nTab, ++itrCache)
    {
        // Only loop within the data area.
        SCCOL nDataCol1 = nCol1, nDataCol2 = nCol2;
        SCROW nDataRow1 = nRow1, nDataRow2 = nRow2;
        bool bShrunk;
        if (!pSrcDoc->ShrinkToUsedDataArea( bShrunk, nTab, nDataCol1, nDataRow1, nDataCol2, nDataRow2, false))
            // no data within specified range.
            continue;

        if (pUsedRange.get())
            // Make sure the used area only grows, not shrinks.
            pUsedRange->ExtendTo(ScRange(nDataCol1, nDataRow1, 0, nDataCol2, nDataRow2, 0));
        else
            pUsedRange.reset(new ScRange(nDataCol1, nDataRow1, 0, nDataCol2, nDataRow2, 0));

        ScMatrixRef xMat = new ScMatrix(
            static_cast<SCSIZE>(nCol2-nCol1+1), static_cast<SCSIZE>(nRow2-nRow1+1));

        for (SCCOL nCol = nDataCol1; nCol <= nDataCol2; ++nCol)
        {
            for (SCROW nRow = nDataRow1; nRow <= nDataRow2; ++nRow)
            {
                SCSIZE nC = nCol - nCol1, nR = nRow - nRow1;
                ScBaseCell* pCell;
                pSrcDoc->GetCell(nCol, nRow, nTab, pCell);
                if (!pCell || pCell->HasEmptyData())
                    // Skip empty cells.  Matrix's default values are empty elements.
                    continue;

                switch (pCell->GetCellType())
                {
                    case CELLTYPE_EDIT:
                    {
                        rtl::OUString aStr = static_cast<ScEditCell*>(pCell)->GetString();
                        xMat->PutString(aStr, nC, nR);
                    }
                    break;
                    case CELLTYPE_STRING:
                    {
                        rtl::OUString aStr = static_cast<ScStringCell*>(pCell)->GetString();
                        xMat->PutString(aStr, nC, nR);
                    }
                    break;
                    case CELLTYPE_VALUE:
                    {
                        double fVal = static_cast<ScValueCell*>(pCell)->GetValue();
                        xMat->PutDouble(fVal, nC, nR);
                    }
                    break;
                    case CELLTYPE_FORMULA:
                    {
                        ScFormulaCell* pFCell = static_cast<ScFormulaCell*>(pCell);
                        sal_uInt16 nError = pFCell->GetErrCode();
                        if (nError)
                            xMat->PutDouble( CreateDoubleError( nError), nC, nR);
                        else if (pFCell->IsValue())
                        {
                            double fVal = pFCell->GetValue();
                            xMat->PutDouble(fVal, nC, nR);
                        }
                        else
                        {
                            rtl::OUString aStr = pFCell->GetString();
                            xMat->PutString(aStr, nC, nR);
                        }
                    }
                    break;
                    default:
                        OSL_FAIL("attempted to convert an unknown cell type.");
                }
            }
        }
        if (!bFirstTab)
            pArray->AddOpCode(ocSep);

        ScMatrixToken aToken(xMat);
        pArray->AddToken(aToken);

        itrCache->mpRangeData = xMat;

        bFirstTab = false;
    }

    if (!pUsedRange.get())
        return NULL;

    s.SetCol(pUsedRange->aStart.Col());
    s.SetRow(pUsedRange->aStart.Row());
    e.SetCol(pUsedRange->aEnd.Col());
    e.SetRow(pUsedRange->aEnd.Row());

    return pArray.release();
}

static ScTokenArray* lcl_fillEmptyMatrix(const ScRange& rRange)
{
    SCSIZE nC = static_cast<SCSIZE>(rRange.aEnd.Col()-rRange.aStart.Col()+1);
    SCSIZE nR = static_cast<SCSIZE>(rRange.aEnd.Row()-rRange.aStart.Row()+1);
    ScMatrixRef xMat = new ScMatrix(nC, nR);

    ScMatrixToken aToken(xMat);
    SAL_WNODEPRECATED_DECLARATIONS_PUSH
    auto_ptr<ScTokenArray> pArray(new ScTokenArray);
    SAL_WNODEPRECATED_DECLARATIONS_POP
    pArray->AddToken(aToken);
    return pArray.release();
}

ScExternalRefManager::ScExternalRefManager(ScDocument* pDoc) :
    mpDoc(pDoc),
    mbInReferenceMarking(false),
    mbUserInteractionEnabled(true)
{
    maSrcDocTimer.SetTimeoutHdl( LINK(this, ScExternalRefManager, TimeOutHdl) );
    maSrcDocTimer.SetTimeout(SRCDOC_SCAN_INTERVAL);
}

ScExternalRefManager::~ScExternalRefManager()
{
    clear();
}

OUString ScExternalRefManager::getCacheTableName(sal_uInt16 nFileId, size_t nTabIndex) const
{
    return maRefCache.getTableName(nFileId, nTabIndex);
}

ScExternalRefCache::TableTypeRef ScExternalRefManager::getCacheTable(sal_uInt16 nFileId, size_t nTabIndex) const
{
    return maRefCache.getCacheTable(nFileId, nTabIndex);
}

ScExternalRefCache::TableTypeRef ScExternalRefManager::getCacheTable(
    sal_uInt16 nFileId, const OUString& rTabName, bool bCreateNew, size_t* pnIndex)
{
    return maRefCache.getCacheTable(nFileId, rTabName, bCreateNew, pnIndex);
}

// ============================================================================

ScExternalRefManager::LinkListener::LinkListener()
{
}

ScExternalRefManager::LinkListener::~LinkListener()
{
}

// ----------------------------------------------------------------------------

ScExternalRefManager::ApiGuard::ApiGuard(ScDocument* pDoc) :
    mpMgr(pDoc->GetExternalRefManager()),
    mbOldInteractionEnabled(mpMgr->mbUserInteractionEnabled)
{
    // We don't want user interaction handled in the API.
    mpMgr->mbUserInteractionEnabled = false;
}

ScExternalRefManager::ApiGuard::~ApiGuard()
{
    // Restore old value.
    mpMgr->mbUserInteractionEnabled = mbOldInteractionEnabled;
}

// ----------------------------------------------------------------------------

void ScExternalRefManager::getAllCachedTableNames(sal_uInt16 nFileId, vector<OUString>& rTabNames) const
{
    maRefCache.getAllTableNames(nFileId, rTabNames);
}

SCsTAB ScExternalRefManager::getCachedTabSpan( sal_uInt16 nFileId, const OUString& rStartTabName, const OUString& rEndTabName ) const
{
    return maRefCache.getTabSpan( nFileId, rStartTabName, rEndTabName);
}

void ScExternalRefManager::getAllCachedNumberFormats(vector<sal_uInt32>& rNumFmts) const
{
    maRefCache.getAllNumberFormats(rNumFmts);
}

sal_uInt16 ScExternalRefManager::getExternalFileCount() const
{
    return static_cast< sal_uInt16 >( maSrcFiles.size() );
}

bool ScExternalRefManager::markUsedByLinkListeners()
{
    bool bAllMarked = false;
    for (LinkListenerMap::const_iterator itr = maLinkListeners.begin();
            itr != maLinkListeners.end() && !bAllMarked; ++itr)
    {
        if (!(*itr).second.empty())
            bAllMarked = maRefCache.setCacheDocReferenced( (*itr).first);
        /* TODO: LinkListeners should remember the table they're listening to.
         * As is, listening to one table will mark all tables of the document
         * being referenced. */
    }
    return bAllMarked;
}

bool ScExternalRefManager::markUsedExternalRefCells()
{
    RefCellMap::iterator itr = maRefCells.begin(), itrEnd = maRefCells.end();
    for (; itr != itrEnd; ++itr)
    {
        RefCellSet::iterator itrCell = itr->second.begin(), itrCellEnd = itr->second.end();
        for (; itrCell != itrCellEnd; ++itrCell)
        {
            ScFormulaCell* pCell = *itrCell;
            bool bUsed = pCell->MarkUsedExternalReferences();
            if (bUsed)
                // Return true when at least one cell references external docs.
                return true;
        }
    }
    return false;
}

bool ScExternalRefManager::setCacheTableReferenced( sal_uInt16 nFileId, const OUString& rTabName, size_t nSheets )
{
    return maRefCache.setCacheTableReferenced( nFileId, rTabName, nSheets, false);
}

void ScExternalRefManager::setAllCacheTableReferencedStati( bool bReferenced )
{
    mbInReferenceMarking = !bReferenced;
    maRefCache.setAllCacheTableReferencedStati( bReferenced );
}

void ScExternalRefManager::storeRangeNameTokens(sal_uInt16 nFileId, const OUString& rName, const ScTokenArray& rArray)
{
    ScExternalRefCache::TokenArrayRef pArray(rArray.Clone());
    maRefCache.setRangeNameTokens(nFileId, rName, pArray);
}

namespace {

/**
 * Put a single cell data into internal cache table.
 *
 * @param pFmt optional cell format index that may need to be stored with
 *             the cell value.
 */
void putCellDataIntoCache(
    ScExternalRefCache& rRefCache, const ScExternalRefCache::TokenRef& pToken,
    sal_uInt16 nFileId, const String& rTabName, const ScAddress& rCell,
    const ScExternalRefCache::CellFormat* pFmt)
{
    // Now, insert the token into cache table but don't cache empty cells.
    if (pToken->GetType() != formula::svEmptyCell)
    {
        sal_uLong nFmtIndex = (pFmt && pFmt->mbIsSet) ? pFmt->mnIndex : 0;
        rRefCache.setCellData(nFileId, rTabName, rCell.Col(), rCell.Row(), pToken, nFmtIndex);
    }
}

/**
 * Put the data into our internal cache table.
 *
 * @param rRefCache cache table set.
 * @param pArray single range data to be returned.
 * @param nFileId external file ID
 * @param rTabName name of the table where the data should be cached.
 * @param rCacheData range data to be cached.
 * @param rCacheRange original cache range, including the empty region if
 *                    any.
 * @param rDataRange reduced cache range that includes only the non-empty
 *                   data area.
 */
void putRangeDataIntoCache(
    ScExternalRefCache& rRefCache, ScExternalRefCache::TokenArrayRef& pArray,
    sal_uInt16 nFileId, const String& rTabName,
    const vector<ScExternalRefCache::SingleRangeData>& rCacheData,
    const ScRange& rCacheRange, const ScRange& rDataRange)
{
    if (pArray)
        // Cache these values.
        rRefCache.setCellRangeData(nFileId, rDataRange, rCacheData, pArray);
    else
    {
        // Array is empty.  Fill it with an empty matrix of the required size.
        pArray.reset(lcl_fillEmptyMatrix(rCacheRange));

        // Make sure to set this range 'cached', to prevent unnecessarily
        // accessing the src document time and time again.
        ScExternalRefCache::TableTypeRef pCacheTab =
            rRefCache.getCacheTable(nFileId, rTabName, true, NULL);
        if (pCacheTab)
            pCacheTab->setCachedCellRange(
                rCacheRange.aStart.Col(), rCacheRange.aStart.Row(), rCacheRange.aEnd.Col(), rCacheRange.aEnd.Row());
    }
}

/**
 * When accessing an external document for the first time, we need to
 * populate the cache with all its sheet names (whether they are referenced
 * or not) in the correct order.  Many client codes that use external
 * references make this assumption.
 *
 * @param rRefCache cache table set.
 * @param pSrcDoc source document instance.
 * @param nFileId external file ID associated with the source document.
 */
void initDocInCache(ScExternalRefCache& rRefCache, const ScDocument* pSrcDoc, sal_uInt16 nFileId)
{
    if (!pSrcDoc)
        return;

    if (rRefCache.isDocInitialized(nFileId))
        // Already initialized.  No need to do this twice.
        return;

    SCTAB nTabCount = pSrcDoc->GetTableCount();
    if (nTabCount)
    {
        // Populate the cache with all table names in the source document.
        vector<OUString> aTabNames;
        aTabNames.reserve(nTabCount);
        for (SCTAB i = 0; i < nTabCount; ++i)
        {
            OUString aName;
            pSrcDoc->GetName(i, aName);
            aTabNames.push_back(aName);
        }
        rRefCache.initializeDoc(nFileId, aTabNames);
    }
}

}

ScExternalRefCache::TokenRef ScExternalRefManager::getSingleRefToken(
    sal_uInt16 nFileId, const OUString& rTabName, const ScAddress& rCell,
    const ScAddress* pCurPos, SCTAB* pTab, ScExternalRefCache::CellFormat* pFmt)
{
    if (pCurPos)
        insertRefCell(nFileId, *pCurPos);

    maybeLinkExternalFile(nFileId);

    if (pTab)
        *pTab = -1;

    if (pFmt)
        pFmt->mbIsSet = false;

    const ScDocument* pSrcDoc = getInMemorySrcDocument(nFileId);
    if (pSrcDoc)
    {
        // source document already loaded in memory.  Re-use this instance.
        SCTAB nTab;
        if (!pSrcDoc->GetTable(rTabName, nTab))
        {
            // specified table name doesn't exist in the source document.
            ScExternalRefCache::TokenRef pToken(new FormulaErrorToken(errNoRef));
            return pToken;
        }

        if (pTab)
            *pTab = nTab;

        ScExternalRefCache::TokenRef pToken =
            getSingleRefTokenFromSrcDoc(
                nFileId, pSrcDoc, ScAddress(rCell.Col(),rCell.Row(),nTab), pFmt);

        putCellDataIntoCache(maRefCache, pToken, nFileId, rTabName, rCell, pFmt);
        return pToken;
    }

    // Check if the given table name and the cell position is cached.
    sal_uInt32 nFmtIndex = 0;
    ScExternalRefCache::TokenRef pToken = maRefCache.getCellData(
        nFileId, rTabName, rCell.Col(), rCell.Row(), &nFmtIndex);
    if (pToken)
    {
        // Cache hit !
        fillCellFormat(nFmtIndex, pFmt);
        return pToken;
    }

    // reference not cached.  read from the source document.
    pSrcDoc = getSrcDocument(nFileId);
    if (!pSrcDoc)
    {
        // Source document not reachable.  Throw a reference error.
        pToken.reset(new FormulaErrorToken(errNoRef));
        return pToken;
    }

    SCTAB nTab;
    if (!pSrcDoc->GetTable(rTabName, nTab))
    {
        // specified table name doesn't exist in the source document.
        pToken.reset(new FormulaErrorToken(errNoRef));
        return pToken;
    }

    if (pTab)
        *pTab = nTab;

    SCCOL nDataCol1 = 0, nDataCol2 = MAXCOL;
    SCROW nDataRow1 = 0, nDataRow2 = MAXROW;
    bool bData = pSrcDoc->ShrinkToDataArea(nTab, nDataCol1, nDataRow1, nDataCol2, nDataRow2);
    if (!bData || rCell.Col() < nDataCol1 || nDataCol2 < rCell.Col() || rCell.Row() < nDataRow1 || nDataRow2 < rCell.Row())
    {
        // requested cell is outside the data area.  Don't even bother caching
        // this data, but add it to the cached range to prevent accessing the
        // source document time and time again.
        ScExternalRefCache::TableTypeRef pCacheTab =
            maRefCache.getCacheTable(nFileId, rTabName, true, NULL);
        if (pCacheTab)
            pCacheTab->setCachedCell(rCell.Col(), rCell.Row());

        pToken.reset(new ScEmptyCellToken(false, false));
        return pToken;
    }

    pToken = getSingleRefTokenFromSrcDoc(
        nFileId, pSrcDoc, ScAddress(rCell.Col(),rCell.Row(),nTab), pFmt);

    putCellDataIntoCache(maRefCache, pToken, nFileId, rTabName, rCell, pFmt);
    return pToken;
}

ScExternalRefCache::TokenArrayRef ScExternalRefManager::getDoubleRefTokens(
    sal_uInt16 nFileId, const OUString& rTabName, const ScRange& rRange, const ScAddress* pCurPos)
{
    if (pCurPos)
        insertRefCell(nFileId, *pCurPos);

    maybeLinkExternalFile(nFileId);

    ScRange aDataRange(rRange);
    const ScDocument* pSrcDoc = getInMemorySrcDocument(nFileId);
    if (pSrcDoc)
    {
        // Document already loaded in memory.
        vector<ScExternalRefCache::SingleRangeData> aCacheData;
        ScExternalRefCache::TokenArrayRef pArray =
            getDoubleRefTokensFromSrcDoc(pSrcDoc, rTabName, aDataRange, aCacheData);

        // Put the data into cache.
        putRangeDataIntoCache(maRefCache, pArray, nFileId, rTabName, aCacheData, rRange, aDataRange);
        return pArray;
    }

    // Check if the given table name and the cell position is cached.
    ScExternalRefCache::TokenArrayRef pArray =
        maRefCache.getCellRangeData(nFileId, rTabName, rRange);
    if (pArray)
        // Cache hit !
        return pArray;

    pSrcDoc = getSrcDocument(nFileId);
    if (!pSrcDoc)
    {
        // Source document is not reachable.  Throw a reference error.
        pArray.reset(new ScTokenArray);
        pArray->AddToken(FormulaErrorToken(errNoRef));
        return pArray;
    }

    vector<ScExternalRefCache::SingleRangeData> aCacheData;
    pArray = getDoubleRefTokensFromSrcDoc(pSrcDoc, rTabName, aDataRange, aCacheData);

    // Put the data into cache.
    putRangeDataIntoCache(maRefCache, pArray, nFileId, rTabName, aCacheData, rRange, aDataRange);
    return pArray;
}

ScExternalRefCache::TokenArrayRef ScExternalRefManager::getRangeNameTokens(
    sal_uInt16 nFileId, const OUString& rName, const ScAddress* pCurPos)
{
    if (pCurPos)
        insertRefCell(nFileId, *pCurPos);

    maybeLinkExternalFile(nFileId);

    OUString aName = rName; // make a copy to have the casing corrected.
    const ScDocument* pSrcDoc = getInMemorySrcDocument(nFileId);
    if (pSrcDoc)
    {
        // Document already loaded in memory.
        ScExternalRefCache::TokenArrayRef pArray =
            getRangeNameTokensFromSrcDoc(nFileId, pSrcDoc, aName);

        if (pArray)
            // Cache this range name array.
            maRefCache.setRangeNameTokens(nFileId, aName, pArray);

        return pArray;
    }

    ScExternalRefCache::TokenArrayRef pArray = maRefCache.getRangeNameTokens(nFileId, rName);
    if (pArray.get())
        // This range name is cached.
        return pArray;

    pSrcDoc = getSrcDocument(nFileId);
    if (!pSrcDoc)
        // failed to load document from disk.
        return ScExternalRefCache::TokenArrayRef();

    pArray = getRangeNameTokensFromSrcDoc(nFileId, pSrcDoc, aName);

    if (pArray)
        // Cache this range name array.
        maRefCache.setRangeNameTokens(nFileId, aName, pArray);

    return pArray;
}

void ScExternalRefManager::refreshAllRefCells(sal_uInt16 nFileId)
{
    RefCellMap::iterator itrFile = maRefCells.find(nFileId);
    if (itrFile == maRefCells.end())
        return;

    RefCellSet& rRefCells = itrFile->second;
    for_each(rRefCells.begin(), rRefCells.end(), UpdateFormulaCell());

    ScViewData* pViewData = ScDocShell::GetViewData();
    if (!pViewData)
        return;

    ScTabViewShell* pVShell = pViewData->GetViewShell();
    if (!pVShell)
        return;

    // Repainting the grid also repaints the texts, but is there a better way
    // to refresh texts?
    pVShell->Invalidate(FID_REPAINT);
    pVShell->PaintGrid();
}

void ScExternalRefManager::insertRefCell(sal_uInt16 nFileId, const ScAddress& rCell)
{
    RefCellMap::iterator itr = maRefCells.find(nFileId);
    if (itr == maRefCells.end())
    {
        RefCellSet aRefCells;
        pair<RefCellMap::iterator, bool> r = maRefCells.insert(
            RefCellMap::value_type(nFileId, aRefCells));
        if (!r.second)
            // insertion failed.
            return;

        itr = r.first;
    }

    ScBaseCell* pCell = mpDoc->GetCell(rCell);
    if (pCell && pCell->GetCellType() == CELLTYPE_FORMULA)
        itr->second.insert(static_cast<ScFormulaCell*>(pCell));
}

void ScExternalRefManager::fillCellFormat(sal_uLong nFmtIndex, ScExternalRefCache::CellFormat* pFmt) const
{
    if (!pFmt)
        return;

    short nFmtType = mpDoc->GetFormatTable()->GetType(nFmtIndex);
    if (nFmtType != NUMBERFORMAT_UNDEFINED)
    {
        pFmt->mbIsSet = true;
        pFmt->mnIndex = nFmtIndex;
        pFmt->mnType = nFmtType;
    }
}

ScExternalRefCache::TokenRef ScExternalRefManager::getSingleRefTokenFromSrcDoc(
    sal_uInt16 nFileId, const ScDocument* pSrcDoc, const ScAddress& rCell,
    ScExternalRefCache::CellFormat* pFmt)
{
    // Get the cell from src doc, and convert it into a token.
    ScBaseCell* pCell = NULL;
    pSrcDoc->GetCell(rCell.Col(), rCell.Row(), rCell.Tab(), pCell);
    ScExternalRefCache::TokenRef pToken(lcl_convertToToken(pCell));

    if (!pToken.get())
    {
        // Generate an error for unresolvable cells.
        pToken.reset( new FormulaErrorToken( errNoValue));
    }

    // Get number format information.
    sal_uInt32 nFmtIndex = 0;
    pSrcDoc->GetNumberFormat(rCell.Col(), rCell.Row(), rCell.Tab(), nFmtIndex);
    nFmtIndex = getMappedNumberFormat(nFileId, nFmtIndex, pSrcDoc);
    fillCellFormat(nFmtIndex, pFmt);
    return pToken;
}

ScExternalRefCache::TokenArrayRef ScExternalRefManager::getDoubleRefTokensFromSrcDoc(
    const ScDocument* pSrcDoc, const OUString& rTabName, ScRange& rRange,
    vector<ScExternalRefCache::SingleRangeData>& rCacheData)
{
    ScExternalRefCache::TokenArrayRef pArray;
    SCTAB nTab1;

    if (!pSrcDoc->GetTable(rTabName, nTab1))
    {
        // specified table name doesn't exist in the source document.
        pArray.reset(new ScTokenArray);
        pArray->AddToken(FormulaErrorToken(errNoRef));
        return pArray;
    }

    ScRange aRange(rRange);
    SCTAB nTabSpan = aRange.aEnd.Tab() - aRange.aStart.Tab();

    vector<ScExternalRefCache::SingleRangeData> aCacheData;
    aCacheData.reserve(nTabSpan+1);
    aCacheData.push_back(ScExternalRefCache::SingleRangeData());
    aCacheData.back().maTableName = ScGlobal::pCharClass->uppercase(rTabName);

    for (SCTAB i = 1; i < nTabSpan + 1; ++i)
    {
        OUString aTabName;
        if (!pSrcDoc->GetName(nTab1 + 1, aTabName))
            // source document doesn't have any table by the specified name.
            break;

        aCacheData.push_back(ScExternalRefCache::SingleRangeData());
        aCacheData.back().maTableName = ScGlobal::pCharClass->uppercase(aTabName);
    }

    aRange.aStart.SetTab(nTab1);
    aRange.aEnd.SetTab(nTab1 + nTabSpan);

    pArray.reset(lcl_convertToTokenArray(pSrcDoc, aRange, aCacheData));
    rRange = aRange;
    rCacheData.swap(aCacheData);
    return pArray;
}

ScExternalRefCache::TokenArrayRef ScExternalRefManager::getRangeNameTokensFromSrcDoc(
    sal_uInt16 nFileId, const ScDocument* pSrcDoc, OUString& rName)
{
    ScRangeName* pExtNames = pSrcDoc->GetRangeName();
    String aUpperName = ScGlobal::pCharClass->uppercase(rName);
    const ScRangeData* pRangeData = pExtNames->findByUpperName(aUpperName);
    if (!pRangeData)
        return ScExternalRefCache::TokenArrayRef();

    // Parse all tokens in this external range data, and replace each absolute
    // reference token with an external reference token, and cache them.  Also
    // register the source document with the link manager if it's a new
    // source.

    ScExternalRefCache::TokenArrayRef pNew(new ScTokenArray);

    ScTokenArray aCode(*pRangeData->GetCode());
    for (const FormulaToken* pToken = aCode.First(); pToken; pToken = aCode.Next())
    {
        bool bTokenAdded = false;
        switch (pToken->GetType())
        {
            case svSingleRef:
            {
                const ScSingleRefData& rRef = static_cast<const ScToken*>(pToken)->GetSingleRef();
                OUString aTabName;
                pSrcDoc->GetName(rRef.nTab, aTabName);
                ScExternalSingleRefToken aNewToken(nFileId, aTabName, static_cast<const ScToken*>(pToken)->GetSingleRef());
                pNew->AddToken(aNewToken);
                bTokenAdded = true;
            }
            break;
            case svDoubleRef:
            {
                const ScSingleRefData& rRef = static_cast<const ScToken*>(pToken)->GetSingleRef();
                OUString aTabName;
                pSrcDoc->GetName(rRef.nTab, aTabName);
                ScExternalDoubleRefToken aNewToken(nFileId, aTabName, static_cast<const ScToken*>(pToken)->GetDoubleRef());
                pNew->AddToken(aNewToken);
                bTokenAdded = true;
            }
            break;
            default:
                ;   // nothing
        }

        if (!bTokenAdded)
            pNew->AddToken(*pToken);
    }

    rName = pRangeData->GetName(); // Get the correctly-cased name.
    return pNew;
}

const ScDocument* ScExternalRefManager::getInMemorySrcDocument(sal_uInt16 nFileId)
{
    const OUString* pFileName = getExternalFileName(nFileId);
    if (!pFileName)
        return NULL;

    ScDocument* pSrcDoc = NULL;
    TypeId aType(TYPE(ScDocShell));
    ScDocShell* pShell = static_cast<ScDocShell*>(SfxObjectShell::GetFirst(&aType, false));
    while (pShell)
    {
        SfxMedium* pMedium = pShell->GetMedium();
        if (pMedium && !pMedium->GetName().isEmpty())
        {
            // TODO: We should make the case sensitivity platform dependent.
            if (pFileName->equalsIgnoreAsciiCase(pMedium->GetName()))
            {
                // Found !
                pSrcDoc = pShell->GetDocument();
                break;
            }
        }
        else
        {
            // handle unsaved documents here
            OUString aName = pShell->GetName();
            if (pFileName->equalsIgnoreAsciiCase(aName))
            {
                // Found !
                SrcShell aSrcDoc;
                aSrcDoc.maShell = pShell;
                maUnsavedDocShells.insert(DocShellMap::value_type(nFileId, aSrcDoc));
                StartListening(*pShell);
                pSrcDoc = pShell->GetDocument();
                break;
            }
        }
        pShell = static_cast<ScDocShell*>(SfxObjectShell::GetNext(*pShell, &aType, false));
    }

    initDocInCache(maRefCache, pSrcDoc, nFileId);
    return pSrcDoc;
}

const ScDocument* ScExternalRefManager::getSrcDocument(sal_uInt16 nFileId)
{
    if (!mpDoc->IsExecuteLinkEnabled())
        return NULL;

    DocShellMap::iterator itrEnd = maDocShells.end();
    DocShellMap::iterator itr = maDocShells.find(nFileId);

    if (itr != itrEnd)
    {
        // document already loaded.

        SfxObjectShell* p = itr->second.maShell;
        itr->second.maLastAccess = Time( Time::SYSTEM );
        return static_cast<ScDocShell*>(p)->GetDocument();
    }

    itrEnd = maUnsavedDocShells.end();
    itr = maUnsavedDocShells.find(nFileId);
    if (itr != itrEnd)
    {
        //document is unsaved document

        SfxObjectShell* p = itr->second.maShell;
        itr->second.maLastAccess = Time( Time::SYSTEM );
        return static_cast<ScDocShell*>(p)->GetDocument();
    }

    const OUString* pFile = getExternalFileName(nFileId);
    if (!pFile)
        // no file name associated with this ID.
        return NULL;

    OUString aFilter;
    SrcShell aSrcDoc;
    aSrcDoc.maShell = loadSrcDocument(nFileId, aFilter);
    if (!aSrcDoc.maShell.Is())
    {
        // source document could not be loaded.
        return NULL;
    }

    if (maDocShells.empty())
    {
        // If this is the first source document insertion, start up the timer.
        maSrcDocTimer.Start();
    }

    maDocShells.insert(DocShellMap::value_type(nFileId, aSrcDoc));
    SfxObjectShell* p = aSrcDoc.maShell;
    ScDocument* pSrcDoc = static_cast<ScDocShell*>(p)->GetDocument();
    initDocInCache(maRefCache, pSrcDoc, nFileId);
    return pSrcDoc;
}

SfxObjectShellRef ScExternalRefManager::loadSrcDocument(sal_uInt16 nFileId, OUString& rFilter)
{
    const SrcFileData* pFileData = getExternalFileData(nFileId);
    if (!pFileData)
        return NULL;

    // Always load the document by using the path created from the relative
    // path.  If the referenced document is not there, simply exit.  The
    // original file name should be used only when the relative path is not
    // given.
    OUString aFile = pFileData->maFileName;
    maybeCreateRealFileName(nFileId);
    if (!pFileData->maRealFileName.isEmpty())
        aFile = pFileData->maRealFileName;

    if (!isFileLoadable(aFile))
        return NULL;

    OUString aOptions = pFileData->maFilterOptions;
    if ( !pFileData->maFilterName.isEmpty() )
        rFilter = pFileData->maFilterName;      // don't overwrite stored filter with guessed filter
    else
        ScDocumentLoader::GetFilterName(aFile, rFilter, aOptions, true, false);;
    ScDocumentLoader::GetFilterName(aFile, rFilter, aOptions, true, false);
    const SfxFilter* pFilter = ScDocShell::Factory().GetFilterContainer()->GetFilter4FilterName(rFilter);

    if (pFileData->maRelativeName.isEmpty())
    {
        // Generate a relative file path.
        INetURLObject aBaseURL(getOwnDocumentName());
        aBaseURL.insertName(OUString(RTL_CONSTASCII_USTRINGPARAM("content.xml")));

        OUString aStr = URIHelper::simpleNormalizedMakeRelative(
            aBaseURL.GetMainURL(INetURLObject::NO_DECODE), aFile);

        setRelativeFileName(nFileId, aStr);
    }

    SfxItemSet* pSet = new SfxAllItemSet(SFX_APP()->GetPool());
    if (!aOptions.isEmpty())
        pSet->Put(SfxStringItem(SID_FILE_FILTEROPTIONS, aOptions));

    // make medium hidden to prevent assertion from progress bar
    pSet->Put( SfxBoolItem(SID_HIDDEN, true) );

    SAL_WNODEPRECATED_DECLARATIONS_PUSH
    auto_ptr<SfxMedium> pMedium(new SfxMedium(aFile, STREAM_STD_READ, pFilter, pSet));
    SAL_WNODEPRECATED_DECLARATIONS_POP
    if (pMedium->GetError() != ERRCODE_NONE)
        return NULL;

    // To load encrypted documents with password, user interaction needs to be enabled.
    pMedium->UseInteractionHandler(mbUserInteractionEnabled);

    ScDocShell* pNewShell = new ScDocShell(SFX_CREATE_MODE_INTERNAL);
    SfxObjectShellRef aRef = pNewShell;

    // increment the recursive link count of the source document.
    ScExtDocOptions* pExtOpt = mpDoc->GetExtDocOptions();
    sal_uInt32 nLinkCount = pExtOpt ? pExtOpt->GetDocSettings().mnLinkCnt : 0;
    ScDocument* pSrcDoc = pNewShell->GetDocument();
    pSrcDoc->EnableExecuteLink(false); // to prevent circular access of external references.
    pSrcDoc->EnableUndo(false);
    pSrcDoc->EnableAdjustHeight(false);
    pSrcDoc->EnableUserInteraction(false);

    ScExtDocOptions* pExtOptNew = pSrcDoc->GetExtDocOptions();
    if (!pExtOptNew)
    {
        pExtOptNew = new ScExtDocOptions;
        pSrcDoc->SetExtDocOptions(pExtOptNew);
    }
    pExtOptNew->GetDocSettings().mnLinkCnt = nLinkCount + 1;

    pNewShell->DoLoad(pMedium.release());

    // with UseInteractionHandler, options may be set by dialog during DoLoad
    OUString aNew = ScDocumentLoader::GetOptions(*pNewShell->GetMedium());
    if (!aNew.isEmpty() && aNew != aOptions)
        aOptions = aNew;
    setFilterData(nFileId, rFilter, aOptions);    // update the filter data, including the new options

    return aRef;
}

bool ScExternalRefManager::isFileLoadable(const OUString& rFile) const
{
    if (rFile.isEmpty())
        return false;

    if (isOwnDocument(rFile))
        return false;
    rtl::OUString aPhysical;
    if (utl::LocalFileHelper::ConvertURLToPhysicalName(rFile, aPhysical) && !aPhysical.isEmpty())
    {
        // #i114504# try IsFolder/Exists only for file URLs

        if (utl::UCBContentHelper::IsFolder(rFile))
            return false;

        return utl::UCBContentHelper::Exists(rFile);
    }
    else
        return true;    // for http and others, Exists doesn't work, but the URL can still be opened
}

void ScExternalRefManager::maybeLinkExternalFile(sal_uInt16 nFileId)
{
    if (maLinkedDocs.count(nFileId))
        // file alerady linked, or the link has been broken.
        return;

    // Source document not linked yet.  Link it now.
    const OUString* pFileName = getExternalFileName(nFileId);
    if (!pFileName)
        return;

    OUString aFilter, aOptions;
    const SrcFileData* pFileData = getExternalFileData(nFileId);
    if (pFileData)
    {
        aFilter = pFileData->maFilterName;
        aOptions = pFileData->maFilterOptions;
    }
    // If a filter was already set (for example, loading the cached table),
    // don't call GetFilterName which has to access the source file.
    if (aFilter.isEmpty())
        ScDocumentLoader::GetFilterName(*pFileName, aFilter, aOptions, true, false);
    sfx2::LinkManager* pLinkMgr = mpDoc->GetLinkManager();
    ScExternalRefLink* pLink = new ScExternalRefLink(mpDoc, nFileId, aFilter);
    OSL_ENSURE(pFileName, "ScExternalRefManager::insertExternalFileLink: file name pointer is NULL");
    String aTmp = aFilter;
    pLinkMgr->InsertFileLink(*pLink, OBJECT_CLIENT_FILE, *pFileName, &aTmp);

    pLink->SetDoReferesh(false);
    pLink->Update();
    pLink->SetDoReferesh(true);

    maLinkedDocs.insert(LinkedDocMap::value_type(nFileId, true));
}

void ScExternalRefManager::SrcFileData::maybeCreateRealFileName(const String& rOwnDocName)
{
    if (maRelativeName.isEmpty())
        // No relative path given.  Nothing to do.
        return;

    if (!maRealFileName.isEmpty())
        // Real file name already created.  Nothing to do.
        return;

    // Formulate the absolute file path from the relative path.
    const OUString& rRelPath = maRelativeName;
    INetURLObject aBaseURL(rOwnDocName);
    aBaseURL.insertName(OUString(RTL_CONSTASCII_USTRINGPARAM("content.xml")));
    bool bWasAbs = false;
    maRealFileName = aBaseURL.smartRel2Abs(rRelPath, bWasAbs).GetMainURL(INetURLObject::NO_DECODE);
}

void ScExternalRefManager::maybeCreateRealFileName(sal_uInt16 nFileId)
{
    if (nFileId >= maSrcFiles.size())
        return;

    maSrcFiles[nFileId].maybeCreateRealFileName(getOwnDocumentName());
}

OUString ScExternalRefManager::getOwnDocumentName() const
{
    SfxObjectShell* pShell = mpDoc->GetDocumentShell();
    if (!pShell)
        // This should not happen!
        return OUString();

    SfxMedium* pMed = pShell->GetMedium();
    if (!pMed)
        return OUString();

    return pMed->GetName();
}

bool ScExternalRefManager::isOwnDocument(const OUString& rFile) const
{
    return getOwnDocumentName().equals(rFile);
}

void ScExternalRefManager::convertToAbsName(OUString& rFile) const
{
    // unsaved documents have no AbsName
    TypeId aType(TYPE(ScDocShell));
    ScDocShell* pShell = static_cast<ScDocShell*>(SfxObjectShell::GetFirst(&aType, false));
    while (pShell)
    {
        if (rFile == rtl::OUString(pShell->GetName()))
            return;

        pShell = static_cast<ScDocShell*>(SfxObjectShell::GetNext(*pShell, &aType, false));
    }

    SfxObjectShell* pDocShell = mpDoc->GetDocumentShell();
    rFile = ScGlobal::GetAbsDocName(rFile, pDocShell);
}

sal_uInt16 ScExternalRefManager::getExternalFileId(const OUString& rFile)
{
    vector<SrcFileData>::const_iterator itrBeg = maSrcFiles.begin(), itrEnd = maSrcFiles.end();
    vector<SrcFileData>::const_iterator itr = find_if(itrBeg, itrEnd, FindSrcFileByName(rFile));
    if (itr != itrEnd)
    {
        size_t nId = distance(itrBeg, itr);
        return static_cast<sal_uInt16>(nId);
    }

    SrcFileData aData;
    aData.maFileName = rFile;
    maSrcFiles.push_back(aData);
    return static_cast<sal_uInt16>(maSrcFiles.size() - 1);
}

const OUString* ScExternalRefManager::getExternalFileName(sal_uInt16 nFileId, bool bForceOriginal)
{
    if (nFileId >= maSrcFiles.size())
        return NULL;

    if (bForceOriginal)
        return &maSrcFiles[nFileId].maFileName;

    maybeCreateRealFileName(nFileId);

    if (!maSrcFiles[nFileId].maRealFileName.isEmpty())
        return &maSrcFiles[nFileId].maRealFileName;

    return &maSrcFiles[nFileId].maFileName;
}

bool ScExternalRefManager::hasExternalFile(sal_uInt16 nFileId) const
{
    return nFileId < maSrcFiles.size();
}

bool ScExternalRefManager::hasExternalFile(const OUString& rFile) const
{
    vector<SrcFileData>::const_iterator itrBeg = maSrcFiles.begin(), itrEnd = maSrcFiles.end();
    vector<SrcFileData>::const_iterator itr = find_if(itrBeg, itrEnd, FindSrcFileByName(rFile));
    return itr != itrEnd;
}

const ScExternalRefManager::SrcFileData* ScExternalRefManager::getExternalFileData(sal_uInt16 nFileId) const
{
    if (nFileId >= maSrcFiles.size())
        return NULL;

    return &maSrcFiles[nFileId];
}

const OUString* ScExternalRefManager::getRealTableName(sal_uInt16 nFileId, const OUString& rTabName) const
{
    return maRefCache.getRealTableName(nFileId, rTabName);
}

const OUString* ScExternalRefManager::getRealRangeName(sal_uInt16 nFileId, const OUString& rRangeName) const
{
    return maRefCache.getRealRangeName(nFileId, rRangeName);
}

template<typename MapContainer>
static void lcl_removeByFileId(sal_uInt16 nFileId, MapContainer& rMap)
{
    typename MapContainer::iterator itr = rMap.find(nFileId);
    if (itr != rMap.end())
    {
        // Close this document shell.
        itr->second.maShell->DoClose();
        rMap.erase(itr);
    }
}

void ScExternalRefManager::clearCache(sal_uInt16 nFileId)
{
    maRefCache.clearCache(nFileId);
}

void ScExternalRefManager::refreshNames(sal_uInt16 nFileId)
{
    clearCache(nFileId);
    lcl_removeByFileId(nFileId, maDocShells);

    if (maDocShells.empty())
        maSrcDocTimer.Stop();

    // Update all cells containing names from this source document.
    refreshAllRefCells(nFileId);

    notifyAllLinkListeners(nFileId, LINK_MODIFIED);
}

void ScExternalRefManager::breakLink(sal_uInt16 nFileId)
{
    // Turn all formula cells referencing this external document into static
    // cells.
    RefCellMap::iterator itrRefs = maRefCells.find(nFileId);
    if (itrRefs != maRefCells.end())
    {
        // Make a copy because removing the formula cells below will modify
        // the original container.
        RefCellSet aSet = itrRefs->second;
        for_each(aSet.begin(), aSet.end(), ConvertFormulaToStatic(mpDoc));
        maRefCells.erase(nFileId);
    }

    // Remove all named ranges that reference this document.

    // Global named ranges.
    ScRangeName* pRanges = mpDoc->GetRangeName();
    if (pRanges)
        removeRangeNamesBySrcDoc(*pRanges, nFileId);

    // Sheet-local named ranges.
    for (SCTAB i = 0, n = mpDoc->GetTableCount(); i < n; ++i)
    {
        pRanges = mpDoc->GetRangeName(i);
        if (pRanges)
            removeRangeNamesBySrcDoc(*pRanges, nFileId);
    }

    clearCache(nFileId);
    lcl_removeByFileId(nFileId, maDocShells);

    if (maDocShells.empty())
        maSrcDocTimer.Stop();

    LinkedDocMap::iterator itr = maLinkedDocs.find(nFileId);
    if (itr != maLinkedDocs.end())
        itr->second = false;

    notifyAllLinkListeners(nFileId, LINK_BROKEN);
}

void ScExternalRefManager::switchSrcFile(sal_uInt16 nFileId, const OUString& rNewFile, const OUString& rNewFilter)
{
    maSrcFiles[nFileId].maFileName = rNewFile;
    maSrcFiles[nFileId].maRelativeName = OUString();
    maSrcFiles[nFileId].maRealFileName = OUString();
    if (!maSrcFiles[nFileId].maFilterName.equals(rNewFilter))
    {
        // Filter type has changed.
        maSrcFiles[nFileId].maFilterName = rNewFilter;
        maSrcFiles[nFileId].maFilterOptions = OUString();
    }
    refreshNames(nFileId);
}

void ScExternalRefManager::setRelativeFileName(sal_uInt16 nFileId, const OUString& rRelUrl)
{
    if (nFileId >= maSrcFiles.size())
        return;
    maSrcFiles[nFileId].maRelativeName = rRelUrl;
}

void ScExternalRefManager::setFilterData(sal_uInt16 nFileId, const OUString& rFilterName, const OUString& rOptions)
{
    if (nFileId >= maSrcFiles.size())
        return;
    maSrcFiles[nFileId].maFilterName = rFilterName;
    maSrcFiles[nFileId].maFilterOptions = rOptions;
}

void ScExternalRefManager::clear()
{
    DocShellMap::iterator itrEnd = maDocShells.end();
    for (DocShellMap::iterator itr = maDocShells.begin(); itr != itrEnd; ++itr)
        itr->second.maShell->DoClose();

    maDocShells.clear();
    maSrcDocTimer.Stop();
}

bool ScExternalRefManager::hasExternalData() const
{
    return !maSrcFiles.empty();
}

void ScExternalRefManager::resetSrcFileData(const OUString& rBaseFileUrl)
{
    for (vector<SrcFileData>::iterator itr = maSrcFiles.begin(), itrEnd = maSrcFiles.end();
          itr != itrEnd; ++itr)
    {
        // Re-generate relative file name from the absolute file name.
        OUString aAbsName = itr->maRealFileName;
        if (aAbsName.isEmpty())
            aAbsName = itr->maFileName;

        itr->maRelativeName = URIHelper::simpleNormalizedMakeRelative(
            rBaseFileUrl, aAbsName);
    }
}

void ScExternalRefManager::updateAbsAfterLoad()
{
    String aOwn( getOwnDocumentName() );
    for (vector<SrcFileData>::iterator itr = maSrcFiles.begin(), itrEnd = maSrcFiles.end();
          itr != itrEnd; ++itr)
    {
        // update maFileName to the real file name,
        // to be called when the original name is no longer needed (after CompileXML)

        itr->maybeCreateRealFileName( aOwn );
        String aReal = itr->maRealFileName;
        if (aReal.Len())
            itr->maFileName = aReal;
    }
}

void ScExternalRefManager::removeRefCell(ScFormulaCell* pCell)
{
    for_each(maRefCells.begin(), maRefCells.end(), RemoveFormulaCell(pCell));
}

void ScExternalRefManager::addLinkListener(sal_uInt16 nFileId, LinkListener* pListener)
{
    LinkListenerMap::iterator itr = maLinkListeners.find(nFileId);
    if (itr == maLinkListeners.end())
    {
        pair<LinkListenerMap::iterator, bool> r = maLinkListeners.insert(
            LinkListenerMap::value_type(nFileId, LinkListeners()));
        if (!r.second)
        {
            OSL_FAIL("insertion of new link listener list failed");
            return;
        }

        itr = r.first;
    }

    LinkListeners& rList = itr->second;
    rList.insert(pListener);
}

void ScExternalRefManager::removeLinkListener(sal_uInt16 nFileId, LinkListener* pListener)
{
    LinkListenerMap::iterator itr = maLinkListeners.find(nFileId);
    if (itr == maLinkListeners.end())
        // no listeners for a specified file.
        return;

    LinkListeners& rList = itr->second;
    rList.erase(pListener);

    if (rList.empty())
        // No more listeners for this file.  Remove its entry.
        maLinkListeners.erase(itr);
}

void ScExternalRefManager::removeLinkListener(LinkListener* pListener)
{
    LinkListenerMap::iterator itr = maLinkListeners.begin(), itrEnd = maLinkListeners.end();
    for (; itr != itrEnd; ++itr)
        itr->second.erase(pListener);
}

void ScExternalRefManager::notifyAllLinkListeners(sal_uInt16 nFileId, LinkUpdateType eType)
{
    LinkListenerMap::iterator itr = maLinkListeners.find(nFileId);
    if (itr == maLinkListeners.end())
        // no listeners for a specified file.
        return;

    LinkListeners& rList = itr->second;
    for_each(rList.begin(), rList.end(), NotifyLinkListener(nFileId, eType));
}

void ScExternalRefManager::purgeStaleSrcDocument(sal_Int32 nTimeOut)
{
    DocShellMap aNewDocShells;
    DocShellMap::iterator itr = maDocShells.begin(), itrEnd = maDocShells.end();
    for (; itr != itrEnd; ++itr)
    {
        // in 100th of a second.
        sal_Int32 nSinceLastAccess = (Time( Time::SYSTEM ) - itr->second.maLastAccess).GetTime();
        if (nSinceLastAccess < nTimeOut)
            aNewDocShells.insert(*itr);
        else
            // Timed out.  Let's close this.
            itr->second.maShell->DoClose();
    }
    maDocShells.swap(aNewDocShells);

    if (maDocShells.empty())
        maSrcDocTimer.Stop();
}

sal_uInt32 ScExternalRefManager::getMappedNumberFormat(sal_uInt16 nFileId, sal_uInt32 nNumFmt, const ScDocument* pSrcDoc)
{
    NumFmtMap::iterator itr = maNumFormatMap.find(nFileId);
    if (itr == maNumFormatMap.end())
    {
        // Number formatter map is not initialized for this external document.
        pair<NumFmtMap::iterator, bool> r = maNumFormatMap.insert(
            NumFmtMap::value_type(nFileId, SvNumberFormatterMergeMap()));

        if (!r.second)
            // insertion failed.
            return nNumFmt;

        itr = r.first;
        mpDoc->GetFormatTable()->MergeFormatter( *pSrcDoc->GetFormatTable());
        SvNumberFormatterMergeMap aMap = mpDoc->GetFormatTable()->ConvertMergeTableToMap();
        itr->second.swap(aMap);
    }
    const SvNumberFormatterMergeMap& rMap = itr->second;
    SvNumberFormatterMergeMap::const_iterator itrNumFmt = rMap.find(nNumFmt);
    if (itrNumFmt != rMap.end())
        // mapped value found.
        return itrNumFmt->second;

    return nNumFmt;
}

void ScExternalRefManager::transformUnsavedRefToSavedRef( SfxObjectShell* pShell )
{
    DocShellMap::iterator itr = maUnsavedDocShells.begin();
    while( itr != maUnsavedDocShells.end() )
    {
        if (&(itr->second.maShell) == pShell)
        {
            // found that the shell is marked as unsaved
            rtl::OUString aFileURL = pShell->GetMedium()->GetURLObject().GetMainURL(INetURLObject::DECODE_TO_IURI);
            switchSrcFile(itr->first, aFileURL, rtl::OUString());
            EndListening(*pShell);
            maUnsavedDocShells.erase(itr++);
        }
    }
}

void ScExternalRefManager::Notify( SfxBroadcaster&, const SfxHint& rHint )
{
    if ( rHint.ISA( SfxEventHint ) )
    {
        sal_uLong nEventId = ((SfxEventHint&)rHint).GetEventId();
        switch ( nEventId )
        {
            case SFX_EVENT_PREPARECLOSEDOC:
                {
                    SfxObjectShell* pObjShell = static_cast<const SfxEventHint&>( rHint ).GetObjShell();
                    ScDocShell* pDocShell = static_cast< ScDocShell* >( pObjShell );
                    WarningBox aBox(  pDocShell->GetActiveDialogParent(), WinBits( WB_OK ),
                                        ScGlobal::GetRscString( STR_CLOSE_WITH_UNSAVED_REFS ) );
                    aBox.Execute();
                }
                break;
            case SFX_EVENT_SAVEDOCDONE:
            case SFX_EVENT_SAVEASDOCDONE:
                {
                    SfxObjectShell* pObjShell = static_cast<const SfxEventHint&>( rHint ).GetObjShell();
                    transformUnsavedRefToSavedRef(pObjShell);
                }
                break;
            default:
                break;
        }
    }
}

IMPL_LINK(ScExternalRefManager, TimeOutHdl, AutoTimer*, pTimer)
{
    if (pTimer == &maSrcDocTimer)
        purgeStaleSrcDocument(SRCDOC_LIFE_SPAN);

    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
