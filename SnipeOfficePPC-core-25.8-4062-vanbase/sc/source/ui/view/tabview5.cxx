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

#include "scitems.hxx"
#include <editeng/eeitem.hxx>

#include <svx/fmshell.hxx>
#include <svx/svdobj.hxx>
#include <svx/svdoutl.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/objsh.hxx>

#include "tabview.hxx"
#include "tabvwsh.hxx"
#include "document.hxx"
#include "gridwin.hxx"
#include "olinewin.hxx"
#include "tabsplit.hxx"
#include "colrowba.hxx"
#include "tabcont.hxx"
#include "hintwin.hxx"
#include "sc.hrc"
#include "pagedata.hxx"
#include "hiranges.hxx"
#include "drawview.hxx"
#include "drwlayer.hxx"
#include "fusel.hxx"                // Start-Function
#include "seltrans.hxx"
#include "scmod.hxx"
#include "AccessibilityHints.hxx"
#include "docsh.hxx"
#include "viewuno.hxx"

#include <vcl/svapp.hxx>

using namespace com::sun::star;

// STATIC DATA -----------------------------------------------------------


void ScTabView::Init()
{
    /*  RTL layout of the view windows is done manually, because it depends on
        the sheet orientation, not the UI setting. Note: controls that are
        already constructed (e.g. scroll bars) have the RTL setting of the GUI.
        Eventually this has to be disabled manually (see below). */
    pFrameWin->EnableRTL( false );

    sal_uInt16 i;

    aScrollTimer.SetTimeout(10);
    aScrollTimer.SetTimeoutHdl( LINK( this, ScTabView, TimerHdl ) );

    for (i=0; i<4; i++)
        pGridWin[i] = NULL;
    pGridWin[SC_SPLIT_BOTTOMLEFT] = new ScGridWindow( pFrameWin, &aViewData, SC_SPLIT_BOTTOMLEFT );

    pSelEngine = new ScViewSelectionEngine( pGridWin[SC_SPLIT_BOTTOMLEFT], this,
                                                SC_SPLIT_BOTTOMLEFT );
    aFunctionSet.SetSelectionEngine( pSelEngine );

    pHdrSelEng = new ScHeaderSelectionEngine( pFrameWin, &aHdrFunc );

    pColBar[SC_SPLIT_LEFT] = new ScColBar( pFrameWin, &aViewData, SC_SPLIT_LEFT,
                                                &aHdrFunc, pHdrSelEng );
    pColBar[SC_SPLIT_RIGHT] = NULL;
    pRowBar[SC_SPLIT_BOTTOM] = new ScRowBar( pFrameWin, &aViewData, SC_SPLIT_BOTTOM,
                                                &aHdrFunc, pHdrSelEng );
    pRowBar[SC_SPLIT_TOP] = NULL;
    for (i=0; i<2; i++)
        pColOutline[i] = pRowOutline[i] = NULL;

    pHSplitter = new ScTabSplitter( pFrameWin, WinBits( WB_HSCROLL ), &aViewData );
    pVSplitter = new ScTabSplitter( pFrameWin, WinBits( WB_VSCROLL ), &aViewData );

    // SSA: override default keyboard step size to allow snap to row/column
    pHSplitter->SetKeyboardStepSize( 1 );
    pVSplitter->SetKeyboardStepSize( 1 );

    pTabControl = new ScTabControl( pFrameWin, &aViewData );
    /*  #i97900# The tab control has to remain in RTL mode if GUI is RTL, this
        is needed to draw the 3D effect correctly. The base TabBar implementes
        mirroring independent from the GUI direction. Have to set RTL mode
        explicitly because the parent frame window is already RTL disabled. */
    pTabControl->EnableRTL( Application::GetSettings().GetLayoutRTL() );

    InitScrollBar( aHScrollLeft,    MAXCOL+1 );
    InitScrollBar( aHScrollRight,   MAXCOL+1 );
    InitScrollBar( aVScrollTop,     MAXROW+1 );
    InitScrollBar( aVScrollBottom,  MAXROW+1 );
    /*  #i97900# scrollbars remain in correct RTL mode, needed mirroring etc.
        is now handled correctly at the respective places. */

    //  Hier noch nichts anzeigen (Show), weil noch falsch angeordnet ist
    //  Show kommt dann aus UpdateShow beim ersten Resize
    //      pTabControl, pGridWin, aHScrollLeft, aVScrollBottom,
    //      aCornerButton, aScrollBarBox, pHSplitter, pVSplitter

    //      Splitter

    pHSplitter->SetSplitHdl( LINK( this, ScTabView, SplitHdl ) );
    pVSplitter->SetSplitHdl( LINK( this, ScTabView, SplitHdl ) );

    //  UpdateShow kommt beim Resize, oder bei Kopie einer bestehenden View aus dem ctor

    pDrawActual = NULL;
    pDrawOld    = NULL;

            //  DrawView darf nicht im TabView - ctor angelegt werden,
            //  wenn die ViewShell noch nicht konstruiert ist...
            //  Das gilt auch fuer ViewOptionsHasChanged()

    TestHintWindow();
}

