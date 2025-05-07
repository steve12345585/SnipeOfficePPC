/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


// INCLUDE ---------------------------------------------------------------
#include "xmlsubti.hxx"
#include "global.hxx"
#include "xmlstyli.hxx"
#include "xmlimprt.hxx"
#include "document.hxx"
#include "markdata.hxx"
#include "XMLConverter.hxx"
#include "docuno.hxx"
#include "cellsuno.hxx"
#include "XMLStylesImportHelper.hxx"
#include "sheetdata.hxx"
#include "tabprotection.hxx"
#include "tokenarray.hxx"
#include <svx/svdpage.hxx>

#include <sax/tools/converter.hxx>
#include <xmloff/xmltkmap.hxx>
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmlerror.hxx>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/util/XMergeable.hpp>
#include <com/sun/star/sheet/XSheetCellRange.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/CellInsertMode.hpp>
#include <com/sun/star/sheet/XCellRangeMovement.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/util/XProtectable.hpp>
#include <com/sun/star/sheet/XArrayFormulaRange.hpp>

#include <memory>

using ::std::auto_ptr;

//------------------------------------------------------------------

using namespace com::sun::star;

ScMyTableData::ScMyTableData(SCTAB nSheet, sal_Int32 nCol, sal_Int32 nRow)
    :   nColsPerCol(nDefaultColCount, 1),
        nRealCols(nDefaultColCount + 1, 0),
        nRowsPerRow(nDefaultRowCount, 1),
        nRealRows(nDefaultRowCount + 1, 0),
        nChangedCols()
{
    aTableCellPos.Sheet = nSheet;
    aTableCellPos.Column = nCol;
    aTableCellPos.Row = nRow;

    for (sal_Int32 i = 0; i < 3; ++i)
        nRealCols[i] = i;
    for (sal_Int32 j = 0; j < 3; ++j)
        nRealRows[j] = j;

    nSpannedCols = 1;
    nColCount = 0;
    nSubTableSpanned = 1;
}

ScMyTableData::~ScMyTableData()
{
}

void ScMyTableData::AddRow()
{
    ++aTableCellPos.Row;
    if (static_cast<sal_uInt32>(aTableCellPos.Row) >= nRowsPerRow.size())
    {
        nRowsPerRow.resize(nRowsPerRow.size() + nDefaultRowCount, 1);
        nRealRows.resize(nRowsPerRow.size() + nDefaultRowCount + 1, 0);
    }
    nRealRows[aTableCellPos.Row + 1] = nRealRows[aTableCellPos.Row] + nRowsPerRow[aTableCellPos.Row];
}

void ScMyTableData::AddColumn()
{
    ++aTableCellPos.Column;
    if (static_cast<sal_uInt32>(aTableCellPos.Column) >= nColsPerCol.size())
    {
        nColsPerCol.resize(nColsPerCol.size() + nDefaultColCount, 1);
        nRealCols.resize(nColsPerCol.size() + nDefaultColCount + 1, 0);
    }
    nRealCols[aTableCellPos.Column + 1] = nRealCols[aTableCellPos.Column] + nColsPerCol[aTableCellPos.Column];
}

sal_Int32 ScMyTableData::GetRealCols(const sal_Int32 nIndex, const bool /* bIsNormal */) const
{
    return (nIndex < 0) ? 0 : nRealCols[nIndex];
}

sal_Int32 ScMyTableData::GetChangedCols(const sal_Int32 nFromIndex, const sal_Int32 nToIndex) const
{
    ScMysalIntList::const_iterator i(nChangedCols.begin());
    ScMysalIntList::const_iterator endi(nChangedCols.end());
    while ((i != endi) && ((*i < nToIndex) && !(*i >= nFromIndex)))
        ++i;
    if (i == endi)
        return -1;
    else
        if ((*i >= nFromIndex) && (*i < nToIndex))
            return *i;
        else
            return -1;
}

