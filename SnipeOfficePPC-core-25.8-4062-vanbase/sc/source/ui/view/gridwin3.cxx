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

#include <svx/svdoutl.hxx>
#include <svx/svdotext.hxx>
#include <svx/svdpagv.hxx>
#include <editeng/sizeitem.hxx>
#include <sfx2/bindings.hxx>
#include <svl/ptitem.hxx>

#include "tabvwsh.hxx"
#include "gridwin.hxx"
#include "dbfunc.hxx"
#include "viewdata.hxx"
#include "output.hxx"
#include "drawview.hxx"
#include "fupoor.hxx"

#include "drawutil.hxx"
#include "document.hxx"
#include "drwlayer.hxx"
#include <vcl/svapp.hxx>
#include "userdat.hxx"
#include "unitconv.hxx"
#include <svx/svdpage.hxx>
// -----------------------------------------------------------------------

bool ScGridWindow::DrawMouseButtonDown(const MouseEvent& rMEvt)
{
    bool bRet = false;
    FuPoor* pDraw = pViewData->GetView()->GetDrawFuncPtr();
    if (pDraw && !pViewData->IsRefMode())
    {
        pDraw->SetWindow( this );
        Point aLogicPos = PixelToLogic(rMEvt.GetPosPixel());
        if ( pDraw->IsDetectiveHit( aLogicPos ) )
        {
            //  auf Detektiv-Pfeilen gar nichts (Doppelklick wird bei ButtonUp ausgewertet)
            bRet = true;
        }
        else
        {
            bRet = pDraw->MouseButtonDown( rMEvt );
            if ( bRet )
                UpdateStatusPosSize();
        }
    }

    //  bei rechter Taste Draw-Aktion abbrechen

    ScDrawView* pDrView = pViewData->GetScDrawView();
    if ( pDrView && !rMEvt.IsLeft() && !bRet )
    {
        pDrView->BrkAction();
        bRet = true;
    }
    return bRet;
}

bool ScGridWindow::DrawMouseButtonUp(const MouseEvent& rMEvt)
{
    ScViewFunc* pView = pViewData->GetView();
    bool bRet = false;
    FuPoor* pDraw = pView->GetDrawFuncPtr();
    if (pDraw && !pViewData->IsRefMode())
    {
        pDraw->SetWindow( this );
        bRet = pDraw->MouseButtonUp( rMEvt );

        // execute "format paint brush" for drawing objects
        SfxItemSet* pDrawBrush = pView->GetDrawBrushSet();
        if ( pDrawBrush )
        {
            ScDrawView* pDrView = pViewData->GetScDrawView();
            if ( pDrView )
            {
                bool bReplaceAll = true;
                pDrView->SetAttrToMarked(*pDrawBrush, bReplaceAll);
            }

            if ( !pView->IsPaintBrushLocked() )
                pView->ResetBrushDocument();        // end paint brush mode if not locked
        }
    }

    return bRet;
}

bool ScGridWindow::DrawMouseMove(const MouseEvent& rMEvt)
{
    FuPoor* pDraw = pViewData->GetView()->GetDrawFuncPtr();
    if (pDraw && !pViewData->IsRefMode())
    {
        pDraw->SetWindow( this );
        bool bRet = pDraw->MouseMove( rMEvt );
        if ( bRet )
            UpdateStatusPosSize();
        return bRet;
    }
    else
    {
        SetPointer( Pointer( POINTER_ARROW ) );
        return false;
    }
}

void ScGridWindow::DrawEndAction()
{
    ScDrawView* pDrView = pViewData->GetScDrawView();
    if ( pDrView && pDrView->IsAction() )
        pDrView->BrkAction();

    FuPoor* pDraw = pViewData->GetView()->GetDrawFuncPtr();
    if (pDraw)
        pDraw->StopDragTimer();

    //  ReleaseMouse beim Aufruf
}

bool ScGridWindow::DrawCommand(const CommandEvent& rCEvt)
{
    ScDrawView* pDrView = pViewData->GetScDrawView();
    FuPoor* pDraw = pViewData->GetView()->GetDrawFuncPtr();
    if (pDrView && pDraw && !pViewData->IsRefMode())
    {
        pDraw->SetWindow( this );
        sal_uInt8 nUsed = pDraw->Command( rCEvt );
        if( nUsed == SC_CMD_USED )
            nButtonDown = 0;                    // MouseButtonUp wird verschluckt...
        if( nUsed || pDrView->IsAction() )
            return true;
    }

    return false;
}

