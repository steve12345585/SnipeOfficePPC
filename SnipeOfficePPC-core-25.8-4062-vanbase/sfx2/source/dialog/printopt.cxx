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


#include <sal/macros.h>
#include <vcl/msgbox.hxx>
#include <unotools/printwarningoptions.hxx>
#include <svtools/printoptions.hxx>
#include <svtools/restartdialog.hxx>
#include <svl/flagitem.hxx>

#include <comphelper/processfactory.hxx>

#include "printopt.hrc"
#include "dialog.hrc"
#include "sfx2/sfxresid.hxx"
#include <sfx2/viewsh.hxx>
#include <sfx2/printopt.hxx>

static sal_uInt16   aDPIArray[] = { 72, 96, 150, 200, 300, 600 };
static sal_Bool     bOutputForPrinter = sal_True;

#define DPI_COUNT (sizeof(aDPIArray)/sizeof(aDPIArray[0 ]))

SfxCommonPrintOptionsTabPage::SfxCommonPrintOptionsTabPage( Window* pParent, const SfxItemSet& rSet ) :
    SfxTabPage( pParent, SfxResId( TP_COMMONPRINTOPTIONS ), rSet ),

    aReduceGB( this, SfxResId( GB_REDUCE ) ),
    aOutputTypeFT( this, SfxResId( FT_OUTPUTTYPE ) ),
    aPrinterOutputRB( this, SfxResId( RB_PRINTEROUTPUT ) ),
    aPrintFileOutputRB( this, SfxResId( RB_PRINTFILEOUTPUT ) ),
    aOutputGB( this, SfxResId( GB_OUTPUT ) ),
    aReduceTransparencyCB( this, SfxResId( CB_REDUCETRANSPARENCY ) ),
    aReduceTransparencyAutoRB( this, SfxResId( RB_REDUCETRANSPARENCY_AUTO ) ),
    aReduceTransparencyNoneRB( this, SfxResId( RB_REDUCETRANSPARENCY_NONE ) ),
    aReduceGradientsCB( this, SfxResId( CB_REDUCEGRADIENTS ) ),
    aReduceGradientsStripesRB( this, SfxResId( RB_REDUCEGRADIENTS_STRIPES ) ),
    aReduceGradientsColorRB( this, SfxResId( RB_REDUCEGRADIENTS_COLOR ) ),
    aReduceGradientsStepCountNF( this, SfxResId( NF_REDUCEGRADIENTS_STEPCOUNT ) ),
    aReduceBitmapsCB( this, SfxResId( CB_REDUCEBITMAPS ) ),
    aReduceBitmapsOptimalRB( this, SfxResId( RB_REDUCEBITMAPS_OPTIMAL ) ),
    aReduceBitmapsNormalRB( this, SfxResId( RB_REDUCEBITMAPS_NORMAL ) ),
    aReduceBitmapsResolutionRB( this, SfxResId( RB_REDUCEBITMAPS_RESOLUTION ) ),
    aReduceBitmapsResolutionLB( this, SfxResId( LB_REDUCEBITMAPS_RESOLUTION ) ),
    aReduceBitmapsTransparencyCB( this, SfxResId( CB_REDUCEBITMAPS_TRANSPARENCY ) ),
    aConvertToGreyscalesCB( this, SfxResId( CB_CONVERTTOGREYSCALES ) ),
    aPDFCB( this, SfxResId( CB_PDF ) ),
    aWarnGB( this, SfxResId( GB_PRINT_WARN ) ),
    aPaperSizeCB( this, SfxResId( CB_PAPERSIZE ) ),
    aPaperOrientationCB( this, SfxResId( CB_PAPERORIENTATION ) ),
    aTransparencyCB( this, SfxResId( CB_TRANSPARENCY ) )
{
    FreeResource();

#ifndef ENABLE_CUPS
    long nDiff = aWarnGB.GetPosPixel().Y() - aPDFCB.GetPosPixel().Y();
    aPDFCB.Hide();

    Point aPoint;

    aPoint = aWarnGB.GetPosPixel();
    aPoint.Y() -= nDiff;
    aWarnGB.SetPosPixel(aPoint);

    aPoint = aPaperSizeCB.GetPosPixel();
    aPoint.Y() -= nDiff;
    aPaperSizeCB.SetPosPixel(aPoint);

    aPoint = aPaperOrientationCB.GetPosPixel();
    aPoint.Y() -= nDiff;
    aPaperOrientationCB.SetPosPixel(aPoint);

    aPoint = aTransparencyCB.GetPosPixel();
    aPoint.Y() -= nDiff;
    aTransparencyCB.SetPosPixel(aPoint);
#endif

    aOutputGB.SetStyle( aOutputGB.GetStyle() | WB_NOLABEL );

    if( bOutputForPrinter )
    {
        aPrinterOutputRB.Check( sal_True );
        aOutputGB.SetText( OutputDevice::GetNonMnemonicString( aPrinterOutputRB.GetText() ) );
    }
    else
    {
        aPrintFileOutputRB.Check( sal_True );
        aOutputGB.SetText( OutputDevice::GetNonMnemonicString( aPrintFileOutputRB.GetText() ) );
        aPDFCB.Disable();
    }

    aPrinterOutputRB.SetToggleHdl( LINK( this, SfxCommonPrintOptionsTabPage, ToggleOutputPrinterRBHdl ) );
    aPrintFileOutputRB.SetToggleHdl( LINK( this, SfxCommonPrintOptionsTabPage, ToggleOutputPrintFileRBHdl ) );

    aReduceTransparencyCB.SetClickHdl( LINK( this, SfxCommonPrintOptionsTabPage, ClickReduceTransparencyCBHdl ) );
    aReduceGradientsCB.SetClickHdl( LINK( this, SfxCommonPrintOptionsTabPage, ClickReduceGradientsCBHdl ) );
    aReduceBitmapsCB.SetClickHdl( LINK( this, SfxCommonPrintOptionsTabPage, ClickReduceBitmapsCBHdl ) );

    aReduceGradientsStripesRB.SetToggleHdl( LINK( this, SfxCommonPrintOptionsTabPage, ToggleReduceGradientsStripesRBHdl ) );
    aReduceBitmapsResolutionRB.SetToggleHdl( LINK( this, SfxCommonPrintOptionsTabPage, ToggleReduceBitmapsResolutionRBHdl ) );

    // #i89164# calculate dynamically the width of radiobutton and listbox
    const long nOffset = 10;
    Size aOldSize = aReduceBitmapsResolutionRB.GetSizePixel();
    Size aNewSize = aReduceBitmapsResolutionRB.GetOptimalSize( WINDOWSIZE_PREFERRED );
    aNewSize.Width() += nOffset;
    aNewSize.Height() = aOldSize.Height();
    long nDelta = aOldSize.Width() - aNewSize.Width();
    aReduceBitmapsResolutionRB.SetSizePixel( aNewSize );
    Point aPos = aReduceBitmapsResolutionLB.GetPosPixel();
    aPos.X() -= nDelta;
    aOldSize = aReduceBitmapsResolutionLB.GetSizePixel();
    aNewSize = aReduceBitmapsResolutionLB.GetOptimalSize( WINDOWSIZE_PREFERRED );
    aNewSize.Width() += nOffset;
    aNewSize.Height() = aOldSize.Height();
    aReduceBitmapsResolutionLB.SetPosSizePixel( aPos, aNewSize );
}