void ScMyTableData::SetChangedCols(const sal_Int32 nValue)
{
    ScMysalIntList::iterator i(nChangedCols.begin());
    ScMysalIntList::iterator endi(nChangedCols.end());
    while ((i != endi) && (*i < nValue))
    {
        ++i;
    }
    if ((i == endi) || (*i != nValue))
        nChangedCols.insert(i, nValue);
}

/*******************************************************************************************************************************/

ScXMLTabProtectionData::ScXMLTabProtectionData() :
    meHash1(PASSHASH_SHA1),
    meHash2(PASSHASH_UNSPECIFIED),
    mbProtected(false),
    mbSelectProtectedCells(true),
    mbSelectUnprotectedCells(true)
{
}

ScMyTables::ScMyTables(ScXMLImport& rTempImport)
    : rImport(rTempImport),
    aFixupOLEs(rTempImport),
    nCurrentColStylePos(0),
    nCurrentDrawPage( -1 ),
    nCurrentXShapes( -1 ),
    nCurrentSheet( -1 ),
    pCurrentTab(NULL)
{
}

ScMyTables::~ScMyTables()
{
}

namespace {

uno::Reference<sheet::XSpreadsheet> getCurrentSheet(const uno::Reference<frame::XModel>& xModel, SCTAB nSheet)
{
    uno::Reference<sheet::XSpreadsheet> xSheet;
    uno::Reference<sheet::XSpreadsheetDocument> xSpreadDoc(xModel, uno::UNO_QUERY);
    if (!xSpreadDoc.is())
        return xSheet;

    uno::Reference <sheet::XSpreadsheets> xSheets(xSpreadDoc->getSheets());
    if (!xSheets.is())
        return xSheet;

    uno::Reference <container::XIndexAccess> xIndex(xSheets, uno::UNO_QUERY);
    if (!xIndex.is())
        return xSheet;

    xSheet.set(xIndex->getByIndex(nSheet), uno::UNO_QUERY);
    return xSheet;
}

}

void ScMyTables::NewSheet(const rtl::OUString& sTableName, const rtl::OUString& sStyleName,
                          const ScXMLTabProtectionData& rProtectData)
{
    if (rImport.GetModel().is())
    {
        nCurrentColStylePos = 0;
        sCurrentSheetName = sTableName;
        maTables.clear();
        pCurrentTab = NULL;
        ++nCurrentSheet;

        maProtectionData = rProtectData;
        ScDocument *pDoc = ScXMLConverter::GetScDocument(rImport.GetModel());

        // The document contains one sheet when created. So for the first
        // sheet, we only need to set its name.
        if (nCurrentSheet > 0)
            pDoc->AppendTabOnLoad(sTableName);
        else
            pDoc->SetTabNameOnLoad(nCurrentSheet, sTableName);

        rImport.SetTableStyle(sStyleName);
        xCurrentSheet = getCurrentSheet(rImport.GetModel(), nCurrentSheet);
        if (xCurrentSheet.is())
        {
            // We need to set the current cell range here regardless of
            // presence of style name.
            xCurrentCellRange.set(xCurrentSheet, uno::UNO_QUERY);
            SetTableStyle(sStyleName);
        }
    }

    NewTable(1);
}

