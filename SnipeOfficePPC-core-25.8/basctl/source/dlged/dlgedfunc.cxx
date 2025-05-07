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

#include <vcl/scrbar.hxx>
#include <svx/svdview.hxx>
#include "dlgedfunc.hxx"
#include "dlged.hxx"
#include "dlgedview.hxx"
#include <vcl/seleng.hxx>


//----------------------------------------------------------------------------

IMPL_LINK_INLINE_START( DlgEdFunc, ScrollTimeout, Timer *, pTimer )
{
    (void)pTimer;
    Window* pWindow = pParent->GetWindow();
    Point aPos = pWindow->ScreenToOutputPixel( pWindow->GetPointerPosPixel() );
    aPos = pWindow->PixelToLogic( aPos );
    ForceScroll( aPos );
    return 0;
}
IMPL_LINK_INLINE_END( DlgEdFunc, ScrollTimeout, Timer *, pTimer )

//----------------------------------------------------------------------------

void DlgEdFunc::ForceScroll( const Point& rPos )
{
    aScrollTimer.Stop();

    Window* pWindow  = pParent->GetWindow();

    static Point aDefPoint;
    Rectangle aOutRect( aDefPoint, pWindow->GetOutputSizePixel() );
    aOutRect = pWindow->PixelToLogic( aOutRect );

    ScrollBar* pHScroll = pParent->GetHScroll();
    ScrollBar* pVScroll = pParent->GetVScroll();
    long nDeltaX = pHScroll->GetLineSize();
    long nDeltaY = pVScroll->GetLineSize();

    if( !aOutRect.IsInside( rPos ) )
    {
        if( rPos.X() < aOutRect.Left() )
            nDeltaX = -nDeltaX;
        else
        if( rPos.X() <= aOutRect.Right() )
            nDeltaX = 0;

        if( rPos.Y() < aOutRect.Top() )
            nDeltaY = -nDeltaY;
        else
        if( rPos.Y() <= aOutRect.Bottom() )
            nDeltaY = 0;

        if( nDeltaX )
            pHScroll->SetThumbPos( pHScroll->GetThumbPos() + nDeltaX );
        if( nDeltaY )
            pVScroll->SetThumbPos( pVScroll->GetThumbPos() + nDeltaY );

        if( nDeltaX )
            pParent->DoScroll( pHScroll );
        if( nDeltaY )
            pParent->DoScroll( pVScroll );
    }

    aScrollTimer.Start();
}

//----------------------------------------------------------------------------

DlgEdFunc::DlgEdFunc( DlgEditor* pParent_ )
{
    DlgEdFunc::pParent = pParent_;
    aScrollTimer.SetTimeoutHdl( LINK( this, DlgEdFunc, ScrollTimeout ) );
    aScrollTimer.SetTimeout( SELENG_AUTOREPEAT_INTERVAL );
}

//----------------------------------------------------------------------------

DlgEdFunc::~DlgEdFunc()
{
}

//----------------------------------------------------------------------------

sal_Bool DlgEdFunc::MouseButtonDown( const MouseEvent& )
{
    return sal_True;
}

//----------------------------------------------------------------------------

sal_Bool DlgEdFunc::MouseButtonUp( const MouseEvent& )
{
    aScrollTimer.Stop();
    return sal_True;
}

//----------------------------------------------------------------------------

sal_Bool DlgEdFunc::MouseMove( const MouseEvent& )
{
    return sal_True;
}

//----------------------------------------------------------------------------

