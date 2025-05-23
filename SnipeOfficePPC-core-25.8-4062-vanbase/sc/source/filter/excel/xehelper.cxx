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

#include <com/sun/star/i18n/XBreakIterator.hpp>
#include <com/sun/star/i18n/ScriptType.hpp>
#include <sfx2/objsh.hxx>
#include <vcl/font.hxx>
#include <tools/urlobj.hxx>
#include <svl/itemset.hxx>
#include <svtools/ctrltool.hxx>
#include <svx/svdotext.hxx>
#include <editeng/outlobj.hxx>
#include "scitems.hxx"
#include <editeng/fhgtitem.hxx>
#include <editeng/flstitem.hxx>
#include <editeng/colritem.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/flditem.hxx>
#include <editeng/escpitem.hxx>
#include <editeng/svxfont.hxx>

#include "document.hxx"
#include "docpool.hxx"
#include "cell.hxx"
#include "editutil.hxx"
#include "patattr.hxx"
#include "xestyle.hxx"
#include "fprogressbar.hxx"
#include "xltracer.hxx"
#include "xecontent.hxx"
#include "xelink.hxx"
#include "xehelper.hxx"

using ::rtl::OUString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::i18n::XBreakIterator;

// Export progress bar ========================================================

XclExpProgressBar::XclExpProgressBar( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot ),
    mxProgress( new ScfProgressBar( rRoot.GetDocShell(), STR_SAVE_DOC ) ),
    mpSubProgress( 0 ),
    mpSubRowCreate( 0 ),
    mpSubRowFinal( 0 ),
    mnSegRowFinal( SCF_INV_SEGMENT ),
    mnRowCount( 0 )
{
}

XclExpProgressBar::~XclExpProgressBar()
{
}

void XclExpProgressBar::Initialize()
{
    const ScDocument& rDoc = GetDoc();
    const XclExpTabInfo& rTabInfo = GetTabInfo();
    SCTAB nScTabCount = rTabInfo.GetScTabCount();

    // *** segment: creation of ROW records *** -------------------------------

    sal_Int32 nSegRowCreate = mxProgress->AddSegment( 2000 );
    mpSubRowCreate = &mxProgress->GetSegmentProgressBar( nSegRowCreate );
    maSubSegRowCreate.resize( nScTabCount, SCF_INV_SEGMENT );

    for( SCTAB nScTab = 0; nScTab < nScTabCount; ++nScTab )
    {
        if( rTabInfo.IsExportTab( nScTab ) )
        {
            SCCOL nLastUsedScCol;
            SCROW nLastUsedScRow;
            rDoc.GetTableArea( nScTab, nLastUsedScCol, nLastUsedScRow );
            sal_Size nSegSize = static_cast< sal_Size >( nLastUsedScRow + 1 );
            maSubSegRowCreate[ nScTab ] = mpSubRowCreate->AddSegment( nSegSize );
        }
    }

    // *** segment: writing all ROW records *** -------------------------------

    mnSegRowFinal = mxProgress->AddSegment( 1000 );
    // sub progress bar and segment are created later in ActivateFinalRowsSegment()
}

void XclExpProgressBar::IncRowRecordCount()
{
    ++mnRowCount;
}

void XclExpProgressBar::ActivateCreateRowsSegment()
{
    OSL_ENSURE( (0 <= GetCurrScTab()) && (GetCurrScTab() < GetTabInfo().GetScTabCount()),
        "XclExpProgressBar::ActivateCreateRowsSegment - invalid sheet" );
    sal_Int32 nSeg = maSubSegRowCreate[ GetCurrScTab() ];
    OSL_ENSURE( nSeg != SCF_INV_SEGMENT, "XclExpProgressBar::ActivateCreateRowsSegment - invalid segment" );
    if( nSeg != SCF_INV_SEGMENT )
    {
        mpSubProgress = mpSubRowCreate;
        mpSubProgress->ActivateSegment( nSeg );
    }
    else
        mpSubProgress = 0;
}

void XclExpProgressBar::ActivateFinalRowsSegment()
{
    if( !mpSubRowFinal && (mnRowCount > 0) )
    {
        mpSubRowFinal = &mxProgress->GetSegmentProgressBar( mnSegRowFinal );
        mpSubRowFinal->AddSegment( mnRowCount );
    }
    mpSubProgress = mpSubRowFinal;
    if( mpSubProgress )
        mpSubProgress->Activate();
}

void XclExpProgressBar::Progress()
{
    if( mpSubProgress && !mpSubProgress->IsFull() )
        mpSubProgress->Progress();
}

// Calc->Excel cell address/range conversion ==================================

namespace {

/** Fills the passed Excel address with the passed Calc cell coordinates without checking any limits. */
inline void lclFillAddress( XclAddress& rXclPos, SCCOL nScCol, SCROW nScRow )
{
    rXclPos.mnCol = static_cast< sal_uInt16 >( nScCol );
    rXclPos.mnRow = static_cast< sal_uInt32 >( nScRow );
}

} // namespace

// ----------------------------------------------------------------------------

XclExpAddressConverter::XclExpAddressConverter( const XclExpRoot& rRoot ) :
    XclAddressConverterBase( rRoot.GetTracer(), rRoot.GetXclMaxPos() )
{
}

// cell address ---------------------------------------------------------------