void ScMyTables::SetTableStyle(const rtl::OUString& sStyleName)
{
    //these uno calls are a bit difficult to remove, XMLTableStyleContext::FillPropertySet uses
    //SvXMLImportPropertyMapper::FillPropertySet
    if ( !sStyleName.isEmpty() )
    {
        // #i57869# All table style properties for all sheets are now applied here,
        // before importing the contents.
        // This is needed for the background color.
        // Sheet visibility has special handling in ScDocFunc::SetTableVisible to
        // allow hiding the first sheet.
        // RTL layout is only remembered, not actually applied, so the shapes can
        // be loaded before mirroring.

        if ( xCurrentSheet.is() )
        {
            xCurrentCellRange.set(xCurrentSheet, uno::UNO_QUERY);
            uno::Reference <beans::XPropertySet> xProperties(xCurrentSheet, uno::UNO_QUERY);
            if ( xProperties.is() )
            {
                XMLTableStylesContext *pStyles = (XMLTableStylesContext *)rImport.GetAutoStyles();
                if ( pStyles )
                {
                    XMLTableStyleContext* pStyle = (XMLTableStyleContext *)pStyles->FindStyleChildContext(
                            XML_STYLE_FAMILY_TABLE_TABLE, sStyleName, true);
                    if ( pStyle )
                    {
                        pStyle->FillPropertySet(xProperties);

                        ScSheetSaveData* pSheetData = ScModelObj::getImplementation(rImport.GetModel())->GetSheetSaveData();
                        pSheetData->AddTableStyle( sStyleName, ScAddress( 0, 0, nCurrentSheet ) );
                    }
                }
            }
        }
    }
}

bool ScMyTables::IsMerged (const uno::Reference <table::XCellRange>& xCellRange, const sal_Int32 nCol, const sal_Int32 nRow,
                            table::CellRangeAddress& aCellAddress) const
{
    uno::Reference <util::XMergeable> xMergeable (xCellRange->getCellRangeByPosition(nCol,nRow,nCol,nRow), uno::UNO_QUERY);
    if (xMergeable.is())
    {
        uno::Reference<sheet::XSheetCellRange> xMergeSheetCellRange (xMergeable, uno::UNO_QUERY);
        uno::Reference<sheet::XSpreadsheet> xTable(xMergeSheetCellRange->getSpreadsheet());
        uno::Reference<sheet::XSheetCellCursor> xMergeSheetCursor(xTable->createCursorByRange(xMergeSheetCellRange));
        if (xMergeSheetCursor.is())
        {
            xMergeSheetCursor->collapseToMergedArea();
            uno::Reference<sheet::XCellRangeAddressable> xMergeCellAddress (xMergeSheetCursor, uno::UNO_QUERY);
            if (xMergeCellAddress.is())
            {
                aCellAddress = xMergeCellAddress->getRangeAddress();
                if (aCellAddress.StartColumn == nCol && aCellAddress.EndColumn == nCol &&
                    aCellAddress.StartRow == nRow && aCellAddress.EndRow == nRow)
                    return false;
                else
                    return true;
            }
        }
    }
    return false;
}

void ScMyTables::UnMerge()
{
    if ( xCurrentCellRange.is() )
    {
        table::CellRangeAddress aCellAddress;
        if (IsMerged(xCurrentCellRange, GetRealCellPos().Column, GetRealCellPos().Row, aCellAddress))
        {
            //unmerge
            uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                    aCellAddress.EndColumn, aCellAddress.EndRow), uno::UNO_QUERY);
            if (xMergeable.is())
                xMergeable->merge(false);
        }
    }
}

void ScMyTables::DoMerge(sal_Int32 nCount)
{
    if ( xCurrentCellRange.is() )
    {
        table::CellRangeAddress aCellAddress;
        if (IsMerged(xCurrentCellRange, GetRealCellPos().Column, GetRealCellPos().Row, aCellAddress))
        {
            //unmerge
            uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                    aCellAddress.EndColumn, aCellAddress.EndRow), uno::UNO_QUERY);
            if (xMergeable.is())
                xMergeable->merge(false);
        }

        //merge
        uno::Reference <table::XCellRange> xMergeCellRange;
        if (nCount == -1)
        {
            const ScMyTableData& r = *pCurrentTab;
            xMergeCellRange.set(
                xCurrentCellRange->getCellRangeByPosition(
                    aCellAddress.StartColumn, aCellAddress.StartRow,
                    aCellAddress.EndColumn + r.GetColsPerCol(r.GetColumn()) - 1,
                    aCellAddress.EndRow + r.GetRowsPerRow(r.GetRow()) - 1));
        }
        else
            xMergeCellRange.set(
                xCurrentCellRange->getCellRangeByPosition(
                    aCellAddress.StartColumn, aCellAddress.StartRow,
                    aCellAddress.StartColumn + nCount - 1,
                    aCellAddress.EndRow));

        uno::Reference <util::XMergeable> xMergeable (xMergeCellRange, uno::UNO_QUERY);
        if (xMergeable.is())
            xMergeable->merge(true);
    }
}

