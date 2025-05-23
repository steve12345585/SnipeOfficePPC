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

#include "calc/CTable.hxx"
#include <com/sun/star/sdbc/ColumnValue.hpp>
#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/XCellRangesQuery.hpp>
#include <com/sun/star/sheet/XDatabaseRanges.hpp>
#include <com/sun/star/sheet/XDatabaseRange.hpp>
#include <com/sun/star/sheet/XCellRangeReferrer.hpp>
#include <com/sun/star/sheet/XUsedAreaCursor.hpp>
#include <com/sun/star/sheet/CellFlags.hpp>
#include <com/sun/star/sheet/FormulaResult.hpp>
#include <com/sun/star/util/NumberFormat.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/text/XText.hpp>
#include <svl/converter.hxx>
#include "calc/CConnection.hxx"
#include "calc/CColumns.hxx"
#include "connectivity/sdbcx/VColumn.hxx"
#include <rtl/ustrbuf.hxx>
#include <osl/thread.h>
#include <comphelper/sequence.hxx>
#include <svl/zforlist.hxx>
#include <rtl/math.hxx>
#include <comphelper/extract.hxx>
#include <connectivity/dbexception.hxx>
#include <connectivity/dbconversion.hxx>
#include <comphelper/types.hxx>
#include <rtl/logfile.hxx>

using namespace connectivity;
using namespace connectivity::calc;
using namespace connectivity::file;
using namespace ::cppu;
using namespace ::dbtools;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::sheet;
using namespace ::com::sun::star::table;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::util;


static void lcl_UpdateArea( const Reference<XCellRange>& xUsedRange, sal_Int32& rEndCol, sal_Int32& rEndRow )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::lcl_UpdateArea" );
    //  update rEndCol, rEndRow if any non-empty cell in xUsedRange is right/below

    const Reference<XCellRangesQuery> xUsedQuery( xUsedRange, UNO_QUERY );
    if ( xUsedQuery.is() )
    {
        const sal_Int16 nContentFlags =
            CellFlags::STRING | CellFlags::VALUE | CellFlags::DATETIME | CellFlags::FORMULA | CellFlags::ANNOTATION;

        const Reference<XSheetCellRanges> xUsedRanges = xUsedQuery->queryContentCells( nContentFlags );
        const Sequence<CellRangeAddress> aAddresses = xUsedRanges->getRangeAddresses();

        const sal_Int32 nCount = aAddresses.getLength();
        const CellRangeAddress* pData = aAddresses.getConstArray();
        for ( sal_Int32 i=0; i<nCount; i++ )
        {
            rEndCol = pData[i].EndColumn > rEndCol ? pData[i].EndColumn : rEndCol;
            rEndRow = pData[i].EndRow    > rEndRow ? pData[i].EndRow    : rEndRow;
        }
    }
}

static void lcl_GetDataArea( const Reference<XSpreadsheet>& xSheet, sal_Int32& rColumnCount, sal_Int32& rRowCount )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::lcl_GetDataArea" );
    Reference<XSheetCellCursor> xCursor = xSheet->createCursor();
    Reference<XCellRangeAddressable> xRange( xCursor, UNO_QUERY );
    if ( !xRange.is() )
    {
        rColumnCount = rRowCount = 0;
        return;
    }

    // first find the contiguous cell area starting at A1

    xCursor->collapseToSize( 1, 1 );        // single (first) cell
    xCursor->collapseToCurrentRegion();     // contiguous data area

    CellRangeAddress aRegionAddr = xRange->getRangeAddress();
    sal_Int32 nEndCol = aRegionAddr.EndColumn;
    sal_Int32 nEndRow = aRegionAddr.EndRow;

    Reference<XUsedAreaCursor> xUsed( xCursor, UNO_QUERY );
    if ( xUsed.is() )
    {
        //  The used area from XUsedAreaCursor includes visible attributes.
        //  If the used area is larger than the contiguous cell area, find non-empty
        //  cells in that area.

        xUsed->gotoEndOfUsedArea( sal_False );
        CellRangeAddress aUsedAddr = xRange->getRangeAddress();

        if ( aUsedAddr.EndColumn > aRegionAddr.EndColumn )
        {
            Reference<XCellRange> xUsedRange = xSheet->getCellRangeByPosition(
                aRegionAddr.EndColumn + 1, 0, aUsedAddr.EndColumn, aUsedAddr.EndRow );
            lcl_UpdateArea( xUsedRange, nEndCol, nEndRow );
        }

        if ( aUsedAddr.EndRow > aRegionAddr.EndRow )
        {
            //  only up to the last column of aRegionAddr, the other columns are handled above
            Reference<XCellRange> xUsedRange = xSheet->getCellRangeByPosition(
                0, aRegionAddr.EndRow + 1, aRegionAddr.EndColumn, aUsedAddr.EndRow );
            lcl_UpdateArea( xUsedRange, nEndCol, nEndRow );
        }
    }

    rColumnCount = nEndCol + 1;     // number of columns
    rRowCount = nEndRow;            // first row (headers) is not counted
}