bool XclExpAddressConverter::CheckAddress( const ScAddress& rScPos, bool bWarn )
{
    // ScAddress::operator<=() doesn't do what we want here
    bool bValidCol = (0 <= rScPos.Col()) && (rScPos.Col() <= maMaxPos.Col());
    bool bValidRow = (0 <= rScPos.Row()) && (rScPos.Row() <= maMaxPos.Row());
    bool bValidTab = (0 <= rScPos.Tab()) && (rScPos.Tab() <= maMaxPos.Tab());

    bool bValid = bValidCol && bValidRow && bValidTab;
    if( !bValid && bWarn )
    {
        mbColTrunc |= !bValidCol;
        mbRowTrunc |= !bValidRow;
        mbTabTrunc |= (rScPos.Tab() > maMaxPos.Tab());  // do not warn for deleted refs
        mrTracer.TraceInvalidAddress( rScPos, maMaxPos );
    }
    return bValid;
}

bool XclExpAddressConverter::ConvertAddress( XclAddress& rXclPos,
        const ScAddress& rScPos, bool bWarn )
{
    bool bValid = CheckAddress( rScPos, bWarn );
    if( bValid )
        lclFillAddress( rXclPos, rScPos.Col(), rScPos.Row() );
    return bValid;
}

XclAddress XclExpAddressConverter::CreateValidAddress( const ScAddress& rScPos, bool bWarn )
{
    XclAddress aXclPos( ScAddress::UNINITIALIZED );
    if( !ConvertAddress( aXclPos, rScPos, bWarn ) )
        lclFillAddress( aXclPos, ::std::min( rScPos.Col(), maMaxPos.Col() ), ::std::min( rScPos.Row(), maMaxPos.Row() ) );
    return aXclPos;
}

// cell range -----------------------------------------------------------------

bool XclExpAddressConverter::CheckRange( const ScRange& rScRange, bool bWarn )
{
    return CheckAddress( rScRange.aStart, bWarn ) && CheckAddress( rScRange.aEnd, bWarn );
}

bool XclExpAddressConverter::ValidateRange( ScRange& rScRange, bool bWarn )
{
    rScRange.Justify();

    // check start position
    bool bValidStart = CheckAddress( rScRange.aStart, bWarn );
    if( bValidStart )
    {
        // check & correct end position
        ScAddress& rScEnd = rScRange.aEnd;
        if( !CheckAddress( rScEnd, bWarn ) )
        {
            rScEnd.SetCol( ::std::min( rScEnd.Col(), maMaxPos.Col() ) );
            rScEnd.SetRow( ::std::min( rScEnd.Row(), maMaxPos.Row() ) );
            rScEnd.SetTab( ::std::min( rScEnd.Tab(), maMaxPos.Tab() ) );
        }
    }

    return bValidStart;
}

bool XclExpAddressConverter::ConvertRange( XclRange& rXclRange,
        const ScRange& rScRange, bool bWarn )
{
    // check start position
    bool bValidStart = CheckAddress( rScRange.aStart, bWarn );
    if( bValidStart )
    {
        lclFillAddress( rXclRange.maFirst, rScRange.aStart.Col(), rScRange.aStart.Row() );

        // check & correct end position
        SCCOL nScCol2 = rScRange.aEnd.Col();
        SCROW nScRow2 = rScRange.aEnd.Row();
        if( !CheckAddress( rScRange.aEnd, bWarn ) )
        {
            nScCol2 = ::std::min( nScCol2, maMaxPos.Col() );
            nScRow2 = ::std::min( nScRow2, maMaxPos.Row() );
        }
        lclFillAddress( rXclRange.maLast, nScCol2, nScRow2 );
    }
    return bValidStart;
}

// cell range list ------------------------------------------------------------

void XclExpAddressConverter::ValidateRangeList( ScRangeList& rScRanges, bool bWarn )
{
    for ( size_t nRange = rScRanges.size(); nRange > 0; )
    {
        ScRange* pScRange = rScRanges[ --nRange ];
        if( !CheckRange( *pScRange, bWarn ) )
            delete rScRanges.Remove(nRange);
    }
}

void XclExpAddressConverter::ConvertRangeList( XclRangeList& rXclRanges,
        const ScRangeList& rScRanges, bool bWarn )
{
    rXclRanges.clear();
    for( size_t nPos = 0, nCount = rScRanges.size(); nPos < nCount; ++nPos )
    {
        if( const ScRange* pScRange = rScRanges[ nPos ] )
        {
            XclRange aXclRange( ScAddress::UNINITIALIZED );
            if( ConvertRange( aXclRange, *pScRange, bWarn ) )
                rXclRanges.push_back( aXclRange );
        }
    }
}

// EditEngine->String conversion ==============================================

namespace {

rtl::OUString lclGetUrlRepresentation( const SvxURLField& rUrlField )
{
    const rtl::OUString& aRepr = rUrlField.GetRepresentation();
    // no representation -> use URL
    return aRepr.isEmpty() ? rUrlField.GetURL() : aRepr;
}

} // namespace

// ----------------------------------------------------------------------------

XclExpHyperlinkHelper::XclExpHyperlinkHelper( const XclExpRoot& rRoot, const ScAddress& rScPos ) :
    XclExpRoot( rRoot ),
    maScPos( rScPos ),
    mbMultipleUrls( false )
{
}

XclExpHyperlinkHelper::~XclExpHyperlinkHelper()
{
}

rtl::OUString XclExpHyperlinkHelper::ProcessUrlField( const SvxURLField& rUrlField )
{
    rtl::OUString aUrlRepr;

    if( GetBiff() == EXC_BIFF8 )    // no HLINK records in BIFF2-BIFF7
    {
        // there was/is already a HLINK record
        mbMultipleUrls = static_cast< bool >(mxLinkRec);

        mxLinkRec.reset( new XclExpHyperlink( GetRoot(), rUrlField, maScPos ) );

        if( const String* pRepr = mxLinkRec->GetRepr() )
            aUrlRepr = *pRepr;

        // add URL to note text
        maUrlList = ScGlobal::addToken( maUrlList, rUrlField.GetURL(), '\n' );
    }

    // no hyperlink representation from Excel HLINK record -> use it from text field
    return aUrlRepr.isEmpty() ? lclGetUrlRepresentation(rUrlField) : aUrlRepr;
}