void ScMyTables::InsertRow()
{
    if ( xCurrentCellRange.is() )
    {
        table::CellRangeAddress aCellAddress;
        sal_Int32 nRow(GetRealCellPos().Row);
        for (sal_Int32 j = 0; j < GetRealCellPos().Column - pCurrentTab->GetColumn() - 1; ++j)
        {
            if (IsMerged(xCurrentCellRange, j, nRow - 1, aCellAddress))
            {
                //unmerge
                uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                        aCellAddress.EndColumn, aCellAddress.EndRow), uno::UNO_QUERY);
                if (xMergeable.is())
                    xMergeable->merge(false);
            }

            //merge
            uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                    aCellAddress.EndColumn, aCellAddress.EndRow + 1), uno::UNO_QUERY);
            if (xMergeable.is())
                xMergeable->merge(true);
            j += aCellAddress.EndColumn - aCellAddress.StartColumn;
        }
        rImport.GetStylesImportHelper()->InsertRow(nRow, nCurrentSheet, rImport.GetDocument());
    }
}

void ScMyTables::NewRow()
{
    size_t n = maTables.size();
    if (n <= 1)
        return;

    if (pCurrentTab->GetRealRows(pCurrentTab->GetRow()) >
        maTables[n-2].GetRowsPerRow(maTables[n-2].GetRow()) - 1)
    {
        if (GetRealCellPos().Column > 0)
            InsertRow();

        for (size_t i = n - 1; i > 0; --i)
        {
            sal_Int32 nRow = maTables[i-1].GetRow();
            maTables[i-1].SetRowsPerRow(
                nRow,
                maTables[i-1].GetRowsPerRow(nRow) + 1);

            maTables[i-1].SetRealRows(
                nRow + 1,
                maTables[i-1].GetRealRows(nRow) + maTables[i-1].GetRowsPerRow(nRow));
        }
    }
}

void ScMyTables::AddRow()
{
    pCurrentTab->AddRow();
    pCurrentTab->SetFirstColumn();
    sal_Int32 nRow = pCurrentTab->GetRow();
    if (nRow > 0)
        NewRow();

    pCurrentTab->SetRealRows(
        nRow + 1, pCurrentTab->GetRealRows(nRow) + pCurrentTab->GetRowsPerRow(nRow));
}

void ScMyTables::SetRowStyle(const rtl::OUString& rCellStyleName)
{
    rImport.GetStylesImportHelper()->SetRowStyle(rCellStyleName);
}

void ScMyTables::InsertColumn()
{
    if ( xCurrentCellRange.is() )
    {
        table::CellRangeAddress aCellAddress;
        sal_Int32 nCol(GetRealCellPos().Column);
        sal_Int32 n = GetRealCellPos().Row - pCurrentTab->GetRow() - 1;
        for (sal_Int32 j = 0; j <= n; ++j)
        {
            table::CellRangeAddress aTempCellAddress;
            if (IsMerged(xCurrentCellRange, nCol - 1, j, aCellAddress))
            {
                //unmerge
                uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                        aCellAddress.EndColumn, aCellAddress.EndRow), uno::UNO_QUERY);
                if (xMergeable.is())
                    xMergeable->merge(false);
                aTempCellAddress = aCellAddress;
                aTempCellAddress.StartColumn = aTempCellAddress.EndColumn + 1;
                aTempCellAddress.EndColumn = aTempCellAddress.StartColumn;
            }
            else
            {
                aTempCellAddress = aCellAddress;
                aTempCellAddress.StartColumn += 1;
                aTempCellAddress.EndColumn = aTempCellAddress.StartColumn;
            }

            //insert Cell
            sheet::CellInsertMode aCellInsertMode(sheet::CellInsertMode_RIGHT);
            uno::Reference <sheet::XCellRangeMovement>  xCellRangeMovement (xCurrentSheet, uno::UNO_QUERY);
            xCellRangeMovement->insertCells(aTempCellAddress, aCellInsertMode);

            //merge
            uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                        aCellAddress.EndColumn + 1, aCellAddress.EndRow), uno::UNO_QUERY);
            if (xMergeable.is())
                xMergeable->merge(true);
            j += aCellAddress.EndRow - aCellAddress.StartRow;
        }
        rImport.GetStylesImportHelper()->InsertCol(nCol, nCurrentSheet, rImport.GetDocument());
    }
}