sal_Bool DlgEdFunc::KeyInput( const KeyEvent& rKEvt )
{
    sal_Bool bReturn = sal_False;

    SdrView* pView = pParent->GetView();
    Window* pWindow = pParent->GetWindow();

    KeyCode aCode = rKEvt.GetKeyCode();
    sal_uInt16 nCode = aCode.GetCode();

    switch ( nCode )
    {
        case KEY_ESCAPE:
        {
            if ( pView->IsAction() )
            {
                pView->BrkAction();
                bReturn = sal_True;
            }
            else if ( pView->AreObjectsMarked() )
            {
                const SdrHdlList& rHdlList = pView->GetHdlList();
                SdrHdl* pHdl = rHdlList.GetFocusHdl();
                if ( pHdl )
                    ((SdrHdlList&)rHdlList).ResetFocusHdl();
                else
                    pView->UnmarkAll();

                bReturn = sal_True;
            }
        }
        break;
        case KEY_TAB:
        {
            if ( !aCode.IsMod1() && !aCode.IsMod2() )
            {
                // mark next object
                if ( !pView->MarkNextObj( !aCode.IsShift() ) )
                {
                    // if no next object, mark first/last
                    pView->UnmarkAllObj();
                    pView->MarkNextObj( !aCode.IsShift() );
                }

                if ( pView->AreObjectsMarked() )
                    pView->MakeVisible( pView->GetAllMarkedRect(), *pWindow );

                bReturn = sal_True;
            }
            else if ( aCode.IsMod1() )
            {
                // selected handle
                const SdrHdlList& rHdlList = pView->GetHdlList();
                ((SdrHdlList&)rHdlList).TravelFocusHdl( !aCode.IsShift() );

                // guarantee visibility of focused handle
                SdrHdl* pHdl = rHdlList.GetFocusHdl();
                if ( pHdl )
                {
                    Point aHdlPosition( pHdl->GetPos() );
                    Rectangle aVisRect( aHdlPosition - Point( 100, 100 ), Size( 200, 200 ) );
                    pView->MakeVisible( aVisRect, *pWindow );
                }

                bReturn = sal_True;
            }
        }
        break;
        case KEY_UP:
        case KEY_DOWN:
        case KEY_LEFT:
        case KEY_RIGHT:
        {
            long nX = 0;
            long nY = 0;

            if ( nCode == KEY_UP )
            {
                // scroll up
                nX =  0;
                nY = -1;
            }
            else if ( nCode == KEY_DOWN )
            {
                // scroll down
                nX =  0;
                nY =  1;
            }
            else if ( nCode == KEY_LEFT )
            {
                // scroll left
                nX = -1;
                nY =  0;
            }
            else if ( nCode == KEY_RIGHT )
            {
                // scroll right
                nX =  1;
                nY =  0;
            }

            if ( pView->AreObjectsMarked() && !aCode.IsMod1() )
            {
                if ( aCode.IsMod2() )
                {
                    // move in 1 pixel distance
                    Size aPixelSize = pWindow ? pWindow->PixelToLogic( Size( 1, 1 ) ) : Size( 100, 100 );
                    nX *= aPixelSize.Width();
                    nY *= aPixelSize.Height();
                }
                else
                {
                    // move in 1 mm distance
                    nX *= 100;
                    nY *= 100;
                }

                const SdrHdlList& rHdlList = pView->GetHdlList();
                SdrHdl* pHdl = rHdlList.GetFocusHdl();

                if ( pHdl == 0 )
                {
                    // no handle selected
                    if ( pView->IsMoveAllowed() )
                    {
                        // restrict movement to work area
                        const Rectangle& rWorkArea = pView->GetWorkArea();

                        if ( !rWorkArea.IsEmpty() )
                        {
                            Rectangle aMarkRect( pView->GetMarkedObjRect() );
                            aMarkRect.Move( nX, nY );

                            if ( !rWorkArea.IsInside( aMarkRect ) )
                            {
                                if ( aMarkRect.Left() < rWorkArea.Left() )
                                    nX += rWorkArea.Left() - aMarkRect.Left();

                                if ( aMarkRect.Right() > rWorkArea.Right() )
                                    nX -= aMarkRect.Right() - rWorkArea.Right();

                                if ( aMarkRect.Top() < rWorkArea.Top() )
                                    nY += rWorkArea.Top() - aMarkRect.Top();

                                if ( aMarkRect.Bottom() > rWorkArea.Bottom() )
                                    nY -= aMarkRect.Bottom() - rWorkArea.Bottom();
                            }
                        }

                        if ( nX != 0 || nY != 0 )
                        {
                            pView->MoveAllMarked( Size( nX, nY ) );
                            pView->MakeVisible( pView->GetAllMarkedRect(), *pWindow );
                        }
                    }
                }
                else
                {
                    // move the handle
                    if ( pHdl && ( nX || nY ) )
                    {
                        Point aStartPoint( pHdl->GetPos() );
                        Point aEndPoint( pHdl->GetPos() + Point( nX, nY ) );
                        const SdrDragStat& rDragStat = pView->GetDragStat();

                        // start dragging
                        pView->BegDragObj( aStartPoint, 0, pHdl, 0 );

                        if ( pView->IsDragObj() )
                        {
                            bool bWasNoSnap = rDragStat.IsNoSnap();
                            sal_Bool bWasSnapEnabled = pView->IsSnapEnabled();

                            // switch snapping off
                            if ( !bWasNoSnap )
                                ((SdrDragStat&)rDragStat).SetNoSnap( sal_True );
                            if ( bWasSnapEnabled )
                                pView->SetSnapEnabled( sal_False );

                            pView->MovAction( aEndPoint );
                            pView->EndDragObj();

                            // restore snap
                            if ( !bWasNoSnap )
                                ((SdrDragStat&)rDragStat).SetNoSnap( bWasNoSnap );
                            if ( bWasSnapEnabled )
                                pView->SetSnapEnabled( bWasSnapEnabled );
                        }

                        // make moved handle visible
                        Rectangle aVisRect( aEndPoint - Point( 100, 100 ), Size( 200, 200 ) );
                        pView->MakeVisible( aVisRect, *pWindow );
                    }
                }
            }
            else
            {
                // scroll page
                ScrollBar* pScrollBar = ( nX != 0 ) ? pParent->GetHScroll() : pParent->GetVScroll();
                if ( pScrollBar )
                {
                    long nRangeMin = pScrollBar->GetRangeMin();
                    long nRangeMax = pScrollBar->GetRangeMax();
                    long nThumbPos = pScrollBar->GetThumbPos() + ( ( nX != 0 ) ? nX : nY ) * pScrollBar->GetLineSize();
                    if ( nThumbPos < nRangeMin )
                        nThumbPos = nRangeMin;
                    if ( nThumbPos > nRangeMax )
                        nThumbPos = nRangeMax;
                    pScrollBar->SetThumbPos( nThumbPos );
                    pParent->DoScroll( pScrollBar );
                }
            }

            bReturn = sal_True;
        }
        break;
        default:
        {
        }
        break;
    }

    if ( bReturn )
        pWindow->ReleaseMouse();

    return bReturn;
}