bool XclExpHyperlinkHelper::HasLinkRecord() const
{
    return !mbMultipleUrls && mxLinkRec;
}

XclExpHyperlinkHelper::XclExpHyperlinkRef XclExpHyperlinkHelper::GetLinkRecord()
{
    if( HasLinkRecord() )
        return mxLinkRec;
    return XclExpHyperlinkRef();
}

// ----------------------------------------------------------------------------

namespace {

/** Creates a new formatted string from the passed unformatted string.

    Creates a Unicode string or a byte string, depending on the current BIFF
    version contained in the passed XclExpRoot object. May create a formatted
    string object, if the text contains different script types.

    @param pCellAttr
        Cell attributes used for font formatting.
    @param nFlags
        Modifiers for string export.
    @param nMaxLen
        The maximum number of characters to store in this string.
    @return
        The new string object.
 */
XclExpStringRef lclCreateFormattedString(
        const XclExpRoot& rRoot, const String& rText, const ScPatternAttr* pCellAttr,
        XclStrFlags nFlags, sal_uInt16 nMaxLen )
{
    /*  Create an empty Excel string object with correctly initialized BIFF mode,
        because this function only uses Append() functions that require this. */
    XclExpStringRef xString = XclExpStringHelper::CreateString( rRoot, EMPTY_STRING, nFlags, nMaxLen );

    // script type handling
    Reference< XBreakIterator > xBreakIt = rRoot.GetDoc().GetBreakIterator();
    namespace ApiScriptType = ::com::sun::star::i18n::ScriptType;
    // #i63255# get script type for leading weak characters
    sal_Int16 nLastScript = XclExpStringHelper::GetLeadingScriptType( rRoot, rText );

    // font buffer and cell item set
    XclExpFontBuffer& rFontBuffer = rRoot.GetFontBuffer();
    const SfxItemSet& rItemSet = pCellAttr ? pCellAttr->GetItemSet() : rRoot.GetDoc().GetDefPattern()->GetItemSet();

    // process all script portions
    OUString aOUText( rText );
    sal_Int32 nPortionPos = 0;
    sal_Int32 nTextLen = aOUText.getLength();
    while( nPortionPos < nTextLen )
    {
        // get script type and end position of next script portion
        sal_Int16 nScript = xBreakIt->getScriptType( aOUText, nPortionPos );
        sal_Int32 nPortionEnd = xBreakIt->endOfScript( aOUText, nPortionPos, nScript );

        // reuse previous script for following weak portions
        if( nScript == ApiScriptType::WEAK )
            nScript = nLastScript;

        // construct font from current text portion
        SvxFont aFont( XclExpFontHelper::GetFontFromItemSet( rRoot, rItemSet, nScript ) );

        // Excel start position of this portion
        sal_uInt16 nXclPortionStart = xString->Len();
        // add portion text to Excel string
        XclExpStringHelper::AppendString( *xString, rRoot, aOUText.copy( nPortionPos, nPortionEnd - nPortionPos ) );
        if( nXclPortionStart < xString->Len() )
        {
            // insert font into buffer
            sal_uInt16 nFontIdx = rFontBuffer.Insert( aFont, EXC_COLOR_CELLTEXT );
            // insert font index into format run vector
            xString->AppendFormat( nXclPortionStart, nFontIdx );
        }

        // go to next script portion
        nLastScript = nScript;
        nPortionPos = nPortionEnd;
    }

    return xString;
}

/** Creates a new formatted string from an edit engine text object.

    Creates a Unicode string or a byte string, depending on the current BIFF
    version contained in the passed XclExpRoot object.

    @param rEE
        The edit engine in use. The text object must already be set.
    @param nFlags
        Modifiers for string export.
    @param nMaxLen
        The maximum number of characters to store in this string.
    @return
        The new string object.
 */
XclExpStringRef lclCreateFormattedString(
        const XclExpRoot& rRoot, EditEngine& rEE, XclExpHyperlinkHelper* pLinkHelper,
        XclStrFlags nFlags, sal_uInt16 nMaxLen )
{
    /*  Create an empty Excel string object with correctly initialized BIFF mode,
        because this function only uses Append() functions that require this. */
    XclExpStringRef xString = XclExpStringHelper::CreateString( rRoot, EMPTY_STRING, nFlags, nMaxLen );

    // font buffer and helper item set for edit engine -> Calc item conversion
    XclExpFontBuffer& rFontBuffer = rRoot.GetFontBuffer();
    SfxItemSet aItemSet( *rRoot.GetDoc().GetPool(), ATTR_PATTERN_START, ATTR_PATTERN_END );

    // script type handling
    Reference< XBreakIterator > xBreakIt = rRoot.GetDoc().GetBreakIterator();
    namespace ApiScriptType = ::com::sun::star::i18n::ScriptType;
    // #i63255# get script type for leading weak characters
    sal_Int16 nLastScript = XclExpStringHelper::GetLeadingScriptType( rRoot, rEE.GetText() );

    // process all paragraphs
    sal_Int32 nParaCount = rEE.GetParagraphCount();
    for( sal_Int32 nPara = 0; nPara < nParaCount; ++nPara )
    {
        ESelection aSel( nPara, 0 );
        String aParaText( rEE.GetText( nPara ) );

        std::vector<sal_uInt16> aPosList;
        rEE.GetPortions( nPara, aPosList );

        // process all portions in the paragraph
        for( std::vector<sal_uInt16>::const_iterator it(aPosList.begin()); it != aPosList.end(); ++it )
        {
            aSel.nEndPos = static_cast< xub_StrLen >( *it );
            String aXclPortionText( aParaText, aSel.nStartPos, aSel.nEndPos - aSel.nStartPos );

            aItemSet.ClearItem();
            SfxItemSet aEditSet( rEE.GetAttribs( aSel ) );
            ScPatternAttr::GetFromEditItemSet( aItemSet, aEditSet );

            // get escapement value
            short nEsc = GETITEM( aEditSet, SvxEscapementItem, EE_CHAR_ESCAPEMENT ).GetEsc();

            // process text fields
            bool bIsHyperlink = false;
            if( aSel.nStartPos + 1 == aSel.nEndPos )
            {
                // test if the character is a text field
                const SfxPoolItem* pItem;
                if( aEditSet.GetItemState( EE_FEATURE_FIELD, false, &pItem ) == SFX_ITEM_SET )
                {
                    const SvxFieldData* pField = static_cast< const SvxFieldItem* >( pItem )->GetField();
                    if( const SvxURLField* pUrlField = PTR_CAST( SvxURLField, pField ) )
                    {
                        // convert URL field to string representation
                        aXclPortionText = pLinkHelper ?
                            pLinkHelper->ProcessUrlField( *pUrlField ) :
                            lclGetUrlRepresentation( *pUrlField );
                        bIsHyperlink = true;
                    }
                    else
                    {
                        OSL_FAIL( "lclCreateFormattedString - unknown text field" );
                        aXclPortionText.Erase();
                    }
                }
            }

            // Excel start position of this portion
            sal_uInt16 nXclPortionStart = xString->Len();
            // add portion text to Excel string
            XclExpStringHelper::AppendString( *xString, rRoot, aXclPortionText );
            if( (nXclPortionStart < xString->Len()) || (aParaText.Len() == 0) )
            {
                /*  Construct font from current edit engine text portion. Edit engine
                    creates different portions for different script types, no need to loop. */
                sal_Int16 nScript = xBreakIt->getScriptType( aXclPortionText, 0 );
                if( nScript == ApiScriptType::WEAK )
                    nScript = nLastScript;
                SvxFont aFont( XclExpFontHelper::GetFontFromItemSet( rRoot, aItemSet, nScript ) );
                nLastScript = nScript;

                // add escapement
                aFont.SetEscapement( nEsc );
                // modify automatic font color for hyperlinks
                if( bIsHyperlink && (GETITEM( aItemSet, SvxColorItem, ATTR_FONT_COLOR ).GetValue().GetColor() == COL_AUTO) )
                    aFont.SetColor( Color( COL_LIGHTBLUE ) );

                // insert font into buffer
                sal_uInt16 nFontIdx = rFontBuffer.Insert( aFont, EXC_COLOR_CELLTEXT );
                // insert font index into format run vector
                xString->AppendFormat( nXclPortionStart, nFontIdx );
            }

            aSel.nStartPos = aSel.nEndPos;
        }

        // add trailing newline (important for correct character index calculation)
        if( nPara + 1 < nParaCount )
            XclExpStringHelper::AppendChar( *xString, rRoot, '\n' );
    }

    return xString;
}

} // namespace

