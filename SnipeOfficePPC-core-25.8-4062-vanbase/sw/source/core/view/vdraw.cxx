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

#include <svx/svdmodel.hxx>
#include <svx/svdpage.hxx>
#include <tools/shl.hxx>
#include <swmodule.hxx>
#include <svtools/accessibilityoptions.hxx>
#include <svx/svdpagv.hxx>
#include <fmtanchr.hxx>
#include <frmfmt.hxx>

#include <svx/svdoutl.hxx>

#include "fesh.hxx"
#include "pagefrm.hxx"
#include "rootfrm.hxx"
#include "viewimp.hxx"
#include "dflyobj.hxx"
#include "viewopt.hxx"
#include "printdata.hxx"
#include "dcontact.hxx"
#include "dview.hxx"
#include "flyfrm.hxx"
#include <vcl/svapp.hxx>

#include <IDocumentDrawModelAccess.hxx>


void SwViewImp::StartAction()
{
    if ( HasDrawView() )
    {
        SET_CURR_SHELL( GetShell() );
        if ( pSh->ISA(SwFEShell) )
            ((SwFEShell*)pSh)->HideChainMarker(); // might have changed
    }
}


void SwViewImp::EndAction()
{
    if ( HasDrawView() )
    {
        SET_CURR_SHELL( GetShell() );
        if ( pSh->ISA(SwFEShell) )
            ((SwFEShell*)pSh)->SetChainMarker(); // might have changed
    }
}


void SwViewImp::LockPaint()
{
    if ( HasDrawView() )
    {
        bResetHdlHiddenPaint = !GetDrawView()->areMarkHandlesHidden();
        GetDrawView()->hideMarkHandles();
    }
    else
    {
        bResetHdlHiddenPaint = sal_False;
    }
}


void SwViewImp::UnlockPaint()
{
    if ( bResetHdlHiddenPaint )
        GetDrawView()->showMarkHandles();
}


void SwViewImp::PaintLayer( const SdrLayerID _nLayerID,
                            SwPrintData const*const pPrintData,
                            const SwRect& aPaintRect,
                            const Color* _pPageBackgrdColor,
                            const bool _bIsPageRightToLeft,
                            sdr::contact::ViewObjectContactRedirector* pRedirector ) const
{
    if ( HasDrawView() )
    {
        //change the draw mode in high contrast mode
        OutputDevice* pOutDev = GetShell()->GetOut();
        sal_uLong nOldDrawMode = pOutDev->GetDrawMode();
        if( GetShell()->GetWin() &&
            Application::GetSettings().GetStyleSettings().GetHighContrastMode() &&
            (!GetShell()->IsPreView()||SW_MOD()->GetAccessibilityOptions().GetIsForPagePreviews()))
        {
            pOutDev->SetDrawMode( nOldDrawMode | DRAWMODE_SETTINGSLINE | DRAWMODE_SETTINGSFILL |
                                DRAWMODE_SETTINGSTEXT | DRAWMODE_SETTINGSGRADIENT );
        }

        // For correct handling of accessibility, high contrast, the
        // page background color is set as the background color at the
        // outliner of the draw view.  Only necessary for the layers
        // hell and heaven
        Color aOldOutlinerBackgrdColor;
        // set default horizontal text direction on painting <hell> or
        // <heaven>.
        EEHorizontalTextDirection aOldEEHoriTextDir = EE_HTEXTDIR_L2R;
        const IDocumentDrawModelAccess* pIDDMA = GetShell()->getIDocumentDrawModelAccess();
        if ( (_nLayerID == pIDDMA->GetHellId()) ||
             (_nLayerID == pIDDMA->GetHeavenId()) )
        {
            OSL_ENSURE( _pPageBackgrdColor,
                    "incorrect usage of SwViewImp::PaintLayer: pPageBackgrdColor have to be set for painting layer <hell> or <heaven>");
            if ( _pPageBackgrdColor )
            {
                aOldOutlinerBackgrdColor =
                        GetDrawView()->GetModel()->GetDrawOutliner().GetBackgroundColor();
                GetDrawView()->GetModel()->GetDrawOutliner().SetBackgroundColor( *_pPageBackgrdColor );
            }

            aOldEEHoriTextDir =
                GetDrawView()->GetModel()->GetDrawOutliner().GetDefaultHorizontalTextDirection();
            EEHorizontalTextDirection aEEHoriTextDirOfPage =
                _bIsPageRightToLeft ? EE_HTEXTDIR_R2L : EE_HTEXTDIR_L2R;
            GetDrawView()->GetModel()->GetDrawOutliner().SetDefaultHorizontalTextDirection( aEEHoriTextDirOfPage );
        }

        pOutDev->Push( PUSH_LINECOLOR ); // #114231#
        if (pPrintData)
        {
            // hide drawings but not form controls (form controls are handled elsewhere)
            SdrView &rSdrView = const_cast< SdrView & >(GetPageView()->GetView());
            rSdrView.setHideDraw( !pPrintData->IsPrintDraw() );
        }
        GetPageView()->DrawLayer( _nLayerID, pOutDev, pRedirector, aPaintRect.SVRect() );
        pOutDev->Pop();

        // reset background color of the outliner & default horiz. text dir.
        if ( (_nLayerID == pIDDMA->GetHellId()) ||
             (_nLayerID == pIDDMA->GetHeavenId()) )
        {
            GetDrawView()->GetModel()->GetDrawOutliner().SetBackgroundColor( aOldOutlinerBackgrdColor );
            GetDrawView()->GetModel()->GetDrawOutliner().SetDefaultHorizontalTextDirection( aOldEEHoriTextDir );
        }

        pOutDev->SetDrawMode( nOldDrawMode );
    }
}