bool ScGridWindow::DrawKeyInput(const KeyEvent& rKEvt)
{
    ScDrawView* pDrView = pViewData->GetScDrawView();
    FuPoor* pDraw = pViewData->GetView()->GetDrawFuncPtr();
    if (pDrView && pDraw && !pViewData->IsRefMode())
    {
        pDraw->SetWindow( this );
        bool bOldMarked = pDrView->AreObjectsMarked();
        if (pDraw->KeyInput( rKEvt ))
        {
            bool bLeaveDraw = false;
            bool bUsed = true;
            bool bNewMarked = pDrView->AreObjectsMarked();
            if ( !pViewData->GetView()->IsDrawSelMode() )
                if ( !bNewMarked )
                {
                    pViewData->GetViewShell()->SetDrawShell( false );
                    bLeaveDraw = true;
                    if ( !bOldMarked &&
                        rKEvt.GetKeyCode().GetCode() == KEY_DELETE )
                        bUsed = false;                  // nichts geloescht
                }
            if (!bLeaveDraw)
                UpdateStatusPosSize();      // for moving/resizing etc. by keyboard
            return bUsed;
        }
    }

    return false;
}

void ScGridWindow::DrawRedraw( ScOutputData& rOutputData, ScUpdateMode eMode, sal_uLong nLayer )
{
    // #109985#
    const ScViewOptions& rOpts = pViewData->GetOptions();

    // use new flags at SdrPaintView for hiding objects
    const bool bDrawOle(VOBJ_MODE_SHOW == rOpts.GetObjMode(VOBJ_TYPE_OLE));
    const bool bDrawChart(VOBJ_MODE_SHOW == rOpts.GetObjMode(VOBJ_TYPE_CHART));
    const bool bDrawDraw(VOBJ_MODE_SHOW == rOpts.GetObjMode(VOBJ_TYPE_DRAW));

    if(bDrawOle || bDrawChart || bDrawDraw)
    {
        ScDrawView* pDrView = pViewData->GetView()->GetScDrawView();

        if(pDrView)
        {
            pDrView->setHideOle(!bDrawOle);
            pDrView->setHideChart(!bDrawChart);
            pDrView->setHideDraw(!bDrawDraw);
            pDrView->setHideFormControl(!bDrawDraw);
        }

        if(SC_UPDATE_CHANGED == eMode)
        {
            rOutputData.DrawingSingle((sal_uInt16)nLayer);
        }
        else
        {
            rOutputData.DrawSelectiveObjects((sal_uInt16)nLayer);
        }
    }
}

void ScGridWindow::DrawSdrGrid( const Rectangle& rDrawingRect, OutputDevice* pContentDev )
{
    //  Draw-Gitterlinien

    ScDrawView* pDrView = pViewData->GetView()->GetScDrawView();
    if ( pDrView && pDrView->IsGridVisible() )
    {
        SdrPageView* pPV = pDrView->GetSdrPageView();
        OSL_ENSURE(pPV, "keine PageView");
        if (pPV)
        {
            pContentDev->SetLineColor(COL_GRAY);

            pPV->DrawPageViewGrid( *pContentDev, rDrawingRect );
        }
    }
}

MapMode ScGridWindow::GetDrawMapMode( bool bForce )
{
    ScDocument* pDoc = pViewData->GetDocument();
    SCTAB nTab = pViewData->GetTabNo();
    sal_Bool bNegativePage = pDoc->IsNegativePage( nTab );

    MapMode aDrawMode = pViewData->GetLogicMode();

    ScDrawView* pDrView = pViewData->GetView()->GetScDrawView();
    if ( pDrView || bForce )
    {
        Fraction aScaleX;
        Fraction aScaleY;
        if (pDrView)
            pDrView->GetScale( aScaleX, aScaleY );
        else
        {
            SCCOL nEndCol = 0;
            SCROW nEndRow = 0;
            pDoc->GetTableArea( nTab, nEndCol, nEndRow );
            if (nEndCol<20) nEndCol = 20;
            if (nEndRow<20) nEndRow = 1000;
            ScDrawUtil::CalcScale( pDoc, nTab, 0,0, nEndCol,nEndRow, this,
                                    pViewData->GetZoomX(),pViewData->GetZoomY(),
                                    pViewData->GetPPTX(),pViewData->GetPPTY(),
                                    aScaleX,aScaleY );
        }
        aDrawMode.SetScaleX(aScaleX);
        aDrawMode.SetScaleY(aScaleY);
    }
    aDrawMode.SetOrigin(Point());
    Point aStartPos = pViewData->GetPixPos(eWhich);
    if ( bNegativePage )
    {
        //  RTL uses negative positions for drawing objects
        aStartPos.X() = -aStartPos.X() + GetOutputSizePixel().Width() - 1;
    }
    aDrawMode.SetOrigin( PixelToLogic( aStartPos, aDrawMode ) );

    return aDrawMode;
}