// ----------------------------------------------------------------------------

XclExpStringRef XclExpStringHelper::CreateString(
        const XclExpRoot& rRoot, const String& rString, XclStrFlags nFlags, sal_uInt16 nMaxLen )
{
    XclExpStringRef xString( new XclExpString );
    if( rRoot.GetBiff() == EXC_BIFF8 )
        xString->Assign( rString, nFlags, nMaxLen );
    else
        xString->AssignByte( rString, rRoot.GetTextEncoding(), nFlags, nMaxLen );
    return xString;
}

XclExpStringRef XclExpStringHelper::CreateString(
        const XclExpRoot& rRoot, sal_Unicode cChar, XclStrFlags nFlags, sal_uInt16 nMaxLen )
{
    XclExpStringRef xString = CreateString( rRoot, EMPTY_STRING, nFlags, nMaxLen );
    AppendChar( *xString, rRoot, cChar );
    return xString;
}

void XclExpStringHelper::AppendString( XclExpString& rXclString, const XclExpRoot& rRoot, const String& rString )
{
    if( rRoot.GetBiff() == EXC_BIFF8 )
        rXclString.Append( rString );
    else
        rXclString.AppendByte( rString, rRoot.GetTextEncoding() );
}

void XclExpStringHelper::AppendChar( XclExpString& rXclString, const XclExpRoot& rRoot, sal_Unicode cChar )
{
    if( rRoot.GetBiff() == EXC_BIFF8 )
        rXclString.Append( rtl::OUString(cChar) );
    else
        rXclString.AppendByte( cChar, rRoot.GetTextEncoding() );
}

XclExpStringRef XclExpStringHelper::CreateCellString(
        const XclExpRoot& rRoot, const ScStringCell& rStringCell, const ScPatternAttr* pCellAttr,
        XclStrFlags nFlags, sal_uInt16 nMaxLen )
{
    rtl::OUString aCellText = rStringCell.GetString();
    return lclCreateFormattedString( rRoot, aCellText, pCellAttr, nFlags, nMaxLen );
}