SfxCommonPrintOptionsTabPage::~SfxCommonPrintOptionsTabPage()
{
}

SfxTabPage* SfxCommonPrintOptionsTabPage::Create( Window* pParent, const SfxItemSet& rAttrSet )
{
    return( new SfxCommonPrintOptionsTabPage( pParent, rAttrSet ) );
}

Window* SfxCommonPrintOptionsTabPage::GetParentLabeledBy( const Window* pWindow ) const
{
    if ( pWindow == (Window *)&aReduceGradientsStepCountNF )
        return (Window *)&aReduceGradientsStripesRB;
    else if ( pWindow == (Window *)&aReduceBitmapsResolutionLB )
        return (Window *)&aReduceBitmapsResolutionRB;
    else
        return SfxTabPage::GetParentLabeledBy( pWindow );
}

Window* SfxCommonPrintOptionsTabPage::GetParentLabelFor( const Window* pWindow ) const
{
    if ( pWindow == (Window *)&aReduceGradientsStripesRB )
        return (Window *)&aReduceGradientsStepCountNF;
    else if ( pWindow == (Window *)&aReduceBitmapsResolutionRB )
        return (Window *)&aReduceBitmapsResolutionLB;
    else
        return SfxTabPage::GetParentLabelFor( pWindow );
}

