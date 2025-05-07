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


#include "printdlg.hxx"
#include "svdata.hxx"
#include "svids.hrc"
#include "jobset.h"

#include "vcl/print.hxx"
#include "vcl/dialog.hxx"
#include "vcl/button.hxx"
#include "vcl/wall.hxx"
#include "vcl/status.hxx"
#include "vcl/decoview.hxx"
#include "vcl/arrange.hxx"
#include "vcl/configsettings.hxx"
#include "vcl/help.hxx"
#include "vcl/decoview.hxx"
#include "vcl/svapp.hxx"
#include "vcl/unohelp.hxx"

#include "unotools/localedatawrapper.hxx"

#include "rtl/strbuf.hxx"

#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "com/sun/star/container/XNameAccess.hpp"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/awt/Size.hpp"

using namespace vcl;
using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::container;
using namespace com::sun::star::beans;

PrintDialog::PrintPreviewWindow::PrintPreviewWindow( Window* i_pParent, const ResId& i_rId )
    : Window( i_pParent, i_rId )
    , maOrigSize( 10, 10 )
    , maPageVDev( *this )
    , maToolTipString(VclResId( SV_PRINT_PRINTPREVIEW_TXT).toString())
    , mbGreyscale( false )
    , maHorzDim( this, WB_HORZ | WB_CENTER  )
    , maVertDim( this, WB_VERT | WB_VCENTER )
{
    SetPaintTransparent( sal_True );
    SetBackground();
    maPageVDev.SetBackground( Color( COL_WHITE ) );
    maHorzDim.Show();
    maVertDim.Show();

    maHorzDim.SetText( String( RTL_CONSTASCII_USTRINGPARAM( "2.0in" ) ) );
    maVertDim.SetText( String( RTL_CONSTASCII_USTRINGPARAM( "2.0in" ) ) );
}

PrintDialog::PrintPreviewWindow::~PrintPreviewWindow()
{
}

void PrintDialog::PrintPreviewWindow::DataChanged( const DataChangedEvent& i_rDCEvt )
{
    // react on settings changed
    if( i_rDCEvt.GetType() == DATACHANGED_SETTINGS )
    {
        maPageVDev.SetBackground( Color( COL_WHITE ) );
    }
    Window::DataChanged( i_rDCEvt );
}

void PrintDialog::PrintPreviewWindow::Resize()
{
    Size aNewSize( GetSizePixel() );
    long nTextHeight = maHorzDim.GetTextHeight();
    // leave small space for decoration
    aNewSize.Width() -= nTextHeight + 2;
    aNewSize.Height() -= nTextHeight + 2;
    Size aScaledSize;
    double fScale = 1.0;

    // #i106435# catch corner case of Size(0,0)
    Size aOrigSize( maOrigSize );
    if( aOrigSize.Width() < 1 )
        aOrigSize.Width() = aNewSize.Width();
    if( aOrigSize.Height() < 1 )
        aOrigSize.Height() = aNewSize.Height();
    if( aOrigSize.Width() > aOrigSize.Height() )
    {
        aScaledSize = Size( aNewSize.Width(), aNewSize.Width() * aOrigSize.Height() / aOrigSize.Width() );
        if( aScaledSize.Height() > aNewSize.Height() )
            fScale = double(aNewSize.Height())/double(aScaledSize.Height());
    }
    else
    {
        aScaledSize = Size( aNewSize.Height() * aOrigSize.Width() / aOrigSize.Height(), aNewSize.Height() );
        if( aScaledSize.Width() > aNewSize.Width() )
            fScale = double(aNewSize.Width())/double(aScaledSize.Width());
    }
    aScaledSize.Width() = long(aScaledSize.Width()*fScale);
    aScaledSize.Height() = long(aScaledSize.Height()*fScale);

    maPreviewSize = aScaledSize;

    // #i104784# if we render the page too small then rounding issues result in
    // layout artifacts looking really bad. So scale the page unto a device that is not
    // full page size but not too small either. This also results in much better visual
    // quality of the preview, e.g. when its height approaches the number of text lines
    // find a good scaling factor
    Size aPreviewMMSize( maPageVDev.PixelToLogic( aScaledSize, MapMode( MAP_100TH_MM ) ) );
    double fZoom = double(maOrigSize.Height())/double(aPreviewMMSize.Height());
    while( fZoom > 10 )
    {
        aScaledSize.Width() *= 2;
        aScaledSize.Height() *= 2;
        fZoom /= 2.0;
    }

    maPageVDev.SetOutputSizePixel( aScaledSize, sal_False );

    // position dimension lines
    Point aRef( nTextHeight + (aNewSize.Width() - maPreviewSize.Width())/2,
                nTextHeight + (aNewSize.Height() - maPreviewSize.Height())/2 );
    maHorzDim.SetPosSizePixel( Point( aRef.X(), aRef.Y() - nTextHeight ),
                               Size( maPreviewSize.Width(), nTextHeight ) );
    maVertDim.SetPosSizePixel( Point( aRef.X() - nTextHeight, aRef.Y() ),
                               Size( nTextHeight, maPreviewSize.Height() ) );

}

void PrintDialog::PrintPreviewWindow::Paint( const Rectangle& )
{
    long nTextHeight = maHorzDim.GetTextHeight();
    Size aSize( GetSizePixel() );
    Point aOffset( (aSize.Width()  - maPreviewSize.Width()  + nTextHeight) / 2 ,
                   (aSize.Height() - maPreviewSize.Height() + nTextHeight) / 2 );

    if( !maReplacementString.isEmpty() )
    {
        // replacement is active
        Push();
        Font aFont( GetSettings().GetStyleSettings().GetLabelFont() );
        SetZoomedPointFont( aFont );
        Rectangle aTextRect( aOffset + Point( 2, 2 ),
            Size( maPreviewSize.Width() - 4, maPreviewSize.Height() - 4 ) );
        DrawText( aTextRect, maReplacementString,
                  TEXT_DRAW_CENTER | TEXT_DRAW_VCENTER | TEXT_DRAW_WORDBREAK | TEXT_DRAW_MULTILINE
                 );
        Pop();
    }
    else
    {
        GDIMetaFile aMtf( maMtf );

        Size aVDevSize( maPageVDev.GetOutputSizePixel() );
        const Size aLogicSize( maPageVDev.PixelToLogic( aVDevSize, MapMode( MAP_100TH_MM ) ) );
        Size aOrigSize( maOrigSize );
        if( aOrigSize.Width() < 1 )
            aOrigSize.Width() = aLogicSize.Width();
        if( aOrigSize.Height() < 1 )
            aOrigSize.Height() = aLogicSize.Height();
        double fScale = double(aLogicSize.Width())/double(aOrigSize.Width());


        maPageVDev.Erase();
        maPageVDev.Push();
        maPageVDev.SetMapMode( MAP_100TH_MM );
        sal_uLong nOldDrawMode = maPageVDev.GetDrawMode();
        if( mbGreyscale )
            maPageVDev.SetDrawMode( maPageVDev.GetDrawMode() |
                                    ( DRAWMODE_GRAYLINE | DRAWMODE_GRAYFILL | DRAWMODE_GRAYTEXT |
                                      DRAWMODE_GRAYBITMAP | DRAWMODE_GRAYGRADIENT ) );
        aMtf.WindStart();
        aMtf.Scale( fScale, fScale );
        aMtf.WindStart();
        aMtf.Play( &maPageVDev, Point( 0, 0 ), aLogicSize );
        maPageVDev.Pop();

        SetMapMode( MAP_PIXEL );
        maPageVDev.SetMapMode( MAP_PIXEL );
        DrawOutDev( aOffset, maPreviewSize, Point( 0, 0 ), aVDevSize, maPageVDev );
        maPageVDev.SetDrawMode( nOldDrawMode );
    }

    Rectangle aFrameRect( aOffset + Point( -1, -1 ),
        Size( maPreviewSize.Width() + 2, maPreviewSize.Height() + 2 ) );
    DecorationView aVw( this );
    aVw.DrawFrame( aFrameRect, FRAME_DRAW_GROUP );
}

void PrintDialog::PrintPreviewWindow::Command( const CommandEvent& rEvt )
{
    if( rEvt.GetCommand() == COMMAND_WHEEL )
    {
        const CommandWheelData* pWheelData = rEvt.GetWheelData();
        PrintDialog* pDlg = dynamic_cast<PrintDialog*>(GetParent());
        if( pDlg )
        {
            if( pWheelData->GetDelta() > 0 )
                pDlg->previewForward();
            else if( pWheelData->GetDelta() < 0 )
                pDlg->previewBackward();
            /*
            else
                huh ?
            */
        }
    }
}

void PrintDialog::PrintPreviewWindow::setPreview( const GDIMetaFile& i_rNewPreview,
                                                  const Size& i_rOrigSize,
                                                  const rtl::OUString& i_rPaperName,
                                                  const rtl::OUString& i_rReplacement,
                                                  sal_Int32 i_nDPIX,
                                                  sal_Int32 i_nDPIY,
                                                  bool i_bGreyscale
                                                 )
{
    rtl::OUStringBuffer aBuf( 256 );
    aBuf.append( maToolTipString );
    SetQuickHelpText( aBuf.makeStringAndClear() );
    maMtf = i_rNewPreview;

    maOrigSize = i_rOrigSize;
    maReplacementString = i_rReplacement;
    mbGreyscale = i_bGreyscale;
    maPageVDev.SetReferenceDevice( i_nDPIX, i_nDPIY );
    maPageVDev.EnableOutput( sal_True );

    // use correct measurements
    const LocaleDataWrapper& rLocWrap( GetSettings().GetLocaleDataWrapper() );
    MapUnit eUnit = MAP_MM;
    int nDigits = 0;
    if( rLocWrap.getMeasurementSystemEnum() == MEASURE_US )
    {
        eUnit = MAP_100TH_INCH;
        nDigits = 2;
    }
    Size aLogicPaperSize( LogicToLogic( i_rOrigSize, MapMode( MAP_100TH_MM ), MapMode( eUnit ) ) );
    String aNumText( rLocWrap.getNum( aLogicPaperSize.Width(), nDigits ) );
    aBuf.append( aNumText );
    aBuf.appendAscii( eUnit == MAP_MM ? "mm" : "in" );
    if( !i_rPaperName.isEmpty() )
    {
        aBuf.appendAscii( " (" );
        aBuf.append( i_rPaperName );
        aBuf.append( sal_Unicode(')') );
    }
    maHorzDim.SetText( aBuf.makeStringAndClear() );

    aNumText = rLocWrap.getNum( aLogicPaperSize.Height(), nDigits );
    aBuf.append( aNumText );
    aBuf.appendAscii( eUnit == MAP_MM ? "mm" : "in" );
    maVertDim.SetText( aBuf.makeStringAndClear() );

    Resize();
    Invalidate();
}

PrintDialog::ShowNupOrderWindow::ShowNupOrderWindow( Window* i_pParent )
    : Window( i_pParent, WB_NOBORDER )
    , mnOrderMode( 0 )
    , mnRows( 1 )
    , mnColumns( 1 )
{
    ImplInitSettings();
}

PrintDialog::ShowNupOrderWindow::~ShowNupOrderWindow()
{
}

void PrintDialog::ShowNupOrderWindow::ImplInitSettings()
{
    SetBackground( Wallpaper( GetSettings().GetStyleSettings().GetFieldColor() ) );
}

void PrintDialog::ShowNupOrderWindow::Paint( const Rectangle& i_rRect )
{
    Window::Paint( i_rRect );
    SetMapMode( MAP_PIXEL );
    SetTextColor( GetSettings().GetStyleSettings().GetFieldTextColor() );

    int nPages = mnRows * mnColumns;
    Font aFont( GetSettings().GetStyleSettings().GetFieldFont() );
    aFont.SetSize( Size( 0, 24 ) );
    SetFont( aFont );
    Size aSampleTextSize( GetTextWidth( rtl::OUString::valueOf( sal_Int32(nPages+1) ) ), GetTextHeight() );

    Size aOutSize( GetOutputSizePixel() );
    Size aSubSize( aOutSize.Width() / mnColumns, aOutSize.Height() / mnRows );
    // calculate font size: shrink the sample text so it fits
    double fX = double(aSubSize.Width())/double(aSampleTextSize.Width());
    double fY = double(aSubSize.Height())/double(aSampleTextSize.Height());
    double fScale = (fX < fY) ? fX : fY;
    long nFontHeight = long(24.0*fScale) - 3;
    if( nFontHeight < 5 )
        nFontHeight = 5;
    aFont.SetSize( Size( 0, nFontHeight ) );
    SetFont( aFont );
    long nTextHeight = GetTextHeight();
    for( int i = 0; i < nPages; i++ )
    {
        rtl::OUString aPageText( rtl::OUString::valueOf( sal_Int32(i+1) ) );
        int nX = 0, nY = 0;
        switch( mnOrderMode )
        {
        case SV_PRINT_PRT_NUP_ORDER_LRTB:
            nX = (i % mnColumns); nY = (i / mnColumns);
            break;
        case SV_PRINT_PRT_NUP_ORDER_TBLR:
            nX = (i / mnRows); nY = (i % mnRows);
            break;
        case SV_PRINT_PRT_NUP_ORDER_RLTB:
            nX = mnColumns - 1 - (i % mnColumns); nY = (i / mnColumns);
            break;
        case SV_PRINT_PRT_NUP_ORDER_TBRL:
            nX = mnColumns - 1 - (i / mnRows); nY = (i % mnRows);
            break;
        }
        Size aTextSize( GetTextWidth( aPageText ), nTextHeight );
        int nDeltaX = (aSubSize.Width() - aTextSize.Width()) / 2;
        int nDeltaY = (aSubSize.Height() - aTextSize.Height()) / 2;
        DrawText( Point( nX * aSubSize.Width() + nDeltaX,
                         nY * aSubSize.Height() + nDeltaY ),
                  aPageText );
    }
    DecorationView aVw( this );
    aVw.DrawFrame( Rectangle( Point( 0, 0), aOutSize ), FRAME_DRAW_GROUP );
}