static CellContentType lcl_GetContentOrResultType( const Reference<XCell>& xCell )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::lcl_GetContentOrResultType" );
    CellContentType eCellType = xCell->getType();
    if ( eCellType == CellContentType_FORMULA )
    {
        static const ::rtl::OUString s_sFormulaResultType("FormulaResultType");
        Reference<XPropertySet> xProp( xCell, UNO_QUERY );
        try
        {
            xProp->getPropertyValue( s_sFormulaResultType ) >>= eCellType;      // type of formula result
        }
        catch (UnknownPropertyException&)
        {
            eCellType = CellContentType_VALUE;  // if FormulaResultType property not available
        }
    }
    return eCellType;
}

static Reference<XCell> lcl_GetUsedCell( const Reference<XSpreadsheet>& xSheet, sal_Int32 nDocColumn, sal_Int32 nDocRow )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::lcl_GetUsedCell" );
    Reference<XCell> xCell = xSheet->getCellByPosition( nDocColumn, nDocRow );
    if ( xCell.is() && xCell->getType() == CellContentType_EMPTY )
    {
        //  get first non-empty cell

        Reference<XCellRangeAddressable> xAddr( xSheet, UNO_QUERY );
        if (xAddr.is())
        {
            CellRangeAddress aTotalRange = xAddr->getRangeAddress();
            sal_Int32 nLastRow = aTotalRange.EndRow;
            Reference<XCellRangesQuery> xQuery( xSheet->getCellRangeByPosition( nDocColumn, nDocRow, nDocColumn, nLastRow ), UNO_QUERY );
            if (xQuery.is())
            {
                // queryIntersection to get a ranges object
                Reference<XSheetCellRanges> xRanges = xQuery->queryIntersection( aTotalRange );
                if (xRanges.is())
                {
                    Reference<XEnumerationAccess> xCells = xRanges->getCells();
                    if (xCells.is())
                    {
                        Reference<XEnumeration> xEnum = xCells->createEnumeration();
                        if ( xEnum.is() && xEnum->hasMoreElements() )
                        {
                            // get first non-empty cell from enumeration
                            xCell.set(xEnum->nextElement(),UNO_QUERY);
                        }
                        // otherwise, keep empty cell
                    }
                }
            }
        }
    }
    return xCell;
}

static bool lcl_HasTextInColumn( const Reference<XSpreadsheet>& xSheet, sal_Int32 nDocColumn, sal_Int32 nDocRow )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::lcl_HasTextInColumn" );
    // look for any text cell or text result in the column

    Reference<XCellRangeAddressable> xAddr( xSheet, UNO_QUERY );
    if (xAddr.is())
    {
        CellRangeAddress aTotalRange = xAddr->getRangeAddress();
        sal_Int32 nLastRow = aTotalRange.EndRow;
        Reference<XCellRangesQuery> xQuery( xSheet->getCellRangeByPosition( nDocColumn, nDocRow, nDocColumn, nLastRow ), UNO_QUERY );
        if (xQuery.is())
        {
            // are there text cells in the column?
            Reference<XSheetCellRanges> xTextContent = xQuery->queryContentCells( CellFlags::STRING );
            if ( xTextContent.is() && xTextContent->hasElements() )
                return true;

            // are there formulas with text results in the column?
            Reference<XSheetCellRanges> xTextFormula = xQuery->queryFormulaCells( FormulaResult::STRING );
            if ( xTextFormula.is() && xTextFormula->hasElements() )
                return true;
        }
    }

    return false;
}

