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


#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif




#include <vcl/edit.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/svapp.hxx>
#include <svl/zforlist.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/i18n/XBreakIterator.hpp>
#include <comphelper/processfactory.hxx>
#include <svtools/scriptedtext.hxx>
#include <svtools/accessibilityoptions.hxx>
#include <svx/framelinkarray.hxx>
#include "swmodule.hxx"
#include "swtypes.hxx"
#include "view.hxx"
#include "wrtsh.hxx"
#include "tblafmt.hxx"
#include "tautofmt.hxx"
#include "shellres.hxx"
#include "tautofmt.hrc"

using namespace com::sun::star;

#define FRAME_OFFSET 4

//========================================================================

class AutoFmtPreview : public Window
{
public:
            AutoFmtPreview( Window* pParent, const ResId& rRes, SwWrtShell* pWrtShell );
            ~AutoFmtPreview();

    void NotifyChange( const SwTableAutoFmt& rNewData );

protected:
    virtual void Paint( const Rectangle& rRect );

private:
    SwTableAutoFmt          aCurData;
    VirtualDevice           aVD;
    SvtScriptedTextHelper   aScriptedText;
    svx::frame::Array       maArray;            /// Implementation to draw the frame borders.
    sal_Bool                    bFitWidth;
    bool                    mbRTL;
    Size                    aPrvSize;
    long                    nLabelColWidth;
    long                    nDataColWidth1;
    long                    nDataColWidth2;
    long                    nRowHeight;
    const String            aStrJan;
    const String            aStrFeb;
    const String            aStrMar;
    const String            aStrNorth;
    const String            aStrMid;
    const String            aStrSouth;
    const String            aStrSum;
    SvNumberFormatter*      pNumFmt;

    uno::Reference< lang::XMultiServiceFactory > m_xMSF;
    uno::Reference< i18n::XBreakIterator >       m_xBreak;

    //-------------------------------------------
    void    Init            ();
    void    DoPaint         ( const Rectangle& rRect );
    void    CalcCellArray   ( sal_Bool bFitWidth );
    void    CalcLineMap     ();
    void    PaintCells      ();

    sal_uInt8                GetFormatIndex( size_t nCol, size_t nRow ) const;
    const SvxBoxItem&   GetBoxItem( size_t nCol, size_t nRow ) const;

    void                DrawString( size_t nCol, size_t nRow );
    void                DrawStrings();
    void                DrawBackground();

    void    MakeFonts       ( sal_uInt8 nIndex, Font& rFont, Font& rCJKFont, Font& rCTLFont );
    String  MakeNumberString( String cellString, sal_Bool bAddDec );
};

//========================================================================

class SwStringInputDlg : public ModalDialog
{
public:
            SwStringInputDlg(     Window* pParent,
                            const String& rTitle,
                            const String& rEditTitle,
                            const String& rDefault );
            ~SwStringInputDlg();

    void GetInputString( String& rString ) const;

private:
    Edit            aEdInput;   // Edit erhaelt so den Focus
    FixedText       aFtEditTitle;
    OKButton        aBtnOk;
    CancelButton    aBtnCancel;
};


SwStringInputDlg::SwStringInputDlg( Window*         pParent,
                                    const String&   rTitle,
                                    const String&   rEditTitle,
                                    const String&   rDefault    ) :
    ModalDialog     ( pParent, SW_RES( DLG_SWDLG_STRINPUT ) ),
    //
    aEdInput        ( this, SW_RES( ED_INPUT ) ),
    aFtEditTitle    ( this, SW_RES( FT_LABEL ) ),
    aBtnOk          ( this, SW_RES( BTN_OK ) ),
    aBtnCancel      ( this, SW_RES( BTN_CANCEL ) )
{
    SetText( rTitle );
    aFtEditTitle.SetText( rEditTitle );
    aEdInput.SetText( rDefault );
    //-------------
    FreeResource();
}

//------------------------------------------------------------------------

void SwStringInputDlg::GetInputString( String& rString ) const
{
    rString = aEdInput.GetText();
}


SwStringInputDlg::~SwStringInputDlg()
{
}

//========================================================================
// AutoFormat-Dialog:


SwAutoFormatDlg::SwAutoFormatDlg( Window* pParent, SwWrtShell* pWrtShell,
                    sal_Bool bSetAutoFormat, const SwTableAutoFmt* pSelFmt )
    : SfxModalDialog( pParent, SW_RES( DLG_AUTOFMT_TABLE ) ),
    //
    aFlFormat       ( this, SW_RES( FL_FORMAT ) ),
    aLbFormat       ( this, SW_RES( LB_FORMAT ) ),
    aFlFormats       ( this, SW_RES( FL_FORMATS ) ),

    aBtnNumFormat   ( this, SW_RES( BTN_NUMFORMAT ) ),
    aBtnBorder      ( this, SW_RES( BTN_BORDER ) ),
    aBtnFont        ( this, SW_RES( BTN_FONT ) ),
    aBtnPattern     ( this, SW_RES( BTN_PATTERN ) ),
    aBtnAlignment   ( this, SW_RES( BTN_ALIGNMENT ) ),
    aBtnOk          ( this, SW_RES( BTN_OK ) ),
    aBtnCancel      ( this, SW_RES( BTN_CANCEL ) ),
    aBtnHelp        ( this, SW_RES( BTN_HELP ) ),
    aBtnAdd         ( this, SW_RES( BTN_ADD ) ),
    aBtnRemove      ( this, SW_RES( BTN_REMOVE ) ),
    aBtnRename      ( this, SW_RES( BTN_RENAME ) ),
    aBtnMore        ( this, SW_RES( BTN_MORE ) ),
    aStrTitle       ( SW_RES( STR_ADD_TITLE ) ),
    aStrLabel       ( SW_RES( STR_ADD_LABEL ) ),
    aStrClose       ( SW_RES( STR_BTN_CLOSE ) ),
    aStrDelTitle    ( SW_RES( STR_DEL_TITLE ) ),
    aStrDelMsg      ( SW_RES( STR_DEL_MSG ) ),
    aStrRenameTitle ( SW_RES( STR_RENAME_TITLE ) ),
    aStrInvalidFmt  ( SW_RES( STR_INVALID_AFNAME )),
    pWndPreview     ( new AutoFmtPreview( this, SW_RES( WND_PREVIEW ), pWrtShell )),
    //
    pShell          ( pWrtShell ),
    nIndex          ( 0 ),
    nDfltStylePos   ( 0 ),
    bCoreDataChanged( sal_False ),
    bSetAutoFmt     ( bSetAutoFormat )
{
    pTableTbl = new SwTableAutoFmtTbl;
    pTableTbl->Load();

    Init( pSelFmt );
    //------------- >
    FreeResource();
}

//------------------------------------------------------------------------


SwAutoFormatDlg::~SwAutoFormatDlg()
{
    delete pWndPreview;

    if( bCoreDataChanged )
        pTableTbl->Save();
    delete pTableTbl;
}

//------------------------------------------------------------------------


void SwAutoFormatDlg::Init( const SwTableAutoFmt* pSelFmt )
{
    Link aLk( LINK( this, SwAutoFormatDlg, CheckHdl ) );
    aBtnBorder.SetClickHdl( aLk );
    aBtnFont.SetClickHdl( aLk );
    aBtnPattern.SetClickHdl( aLk );
    aBtnAlignment.SetClickHdl( aLk );
    aBtnNumFormat.SetClickHdl( aLk );

    aBtnAdd.SetClickHdl ( LINK( this, SwAutoFormatDlg, AddHdl ) );
    aBtnRemove.SetClickHdl ( LINK( this, SwAutoFormatDlg, RemoveHdl ) );
    aBtnRename.SetClickHdl ( LINK( this, SwAutoFormatDlg, RenameHdl ) );
    aBtnOk.SetClickHdl ( LINK( this, SwAutoFormatDlg, OkHdl ) );
    aLbFormat.SetSelectHdl( LINK( this, SwAutoFormatDlg, SelFmtHdl ) );

    aBtnMore.AddWindow( &aBtnNumFormat );
    aBtnMore.AddWindow( &aBtnBorder );
    aBtnMore.AddWindow( &aBtnFont );
    aBtnMore.AddWindow( &aBtnPattern );
    aBtnMore.AddWindow( &aBtnAlignment );
    aBtnMore.AddWindow( &aFlFormats );
    aBtnMore.AddWindow( &aBtnRename );

    aBtnAdd.Enable( bSetAutoFmt );

    nIndex = 0;
    if( !bSetAutoFmt )
    {
        // dann muss die Liste um den Eintrag <Keins> erweitert werden.
        aLbFormat.InsertEntry( ViewShell::GetShellRes()->aStrNone );
        nDfltStylePos = 1;
        nIndex = 255;
    }

    for (sal_uInt8 i = 0, nCount = static_cast<sal_uInt8>(pTableTbl->size());
            i < nCount; i++)
    {
        SwTableAutoFmt const& rFmt = (*pTableTbl)[ i ];
        aLbFormat.InsertEntry(rFmt.GetName());
        if (pSelFmt && rFmt.GetName() == pSelFmt->GetName())
            nIndex = i;
    }

    aLbFormat.SelectEntryPos( 255 != nIndex ? (nDfltStylePos + nIndex) : 0 );
    SelFmtHdl( 0 );
}