void ScMyTables::NewColumn(bool bIsCovered)
{
    if (bIsCovered)
        return;

    sal_Int32 nColCount = pCurrentTab->GetColCount();
    sal_Int32 nSpannedCols = pCurrentTab->GetSpannedCols();
    if ( (nSpannedCols > nColCount) &&
        (pCurrentTab->GetRow() == 0) &&
        (pCurrentTab->GetColumn() == 0) )
    {
        if (nColCount > 0)
        {
            sal_Int32 FirstColsSpanned(nSpannedCols / nColCount);
            sal_Int32 LastColSpanned(FirstColsSpanned
                + (nSpannedCols % nColCount));
            for (sal_Int32 i = 0; i < nColCount - 1; ++i)
            {
                pCurrentTab->SetColsPerCol(i, FirstColsSpanned);
                pCurrentTab->SetRealCols(i + 1, pCurrentTab->GetRealCols(i) + FirstColsSpanned);
            }
            pCurrentTab->SetColsPerCol(nColCount - 1, LastColSpanned);
            pCurrentTab->SetRealCols(
                nColCount, pCurrentTab->GetRealCols(nColCount - 1) + LastColSpanned);
        }
    }
    if (pCurrentTab->GetRealCols(pCurrentTab->GetColumn()) > nSpannedCols - 1)
    {
        if (pCurrentTab->GetRow() == 0)
        {
            InsertColumn();
            size_t n = maTables.size();
            for (size_t i = n - 1; i > 0; --i)
            {
                sal_Int32 nColPos =
                    maTables[i-1].GetColumn() + maTables[i].GetSpannedCols() - 1;

                maTables[i-1].SetColsPerCol(nColPos,
                    maTables[i-1].GetColsPerCol(nColPos) +
                    pCurrentTab->GetColsPerCol(pCurrentTab->GetColumn()));

                maTables[i-1].SetRealCols(
                    nColPos + 1,
                    maTables[i-1].GetRealCols(nColPos) + maTables[i-1].GetColsPerCol(nColPos));

                maTables[i-1].SetChangedCols(nColPos);
            }
        }
    }
}

void ScMyTables::AddColumn(bool bIsCovered)
{
    pCurrentTab->AddColumn();
    if (pCurrentTab->GetSubTableSpanned() > 1)
        pCurrentTab->SetSubTableSpanned(pCurrentTab->GetSubTableSpanned() - 1);
    else
    {
        NewColumn(bIsCovered);
        sal_Int32 nCol = pCurrentTab->GetColumn();
        sal_Int32 nRow = pCurrentTab->GetRow();
        pCurrentTab->SetRealCols(
            nCol + 1, pCurrentTab->GetRealCols(nCol) + pCurrentTab->GetColsPerCol(nCol));

        if ((!bIsCovered) || (bIsCovered && (pCurrentTab->GetColsPerCol(nCol) > 1)))
        {
            if ((pCurrentTab->GetRowsPerRow(nRow) > 1) || (pCurrentTab->GetColsPerCol(nCol) > 1))
                DoMerge();
        }
    }
}