static void lcl_GetColumnInfo( const Reference<XSpreadsheet>& xSheet, const Reference<XNumberFormats>& xFormats,
                        sal_Int32 nDocColumn, sal_Int32 nStartRow, sal_Bool bHasHeaders,
                        ::rtl::OUString& rName, sal_Int32& rDataType, sal_Bool& rCurrency )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::lcl_GetColumnInfo" );
    //! avoid duplicate field names

    //  get column name from first row, if range contains headers

    if ( bHasHeaders )
    {
        Reference<XText> xHeaderText( xSheet->getCellByPosition( nDocColumn, nStartRow ), UNO_QUERY );
        if ( xHeaderText.is() )
            rName = xHeaderText->getString();
    }

    // get column type from first data row

    sal_Int32 nDataRow = nStartRow;
    if ( bHasHeaders )
        ++nDataRow;
    Reference<XCell> xDataCell = lcl_GetUsedCell( xSheet, nDocColumn, nDataRow );

    Reference<XPropertySet> xProp( xDataCell, UNO_QUERY );
    if ( xProp.is() )
    {
        rCurrency = sal_False;          // set to true for currency below

        const CellContentType eCellType = lcl_GetContentOrResultType( xDataCell );
        // #i35178# use "text" type if there is any text cell in the column
        if ( eCellType == CellContentType_TEXT || lcl_HasTextInColumn( xSheet, nDocColumn, nDataRow ) )
            rDataType = DataType::VARCHAR;
        else if ( eCellType == CellContentType_VALUE )
        {
            //  get number format to distinguish between different types

            sal_Int16 nNumType = NumberFormat::NUMBER;
            try
            {
                static ::rtl::OUString s_NumberFormat("NumberFormat");
                sal_Int32 nKey = 0;

                if ( xProp->getPropertyValue( s_NumberFormat ) >>= nKey )
                {
                    const Reference<XPropertySet> xFormat = xFormats->getByKey( nKey );
                    if ( xFormat.is() )
                    {
                        xFormat->getPropertyValue( OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE) ) >>= nNumType;
                    }
                }
            }
            catch ( Exception& )
            {
            }

            if ( nNumType & NumberFormat::TEXT )
                rDataType = DataType::VARCHAR;
            else if ( nNumType & NumberFormat::NUMBER )
                rDataType = DataType::DECIMAL;
            else if ( nNumType & NumberFormat::CURRENCY )
            {
                rCurrency = sal_True;
                rDataType = DataType::DECIMAL;
            }
            else if ( ( nNumType & NumberFormat::DATETIME ) == NumberFormat::DATETIME )
            {
                //  NumberFormat::DATETIME is DATE | TIME
                rDataType = DataType::TIMESTAMP;
            }
            else if ( nNumType & NumberFormat::DATE )
                rDataType = DataType::DATE;
            else if ( nNumType & NumberFormat::TIME )
                rDataType = DataType::TIME;
            else if ( nNumType & NumberFormat::LOGICAL )
                rDataType = DataType::BIT;
            else
                rDataType = DataType::DECIMAL;
        }
        else
        {
            //  whole column empty
            rDataType = DataType::VARCHAR;
        }
    }
}

// -------------------------------------------------------------------------