ScTabView::~ScTabView()
{
    sal_uInt16 i;

    //  remove selection object
    ScModule* pScMod = SC_MOD();
    ScSelectionTransferObj* pOld = pScMod->GetSelectionTransfer();
    if ( pOld && pOld->GetView() == this )
    {
        pOld->ForgetView();
        pScMod->SetSelectionTransfer( NULL );
        TransferableHelper::ClearSelection( GetActiveWin() );       // may delete pOld
    }

    DELETEZ(pBrushDocument);
    DELETEZ(pDrawBrushSet);

    DELETEZ(pPageBreakData);

    DELETEZ(pDrawOld);
    DELETEZ(pDrawActual);

    aViewData.KillEditView();           // solange GridWin's noch existieren

    if (pDrawView)
    {
        for (i=0; i<4; i++)
            if (pGridWin[i])
            {
                pDrawView->VCRemoveWin(pGridWin[i]);
                pDrawView->DeleteWindowFromPaintView(pGridWin[i]);
            }

        pDrawView->HideSdrPage();
        delete pDrawView;
    }

    delete pSelEngine;

    // Delete this before the grid windows, since its a child window of one of them.
    mpInputHintWindow.reset();
    for (i=0; i<4; i++)
        delete pGridWin[i];

    delete pHdrSelEng;

    for (i=0; i<2; i++)
    {
        delete pColBar[i];
        delete pRowBar[i];
        delete pColOutline[i];
        delete pRowOutline[i];
    }

    delete pHSplitter;
    delete pVSplitter;

    delete pTabControl;
}

void ScTabView::MakeDrawView( sal_uInt8 nForceDesignMode )
{
    if (!pDrawView)
    {
        ScDrawLayer* pLayer = aViewData.GetDocument()->GetDrawLayer();
        OSL_ENSURE(pLayer, "wo ist der Draw Layer ??");

        sal_uInt16 i;
        pDrawView = new ScDrawView( pGridWin[SC_SPLIT_BOTTOMLEFT], &aViewData );
        for (i=0; i<4; i++)
            if (pGridWin[i])
            {
                if ( SC_SPLIT_BOTTOMLEFT != (ScSplitPos)i )
                    pDrawView->AddWindowToPaintView(pGridWin[i]);
                pDrawView->VCAddWin(pGridWin[i]);
            }
        pDrawView->RecalcScale();
        for (i=0; i<4; i++)
            if (pGridWin[i])
            {
                pGridWin[i]->SetMapMode(pGridWin[i]->GetDrawMapMode());

                pGridWin[i]->Update();      // wegen Invalidate im DrawView ctor (ShowPage),
                                            // damit gleich gezeichnet werden kann
            }
        SfxRequest aSfxRequest(SID_OBJECT_SELECT, 0,aViewData.GetViewShell()->GetPool());
        SetDrawFuncPtr(new FuSelection( aViewData.GetViewShell(), GetActiveWin(), pDrawView,
                                        pLayer,aSfxRequest));

        //  used when switching back from page preview: restore saved design mode state
        //  (otherwise, keep the default from the draw view ctor)
        if ( nForceDesignMode != SC_FORCEMODE_NONE )
            pDrawView->SetDesignMode( (sal_Bool)nForceDesignMode );

        //  an der FormShell anmelden
        FmFormShell* pFormSh = aViewData.GetViewShell()->GetFormShell();
        if (pFormSh)
            pFormSh->SetView(pDrawView);

        if (aViewData.GetViewShell()->HasAccessibilityObjects())
            aViewData.GetViewShell()->BroadcastAccessibility(SfxSimpleHint(SC_HINT_ACC_MAKEDRAWLAYER));

    }
}