XclExpStringRef XclExpStringHelper::CreateCellString(
        const XclExpRoot& rRoot, const ScEditCell& rEditCell, const ScPatternAttr* pCellAttr,
        XclExpHyperlinkHelper& rLinkHelper, XclStrFlags nFlags, sal_uInt16 nMaxLen )
{
    XclExpStringRef xString;
    if( const EditTextObject* pEditObj = rEditCell.GetData() )
    {
        // formatted cell
        ScEditEngineDefaulter& rEE = rRoot.GetEditEngine();
        sal_Bool bOldUpdateMode = rEE.GetUpdateMode();
        rEE.SetUpdateMode( sal_True );
        // default items
        const SfxItemSet& rItemSet = pCellAttr ? pCellAttr->GetItemSet() : rRoot.GetDoc().GetDefPattern()->GetItemSet();
        SfxItemSet* pEEItemSet = new SfxItemSet( rEE.GetEmptyItemSet() );
        ScPatternAttr::FillToEditItemSet( *pEEItemSet, rItemSet );
        rEE.SetDefaults( pEEItemSet );      // edit engine takes ownership
        // create the string
        rEE.SetText( *pEditObj );
        xString = lclCreateFormattedString( rRoot, rEE, &rLinkHelper, nFlags, nMaxLen );
        rEE.SetUpdateMode( bOldUpdateMode );
    }
    else
    {
        // unformatted cell
        String aCellText = rEditCell.GetString();
        xString = lclCreateFormattedString( rRoot, aCellText, pCellAttr, nFlags, nMaxLen );
    }
    return xString;
}

XclExpStringRef XclExpStringHelper::CreateString(
        const XclExpRoot& rRoot, const SdrTextObj& rTextObj,
        XclStrFlags nFlags, sal_uInt16 nMaxLen )
{
    XclExpStringRef xString;
    if( const OutlinerParaObject* pParaObj = rTextObj.GetOutlinerParaObject() )
    {
        EditEngine& rEE = rRoot.GetDrawEditEngine();
        sal_Bool bOldUpdateMode = rEE.GetUpdateMode();
        rEE.SetUpdateMode( sal_True );
        // create the string
        rEE.SetText( pParaObj->GetTextObject() );
        xString = lclCreateFormattedString( rRoot, rEE, 0, nFlags, nMaxLen );
        rEE.SetUpdateMode( bOldUpdateMode );
        // limit formats - TODO: BIFF dependent
        if( !xString->IsEmpty() )
        {
            xString->LimitFormatCount( EXC_MAXRECSIZE_BIFF8 / 8 - 1 );
            xString->AppendTrailingFormat( EXC_FONT_APP );
        }
    }
    else
    {
        OSL_FAIL( "XclExpStringHelper::CreateString - textbox without para object" );
        // create BIFF dependent empty Excel string
        xString = CreateString( rRoot, EMPTY_STRING, nFlags, nMaxLen );
    }
    return xString;
}

XclExpStringRef XclExpStringHelper::CreateString(
        const XclExpRoot& rRoot, const EditTextObject& rEditObj,
        XclStrFlags nFlags, sal_uInt16 nMaxLen )
{
    XclExpStringRef xString;
    EditEngine& rEE = rRoot.GetDrawEditEngine();
    sal_Bool bOldUpdateMode = rEE.GetUpdateMode();
    rEE.SetUpdateMode( sal_True );
    rEE.SetText( rEditObj );
    xString = lclCreateFormattedString( rRoot, rEE, 0, nFlags, nMaxLen );
    rEE.SetUpdateMode( bOldUpdateMode );
    // limit formats - TODO: BIFF dependent
    if( !xString->IsEmpty() )
    {
        xString->LimitFormatCount( EXC_MAXRECSIZE_BIFF8 / 8 - 1 );
        xString->AppendTrailingFormat( EXC_FONT_APP );
    }
    return xString;
}

sal_Int16 XclExpStringHelper::GetLeadingScriptType( const XclExpRoot& rRoot, const String& rString )
{
    namespace ApiScriptType = ::com::sun::star::i18n::ScriptType;
    Reference< XBreakIterator > xBreakIt = rRoot.GetDoc().GetBreakIterator();
    OUString aOUString( rString );
    sal_Int32 nStrPos = 0;
    sal_Int32 nStrLen = aOUString.getLength();
    sal_Int16 nScript = ApiScriptType::WEAK;
    while( (nStrPos < nStrLen) && (nScript == ApiScriptType::WEAK) )
    {
        nScript = xBreakIt->getScriptType( aOUString, nStrPos );
        nStrPos = xBreakIt->endOfScript( aOUString, nStrPos, nScript );
    }
    return (nScript == ApiScriptType::WEAK) ? rRoot.GetDefApiScript() : nScript;
}

// Header/footer conversion ===================================================

XclExpHFConverter::XclExpHFConverter( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot ),
    mrEE( rRoot.GetHFEditEngine() ),
    mnTotalHeight( 0 )
{
}

void XclExpHFConverter::GenerateString(
        const EditTextObject* pLeftObj,
        const EditTextObject* pCenterObj,
        const EditTextObject* pRightObj )
{
    maHFString.Erase();
    mnTotalHeight = 0;
    AppendPortion( pLeftObj, 'L' );
    AppendPortion( pCenterObj, 'C' );
    AppendPortion( pRightObj, 'R' );
}