PrintDialog::NUpTabPage::NUpTabPage( Window* i_pParent, const ResId& rResId )
    : TabPage( i_pParent, rResId )
    , maNupLine( this, VclResId( SV_PRINT_PRT_NUP_LAYOUT_FL ) )
    , maPagesBtn( this, VclResId( SV_PRINT_PRT_NUP_PAGES_BTN ) )
    , maBrochureBtn( this, VclResId( SV_PRINT_PRT_NUP_BROCHURE_BTN ) )
    , maPagesBoxTitleTxt( this, 0 )
    , maNupPagesBox( this, VclResId( SV_PRINT_PRT_NUP_PAGES_BOX ) )
    , maNupNumPagesTxt( this, VclResId( SV_PRINT_PRT_NUP_NUM_PAGES_TXT ) )
    , maNupColEdt( this, VclResId( SV_PRINT_PRT_NUP_COLS_EDT ) )
    , maNupTimesTxt( this, VclResId( SV_PRINT_PRT_NUP_TIMES_TXT ) )
    , maNupRowsEdt( this, VclResId( SV_PRINT_PRT_NUP_ROWS_EDT ) )
    , maPageMarginTxt1( this, VclResId( SV_PRINT_PRT_NUP_MARGINS_PAGES_1_TXT ) )
    , maPageMarginEdt( this, VclResId( SV_PRINT_PRT_NUP_MARGINS_PAGES_EDT ) )
    , maPageMarginTxt2( this, VclResId( SV_PRINT_PRT_NUP_MARGINS_PAGES_2_TXT ) )
    , maSheetMarginTxt1( this, VclResId( SV_PRINT_PRT_NUP_MARGINS_SHEET_1_TXT ) )
    , maSheetMarginEdt( this, VclResId( SV_PRINT_PRT_NUP_MARGINS_SHEET_EDT ) )
    , maSheetMarginTxt2( this, VclResId( SV_PRINT_PRT_NUP_MARGINS_SHEET_2_TXT ) )
    , maNupOrientationTxt( this, VclResId( SV_PRINT_PRT_NUP_ORIENTATION_TXT ) )
    , maNupOrientationBox( this, VclResId( SV_PRINT_PRT_NUP_ORIENTATION_BOX ) )
    , maNupOrderTxt( this, VclResId( SV_PRINT_PRT_NUP_ORDER_TXT ) )
    , maNupOrderBox( this, VclResId( SV_PRINT_PRT_NUP_ORDER_BOX ) )
    , maNupOrderWin( this )
    , maBorderCB( this, VclResId( SV_PRINT_PRT_NUP_BORDER_CB ) )
{
    FreeResource();

    maNupOrderWin.Show();
    maPagesBtn.Check( sal_True );
    maBrochureBtn.Show( sal_False );

    // setup field units for metric fields
    const LocaleDataWrapper& rLocWrap( maPageMarginEdt.GetLocaleDataWrapper() );
    FieldUnit eUnit = FUNIT_MM;
    sal_uInt16 nDigits = 0;
    if( rLocWrap.getMeasurementSystemEnum() == MEASURE_US )
    {
        eUnit = FUNIT_INCH;
        nDigits = 2;
    }
    // set units
    maPageMarginEdt.SetUnit( eUnit );
    maSheetMarginEdt.SetUnit( eUnit );

    // set precision
    maPageMarginEdt.SetDecimalDigits( nDigits );
    maSheetMarginEdt.SetDecimalDigits( nDigits );

    setupLayout();
}

PrintDialog::NUpTabPage::~NUpTabPage()
{
}

void PrintDialog::NUpTabPage::enableNupControls( bool bEnable )
{
    maNupPagesBox.Enable( sal_True );
    maNupNumPagesTxt.Enable( bEnable );
    maNupColEdt.Enable( bEnable );
    maNupTimesTxt.Enable( bEnable );
    maNupRowsEdt.Enable( bEnable );
    maPageMarginTxt1.Enable( bEnable );
    maPageMarginEdt.Enable( bEnable );
    maPageMarginTxt2.Enable( bEnable );
    maSheetMarginTxt1.Enable( bEnable );
    maSheetMarginEdt.Enable( bEnable );
    maSheetMarginTxt2.Enable( bEnable );
    maNupOrientationTxt.Enable( bEnable );
    maNupOrientationBox.Enable( bEnable );
    maNupOrderTxt.Enable( bEnable );
    maNupOrderBox.Enable( bEnable );
    maNupOrderWin.Enable( bEnable );
    maBorderCB.Enable( bEnable );
}

void PrintDialog::NUpTabPage::showAdvancedControls( bool i_bShow )
{
    maNupNumPagesTxt.Show( i_bShow );
    maNupColEdt.Show( i_bShow );
    maNupTimesTxt.Show( i_bShow );
    maNupRowsEdt.Show( i_bShow );
    maPageMarginTxt1.Show( i_bShow );
    maPageMarginEdt.Show( i_bShow );
    maPageMarginTxt2.Show( i_bShow );
    maSheetMarginTxt1.Show( i_bShow );
    maSheetMarginEdt.Show( i_bShow );
    maSheetMarginTxt2.Show( i_bShow );
    maNupOrientationTxt.Show( i_bShow );
    maNupOrientationBox.Show( i_bShow );
    getLayout()->resize();
}

void PrintDialog::NUpTabPage::setupLayout()
{
    boost::shared_ptr<vcl::RowOrColumn> xLayout =
        boost::dynamic_pointer_cast<vcl::RowOrColumn>( getLayout() );
    Size aBorder( LogicToPixel( Size( 6, 6 ), MapMode( MAP_APPFONT ) ) );
    /*  According to OOo style guide, the horizontal indentation of child
        elements to their parent element should always be 6 map units. */
    long nIndent = aBorder.Width();

    xLayout->addWindow( &maNupLine );
    boost::shared_ptr< vcl::RowOrColumn > xRow( new vcl::RowOrColumn( xLayout.get(), false ) );
    xLayout->addChild( xRow );
    boost::shared_ptr< vcl::Indenter > xIndent( new vcl::Indenter( xRow.get() ) );
    xRow->addChild( xIndent );

    boost::shared_ptr< vcl::RowOrColumn > xShowNupCol( new vcl::RowOrColumn( xRow.get() ) );
    xRow->addChild( xShowNupCol, -1 );
    xShowNupCol->setMinimumSize( xShowNupCol->addWindow( &maNupOrderWin ), Size( 70, 70 ) );
    boost::shared_ptr< vcl::Spacer > xSpacer( new vcl::Spacer( xShowNupCol.get() ) );
    xShowNupCol->addChild( xSpacer );

    boost::shared_ptr< vcl::LabelColumn > xMainCol( new vcl::LabelColumn( xIndent.get() ) );
    xIndent->setChild( xMainCol );

    size_t nPagesIndex = xMainCol->addRow( &maPagesBtn, &maNupPagesBox );
    mxPagesBtnLabel = boost::dynamic_pointer_cast<vcl::LabeledElement>( xMainCol->getChild( nPagesIndex ) );

    xRow.reset( new vcl::RowOrColumn( xMainCol.get(), false ) );
    xMainCol->addRow( &maNupNumPagesTxt, xRow, nIndent );
    xRow->addWindow( &maNupColEdt );
    xRow->addWindow( &maNupTimesTxt );
    xRow->addWindow( &maNupRowsEdt );

    boost::shared_ptr< vcl::LabeledElement > xLab( new vcl::LabeledElement( xMainCol.get(), 2 ) );
    xLab->setLabel( &maPageMarginEdt );
    xLab->setElement( &maPageMarginTxt2 );
    xMainCol->addRow( &maPageMarginTxt1, xLab, nIndent );

    xLab.reset( new vcl::LabeledElement( xMainCol.get(), 2 ) );
    xLab->setLabel( &maSheetMarginEdt );
    xLab->setElement( &maSheetMarginTxt2 );
    xMainCol->addRow( &maSheetMarginTxt1, xLab, nIndent );

    xMainCol->addRow( &maNupOrientationTxt, &maNupOrientationBox, nIndent );
    xMainCol->addRow( &maNupOrderTxt, &maNupOrderBox, nIndent );
    xMainCol->setBorders( xMainCol->addWindow( &maBorderCB ), nIndent, 0, 0, 0 );

    xSpacer.reset( new vcl::Spacer( xMainCol.get(), 0, Size( 10, WindowArranger::getDefaultBorder() ) ) );
    xMainCol->addChild( xSpacer );

    xRow.reset( new vcl::RowOrColumn( xMainCol.get(), false ) );
    xMainCol->addRow( &maBrochureBtn, xRow );
    // remember brochure row for dependencies
    mxBrochureDep = xRow;

    // initially advanced controls are not shown, rows=columns=1
    showAdvancedControls( false );
}

void PrintDialog::NUpTabPage::initFromMultiPageSetup( const vcl::PrinterController::MultiPageSetup& i_rMPS )
{
    maSheetMarginEdt.SetValue( maSheetMarginEdt.Normalize( i_rMPS.nLeftMargin ), FUNIT_100TH_MM );
    maPageMarginEdt.SetValue( maPageMarginEdt.Normalize( i_rMPS.nHorizontalSpacing ), FUNIT_100TH_MM );
    maBorderCB.Check( i_rMPS.bDrawBorder );
    maNupRowsEdt.SetValue( i_rMPS.nRows );
    maNupColEdt.SetValue( i_rMPS.nColumns );
}

void PrintDialog::NUpTabPage::readFromSettings()
{
}

void PrintDialog::NUpTabPage::storeToSettings()
{
}

PrintDialog::JobTabPage::JobTabPage( Window* i_pParent, const ResId& rResId )
    : TabPage( i_pParent, rResId )
    , maPrinterFL( this, VclResId( SV_PRINT_PRINTERS_FL ) )
    , maPrinters( this, VclResId( SV_PRINT_PRINTERS ) )
    , maDetailsBtn( this, VclResId( SV_PRINT_DETAILS_BTN ) )
    , maStatusLabel( this, VclResId( SV_PRINT_STATUS_TXT ) )
    , maStatusTxt( this, 0 )
    , maLocationLabel( this, VclResId( SV_PRINT_LOCATION_TXT ) )
    , maLocationTxt( this, 0 )
    , maCommentLabel( this, VclResId( SV_PRINT_COMMENT_TXT ) )
    , maCommentTxt( this, 0 )
    , maSetupButton( this, VclResId( SV_PRINT_PRT_SETUP ) )
    , maCopies( this, VclResId( SV_PRINT_COPIES ) )
    , maCopySpacer( this, WB_VERT )
    , maCopyCount( this, VclResId( SV_PRINT_COPYCOUNT ) )
    , maCopyCountField( this, VclResId( SV_PRINT_COPYCOUNT_FIELD ) )
    , maCollateBox( this, VclResId( SV_PRINT_COLLATE ) )
    , maCollateImage( this, VclResId( SV_PRINT_COLLATE_IMAGE ) )
    , maReverseOrderBox( this, VclResId( SV_PRINT_OPT_REVERSE ) )
    , maCollateImg( VclResId( SV_PRINT_COLLATE_IMG ) )
    , maNoCollateImg( VclResId( SV_PRINT_NOCOLLATE_IMG ) )
    , mnCollateUIMode( 0 )
{
    FreeResource();


    maCopySpacer.Show();
    maStatusTxt.Show();
    maCommentTxt.Show();
    maLocationTxt.Show();

    setupLayout();
}

PrintDialog::JobTabPage::~JobTabPage()
{
}