void ScTabView::DoAddWin( ScGridWindow* pWin )
{
    if (pDrawView)
    {
        pDrawView->AddWindowToPaintView(pWin);
        pDrawView->VCAddWin(pWin);

        // #114409#
        pWin->DrawLayerCreated();
    }
}

//==================================================================

void ScTabView::TabChanged( bool bSameTabButMoved )
{
    if (pDrawView)
    {
        DrawDeselectAll();      // beendet auch Text-Edit-Modus

        sal_uInt16 i;
        for (i=0; i<4; i++)
            if (pGridWin[i])
                pDrawView->VCRemoveWin(pGridWin[i]);    // fuer alte Page

        SCTAB nTab = aViewData.GetTabNo();
        pDrawView->HideSdrPage();
        pDrawView->ShowSdrPage(pDrawView->GetModel()->GetPage(nTab));

        UpdateLayerLocks();

        pDrawView->RecalcScale();
        pDrawView->UpdateWorkArea();    // PageSize ist pro Page unterschiedlich

        for (i=0; i<4; i++)
            if (pGridWin[i])
                pDrawView->VCAddWin(pGridWin[i]);       // fuer neue Page
    }

    SfxBindings& rBindings = aViewData.GetBindings();

    //  Es gibt keine einfache Moeglichkeit, alle Slots der FormShell zu invalidieren
    //  (fuer disablete Slots auf geschuetzten Tabellen), darum hier einfach alles...
    rBindings.InvalidateAll(false);

    if (aViewData.GetViewShell()->HasAccessibilityObjects())
    {
        SfxSimpleHint aAccHint(SC_HINT_ACC_TABLECHANGED);
        aViewData.GetViewShell()->BroadcastAccessibility(aAccHint);
    }

    // notification for XActivationBroadcaster
    SfxViewFrame* pViewFrame = aViewData.GetViewShell()->GetViewFrame();
    if (pViewFrame)
    {
        uno::Reference<frame::XController> xController = pViewFrame->GetFrame().GetController();
        if (xController.is())
        {
            ScTabViewObj* pImp = ScTabViewObj::getImplementation( xController );
            if (pImp)
                pImp->SheetChanged( bSameTabButMoved );
        }
    }
}