//------------------------------------------------------------------------


void SwAutoFormatDlg::UpdateChecks( const SwTableAutoFmt& rFmt, sal_Bool bEnable )
{
    aBtnNumFormat.Enable( bEnable );
    aBtnNumFormat.Check( rFmt.IsValueFormat() );

    aBtnBorder.Enable( bEnable );
    aBtnBorder.Check( rFmt.IsFrame() );

    aBtnFont.Enable( bEnable );
    aBtnFont.Check( rFmt.IsFont() );

    aBtnPattern.Enable( bEnable );
    aBtnPattern.Check( rFmt.IsBackground() );

    aBtnAlignment.Enable( bEnable );
    aBtnAlignment.Check( rFmt.IsJustify() );
}

void SwAutoFormatDlg::FillAutoFmtOfIndex( SwTableAutoFmt*& rToFill ) const
{
    if( 255 != nIndex )
    {
        if( rToFill )
            *rToFill = (*pTableTbl)[ nIndex ];
        else
            rToFill = new SwTableAutoFmt( (*pTableTbl)[ nIndex ] );
    }
    else
        delete rToFill, rToFill = 0;
}


/*------------------------------------------------------------------------
  Handler:
  ---------*/


IMPL_LINK( SwAutoFormatDlg, CheckHdl, Button *, pBtn )
{
    SwTableAutoFmt* pData  = &(*pTableTbl)[nIndex];
    sal_Bool bCheck = ((CheckBox*)pBtn)->IsChecked(), bDataChgd = sal_True;

    if( pBtn == &aBtnNumFormat )
        pData->SetValueFormat( bCheck );
    else if ( pBtn == &aBtnBorder )
        pData->SetFrame( bCheck );
    else if ( pBtn == &aBtnFont )
        pData->SetFont( bCheck );
    else if ( pBtn == &aBtnPattern )
        pData->SetBackground( bCheck );
    else if ( pBtn == &aBtnAlignment )
        pData->SetJustify( bCheck );
    else
        bDataChgd = sal_False;

    if( bDataChgd )
    {
        if( !bCoreDataChanged )
        {
            aBtnCancel.SetText( aStrClose );
            bCoreDataChanged = sal_True;
        }

        pWndPreview->NotifyChange( *pData );
    }
    return 0;
}

/*------------------------------------------------------------------------*/


IMPL_LINK_NOARG(SwAutoFormatDlg, AddHdl)
{
    sal_Bool bOk = sal_False, bFmtInserted = sal_False;
    while( !bOk )
    {
        SwStringInputDlg*   pDlg = new SwStringInputDlg( this,
                                                            aStrTitle,
                                                            aStrLabel,
                                                            aEmptyStr );
        if( RET_OK == pDlg->Execute() )
        {
            String aFormatName;
            pDlg->GetInputString( aFormatName );

            if( aFormatName.Len() > 0 )
            {
                sal_uInt16 n;
                for( n = 0; n < pTableTbl->size(); ++n )
                    if( (*pTableTbl)[n].GetName() == aFormatName )
                        break;

                if( n >= pTableTbl->size() )
                {
                    // Format mit dem Namen noch nicht vorhanden, also
                    // aufnehmen
                    SwTableAutoFmt* pNewData = new
                                        SwTableAutoFmt( aFormatName );
                    pShell->GetTableAutoFmt( *pNewData );

                    // Sortiert einfuegen!!
                    for( n = 1; n < pTableTbl->size(); ++n )
                        if( (*pTableTbl)[ n ].GetName() > aFormatName )
                            break;

                    pTableTbl->InsertAutoFmt(n, pNewData);
                    aLbFormat.InsertEntry( aFormatName, nDfltStylePos + n );
                    aLbFormat.SelectEntryPos( nDfltStylePos + n );
                    bFmtInserted = sal_True;
                    aBtnAdd.Enable( sal_False );
                    if ( !bCoreDataChanged )
                    {
                        aBtnCancel.SetText( aStrClose );
                        bCoreDataChanged = sal_True;
                    }

                    SelFmtHdl( 0 );
                    bOk = sal_True;
                }
            }

            if( !bFmtInserted )
            {
                bOk = RET_CANCEL == ErrorBox( this,
                                    WinBits( WB_OK_CANCEL | WB_DEF_OK),
                                    aStrInvalidFmt
                                    ).Execute();
            }
        }
        else
            bOk = sal_True;
        delete pDlg;
    }
    return 0;
}

//------------------------------------------------------------------------