void ScGridWindow::DrawAfterScroll()
{
    Update();       // immer, damit das Verhalten mit/ohne DrawingLayer gleich ist

    ScDrawView* pDrView = pViewData->GetView()->GetScDrawView();
    if (pDrView)
    {
        OutlinerView* pOlView = pDrView->GetTextEditOutlinerView();
        if (pOlView && pOlView->GetWindow() == this)
            pOlView->ShowCursor(false);                 // ist beim Scrollen weggekommen
    }
}

void ScGridWindow::CreateAnchorHandle(SdrHdlList& rHdl, const ScAddress& rAddress)
{
    ScDrawView* pDrView = pViewData->GetView()->GetScDrawView();
    if (pDrView)
    {
        const ScViewOptions& rOpts = pViewData->GetOptions();
        if(rOpts.GetOption( VOPT_ANCHOR ))
        {
            sal_Bool bNegativePage = pViewData->GetDocument()->IsNegativePage( pViewData->GetTabNo() );
            Point aPos = pViewData->GetScrPos( rAddress.Col(), rAddress.Row(), eWhich, sal_True );
            aPos = PixelToLogic(aPos);
            rHdl.AddHdl(new SdrHdl(aPos, bNegativePage ? HDL_ANCHOR_TR : HDL_ANCHOR));
        }
    }
}

SdrObject* ScGridWindow::GetEditObject()
{
    ScDrawView* pDrView = pViewData->GetView()->GetScDrawView();
    if (pDrView)
    {
        OutlinerView* pOlView = pDrView->GetTextEditOutlinerView();
        if (pOlView && pOlView->GetWindow() == this)
            return pDrView->GetTextEditObject();
    }

    return NULL;
}

void ScGridWindow::UpdateStatusPosSize()
{
    ScDrawView* pDrView = pViewData->GetView()->GetScDrawView();
    if (!pDrView)
        return;         // shouldn't be called in that case

    SdrPageView* pPV = pDrView->GetSdrPageView();
    if (!pPV)
        return;         // shouldn't be called in that case either

    SfxItemSet aSet(pViewData->GetViewShell()->GetPool(), SID_ATTR_POSITION, SID_ATTR_SIZE);

    //  Fill items for position and size:
    //  show action rectangle during action,
    //  position and size of selected object(s) if something is selected,
    //  mouse position otherwise

    sal_Bool bActionItem = false;
    if ( pDrView->IsAction() )              // action rectangle
    {
        Rectangle aRect;
        pDrView->TakeActionRect( aRect );
        if ( !aRect.IsEmpty() )
        {
            // mouse position will have been adjusted for offset
            // at current position and zoom, restore that adjustment here
            // so status shows correct value
            aRect -= pDrView->GetGridOffset();
            pPV->LogicToPagePos(aRect);
            aSet.Put( SfxPointItem( SID_ATTR_POSITION, aRect.TopLeft() ) );
            aSet.Put( SvxSizeItem( SID_ATTR_SIZE,
                    Size( aRect.Right() - aRect.Left(), aRect.Bottom() - aRect.Top() ) ) );
            bActionItem = sal_True;
        }
    }
    if ( !bActionItem )
    {
        if ( pDrView->AreObjectsMarked() )      // selected objects
        {
            Rectangle aRect = pDrView->GetAllMarkedRect();
            // mouse position will have been adjusted for offset
            // at current position and zoom, restore that adjustment here
            // so status shows correct value
            aRect -=  pDrView->GetGridOffset();
            pPV->LogicToPagePos(aRect);
            aSet.Put( SfxPointItem( SID_ATTR_POSITION, aRect.TopLeft() ) );
            aSet.Put( SvxSizeItem( SID_ATTR_SIZE,
                    Size( aRect.Right() - aRect.Left(), aRect.Bottom() - aRect.Top() ) ) );
        }
        else                                // mouse position
        {
            Point aPos = PixelToLogic(aCurMousePos);
            pPV->LogicToPagePos(aPos);
            aSet.Put( SfxPointItem( SID_ATTR_POSITION, aPos ) );
            aSet.Put( SvxSizeItem( SID_ATTR_SIZE, Size( 0, 0 ) ) );
        }
    }

    pViewData->GetBindings().SetState(aSet);
}

bool ScGridWindow::DrawHasMarkedObj()
{
    ScDrawView* p = pViewData->GetScDrawView();
    return p ? p->AreObjectsMarked() : false;
}

void ScGridWindow::DrawMarkDropObj( SdrObject* pObj )
{
    ScDrawView* pDrView = pViewData->GetView()->GetScDrawView();
    if (pDrView)
        pDrView->MarkDropObj(pObj);
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