sal_Bool SfxCommonPrintOptionsTabPage::FillItemSet( SfxItemSet& /*rSet*/ )
{
    SvtPrintWarningOptions  aWarnOptions;
    SvtPrinterOptions       aPrinterOptions;
    SvtPrintFileOptions     aPrintFileOptions;
    sal_Bool                    bModified = sal_False;


    if( aPaperSizeCB.IsChecked() != aPaperSizeCB.GetSavedValue())
        aWarnOptions.SetPaperSize(aPaperSizeCB.IsChecked());
    if( aPaperOrientationCB.IsChecked() != aPaperOrientationCB.GetSavedValue() )
        aWarnOptions.SetPaperOrientation(aPaperOrientationCB.IsChecked());

    if( aTransparencyCB.IsChecked() != aTransparencyCB.GetSavedValue() )
        aWarnOptions.SetTransparency( aTransparencyCB.IsChecked() );

    ImplSaveControls( aPrinterOutputRB.IsChecked() ? &maPrinterOptions : &maPrintFileOptions );

    aPrinterOptions.SetPrinterOptions( maPrinterOptions );
    aPrintFileOptions.SetPrinterOptions( maPrintFileOptions );

    return bModified;
}

void SfxCommonPrintOptionsTabPage::Reset( const SfxItemSet& /*rSet*/ )
{
    SvtPrintWarningOptions  aWarnOptions;
    SvtPrinterOptions       aPrinterOptions;
    SvtPrintFileOptions     aPrintFileOptions;

    aPaperSizeCB.Check( aWarnOptions.IsPaperSize() );
    aPaperOrientationCB.Check( aWarnOptions.IsPaperOrientation() );

    aTransparencyCB.Check( aWarnOptions.IsTransparency() );

    aPaperSizeCB.SaveValue();
    aPaperOrientationCB.SaveValue();
    aTransparencyCB.SaveValue();

    aPrinterOptions.GetPrinterOptions( maPrinterOptions );
    aPrintFileOptions.GetPrinterOptions( maPrintFileOptions );

    ImplUpdateControls( aPrinterOutputRB.IsChecked() ? &maPrinterOptions : &maPrintFileOptions );

    // #i63982#
    ImplSetAccessibleNames();
}

int SfxCommonPrintOptionsTabPage::DeactivatePage( SfxItemSet* pItemSet )
{
    if( pItemSet )
        FillItemSet( *pItemSet );

    return LEAVE_PAGE;
}