//----------------------------------------------------------------------------

DlgEdFuncInsert::DlgEdFuncInsert( DlgEditor* pParent_ ) :
    DlgEdFunc( pParent_ )
{
    pParent_->GetView()->SetCreateMode( sal_True );
}

//----------------------------------------------------------------------------

DlgEdFuncInsert::~DlgEdFuncInsert()
{
    pParent->GetView()->SetEditMode( sal_True );
}

//----------------------------------------------------------------------------

sal_Bool DlgEdFuncInsert::MouseButtonDown( const MouseEvent& rMEvt )
{
    if( !rMEvt.IsLeft() )
        return sal_True;

    SdrView* pView  = pParent->GetView();
    Window*  pWindow= pParent->GetWindow();
    pView->SetActualWin( pWindow );

    Point aPos = pWindow->PixelToLogic( rMEvt.GetPosPixel() );
    sal_uInt16 nHitLog = sal_uInt16 ( pWindow->PixelToLogic(Size(3,0)).Width() );
    sal_uInt16 nDrgLog = sal_uInt16 ( pWindow->PixelToLogic(Size(3,0)).Width() );

    pWindow->CaptureMouse();

    if ( rMEvt.IsLeft() && rMEvt.GetClicks() == 1 )
    {
        SdrHdl* pHdl = pView->PickHandle(aPos);

        // if selected object was hit, drag object
        if ( pHdl!=NULL || pView->IsMarkedHit(aPos, nHitLog) )
            pView->BegDragObj(aPos, (OutputDevice*) NULL, pHdl, nDrgLog);
        else if ( pView->AreObjectsMarked() )
            pView->UnmarkAll();

        // if no action, create object
        if ( !pView->IsAction() )
            pView->BegCreateObj(aPos);
    }
    else if ( rMEvt.IsLeft() && rMEvt.GetClicks() == 2 )
    {
        // if object was hit, show property browser
        if ( pView->IsMarkedHit(aPos, nHitLog) && pParent->GetMode() != DLGED_READONLY )
            pParent->ShowProperties();
    }

    return sal_True;
}