static void lcl_SetValue( ORowSetValue& rValue, const Reference<XSpreadsheet>& xSheet,
                    sal_Int32 nStartCol, sal_Int32 nStartRow, sal_Bool bHasHeaders,
                    const ::Date& rNullDate,
                    sal_Int32 nDBRow, sal_Int32 nDBColumn, sal_Int32 nType )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::lcl_SetValue" );
    sal_Int32 nDocColumn = nStartCol + nDBColumn - 1;   // database counts from 1
    sal_Int32 nDocRow = nStartRow + nDBRow - 1;
    if (bHasHeaders)
        ++nDocRow;

    const Reference<XCell> xCell = xSheet->getCellByPosition( nDocColumn, nDocRow );
    if ( xCell.is() )
    {
        CellContentType eCellType = lcl_GetContentOrResultType( xCell );
        switch (nType)
        {
            case DataType::VARCHAR:
                if ( eCellType == CellContentType_EMPTY )
                    rValue.setNull();
                else
                {
                    // #i25840# still let Calc convert numbers to text
                    const Reference<XText> xText( xCell, UNO_QUERY );
                    if ( xText.is() )
                        rValue = xText->getString();
                }
                break;
            case DataType::DECIMAL:
                if ( eCellType == CellContentType_VALUE )
                    rValue = xCell->getValue();         // double
                else
                    rValue.setNull();
                break;
            case DataType::BIT:
                if ( eCellType == CellContentType_VALUE )
                    rValue = (sal_Bool)( xCell->getValue() != 0.0 );
                else
                    rValue.setNull();
                break;
            case DataType::DATE:
                if ( eCellType == CellContentType_VALUE )
                {
                    ::Date aDate( rNullDate );
                    aDate += (long)::rtl::math::approxFloor( xCell->getValue() );
                    ::com::sun::star::util::Date aDateStruct( aDate.GetDay(), aDate.GetMonth(), aDate.GetYear() );
                    rValue = aDateStruct;
                }
                else
                    rValue.setNull();
                break;
            case DataType::TIME:
                if ( eCellType == CellContentType_VALUE )
                {
                    double fCellVal = xCell->getValue();
                    double fTime = fCellVal - rtl::math::approxFloor( fCellVal );
                    long nIntTime = (long)rtl::math::round( fTime * 8640000.0 );
                    if ( nIntTime == 8640000 )
                        nIntTime = 0;                       // 23:59:59.995 and above is 00:00:00.00
                    ::com::sun::star::util::Time aTime;
                    aTime.HundredthSeconds = (sal_uInt16)( nIntTime % 100 );
                    nIntTime /= 100;
                    aTime.Seconds = (sal_uInt16)( nIntTime % 60 );
                    nIntTime /= 60;
                    aTime.Minutes = (sal_uInt16)( nIntTime % 60 );
                    nIntTime /= 60;
                    OSL_ENSURE( nIntTime < 24, "error in time calculation" );
                    aTime.Hours = (sal_uInt16) nIntTime;
                    rValue = aTime;
                }
                else
                    rValue.setNull();
                break;
            case DataType::TIMESTAMP:
                if ( eCellType == CellContentType_VALUE )
                {
                    double fCellVal = xCell->getValue();
                    double fDays = ::rtl::math::approxFloor( fCellVal );
                    double fTime = fCellVal - fDays;
                    long nIntDays = (long)fDays;
                    long nIntTime = (long)::rtl::math::round( fTime * 8640000.0 );
                    if ( nIntTime == 8640000 )
                    {
                        nIntTime = 0;                       // 23:59:59.995 and above is 00:00:00.00
                        ++nIntDays;                         // (next day)
                    }

                    ::com::sun::star::util::DateTime aDateTime;

                    aDateTime.HundredthSeconds = (sal_uInt16)( nIntTime % 100 );
                    nIntTime /= 100;
                    aDateTime.Seconds = (sal_uInt16)( nIntTime % 60 );
                    nIntTime /= 60;
                    aDateTime.Minutes = (sal_uInt16)( nIntTime % 60 );
                    nIntTime /= 60;
                    OSL_ENSURE( nIntTime < 24, "error in time calculation" );
                    aDateTime.Hours = (sal_uInt16) nIntTime;

                    ::Date aDate( rNullDate );
                    aDate += nIntDays;
                    aDateTime.Day = aDate.GetDay();
                    aDateTime.Month = aDate.GetMonth();
                    aDateTime.Year = aDate.GetYear();

                    rValue = aDateTime;
                }
                else
                    rValue.setNull();
                break;
        } // switch (nType)
    }

//  rValue.setTypeKind(nType);
}

// -------------------------------------------------------------------------