void SfxCommonPrintOptionsTabPage::ImplUpdateControls( const PrinterOptions* pCurrentOptions )
{
    aReduceTransparencyCB.Check( pCurrentOptions->IsReduceTransparency() );

    if( pCurrentOptions->GetReducedTransparencyMode() == PRINTER_TRANSPARENCY_AUTO )
        aReduceTransparencyAutoRB.Check( sal_True );
    else
        aReduceTransparencyNoneRB.Check( sal_True );

    aReduceGradientsCB.Check( pCurrentOptions->IsReduceGradients() );

    if( pCurrentOptions->GetReducedGradientMode() == PRINTER_GRADIENT_STRIPES )
        aReduceGradientsStripesRB.Check( sal_True );
    else
        aReduceGradientsColorRB.Check( sal_True );

    aReduceGradientsStepCountNF.SetValue( pCurrentOptions->GetReducedGradientStepCount() );

    aReduceBitmapsCB.Check( pCurrentOptions->IsReduceBitmaps() );

    if( pCurrentOptions->GetReducedBitmapMode() == PRINTER_BITMAP_OPTIMAL )
        aReduceBitmapsOptimalRB.Check( sal_True );
    else if( pCurrentOptions->GetReducedBitmapMode() == PRINTER_BITMAP_NORMAL )
        aReduceBitmapsNormalRB.Check( sal_True );
    else
        aReduceBitmapsResolutionRB.Check( sal_True );

    const sal_uInt16 nDPI = pCurrentOptions->GetReducedBitmapResolution();

    if( nDPI < aDPIArray[ 0 ] )
        aReduceBitmapsResolutionLB.SelectEntryPos( 0 );
    else
    {
        for( long i = ( DPI_COUNT - 1 ); i >= 0; i-- )
        {
            if( nDPI >= aDPIArray[ i ] )
            {
                aReduceBitmapsResolutionLB.SelectEntryPos( (sal_uInt16) i );
                i = -1;
            }
        }
    }

    aReduceBitmapsResolutionLB.SetText( aReduceBitmapsResolutionLB.GetEntry( aReduceBitmapsResolutionLB.GetSelectEntryPos() ) );

    aReduceBitmapsTransparencyCB.Check( pCurrentOptions->IsReducedBitmapIncludesTransparency() );
    aConvertToGreyscalesCB.Check( pCurrentOptions->IsConvertToGreyscales() );
    aPDFCB.Check( pCurrentOptions->IsPDFAsStandardPrintJobFormat() );

    ClickReduceTransparencyCBHdl( &aReduceTransparencyCB );
    ClickReduceGradientsCBHdl( &aReduceGradientsCB );
    ClickReduceBitmapsCBHdl( &aReduceBitmapsCB );
}

void SfxCommonPrintOptionsTabPage::ImplSetAccessibleNames()
{
    const rtl::OUString cSeparator(" - ");

    String sReduceText = aReduceGB.GetDisplayText();
    sReduceText += cSeparator;

    String sAccessibleName = sReduceText;
    sAccessibleName += aPrinterOutputRB.GetDisplayText();
    aPrinterOutputRB.SetAccessibleName( sAccessibleName );

    sAccessibleName = sReduceText;
    sAccessibleName += aPrintFileOutputRB.GetDisplayText();
    aPrintFileOutputRB.SetAccessibleName( sAccessibleName );

    String sOutputText = sReduceText;
    sOutputText += aOutputGB.GetDisplayText();
    sOutputText += cSeparator;

    sAccessibleName = sOutputText;
    sAccessibleName += aReduceTransparencyCB.GetDisplayText();
    aReduceTransparencyCB.SetAccessibleName( sAccessibleName );

    String sTransparencyText = aReduceTransparencyCB.GetAccessibleName();
    sTransparencyText += cSeparator;

    sAccessibleName = sTransparencyText;
    sAccessibleName += aReduceTransparencyAutoRB.GetDisplayText();
    aReduceTransparencyAutoRB.SetAccessibleName( sAccessibleName );

    sAccessibleName = sTransparencyText;
    sAccessibleName += aReduceTransparencyNoneRB.GetDisplayText();
    aReduceTransparencyNoneRB.SetAccessibleName( sAccessibleName );

    sAccessibleName = sOutputText;
    sAccessibleName += aReduceGradientsCB.GetDisplayText();
    aReduceGradientsCB.SetAccessibleName( sAccessibleName );

    String sGradientText = aReduceGradientsCB.GetAccessibleName();
    sGradientText += cSeparator;

    sAccessibleName = sGradientText;
    sAccessibleName += aReduceGradientsStripesRB.GetDisplayText();
    aReduceGradientsStripesRB.SetAccessibleName( sAccessibleName );

    sAccessibleName = aReduceGradientsStripesRB.GetAccessibleName();
    aReduceGradientsStepCountNF.SetAccessibleName( sAccessibleName );

    sAccessibleName = sGradientText;
    sAccessibleName += aReduceGradientsColorRB.GetDisplayText();
    aReduceGradientsColorRB.SetAccessibleName( sAccessibleName );

    sAccessibleName = sOutputText;
    sAccessibleName += aReduceBitmapsCB.GetDisplayText();
    aReduceBitmapsCB.SetAccessibleName( sAccessibleName );

    String sBitmapText = aReduceBitmapsCB.GetAccessibleName();
    sBitmapText += cSeparator;

    sAccessibleName = sBitmapText;
    sAccessibleName += aReduceBitmapsOptimalRB.GetDisplayText();
    aReduceBitmapsOptimalRB.SetAccessibleName( sAccessibleName );

    sAccessibleName = sBitmapText;
    sAccessibleName += aReduceBitmapsNormalRB.GetDisplayText();
    aReduceBitmapsNormalRB.SetAccessibleName( sAccessibleName );

    sAccessibleName = sBitmapText;
    sAccessibleName += aReduceBitmapsResolutionRB.GetDisplayText();
    aReduceBitmapsResolutionRB.SetAccessibleName( sAccessibleName );

    sAccessibleName = aReduceBitmapsResolutionRB.GetAccessibleName();
    aReduceBitmapsResolutionLB.SetAccessibleName( sAccessibleName );

    sAccessibleName = sBitmapText;
    sAccessibleName += aReduceBitmapsTransparencyCB.GetDisplayText();
    aReduceBitmapsTransparencyCB.SetAccessibleName( sAccessibleName );

    sAccessibleName = sOutputText;
    sAccessibleName += aConvertToGreyscalesCB.GetDisplayText();
    aConvertToGreyscalesCB.SetAccessibleName( sAccessibleName );

    String sWarnText = aWarnGB.GetDisplayText();
    sWarnText += cSeparator;

    sAccessibleName = sWarnText;
    sAccessibleName += aPaperSizeCB.GetDisplayText();
    aPaperSizeCB.SetAccessibleName( sAccessibleName );

    sAccessibleName = sWarnText;
    sAccessibleName += aPaperOrientationCB.GetDisplayText();
    aPaperOrientationCB.SetAccessibleName( sAccessibleName );

    sAccessibleName = sWarnText;
    sAccessibleName += aTransparencyCB.GetDisplayText();
    aTransparencyCB.SetAccessibleName( sAccessibleName );
}

