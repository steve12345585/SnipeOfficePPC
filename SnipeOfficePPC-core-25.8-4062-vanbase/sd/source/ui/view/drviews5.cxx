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


#include "DrawViewShell.hxx"
#include "PresentationViewShell.hxx"
#include <editeng/outliner.hxx>
#include <svx/svxids.hrc>
#include <sfx2/request.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/svdpagv.hxx>
#include <vcl/scrbar.hxx>
#include <tools/poly.hxx>
#include <svx/fmshell.hxx>
#include <editeng/eeitem.hxx>
#include <svtools/colorcfg.hxx>
#include "AccessibleDrawDocumentView.hxx"

#include <sfx2/viewfrm.hxx>
#include "LayerTabBar.hxx"

#include "strings.hrc"
#include "res_bmp.hrc"
#include "glob.hrc"
#include "app.hrc"
#include "helpids.h"
#include "optsitem.hxx"
#include "app.hxx"
#include "FrameView.hxx"
#include "sdattr.hxx"
#include "futext.hxx"
#include "sdpage.hxx"
#include "stlpool.hxx"
#include "prntopts.hxx"
#include "sdresid.hxx"
#include "Window.hxx"
#include "drawview.hxx"
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#include "Outliner.hxx"
#include "Client.hxx"
#include "slideshow.hxx"
#include "unokywds.hxx"
#include "SdUnoDrawView.hxx"
#include "ViewShellBase.hxx"
#include "FormShellManager.hxx"
#include "LayerDialogContent.hxx"
#include "DrawController.hxx"