void PrintDialog::JobTabPage::setupLayout()
{
    // HACK: this is not a dropdown box, but the dropdown line count
    // sets the results of GetOptimalSize in a normal ListBox
    maPrinters.SetDropDownLineCount( 4 );

    boost::shared_ptr<vcl::RowOrColumn> xLayout =
        boost::dynamic_pointer_cast<vcl::RowOrColumn>( getLayout() );

    // add printer fixed line
    xLayout->addWindow( &maPrinterFL );
    // add print LB
    xLayout->addWindow( &maPrinters, 3 );

    // create a row for details button/text and properties button
    boost::shared_ptr< vcl::RowOrColumn > xDetRow( new vcl::RowOrColumn( xLayout.get(), false ) );
    xLayout->addChild( xDetRow );
    xDetRow->addWindow( &maDetailsBtn );
    xDetRow->addChild( new vcl::Spacer( xDetRow.get(), 2 ) );
    xDetRow->addWindow( &maSetupButton );

    // create an indent for details
    boost::shared_ptr< vcl::Indenter > xIndent( new vcl::Indenter( xLayout.get() ) );
    xLayout->addChild( xIndent );
    // remember details controls
    mxDetails = xIndent;
    // create a column for the details
    boost::shared_ptr< vcl::LabelColumn > xLabelCol( new vcl::LabelColumn( xIndent.get() ) );
    xIndent->setChild( xLabelCol );
    xLabelCol->addRow( &maStatusLabel, &maStatusTxt );
    xLabelCol->addRow( &maLocationLabel, &maLocationTxt );
    xLabelCol->addRow( &maCommentLabel, &maCommentTxt );

    // add print range and copies columns
    xLayout->addWindow( &maCopies );
    boost::shared_ptr< vcl::RowOrColumn > xRangeRow( new vcl::RowOrColumn( xLayout.get(), false ) );
    xLayout->addChild( xRangeRow );

    // create print range and add to range row
    mxPrintRange.reset( new vcl::RowOrColumn( xRangeRow.get() ) );
    xRangeRow->addChild( mxPrintRange );
    xRangeRow->addWindow( &maCopySpacer );

    boost::shared_ptr< vcl::RowOrColumn > xCopyCollateCol( new vcl::RowOrColumn( xRangeRow.get() ) );
    xRangeRow->addChild( xCopyCollateCol );

    // add copies row to copy/collate column
    boost::shared_ptr< vcl::LabeledElement > xCopiesRow( new vcl::LabeledElement( xCopyCollateCol.get(), 2 ) );
    xCopyCollateCol->addChild( xCopiesRow );
    xCopiesRow->setLabel( &maCopyCount );
    xCopiesRow->setElement( &maCopyCountField );
    boost::shared_ptr< vcl::LabeledElement > xCollateRow( new vcl::LabeledElement( xCopyCollateCol.get(), 2 ) );
    xCopyCollateCol->addChild( xCollateRow );
    xCollateRow->setLabel( &maCollateBox );
    xCollateRow->setElement( &maCollateImage );

    // maDetailsBtn.SetStyle( maDetailsBtn.GetStyle() | (WB_SMALLSTYLE | WB_BEVELBUTTON) );
    mxDetails->show( false, false );
}

void PrintDialog::JobTabPage::readFromSettings()
{
    SettingsConfigItem* pItem = SettingsConfigItem::get();
    rtl::OUString aValue;

    aValue = pItem->getValue( rtl::OUString( "PrintDialog"  ),
                              rtl::OUString( "CollateBox"  ) );
    if( aValue.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("alwaysoff")) )
    {
        mnCollateUIMode = 1;
        maCollateBox.Check( sal_False );
        maCollateBox.Enable( sal_False );
    }
    else
    {
        mnCollateUIMode = 0;
        aValue = pItem->getValue( rtl::OUString( "PrintDialog"  ),
                                  rtl::OUString( "Collate"  ) );
        maCollateBox.Check( aValue.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("true")) );
    }
    Resize();
}

void PrintDialog::JobTabPage::storeToSettings()
{
    SettingsConfigItem* pItem = SettingsConfigItem::get();
    pItem->setValue( rtl::OUString( "PrintDialog"  ),
                     rtl::OUString( "CopyCount"  ),
                     maCopyCountField.GetText() );
    pItem->setValue( rtl::OUString( "PrintDialog"  ),
                     rtl::OUString( "Collate"  ),
                     maCollateBox.IsChecked() ? rtl::OUString("true") :
                                                rtl::OUString("false") );
}

PrintDialog::OutputOptPage::OutputOptPage( Window* i_pParent, const ResId& i_rResId )
    : TabPage( i_pParent, i_rResId )
    , maOptionsLine( this, VclResId( SV_PRINT_OPT_PRINT_FL ) )
    , maToFileBox( this, VclResId( SV_PRINT_OPT_TOFILE ) )
    , maCollateSingleJobsBox( this, VclResId( SV_PRINT_OPT_SINGLEJOBS ) )
{
    FreeResource();

    setupLayout();
}

PrintDialog::OutputOptPage::~OutputOptPage()
{
}

void PrintDialog::OutputOptPage::setupLayout()
{
    boost::shared_ptr<vcl::RowOrColumn> xLayout =
        boost::dynamic_pointer_cast<vcl::RowOrColumn>( getLayout() );

    xLayout->addWindow( &maOptionsLine );
    boost::shared_ptr<vcl::Indenter> xIndent( new vcl::Indenter( xLayout.get(), -1 ) );
    xLayout->addChild( xIndent );
    boost::shared_ptr<vcl::RowOrColumn> xCol( new vcl::RowOrColumn( xIndent.get() ) );
    xIndent->setChild( xCol );
    mxOptGroup = xCol;
    xCol->addWindow( &maToFileBox );
    xCol->addWindow( &maCollateSingleJobsBox );
}

void PrintDialog::OutputOptPage::readFromSettings()
{
    SettingsConfigItem* pItem = SettingsConfigItem::get();
    rtl::OUString aValue;
    aValue = pItem->getValue( rtl::OUString( "PrintDialog"  ),
                              rtl::OUString( "CollateSingleJobs"  ) );
    if ( aValue.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("true")) )
    {
        maCollateSingleJobsBox.Check( sal_True );
    }
    else
    {
        maCollateSingleJobsBox.Check( sal_False );
    }
    Resize();
}

void PrintDialog::OutputOptPage::storeToSettings()
{
    SettingsConfigItem* pItem = SettingsConfigItem::get();
    pItem->setValue( rtl::OUString( "PrintDialog"  ),
                     rtl::OUString( "ToFile"  ),
                     maToFileBox.IsChecked() ? rtl::OUString("true")
                                             : rtl::OUString("false") );
    pItem->setValue( rtl::OUString( "PrintDialog"  ),
                     rtl::OUString( "CollateSingleJobs"  ),
                     maCollateSingleJobsBox.IsChecked() ? rtl::OUString("true") :
                                                rtl::OUString("false") );
}

PrintDialog::PrintDialog( Window* i_pParent, const boost::shared_ptr<PrinterController>& i_rController )
    : ModalDialog( i_pParent, VclResId( SV_DLG_PRINT ) )
    , maTabCtrl( this, VclResId( SV_PRINT_TABCTRL ) )
    , maNUpPage( &maTabCtrl, VclResId( SV_PRINT_TAB_NUP ) )
    , maJobPage( &maTabCtrl, VclResId( SV_PRINT_TAB_JOB ) )
    , maOptionsPage( &maTabCtrl, VclResId( SV_PRINT_TAB_OPT ) )
    , maPreviewWindow( this, VclResId( SV_PRINT_PAGE_PREVIEW ) )
    , maPageEdit( this, VclResId( SV_PRINT_PAGE_EDIT ) )
    , maNumPagesText( this, VclResId( SV_PRINT_PAGE_TXT ) )
    , maBackwardBtn( this, VclResId( SV_PRINT_PAGE_BACKWARD ) )
    , maForwardBtn( this, VclResId( SV_PRINT_PAGE_FORWARD ) )
    , maButtonLine( this, VclResId( SV_PRINT_BUTTONLINE ) )
    , maOKButton( this, VclResId( SV_PRINT_OK ) )
    , maCancelButton( this, VclResId( SV_PRINT_CANCEL ) )
    , maHelpButton( this, VclResId( SV_PRINT_HELP ) )
    , maPController( i_rController )
    , maNoPageStr( VclResId( SV_PRINT_NOPAGES ).toString() )
    , mnCurPage( 0 )
    , mnCachedPages( 0 )
    , maPrintToFileText( VclResId( SV_PRINT_TOFILE_TXT ).toString() )
    , maDefPrtText( VclResId( SV_PRINT_DEFPRT_TXT ).toString() )
    , mbShowLayoutPage( sal_True )
{
    FreeResource();

    // save printbutton text, gets exchanged occasionally with print to file
    maPrintText = maOKButton.GetText();

    // setup preview controls
    maForwardBtn.SetStyle( maForwardBtn.GetStyle() | WB_BEVELBUTTON );
    maBackwardBtn.SetStyle( maBackwardBtn.GetStyle() | WB_BEVELBUTTON );

    // insert the job (general) tab page first
    maTabCtrl.InsertPage( SV_PRINT_TAB_JOB, maJobPage.GetText() );
    maTabCtrl.SetTabPage( SV_PRINT_TAB_JOB, &maJobPage );

    // set symbols on forward and backward button
    maBackwardBtn.SetSymbol( SYMBOL_PREV );
    maForwardBtn.SetSymbol( SYMBOL_NEXT );
    maBackwardBtn.ImplSetSmallSymbol( sal_True );
    maForwardBtn.ImplSetSmallSymbol( sal_True );

    maPageStr = maNumPagesText.GetText();

    // init reverse print
    maJobPage.maReverseOrderBox.Check( maPController->getReversePrint() );


    // fill printer listbox
    const std::vector< rtl::OUString >& rQueues( Printer::GetPrinterQueues() );
    for( std::vector< rtl::OUString >::const_iterator it = rQueues.begin();
         it != rQueues.end(); ++it )
    {
        maJobPage.maPrinters.InsertEntry( *it );
    }
    // select current printer
    if( maJobPage.maPrinters.GetEntryPos( maPController->getPrinter()->GetName() ) != LISTBOX_ENTRY_NOTFOUND )
    {
        maJobPage.maPrinters.SelectEntry( maPController->getPrinter()->GetName() );
    }
    else
    {
        // fall back to last printer
        SettingsConfigItem* pItem = SettingsConfigItem::get();
        String aValue( pItem->getValue( rtl::OUString( "PrintDialog"  ),
                                        rtl::OUString( "LastPrinter"  ) ) );
        if( maJobPage.maPrinters.GetEntryPos( aValue ) != LISTBOX_ENTRY_NOTFOUND )
        {
            maJobPage.maPrinters.SelectEntry( aValue );
            maPController->setPrinter( boost::shared_ptr<Printer>( new Printer( aValue ) ) );
        }
        else
        {
            // fall back to default printer
            maJobPage.maPrinters.SelectEntry( Printer::GetDefaultPrinterName() );
            maPController->setPrinter( boost::shared_ptr<Printer>( new Printer( Printer::GetDefaultPrinterName() ) ) );
        }
    }
    // not printing to file
    maPController->resetPrinterOptions( false );

    // get the first page
    preparePreview( true, true );

    // update the text fields for the printer
    updatePrinterText();

    // set a select handler
    maJobPage.maPrinters.SetSelectHdl( LINK( this, PrintDialog, SelectHdl ) );

    // setup sizes for N-Up
    Size aNupSize( maPController->getPrinter()->PixelToLogic(
                         maPController->getPrinter()->GetPaperSizePixel(), MapMode( MAP_100TH_MM ) ) );
    if( maPController->getPrinter()->GetOrientation() == ORIENTATION_LANDSCAPE )
    {
        maNupLandscapeSize = aNupSize;
        maNupPortraitSize = Size( aNupSize.Height(), aNupSize.Width() );
    }
    else
    {
        maNupPortraitSize = aNupSize;
        maNupLandscapeSize = Size( aNupSize.Height(), aNupSize.Width() );
    }
    maNUpPage.initFromMultiPageSetup( maPController->getMultipage() );


    // setup click handler on the various buttons
    maOKButton.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    #if OSL_DEBUG_LEVEL > 1
    maCancelButton.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    #endif
    maHelpButton.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    maForwardBtn.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    maBackwardBtn.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    maJobPage.maCollateBox.SetToggleHdl( LINK( this, PrintDialog, ClickHdl ) );
    maJobPage.maSetupButton.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    maJobPage.maDetailsBtn.SetToggleHdl( LINK( this, PrintDialog, ClickHdl ) );
    maNUpPage.maBorderCB.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    maOptionsPage.maToFileBox.SetToggleHdl( LINK( this, PrintDialog, ClickHdl ) );
    maJobPage.maReverseOrderBox.SetToggleHdl( LINK( this, PrintDialog, ClickHdl ) );
    maOptionsPage.maCollateSingleJobsBox.SetToggleHdl( LINK( this, PrintDialog, ClickHdl ) );
    maNUpPage.maPagesBtn.SetToggleHdl( LINK( this, PrintDialog, ClickHdl ) );

    // setup modify hdl
    maPageEdit.SetModifyHdl( LINK( this, PrintDialog, ModifyHdl ) );
    maJobPage.maCopyCountField.SetModifyHdl( LINK( this, PrintDialog, ModifyHdl ) );
    maNUpPage.maNupRowsEdt.SetModifyHdl( LINK( this, PrintDialog, ModifyHdl ) );
    maNUpPage.maNupColEdt.SetModifyHdl( LINK( this, PrintDialog, ModifyHdl ) );
    maNUpPage.maPageMarginEdt.SetModifyHdl( LINK( this, PrintDialog, ModifyHdl ) );
    maNUpPage.maSheetMarginEdt.SetModifyHdl( LINK( this, PrintDialog, ModifyHdl ) );

    // setup select hdl
    maNUpPage.maNupPagesBox.SetSelectHdl( LINK( this, PrintDialog, SelectHdl ) );
    maNUpPage.maNupOrientationBox.SetSelectHdl( LINK( this, PrintDialog, SelectHdl ) );
    maNUpPage.maNupOrderBox.SetSelectHdl( LINK( this, PrintDialog, SelectHdl ) );

    // setup the layout
    setupLayout();

    // setup optional UI options set by application
    setupOptionalUI();

    // set change handler for UI options
    maPController->setOptionChangeHdl( LINK( this, PrintDialog, UIOptionsChanged ) );

    // set min size pixel to current size
    Size aOutSize( GetOutputSizePixel() );
    SetMinOutputSizePixel( aOutSize );

    // if there is space enough, enlarge the preview so it gets roughly as
    // high as the tab control
    if( aOutSize.Width() < 768 )
    {
        Size aJobPageSize( getJobPageSize() );
        Size aTabSize( maTabCtrl.GetSizePixel() );
        if( aJobPageSize.Width() < 1 )
            aJobPageSize.Width() = aTabSize.Width();
        if( aJobPageSize.Height() < 1 )
            aJobPageSize.Height() = aTabSize.Height();
        long nOptPreviewWidth = aTabSize.Height() * aJobPageSize.Width() / aJobPageSize.Height();
        // add space for borders
        nOptPreviewWidth += 15;
        if( aOutSize.Width() - aTabSize.Width() < nOptPreviewWidth )
        {
            aOutSize.Width() = aTabSize.Width() + nOptPreviewWidth;
            if( aOutSize.Width() > 768 ) // don't enlarge the dialog too much
                aOutSize.Width() = 768;
            SetOutputSizePixel( aOutSize );
        }
    }

    // append further tab pages
    if( mbShowLayoutPage )
    {
        maTabCtrl.InsertPage( SV_PRINT_TAB_NUP, maNUpPage.GetText() );
        maTabCtrl.SetTabPage( SV_PRINT_TAB_NUP, &maNUpPage );
    }
    maTabCtrl.InsertPage( SV_PRINT_TAB_OPT, maOptionsPage.GetText() );
    maTabCtrl.SetTabPage( SV_PRINT_TAB_OPT, &maOptionsPage );

    // restore settings from last run
    readFromSettings();

    // setup dependencies
    checkControlDependencies();

    // set initial focus to "Number of copies"
    maJobPage.maCopyCountField.GrabFocus();
    maJobPage.maCopyCountField.SetSelection( Selection(0, 0xFFFF) );
}