//----------------------------------------------------------------------------

sal_Bool DlgEdFuncInsert::MouseButtonUp( const MouseEvent& rMEvt )
{
    DlgEdFunc::MouseButtonUp( rMEvt );

    SdrView* pView  = pParent->GetView();
    Window*  pWindow= pParent->GetWindow();
    pView->SetActualWin( pWindow );

    pWindow->ReleaseMouse();

    // object creation active?
    if ( pView->IsCreateObj() )
    {
        pView->EndCreateObj(SDRCREATE_FORCEEND);

        if ( !pView->AreObjectsMarked() )
        {
            sal_uInt16 nHitLog = sal_uInt16 ( pWindow->PixelToLogic(Size(3,0)).Width() );
            Point aPos( pWindow->PixelToLogic( rMEvt.GetPosPixel() ) );
            pView->MarkObj(aPos, nHitLog);
        }

        if( pView->AreObjectsMarked() )
            return sal_True;
        else
            return sal_False;
    }
    else
    {
        if ( pView->IsDragObj() )
             pView->EndDragObj( rMEvt.IsMod1() );
        return sal_True;
    }
}

//----------------------------------------------------------------------------

sal_Bool DlgEdFuncInsert::MouseMove( const MouseEvent& rMEvt )
{
    SdrView* pView  = pParent->GetView();
    Window*  pWindow= pParent->GetWindow();
    pView->SetActualWin( pWindow );

    Point   aPos( pWindow->PixelToLogic( rMEvt.GetPosPixel() ) );
    sal_uInt16 nHitLog = sal_uInt16 ( pWindow->PixelToLogic(Size(3,0)).Width() );

    if ( pView->IsAction() )
    {
        ForceScroll(aPos);
        pView->MovAction(aPos);
    }

    pWindow->SetPointer( pView->GetPreferedPointer( aPos, pWindow, nHitLog ) );

    return sal_True;
}

//----------------------------------------------------------------------------

DlgEdFuncSelect::DlgEdFuncSelect( DlgEditor* pParent_ ) :
    DlgEdFunc( pParent_ ),
    bMarkAction(sal_False)
{
}

//----------------------------------------------------------------------------

DlgEdFuncSelect::~DlgEdFuncSelect()
{
}

//----------------------------------------------------------------------------