static ::rtl::OUString lcl_GetColumnStr( sal_Int32 nColumn )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::lcl_GetColumnStr" );
    if ( nColumn < 26 )
        return ::rtl::OUString::valueOf( (sal_Unicode) ( 'A' + nColumn ) );
    else
    {
        ::rtl::OUStringBuffer aBuffer(2);
        aBuffer.setLength( 2 );
        aBuffer[0] = (sal_Unicode) ( 'A' + ( nColumn / 26 ) - 1 );
        aBuffer[1] = (sal_Unicode) ( 'A' + ( nColumn % 26 ) );
        return aBuffer.makeStringAndClear();
    }
}

void OCalcTable::fillColumns()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::fillColumns" );
    if ( !m_xSheet.is() )
        throw SQLException();

    String aStrFieldName;
    aStrFieldName.AssignAscii("Column");
    ::rtl::OUString aTypeName;
    ::comphelper::UStringMixEqual aCase(m_pConnection->getMetaData()->supportsMixedCaseQuotedIdentifiers());
    const sal_Bool bStoresMixedCaseQuotedIdentifiers = getConnection()->getMetaData()->supportsMixedCaseQuotedIdentifiers();

    for (sal_Int32 i = 0; i < m_nDataCols; i++)
    {
        ::rtl::OUString aColumnName;
        sal_Int32 eType = DataType::OTHER;
        sal_Bool bCurrency = sal_False;

        lcl_GetColumnInfo( m_xSheet, m_xFormats, m_nStartCol + i, m_nStartRow, m_bHasHeaders,
                            aColumnName, eType, bCurrency );

        if ( aColumnName.isEmpty() )
            aColumnName = lcl_GetColumnStr( i );

        sal_Int32 nPrecision = 0;   //! ...
        sal_Int32 nDecimals = 0;    //! ...

        switch ( eType )
        {
            case DataType::VARCHAR:
                {
                    static const ::rtl::OUString s_sType("VARCHAR");
                    aTypeName = s_sType;
                }
                break;
            case DataType::DECIMAL:
                aTypeName = ::rtl::OUString("DECIMAL");
                break;
            case DataType::BIT:
                aTypeName = ::rtl::OUString("BOOL");
                break;
            case DataType::DATE:
                aTypeName = ::rtl::OUString("DATE");
                break;
            case DataType::TIME:
                aTypeName = ::rtl::OUString("TIME");
                break;
            case DataType::TIMESTAMP:
                aTypeName = ::rtl::OUString("TIMESTAMP");
                break;
            default:
                OSL_FAIL("missing type name");
                aTypeName = ::rtl::OUString();
        }

        // check if the column name already exists
        ::rtl::OUString aAlias = aColumnName;
        OSQLColumns::Vector::const_iterator aFind = connectivity::find(m_aColumns->get().begin(),m_aColumns->get().end(),aAlias,aCase);
        sal_Int32 nExprCnt = 0;
        while(aFind != m_aColumns->get().end())
        {
            (aAlias = aColumnName) += ::rtl::OUString::valueOf((sal_Int32)++nExprCnt);
            aFind = connectivity::find(m_aColumns->get().begin(),m_aColumns->get().end(),aAlias,aCase);
        }

        sdbcx::OColumn* pColumn = new sdbcx::OColumn( aAlias, aTypeName, ::rtl::OUString(),::rtl::OUString(),
                                                ColumnValue::NULLABLE, nPrecision, nDecimals,
                                                eType, sal_False, sal_False, bCurrency,
                                                bStoresMixedCaseQuotedIdentifiers,
                                                m_CatalogName, getSchema(), getName());
        Reference< XPropertySet> xCol = pColumn;
        m_aColumns->get().push_back(xCol);
        m_aTypes.push_back(eType);
        m_aPrecisions.push_back(nPrecision);
        m_aScales.push_back(nDecimals);
    }
}