PrintDialog::~PrintDialog()
{
    while( ! maControls.empty() )
    {
        delete maControls.front();
        maControls.pop_front();
    }
}

void PrintDialog::setupLayout()
{
    boost::shared_ptr<vcl::RowOrColumn> xLayout =
        boost::dynamic_pointer_cast<vcl::RowOrColumn>( getLayout() );
    xLayout->setOuterBorder( 0 );


    boost::shared_ptr< vcl::RowOrColumn > xPreviewAndTab( new vcl::RowOrColumn( xLayout.get(), false ) );
    size_t nIndex = xLayout->addChild( xPreviewAndTab, 5 );
    xLayout->setBorders( nIndex, -1, -1, -1, 0 );

    // setup column for preview and sub controls
    boost::shared_ptr< vcl::RowOrColumn > xPreview( new vcl::RowOrColumn( xPreviewAndTab.get() ) );
    xPreviewAndTab->addChild( xPreview, 5 );
    xPreview->addWindow( &maPreviewWindow, 5 );
    // get a row for the preview controls
    mxPreviewCtrls.reset( new vcl::RowOrColumn( xPreview.get(), false ) );
    nIndex = xPreview->addChild( mxPreviewCtrls );
    boost::shared_ptr< vcl::Spacer > xSpacer( new vcl::Spacer( mxPreviewCtrls.get(), 2 ) );
    mxPreviewCtrls->addChild( xSpacer );
    mxPreviewCtrls->addWindow( &maPageEdit );
    mxPreviewCtrls->addWindow( &maNumPagesText );
    xSpacer.reset( new vcl::Spacer( mxPreviewCtrls.get(), 2 ) );
    mxPreviewCtrls->addChild( xSpacer );
    mxPreviewCtrls->addWindow( &maBackwardBtn );
    mxPreviewCtrls->addWindow( &maForwardBtn );
    xSpacer.reset( new vcl::Spacer( mxPreviewCtrls.get(), 2 ) );
    mxPreviewCtrls->addChild( xSpacer );

    // continue with the tab ctrl
    xPreviewAndTab->addWindow( &maTabCtrl );

    // add the button line
    xLayout->addWindow( &maButtonLine );

    // add the row for the buttons
    boost::shared_ptr< vcl::RowOrColumn > xButtons( new vcl::RowOrColumn( xLayout.get(), false ) );
    nIndex = xLayout->addChild( xButtons );
    xLayout->setBorders( nIndex, -1, 0, -1, -1 );

    Size aMinSize( maCancelButton.GetSizePixel() );
    // insert help button
    xButtons->setMinimumSize( xButtons->addWindow( &maHelpButton ), aMinSize );
    // insert a spacer, cancel and OK buttons are right aligned
    xSpacer.reset( new vcl::Spacer( xButtons.get(), 2 ) );
    xButtons->addChild( xSpacer );
    xButtons->setMinimumSize( xButtons->addWindow( &maOKButton ), aMinSize );
    xButtons->setMinimumSize( xButtons->addWindow( &maCancelButton ), aMinSize );
}

void PrintDialog::readFromSettings()
{
    maJobPage.readFromSettings();
    maNUpPage.readFromSettings();
    maOptionsPage.readFromSettings();

    // read last selected tab page; if it exists, actiavte it
    SettingsConfigItem* pItem = SettingsConfigItem::get();
    rtl::OUString aValue = pItem->getValue( rtl::OUString( "PrintDialog"  ),
                                            rtl::OUString( "LastPage"  ) );
    sal_uInt16 nCount = maTabCtrl.GetPageCount();
    for( sal_uInt16 i = 0; i < nCount; i++ )
    {
        sal_uInt16 nPageId = maTabCtrl.GetPageId( i );
        if( aValue.equals( maTabCtrl.GetPageText( nPageId ) ) )
        {
            maTabCtrl.SelectTabPage( nPageId );
            break;
        }
    }
    maOKButton.SetText( maOptionsPage.maToFileBox.IsChecked() ? maPrintToFileText : maPrintText );

    // persistent window state
    rtl::OUString aWinState( pItem->getValue( rtl::OUString( "PrintDialog"  ),
                                              rtl::OUString( "WindowState"  ) ) );
    if( !aWinState.isEmpty() )
        SetWindowState( rtl::OUStringToOString( aWinState, RTL_TEXTENCODING_UTF8 ) );

    if( maOptionsPage.maToFileBox.IsChecked() )
    {
        maPController->resetPrinterOptions( true );
        preparePreview( true, true );
    }
}

void PrintDialog::storeToSettings()
{
    maJobPage.storeToSettings();
    maNUpPage.storeToSettings();
    maOptionsPage.storeToSettings();

    // store last selected printer
    SettingsConfigItem* pItem = SettingsConfigItem::get();
    pItem->setValue( rtl::OUString( "PrintDialog"  ),
                     rtl::OUString( "LastPrinter"  ),
                     maJobPage.maPrinters.GetSelectEntry() );

    pItem->setValue( rtl::OUString( "PrintDialog"  ),
                     rtl::OUString( "LastPage"  ),
                     maTabCtrl.GetPageText( maTabCtrl.GetCurPageId() ) );
    pItem->setValue( rtl::OUString( "PrintDialog"  ),
                     rtl::OUString( "WindowState"  ),
                     rtl::OStringToOUString( GetWindowState(), RTL_TEXTENCODING_UTF8 )
                     );
    pItem->Commit();
}

bool PrintDialog::isPrintToFile()
{
    return maOptionsPage.maToFileBox.IsChecked();
}

bool PrintDialog::isCollate()
{
    return maJobPage.maCopyCountField.GetValue() > 1 ? maJobPage.maCollateBox.IsChecked() : sal_False;
}

bool PrintDialog::isSingleJobs()
{
    return maOptionsPage.maCollateSingleJobsBox.IsChecked();
}

void setHelpId( Window* i_pWindow, const Sequence< rtl::OUString >& i_rHelpIds, sal_Int32 i_nIndex )
{
    if( i_nIndex >= 0 && i_nIndex < i_rHelpIds.getLength() )
        i_pWindow->SetHelpId( rtl::OUStringToOString( i_rHelpIds.getConstArray()[i_nIndex], RTL_TEXTENCODING_UTF8 ) );
}

static void setHelpText( Window* i_pWindow, const Sequence< rtl::OUString >& i_rHelpTexts, sal_Int32 i_nIndex )
{
    // without a help text set and the correct smartID,
    // help texts will be retrieved from the online help system
    if( i_nIndex >= 0 && i_nIndex < i_rHelpTexts.getLength() )
        i_pWindow->SetHelpText( i_rHelpTexts.getConstArray()[i_nIndex] );
}

void updateMaxSize( const Size& i_rCheckSize, Size& o_rMaxSize )
{
    if( i_rCheckSize.Width() > o_rMaxSize.Width() )
        o_rMaxSize.Width() = i_rCheckSize.Width();
    if( i_rCheckSize.Height() > o_rMaxSize.Height() )
        o_rMaxSize.Height() = i_rCheckSize.Height();
}