void XclExpHFConverter::AppendPortion( const EditTextObject* pTextObj, sal_Unicode cPortionCode )
{
    if( !pTextObj ) return;

    String aText;
    sal_Int32 nHeight = 0;
    SfxItemSet aItemSet( *GetDoc().GetPool(), ATTR_PATTERN_START, ATTR_PATTERN_END );

    // edit engine
    sal_Bool bOldUpdateMode = mrEE.GetUpdateMode();
    mrEE.SetUpdateMode( sal_True );
    mrEE.SetText( *pTextObj );

    // font information
    XclFontData aFontData, aNewData;
    if( const XclExpFont* pFirstFont = GetFontBuffer().GetFont( EXC_FONT_APP ) )
    {
        aFontData = pFirstFont->GetFontData();
        (aFontData.mnHeight += 10) /= 20;   // using pt here, not twips
    }
    else
        aFontData.mnHeight = 10;

    const FontList* pFontList = 0;
    if( SfxObjectShell* pDocShell = GetDocShell() )
    {
        if( const SvxFontListItem* pInfoItem = static_cast< const SvxFontListItem* >(
                pDocShell->GetItem( SID_ATTR_CHAR_FONTLIST ) ) )
            pFontList = pInfoItem->GetFontList();
    }

    sal_Int32 nParaCount = mrEE.GetParagraphCount();
    for( sal_Int32 nPara = 0; nPara < nParaCount; ++nPara )
    {
        ESelection aSel( nPara, 0 );
        String aParaText;
        sal_Int32 nParaHeight = 0;
        std::vector<sal_uInt16> aPosList;
        mrEE.GetPortions( nPara, aPosList );

        for( std::vector<sal_uInt16>::const_iterator it( aPosList.begin() ); it != aPosList.end(); ++it )
        {
            aSel.nEndPos = static_cast< xub_StrLen >( *it );
            if( aSel.nStartPos < aSel.nEndPos )
            {

// --- font attributes ---

                Font aFont;
                aItemSet.ClearItem();
                SfxItemSet aEditSet( mrEE.GetAttribs( aSel ) );
                ScPatternAttr::GetFromEditItemSet( aItemSet, aEditSet );
                ScPatternAttr::GetFont( aFont, aItemSet, SC_AUTOCOL_RAW );

                // font name and style
                aNewData.maName = XclTools::GetXclFontName( aFont.GetName() );
                aNewData.mnWeight = (aFont.GetWeight() > WEIGHT_NORMAL) ? EXC_FONTWGHT_BOLD : EXC_FONTWGHT_NORMAL;
                aNewData.mbItalic = (aFont.GetItalic() != ITALIC_NONE);
                bool bNewFont = !(aFontData.maName == aNewData.maName);
                bool bNewStyle = (aFontData.mnWeight != aNewData.mnWeight) ||
                                 (aFontData.mbItalic != aNewData.mbItalic);
                if( bNewFont || (bNewStyle && pFontList) )
                {
                    aParaText.AppendAscii( "&\"" ).Append( aNewData.maName );
                    if( pFontList )
                    {
                        FontInfo aFontInfo( pFontList->Get(
                            aNewData.maName,
                            (aNewData.mnWeight > EXC_FONTWGHT_NORMAL) ? WEIGHT_BOLD : WEIGHT_NORMAL,
                            aNewData.mbItalic ? ITALIC_NORMAL : ITALIC_NONE ) );
                        aNewData.maStyle = pFontList->GetStyleName( aFontInfo );
                        if( aNewData.maStyle.Len() )
                            aParaText.Append( ',' ).Append( aNewData.maStyle );
                    }
                    aParaText.Append( '"' );
                }

                // height
                // is calculated wrong in ScPatternAttr::GetFromEditItemSet, because already in twips and not 100thmm
                // -> get it directly from edit engine item set
                aNewData.mnHeight = ulimit_cast< sal_uInt16 >( GETITEM( aEditSet, SvxFontHeightItem, EE_CHAR_FONTHEIGHT ).GetHeight() );
                (aNewData.mnHeight += 10) /= 20;
                bool bFontHtChanged = (aFontData.mnHeight != aNewData.mnHeight);
                if( bFontHtChanged )
                    aParaText.Append( '&' ).Append( String::CreateFromInt32( aNewData.mnHeight ) );
                // update maximum paragraph height, convert to twips
                nParaHeight = ::std::max< sal_Int32 >( nParaHeight, aNewData.mnHeight * 20 );

                // underline
                aNewData.mnUnderline = EXC_FONTUNDERL_NONE;
                switch( aFont.GetUnderline() )
                {
                    case UNDERLINE_NONE:    aNewData.mnUnderline = EXC_FONTUNDERL_NONE;    break;
                    case UNDERLINE_SINGLE:  aNewData.mnUnderline = EXC_FONTUNDERL_SINGLE;  break;
                    case UNDERLINE_DOUBLE:  aNewData.mnUnderline = EXC_FONTUNDERL_DOUBLE;  break;
                    default:                aNewData.mnUnderline = EXC_FONTUNDERL_SINGLE;
                }
                if( aFontData.mnUnderline != aNewData.mnUnderline )
                {
                    sal_uInt8 nTmpUnderl = (aNewData.mnUnderline == EXC_FONTUNDERL_NONE) ?
                        aFontData.mnUnderline : aNewData.mnUnderline;
                    aParaText.AppendAscii( (nTmpUnderl == EXC_FONTUNDERL_SINGLE) ? "&U" : "&E" );
                }

                // strikeout
                aNewData.mbStrikeout = (aFont.GetStrikeout() != STRIKEOUT_NONE);
                if( aFontData.mbStrikeout != aNewData.mbStrikeout )
                    aParaText.AppendAscii( "&S" );

                // super/sub script
                const SvxEscapementItem& rEscapeItem = GETITEM( aEditSet, SvxEscapementItem, EE_CHAR_ESCAPEMENT );
                aNewData.SetScEscapement( rEscapeItem.GetEsc() );
                if( aFontData.mnEscapem != aNewData.mnEscapem )
                {
                    switch(aNewData.mnEscapem)
                    {
                        // close the previous super/sub script.
                        case EXC_FONTESC_NONE:  aParaText.AppendAscii( (aFontData.mnEscapem == EXC_FONTESC_SUPER) ? "&X" : "&Y" ); break;
                        case EXC_FONTESC_SUPER: aParaText.AppendAscii( "&X" );  break;
                        case EXC_FONTESC_SUB:   aParaText.AppendAscii( "&Y" );  break;
                        default: break;
                    }
                }

                aFontData = aNewData;

// --- text content or text fields ---

                const SfxPoolItem* pItem;
                if( (aSel.nStartPos + 1 == aSel.nEndPos) &&     // fields are single characters
                    (aEditSet.GetItemState( EE_FEATURE_FIELD, false, &pItem ) == SFX_ITEM_SET) )
                {
                    if( const SvxFieldData* pFieldData = static_cast< const SvxFieldItem* >( pItem )->GetField() )
                    {
                        if( pFieldData->ISA( SvxPageField ) )
                            aParaText.AppendAscii( "&P" );
                        else if( pFieldData->ISA( SvxPagesField ) )
                            aParaText.AppendAscii( "&N" );
                        else if( pFieldData->ISA( SvxDateField ) )
                            aParaText.AppendAscii( "&D" );
                        else if( pFieldData->ISA( SvxTimeField ) || pFieldData->ISA( SvxExtTimeField ) )
                            aParaText.AppendAscii( "&T" );
                        else if( pFieldData->ISA( SvxTableField ) )
                            aParaText.AppendAscii( "&A" );
                        else if( pFieldData->ISA( SvxFileField ) )  // title -> file name
                            aParaText.AppendAscii( "&F" );
                        else if( const SvxExtFileField* pFileField = PTR_CAST( SvxExtFileField, pFieldData ) )
                        {
                            switch( pFileField->GetFormat() )
                            {
                                case SVXFILEFORMAT_NAME_EXT:
                                case SVXFILEFORMAT_NAME:
                                    aParaText.AppendAscii( "&F" );
                                break;
                                case SVXFILEFORMAT_PATH:
                                    aParaText.AppendAscii( "&Z" );
                                break;
                                case SVXFILEFORMAT_FULLPATH:
                                    aParaText.AppendAscii( "&Z&F" );
                                break;
                                default:
                                    OSL_FAIL( "XclExpHFConverter::AppendPortion - unknown file field" );
                            }
                        }
                    }
                }
                else
                {
                    String aPortionText( mrEE.GetText( aSel ) );
                    aPortionText.SearchAndReplaceAll( rtl::OUString('&'), rtl::OUString("&&") );
                    // #i17440# space between font height and numbers in text
                    if( bFontHtChanged && aParaText.Len() && aPortionText.Len() )
                    {
                        sal_Unicode cLast = aParaText.GetChar( aParaText.Len() - 1 );
                        sal_Unicode cFirst = aPortionText.GetChar( 0 );
                        if( ('0' <= cLast) && (cLast <= '9') && ('0' <= cFirst) && (cFirst <= '9') )
                            aParaText.Append( ' ' );
                    }
                    aParaText.Append( aPortionText );
                }
            }

            aSel.nStartPos = aSel.nEndPos;
        }

        aText = ScGlobal::addToken( aText, aParaText, '\n' );
        if( nParaHeight == 0 )
            nParaHeight = aFontData.mnHeight * 20;  // points -> twips
        nHeight += nParaHeight;
    }

    mrEE.SetUpdateMode( bOldUpdateMode );

    if( aText.Len() )
    {
        maHFString.Append( '&' ).Append( cPortionCode ).Append( aText );
        mnTotalHeight = ::std::max( mnTotalHeight, nHeight );
    }
}