// -------------------------------------------------------------------------
OCalcTable::OCalcTable(sdbcx::OCollection* _pTables,OCalcConnection* _pConnection,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type,
                    const ::rtl::OUString& _Description ,
                    const ::rtl::OUString& _SchemaName,
                    const ::rtl::OUString& _CatalogName
                ) : OCalcTable_BASE(_pTables,_pConnection,_Name,
                                  _Type,
                                  _Description,
                                  _SchemaName,
                                  _CatalogName)
                ,m_pConnection(_pConnection)
                ,m_nStartCol(0)
                ,m_nStartRow(0)
                ,m_nDataCols(0)
                ,m_nDataRows(0)
                ,m_bHasHeaders(sal_False)
                ,m_aNullDate(::Date::EMPTY)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::OCalcTable" );
}
// -----------------------------------------------------------------------------
void OCalcTable::construct()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::construct" );
    //  get sheet object
    Reference< XSpreadsheetDocument> xDoc = m_pConnection->acquireDoc();
    if (xDoc.is())
    {
        Reference<XSpreadsheets> xSheets = xDoc->getSheets();
        if ( xSheets.is() && xSheets->hasByName( m_Name ) )
        {
            m_xSheet.set(xSheets->getByName( m_Name ),UNO_QUERY);
            if ( m_xSheet.is() )
            {
                lcl_GetDataArea( m_xSheet, m_nDataCols, m_nDataRows );
                m_bHasHeaders = sal_True;
                // whole sheet is always assumed to include a header row
            }
        }
        else        // no sheet -> try database range
        {
            Reference<XPropertySet> xDocProp( xDoc, UNO_QUERY );
            if ( xDocProp.is() )
            {
                Reference<XDatabaseRanges> xRanges(xDocProp->getPropertyValue( ::rtl::OUString("DatabaseRanges") ),UNO_QUERY);

                if ( xRanges.is() && xRanges->hasByName( m_Name ) )
                {
                    Reference<XDatabaseRange> xDBRange(xRanges->getByName( m_Name ),UNO_QUERY);
                    Reference<XCellRangeReferrer> xRefer( xDBRange, UNO_QUERY );
                    if ( xRefer.is() )
                    {
                        //  Header flag is always stored with database range
                        //  Get flag from FilterDescriptor

                        sal_Bool bRangeHeader = sal_True;
                        Reference<XPropertySet> xFiltProp( xDBRange->getFilterDescriptor(), UNO_QUERY );
                        if ( xFiltProp.is() )
                            xFiltProp->getPropertyValue(::rtl::OUString("ContainsHeader")) >>= bRangeHeader;

                        Reference<XSheetCellRange> xSheetRange( xRefer->getReferredCells(), UNO_QUERY );
                        Reference<XCellRangeAddressable> xAddr( xSheetRange, UNO_QUERY );
                        if ( xSheetRange.is() && xAddr.is() )
                        {
                            m_xSheet = xSheetRange->getSpreadsheet();
                            CellRangeAddress aRangeAddr = xAddr->getRangeAddress();
                            m_nStartCol = aRangeAddr.StartColumn;
                            m_nStartRow = aRangeAddr.StartRow;
                            m_nDataCols = aRangeAddr.EndColumn - m_nStartCol + 1;
                            //  m_nDataRows is excluding header row
                            m_nDataRows = aRangeAddr.EndRow - m_nStartRow;
                            if ( !bRangeHeader )
                            {
                                //  m_nDataRows counts the whole range
                                m_nDataRows += 1;
                            }

                            m_bHasHeaders = bRangeHeader;
                        }
                    }
                }
            }
        }

        Reference<XNumberFormatsSupplier> xSupp( xDoc, UNO_QUERY );
        if (xSupp.is())
            m_xFormats = xSupp->getNumberFormats();

        Reference<XPropertySet> xProp( xDoc, UNO_QUERY );
        if (xProp.is())
        {
            ::com::sun::star::util::Date aDateStruct;
            if ( xProp->getPropertyValue( ::rtl::OUString("NullDate") ) >>= aDateStruct )
                m_aNullDate = ::Date( aDateStruct.Day, aDateStruct.Month, aDateStruct.Year );
        }
    }

    //! default if no null date available?

    fillColumns();

    refreshColumns();
}
// -------------------------------------------------------------------------
void OCalcTable::refreshColumns()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::refreshColumns" );
    ::osl::MutexGuard aGuard( m_aMutex );

    TStringVector aVector;

    OSQLColumns::Vector::const_iterator aEnd = m_aColumns->get().end();
    for(OSQLColumns::Vector::const_iterator aIter = m_aColumns->get().begin();aIter != aEnd;++aIter)
        aVector.push_back(Reference< XNamed>(*aIter,UNO_QUERY)->getName());

    if(m_pColumns)
        m_pColumns->reFill(aVector);
    else
        m_pColumns  = new OCalcColumns(this,m_aMutex,aVector);
}
// -------------------------------------------------------------------------
void OCalcTable::refreshIndexes()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::refreshIndexes" );
    //  Calc table has no index
}