void PrintDialog::setupOptionalUI()
{
    std::vector< boost::shared_ptr<vcl::RowOrColumn> > aDynamicColumns;
    boost::shared_ptr< vcl::RowOrColumn > pCurColumn;

    Window* pCurParent = 0, *pDynamicPageParent = 0;
    sal_uInt16 nOptPageId = 9;
    bool bOnStaticPage = false;
    bool bSubgroupOnStaticPage = false;

    std::multimap< rtl::OUString, boost::shared_ptr<vcl::RowOrColumn> > aPropertyToDependencyRowMap;

    const Sequence< PropertyValue >& rOptions( maPController->getUIOptions() );
    for( int i = 0; i < rOptions.getLength(); i++ )
    {
        Sequence< beans::PropertyValue > aOptProp;
        rOptions[i].Value >>= aOptProp;

        // extract ui element
        rtl::OUString aCtrlType;
        rtl::OUString aText;
        rtl::OUString aPropertyName;
        Sequence< rtl::OUString > aChoices;
        Sequence< sal_Bool > aChoicesDisabled;
        Sequence< rtl::OUString > aHelpTexts;
        Sequence< rtl::OUString > aHelpIds;
        sal_Int64 nMinValue = 0, nMaxValue = 0;
        rtl::OUString aGroupingHint;
        rtl::OUString aDependsOnName;
        sal_Int32 nDependsOnValue = 0;
        sal_Bool bUseDependencyRow = sal_False;

        for( int n = 0; n < aOptProp.getLength(); n++ )
        {
            const beans::PropertyValue& rEntry( aOptProp[ n ] );
            if ( rEntry.Name == "Text" )
            {
                rEntry.Value >>= aText;
            }
            else if ( rEntry.Name == "ControlType" )
            {
                rEntry.Value >>= aCtrlType;
            }
            else if ( rEntry.Name == "Choices" )
            {
                rEntry.Value >>= aChoices;
            }
            else if ( rEntry.Name == "ChoicesDisabled" )
            {
                rEntry.Value >>= aChoicesDisabled;
            }
            else if ( rEntry.Name == "Property" )
            {
                PropertyValue aVal;
                rEntry.Value >>= aVal;
                aPropertyName = aVal.Name;
            }
            else if ( rEntry.Name == "Enabled" )
            {
                sal_Bool bValue = sal_True;
                rEntry.Value >>= bValue;
            }
            else if ( rEntry.Name == "GroupingHint" )
            {
                rEntry.Value >>= aGroupingHint;
            }
            else if ( rEntry.Name == "DependsOnName" )
            {
                rEntry.Value >>= aDependsOnName;
            }
            else if ( rEntry.Name == "DependsOnEntry" )
            {
                rEntry.Value >>= nDependsOnValue;
            }
            else if ( rEntry.Name == "AttachToDependency" )
            {
                rEntry.Value >>= bUseDependencyRow;
            }
            else if ( rEntry.Name == "MinValue" )
            {
                rEntry.Value >>= nMinValue;
            }
            else if ( rEntry.Name == "MaxValue" )
            {
                rEntry.Value >>= nMaxValue;
            }
            else if ( rEntry.Name == "HelpText" )
            {
                if( ! (rEntry.Value >>= aHelpTexts) )
                {
                    rtl::OUString aHelpText;
                    if( (rEntry.Value >>= aHelpText) )
                    {
                        aHelpTexts.realloc( 1 );
                        *aHelpTexts.getArray() = aHelpText;
                    }
                }
            }
            else if ( rEntry.Name == "HelpId" )
            {
                if( ! (rEntry.Value >>= aHelpIds ) )
                {
                    rtl::OUString aHelpId;
                    if( (rEntry.Value >>= aHelpId) )
                    {
                        aHelpIds.realloc( 1 );
                        *aHelpIds.getArray() = aHelpId;
                    }
                }
            }
            else if ( rEntry.Name == "HintNoLayoutPage" )
            {
                sal_Bool bNoLayoutPage = sal_False;
                rEntry.Value >>= bNoLayoutPage;
                mbShowLayoutPage = ! bNoLayoutPage;
            }
        }

        // bUseDependencyRow should only be true if a dependency exists
        bUseDependencyRow = bUseDependencyRow && !aDependsOnName.isEmpty();

        // is it necessary to switch between static and dynamic pages ?
        bool bSwitchPage = false;
        if( !aGroupingHint.isEmpty() )
            bSwitchPage = true;
        else if( aCtrlType == "Subgroup" || (bOnStaticPage && ! bSubgroupOnStaticPage ) )
            bSwitchPage = true;
        if( bSwitchPage )
        {
            // restore to dynamic
            pCurParent = pDynamicPageParent;
            if( ! aDynamicColumns.empty() )
                pCurColumn = aDynamicColumns.back();
            else
                pCurColumn.reset();
            bOnStaticPage = false;
            bSubgroupOnStaticPage = false;

            if ( aGroupingHint == "PrintRange" )
            {
                pCurColumn = maJobPage.mxPrintRange;
                pCurParent = &maJobPage;            // set job page as current parent
                bOnStaticPage = true;
            }
            else if ( aGroupingHint == "OptionsPage" )
            {
                pCurColumn = boost::dynamic_pointer_cast<vcl::RowOrColumn>(maOptionsPage.getLayout());
                pCurParent = &maOptionsPage;        // set options page as current parent
                bOnStaticPage = true;
            }
            else if ( aGroupingHint == "OptionsPageOptGroup" )
            {
                pCurColumn = maOptionsPage.mxOptGroup;
                pCurParent = &maOptionsPage;        // set options page as current parent
                bOnStaticPage = true;
            }
            else if ( aGroupingHint == "LayoutPage" )
            {
                pCurColumn = boost::dynamic_pointer_cast<vcl::RowOrColumn>(maNUpPage.getLayout());
                pCurParent = &maNUpPage;            // set layout page as current parent
                bOnStaticPage = true;
            }
            else if( !aGroupingHint.isEmpty() )
            {
                pCurColumn = boost::dynamic_pointer_cast<vcl::RowOrColumn>(maJobPage.getLayout());
                pCurParent = &maJobPage;            // set job page as current parent
                bOnStaticPage = true;
            }
        }

        if( aCtrlType == "Group" || ( ! pCurParent && ! (bOnStaticPage || !aGroupingHint.isEmpty() ) ) )
        {
            // add new tab page
            TabPage* pNewGroup = new TabPage( &maTabCtrl );
            maControls.push_front( pNewGroup );
            pDynamicPageParent = pCurParent = pNewGroup;
            pNewGroup->SetText( aText );
            maTabCtrl.InsertPage( ++nOptPageId, aText );
            maTabCtrl.SetTabPage( nOptPageId, pNewGroup );

            // set help id
            setHelpId( pNewGroup, aHelpIds, 0 );
            // set help text
            setHelpText( pNewGroup, aHelpTexts, 0 );

            aDynamicColumns.push_back( boost::dynamic_pointer_cast<vcl::RowOrColumn>(pNewGroup->getLayout()) );
            pCurColumn = aDynamicColumns.back();
            pCurColumn->setParentWindow( pNewGroup );
            bSubgroupOnStaticPage = false;
            bOnStaticPage = false;
        }
        else if( aCtrlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "Subgroup" ) ) && (pCurParent || !aGroupingHint.isEmpty() ) )
        {
            bSubgroupOnStaticPage = !aGroupingHint.isEmpty();
            // create group FixedLine
            if( ! aGroupingHint.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "PrintRange" ) ) ||
                ! pCurColumn->countElements() == 0
               )
            {
                Window* pNewSub = NULL;
                if ( aGroupingHint == "PrintRange" )
                    pNewSub = new FixedText( pCurParent, WB_VCENTER );
                else
                    pNewSub = new FixedLine( pCurParent );
                maControls.push_front( pNewSub );
                pNewSub->SetText( aText );
                pNewSub->Show();

                // set help id
                setHelpId( pNewSub, aHelpIds, 0 );
                // set help text
                setHelpText( pNewSub, aHelpTexts, 0 );
                // add group to current column
                pCurColumn->addWindow( pNewSub );
            }

            // add an indent to the current column
            vcl::Indenter* pIndent = new vcl::Indenter( pCurColumn.get(), -1 );
            pCurColumn->addChild( pIndent );
            // and create a column inside the indent
            pCurColumn.reset( new vcl::RowOrColumn( pIndent ) );
            pIndent->setChild( pCurColumn );
        }
        // EVIL
        else if( aCtrlType == "Bool" && aGroupingHint == "LayoutPage" && aPropertyName == "PrintProspect" )
        {
            maNUpPage.maBrochureBtn.SetText( aText );
            maNUpPage.maBrochureBtn.Show();
            setHelpText( &maNUpPage.maBrochureBtn, aHelpTexts, 0 );

            sal_Bool bVal = sal_False;
            PropertyValue* pVal = maPController->getValue( aPropertyName );
            if( pVal )
                pVal->Value >>= bVal;
            maNUpPage.maBrochureBtn.Check( bVal );
            maNUpPage.maBrochureBtn.Enable( maPController->isUIOptionEnabled( aPropertyName ) && pVal != NULL );
            maNUpPage.maBrochureBtn.SetToggleHdl( LINK( this, PrintDialog, ClickHdl ) );

            maPropertyToWindowMap[ aPropertyName ].push_back( &maNUpPage.maBrochureBtn );
            maControlToPropertyMap[&maNUpPage.maBrochureBtn] = aPropertyName;

            aPropertyToDependencyRowMap.insert( std::pair< rtl::OUString, boost::shared_ptr<vcl::RowOrColumn> >( aPropertyName, maNUpPage.mxBrochureDep ) );
        }
        else
        {
            boost::shared_ptr<vcl::RowOrColumn> pSaveCurColumn( pCurColumn );

            if( bUseDependencyRow )
            {
                // find the correct dependency row (if any)
                std::pair< std::multimap< rtl::OUString, boost::shared_ptr<vcl::RowOrColumn> >::iterator,
                           std::multimap< rtl::OUString, boost::shared_ptr<vcl::RowOrColumn> >::iterator > aDepRange;
                aDepRange = aPropertyToDependencyRowMap.equal_range( aDependsOnName );
                if( aDepRange.first != aDepRange.second )
                {
                    while( nDependsOnValue && aDepRange.first != aDepRange.second )
                    {
                        nDependsOnValue--;
                        ++aDepRange.first;
                    }
                    if( aDepRange.first != aPropertyToDependencyRowMap.end() )
                    {
                        pCurColumn = aDepRange.first->second;
                        maReverseDependencySet.insert( aPropertyName );
                    }
                }
            }
            if( aCtrlType == "Bool" && pCurParent )
            {
                // add a check box
                CheckBox* pNewBox = new CheckBox( pCurParent );
                maControls.push_front( pNewBox );
                pNewBox->SetText( aText );
                pNewBox->Show();

                sal_Bool bVal = sal_False;
                PropertyValue* pVal = maPController->getValue( aPropertyName );
                if( pVal )
                    pVal->Value >>= bVal;
                pNewBox->Check( bVal );
                pNewBox->SetToggleHdl( LINK( this, PrintDialog, UIOption_CheckHdl ) );

                maPropertyToWindowMap[ aPropertyName ].push_back( pNewBox );
                maControlToPropertyMap[pNewBox] = aPropertyName;

                // set help id
                setHelpId( pNewBox, aHelpIds, 0 );
                // set help text
                setHelpText( pNewBox, aHelpTexts, 0 );

                boost::shared_ptr<vcl::RowOrColumn> pDependencyRow( new vcl::RowOrColumn( pCurColumn.get(), false ) );
                pCurColumn->addChild( pDependencyRow );
                aPropertyToDependencyRowMap.insert( std::pair< rtl::OUString, boost::shared_ptr<vcl::RowOrColumn> >( aPropertyName, pDependencyRow ) );

                // add checkbox to current column
                pDependencyRow->addWindow( pNewBox );
            }
            else if( aCtrlType == "Radio" && pCurParent )
            {
                sal_Int32 nCurHelpText = 0;
                boost::shared_ptr<vcl::RowOrColumn> pRadioColumn( pCurColumn );
                if( !aText.isEmpty() )
                {
                    // add a FixedText:
                    FixedText* pHeading = new FixedText( pCurParent );
                    maControls.push_front( pHeading );
                    pHeading->SetText( aText );
                    pHeading->Show();

                    // set help id
                    setHelpId( pHeading, aHelpIds, nCurHelpText );
                    // set help text
                    setHelpText( pHeading, aHelpTexts, nCurHelpText );
                    nCurHelpText++;
                    // add fixed text to current column
                    pCurColumn->addWindow( pHeading );
                    // add an indent to the current column
                    vcl::Indenter* pIndent = new vcl::Indenter( pCurColumn.get(), 15 );
                    pCurColumn->addChild( pIndent );
                    // and create a column inside the indent
                    pRadioColumn.reset( new vcl::RowOrColumn( pIndent ) );
                    pIndent->setChild( pRadioColumn );
                }
                // iterate options
                sal_Int32 nSelectVal = 0;
                PropertyValue* pVal = maPController->getValue( aPropertyName );
                if( pVal && pVal->Value.hasValue() )
                    pVal->Value >>= nSelectVal;
                for( sal_Int32 m = 0; m < aChoices.getLength(); m++ )
                {
                    boost::shared_ptr<vcl::LabeledElement> pLabel( new vcl::LabeledElement( pRadioColumn.get(), 1 ) );
                    pRadioColumn->addChild( pLabel );
                    boost::shared_ptr<vcl::RowOrColumn> pDependencyRow( new vcl::RowOrColumn( pLabel.get(), false ) );
                    pLabel->setElement( pDependencyRow );
                    aPropertyToDependencyRowMap.insert( std::pair< rtl::OUString, boost::shared_ptr<vcl::RowOrColumn> >( aPropertyName, pDependencyRow ) );

                    RadioButton* pBtn = new RadioButton( pCurParent, m == 0 ? WB_GROUP : 0 );
                    maControls.push_front( pBtn );
                    pBtn->SetText( aChoices[m] );
                    pBtn->Check( m == nSelectVal );
                    pBtn->SetToggleHdl( LINK( this, PrintDialog, UIOption_RadioHdl ) );
                    if( aChoicesDisabled.getLength() > m && aChoicesDisabled[m] == sal_True )
                        pBtn->Enable( sal_False );
                    pBtn->Show();
                    maPropertyToWindowMap[ aPropertyName ].push_back( pBtn );
                    maControlToPropertyMap[pBtn] = aPropertyName;
                    maControlToNumValMap[pBtn] = m;

                    // set help id
                    setHelpId( pBtn, aHelpIds, nCurHelpText );
                    // set help text
                    setHelpText( pBtn, aHelpTexts, nCurHelpText );
                    nCurHelpText++;
                    // add the radio button to the column
                    pLabel->setLabel( pBtn );
                }
            }
            else if( ( aCtrlType == "List" || aCtrlType == "Range" || aCtrlType == "Edit" ) && pCurParent )
            {
                // create a row in the current column
                boost::shared_ptr<vcl::RowOrColumn> pFieldColumn( new vcl::RowOrColumn( pCurColumn.get(), false ) );
                pCurColumn->addChild( pFieldColumn );
                aPropertyToDependencyRowMap.insert( std::pair< rtl::OUString, boost::shared_ptr<vcl::RowOrColumn> >( aPropertyName, pFieldColumn ) );

                vcl::LabeledElement* pLabel = NULL;
                if( !aText.isEmpty() )
                {
                    // add a FixedText:
                    FixedText* pHeading = new FixedText( pCurParent, WB_VCENTER );
                    maControls.push_front( pHeading );
                    pHeading->SetText( aText );
                    pHeading->Show();


                    // add to row
                    pLabel = new vcl::LabeledElement( pFieldColumn.get(), 2 );
                    pFieldColumn->addChild( pLabel );
                    pLabel->setLabel( pHeading );
                }

                if ( aCtrlType == "List" )
                {
                    ListBox* pList = new ListBox( pCurParent, WB_DROPDOWN | WB_BORDER );
                    maControls.push_front( pList );

                    // iterate options
                    for( sal_Int32 m = 0; m < aChoices.getLength(); m++ )
                    {
                        pList->InsertEntry( aChoices[m] );
                    }
                    sal_Int32 nSelectVal = 0;
                    PropertyValue* pVal = maPController->getValue( aPropertyName );
                    if( pVal && pVal->Value.hasValue() )
                        pVal->Value >>= nSelectVal;
                    pList->SelectEntryPos( static_cast<sal_uInt16>(nSelectVal) );
                    pList->SetSelectHdl( LINK( this, PrintDialog, UIOption_SelectHdl ) );
                    pList->SetDropDownLineCount( static_cast<sal_uInt16>(aChoices.getLength()) );
                    pList->Show();

                    // set help id
                    setHelpId( pList, aHelpIds, 0 );
                    // set help text
                    setHelpText( pList, aHelpTexts, 0 );

                    maPropertyToWindowMap[ aPropertyName ].push_back( pList );
                    maControlToPropertyMap[pList] = aPropertyName;

                    // finish the pair
                    if( pLabel )
                        pLabel->setElement( pList );
                    else
                        pFieldColumn->addWindow( pList );
                }
                else if ( aCtrlType == "Range" )
                {
                    NumericField* pField = new NumericField( pCurParent, WB_BORDER | WB_SPIN );
                    maControls.push_front( pField );

                    // set min/max and current value
                    if( nMinValue != nMaxValue )
                    {
                        pField->SetMin( nMinValue );
                        pField->SetMax( nMaxValue );
                    }
                    sal_Int64 nCurVal = 0;
                    PropertyValue* pVal = maPController->getValue( aPropertyName );
                    if( pVal && pVal->Value.hasValue() )
                        pVal->Value >>= nCurVal;
                    pField->SetValue( nCurVal );
                    pField->SetModifyHdl( LINK( this, PrintDialog, UIOption_ModifyHdl ) );
                    pField->Show();

                    // set help id
                    setHelpId( pField, aHelpIds, 0 );
                    // set help text
                    setHelpText( pField, aHelpTexts, 0 );

                    maPropertyToWindowMap[ aPropertyName ].push_back( pField );
                    maControlToPropertyMap[pField] = aPropertyName;

                    // add to row
                    if( pLabel )
                        pLabel->setElement( pField );
                    else
                        pFieldColumn->addWindow( pField );
                }
                else if ( aCtrlType == "Edit" )
                {
                    Edit* pField = new Edit( pCurParent, WB_BORDER );
                    maControls.push_front( pField );

                    rtl::OUString aCurVal;
                    PropertyValue* pVal = maPController->getValue( aPropertyName );
                    if( pVal && pVal->Value.hasValue() )
                        pVal->Value >>= aCurVal;
                    pField->SetText( aCurVal );
                    pField->SetModifyHdl( LINK( this, PrintDialog, UIOption_ModifyHdl ) );
                    pField->Show();

                    // set help id
                    setHelpId( pField, aHelpIds, 0 );
                    // set help text
                    setHelpText( pField, aHelpTexts, 0 );

                    maPropertyToWindowMap[ aPropertyName ].push_back( pField );
                    maControlToPropertyMap[pField] = aPropertyName;

                    // add to row
                    if( pLabel )
                        pLabel->setElement( pField );
                    else
                        pFieldColumn->addWindow( pField, 2 );
                }
            }
            else
            {
                rtl::OStringBuffer sMessage;
                sMessage.append(RTL_CONSTASCII_STRINGPARAM("Unsupported UI option: \""));
                sMessage.append(rtl::OUStringToOString(aCtrlType, RTL_TEXTENCODING_UTF8));
                sMessage.append('"');
                OSL_FAIL( sMessage.getStr() );
            }

            pCurColumn = pSaveCurColumn;
        }
    }

    // #i106506# if no brochure button, then the singular Pages radio button
    // makes no sense, so replace it by a FixedText label
    if( ! maNUpPage.maBrochureBtn.IsVisible() )
    {
        if( maNUpPage.mxPagesBtnLabel.get() )
        {
            maNUpPage.maPagesBoxTitleTxt.SetText( maNUpPage.maPagesBtn.GetText() );
            maNUpPage.maPagesBoxTitleTxt.Show( sal_True );
            maNUpPage.mxPagesBtnLabel->setLabel( &maNUpPage.maPagesBoxTitleTxt );
            maNUpPage.maPagesBtn.Show( sal_False );
        }
    }

    // update enable states
    checkOptionalControlDependencies();

    // print range empty (currently math only) -> hide print range and spacer line
    if( maJobPage.mxPrintRange->countElements() == 0 )
    {
        maJobPage.mxPrintRange->show( false, false );
        maJobPage.maCopySpacer.Show( sal_False );
        maJobPage.maReverseOrderBox.Show( sal_False );
    }
    else
    {
        // add an indent to the current column
        vcl::Indenter* pIndent = new vcl::Indenter( maJobPage.mxPrintRange.get(), -1 );
        maJobPage.mxPrintRange->addChild( pIndent );
        // and create a column inside the indent
        pIndent->setWindow( &maJobPage.maReverseOrderBox );
        maJobPage.maReverseOrderBox.Show( sal_True );
    }