void SfxCommonPrintOptionsTabPage::ImplSaveControls( PrinterOptions* pCurrentOptions )
{
    pCurrentOptions->SetReduceTransparency( aReduceTransparencyCB.IsChecked() );
    pCurrentOptions->SetReducedTransparencyMode( aReduceTransparencyAutoRB.IsChecked() ? PRINTER_TRANSPARENCY_AUTO : PRINTER_TRANSPARENCY_NONE );
    pCurrentOptions->SetReduceGradients( aReduceGradientsCB.IsChecked() );
    pCurrentOptions->SetReducedGradientMode( aReduceGradientsStripesRB.IsChecked() ? PRINTER_GRADIENT_STRIPES : PRINTER_GRADIENT_COLOR  );
    pCurrentOptions->SetReducedGradientStepCount( (sal_uInt16) aReduceGradientsStepCountNF.GetValue() );
    pCurrentOptions->SetReduceBitmaps( aReduceBitmapsCB.IsChecked() );
    pCurrentOptions->SetReducedBitmapMode( aReduceBitmapsOptimalRB.IsChecked() ? PRINTER_BITMAP_OPTIMAL :
                                           ( aReduceBitmapsNormalRB.IsChecked() ? PRINTER_BITMAP_NORMAL : PRINTER_BITMAP_RESOLUTION ) );
    pCurrentOptions->SetReducedBitmapResolution( aDPIArray[ Min( (sal_uInt16) aReduceBitmapsResolutionLB.GetSelectEntryPos(),
                                                            (sal_uInt16)( (sizeof (aDPIArray) / sizeof (aDPIArray[0])) - 1 ) ) ] );
    pCurrentOptions->SetReducedBitmapIncludesTransparency( aReduceBitmapsTransparencyCB.IsChecked() );
    pCurrentOptions->SetConvertToGreyscales( aConvertToGreyscalesCB.IsChecked() );
    sal_Bool bOrigBackEnd = pCurrentOptions->IsPDFAsStandardPrintJobFormat();
    if (bOrigBackEnd != aPDFCB.IsChecked())
    {
        pCurrentOptions->SetPDFAsStandardPrintJobFormat( aPDFCB.IsChecked() );
            svtools::executeRestartDialog(
                comphelper::getProcessComponentContext(), 0,
                svtools::RESTART_REASON_PDF_AS_STANDARD_JOB_FORMAT);
    }
}