#define WIEDUWILLST 400

sal_Bool SwViewImp::IsDragPossible( const Point &rPoint )
{
    if ( !HasDrawView() )
        return sal_False;

    const SdrMarkList &rMrkList = GetDrawView()->GetMarkedObjectList();

    if( !rMrkList.GetMarkCount() )
        return sal_False;

    SdrObject *pO = rMrkList.GetMark(rMrkList.GetMarkCount()-1)->GetMarkedSdrObj();

    SwRect aRect;
    if( pO && ::CalcClipRect( pO, aRect, sal_False ) )
    {
        SwRect aTmp;
        ::CalcClipRect( pO, aTmp, sal_True );
        aRect.Union( aTmp );
    }
    else
        aRect = GetShell()->GetLayout()->Frm();

    aRect.Top(    aRect.Top()    - WIEDUWILLST );
    aRect.Bottom( aRect.Bottom() + WIEDUWILLST );
    aRect.Left(   aRect.Left()   - WIEDUWILLST );
    aRect.Right(  aRect.Right()  + WIEDUWILLST );
    return aRect.IsInside( rPoint );
}


void SwViewImp::NotifySizeChg( const Size &rNewSz )
{
    if ( !HasDrawView() )
        return;

    if ( GetPageView() )
        GetPageView()->GetPage()->SetSize( rNewSz );

    // Limitation of the work area
    const Rectangle aRect( Point( DOCUMENTBORDER, DOCUMENTBORDER ), rNewSz );
    const Rectangle &rOldWork = GetDrawView()->GetWorkArea();
    sal_Bool bCheckDrawObjs = sal_False;
    if ( aRect != rOldWork )
    {
        if ( rOldWork.Bottom() > aRect.Bottom() || rOldWork.Right() > aRect.Right())
            bCheckDrawObjs = sal_True;
        GetDrawView()->SetWorkArea( aRect );
    }
    if ( !bCheckDrawObjs )
        return;

    OSL_ENSURE( pSh->getIDocumentDrawModelAccess()->GetDrawModel(), "NotifySizeChg without DrawModel" );
    SdrPage* pPage = pSh->getIDocumentDrawModelAccess()->GetDrawModel()->GetPage( 0 );
    const sal_uLong nObjs = pPage->GetObjCount();
    for( sal_uLong nObj = 0; nObj < nObjs; ++nObj )
    {
        SdrObject *pObj = pPage->GetObj( nObj );
        if( !pObj->ISA(SwVirtFlyDrawObj) )
        {
            // Objects not anchored to the frame, do not need to be adjusted
            const SwContact *pCont = (SwContact*)GetUserCall(pObj);
            // this function might be called by the InsertDocument, when
            // a PageDesc-Attribute is set on a node. Then the SdrObject
            // must not have an UserCall.
            if( !pCont || !pCont->ISA(SwDrawContact) )
                continue;

            const SwFrm *pAnchor = ((SwDrawContact*)pCont)->GetAnchorFrm();
            if ( !pAnchor || pAnchor->IsInFly() || !pAnchor->IsValid() ||
                 !pAnchor->GetUpper() || !pAnchor->FindPageFrm() ||
                 (FLY_AS_CHAR == pCont->GetFmt()->GetAnchor().GetAnchorId()) )
            {
                continue;
            }

            // no move for drawing objects in header/footer
            if ( pAnchor->FindFooterOrHeader() )
            {
                continue;
            }

            const Rectangle aBound( pObj->GetCurrentBoundRect() );
            if ( !aRect.IsInside( aBound ) )
            {
                Size aSz;
                if ( aBound.Left() > aRect.Right() )
                    aSz.Width() = (aRect.Right() - aBound.Left()) - MINFLY;
                if ( aBound.Top() > aRect.Bottom() )
                    aSz.Height() = (aRect.Bottom() - aBound.Top()) - MINFLY;
                if ( aSz.Width() || aSz.Height() )
                    pObj->Move( aSz );

                // Don't let large objects dissappear to the top
                aSz.Width() = aSz.Height() = 0;
                if ( aBound.Bottom() < aRect.Top() )
                    aSz.Width() = (aBound.Bottom() - aRect.Top()) - MINFLY;
                if ( aBound.Right() < aRect.Left() )
                    aSz.Height() = (aBound.Right() - aRect.Left()) - MINFLY;
                if ( aSz.Width() || aSz.Height() )
                    pObj->Move( aSz );
            }
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