#ifdef WNT
    // FIXME: the GetNativeControlRegion call on Windows has some issues
    // (which skew the results of GetOptimalSize())
    // however fixing this thoroughly needs to take interaction with paint into
    // account, making the right fix less simple. Fix this the right way
    // at some point. For now simply add some space at the lowest element
    size_t nIndex = maJobPage.getLayout()->countElements();
    if( nIndex > 0 ) // sanity check
        maJobPage.getLayout()->setBorders( nIndex-1, 0, 0, 0, -1 );
#endif

    // create auto mnemomnics now so they can be calculated in layout
    ImplWindowAutoMnemonic( &maJobPage );
    ImplWindowAutoMnemonic( &maNUpPage );
    ImplWindowAutoMnemonic( &maOptionsPage );
    ImplWindowAutoMnemonic( this );

    // calculate job page
    Size aMaxSize = maJobPage.getLayout()->getOptimalSize( WINDOWSIZE_PREFERRED );
    // and layout page
    updateMaxSize( maNUpPage.getLayout()->getOptimalSize( WINDOWSIZE_PREFERRED ), aMaxSize );
    // and options page
    updateMaxSize( maOptionsPage.getLayout()->getOptimalSize( WINDOWSIZE_PREFERRED ), aMaxSize );

    for( std::vector< boost::shared_ptr<vcl::RowOrColumn> >::iterator it = aDynamicColumns.begin();
         it != aDynamicColumns.end(); ++it )
    {
        Size aPageSize( (*it)->getOptimalSize( WINDOWSIZE_PREFERRED ) );
        updateMaxSize( aPageSize, aMaxSize );
    }

    // resize dialog if necessary
    Size aTabSize = maTabCtrl.GetTabPageSizePixel();
    maTabCtrl.SetMinimumSizePixel( maTabCtrl.GetSizePixel() );
    if( aMaxSize.Height() > aTabSize.Height() || aMaxSize.Width() > aTabSize.Width() )
    {
        Size aCurSize( GetOutputSizePixel() );
        if( aMaxSize.Height() > aTabSize.Height() )
        {
            aCurSize.Height() += aMaxSize.Height() - aTabSize.Height();
            aTabSize.Height() = aMaxSize.Height();
        }
        if( aMaxSize.Width() > aTabSize.Width() )
        {
            aCurSize.Width() += aMaxSize.Width() - aTabSize.Width();
            // and the tab ctrl needs more space, too
            aTabSize.Width() = aMaxSize.Width();
        }
        maTabCtrl.SetTabPageSizePixel( aTabSize );
        maTabCtrl.SetMinimumSizePixel( maTabCtrl.GetSizePixel() );
    }

    Size aSz = getLayout()->getOptimalSize( WINDOWSIZE_PREFERRED );

    SetOutputSizePixel( aSz );
}

void PrintDialog::DataChanged( const DataChangedEvent& i_rDCEvt )
{
    // react on settings changed
    if( i_rDCEvt.GetType() == DATACHANGED_SETTINGS )
        checkControlDependencies();
    ModalDialog::DataChanged( i_rDCEvt );
}

void PrintDialog::checkControlDependencies()
{
    if( maJobPage.maCopyCountField.GetValue() > 1 )
        maJobPage.maCollateBox.Enable( maJobPage.mnCollateUIMode == 0 );
    else
        maJobPage.maCollateBox.Enable( sal_False );

    Image aImg( maJobPage.maCollateBox.IsChecked() ? maJobPage.maCollateImg : maJobPage.maNoCollateImg );

    Size aImgSize( aImg.GetSizePixel() );

    // adjust size of image
    maJobPage.maCollateImage.SetSizePixel( aImgSize );
    maJobPage.maCollateImage.SetImage( aImg );
    maJobPage.getLayout()->resize();

    // enable setup button only for printers that can be setup
    bool bHaveSetup = maPController->getPrinter()->HasSupport( SUPPORT_SETUPDIALOG );
    maJobPage.maSetupButton.Enable( bHaveSetup );
    if( bHaveSetup )
    {
        if( ! maJobPage.maSetupButton.IsVisible() )
        {
            Point aPrinterPos( maJobPage.maPrinters.GetPosPixel() );
            Point aSetupPos( maJobPage.maSetupButton.GetPosPixel() );
            Size aPrinterSize( maJobPage.maPrinters.GetSizePixel() );
            aPrinterSize.Width() = aSetupPos.X() - aPrinterPos.X() - LogicToPixel( Size( 5, 5 ), MapMode( MAP_APPFONT ) ).Width();
            maJobPage.maPrinters.SetSizePixel( aPrinterSize );
            maJobPage.maSetupButton.Show();
            getLayout()->resize();
        }
    }
    else
    {
        if( maJobPage.maSetupButton.IsVisible() )
        {
            Point aPrinterPos( maJobPage.maPrinters.GetPosPixel() );
            Point aSetupPos( maJobPage.maSetupButton.GetPosPixel() );
            Size aPrinterSize( maJobPage.maPrinters.GetSizePixel() );
            Size aSetupSize( maJobPage.maSetupButton.GetSizePixel() );
            aPrinterSize.Width() = aSetupPos.X() + aSetupSize.Width() - aPrinterPos.X();
            maJobPage.maPrinters.SetSizePixel( aPrinterSize );
            maJobPage.maSetupButton.Hide();
            getLayout()->resize();
        }
    }
}

void PrintDialog::checkOptionalControlDependencies()
{
    for( std::map< Window*, rtl::OUString >::iterator it = maControlToPropertyMap.begin();
         it != maControlToPropertyMap.end(); ++it )
    {
        bool bShouldbeEnabled = maPController->isUIOptionEnabled( it->second );
        if( ! bShouldbeEnabled )
        {
            // enable controls that are directly attached to a dependency anyway
            // if the normally disabled controls get modified, change the dependency
            // so the control would be enabled
            // example: in print range "Print All" is selected, "Page Range" is then of course
            // not selected and the Edit for the Page Range would be disabled
            // as a convenience we should enable the Edit anyway and automatically select
            // "Page Range" instead of "Print All" if the Edit gets modified
            if( maReverseDependencySet.find( it->second ) != maReverseDependencySet.end() )
            {
                rtl::OUString aDep( maPController->getDependency( it->second ) );
                // if the dependency is at least enabled, then enable this control anyway
                if( !aDep.isEmpty() && maPController->isUIOptionEnabled( aDep ) )
                    bShouldbeEnabled = true;
            }
        }

        if( bShouldbeEnabled && dynamic_cast<RadioButton*>(it->first) )
        {
            std::map< Window*, sal_Int32 >::const_iterator r_it = maControlToNumValMap.find( it->first );
            if( r_it != maControlToNumValMap.end() )
            {
                bShouldbeEnabled = maPController->isUIChoiceEnabled( it->second, r_it->second );
            }
        }


        bool bIsEnabled = it->first->IsEnabled();
        // Enable does not do a change check first, so can be less cheap than expected
        if( bShouldbeEnabled != bIsEnabled )
            it->first->Enable( bShouldbeEnabled );
    }
}

static rtl::OUString searchAndReplace( const rtl::OUString& i_rOrig, const char* i_pRepl, sal_Int32 i_nReplLen, const rtl::OUString& i_rRepl )
{
    sal_Int32 nPos = i_rOrig.indexOfAsciiL( i_pRepl, i_nReplLen );
    if( nPos != -1 )
    {
        rtl::OUStringBuffer aBuf( i_rOrig.getLength() );
        aBuf.append( i_rOrig.getStr(), nPos );
        aBuf.append( i_rRepl );
        if( nPos + i_nReplLen < i_rOrig.getLength() )
            aBuf.append( i_rOrig.getStr() + nPos + i_nReplLen );
        return aBuf.makeStringAndClear();
    }
    return i_rOrig;
}

void PrintDialog::updatePrinterText()
{
    const rtl::OUString aDefPrt( Printer::GetDefaultPrinterName() );
    const QueueInfo* pInfo = Printer::GetQueueInfo( maJobPage.maPrinters.GetSelectEntry(), true );
    if( pInfo )
    {
        maJobPage.maLocationTxt.SetText( pInfo->GetLocation() );
        maJobPage.maCommentTxt.SetText( pInfo->GetComment() );
        // FIXME: status text
        rtl::OUString aStatus;
        if( aDefPrt == pInfo->GetPrinterName() )
            aStatus = maDefPrtText;
        maJobPage.maStatusTxt.SetText( aStatus );
    }
    else
    {
        maJobPage.maLocationTxt.SetText( String() );
        maJobPage.maCommentTxt.SetText( String() );
        maJobPage.maStatusTxt.SetText( String() );
    }
}

void PrintDialog::setPreviewText( sal_Int32 )
{
    rtl::OUString aNewText( searchAndReplace( maPageStr, "%n", 2, rtl::OUString::valueOf( mnCachedPages )  ) );
    maNumPagesText.SetText( aNewText );

    // if layout is already established the refresh layout of
    // preview controls since text length may have changes
    if( mxPreviewCtrls.get() )
        mxPreviewCtrls->setManagedArea( mxPreviewCtrls->getManagedArea() );
}