void ScTabView::UpdateLayerLocks()
{
    if (pDrawView)
    {
        SCTAB nTab = aViewData.GetTabNo();
        sal_Bool bEx = aViewData.GetViewShell()->IsDrawSelMode();
        sal_Bool bProt = aViewData.GetDocument()->IsTabProtected( nTab ) ||
                     aViewData.GetSfxDocShell()->IsReadOnly();
        sal_Bool bShared = aViewData.GetDocShell()->IsDocShared();

        SdrLayer* pLayer;
        SdrLayerAdmin& rAdmin = pDrawView->GetModel()->GetLayerAdmin();
        pLayer = rAdmin.GetLayerPerID(SC_LAYER_BACK);
        if (pLayer)
            pDrawView->SetLayerLocked( pLayer->GetName(), bProt || !bEx || bShared );
        pLayer = rAdmin.GetLayerPerID(SC_LAYER_INTERN);
        if (pLayer)
            pDrawView->SetLayerLocked( pLayer->GetName(), sal_True );
        pLayer = rAdmin.GetLayerPerID(SC_LAYER_FRONT);
        if (pLayer)
            pDrawView->SetLayerLocked( pLayer->GetName(), bProt || bShared );
        pLayer = rAdmin.GetLayerPerID(SC_LAYER_CONTROLS);
        if (pLayer)
            pDrawView->SetLayerLocked( pLayer->GetName(), bProt || bShared );
        pLayer = rAdmin.GetLayerPerID(SC_LAYER_HIDDEN);
        if (pLayer)
        {
            pDrawView->SetLayerLocked( pLayer->GetName(), bProt || bShared );
            pDrawView->SetLayerVisible( pLayer->GetName(), false);
        }
    }
}

void ScTabView::DrawDeselectAll()
{
    if (pDrawView)
    {
        ScTabViewShell* pViewSh = aViewData.GetViewShell();
        if ( pDrawActual &&
            ( pViewSh->IsDrawTextShell() || pDrawActual->GetSlotID() == SID_DRAW_NOTEEDIT ) )
        {
            // end text edit (as if escape pressed, in FuDraw)
            aViewData.GetDispatcher().Execute( pDrawActual->GetSlotID(),
                                        SFX_CALLMODE_SLOT | SFX_CALLMODE_RECORD );
        }

        pDrawView->ScEndTextEdit();
        pDrawView->UnmarkAll();

        if (!pViewSh->IsDrawSelMode())
            pViewSh->SetDrawShell( false );
    }
}

bool ScTabView::IsDrawTextEdit() const
{
    if (pDrawView)
        return pDrawView->IsTextEdit();
    else
        return false;
}

SvxZoomType ScTabView::GetZoomType() const
{
    return aViewData.GetZoomType();
}

void ScTabView::SetZoomType( SvxZoomType eNew, bool bAll )
{
    aViewData.SetZoomType( eNew, bAll );
}

void ScTabView::SetZoom( const Fraction& rNewX, const Fraction& rNewY, bool bAll )
{
    aViewData.SetZoom( rNewX, rNewY, bAll );
    if (pDrawView)
        pDrawView->RecalcScale();
    ZoomChanged();              // einzeln wegen CLOOKs
}

void ScTabView::RefreshZoom()
{
    aViewData.RefreshZoom();
    if (pDrawView)
        pDrawView->RecalcScale();
    ZoomChanged();
}

void ScTabView::SetPagebreakMode( bool bSet )
{
    aViewData.SetPagebreakMode(bSet);
    if (pDrawView)
        pDrawView->RecalcScale();
    ZoomChanged();              // einzeln wegen CLOOKs
}

void ScTabView::ResetDrawDragMode()
{
    if (pDrawView)
        pDrawView->SetDragMode( SDRDRAG_MOVE );
}

void ScTabView::ViewOptionsHasChanged( bool bHScrollChanged, bool bGraphicsChanged )
{
    //  DrawView erzeugen, wenn Gitter angezeigt werden soll
    if ( !pDrawView && aViewData.GetOptions().GetGridOptions().GetGridVisible() )
        MakeDrawLayer();

    if (pDrawView)
        pDrawView->UpdateUserViewOptions();

    if (bGraphicsChanged)
        DrawEnableAnim(sal_True);   // DrawEnableAnim checks the options state

    // if TabBar is set to visible, make sure its size is not 0
    bool bGrow = ( aViewData.IsTabMode() && pTabControl->GetSizePixel().Width() <= 0 );

    // if ScrollBar is set to visible, TabBar must make room
    bool bShrink = ( bHScrollChanged && aViewData.IsTabMode() && aViewData.IsHScrollMode() &&
                     pTabControl->GetSizePixel().Width() > SC_TABBAR_DEFWIDTH );

    if ( bGrow || bShrink )
    {
        Size aSize = pTabControl->GetSizePixel();
        aSize.Width() = SC_TABBAR_DEFWIDTH;             // initial size
        pTabControl->SetSizePixel(aSize);               // DoResize is called later...
    }
}