void ScMyTables::NewTable(sal_Int32 nTempSpannedCols)
{
    maTables.push_back(new ScMyTableData(nCurrentSheet));
    pCurrentTab = & maTables.back();

    size_t nTables = maTables.size();
    if (nTables > 1)
    {
        ScMyTableData& rFirstTab = maTables.front();

        const sal_Int32 nCol = rFirstTab.GetColumn();
        const sal_Int32 nColCount = rFirstTab.GetColCount();
        const sal_Int32 nColsPerCol = rFirstTab.GetColsPerCol(nCol);

        sal_Int32 nSpannedCols = rFirstTab.GetSpannedCols();
        sal_Int32 nTemp = nSpannedCols - nColCount;
        sal_Int32 nTemp2 = nCol - nColCount + 1;
        if ((nTemp > 0) && (nTemp2 == 0))
            nTempSpannedCols *= nTemp + 1;
        else
            if (nColsPerCol > 1)
                nTempSpannedCols *= nColsPerCol;

        sal_Int32 nToMerge;
        if (nSpannedCols > nColCount)
            nToMerge = rFirstTab.GetChangedCols(nCol, nCol + nColsPerCol + nSpannedCols - nColCount);
        else
            nToMerge = rFirstTab.GetChangedCols(nCol, nCol + nColsPerCol);
        if (nToMerge > nCol)
            nTempSpannedCols += nToMerge;
    }

    pCurrentTab->SetSpannedCols(nTempSpannedCols);

    if (nTables > 1)
    {
        maTables[nTables-2].SetSubTableSpanned(pCurrentTab->GetSpannedCols());
        UnMerge();
    }
}

void ScMyTables::UpdateRowHeights()
{
    if (rImport.GetModel().is())
    {
        ScXMLImport::MutexGuard aGuard(rImport);

        // update automatic row heights

        // For sheets with any kind of shapes (including notes),
        // update row heights immediately (before setting the positions).
        // For sheets without shapes, set "pending" flag
        // and update row heights when a sheet is shown.
        // The current sheet (from view settings) is always updated immediately.

        ScDocument* pDoc = ScXMLConverter::GetScDocument(rImport.GetModel());
        if (pDoc)
        {
            SCTAB nCount = pDoc->GetTableCount();
            ScDrawLayer* pDrawLayer = pDoc->GetDrawLayer();

            SCTAB nVisible = rImport.GetVisibleSheet();

            ScMarkData aUpdateSheets;
            for (SCTAB nTab=0; nTab<nCount; ++nTab)
            {
                const SdrPage* pPage = pDrawLayer ? pDrawLayer->GetPage(nTab) : NULL;
                if ( nTab == nVisible || ( pPage && pPage->GetObjCount() != 0 ) )
                    aUpdateSheets.SelectTable( nTab, true );
                else
                    pDoc->SetPendingRowHeights( nTab, true );
            }

            if (aUpdateSheets.GetSelectCount())
            {
                pDoc->LockStreamValid( true );      // ignore draw page size (but not formula results)
                ScModelObj::getImplementation(rImport.GetModel())->UpdateAllRowHeights(&aUpdateSheets);
                pDoc->LockStreamValid( false );
            }
        }
    }
}