IMPL_LINK_NOARG(SwAutoFormatDlg, RemoveHdl)
{
    String aMessage = aStrDelMsg ;
    aMessage.AppendAscii("\n\n");
    aMessage += aLbFormat.GetSelectEntry() ;
    aMessage += '\n';

    MessBox* pBox = new MessBox( this, WinBits( WB_OK_CANCEL ),
                                    aStrDelTitle, aMessage);

    if ( pBox->Execute() == RET_OK )
    {
        aLbFormat.RemoveEntry( nDfltStylePos + nIndex );
        aLbFormat.SelectEntryPos( nDfltStylePos + nIndex-1 );

        pTableTbl->EraseAutoFmt(nIndex);
        nIndex--;

        if( !nIndex )
        {
            aBtnRemove.Enable(sal_False);
            aBtnRename.Enable(sal_False);
        }

        if( !bCoreDataChanged )
        {
            aBtnCancel.SetText( aStrClose );
            bCoreDataChanged = sal_True;
        }
    }
    delete pBox;

    SelFmtHdl( 0 );

    return 0;
}

IMPL_LINK_NOARG(SwAutoFormatDlg, RenameHdl)
{
    sal_Bool bOk = sal_False;
    while( !bOk )
    {
        SwStringInputDlg* pDlg = new SwStringInputDlg( this,
                        aStrRenameTitle, aLbFormat.GetSelectEntry(),
                                                        aEmptyStr );
        if( pDlg->Execute() == RET_OK )
        {
            sal_Bool bFmtRenamed = sal_False;
            String aFormatName;
            pDlg->GetInputString( aFormatName );

            if ( aFormatName.Len() > 0 )
            {
                sal_uInt16 n;
                for( n = 0; n < pTableTbl->size(); ++n )
                    if ((*pTableTbl)[n].GetName() == aFormatName)
                        break;

                if( n >= pTableTbl->size() )
                {
                    // no format with this name exists, so rename it
                    aLbFormat.RemoveEntry( nDfltStylePos + nIndex );
                    SwTableAutoFmt* p = &(*pTableTbl)[ nIndex ];

                    p->SetName( aFormatName );

                    // keep all arrays sorted!
                    for( n = 1; n < pTableTbl->size(); ++n )
                        if ((n != nIndex) &&
                            ((*pTableTbl)[n].GetName() > aFormatName))
                        {
                            break;
                        }

                    pTableTbl->MoveAutoFmt(n, nIndex);
                    aLbFormat.InsertEntry( aFormatName, nDfltStylePos + n );
                    aLbFormat.SelectEntryPos( nDfltStylePos + n );

                    if ( !bCoreDataChanged )
                    {
                        aBtnCancel.SetText( aStrClose );
                        bCoreDataChanged = sal_True;
                    }

                    SelFmtHdl( 0 );
                    bOk = sal_True;
                    bFmtRenamed = sal_True;
                }
            }

            if( !bFmtRenamed )
            {
                bOk = RET_CANCEL == ErrorBox( this,
                                    WinBits( WB_OK_CANCEL | WB_DEF_OK),
                                    aStrInvalidFmt
                                    ).Execute();
            }
        }
        else
            bOk = sal_True;
        delete pDlg;
    }
    return 0;
}

//------------------------------------------------------------------------

IMPL_LINK_NOARG(SwAutoFormatDlg, SelFmtHdl)
{
    sal_Bool bBtnEnable = sal_False;
    sal_uInt8 nSelPos = (sal_uInt8) aLbFormat.GetSelectEntryPos(), nOldIdx = nIndex;
    if( nSelPos >= nDfltStylePos )
    {
        nIndex = nSelPos - nDfltStylePos;
        pWndPreview->NotifyChange( (*pTableTbl)[nIndex] );
        bBtnEnable = 0 != nIndex;
        UpdateChecks( (*pTableTbl)[nIndex], sal_True );
    }
    else
    {
        nIndex = 255;

        SwTableAutoFmt aTmp( ViewShell::GetShellRes()->aStrNone );
        aTmp.SetFont( sal_False );
        aTmp.SetJustify( sal_False );
        aTmp.SetFrame( sal_False );
        aTmp.SetBackground( sal_False );
        aTmp.SetValueFormat( sal_False );
        aTmp.SetWidthHeight( sal_False );

        if( nOldIdx != nIndex )
            pWndPreview->NotifyChange( aTmp );
        UpdateChecks( aTmp, sal_False );
    }

    aBtnRemove.Enable( bBtnEnable );
    aBtnRename.Enable( bBtnEnable );

    return 0;
}
//------------------------------------------------------------------------