// Helper-Funktion gegen das Include des Drawing Layers

SdrView* ScTabView::GetSdrView()
{
    return pDrawView;
}

void ScTabView::DrawMarkListHasChanged()
{
    if ( pDrawView )
        pDrawView->MarkListHasChanged();
}

void ScTabView::UpdateAnchorHandles()
{
    if ( pDrawView )
        pDrawView->AdjustMarkHdl();
}

void ScTabView::UpdateIMap( SdrObject* pObj )
{
    if ( pDrawView )
        pDrawView->UpdateIMap( pObj );
}

void ScTabView::DrawMarkRect( const Rectangle& rRect )
{
    //! store rectangle for repaint during drag

    for (sal_uInt16 i=0; i<4; i++)
    {
        if ( pGridWin[i] && pGridWin[i]->IsVisible() )
        {
            RasterOp aROp = pGridWin[i]->GetRasterOp();
            sal_Bool bHasLine = pGridWin[i]->IsLineColor();
            Color aLine   = pGridWin[i]->GetLineColor();
            sal_Bool bHasFill = pGridWin[i]->IsFillColor();
            Color aFill   = pGridWin[i]->GetFillColor();

            pGridWin[i]->SetRasterOp( ROP_INVERT );
            pGridWin[i]->SetLineColor( COL_BLACK );
            pGridWin[i]->SetFillColor();

            pGridWin[i]->DrawRect(rRect);

            pGridWin[i]->SetRasterOp(aROp);
            if (bHasLine)
                pGridWin[i]->SetLineColor(aLine);
            else
                pGridWin[i]->SetLineColor();
            if (bHasFill)
                pGridWin[i]->SetFillColor(aFill);
            else
                pGridWin[i]->SetFillColor();
        }
    }
}

void ScTabView::DrawEnableAnim(bool bSet)
{
    sal_uInt16 i;
    if ( pDrawView )
    {
        //  dont start animations if display of graphics is disabled
        //  graphics are controlled by VOBJ_TYPE_OLE
        if ( bSet && aViewData.GetOptions().GetObjMode(VOBJ_TYPE_OLE) == VOBJ_MODE_SHOW )
        {
            if ( !pDrawView->IsAnimationEnabled() )
            {
                pDrawView->SetAnimationEnabled(true);

                //  Animierte GIFs muessen wieder gestartet werden:
                ScDocument* pDoc = aViewData.GetDocument();
                for (i=0; i<4; i++)
                    if ( pGridWin[i] && pGridWin[i]->IsVisible() )
                        pDoc->StartAnimations( aViewData.GetTabNo(), pGridWin[i] );
            }
        }
        else
        {
            pDrawView->SetAnimationEnabled(false);
        }
    }
}

void ScTabView::UpdateDrawTextOutliner()
{
    if ( pDrawView )
    {
        Outliner* pOL = pDrawView->GetTextEditOutliner();
        if (pOL)
            aViewData.UpdateOutlinerFlags( *pOL );
    }
}

void ScTabView::DigitLanguageChanged()
{
    LanguageType eNewLang = SC_MOD()->GetOptDigitLanguage();
    for (sal_uInt16 i=0; i<4; i++)
        if ( pGridWin[i] )
            pGridWin[i]->SetDigitLanguage( eNewLang );
}

//---------------------------------------------------------------

void ScTabView::ScrollToObject( SdrObject* pDrawObj )
{
    if ( pDrawObj )
    {
        // #i118524# use the BoundRect, this defines the visible area
        MakeVisible(pDrawObj->GetCurrentBoundRect());
    }
}