void PrintDialog::preparePreview( bool i_bNewPage, bool i_bMayUseCache )
{
    // page range may have changed depending on options
    sal_Int32 nPages = maPController->getFilteredPageCount();
    mnCachedPages = nPages;

    if( mnCurPage >= nPages )
        mnCurPage = nPages-1;
    if( mnCurPage < 0 )
        mnCurPage = 0;

    setPreviewText( mnCurPage );

    maPageEdit.SetMin( 1 );
    maPageEdit.SetMax( nPages );

    if( i_bNewPage )
    {
        const MapMode aMapMode( MAP_100TH_MM );
        GDIMetaFile aMtf;
        boost::shared_ptr<Printer> aPrt( maPController->getPrinter() );
        if( nPages > 0 )
        {
            PrinterController::PageSize aPageSize =
                maPController->getFilteredPageFile( mnCurPage, aMtf, i_bMayUseCache );
            if( ! aPageSize.bFullPaper )
            {
                Point aOff( aPrt->PixelToLogic( aPrt->GetPageOffsetPixel(), aMapMode ) );
                aMtf.Move( aOff.X(), aOff.Y() );
            }
        }

        Size aCurPageSize = aPrt->PixelToLogic( aPrt->GetPaperSizePixel(), MapMode( MAP_100TH_MM ) );
        maPreviewWindow.setPreview( aMtf, aCurPageSize,
                                    aPrt->GetPaperName( false ),
                                    nPages > 0 ? rtl::OUString() : maNoPageStr,
                                    aPrt->ImplGetDPIX(), aPrt->ImplGetDPIY(),
                                    aPrt->GetPrinterOptions().IsConvertToGreyscales()
                                   );

        maForwardBtn.Enable( mnCurPage < nPages-1 );
        maBackwardBtn.Enable( mnCurPage != 0 );
        maPageEdit.Enable( nPages > 1 );
    }
}

Size PrintDialog::getJobPageSize()
{
    if( maFirstPageSize.Width() == 0 && maFirstPageSize.Height() == 0)
    {
        maFirstPageSize = maNupPortraitSize;
        GDIMetaFile aMtf;
        if( maPController->getPageCountProtected() > 0 )
        {
            PrinterController::PageSize aPageSize = maPController->getPageFile( 0, aMtf, true );
            maFirstPageSize = aPageSize.aSize;
        }
    }
    return maFirstPageSize;
}

void PrintDialog::updateNupFromPages()
{
    long nPages = long(maNUpPage.maNupPagesBox.GetEntryData(maNUpPage.maNupPagesBox.GetSelectEntryPos()));
    int nRows   = int(maNUpPage.maNupRowsEdt.GetValue());
    int nCols   = int(maNUpPage.maNupColEdt.GetValue());
    long nPageMargin  = long(maNUpPage.maPageMarginEdt.Denormalize(maNUpPage.maPageMarginEdt.GetValue( FUNIT_100TH_MM )));
    long nSheetMargin = long(maNUpPage.maSheetMarginEdt.Denormalize(maNUpPage.maSheetMarginEdt.GetValue( FUNIT_100TH_MM )));
    bool bCustom = false;

    if( nPages == 1 )
    {
        nRows = nCols = 1;
        nSheetMargin = 0;
        nPageMargin = 0;
    }
    else if( nPages == 2 || nPages == 4 || nPages == 6 || nPages == 9 || nPages == 16 )
    {
        Size aJobPageSize( getJobPageSize() );
        bool bPortrait = aJobPageSize.Width() < aJobPageSize.Height();
        if( nPages == 2 )
        {
            if( bPortrait )
                nRows = 1, nCols = 2;
            else
                nRows = 2, nCols = 1;
        }
        else if( nPages == 4 )
            nRows = nCols = 2;
        else if( nPages == 6 )
        {
            if( bPortrait )
                nRows = 2, nCols = 3;
            else
                nRows = 3, nCols = 2;
        }
        else if( nPages == 9 )
            nRows = nCols = 3;
        else if( nPages == 16 )
            nRows = nCols = 4;
        nPageMargin = 0;
        nSheetMargin = 0;
    }
    else
        bCustom = true;

    if( nPages > 1 )
    {
        // set upper limits for margins based on job page size and rows/columns
        Size aSize( getJobPageSize() );

        // maximum sheet distance: 1/2 sheet
        long nHorzMax = aSize.Width()/2;
        long nVertMax = aSize.Height()/2;
        if( nSheetMargin > nHorzMax )
            nSheetMargin = nHorzMax;
        if( nSheetMargin > nVertMax )
            nSheetMargin = nVertMax;

        maNUpPage.maSheetMarginEdt.SetMax(
                  maNUpPage.maSheetMarginEdt.Normalize(
                           nHorzMax > nVertMax ? nVertMax : nHorzMax ), FUNIT_100TH_MM );

        // maximum page distance
        nHorzMax = (aSize.Width() - 2*nSheetMargin);
        if( nCols > 1 )
            nHorzMax /= (nCols-1);
        nVertMax = (aSize.Height() - 2*nSheetMargin);
        if( nRows > 1 )
            nHorzMax /= (nRows-1);

        if( nPageMargin > nHorzMax )
            nPageMargin = nHorzMax;
        if( nPageMargin > nVertMax )
            nPageMargin = nVertMax;

        maNUpPage.maPageMarginEdt.SetMax(
                 maNUpPage.maSheetMarginEdt.Normalize(
                           nHorzMax > nVertMax ? nVertMax : nHorzMax ), FUNIT_100TH_MM );
    }

    maNUpPage.maNupRowsEdt.SetValue( nRows );
    maNUpPage.maNupColEdt.SetValue( nCols );
    maNUpPage.maPageMarginEdt.SetValue( maNUpPage.maPageMarginEdt.Normalize( nPageMargin ), FUNIT_100TH_MM );
    maNUpPage.maSheetMarginEdt.SetValue( maNUpPage.maSheetMarginEdt.Normalize( nSheetMargin ), FUNIT_100TH_MM );

    maNUpPage.showAdvancedControls( bCustom );
    if( bCustom )
    {
        // see if we have to enlarge the dialog to make the tab page fit
        Size aCurSize( maNUpPage.getLayout()->getOptimalSize( WINDOWSIZE_PREFERRED ) );
        Size aTabSize( maTabCtrl.GetTabPageSizePixel() );
        if( aTabSize.Height() < aCurSize.Height() )
        {
            Size aDlgSize( GetSizePixel() );
            aDlgSize.Height() += aCurSize.Height() - aTabSize.Height();
            SetSizePixel( aDlgSize );
        }
    }

    updateNup();
}

void PrintDialog::updateNup()
{
    int nRows         = int(maNUpPage.maNupRowsEdt.GetValue());
    int nCols         = int(maNUpPage.maNupColEdt.GetValue());
    long nPageMargin  = long(maNUpPage.maPageMarginEdt.Denormalize(maNUpPage.maPageMarginEdt.GetValue( FUNIT_100TH_MM )));
    long nSheetMargin = long(maNUpPage.maSheetMarginEdt.Denormalize(maNUpPage.maSheetMarginEdt.GetValue( FUNIT_100TH_MM )));

    PrinterController::MultiPageSetup aMPS;
    aMPS.nRows         = nRows;
    aMPS.nColumns      = nCols;
    aMPS.nRepeat       = 1;
    aMPS.nLeftMargin   =
    aMPS.nTopMargin    =
    aMPS.nRightMargin  =
    aMPS.nBottomMargin = nSheetMargin;

    aMPS.nHorizontalSpacing =
    aMPS.nVerticalSpacing   = nPageMargin;

    aMPS.bDrawBorder        = maNUpPage.maBorderCB.IsChecked();

    int nOrderMode = int(sal_IntPtr(maNUpPage.maNupOrderBox.GetEntryData(
                           maNUpPage.maNupOrderBox.GetSelectEntryPos() )));
    if( nOrderMode == SV_PRINT_PRT_NUP_ORDER_LRTB )
        aMPS.nOrder = PrinterController::LRTB;
    else if( nOrderMode == SV_PRINT_PRT_NUP_ORDER_TBLR )
        aMPS.nOrder = PrinterController::TBLR;
    else if( nOrderMode == SV_PRINT_PRT_NUP_ORDER_RLTB )
        aMPS.nOrder = PrinterController::RLTB;
    else if( nOrderMode == SV_PRINT_PRT_NUP_ORDER_TBRL )
        aMPS.nOrder = PrinterController::TBRL;

    int nOrientationMode = int(sal_IntPtr(maNUpPage.maNupOrientationBox.GetEntryData(
                                 maNUpPage.maNupOrientationBox.GetSelectEntryPos() )));
    if( nOrientationMode == SV_PRINT_PRT_NUP_ORIENTATION_LANDSCAPE )
        aMPS.aPaperSize = maNupLandscapeSize;
    else if( nOrientationMode == SV_PRINT_PRT_NUP_ORIENTATION_PORTRAIT )
        aMPS.aPaperSize = maNupPortraitSize;
    else // automatic mode
    {
        // get size of first real page to see if it is portrait or landscape
        // we assume same page sizes for all the pages for this
        Size aPageSize = getJobPageSize();

        Size aMultiSize( aPageSize.Width() * nCols, aPageSize.Height() * nRows );
        if( aMultiSize.Width() > aMultiSize.Height() ) // fits better on landscape
            aMPS.aPaperSize = maNupLandscapeSize;
        else
            aMPS.aPaperSize = maNupPortraitSize;
    }

    maPController->setMultipage( aMPS );

    maNUpPage.maNupOrderWin.setValues( nOrderMode, nCols, nRows );

    preparePreview( true, true );
}

IMPL_LINK( PrintDialog, SelectHdl, ListBox*, pBox )
{
    if(  pBox == &maJobPage.maPrinters )
    {
        String aNewPrinter( pBox->GetSelectEntry() );
        // set new printer
        maPController->setPrinter( boost::shared_ptr<Printer>( new Printer( aNewPrinter ) ) );
        maPController->resetPrinterOptions( maOptionsPage.maToFileBox.IsChecked() );
        // update text fields
        updatePrinterText();
        preparePreview( true, false );
    }
    else if( pBox == &maNUpPage.maNupOrientationBox || pBox == &maNUpPage.maNupOrderBox )
    {
        updateNup();
    }
    else if( pBox == &maNUpPage.maNupPagesBox )
    {
        if( !maNUpPage.maPagesBtn.IsChecked() )
            maNUpPage.maPagesBtn.Check();
        updateNupFromPages();
    }

    return 0;
}

IMPL_LINK( PrintDialog, ClickHdl, Button*, pButton )
{
    if( pButton == &maOKButton || pButton == &maCancelButton )
    {
        storeToSettings();
        EndDialog( pButton == &maOKButton );
    }
    else if( pButton == &maHelpButton )
    {
        // start help system
        Help* pHelp = Application::GetHelp();
        if( pHelp )
        {
            pHelp->Start( rtl::OUString( ".HelpID:vcl:PrintDialog:OK"  ), &maOKButton );
        }
    }
    else if( pButton == &maForwardBtn )
    {
        previewForward();
    }
    else if( pButton == &maBackwardBtn )
    {
        previewBackward();
    }
    else if( pButton == &maOptionsPage.maToFileBox )
    {
        maOKButton.SetText( maOptionsPage.maToFileBox.IsChecked() ? maPrintToFileText : maPrintText );
        maPController->resetPrinterOptions( maOptionsPage.maToFileBox.IsChecked() );
        getLayout()->resize();
        preparePreview( true, true );
    }
    else if( pButton == &maNUpPage.maBrochureBtn )
    {
        PropertyValue* pVal = getValueForWindow( pButton );
        if( pVal )
        {
            sal_Bool bVal = maNUpPage.maBrochureBtn.IsChecked();
            pVal->Value <<= bVal;

            checkOptionalControlDependencies();

            // update preview and page settings
            preparePreview();
        }
        if( maNUpPage.maBrochureBtn.IsChecked() )
        {
            maNUpPage.maNupPagesBox.SelectEntryPos( 0 );
            updateNupFromPages();
            maNUpPage.showAdvancedControls( false );
            maNUpPage.enableNupControls( false );
        }
    }
    else if( pButton == &maNUpPage.maPagesBtn )
    {
        maNUpPage.enableNupControls( true );
        updateNupFromPages();
    }
    else if( pButton == &maJobPage.maDetailsBtn )
    {
        bool bShow = maJobPage.maDetailsBtn.IsChecked();
        maJobPage.mxDetails->show( bShow );
        if( bShow )
        {
            maDetailsCollapsedSize = GetOutputSizePixel();
            // enlarge dialog if necessary
            Size aMinSize( maJobPage.getLayout()->getOptimalSize( WINDOWSIZE_MINIMUM ) );
            Size aCurSize( maJobPage.GetSizePixel() );
            if( aCurSize.Height() < aMinSize.Height() )
            {
                Size aDlgSize( GetOutputSizePixel() );
                aDlgSize.Height() += aMinSize.Height() - aCurSize.Height();
                SetOutputSizePixel( aDlgSize );
            }
            maDetailsExpandedSize = GetOutputSizePixel();
        }
        else if( maDetailsCollapsedSize.Width() > 0   &&
                 maDetailsCollapsedSize.Height() > 0 )
        {
            // if the user did not resize the dialog
            // make it smaller again on collapsing the details
            Size aDlgSize( GetOutputSizePixel() );
            if( aDlgSize == maDetailsExpandedSize &&
                aDlgSize.Height() > maDetailsCollapsedSize.Height() )
            {
                SetOutputSizePixel( maDetailsCollapsedSize );
            }
        }
    }
    else if( pButton == &maJobPage.maCollateBox )
    {
        maPController->setValue( rtl::OUString( "Collate"  ),
                                 makeAny( sal_Bool(isCollate()) ) );
        checkControlDependencies();
    }
    else if( pButton == &maJobPage.maReverseOrderBox )
    {
        sal_Bool bChecked = maJobPage.maReverseOrderBox.IsChecked();
        maPController->setReversePrint( bChecked );
        maPController->setValue( rtl::OUString( "PrintReverse"  ),
                                 makeAny( bChecked ) );
        preparePreview( true, true );
    }
    else if( pButton == &maNUpPage.maBorderCB )
    {
        updateNup();
    }
    else
    {
        if( pButton == &maJobPage.maSetupButton )
        {
            maPController->setupPrinter( this );
            preparePreview( true, true );
        }
        checkControlDependencies();
    }
    return 0;
}