sal_Bool DlgEdFuncSelect::MouseButtonDown( const MouseEvent& rMEvt )
{
    // get view from parent
    SdrView* pView   = pParent->GetView();
    Window*  pWindow = pParent->GetWindow();
    pView->SetActualWin( pWindow );

    sal_uInt16 nDrgLog = sal_uInt16 ( pWindow->PixelToLogic(Size(3,0)).Width() );
    sal_uInt16 nHitLog = sal_uInt16 ( pWindow->PixelToLogic(Size(3,0)).Width() );
    Point  aMDPos = pWindow->PixelToLogic( rMEvt.GetPosPixel() );

    if ( rMEvt.IsLeft() && rMEvt.GetClicks() == 1 )
    {
        SdrHdl* pHdl = pView->PickHandle(aMDPos);
        SdrObject* pObj;
        SdrPageView* pPV;

        // hit selected object?
        if ( pHdl!=NULL || pView->IsMarkedHit(aMDPos, nHitLog) )
        {
            pView->BegDragObj(aMDPos, (OutputDevice*) NULL, pHdl, nDrgLog);
        }
        else
        {
            // if not multi selection, unmark all
            if ( !rMEvt.IsShift() )
                pView->UnmarkAll();
            else
            {
                if( pView->PickObj( aMDPos, nHitLog, pObj, pPV ) )
                {
                    //if( pObj->ISA( DlgEdForm ) )
                    //  pView->UnmarkAll();
                    //else
                    //  pParent->UnmarkDialog();
                }
            }

            if ( pView->MarkObj(aMDPos, nHitLog) )
            {
                // drag object
                pHdl=pView->PickHandle(aMDPos);
                pView->BegDragObj(aMDPos, (OutputDevice*) NULL, pHdl, nDrgLog);
            }
            else
            {
                // select object
                pView->BegMarkObj(aMDPos);
                bMarkAction = sal_True;
            }
        }
    }
    else if ( rMEvt.IsLeft() && rMEvt.GetClicks() == 2 )
    {
        // if object was hit, show property browser
        if ( pView->IsMarkedHit(aMDPos, nHitLog) && pParent->GetMode() != DLGED_READONLY )
            pParent->ShowProperties();
    }

    return sal_True;
}

//----------------------------------------------------------------------------

sal_Bool DlgEdFuncSelect::MouseButtonUp( const MouseEvent& rMEvt )
{
    DlgEdFunc::MouseButtonUp( rMEvt );

    // get view from parent
    SdrView* pView  = pParent->GetView();
    Window*  pWindow= pParent->GetWindow();
    pView->SetActualWin( pWindow );

    Point aPnt( pWindow->PixelToLogic( rMEvt.GetPosPixel() ) );
    sal_uInt16 nHitLog = sal_uInt16 ( pWindow->PixelToLogic(Size(3,0)).Width() );

    if ( rMEvt.IsLeft() )
    {
        if ( pView->IsDragObj() )
        {
            // object was dragged
            pView->EndDragObj( rMEvt.IsMod1() );
            pView->ForceMarkedToAnotherPage();
        }
        else
        if (pView->IsAction() )
        {
            pView->EndAction();
        }
    }

    bMarkAction = sal_False;

    pWindow->SetPointer( pView->GetPreferedPointer( aPnt, pWindow, nHitLog ) );
    pWindow->ReleaseMouse();

    return sal_True;
}

//----------------------------------------------------------------------------

sal_Bool DlgEdFuncSelect::MouseMove( const MouseEvent& rMEvt )
{
    SdrView* pView  = pParent->GetView();
    Window*  pWindow= pParent->GetWindow();
    pView->SetActualWin( pWindow );

    Point aPnt( pWindow->PixelToLogic( rMEvt.GetPosPixel() ) );
    sal_uInt16 nHitLog = sal_uInt16 ( pWindow->PixelToLogic(Size(3,0)).Width() );

    if ( pView->IsAction() )
    {
        Point aPix(rMEvt.GetPosPixel());
        Point aPnt_(pWindow->PixelToLogic(aPix));

        ForceScroll(aPnt_);
        pView->MovAction(aPnt_);
    }

    pWindow->SetPointer( pView->GetPreferedPointer( aPnt, pWindow, nHitLog ) );

    return sal_True;
}

//----------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