// -------------------------------------------------------------------------
void SAL_CALL OCalcTable::disposing(void)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::disposing" );
    OFileTable::disposing();
    ::osl::MutexGuard aGuard(m_aMutex);
    m_aColumns = NULL;
    if ( m_pConnection )
        m_pConnection->releaseDoc();
    m_pConnection = NULL;

}
// -------------------------------------------------------------------------
Sequence< Type > SAL_CALL OCalcTable::getTypes(  ) throw(RuntimeException)
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::getTypes" );
    Sequence< Type > aTypes = OTable_TYPEDEF::getTypes();
    ::std::vector<Type> aOwnTypes;
    aOwnTypes.reserve(aTypes.getLength());

    const Type* pBegin = aTypes.getConstArray();
    const Type* pEnd = pBegin + aTypes.getLength();
    for(;pBegin != pEnd;++pBegin)
    {
        if(!(   *pBegin == ::getCppuType((const Reference<XKeysSupplier>*)0) ||
                *pBegin == ::getCppuType((const Reference<XIndexesSupplier>*)0) ||
                *pBegin == ::getCppuType((const Reference<XRename>*)0) ||
                *pBegin == ::getCppuType((const Reference<XAlterTable>*)0) ||
                *pBegin == ::getCppuType((const Reference<XDataDescriptorFactory>*)0)))
            aOwnTypes.push_back(*pBegin);
    }
    aOwnTypes.push_back(::getCppuType( (const Reference< ::com::sun::star::lang::XUnoTunnel > *)0 ));

    const Type* pAttrs = aOwnTypes.empty() ? 0 : &aOwnTypes[0];
    return Sequence< Type >(pAttrs, aOwnTypes.size());
}

// -------------------------------------------------------------------------
Any SAL_CALL OCalcTable::queryInterface( const Type & rType ) throw(RuntimeException)
{
    if( rType == ::getCppuType((const Reference<XKeysSupplier>*)0) ||
        rType == ::getCppuType((const Reference<XIndexesSupplier>*)0) ||
        rType == ::getCppuType((const Reference<XRename>*)0) ||
        rType == ::getCppuType((const Reference<XAlterTable>*)0) ||
        rType == ::getCppuType((const Reference<XDataDescriptorFactory>*)0))
        return Any();

    const Any aRet = ::cppu::queryInterface(rType,static_cast< ::com::sun::star::lang::XUnoTunnel*> (this));
    return aRet.hasValue() ? aRet : OTable_TYPEDEF::queryInterface(rType);
}

//--------------------------------------------------------------------------
Sequence< sal_Int8 > OCalcTable::getUnoTunnelImplementationId()
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::getUnoTunnelImplementationId" );
    static ::cppu::OImplementationId * pId = 0;
    if (! pId)
    {
        ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
        if (! pId)
        {
            static ::cppu::OImplementationId aId;
            pId = &aId;
        }
    }
    return pId->getImplementationId();
}