// URL conversion =============================================================

namespace {

/** Encodes special parts of the URL, i.e. directory separators and volume names.
    @param pTableName  Pointer to a table name to be encoded in this URL, or 0. */
rtl::OUString lclEncodeDosUrl(
    XclBiff eBiff, const rtl::OUString& rUrl, const rtl::OUString& rBase, const rtl::OUString* pTableName)
{
    rtl::OUStringBuffer aBuf;

    if (!rUrl.isEmpty())
    {
        rtl::OUString aOldUrl = rUrl;
        aBuf.append(EXC_URLSTART_ENCODED);

        if ( aOldUrl.getLength() > 2 && aOldUrl.copy(0,2) == "\\\\" )
        {
            // UNC
            aBuf.append(EXC_URL_DOSDRIVE).append(sal_Unicode('@'));
            aOldUrl = aOldUrl.copy(2);
        }
        else if ( aOldUrl.getLength() > 2 && aOldUrl.copy(1,2) == ":\\" )
        {
            // drive letter
            sal_Unicode cThisDrive = rBase.isEmpty() ? ' ' : rBase.getStr()[0];
            sal_Unicode cDrive = aOldUrl.getStr()[0];
            if (cThisDrive == cDrive)
                // This document and the referenced document are under the same drive.
                aBuf.append(EXC_URL_DRIVEROOT);
            else
                aBuf.append(EXC_URL_DOSDRIVE).append(cDrive);
            aOldUrl = aOldUrl.copy(3);
        }

        // directories
        sal_Int32 nPos = -1;
        while((nPos = aOldUrl.indexOf('\\')) != -1)
        {
            if ( aOldUrl.copy(0,2) == ".." )
                // parent dir (NOTE: the MS-XLS spec doesn't mention this, and
                // Excel seems confused by this token).
                aBuf.append(EXC_URL_PARENTDIR);
            else
                aBuf.append(aOldUrl.copy(0,nPos)).append(EXC_URL_SUBDIR);

            aOldUrl = aOldUrl.copy(nPos + 1);
        }

        // file name
        if (pTableName)    // enclose file name in brackets if table name follows
            aBuf.append(sal_Unicode('[')).append(aOldUrl).append(sal_Unicode(']'));
        else
            aBuf.append(aOldUrl);
    }
    else    // empty URL -> self reference
    {
        switch( eBiff )
        {
            case EXC_BIFF5:
                aBuf.append(pTableName ? EXC_URLSTART_SELFENCODED : EXC_URLSTART_SELF);
            break;
            case EXC_BIFF8:
                DBG_ASSERT( pTableName, "lclEncodeDosUrl - sheet name required for BIFF8" );
                aBuf.append(EXC_URLSTART_SELF);
            break;
            default:
                DBG_ERROR_BIFF();
        }
    }

    // table name
    if (pTableName)
        aBuf.append(*pTableName);

    return aBuf.makeStringAndClear();
}

} // namespace

