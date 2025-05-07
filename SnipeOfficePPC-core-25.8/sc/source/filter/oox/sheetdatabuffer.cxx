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

#include "sheetdatabuffer.hxx"

#include <algorithm>
#include <com/sun/star/sheet/XArrayFormulaTokens.hpp>
#include <com/sun/star/sheet/XCellRangeData.hpp>
#include <com/sun/star/sheet/XFormulaTokens.hpp>
#include <com/sun/star/sheet/XMultipleOperation.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/util/NumberFormat.hpp>
#include <com/sun/star/util/XMergeable.hpp>
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <rtl/ustrbuf.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/editobj.hxx>
#include <svl/eitem.hxx>
#include "oox/helper/containerhelper.hxx"
#include "oox/helper/propertymap.hxx"
#include "oox/helper/propertyset.hxx"
#include "oox/token/tokens.hxx"
#include "addressconverter.hxx"
#include "biffinputstream.hxx"
#include "formulaparser.hxx"
#include "sharedstringsbuffer.hxx"
#include "unitconverter.hxx"
#include "convuno.hxx"
#include "markdata.hxx"
#include "rangelst.hxx"
#include "document.hxx"
#include "scitems.hxx"
#include "cell.hxx"

namespace oox {
namespace xls {

// ============================================================================

using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::sheet;
using namespace ::com::sun::star::table;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;

// ============================================================================

CellModel::CellModel() :
    mnCellType( XML_TOKEN_INVALID ),
    mnXfId( -1 ),
    mbShowPhonetic( false )
{
}

// ----------------------------------------------------------------------------

CellFormulaModel::CellFormulaModel() :
    mnFormulaType( XML_TOKEN_INVALID ),
    mnSharedId( -1 )
{
}

bool CellFormulaModel::isValidArrayRef( const CellAddress& rCellAddr )
{
    return
        (maFormulaRef.Sheet == rCellAddr.Sheet) &&
        (maFormulaRef.StartColumn == rCellAddr.Column) &&
        (maFormulaRef.StartRow == rCellAddr.Row);
}

bool CellFormulaModel::isValidSharedRef( const CellAddress& rCellAddr )
{
    return
        (maFormulaRef.Sheet == rCellAddr.Sheet) &&
        (maFormulaRef.StartColumn <= rCellAddr.Column) && (rCellAddr.Column <= maFormulaRef.EndColumn) &&
        (maFormulaRef.StartRow <= rCellAddr.Row) && (rCellAddr.Row <= maFormulaRef.EndRow);
}

// ----------------------------------------------------------------------------

DataTableModel::DataTableModel() :
    mb2dTable( false ),
    mbRowTable( false ),
    mbRef1Deleted( false ),
    mbRef2Deleted( false )
{
}

// ============================================================================

namespace {

const sal_Int32 CELLBLOCK_MAXROWS  = 16;    /// Number of rows in a cell block.

} // namespace

CellBlock::CellBlock( const WorksheetHelper& rHelper, const ValueRange& rColSpan, sal_Int32 nRow ) :
    WorksheetHelper( rHelper ),
    maRange( rHelper.getSheetIndex(), rColSpan.mnFirst, nRow, rColSpan.mnLast, nRow ),
    mnRowLength( rColSpan.mnLast - rColSpan.mnFirst + 1 ),
    mnFirstFreeIndex( 0 )
{
    maCellArray.realloc( 1 );
    maCellArray[ 0 ].realloc( mnRowLength );
    mpCurrCellRow = maCellArray[ 0 ].getArray();
}

void CellBlock::finalizeImport()
{
    // fill last cells in last row with empty strings (placeholder for empty cells)
    fillUnusedCells( mnRowLength );
    // insert all buffered cells into the Calc sheet
    try
    {
        Reference< XCellRangeData > xRangeData( getCellRange( maRange ), UNO_QUERY_THROW );
        xRangeData->setDataArray( maCellArray );
    }
    catch( Exception& )
    {
    }
    // insert uncacheable cells separately
    for( RichStringCellList::const_iterator aIt = maRichStrings.begin(), aEnd = maRichStrings.end(); aIt != aEnd; ++aIt )
        putRichString( aIt->maCellAddr, *aIt->mxString, aIt->mpFirstPortionFont );
}

// private --------------------------------------------------------------------

CellBlock::RichStringCell::RichStringCell( const CellAddress& rCellAddr, const RichStringRef& rxString, const Font* pFirstPortionFont ) :
    maCellAddr( rCellAddr ),
    mxString( rxString ),
    mpFirstPortionFont( pFirstPortionFont )
{
}

void CellBlock::fillUnusedCells( sal_Int32 nIndex )
{
    if( mnFirstFreeIndex < nIndex )
    {
        Any* pCellEnd = mpCurrCellRow + nIndex;
        for( Any* pCell = mpCurrCellRow + mnFirstFreeIndex; pCell < pCellEnd; ++pCell )
            *pCell <<= OUString();
    }
}

// ============================================================================

CellBlockBuffer::CellBlockBuffer( const WorksheetHelper& rHelper ) :
    WorksheetHelper( rHelper ),
    mnCurrRow( -1 )
{
    maCellBlockIt = maCellBlocks.end();
}

void CellBlockBuffer::setColSpans( sal_Int32 nRow, const ValueRangeSet& rColSpans )
{
    OSL_ENSURE( maColSpans.count( nRow ) == 0, "CellBlockBuffer::setColSpans - multiple column spans for the same row" );
    OSL_ENSURE( (mnCurrRow < nRow) && (maColSpans.empty() || (maColSpans.rbegin()->first < nRow)), "CellBlockBuffer::setColSpans - rows are unsorted" );
    if( (mnCurrRow < nRow) && (maColSpans.count( nRow ) == 0) )
        maColSpans[ nRow ] = rColSpans.getRanges();
}

void CellBlockBuffer::finalizeImport()
{
    maCellBlocks.forEachMem( &CellBlock::finalizeImport );
}

// ============================================================================

SheetDataBuffer::SheetDataBuffer( const WorksheetHelper& rHelper ) :
    WorksheetHelper( rHelper ),
    maCellBlocks( rHelper ),
    mbPendingSharedFmla( false )
{
}

void SheetDataBuffer::setColSpans( sal_Int32 nRow, const ValueRangeSet& rColSpans )
{
    maCellBlocks.setColSpans( nRow, rColSpans );
}

void SheetDataBuffer::setBlankCell( const CellModel& rModel )
{
    setCellFormat( rModel );
}

void SheetDataBuffer::setValueCell( const CellModel& rModel, double fValue )
{
    putValue( rModel.maCellAddr, fValue );
    setCellFormat( rModel );
}

void SheetDataBuffer::setStringCell( const CellModel& rModel, const OUString& rText )
{
    putString( rModel.maCellAddr, rText );
    setCellFormat( rModel );
}

void SheetDataBuffer::setStringCell( const CellModel& rModel, const RichStringRef& rxString )
{
    OSL_ENSURE( rxString.get(), "SheetDataBuffer::setStringCell - missing rich string object" );
    const Font* pFirstPortionFont = getStyles().getFontFromCellXf( rModel.mnXfId ).get();
    OUString aText;
    if( rxString->extractPlainString( aText, pFirstPortionFont ) )
    {
        setStringCell( rModel, aText );
    }
    else
    {
        putRichString( rModel.maCellAddr, *rxString, pFirstPortionFont );
        setCellFormat( rModel );
    }
}

void SheetDataBuffer::setStringCell( const CellModel& rModel, sal_Int32 nStringId )
{
    RichStringRef xString = getSharedStrings().getString( nStringId );
    if( xString.get() )
        setStringCell( rModel, xString );
    else
        setBlankCell( rModel );
}

void SheetDataBuffer::setDateTimeCell( const CellModel& rModel, const ::com::sun::star::util::DateTime& rDateTime )
{
    // write serial date/time value into the cell
    double fSerial = getUnitConverter().calcSerialFromDateTime( rDateTime );
    setValueCell( rModel, fSerial );
    // set appropriate number format
    using namespace ::com::sun::star::util::NumberFormat;
    sal_Int16 nStdFmt = (fSerial < 1.0) ? TIME : (((rDateTime.Hours > 0) || (rDateTime.Minutes > 0) || (rDateTime.Seconds > 0)) ? DATETIME : DATE);
    setStandardNumFmt( rModel.maCellAddr, nStdFmt );
}

void SheetDataBuffer::setBooleanCell( const CellModel& rModel, bool bValue )
{
    setCellFormula( rModel.maCellAddr, getFormulaParser().convertBoolToFormula( bValue ) );
    // #108770# set 'Standard' number format for all Boolean cells
    setCellFormat( rModel, 0 );
}

void SheetDataBuffer::setErrorCell( const CellModel& rModel, const OUString& rErrorCode )
{
    setErrorCell( rModel, getUnitConverter().calcBiffErrorCode( rErrorCode ) );
}

void SheetDataBuffer::setErrorCell( const CellModel& rModel, sal_uInt8 nErrorCode )
{
    setCellFormula( rModel.maCellAddr, getFormulaParser().convertErrorToFormula( nErrorCode ) );
    setCellFormat( rModel );
}

void SheetDataBuffer::setFormulaCell( const CellModel& rModel, const ApiTokenSequence& rTokens )
{
    mbPendingSharedFmla = false;
    ApiTokenSequence aTokens;

    /*  Detect special token passed as placeholder for array formulas, shared
        formulas, and table operations. In BIFF, these formulas are represented
        by a single tExp resp. tTbl token. If the formula parser finds these
        tokens, it puts a single OPCODE_BAD token with the base address and
        formula type into the token sequence. This information will be
        extracted here, and in case of a shared formula, the shared formula
        buffer will generate the resulting formula token array. */
    ApiSpecialTokenInfo aTokenInfo;
    if( rTokens.hasElements() && getFormulaParser().extractSpecialTokenInfo( aTokenInfo, rTokens ) )
    {
        /*  The second member of the token info is set to true, if the formula
            represents a table operation, which will be skipped. In BIFF12 it
            is not possible to distinguish array and shared formulas
            (BIFF5/BIFF8 provide this information with a special flag in the
            FORMULA record). */
        if( !aTokenInfo.Second )
        {
            /*  Construct the token array representing the shared formula. If
                the returned sequence is empty, the definition of the shared
                formula has not been loaded yet, or the cell is part of an
                array formula. In this case, the cell will be remembered. After
                reading the formula definition it will be retried to insert the
                formula via retryPendingSharedFormulaCell(). */
            BinAddress aBaseAddr( aTokenInfo.First );
            aTokens = resolveSharedFormula( aBaseAddr );
            if( !aTokens.hasElements() )
            {
                maSharedFmlaAddr = rModel.maCellAddr;
                maSharedBaseAddr = aBaseAddr;
                mbPendingSharedFmla = true;
            }
        }
    }
    else
    {
        // simple formula, use the passed token array
        aTokens = rTokens;
    }

    setCellFormula( rModel.maCellAddr, aTokens );
    setCellFormat( rModel );
}

void SheetDataBuffer::setFormulaCell( const CellModel& rModel, sal_Int32 nSharedId )
{
    setCellFormula( rModel.maCellAddr, resolveSharedFormula( BinAddress( nSharedId, 0 ) ) );
    setCellFormat( rModel );
}

void SheetDataBuffer::createArrayFormula( const CellRangeAddress& rRange, const ApiTokenSequence& rTokens )
{
    /*  Array formulas will be inserted later in finalizeImport(). This is
        needed to not disturb collecting all the cells, which will be put into
        the sheet in large blocks to increase performance. */
    maArrayFormulas.push_back( ArrayFormula( rRange, rTokens ) );
}

void SheetDataBuffer::createTableOperation( const CellRangeAddress& rRange, const DataTableModel& rModel )
{
    /*  Table operations will be inserted later in finalizeImport(). This is
        needed to not disturb collecting all the cells, which will be put into
        the sheet in large blocks to increase performance. */
    maTableOperations.push_back( TableOperation( rRange, rModel ) );
}

void SheetDataBuffer::createSharedFormula( sal_Int32 nSharedId, const ApiTokenSequence& rTokens )
{
    createSharedFormula( BinAddress( nSharedId, 0 ), rTokens );
}

void SheetDataBuffer::createSharedFormula( const CellAddress& rCellAddr, const ApiTokenSequence& rTokens )
{
    createSharedFormula( BinAddress( rCellAddr ), rTokens );
}

void SheetDataBuffer::setRowFormat( sal_Int32 nRow, sal_Int32 nXfId, bool bCustomFormat )
{
    // set row formatting
    if( bCustomFormat )
    {
        // try to expand cached row range, if formatting is equal
        if( (maXfIdRowRange.maRowRange.mnLast < 0) || !maXfIdRowRange.tryExpand( nRow, nXfId ) )
        {

            maXfIdRowRangeList[ maXfIdRowRange.mnXfId ].push_back( maXfIdRowRange.maRowRange );
            maXfIdRowRange.set( nRow, nXfId );
        }
    }
    else if( maXfIdRowRange.maRowRange.mnLast >= 0 )
    {
        // finish last cached row range
        maXfIdRowRangeList[ maXfIdRowRange.mnXfId ].push_back( maXfIdRowRange.maRowRange );
        maXfIdRowRange.set( -1, -1 );
    }
}

void SheetDataBuffer::setMergedRange( const CellRangeAddress& rRange )
{
    maMergedRanges.push_back( MergedRange( rRange ) );
}

void SheetDataBuffer::setStandardNumFmt( const CellAddress& rCellAddr, sal_Int16 nStdNumFmt )
{
    try
    {
        Reference< XNumberFormatsSupplier > xNumFmtsSupp( getDocument(), UNO_QUERY_THROW );
        Reference< XNumberFormatTypes > xNumFmtTypes( xNumFmtsSupp->getNumberFormats(), UNO_QUERY_THROW );
        sal_Int32 nIndex = xNumFmtTypes->getStandardFormat( nStdNumFmt, Locale() );
        PropertySet aPropSet( getCell( rCellAddr ) );
        aPropSet.setProperty( PROP_NumberFormat, nIndex );
    }
    catch( Exception& )
    {
    }
}

void addIfNotInMyMap( StylesBuffer& rStyles, std::map< std::pair< sal_Int32, sal_Int32 >, ApiCellRangeList >& rMap, sal_Int32 nXfId, sal_Int32 nFormatId, const ApiCellRangeList& rRangeList )
{
    Xf* pXf1 = rStyles.getCellXf( nXfId ).get();
    if ( pXf1 )
    {
        for ( std::map< std::pair< sal_Int32, sal_Int32 >, ApiCellRangeList >::iterator it = rMap.begin(), it_end = rMap.end(); it != it_end; ++it )
        {
            if ( it->first.second == nFormatId )
            {
                Xf* pXf2 = rStyles.getCellXf( it->first.first ).get();
                if ( *pXf1 == *pXf2 ) // already exists
                {
                    // add ranges from the rangelist to the existing rangelist for the
                    // matching style ( should we check if they overlap ? )
                    for ( ApiCellRangeList::const_iterator iter = rRangeList.begin(), iter_end =  rRangeList.end(); iter != iter_end; ++iter )
                       it->second.push_back( *iter );
                    return;
                }
            }
        }
        rMap[ std::pair<sal_Int32, sal_Int32>( nXfId, nFormatId ) ] = rRangeList;
    }
}

void SheetDataBuffer::finalizeImport()
{
    // insert all cells of all open cell blocks
    maCellBlocks.finalizeImport();

    // create all array formulas
    for( ArrayFormulaList::iterator aIt = maArrayFormulas.begin(), aEnd = maArrayFormulas.end(); aIt != aEnd; ++aIt )
        finalizeArrayFormula( aIt->first, aIt->second );

    // create all table operations
    for( TableOperationList::iterator aIt = maTableOperations.begin(), aEnd = maTableOperations.end(); aIt != aEnd; ++aIt )
        finalizeTableOperation( aIt->first, aIt->second );

    // write default formatting of remaining row range
    maXfIdRowRangeList[ maXfIdRowRange.mnXfId ].push_back( maXfIdRowRange.maRowRange );
    for ( std::map< sal_Int32, std::vector< ValueRange > >::iterator it = maXfIdRowRangeList.begin(), it_end =  maXfIdRowRangeList.end(); it != it_end; ++it )
    {
        ApiCellRangeList rangeList;
        AddressConverter& rAddrConv = getAddressConverter();
        // get all row ranges for id
        for ( std::vector< ValueRange >::iterator rangeIter = it->second.begin(), rangeIter_end = it->second.end(); rangeIter != rangeIter_end; ++rangeIter )
        {
            CellRangeAddress aRange( getSheetIndex(), 0, rangeIter->mnFirst, rAddrConv.getMaxApiAddress().Column, rangeIter->mnLast );
            rangeList.push_back( aRange );
        }
        ScRangeList aList;
        for ( ApiCellRangeList::const_iterator itRange = rangeList.begin(), itRange_end = rangeList.end(); itRange!=itRange_end; ++itRange )
        {
            ScRange* pRange = new ScRange();
            ScUnoConversion::FillScRange( *pRange, *itRange );
            aList.push_back( pRange );
        }
        ScMarkData aMark;
        aMark.MarkFromRangeList( aList, false );

        getStyles().writeCellXfToMarkData( aMark, it->first, -1 );
    }
    std::map< std::pair< sal_Int32, sal_Int32 >, ApiCellRangeList > rangeStyleListMap;
    // gather all ranges that have the same style and apply them in bulk
    for( XfIdRangeListMap::const_iterator aIt = maXfIdRangeLists.begin(), aEnd = maXfIdRangeLists.end(); aIt != aEnd; ++aIt )
        addIfNotInMyMap( getStyles(), rangeStyleListMap, aIt->first.first, aIt->first.second, aIt->second );
    for (  std::map< std::pair< sal_Int32, sal_Int32 >, ApiCellRangeList >::iterator it = rangeStyleListMap.begin(), it_end = rangeStyleListMap.end(); it != it_end; ++it )
        writeXfIdRangeListProperties( it->first.first, it->first.second, it->second );
    // merge all cached merged ranges and update right/bottom cell borders
    for( MergedRangeList::iterator aIt = maMergedRanges.begin(), aEnd = maMergedRanges.end(); aIt != aEnd; ++aIt )
        applyCellMerging( aIt->maRange );
    for( MergedRangeList::iterator aIt = maCenterFillRanges.begin(), aEnd = maCenterFillRanges.end(); aIt != aEnd; ++aIt )
        applyCellMerging( aIt->maRange );
}

// private --------------------------------------------------------------------

SheetDataBuffer::XfIdRowRange::XfIdRowRange() :
    maRowRange( -1 ),
    mnXfId( -1 )
{
}

void SheetDataBuffer::XfIdRowRange::set( sal_Int32 nRow, sal_Int32 nXfId )
{
    maRowRange = ValueRange( nRow );
    mnXfId = nXfId;
}

bool SheetDataBuffer::XfIdRowRange::tryExpand( sal_Int32 nRow, sal_Int32 nXfId )
{
    if( mnXfId == nXfId )
    {
        if( maRowRange.mnLast + 1 == nRow )
        {
            ++maRowRange.mnLast;
            return true;
        }
        if( maRowRange.mnFirst == nRow + 1 )
        {
            --maRowRange.mnFirst;
            return true;
        }
    }
    return false;
}


SheetDataBuffer::MergedRange::MergedRange( const CellRangeAddress& rRange ) :
    maRange( rRange ),
    mnHorAlign( XML_TOKEN_INVALID )
{
}

SheetDataBuffer::MergedRange::MergedRange( const CellAddress& rAddress, sal_Int32 nHorAlign ) :
    maRange( rAddress.Sheet, rAddress.Column, rAddress.Row, rAddress.Column, rAddress.Row ),
    mnHorAlign( nHorAlign )
{
}

bool SheetDataBuffer::MergedRange::tryExpand( const CellAddress& rAddress, sal_Int32 nHorAlign )
{
    if( (mnHorAlign == nHorAlign) && (maRange.StartRow == rAddress.Row) &&
        (maRange.EndRow == rAddress.Row) && (maRange.EndColumn + 1 == rAddress.Column) )
    {
        ++maRange.EndColumn;
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------

void SheetDataBuffer::setCellFormula( const CellAddress& rCellAddr, const ApiTokenSequence& rTokens )
{
    if( rTokens.hasElements() )
    {
        putFormulaTokens( rCellAddr, rTokens );
    }
}

void SheetDataBuffer::createSharedFormula( const BinAddress& rMapKey, const ApiTokenSequence& rTokens )
{
    // create the defined name that will represent the shared formula
    OUString aName = OUStringBuffer().appendAscii( RTL_CONSTASCII_STRINGPARAM( "__shared_" ) ).
        append( static_cast< sal_Int32 >( getSheetIndex() + 1 ) ).
        append( sal_Unicode( '_' ) ).append( rMapKey.mnRow ).
        append( sal_Unicode( '_' ) ).append( rMapKey.mnCol ).makeStringAndClear();
    ScRangeData* pScRangeData = createNamedRangeObject( aName, rTokens, 0 );
    pScRangeData->SetType(RT_SHARED);

    // get and store the token index of the defined name
    OSL_ENSURE( maSharedFormulas.count( rMapKey ) == 0, "SheetDataBuffer::createSharedFormula - shared formula exists already" );
    sal_Int32 nTokenIndex = static_cast< sal_Int32 >( pScRangeData->GetIndex() );
    if( nTokenIndex >= 0 ) try
    {
        // store the token index in the map
        maSharedFormulas[ rMapKey ] = nTokenIndex;
        // retry to insert a pending shared formula cell
        if( mbPendingSharedFmla )
            setCellFormula( maSharedFmlaAddr, resolveSharedFormula( maSharedBaseAddr ) );
    }
    catch( Exception& )
    {
    }
    mbPendingSharedFmla = false;
}

ApiTokenSequence SheetDataBuffer::resolveSharedFormula( const BinAddress& rMapKey ) const
{
    sal_Int32 nTokenIndex = ContainerHelper::getMapElement( maSharedFormulas, rMapKey, -1 );
    return (nTokenIndex >= 0) ? getFormulaParser().convertNameToFormula( nTokenIndex ) : ApiTokenSequence();
}

void SheetDataBuffer::finalizeArrayFormula( const CellRangeAddress& rRange, const ApiTokenSequence& rTokens ) const
{
    Reference< XArrayFormulaTokens > xTokens( getCellRange( rRange ), UNO_QUERY );
    OSL_ENSURE( xTokens.is(), "SheetDataBuffer::finalizeArrayFormula - missing formula token interface" );
    if( xTokens.is() )
        xTokens->setArrayTokens( rTokens );
}

void SheetDataBuffer::finalizeTableOperation( const CellRangeAddress& rRange, const DataTableModel& rModel ) const
{
    sal_Int16 nSheet = getSheetIndex();
    bool bOk = false;
    if( !rModel.mbRef1Deleted && !rModel.maRef1.isEmpty() && (rRange.StartColumn > 0) && (rRange.StartRow > 0) )
    {
        CellRangeAddress aOpRange = rRange;
        CellAddress aRef1;
        if( getAddressConverter().convertToCellAddress( aRef1, rModel.maRef1, nSheet, true ) ) try
        {
            if( rModel.mb2dTable )
            {
                CellAddress aRef2;
                if( !rModel.mbRef2Deleted && getAddressConverter().convertToCellAddress( aRef2, rModel.maRef2, nSheet, true ) )
                {
                    // API call expects input values inside operation range
                    --aOpRange.StartColumn;
                    --aOpRange.StartRow;
                    // formula range is top-left cell of operation range
                    CellRangeAddress aFormulaRange( nSheet, aOpRange.StartColumn, aOpRange.StartRow, aOpRange.StartColumn, aOpRange.StartRow );
                    // set multiple operation
                    Reference< XMultipleOperation > xMultOp( getCellRange( aOpRange ), UNO_QUERY_THROW );
                    xMultOp->setTableOperation( aFormulaRange, TableOperationMode_BOTH, aRef2, aRef1 );
                    bOk = true;
                }
            }
            else if( rModel.mbRowTable )
            {
                // formula range is column to the left of operation range
                CellRangeAddress aFormulaRange( nSheet, aOpRange.StartColumn - 1, aOpRange.StartRow, aOpRange.StartColumn - 1, aOpRange.EndRow );
                // API call expects input values (top row) inside operation range
                --aOpRange.StartRow;
                // set multiple operation
                Reference< XMultipleOperation > xMultOp( getCellRange( aOpRange ), UNO_QUERY_THROW );
                xMultOp->setTableOperation( aFormulaRange, TableOperationMode_ROW, aRef1, aRef1 );
                bOk = true;
            }
            else
            {
                // formula range is row above operation range
                CellRangeAddress aFormulaRange( nSheet, aOpRange.StartColumn, aOpRange.StartRow - 1, aOpRange.EndColumn, aOpRange.StartRow - 1 );
                // API call expects input values (left column) inside operation range
                --aOpRange.StartColumn;
                // set multiple operation
                Reference< XMultipleOperation > xMultOp( getCellRange( aOpRange ), UNO_QUERY_THROW );
                xMultOp->setTableOperation( aFormulaRange, TableOperationMode_COLUMN, aRef1, aRef1 );
                bOk = true;
            }
        }
        catch( Exception& )
        {
        }
    }

    // on error: fill cell range with #REF! error codes
    if( !bOk ) try
    {
        Reference< XCellRangeData > xCellRangeData( getCellRange( rRange ), UNO_QUERY_THROW );
        size_t nWidth = static_cast< size_t >( rRange.EndColumn - rRange.StartColumn + 1 );
        size_t nHeight = static_cast< size_t >( rRange.EndRow - rRange.StartRow + 1 );
        Matrix< Any > aErrorCells( nWidth, nHeight, Any( getFormulaParser().convertErrorToFormula( BIFF_ERR_REF ) ) );
        xCellRangeData->setDataArray( ContainerHelper::matrixToSequenceSequence( aErrorCells ) );
    }
    catch( Exception& )
    {
    }
}

void SheetDataBuffer::setCellFormat( const CellModel& rModel, sal_Int32 nNumFmtId )
{
    if( (rModel.mnXfId >= 0) || (nNumFmtId >= 0) )
    {
        ApiCellRangeList::reverse_iterator aIt = maXfIdRangeLists[ XfIdNumFmtKey( rModel.mnXfId, nNumFmtId ) ].rbegin();
        ApiCellRangeList::reverse_iterator aItEnd = maXfIdRangeLists[ XfIdNumFmtKey( rModel.mnXfId, nNumFmtId ) ].rend();
        /* The xlsx sheet data contains row wise information.
         * It is sufficient to check if the row range size is one
         */
        if(     aIt                 != aItEnd &&
                aIt->Sheet          == rModel.maCellAddr.Sheet &&
                aIt->StartRow       == aIt->EndRow &&
                aIt->StartRow       == rModel.maCellAddr.Row &&
                (aIt->EndColumn+1)  == rModel.maCellAddr.Column )
        {
            aIt->EndColumn++;       // Expand Column
        }
        else
        {
            maXfIdRangeLists[ XfIdNumFmtKey (rModel.mnXfId, nNumFmtId ) ].push_back(
                              CellRangeAddress( rModel.maCellAddr.Sheet, rModel.maCellAddr.Column, rModel.maCellAddr.Row,
                              rModel.maCellAddr.Column, rModel.maCellAddr.Row ) );
        }

        aIt = maXfIdRangeLists[ XfIdNumFmtKey( rModel.mnXfId, nNumFmtId ) ].rbegin();
        aItEnd = maXfIdRangeLists[ XfIdNumFmtKey( rModel.mnXfId, nNumFmtId ) ].rend();
        ApiCellRangeList::reverse_iterator aItM = aIt+1;
        while( aItM != aItEnd )
        {
            if( aIt->Sheet == aItM->Sheet )
            {
                /* Try to merge this with the previous range */
                if( aIt->StartRow == (aItM->EndRow + 1) &&
                        aIt->StartColumn == aItM->StartColumn &&
                        aIt->EndColumn == aItM->EndColumn)
                {
                    aItM->EndRow = aIt->EndRow;
                    maXfIdRangeLists[ XfIdNumFmtKey( rModel.mnXfId, nNumFmtId ) ].pop_back();
                    break;
                }
                else if( aIt->StartRow > aItM->EndRow + 1 )
                    break; // Un-necessary to check with any other rows
            }
            else
                break;
            ++aItM;
        }

        // update merged ranges for 'center across selection' and 'fill'
        if( const Xf* pXf = getStyles().getCellXf( rModel.mnXfId ).get() )
        {
            sal_Int32 nHorAlign = pXf->getAlignment().getModel().mnHorAlign;
            if( (nHorAlign == XML_centerContinuous) || (nHorAlign == XML_fill) )
            {
                /*  start new merged range, if cell is not empty (#108781#),
                    or try to expand last range with empty cell */
                if( rModel.mnCellType != XML_TOKEN_INVALID )
                    maCenterFillRanges.push_back( MergedRange( rModel.maCellAddr, nHorAlign ) );
                else if( !maCenterFillRanges.empty() )
                    maCenterFillRanges.rbegin()->tryExpand( rModel.maCellAddr, nHorAlign );
            }
        }
    }
}

void SheetDataBuffer::writeXfIdRangeListProperties( sal_Int32 nXfId, sal_Int32 nNumFmtId, const ApiCellRangeList& rRanges ) const
{
    StylesBuffer& rStyles = getStyles();
    ScRangeList aList;
    for ( ApiCellRangeList::const_iterator it = rRanges.begin(), it_end = rRanges.end(); it!=it_end; ++it )
    {
        ScRange* pRange = new ScRange();
        ScUnoConversion::FillScRange( *pRange, *it );
        aList.push_back( pRange );
    }
    ScMarkData aMark;
    aMark.MarkFromRangeList( aList, false );
    rStyles.writeCellXfToMarkData( aMark, nXfId, nNumFmtId );
}

void lcl_SetBorderLine( ScDocument& rDoc, ScRange& rRange, SCTAB nScTab, sal_uInt16 nLine )
{
    SCCOL nFromScCol = (nLine == BOX_LINE_RIGHT) ? rRange.aEnd.Col() : rRange.aStart.Col();
    SCROW nFromScRow = (nLine == BOX_LINE_BOTTOM) ? rRange.aEnd.Row() : rRange.aStart.Row();

    const SvxBoxItem* pFromItem = static_cast< const SvxBoxItem* >(
        rDoc.GetAttr( nFromScCol, nFromScRow, nScTab, ATTR_BORDER ) );
    const SvxBoxItem* pToItem = static_cast< const SvxBoxItem* >(
        rDoc.GetAttr( rRange.aStart.Col(), rRange.aStart.Row(), nScTab, ATTR_BORDER ) );

    SvxBoxItem aNewItem( *pToItem );
    aNewItem.SetLine( pFromItem->GetLine( nLine ), nLine );
    rDoc.ApplyAttr( rRange.aStart.Col(), rRange.aStart.Row(), nScTab, aNewItem );
}

void SheetDataBuffer::applyCellMerging( const CellRangeAddress& rRange )
{
    bool bMultiCol = rRange.StartColumn < rRange.EndColumn;
    bool bMultiRow = rRange.StartRow < rRange.EndRow;

    ScRange aRange;
    ScUnoConversion::FillScRange( aRange, rRange );
    const ScAddress& rStart = aRange.aStart;
    const ScAddress& rEnd = aRange.aEnd;
    ScDocument& rDoc = getScDocument();
    // set correct right border
    if( bMultiCol )
        lcl_SetBorderLine( rDoc, aRange, getSheetIndex(), BOX_LINE_RIGHT );
        // set correct lower border
    if( bMultiRow )
        lcl_SetBorderLine( rDoc, aRange, getSheetIndex(), BOX_LINE_BOTTOM );
    // do merge
    if( bMultiCol || bMultiRow )
        rDoc.DoMerge( getSheetIndex(), rStart.Col(), rStart.Row(), rEnd.Col(), rEnd.Row() );
    // #i93609# merged range in a single row: test if manual row height is needed
    if( !bMultiRow )
    {
        bool bTextWrap = static_cast< const SfxBoolItem* >( rDoc.GetAttr( rStart.Col(), rStart.Row(), rStart.Tab(), ATTR_LINEBREAK ) )->GetValue();
        if( !bTextWrap && (rDoc.GetCellType( rStart ) == CELLTYPE_EDIT) )
        {
            if( const EditTextObject* pEditObj = static_cast< const ScEditCell* >( rDoc.GetCell( rStart ) )->GetData() )
                bTextWrap = pEditObj->GetParagraphCount() > 1;
        }
    }
}

} // namespace xls
} // namespace oox

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