namespace sd {

static const int TABCONTROL_INITIAL_SIZE = 350;
static const int PAPER_SHADOW_EXT_PIXEL = 2;


/*************************************************************************
|*
|* Wird gerufen, wenn sich das Model aendert
|*
\************************************************************************/

void DrawViewShell::ModelHasChanged()
{
    Invalidate();
    // Damit der Navigator auch einen aktuellen Status bekommt
    GetViewFrame()->GetBindings().Invalidate( SID_NAVIGATOR_STATE, sal_True, sal_False );

    SfxBoolItem aItem( SID_3D_STATE, sal_True );
    GetViewFrame()->GetDispatcher()->Execute(
        SID_3D_STATE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );

    // jetzt den von der Drawing Engine neu erzeugten TextEditOutliner
    // initialisieren
    ::Outliner* pOutliner     = mpDrawView->GetTextEditOutliner();
    if (pOutliner)
    {
        SfxStyleSheetPool* pSPool = (SfxStyleSheetPool*) GetDocSh()->GetStyleSheetPool();
        pOutliner->SetStyleSheetPool(pSPool);
    }
}




void DrawViewShell::Resize (void)
{
    ViewShell::Resize();

    if ( GetDocSh()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
    {
        SetZoomRect( GetDocSh()->GetVisArea(ASPECT_CONTENT) );
    }

    rtl::Reference< sd::SlideShow > xSlideshow( SlideShow::GetSlideShow( GetViewShellBase() ) );
    if( xSlideshow.is() && xSlideshow->isRunning() && !xSlideshow->isFullScreen() )
    {
        xSlideshow->resize(maViewSize);
    }
}




void DrawViewShell::ArrangeGUIElements (void)
{
    // Retrieve the current size (thickness) of the scroll bars.  That is
    // the width of the vertical and the height of the horizontal scroll
    // bar.
    int nScrollBarSize =
        GetParentWindow()->GetSettings().GetStyleSettings().GetScrollBarSize();
    maScrBarWH = Size (nScrollBarSize, nScrollBarSize);

    Point aHPos = maViewPos;
    aHPos.Y() += maViewSize.Height();


    ViewShell::ArrangeGUIElements ();

    maTabControl.Hide();

    OSL_ASSERT (GetViewShell()!=NULL);
    Client* pIPClient = static_cast<Client*>(GetViewShell()->GetIPClient());
    sal_Bool bClientActive = sal_False;
    if ( pIPClient && pIPClient->IsObjectInPlaceActive() )
        bClientActive = sal_True;

    sal_Bool bInPlaceActive = GetViewFrame()->GetFrame().IsInPlace();

    if ( mbZoomOnPage && !bInPlaceActive && !bClientActive )
    {
        // bei Split immer erstes Fenster resizen
        //af pWindow = mpContentWindow.get();
        SfxRequest aReq(SID_SIZE_PAGE, 0, GetDoc()->GetItemPool());
        ExecuteSlot( aReq );
    }
}

/*************************************************************************
|*
|* Daten der FrameView auf die aktuelle View uebertragen
|*
\************************************************************************/

void DrawViewShell::ReadFrameViewData(FrameView* pView)
{
    ModifyGuard aGuard( GetDoc() );

    // Diese Option wird am Model eingestellt
    GetDoc()->SetPickThroughTransparentTextFrames(
             SD_MOD()->GetSdOptions(GetDoc()->GetDocumentType())->IsPickThrough());

    // Initialisierungen der Zeichen-(Bildschirm-)Attribute
    if (HasRuler() != pView->HasRuler())
        SetRuler( pView->HasRuler() );

    if (mpDrawView->GetGridCoarse() != pView->GetGridCoarse())
        mpDrawView->SetGridCoarse( pView->GetGridCoarse() );

    if (mpDrawView->GetGridFine() != pView->GetGridFine())
        mpDrawView->SetGridFine( pView->GetGridFine() );

    if (mpDrawView->GetSnapGridWidthX() != pView->GetSnapGridWidthX() || mpDrawView->GetSnapGridWidthY() != pView->GetSnapGridWidthY())
        mpDrawView->SetSnapGridWidth(pView->GetSnapGridWidthX(), pView->GetSnapGridWidthY());

    if (mpDrawView->IsGridVisible() != pView->IsGridVisible())
        mpDrawView->SetGridVisible( pView->IsGridVisible() );

    if (mpDrawView->IsGridFront() != pView->IsGridFront())
        mpDrawView->SetGridFront( pView->IsGridFront() );

    if (mpDrawView->GetSnapAngle() != pView->GetSnapAngle())
        mpDrawView->SetSnapAngle( pView->GetSnapAngle() );

    if (mpDrawView->IsGridSnap() !=  pView->IsGridSnap() )
        mpDrawView->SetGridSnap( pView->IsGridSnap() );

    if (mpDrawView->IsBordSnap() !=  pView->IsBordSnap() )
        mpDrawView->SetBordSnap( pView->IsBordSnap() );

    if (mpDrawView->IsHlplSnap() !=  pView->IsHlplSnap() )
        mpDrawView->SetHlplSnap( pView->IsHlplSnap() );

    if (mpDrawView->IsOFrmSnap() !=  pView->IsOFrmSnap() )
        mpDrawView->SetOFrmSnap( pView->IsOFrmSnap() );

    if (mpDrawView->IsOPntSnap() !=  pView->IsOPntSnap() )
        mpDrawView->SetOPntSnap( pView->IsOPntSnap() );

    if (mpDrawView->IsOConSnap() !=  pView->IsOConSnap() )
        mpDrawView->SetOConSnap( pView->IsOConSnap() );

    if (mpDrawView->IsHlplVisible() != pView->IsHlplVisible() )
        mpDrawView->SetHlplVisible( pView->IsHlplVisible() );

    if (mpDrawView->IsDragStripes() != pView->IsDragStripes() )
        mpDrawView->SetDragStripes( pView->IsDragStripes() );

    if (mpDrawView->IsPlusHandlesAlwaysVisible() != pView->IsPlusHandlesAlwaysVisible() )
        mpDrawView->SetPlusHandlesAlwaysVisible( pView->IsPlusHandlesAlwaysVisible() );

    if (mpDrawView->GetSnapMagneticPixel() != pView->GetSnapMagneticPixel() )
        mpDrawView->SetSnapMagneticPixel( pView->GetSnapMagneticPixel() );

    if (mpDrawView->IsMarkedHitMovesAlways() != pView->IsMarkedHitMovesAlways() )
        mpDrawView->SetMarkedHitMovesAlways( pView->IsMarkedHitMovesAlways() );

    if (mpDrawView->IsMoveOnlyDragging() != pView->IsMoveOnlyDragging() )
        mpDrawView->SetMoveOnlyDragging( pView->IsMoveOnlyDragging() );

    if (mpDrawView->IsNoDragXorPolys() != pView->IsNoDragXorPolys() )
        mpDrawView->SetNoDragXorPolys( pView->IsNoDragXorPolys() );

    if (mpDrawView->IsCrookNoContortion() != pView->IsCrookNoContortion() )
        mpDrawView->SetCrookNoContortion( pView->IsCrookNoContortion() );

    if (mpDrawView->IsAngleSnapEnabled() != pView->IsAngleSnapEnabled() )
        mpDrawView->SetAngleSnapEnabled( pView->IsAngleSnapEnabled() );

    if (mpDrawView->IsBigOrtho() != pView->IsBigOrtho() )
        mpDrawView->SetBigOrtho( pView->IsBigOrtho() );

    if (mpDrawView->IsOrtho() != pView->IsOrtho() )
        mpDrawView->SetOrtho( pView->IsOrtho() );

    if (mpDrawView->GetEliminatePolyPointLimitAngle() != pView->GetEliminatePolyPointLimitAngle() )
        mpDrawView->SetEliminatePolyPointLimitAngle( pView->GetEliminatePolyPointLimitAngle() );

    if (mpDrawView->IsEliminatePolyPoints() != pView->IsEliminatePolyPoints() )
        mpDrawView->SetEliminatePolyPoints( pView->IsEliminatePolyPoints() );

    if (mpDrawView->IsSolidDragging() != pView->IsSolidDragging() )
        mpDrawView->SetSolidDragging( pView->IsSolidDragging() );

    if (mpDrawView->IsQuickTextEditMode() != pView->IsQuickEdit())
        mpDrawView->SetQuickTextEditMode( pView->IsQuickEdit() );

    // #i26631#
    if (mpDrawView->IsMasterPagePaintCaching() != pView->IsMasterPagePaintCaching())
        mpDrawView->SetMasterPagePaintCaching( pView->IsMasterPagePaintCaching() );

    // handle size: 9 pixels
    sal_uInt16 nTmp = mpDrawView->GetMarkHdlSizePixel();
    if( nTmp != 9 )
        mpDrawView->SetMarkHdlSizePixel( 9 );


    SdrPageView* pPageView = mpDrawView->GetSdrPageView();
    if (pPageView)
    {
        if ( pPageView->GetVisibleLayers() != pView->GetVisibleLayers() )
            pPageView->SetVisibleLayers( pView->GetVisibleLayers() );

        if ( pPageView->GetPrintableLayers() != pView->GetPrintableLayers() )
            pPageView->SetPrintableLayers( pView->GetPrintableLayers() );

        if ( pPageView->GetLockedLayers() != pView->GetLockedLayers() )
            pPageView->SetLockedLayers( pView->GetLockedLayers() );

        if (mePageKind == PK_NOTES)
        {
            if (pPageView->GetHelpLines() != pView->GetNotesHelpLines())
                pPageView->SetHelpLines( pView->GetNotesHelpLines() );
        }
        else if (mePageKind == PK_HANDOUT)
        {
            if (pPageView->GetHelpLines() != pView->GetHandoutHelpLines())
                pPageView->SetHelpLines( pView->GetHandoutHelpLines() );
        }
        else
        {
            if (pPageView->GetHelpLines() != pView->GetStandardHelpLines())
                pPageView->SetHelpLines( pView->GetStandardHelpLines() );
        }
    }

    if ( mpDrawView->GetActiveLayer() != pView->GetActiveLayer() )
        mpDrawView->SetActiveLayer( pView->GetActiveLayer() );

    sal_uInt16 nSelectedPage = 0;

    if (mePageKind != PK_HANDOUT)
    {
        nSelectedPage = pView->GetSelectedPage();
    }

    EditMode eNewEditMode = pView->GetViewShEditMode(mePageKind);
    sal_Bool bNewLayerMode = pView->IsLayerMode();
    ChangeEditMode(eNewEditMode, bNewLayerMode);
    SwitchPage(nSelectedPage);

    // DrawMode fuer 'Normales' Fenster wiederherstellen
    if(GetActiveWindow()->GetDrawMode() != pView->GetDrawMode())
      GetActiveWindow()->SetDrawMode(pView->GetDrawMode());

    if ( mpDrawView->IsDesignMode() != pView->IsDesignMode() )
    {
        SfxBoolItem aDesignModeItem( SID_FM_DESIGN_MODE, pView->IsDesignMode() );
        GetViewFrame()->GetDispatcher()->Execute( SID_FM_DESIGN_MODE, SFX_CALLMODE_SYNCHRON | SFX_CALLMODE_RECORD, &aDesignModeItem, 0L );
    }

    // Muss am Ende gerufen werden, da ein WriteFrameViewData() ausgeloest wird
    if (mpDrawView->IsFrameDragSingles() != pView->IsFrameDragSingles() )
        mpDrawView->SetFrameDragSingles( pView->IsFrameDragSingles() );
}

/*************************************************************************
|*
|* Daten der aktuellen View auf die FrameView uebertragen
|*
\************************************************************************/

void DrawViewShell::WriteFrameViewData()
{
    // Zeichen-(Bildschirm-)Attribute an FrameView merken
    mpFrameView->SetRuler( HasRuler() );
    mpFrameView->SetGridCoarse( mpDrawView->GetGridCoarse() );
    mpFrameView->SetGridFine( mpDrawView->GetGridFine() );
    mpFrameView->SetSnapGridWidth(mpDrawView->GetSnapGridWidthX(), mpDrawView->GetSnapGridWidthY());
    mpFrameView->SetGridVisible( mpDrawView->IsGridVisible() );
    mpFrameView->SetGridFront( mpDrawView->IsGridFront() );
    mpFrameView->SetSnapAngle( mpDrawView->GetSnapAngle() );
    mpFrameView->SetGridSnap( mpDrawView->IsGridSnap() );
    mpFrameView->SetBordSnap( mpDrawView->IsBordSnap() );
    mpFrameView->SetHlplSnap( mpDrawView->IsHlplSnap() );
    mpFrameView->SetOFrmSnap( mpDrawView->IsOFrmSnap() );
    mpFrameView->SetOPntSnap( mpDrawView->IsOPntSnap() );
    mpFrameView->SetOConSnap( mpDrawView->IsOConSnap() );
    mpFrameView->SetHlplVisible( mpDrawView->IsHlplVisible() );
    mpFrameView->SetDragStripes( mpDrawView->IsDragStripes() );
    mpFrameView->SetPlusHandlesAlwaysVisible( mpDrawView->IsPlusHandlesAlwaysVisible() );
    mpFrameView->SetFrameDragSingles( mpDrawView->IsFrameDragSingles() );
    mpFrameView->SetMarkedHitMovesAlways( mpDrawView->IsMarkedHitMovesAlways() );
    mpFrameView->SetMoveOnlyDragging( mpDrawView->IsMoveOnlyDragging() );
    mpFrameView->SetNoDragXorPolys( mpDrawView->IsNoDragXorPolys() );
    mpFrameView->SetCrookNoContortion( mpDrawView->IsCrookNoContortion() );
    mpFrameView->SetBigOrtho( mpDrawView->IsBigOrtho() );
    mpFrameView->SetEliminatePolyPointLimitAngle( mpDrawView->GetEliminatePolyPointLimitAngle() );
    mpFrameView->SetEliminatePolyPoints( mpDrawView->IsEliminatePolyPoints() );

    mpFrameView->SetSolidDragging( mpDrawView->IsSolidDragging() );
    mpFrameView->SetQuickEdit( mpDrawView->IsQuickTextEditMode() );

    mpFrameView->SetDesignMode( mpDrawView->IsDesignMode() );

    Size aVisSizePixel = GetActiveWindow()->GetOutputSizePixel();
    Rectangle aVisArea = GetActiveWindow()->PixelToLogic( Rectangle( Point(0,0), aVisSizePixel) );
    mpFrameView->SetVisArea(aVisArea);

    if( mePageKind == PK_HANDOUT )
        mpFrameView->SetSelectedPage(0);
    else
    {
        mpFrameView->SetSelectedPage( maTabControl.GetCurPageId() - 1 );
    }

    mpFrameView->SetViewShEditMode(meEditMode, mePageKind);
    mpFrameView->SetLayerMode(IsLayerModeActive());

    SdrPageView* pPageView = mpDrawView->GetSdrPageView();

    if (pPageView)
    {
        if ( mpFrameView->GetVisibleLayers() != pPageView->GetVisibleLayers() )
            mpFrameView->SetVisibleLayers( pPageView->GetVisibleLayers() );

        if ( mpFrameView->GetPrintableLayers() != pPageView->GetPrintableLayers() )
            mpFrameView->SetPrintableLayers( pPageView->GetPrintableLayers() );

        if ( mpFrameView->GetLockedLayers() != pPageView->GetLockedLayers() )
            mpFrameView->SetLockedLayers( pPageView->GetLockedLayers() );

        if (mePageKind == PK_NOTES)
        {
            mpFrameView->SetNotesHelpLines( pPageView->GetHelpLines() );
        }
        else if (mePageKind == PK_HANDOUT)
        {
            mpFrameView->SetHandoutHelpLines( pPageView->GetHelpLines() );
        }
        else
        {
            mpFrameView->SetStandardHelpLines( pPageView->GetHelpLines() );
        }
    }

    if ( mpFrameView->GetActiveLayer() != mpDrawView->GetActiveLayer() )
        mpFrameView->SetActiveLayer( mpDrawView->GetActiveLayer() );

    // DrawMode fuer 'Normales' Fenster merken
    if(mpFrameView->GetDrawMode() != GetActiveWindow()->GetDrawMode())
      mpFrameView->SetDrawMode(GetActiveWindow()->GetDrawMode());
}



/*************************************************************************
|*
|* PrePaint-Method
|*
\************************************************************************/

void DrawViewShell::PrePaint()
{
    mpDrawView->PrePaint();
}

/*************************************************************************
|*
|* Paint-Methode: das Ereignis wird vom Fenster pWin an
|* die Viewshell und die aktuelle Funktion weitergeleitet
|*
|* Anmerkung: pWin==NULL, wenn Paint() vom ShowWindow gerufen wird!
|*
\************************************************************************/

void DrawViewShell::Paint(const Rectangle& rRect, ::sd::Window* pWin)
{
    // Fill var FillColor here to have it available on later call
    svtools::ColorConfig aColorConfig;
    Color aFillColor;

    if(DOCUMENT_TYPE_IMPRESS == GetDoc()->GetDocumentType())
    {
        aFillColor = Color( aColorConfig.GetColorValue( svtools::APPBACKGROUND ).nColor );
    }
    else
    {
        aFillColor = Color( aColorConfig.GetColorValue( svtools::DOCCOLOR ).nColor );
    }

    /* This is done before each text edit, so why not do it before every paint.
                The default language is only used if the outliner only contains one
                character in a symbol font */
    GetDoc()->GetDrawOutliner( NULL ).SetDefaultLanguage( GetDoc()->GetLanguage( EE_CHAR_LANGUAGE ) );

    // Set Application Background color for usage in SdrPaintView(s)
    mpDrawView->SetApplicationBackgroundColor(aFillColor);

    /* This is done before each text edit, so why not do it before every paint.
                The default language is only used if the outliner only contains one
                character in a symbol font */
    GetDoc()->GetDrawOutliner( NULL ).SetDefaultLanguage( Application::GetSettings().GetLanguageTag().getLanguageType() );

    mpDrawView->CompleteRedraw( pWin, Region( rRect ) );

    if( pWin )
    {
        if( GetDocSh()->GetDocShellFunction().is() )
            GetDocSh()->GetDocShellFunction()->Paint( rRect, pWin );

        if( HasCurrentFunction() )
            GetCurrentFunction()->Paint( rRect, pWin );
    }
}

/*************************************************************************
|*
|* Zoom-Faktor fuer InPlace einstellen
|*
\************************************************************************/

void DrawViewShell::SetZoomFactor(const Fraction& rZoomX, const Fraction& rZoomY)
{
    ViewShell::SetZoomFactor(rZoomX, rZoomY);
    mbZoomOnPage = sal_False;
    Point aOrigin = GetActiveWindow()->GetViewOrigin();
    GetActiveWindow()->SetWinViewPos(aOrigin);
}

/*************************************************************************
|*
|* Optimale Groesse zurueckgeben
|*
\************************************************************************/

Size DrawViewShell::GetOptimalSizePixel() const
{
    Size aSize;

    SdrPageView* pPV = mpDrawView->GetSdrPageView();
    if (pPV)
    {
        SdPage* pPage = (SdPage*) pPV->GetPage();

        if (pPage)
        {
            if (!mbZoomOnPage)
            {
                // Gegenwaertigen MapMode beruecksichtigen
                aSize = GetActiveWindow()->LogicToPixel( pPage->GetSize() );
            }
            else
            {
                // 1:1 Darstellung
                MapMode aMapMode(MAP_100TH_MM);
                aSize = GetActiveWindow()->LogicToPixel( pPage->GetSize(), aMapMode );
                const_cast< DrawViewShell* >(this)->mbZoomOnPage = sal_True;
            }
        }
    }

    return(aSize);
}


/*************************************************************************
|*
|* Seite wird gehided
|*
\************************************************************************/

void DrawViewShell::HidePage()
{
    FmFormShell* pFormShell = GetViewShellBase().GetFormShellManager()->GetFormShell();
    if (pFormShell != NULL)
        pFormShell->PrepareClose (sal_False);
}



void DrawViewShell::WriteUserDataSequence ( ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >& rSequence, sal_Bool bBrowse )
{
    WriteFrameViewData();

    ViewShell::WriteUserDataSequence( rSequence, bBrowse );

    const sal_Int32 nIndex = rSequence.getLength();
    rSequence.realloc( nIndex + 1 );
    rSequence[nIndex].Name = sUNO_View_ZoomOnPage ;
    rSequence[nIndex].Value <<= (sal_Bool)mbZoomOnPage;
}

void DrawViewShell::ReadUserDataSequence ( const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >& rSequence, sal_Bool bBrowse )
{
    WriteFrameViewData();

    ViewShell::ReadUserDataSequence( rSequence, bBrowse );

    const sal_Int32 nLength = rSequence.getLength();
    const com::sun::star::beans::PropertyValue *pValue = rSequence.getConstArray();
    for (sal_Int16 i = 0 ; i < nLength; i++, pValue++ )
    {
        if ( pValue->Name == sUNO_View_ZoomOnPage )
        {
            sal_Bool bZoomPage = sal_False;
            if( pValue->Value >>= bZoomPage )
            {
                mbZoomOnPage = bZoomPage;
            }
        }
    }

    if( mpFrameView->GetPageKind() != mePageKind )
    {
        mePageKind = mpFrameView->GetPageKind();

        if (mePageKind == PK_NOTES)
        {
            SetHelpId( SID_NOTESMODE );
            GetActiveWindow()->SetHelpId( CMD_SID_NOTESMODE );
            GetActiveWindow()->SetUniqueId( CMD_SID_NOTESMODE );
        }
        else if (mePageKind == PK_HANDOUT)
        {
            SetHelpId( SID_HANDOUTMODE );
            GetActiveWindow()->SetHelpId( CMD_SID_HANDOUTMODE );
            GetActiveWindow()->SetUniqueId( CMD_SID_HANDOUTMODE );
        }
        else
        {
            SetHelpId( SD_IF_SDDRAWVIEWSHELL );
            GetActiveWindow()->SetHelpId( HID_SDDRAWVIEWSHELL );
            GetActiveWindow()->SetUniqueId( HID_SDDRAWVIEWSHELL );
        }
    }

    ReadFrameViewData( mpFrameView );

    if( !mbZoomOnPage )
    {
        const Rectangle aVisArea( mpFrameView->GetVisArea() );

        if ( GetDocSh()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
        {
            GetDocSh()->SetVisArea(aVisArea);
        }

        VisAreaChanged(aVisArea);

        ::sd::View* pView = GetView();

        if (pView)
        {
            pView->VisAreaChanged(GetActiveWindow());
        }

        SetZoomRect(aVisArea);
    }

    ChangeEditMode (meEditMode, ! IsLayerModeActive());
    ChangeEditMode (meEditMode, ! IsLayerModeActive());
    ResetActualLayer();
}

void DrawViewShell::VisAreaChanged(const Rectangle& rRect)
{
    ViewShell::VisAreaChanged( rRect );

    DrawController& rController = GetViewShellBase().GetDrawController();
    rController.FireVisAreaChanged (rRect);
}




/** If there is a valid controller then create a new instance of
    <type>AccessibleDrawDocumentView</type>.  Otherwise return an empty
    reference.
*/
::com::sun::star::uno::Reference<
    ::com::sun::star::accessibility::XAccessible>
    DrawViewShell::CreateAccessibleDocumentView (::sd::Window* pWindow)
{
    if (GetViewShellBase().GetController() != NULL)
    {
        accessibility::AccessibleDrawDocumentView* pDocumentView =
            new accessibility::AccessibleDrawDocumentView (
                pWindow,
                this,
                GetViewShellBase().GetController(),
                pWindow->GetAccessibleParentWindow()->GetAccessible());
        pDocumentView->Init();
        return ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible>
            (static_cast< ::com::sun::star::uno::XWeak*>(pDocumentView),
                ::com::sun::star::uno::UNO_QUERY);
    }

    OSL_TRACE ("DrawViewShell::CreateAccessibleDocumentView: no controller");
    return ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible>();
}




int DrawViewShell::GetActiveTabLayerIndex (void) const
{
    const LayerTabBar* pBar
        = const_cast<DrawViewShell*>(this)->GetLayerTabControl ();
    if (pBar != NULL)
        return pBar->GetPagePos (pBar->GetCurPageId());
    else
        return -1;
}




void DrawViewShell::SetActiveTabLayerIndex (int nIndex)
{
    LayerTabBar* pBar = GetLayerTabControl ();
    if (pBar != NULL)
    {
        // Ignore invalid indices silently.
        if (nIndex>=0 && nIndex<pBar->GetPageCount())
        {
            // Tell the draw view and the tab control of the new active layer.
            mpDrawView->SetActiveLayer (pBar->GetPageText (pBar->GetPageId ((sal_uInt16)nIndex)));
            pBar->SetCurPageId (pBar->GetPageId ((sal_uInt16)nIndex));
        }
    }
}




TabControl* DrawViewShell::GetPageTabControl (void)
{
    return &maTabControl;
}




LayerTabBar* DrawViewShell::GetLayerTabControl (void)
{
    return mpLayerTabBar.get();
}




int DrawViewShell::GetTabLayerCount (void) const
{
    const LayerTabBar* pBar
        = const_cast<DrawViewShell*>(this)->GetLayerTabControl ();
    if (pBar != NULL)
        return pBar->GetPageCount();
    else
        return 0;
}


} // end of namespace sd

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