// ----------------------------------------------------------------------------

rtl::OUString XclExpUrlHelper::EncodeUrl( const XclExpRoot& rRoot, const rtl::OUString& rAbsUrl, const rtl::OUString* pTableName )
{
    rtl::OUString aDosUrl = INetURLObject(rAbsUrl).getFSysPath(INetURLObject::FSYS_DOS);
    rtl::OUString aDosBase = INetURLObject(rRoot.GetBasePath()).getFSysPath(INetURLObject::FSYS_DOS);
    return lclEncodeDosUrl(rRoot.GetBiff(), aDosUrl, aDosBase, pTableName);
}

rtl::OUString XclExpUrlHelper::EncodeDde( const rtl::OUString& rApplic, const rtl::OUString& rTopic )
{
    rtl::OUStringBuffer aBuf;
    aBuf.append(rApplic).append(EXC_DDE_DELIM).append(rTopic);
    return aBuf.makeStringAndClear();
}

// Cached Value Lists =========================================================

XclExpCachedMatrix::XclExpCachedMatrix( const ScMatrix& rMatrix )
    : mrMatrix( rMatrix )
{
    mrMatrix.IncRef();
}
XclExpCachedMatrix::~XclExpCachedMatrix()
{
    mrMatrix.DecRef();
}

void XclExpCachedMatrix::GetDimensions( SCSIZE & nCols, SCSIZE & nRows ) const
{
    mrMatrix.GetDimensions( nCols, nRows );

    OSL_ENSURE( nCols && nRows, "XclExpCachedMatrix::GetDimensions - empty matrix" );
    OSL_ENSURE( nCols <= 256, "XclExpCachedMatrix::GetDimensions - too many columns" );
}

sal_Size XclExpCachedMatrix::GetSize() const
{
    SCSIZE nCols, nRows;

    GetDimensions( nCols, nRows );

    /*  The returned size may be wrong if the matrix contains strings. The only
        effect is that the export stream has to update a wrong record size which is
        faster than to iterate through all cached values and calculate their sizes. */
    return 3 + 9 * (nCols * nRows);
}

void XclExpCachedMatrix::Save( XclExpStream& rStrm ) const
{
    SCSIZE nCols, nRows;

    GetDimensions( nCols, nRows );

    if( rStrm.GetRoot().GetBiff() <= EXC_BIFF5 )
        // in BIFF2-BIFF7: 256 columns represented by 0 columns
        rStrm << static_cast< sal_uInt8 >( nCols ) << static_cast< sal_uInt16 >( nRows );
    else
        // in BIFF8: columns and rows decreaed by 1
        rStrm << static_cast< sal_uInt8 >( nCols - 1 ) << static_cast< sal_uInt16 >( nRows - 1 );

    for( SCSIZE nRow = 0; nRow < nRows; ++nRow )
    {
        for( SCSIZE nCol = 0; nCol < nCols; ++nCol )
        {
            ScMatrixValue nMatVal = mrMatrix.Get( nCol, nRow );

            if( SC_MATVAL_EMPTY == nMatVal.nType )
            {
                rStrm.SetSliceSize( 9 );
                rStrm << EXC_CACHEDVAL_EMPTY;
                rStrm.WriteZeroBytes( 8 );
            }
            else if( ScMatrix::IsNonValueType( nMatVal.nType ) )
            {
                XclExpString aStr( nMatVal.GetString(), EXC_STR_DEFAULT );
                rStrm.SetSliceSize( 6 );
                rStrm << EXC_CACHEDVAL_STRING << aStr;
            }
            else if( SC_MATVAL_BOOLEAN == nMatVal.nType )
            {
                sal_Int8 nBool = nMatVal.GetBoolean();
                rStrm.SetSliceSize( 9 );
                rStrm << EXC_CACHEDVAL_BOOL << nBool;
                rStrm.WriteZeroBytes( 7 );
            }
            else if( sal_uInt16 nScError = nMatVal.GetError() )
            {
                sal_Int8 nError ( XclTools::GetXclErrorCode( nScError ) );
                rStrm.SetSliceSize( 9 );
                rStrm << EXC_CACHEDVAL_ERROR << nError;
                rStrm.WriteZeroBytes( 7 );
            }
            else
            {
                rStrm.SetSliceSize( 9 );
                rStrm << EXC_CACHEDVAL_DOUBLE << nMatVal.fVal;
            }
        }
    }
}

// ============================================================================

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