IMPL_LINK( PrintDialog, ModifyHdl, Edit*, pEdit )
{
    checkControlDependencies();
    if( pEdit == &maNUpPage.maNupRowsEdt || pEdit == &maNUpPage.maNupColEdt ||
        pEdit == &maNUpPage.maSheetMarginEdt || pEdit == &maNUpPage.maPageMarginEdt
       )
    {
        updateNupFromPages();
    }
    else if( pEdit == &maPageEdit )
    {
        mnCurPage = sal_Int32( maPageEdit.GetValue() - 1 );
        preparePreview( true, true );
    }
    else if( pEdit == &maJobPage.maCopyCountField )
    {
        maPController->setValue( rtl::OUString( "CopyCount"  ),
                               makeAny( sal_Int32(maJobPage.maCopyCountField.GetValue()) ) );
        maPController->setValue( rtl::OUString( "Collate"  ),
                               makeAny( sal_Bool(isCollate()) ) );
    }
    return 0;
}

IMPL_LINK_NOARG(PrintDialog, UIOptionsChanged)
{
    checkOptionalControlDependencies();
    return 0;
}

PropertyValue* PrintDialog::getValueForWindow( Window* i_pWindow ) const
{
    PropertyValue* pVal = NULL;
    std::map< Window*, rtl::OUString >::const_iterator it = maControlToPropertyMap.find( i_pWindow );
    if( it != maControlToPropertyMap.end() )
    {
        pVal = maPController->getValue( it->second );
        DBG_ASSERT( pVal, "property value not found" );
    }
    else
    {
        OSL_FAIL( "changed control not in property map" );
    }
    return pVal;
}

void PrintDialog::updateWindowFromProperty( const rtl::OUString& i_rProperty )
{
    beans::PropertyValue* pValue = maPController->getValue( i_rProperty );
    std::map< rtl::OUString, std::vector< Window* > >::const_iterator it = maPropertyToWindowMap.find( i_rProperty );
    if( pValue && it != maPropertyToWindowMap.end() )
    {
        const std::vector< Window* >& rWindows( it->second );
        if( ! rWindows.empty() )
        {
            sal_Bool bVal = sal_False;
            sal_Int32 nVal = -1;
            if( pValue->Value >>= bVal )
            {
                // we should have a CheckBox for this one
                CheckBox* pBox = dynamic_cast< CheckBox* >( rWindows.front() );
                if( pBox )
                {
                    pBox->Check( bVal );
                }
                else if ( i_rProperty == "PrintProspect" )
                {
                    // EVIL special case
                    if( bVal )
                        maNUpPage.maBrochureBtn.Check();
                    else
                        maNUpPage.maPagesBtn.Check();
                }
                else
                {
                    DBG_ASSERT( 0, "missing a checkbox" );
                }
            }
            else if( pValue->Value >>= nVal )
            {
                // this could be a ListBox or a RadioButtonGroup
                ListBox* pList = dynamic_cast< ListBox* >( rWindows.front() );
                if( pList )
                {
                    pList->SelectEntryPos( static_cast< sal_uInt16 >(nVal) );
                }
                else if( nVal >= 0 && nVal < sal_Int32(rWindows.size() ) )
                {
                    RadioButton* pBtn = dynamic_cast< RadioButton* >( rWindows[nVal] );
                    DBG_ASSERT( pBtn, "unexpected control for property" );
                    if( pBtn )
                        pBtn->Check();
                }
            }
        }
    }
}

void PrintDialog::makeEnabled( Window* i_pWindow )
{
    std::map< Window*, rtl::OUString >::const_iterator it = maControlToPropertyMap.find( i_pWindow );
    if( it != maControlToPropertyMap.end() )
    {
        rtl::OUString aDependency( maPController->makeEnabled( it->second ) );
        if( !aDependency.isEmpty() )
            updateWindowFromProperty( aDependency );
    }
}

IMPL_LINK( PrintDialog, UIOption_CheckHdl, CheckBox*, i_pBox )
{
    PropertyValue* pVal = getValueForWindow( i_pBox );
    if( pVal )
    {
        makeEnabled( i_pBox );

        sal_Bool bVal = i_pBox->IsChecked();
        pVal->Value <<= bVal;

        checkOptionalControlDependencies();

        // update preview and page settings
        preparePreview();
    }
    return 0;
}

IMPL_LINK( PrintDialog, UIOption_RadioHdl, RadioButton*, i_pBtn )
{
    // this handler gets called for all radiobuttons that get unchecked, too
    // however we only want one notificaction for the new value (that is for
    // the button that gets checked)
    if( i_pBtn->IsChecked() )
    {
        PropertyValue* pVal = getValueForWindow( i_pBtn );
        std::map< Window*, sal_Int32 >::const_iterator it = maControlToNumValMap.find( i_pBtn );
        if( pVal && it != maControlToNumValMap.end() )
        {
            makeEnabled( i_pBtn );

            sal_Int32 nVal = it->second;
            pVal->Value <<= nVal;

            checkOptionalControlDependencies();

            // update preview and page settings
            preparePreview();
        }
    }
    return 0;
}

IMPL_LINK( PrintDialog, UIOption_SelectHdl, ListBox*, i_pBox )
{
    PropertyValue* pVal = getValueForWindow( i_pBox );
    if( pVal )
    {
        makeEnabled( i_pBox );

        sal_Int32 nVal( i_pBox->GetSelectEntryPos() );
        pVal->Value <<= nVal;

        checkOptionalControlDependencies();

        // update preview and page settings
        preparePreview();
    }
    return 0;
}

IMPL_LINK( PrintDialog, UIOption_ModifyHdl, Edit*, i_pBox )
{
    PropertyValue* pVal = getValueForWindow( i_pBox );
    if( pVal )
    {
        makeEnabled( i_pBox );

        NumericField* pNum = dynamic_cast<NumericField*>(i_pBox);
        MetricField* pMetric = dynamic_cast<MetricField*>(i_pBox);
        if( pNum )
        {
            sal_Int64 nVal = pNum->GetValue();
            pVal->Value <<= nVal;
        }
        else if( pMetric )
        {
            sal_Int64 nVal = pMetric->GetValue();
            pVal->Value <<= nVal;
        }
        else
        {
            rtl::OUString aVal( i_pBox->GetText() );
            pVal->Value <<= aVal;
        }

        checkOptionalControlDependencies();

        // update preview and page settings
        preparePreview();
    }
    return 0;
}

void PrintDialog::Command( const CommandEvent& rEvt )
{
    if( rEvt.GetCommand() == COMMAND_WHEEL )
    {
        const CommandWheelData* pWheelData = rEvt.GetWheelData();
        if( pWheelData->GetDelta() > 0 )
            previewForward();
        else if( pWheelData->GetDelta() < 0 )
            previewBackward();
        /*
        else
            huh ?
        */
    }
}

void PrintDialog::Resize()
{
    // maLayout.setManagedArea( Rectangle( Point( 0, 0 ), GetSizePixel() ) );
    // and do the preview; however the metafile does not need to be gotten anew
    preparePreview( false );

    // do an invalidate for the benefit of the grouping elements
    Invalidate();
}

void PrintDialog::previewForward()
{
    maPageEdit.Up();
}

void PrintDialog::previewBackward()
{
    maPageEdit.Down();
}

// -----------------------------------------------------------------------------
//
// PrintProgressDialog
//
// -----------------------------------------------------------------------------

PrintProgressDialog::PrintProgressDialog( Window* i_pParent, int i_nMax ) :
    ModelessDialog( i_pParent, VclResId( SV_DLG_PRINT_PROGRESS ) ),
    maText( this, VclResId( SV_PRINT_PROGRESS_TEXT ) ),
    maButton( this, VclResId( SV_PRINT_PROGRESS_CANCEL ) ),
    mbCanceled( false ),
    mnCur( 0 ),
    mnMax( i_nMax ),
    mnProgressHeight( 15 ),
    mbNativeProgress( false )
{
    FreeResource();

    if( mnMax < 1 )
        mnMax = 1;

    maStr = maText.GetText();

    maButton.SetClickHdl( LINK( this, PrintProgressDialog, ClickHdl ) );

}

PrintProgressDialog::~PrintProgressDialog()
{
}

IMPL_LINK( PrintProgressDialog, ClickHdl, Button*, pButton )
{
    if( pButton == &maButton )
        mbCanceled = true;

    return 0;
}

void PrintProgressDialog::implCalcProgressRect()
{
    if( IsNativeControlSupported( CTRL_PROGRESS, PART_ENTIRE_CONTROL ) )
    {
        ImplControlValue aValue;
        Rectangle aControlRegion( Point(), Size( 100, mnProgressHeight ) );
        Rectangle aNativeControlRegion, aNativeContentRegion;
        if( GetNativeControlRegion( CTRL_PROGRESS, PART_ENTIRE_CONTROL, aControlRegion,
                                    CTRL_STATE_ENABLED, aValue, rtl::OUString(),
                                    aNativeControlRegion, aNativeContentRegion ) )
        {
            mnProgressHeight = aNativeControlRegion.GetHeight();
        }
        mbNativeProgress = true;
    }
    maProgressRect = Rectangle( Point( 10, maText.GetPosPixel().Y() + maText.GetSizePixel().Height() + 8 ),
                                Size( GetSizePixel().Width() - 20, mnProgressHeight ) );
}

void PrintProgressDialog::setProgress( int i_nCurrent, int i_nMax )
{
    if( maProgressRect.IsEmpty() )
        implCalcProgressRect();

    mnCur = i_nCurrent;
    if( i_nMax != -1 )
        mnMax = i_nMax;

    if( mnMax < 1 )
        mnMax = 1;

    rtl::OUString aNewText( searchAndReplace( maStr, "%p", 2, rtl::OUString::valueOf( mnCur ) ) );
    aNewText = searchAndReplace( aNewText, "%n", 2, rtl::OUString::valueOf( mnMax ) );
    maText.SetText( aNewText );

    // update progress
    Invalidate( maProgressRect, INVALIDATE_UPDATE );
}

void PrintProgressDialog::tick()
{
    if( mnCur < mnMax )
        setProgress( ++mnCur );
}

void PrintProgressDialog::reset()
{
    mbCanceled = false;
    setProgress( 0 );
}

void PrintProgressDialog::Paint( const Rectangle& )
{
    if( maProgressRect.IsEmpty() )
        implCalcProgressRect();

    Push( PUSH_LINECOLOR | PUSH_FILLCOLOR );
    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
    Color aPrgsColor = rStyleSettings.GetHighlightColor();
    if ( aPrgsColor == rStyleSettings.GetFaceColor() )
        aPrgsColor = rStyleSettings.GetDarkShadowColor();
    SetLineColor();
    SetFillColor( aPrgsColor );

    const long nOffset = 3;
    const long nWidth = 3*mnProgressHeight/2;
    const long nFullWidth = nWidth + nOffset;
    const long nMaxCount = maProgressRect.GetWidth() / nFullWidth;
    DrawProgress( this, maProgressRect.TopLeft(),
                        nOffset,
                        nWidth,
                        mnProgressHeight,
                        static_cast<sal_uInt16>(0),
                        static_cast<sal_uInt16>(10000*mnCur/mnMax),
                        static_cast<sal_uInt16>(10000/nMaxCount),
                        maProgressRect
                        );
    Pop();

    if( ! mbNativeProgress )
    {
        DecorationView aDecoView( this );
        Rectangle aFrameRect( maProgressRect );
        aFrameRect.Left() -= nOffset;
        aFrameRect.Right() += nOffset;
        aFrameRect.Top() -= nOffset;
        aFrameRect.Bottom() += nOffset;
        aDecoView.DrawFrame( aFrameRect );
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