void ScMyTables::DeleteTable()
{
    ScXMLImport::MutexGuard aGuard(rImport);

    nCurrentColStylePos = 0;
    if (!maTables.empty())
    {
        maTables.pop_back();
        if (!maTables.empty())
            pCurrentTab = &maTables.back();
        else
            pCurrentTab = NULL;
    }

    if (maTables.empty()) // only set the styles if all subtables are imported and the table is finished
    {
        rImport.GetStylesImportHelper()->SetStylesToRanges();
        rImport.SetStylesToRangesFinished();
    }

    //#i48793#; has to be set before protection
    if (!aMatrixRangeList.empty())
    {
        ScMyMatrixRangeList::iterator aItr = aMatrixRangeList.begin();
        ScMyMatrixRangeList::iterator aEndItr = aMatrixRangeList.end();
        while(aItr != aEndItr)
        {
            SetMatrix(aItr->aScRange, aItr->sFormula, aItr->sFormulaNmsp, aItr->eGrammar);
            ++aItr;
        }
        aMatrixRangeList.clear();
    }

    if (rImport.GetDocument() && maProtectionData.mbProtected)
    {
        uno::Sequence<sal_Int8> aHash;
        ::sax::Converter::decodeBase64(aHash, maProtectionData.maPassword);

        SAL_WNODEPRECATED_DECLARATIONS_PUSH
        auto_ptr<ScTableProtection> pProtect(new ScTableProtection);
        SAL_WNODEPRECATED_DECLARATIONS_POP
        pProtect->setProtected(maProtectionData.mbProtected);
        pProtect->setPasswordHash(aHash, maProtectionData.meHash1, maProtectionData.meHash2);
        pProtect->setOption(ScTableProtection::SELECT_LOCKED_CELLS,   maProtectionData.mbSelectProtectedCells);
        pProtect->setOption(ScTableProtection::SELECT_UNLOCKED_CELLS, maProtectionData.mbSelectUnprotectedCells);
        rImport.GetDocument()->SetTabProtection(nCurrentSheet, pProtect.get());
    }
}

table::CellAddress ScMyTables::GetRealCellPos()
{
    sal_Int32 nRow = 0;
    sal_Int32 nCol = 0;
    size_t n = maTables.size();
    for (size_t i = 0; i < n; ++i)
    {
        const ScMyTableData& rTab = maTables[i];
        nCol += rTab.GetRealCols(rTab.GetColumn());
        nRow += rTab.GetRealRows(rTab.GetRow());
    }

    aRealCellPos.Row = nRow;
    aRealCellPos.Column = nCol;
    aRealCellPos.Sheet = nCurrentSheet;
    return aRealCellPos;
}

ScAddress ScMyTables::GetRealScCellPos() const
{
    sal_Int32 nRow = 0;
    sal_Int32 nCol = 0;
    size_t n = maTables.size();
    for (size_t i = 0; i < n; ++i)
    {
        const ScMyTableData& rTab = maTables[i];
        nCol += rTab.GetRealCols(rTab.GetColumn());
        nRow += rTab.GetRealRows(rTab.GetRow());
    }
    return ScAddress( static_cast<SCCOL>(nCol), static_cast<SCCOL>(nRow), nCurrentSheet );
}

void ScMyTables::AddColCount(sal_Int32 nTempColCount)
{
    pCurrentTab->SetColCount(pCurrentTab->GetColCount() + nTempColCount);
}

void ScMyTables::AddColStyle(const sal_Int32 nRepeat, const rtl::OUString& rCellStyleName)
{
    OSL_ENSURE(maTables.size() == 1, "not possible to use default styles on columns in subtables");
    rImport.GetStylesImportHelper()->AddColumnStyle(rCellStyleName, nCurrentColStylePos, nRepeat);
    nCurrentColStylePos += nRepeat;
}

uno::Reference< drawing::XDrawPage > ScMyTables::GetCurrentXDrawPage()
{
    if( (nCurrentSheet != nCurrentDrawPage) || !xDrawPage.is() )
    {
        uno::Reference<drawing::XDrawPageSupplier> xDrawPageSupplier( xCurrentSheet, uno::UNO_QUERY );
        if( xDrawPageSupplier.is() )
            xDrawPage.set(xDrawPageSupplier->getDrawPage());
        nCurrentDrawPage = sal::static_int_cast<sal_Int16>(nCurrentSheet);
    }
    return xDrawPage;
}

uno::Reference< drawing::XShapes > ScMyTables::GetCurrentXShapes()
{
    if( (nCurrentSheet != nCurrentXShapes) || !xShapes.is() )
    {
        xShapes.set(GetCurrentXDrawPage(), uno::UNO_QUERY);
        rImport.GetShapeImport()->startPage(xShapes);
        rImport.GetShapeImport()->pushGroupForSorting ( xShapes );
        nCurrentXShapes = sal::static_int_cast<sal_Int16>(nCurrentSheet);
        return xShapes;
    }
    else
        return xShapes;
}