void ScTabView::MakeVisible( const Rectangle& rHMMRect )
{
    Window* pWin = GetActiveWin();
    Size aWinSize = pWin->GetOutputSizePixel();
    SCTAB nTab = aViewData.GetTabNo();

    Rectangle aRect = pWin->LogicToPixel( rHMMRect );

    long nScrollX=0, nScrollY=0;        // Pixel

    if ( aRect.Right() >= aWinSize.Width() )                // rechts raus
    {
        nScrollX = aRect.Right() - aWinSize.Width() + 1;    // rechter Rand sichtbar
        if ( aRect.Left() < nScrollX )
            nScrollX = aRect.Left();                        // links sichtbar (falls zu gross)
    }
    if ( aRect.Bottom() >= aWinSize.Height() )              // unten raus
    {
        nScrollY = aRect.Bottom() - aWinSize.Height() + 1;  // unterer Rand sichtbar
        if ( aRect.Top() < nScrollY )
            nScrollY = aRect.Top();                         // oben sichtbar (falls zu gross)
    }

    if ( aRect.Left() < 0 )             // links raus
        nScrollX = aRect.Left();        // linker Rand sichtbar
    if ( aRect.Top() < 0 )              // oben raus
        nScrollY = aRect.Top();         // oberer Rand sichtbar

    if (nScrollX || nScrollY)
    {
        ScDocument* pDoc = aViewData.GetDocument();
        if ( pDoc->IsNegativePage( nTab ) )
            nScrollX = -nScrollX;

        double nPPTX = aViewData.GetPPTX();
        double nPPTY = aViewData.GetPPTY();
        ScSplitPos eWhich = aViewData.GetActivePart();
        SCCOL nPosX = aViewData.GetPosX(WhichH(eWhich));
        SCROW nPosY = aViewData.GetPosY(WhichV(eWhich));

        long nLinesX=0, nLinesY=0;      // Spalten/Zeilen - um mindestens nScrollX/Y scrollen

        if (nScrollX > 0)
            while (nScrollX > 0 && nPosX < MAXCOL)
            {
                nScrollX -= (long) ( pDoc->GetColWidth(nPosX, nTab) * nPPTX );
                ++nPosX;
                ++nLinesX;
            }
        else if (nScrollX < 0)
            while (nScrollX < 0 && nPosX > 0)
            {
                --nPosX;
                nScrollX += (long) ( pDoc->GetColWidth(nPosX, nTab) * nPPTX );
                --nLinesX;
            }

        if (nScrollY > 0)
            while (nScrollY > 0 && nPosY < MAXROW)
            {
                nScrollY -= (long) ( pDoc->GetRowHeight(nPosY, nTab) * nPPTY );
                ++nPosY;
                ++nLinesY;
            }
        else if (nScrollY < 0)
            while (nScrollY < 0 && nPosY > 0)
            {
                --nPosY;
                nScrollY += (long) ( pDoc->GetRowHeight(nPosY, nTab) * nPPTY );
                --nLinesY;
            }

        ScrollLines( nLinesX, nLinesY );                    // ausfuehren
    }
}

//---------------------------------------------------------------

void ScTabView::SetBrushDocument( ScDocument* pNew, bool bLock )
{
    delete pBrushDocument;
    delete pDrawBrushSet;

    pBrushDocument = pNew;
    pDrawBrushSet = NULL;

    bLockPaintBrush = bLock;

    aViewData.GetBindings().Invalidate(SID_FORMATPAINTBRUSH);
}

void ScTabView::SetDrawBrushSet( SfxItemSet* pNew, bool bLock )
{
    delete pBrushDocument;
    delete pDrawBrushSet;

    pBrushDocument = NULL;
    pDrawBrushSet = pNew;

    bLockPaintBrush = bLock;

    aViewData.GetBindings().Invalidate(SID_FORMATPAINTBRUSH);
}

void ScTabView::ResetBrushDocument()
{
    if ( HasPaintBrush() )
    {
        SetBrushDocument( NULL, false );
        SetActivePointer( Pointer( POINTER_ARROW ) );   // switch pointers also when ended with escape key
    }
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