IMPL_LINK( SfxCommonPrintOptionsTabPage, ClickReduceTransparencyCBHdl, CheckBox*, pBox )
{
    (void)pBox; //unused
    const sal_Bool bReduceTransparency = aReduceTransparencyCB.IsChecked();

    aReduceTransparencyAutoRB.Enable( bReduceTransparency );
    aReduceTransparencyNoneRB.Enable( bReduceTransparency );

    aTransparencyCB.Enable( !bReduceTransparency );

    return 0;
}

IMPL_LINK( SfxCommonPrintOptionsTabPage, ClickReduceGradientsCBHdl, CheckBox*, pBox )
{
    (void)pBox; //unused
    const sal_Bool bEnable = aReduceGradientsCB.IsChecked();

    aReduceGradientsStripesRB.Enable( bEnable );
    aReduceGradientsColorRB.Enable( bEnable );
    aReduceGradientsStepCountNF.Enable( bEnable );

    ToggleReduceGradientsStripesRBHdl( &aReduceGradientsStripesRB );

    return 0;
}

IMPL_LINK( SfxCommonPrintOptionsTabPage, ClickReduceBitmapsCBHdl, CheckBox*, pBox )
{
    (void)pBox; //unused
    const sal_Bool bEnable = aReduceBitmapsCB.IsChecked();

    aReduceBitmapsOptimalRB.Enable( bEnable );
    aReduceBitmapsNormalRB.Enable( bEnable );
    aReduceBitmapsResolutionRB.Enable( bEnable );
    aReduceBitmapsTransparencyCB.Enable( bEnable );
    aReduceBitmapsResolutionLB.Enable( bEnable );

    ToggleReduceBitmapsResolutionRBHdl( &aReduceBitmapsResolutionRB );

    return 0;
}

IMPL_LINK( SfxCommonPrintOptionsTabPage, ToggleReduceGradientsStripesRBHdl, RadioButton*, pButton )
{
    (void)pButton; //unused
    const sal_Bool bEnable = aReduceGradientsCB.IsChecked() && aReduceGradientsStripesRB.IsChecked();

    aReduceGradientsStepCountNF.Enable( bEnable );

    return 0;
}

IMPL_LINK( SfxCommonPrintOptionsTabPage, ToggleReduceBitmapsResolutionRBHdl, RadioButton*, pButton )
{
    (void)pButton; //unused
    const sal_Bool bEnable = aReduceBitmapsCB.IsChecked() && aReduceBitmapsResolutionRB.IsChecked();

    aReduceBitmapsResolutionLB.Enable( bEnable );

    return 0;
}

IMPL_LINK( SfxCommonPrintOptionsTabPage, ToggleOutputPrinterRBHdl, RadioButton*, pButton )
{
    if( pButton->IsChecked() )
    {
        aOutputGB.SetText( OutputDevice::GetNonMnemonicString( pButton->GetText() ) );
        ImplUpdateControls( &maPrinterOptions );
        bOutputForPrinter = sal_True;
        // #i63982#
        ImplSetAccessibleNames();
    }
    else
        ImplSaveControls( &maPrinterOptions );

    return 0;
}

IMPL_LINK( SfxCommonPrintOptionsTabPage, ToggleOutputPrintFileRBHdl, RadioButton*, pButton )
{
    if( pButton->IsChecked() )
    {
        aOutputGB.SetText( OutputDevice::GetNonMnemonicString( pButton->GetText() ) );
        ImplUpdateControls( &maPrintFileOptions );
        bOutputForPrinter = sal_False;
        // #i63982#
        ImplSetAccessibleNames();
        aPDFCB.Disable();
    }
    else
    {
        ImplSaveControls( &maPrintFileOptions );
        aPDFCB.Enable();
    }

    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