// com::sun::star::lang::XUnoTunnel
//------------------------------------------------------------------
sal_Int64 OCalcTable::getSomething( const Sequence< sal_Int8 > & rId ) throw (RuntimeException)
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::getSomething" );
    return (rId.getLength() == 16 && 0 == memcmp(getUnoTunnelImplementationId().getConstArray(),  rId.getConstArray(), 16 ) )
                ? reinterpret_cast< sal_Int64 >( this )
                : OCalcTable_BASE::getSomething(rId);
}
//------------------------------------------------------------------
sal_Int32 OCalcTable::getCurrentLastPos() const
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::getCurrentLastPos" );
    return m_nDataRows;
}
//------------------------------------------------------------------
sal_Bool OCalcTable::seekRow(IResultSetHelper::Movement eCursorPosition, sal_Int32 nOffset, sal_Int32& nCurPos)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::seekRow" );
    // ----------------------------------------------------------
    // prepare positioning:

    sal_uInt32 nNumberOfRecords = m_nDataRows;
    sal_uInt32 nTempPos = m_nFilePos;
    m_nFilePos = nCurPos;

    switch(eCursorPosition)
    {
        case IResultSetHelper::NEXT:
            m_nFilePos++;
            break;
        case IResultSetHelper::PRIOR:
            if (m_nFilePos > 0)
                m_nFilePos--;
            break;
        case IResultSetHelper::FIRST:
            m_nFilePos = 1;
            break;
        case IResultSetHelper::LAST:
            m_nFilePos = nNumberOfRecords;
            break;
        case IResultSetHelper::RELATIVE:
            m_nFilePos = (((sal_Int32)m_nFilePos) + nOffset < 0) ? 0L
                            : (sal_uInt32)(((sal_Int32)m_nFilePos) + nOffset);
            break;
        case IResultSetHelper::ABSOLUTE:
        case IResultSetHelper::BOOKMARK:
            m_nFilePos = (sal_uInt32)nOffset;
            break;
    }

    if (m_nFilePos > (sal_Int32)nNumberOfRecords)
        m_nFilePos = (sal_Int32)nNumberOfRecords + 1;

    if (m_nFilePos == 0 || m_nFilePos == (sal_Int32)nNumberOfRecords + 1)
        goto Error;
    else
    {
        //! read buffer / setup row object etc?
    }
    goto End;

Error:
    switch(eCursorPosition)
    {
        case IResultSetHelper::PRIOR:
        case IResultSetHelper::FIRST:
            m_nFilePos = 0;
            break;
        case IResultSetHelper::LAST:
        case IResultSetHelper::NEXT:
        case IResultSetHelper::ABSOLUTE:
        case IResultSetHelper::RELATIVE:
            if (nOffset > 0)
                m_nFilePos = nNumberOfRecords + 1;
            else if (nOffset < 0)
                m_nFilePos = 0;
            break;
        case IResultSetHelper::BOOKMARK:
            m_nFilePos = nTempPos;   // previous position
    }
    //  aStatus.Set(SDB_STAT_NO_DATA_FOUND);
    return sal_False;

End:
    nCurPos = m_nFilePos;
    return sal_True;
}
//------------------------------------------------------------------
sal_Bool OCalcTable::fetchRow( OValueRefRow& _rRow, const OSQLColumns & _rCols,
                                sal_Bool _bUseTableDefs, sal_Bool bRetrieveData )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::fetchRow" );
    // read the bookmark

    sal_Bool bIsCurRecordDeleted = sal_False;
    _rRow->setDeleted(bIsCurRecordDeleted);
    *(_rRow->get())[0] = m_nFilePos;

    if (!bRetrieveData)
        return sal_True;

    // fields

    OSQLColumns::Vector::const_iterator aIter = _rCols.get().begin();
    OSQLColumns::Vector::const_iterator aEnd = _rCols.get().end();
    const OValueRefVector::Vector::size_type nCount = _rRow->get().size();
    for (OValueRefVector::Vector::size_type i = 1; aIter != aEnd && i < nCount;
         ++aIter, i++)
    {
        if ( (_rRow->get())[i]->isBound() )
        {
            sal_Int32 nType = 0;
            if ( _bUseTableDefs )
                nType = m_aTypes[i-1];
            else
                (*aIter)->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE)) >>= nType;


            lcl_SetValue( (_rRow->get())[i]->get(), m_xSheet, m_nStartCol, m_nStartRow, m_bHasHeaders,
                                m_aNullDate, m_nFilePos, i, nType );
        }
    }
    return sal_True;
}
// -------------------------------------------------------------------------
void OCalcTable::FileClose()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "calc", "Ocke.Janssen@sun.com", "OCalcTable::FileClose" );
    ::osl::MutexGuard aGuard(m_aMutex);

    OCalcTable_BASE::FileClose();
}
// -------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