bool ScMyTables::HasDrawPage()
{
    return !((nCurrentSheet != nCurrentDrawPage) || !xDrawPage.is());
}

bool ScMyTables::HasXShapes()
{
    return !((nCurrentSheet != nCurrentXShapes) || !xShapes.is());
}

void ScMyTables::AddOLE(uno::Reference <drawing::XShape>& rShape,
      const rtl::OUString &rRangeList)
{
      aFixupOLEs.AddOLE(rShape, rRangeList);
}

void ScMyTables::AddMatrixRange(
        const SCCOL nStartColumn, const SCROW nStartRow, const SCCOL nEndColumn, const SCROW nEndRow,
        const rtl::OUString& rFormula, const rtl::OUString& rFormulaNmsp, const formula::FormulaGrammar::Grammar eGrammar)
{
    OSL_ENSURE(nEndRow >= nStartRow, "wrong row order");
    OSL_ENSURE(nEndColumn >= nStartColumn, "wrong column order");
    ScRange aScRange(
        nStartColumn, nStartRow, nCurrentSheet,
        nEndColumn, nEndRow, nCurrentSheet
    );
    ScMatrixRange aMRange(aScRange, rFormula, rFormulaNmsp, eGrammar);
    aMatrixRangeList.push_back(aMRange);
}

bool ScMyTables::IsPartOfMatrix(const SCCOL nColumn, const SCROW nRow)
{
    bool bResult(false);
    if (!aMatrixRangeList.empty())
    {
        ScMyMatrixRangeList::iterator aItr(aMatrixRangeList.begin());
        ScMyMatrixRangeList::iterator aEndItr(aMatrixRangeList.end());
        bool bReady(false);
        while(!bReady && aItr != aEndItr)
        {
            if (nCurrentSheet > aItr->aScRange.aStart.Tab())
            {
                OSL_FAIL("should never hapen, because the list should be cleared in DeleteTable");
                aItr = aMatrixRangeList.erase(aItr);
            }
            else if ((nRow > aItr->aScRange.aEnd.Row()) && (nColumn > aItr->aScRange.aEnd.Col()))
            {
                SetMatrix(aItr->aScRange, aItr->sFormula, aItr->sFormulaNmsp, aItr->eGrammar);
                aItr = aMatrixRangeList.erase(aItr);
            }
            else if (nColumn < aItr->aScRange.aStart.Col())
                bReady = true;
            else if ( nColumn >= aItr->aScRange.aStart.Col() && nColumn <= aItr->aScRange.aEnd.Col() &&
                      nRow >= aItr->aScRange.aStart.Row() && nRow <= aItr->aScRange.aEnd.Row() )
            {
                bReady = true;
                bResult = true;
            }
            else
                ++aItr;
        }
    }
    return bResult;
}

void ScMyTables::SetMatrix(const ScRange& rScRange, const rtl::OUString& rFormula,
        const rtl::OUString& rFormulaNmsp, const formula::FormulaGrammar::Grammar eGrammar)
{
    ScDocument* pDoc = rImport.GetDocument();
    ScMarkData aMark;
    aMark.SetMarkArea( rScRange );
    aMark.SelectTable( rScRange.aStart.Tab(), sal_True );
    ScTokenArray* pCode = new ScTokenArray;
    pCode->AddStringXML( rFormula );
    if( (eGrammar == formula::FormulaGrammar::GRAM_EXTERNAL) && !rFormulaNmsp.isEmpty() )
        pCode->AddStringXML( rFormulaNmsp );
    pDoc->InsertMatrixFormula(
        rScRange.aStart.Col(), rScRange.aStart.Row(),
        rScRange.aEnd.Col(), rScRange.aEnd.Row(),
        aMark, EMPTY_STRING, pCode, eGrammar );
    delete pCode;
    pDoc->IncXMLImportedFormulaCount( rFormula.getLength() );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