IMPL_LINK_NOARG_INLINE_START(SwAutoFormatDlg, OkHdl)
{
    if( bSetAutoFmt )
        pShell->SetTableAutoFmt( (*pTableTbl)[ nIndex ] );
    EndDialog( RET_OK );
    return sal_True;
}
IMPL_LINK_NOARG_INLINE_END(SwAutoFormatDlg, OkHdl)

//========================================================================
// AutoFmtPreview

//------------------------------------------------------------------------

AutoFmtPreview::AutoFmtPreview( Window* pParent, const ResId& rRes, SwWrtShell* pWrtShell ) :
        Window          ( pParent, rRes ),

        aCurData        ( aEmptyStr ),
        aVD             ( *this ),
        aScriptedText   ( aVD ),
        bFitWidth       ( sal_False ),
        mbRTL           ( false ),
        aPrvSize        ( GetSizePixel().Width() - 6, GetSizePixel().Height() - 30 ),
        nLabelColWidth  ( (aPrvSize.Width() - 4) / 4 - 12 ),
        nDataColWidth1  ( (aPrvSize.Width() - 4 - 2 * nLabelColWidth) / 3 ),
        nDataColWidth2  ( (aPrvSize.Width() - 4 - 2 * nLabelColWidth) / 4 ),
        nRowHeight      ( (aPrvSize.Height() - 4) / 5 ),
        aStrJan         ( SW_RES( STR_JAN ) ),
        aStrFeb         ( SW_RES( STR_FEB ) ),
        aStrMar         ( SW_RES( STR_MAR ) ),
        aStrNorth       ( SW_RES( STR_NORTH ) ),
        aStrMid         ( SW_RES( STR_MID ) ),
        aStrSouth       ( SW_RES( STR_SOUTH ) ),
        aStrSum         ( SW_RES( STR_SUM ) ),
        m_xMSF          ( comphelper::getProcessServiceFactory() )
{
    if (!pWrtShell->IsCrsrInTbl()) // We haven't created the table yet
        mbRTL = Application::GetSettings().GetLayoutRTL();
    else
        mbRTL = pWrtShell->IsTableRightToLeft();

    OSL_ENSURE( m_xMSF.is(), "AutoFmtPreview: no MultiServiceFactory");
    if ( m_xMSF.is() )
    {
        m_xBreak = uno::Reference< i18n::XBreakIterator >(
            m_xMSF->createInstance (
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.i18n.BreakIterator")) ),
            uno::UNO_QUERY);
    }
    pNumFmt = new SvNumberFormatter( m_xMSF, LANGUAGE_SYSTEM );

    Init();
}

//------------------------------------------------------------------------

AutoFmtPreview::~AutoFmtPreview()
{
    delete pNumFmt;
}

//------------------------------------------------------------------------

static void lcl_SetFontProperties(
        Font& rFont,
        const SvxFontItem& rFontItem,
        const SvxWeightItem& rWeightItem,
        const SvxPostureItem& rPostureItem )
{
    rFont.SetFamily     ( rFontItem.GetFamily() );
    rFont.SetName       ( rFontItem.GetFamilyName() );
    rFont.SetStyleName  ( rFontItem.GetStyleName() );
    rFont.SetCharSet    ( rFontItem.GetCharSet() );
    rFont.SetPitch      ( rFontItem.GetPitch() );
    rFont.SetWeight     ( (FontWeight)rWeightItem.GetValue() );
    rFont.SetItalic     ( (FontItalic)rPostureItem.GetValue() );
}

#define SETONALLFONTS( MethodName, Value )                  \
rFont.MethodName( Value );                                  \
rCJKFont.MethodName( Value );                               \
rCTLFont.MethodName( Value );

void AutoFmtPreview::MakeFonts( sal_uInt8 nIndex, Font& rFont, Font& rCJKFont, Font& rCTLFont )
{
    const SwBoxAutoFmt& rBoxFmt = aCurData.GetBoxFmt( nIndex );

    rFont = rCJKFont = rCTLFont = GetFont();
    Size aFontSize( rFont.GetSize().Width(), 10 );

    lcl_SetFontProperties( rFont, rBoxFmt.GetFont(), rBoxFmt.GetWeight(), rBoxFmt.GetPosture() );
    lcl_SetFontProperties( rCJKFont, rBoxFmt.GetCJKFont(), rBoxFmt.GetCJKWeight(), rBoxFmt.GetCJKPosture() );
    lcl_SetFontProperties( rCTLFont, rBoxFmt.GetCTLFont(), rBoxFmt.GetCTLWeight(), rBoxFmt.GetCTLPosture() );

    SETONALLFONTS( SetUnderline,    (FontUnderline)rBoxFmt.GetUnderline().GetValue() );
    SETONALLFONTS( SetOverline,     (FontUnderline)rBoxFmt.GetOverline().GetValue() );
    SETONALLFONTS( SetStrikeout,    (FontStrikeout)rBoxFmt.GetCrossedOut().GetValue() );
    SETONALLFONTS( SetOutline,      rBoxFmt.GetContour().GetValue() );
    SETONALLFONTS( SetShadow,       rBoxFmt.GetShadowed().GetValue() );
    SETONALLFONTS( SetColor,        rBoxFmt.GetColor().GetValue() );
    SETONALLFONTS( SetSize,         aFontSize );
    SETONALLFONTS( SetTransparent,  sal_True );
}

//------------------------------------------------------------------------

sal_uInt8 AutoFmtPreview::GetFormatIndex( size_t nCol, size_t nRow ) const
{
    static const sal_uInt8 pnFmtMap[] =
    {
        0,  1,  2,  1,  3,
        4,  5,  6,  5,  7,
        8,  9,  10, 9,  11,
        4,  5,  6,  5,  7,
        12, 13, 14, 13, 15
    };
    return pnFmtMap[ maArray.GetCellIndex( nCol, nRow, mbRTL ) ];
}

const SvxBoxItem& AutoFmtPreview::GetBoxItem( size_t nCol, size_t nRow ) const
{
    return aCurData.GetBoxFmt( GetFormatIndex( nCol, nRow ) ).GetBox();
}

//------------------------------------------------------------------------

void AutoFmtPreview::DrawString( size_t nCol, size_t nRow )
{
    //------------------------
    // Ausgabe des Zelltextes:
    //------------------------
    sal_uLong   nNum;
    double  nVal;
    String cellString;
    sal_uInt8    nIndex = static_cast< sal_uInt8 >( maArray.GetCellIndex( nCol, nRow, mbRTL ) );

    switch( nIndex )
    {
        case  1: cellString = aStrJan;          break;
        case  2: cellString = aStrFeb;          break;
        case  3: cellString = aStrMar;          break;
        case  5: cellString = aStrNorth;        break;
        case 10: cellString = aStrMid;          break;
        case 15: cellString = aStrSouth;        break;
        case  4:
        case 20: cellString = aStrSum;          break;

        case  6:
        case  8:
        case 16:
        case 18:    nVal = nIndex;
                    nNum = 5;
                    goto MAKENUMSTR;
        case 17:
        case  7:    nVal = nIndex;
                    nNum = 6;
                    goto MAKENUMSTR;
        case 11:
        case 12:
        case 13:    nVal = nIndex;
                    nNum = 12 == nIndex ? 10 : 9;
                    goto MAKENUMSTR;

        case  9:    nVal = 21; nNum = 7;    goto MAKENUMSTR;
        case 14:    nVal = 36; nNum = 11;   goto MAKENUMSTR;
        case 19:    nVal = 51; nNum = 7;    goto MAKENUMSTR;
        case 21:    nVal = 33; nNum = 13;   goto MAKENUMSTR;
        case 22:    nVal = 36; nNum = 14;   goto MAKENUMSTR;
        case 23:    nVal = 39; nNum = 13;   goto MAKENUMSTR;
        case 24:    nVal = 108; nNum = 15;  goto MAKENUMSTR;
MAKENUMSTR:
            if( aCurData.IsValueFormat() )
            {
                String sFmt; LanguageType eLng, eSys;
                aCurData.GetBoxFmt( (sal_uInt8)nNum ).GetValueFormat( sFmt, eLng, eSys );

                short nType;
                bool bNew;
                xub_StrLen nCheckPos;
                sal_uInt32 nKey = pNumFmt->GetIndexPuttingAndConverting( sFmt, eLng,
                        eSys, nType, bNew, nCheckPos);
                Color* pDummy;
                pNumFmt->GetOutputString( nVal, nKey, cellString, &pDummy );
            }
            else
                cellString = String::CreateFromInt32((sal_Int32)nVal);
            break;

    }

    if( cellString.Len() )
    {
        Size                aStrSize;
        sal_uInt8                nFmtIndex       = GetFormatIndex( nCol, nRow );
        Rectangle           cellRect        = maArray.GetCellRect( nCol, nRow );
        Point               aPos            = cellRect.TopLeft();
        sal_uInt16              nRightX         = 0;

        Size theMaxStrSize( cellRect.GetWidth() - FRAME_OFFSET,
                            cellRect.GetHeight() - FRAME_OFFSET );
        if( aCurData.IsFont() )
        {
            Font aFont, aCJKFont, aCTLFont;
            MakeFonts( nFmtIndex, aFont, aCJKFont, aCTLFont );
            aScriptedText.SetFonts( &aFont, &aCJKFont, &aCTLFont );
        }
        else
            aScriptedText.SetDefaultFont();

        aScriptedText.SetText( cellString, m_xBreak );
        aStrSize = aScriptedText.GetTextSize();

        if( aCurData.IsFont() &&
            theMaxStrSize.Height() < aStrSize.Height() )
        {
                // wenn der String in diesem Font nicht
                // in die Zelle passt, wird wieder der
                // Standard-Font genommen:
                aScriptedText.SetDefaultFont();
                aStrSize = aScriptedText.GetTextSize();
        }

        while( theMaxStrSize.Width() <= aStrSize.Width() &&
                cellString.Len() > 1 )
        {
            cellString.Erase( cellString.Len() - 1 );
            aScriptedText.SetText( cellString, m_xBreak );
            aStrSize = aScriptedText.GetTextSize();
        }

        nRightX  = (sal_uInt16)(  cellRect.GetWidth()
                                - aStrSize.Width()
                                - FRAME_OFFSET );

        //-----------------------------
        // vertikal (immer zentrieren):
        //-----------------------------
        aPos.Y() += (nRowHeight - (sal_uInt16)aStrSize.Height()) / 2;

        //-----------
        // horizontal
        //-----------
        if( mbRTL )
            aPos.X() += nRightX;
        else if (aCurData.IsJustify())
        {
            sal_uInt16 nHorPos = (sal_uInt16)
                    ((cellRect.GetWidth()-aStrSize.Width())/2);
            const SvxAdjustItem& rAdj = aCurData.GetBoxFmt(nFmtIndex).GetAdjust();
            switch ( rAdj.GetAdjust() )
            {
                case SVX_ADJUST_LEFT:
                    aPos.X() += FRAME_OFFSET;
                    break;
                case SVX_ADJUST_RIGHT:
                    aPos.X() += nRightX;
                    break;
                default:
                    aPos.X() += nHorPos;
                    break;
            }
        }
        else
        {
            //---------------------
            // Standardausrichtung:
            //---------------------
            if ( (nCol == 0) || (nIndex == 4) )
            {
                // Text-Label links oder Summe linksbuendig
                aPos.X() += FRAME_OFFSET;
            }
            else
            {
                    // Zahlen/Datum rechtsbuendig
                aPos.X() += nRightX;
            }
        }

        //-------------------------------
        aScriptedText.DrawText( aPos );
        //-------------------------------
    }
}

#undef FRAME_OFFSET

//------------------------------------------------------------------------

void AutoFmtPreview::DrawStrings()
{
    for( size_t nRow = 0; nRow < 5; ++nRow )
        for( size_t nCol = 0; nCol < 5; ++nCol )
            DrawString( nCol, nRow );
}

//------------------------------------------------------------------------


void AutoFmtPreview::DrawBackground()
{
    for( size_t nRow = 0; nRow < 5; ++nRow )
    {
        for( size_t nCol = 0; nCol < 5; ++nCol )
        {
            SvxBrushItem aBrushItem( aCurData.GetBoxFmt( GetFormatIndex( nCol, nRow ) ).GetBackground() );

            aVD.Push( PUSH_LINECOLOR | PUSH_FILLCOLOR );
            aVD.SetLineColor();
            aVD.SetFillColor( aBrushItem.GetColor() );
            aVD.DrawRect( maArray.GetCellRect( nCol, nRow ) );
            aVD.Pop();
        }
    }
}

//------------------------------------------------------------------------


void AutoFmtPreview::PaintCells()
{
    // 1) background
    if ( aCurData.IsBackground() )
        DrawBackground();

    // 2) values
    DrawStrings();

    // 3) border
    if ( aCurData.IsFrame() )
        maArray.DrawArray( aVD );
}

//------------------------------------------------------------------------


void AutoFmtPreview::Init()
{
    SetBorderStyle( GetBorderStyle() | WINDOW_BORDER_MONO );
    maArray.Initialize( 5, 5 );
    maArray.SetUseDiagDoubleClipping( false );
    CalcCellArray( sal_False );
    CalcLineMap();
}

//------------------------------------------------------------------------


void AutoFmtPreview::CalcCellArray( sal_Bool _bFitWidth )
{
    maArray.SetXOffset( 2 );
    maArray.SetAllColWidths( _bFitWidth ? nDataColWidth2 : nDataColWidth1 );
    maArray.SetColWidth( 0, nLabelColWidth );
    maArray.SetColWidth( 4, nLabelColWidth );

    maArray.SetYOffset( 2 );
    maArray.SetAllRowHeights( nRowHeight );

    aPrvSize.Width() = maArray.GetWidth() + 4;
    aPrvSize.Height() = maArray.GetHeight() + 4;
}

//------------------------------------------------------------------------

inline void lclSetStyleFromBorder( svx::frame::Style& rStyle, const ::editeng::SvxBorderLine* pBorder )
{
    rStyle.Set( pBorder, 0.05, 5 );
}

void AutoFmtPreview::CalcLineMap()
{
    for( size_t nRow = 0; nRow < 5; ++nRow )
    {
        for( size_t nCol = 0; nCol < 5; ++nCol )
        {
            svx::frame::Style aStyle;

            const SvxBoxItem& rItem = GetBoxItem( nCol, nRow );
            lclSetStyleFromBorder( aStyle, rItem.GetLeft() );
            maArray.SetCellStyleLeft( nCol, nRow, aStyle );
            lclSetStyleFromBorder( aStyle, rItem.GetRight() );
            maArray.SetCellStyleRight( nCol, nRow, aStyle );
            lclSetStyleFromBorder( aStyle, rItem.GetTop() );
            maArray.SetCellStyleTop( nCol, nRow, aStyle );
            lclSetStyleFromBorder( aStyle, rItem.GetBottom() );
            maArray.SetCellStyleBottom( nCol, nRow, aStyle );

// FIXME - uncomment to draw diagonal borders
//            lclSetStyleFromBorder( aStyle, GetDiagItem( nCol, nRow, true ).GetLine() );
//            maArray.SetCellStyleTLBR( nCol, nRow, aStyle );
//            lclSetStyleFromBorder( aStyle, GetDiagItem( nCol, nRow, false ).GetLine() );
//            maArray.SetCellStyleBLTR( nCol, nRow, aStyle );
        }
    }
}

//------------------------------------------------------------------------


void AutoFmtPreview::NotifyChange( const SwTableAutoFmt& rNewData )
{
    aCurData  = rNewData;
    bFitWidth = aCurData.IsJustify();//sal_True;  //???
    CalcCellArray( bFitWidth );
    CalcLineMap();
    DoPaint( Rectangle( Point(0,0), GetSizePixel() ) );
}

//------------------------------------------------------------------------


void AutoFmtPreview::DoPaint( const Rectangle& /*rRect*/ )
{
    sal_uInt32 nOldDrawMode = aVD.GetDrawMode();
    if( GetSettings().GetStyleSettings().GetHighContrastMode() )
        aVD.SetDrawMode( DRAWMODE_SETTINGSLINE | DRAWMODE_SETTINGSFILL | DRAWMODE_SETTINGSTEXT | DRAWMODE_SETTINGSGRADIENT );

    Bitmap  thePreview;
    Point   aCenterPos;
    Size    theWndSize = GetSizePixel();
    Size    thePrevSize;
    Color   oldColor;
    Font    aFont;

    aFont = aVD.GetFont();
    aFont.SetTransparent( sal_True );

    aVD.SetFont          ( aFont );
    aVD.SetLineColor     ();
    const Color& rWinColor = GetSettings().GetStyleSettings().GetWindowColor();
    aVD.SetBackground    ( Wallpaper(rWinColor) );
    aVD.SetFillColor     ( rWinColor );
    aVD.SetOutputSizePixel  ( aPrvSize );

    //--------------------------------
    // Zellen auf virtual Device malen
    // und Ergebnis sichern
    //--------------------------------
    PaintCells();
    thePreview = aVD.GetBitmap( Point(0,0), aPrvSize );

    //--------------------------------------
    // Rahmen malen und Vorschau zentrieren:
    // (virtual Device fuer Fensterausgabe)
    //--------------------------------------
    aVD.SetOutputSizePixel( theWndSize );
    oldColor = aVD.GetLineColor();
    aVD.SetLineColor();
    aVD.DrawRect( Rectangle( Point(0,0), theWndSize ) );
    SetLineColor( oldColor );
    aCenterPos  = Point( (theWndSize.Width()  - aPrvSize.Width() ) / 2,
                         (theWndSize.Height() - aPrvSize.Height()) / 2 );
    aVD.DrawBitmap( aCenterPos, thePreview );

    //----------------------------
    // Ausgabe im Vorschaufenster:
    //----------------------------
    DrawBitmap( Point(0,0), aVD.GetBitmap( Point(0,0), theWndSize ) );

    aVD.SetDrawMode( nOldDrawMode );
}

//------------------------------------------------------------------------

void AutoFmtPreview::Paint( const Rectangle& rRect )
{
    DoPaint( rRect );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
