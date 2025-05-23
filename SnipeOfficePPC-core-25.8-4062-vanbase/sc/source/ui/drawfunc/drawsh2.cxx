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

#include <com/sun/star/embed/EmbedMisc.hpp>

#include "scitems.hxx"
#include <editeng/eeitem.hxx>
#include <editeng/sizeitem.hxx>
#include <svx/svdpagv.hxx>
#include <svx/xdef.hxx>
#include <sfx2/app.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/viewfrm.hxx>
#include <svl/ptitem.hxx>
#include <svl/whiter.hxx>
#include <svx/svdobj.hxx>
#include <svx/svdouno.hxx>
#include <svx/extrusionbar.hxx>
#include <svx/fontworkbar.hxx>

#include "drawsh.hxx"
#include "drawview.hxx"
#include "viewdata.hxx"
#include "sc.hrc"
#include "tabvwsh.hxx"
#include "document.hxx"
#include "drwlayer.hxx"
#include "userdat.hxx"
#include <svx/svdoole2.hxx>
#include <svx/svdocapt.hxx>

sal_uInt16 ScGetFontWorkId();       // in drtxtob

using namespace com::sun::star;

//------------------------------------------------------------------

ScDrawShell::ScDrawShell( ScViewData* pData ) :
    SfxShell(pData->GetViewShell()),
    pViewData( pData )
{
    SetPool( &pViewData->GetScDrawView()->GetModel()->GetItemPool() );
    ::svl::IUndoManager* pMgr = pViewData->GetSfxDocShell()->GetUndoManager();
    SetUndoManager( pMgr );
    if ( !pViewData->GetDocument()->IsUndoEnabled() )
    {
        pMgr->SetMaxUndoActionCount( 0 );
    }
    SetHelpId( HID_SCSHELL_DRAWSH );
    SetName(rtl::OUString("Drawing"));
}

ScDrawShell::~ScDrawShell()
{
}

void ScDrawShell::GetState( SfxItemSet& rSet )          // Zustaende / Toggles
{
    ScDrawView* pView    = pViewData->GetScDrawView();
    SdrDragMode eMode    = pView->GetDragMode();

    rSet.Put( SfxBoolItem( SID_OBJECT_ROTATE, eMode == SDRDRAG_ROTATE ) );
    rSet.Put( SfxBoolItem( SID_OBJECT_MIRROR, eMode == SDRDRAG_MIRROR ) );
    rSet.Put( SfxBoolItem( SID_BEZIER_EDIT, !pView->IsFrameDragSingles() ) );

    sal_uInt16 nFWId = ScGetFontWorkId();
    SfxViewFrame* pViewFrm = pViewData->GetViewShell()->GetViewFrame();
    rSet.Put(SfxBoolItem(SID_FONTWORK, pViewFrm->HasChildWindow(nFWId)));

        // Notes always default to Page anchor.
    bool bDisableAnchor = false;
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    sal_uLong nMarkCount = rMarkList.GetMarkCount();
    if ( nMarkCount == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();
        if( ScDrawLayer::IsNoteCaption( pObj ) )
        {
            bDisableAnchor = true;
            rSet.DisableItem( SID_ANCHOR_PAGE );
            rSet.DisableItem( SID_ANCHOR_CELL );
        }
    }

    if ( !bDisableAnchor )
    {
        switch( pView->GetAnchorType() )
        {
        case SCA_PAGE:
            rSet.Put( SfxBoolItem( SID_ANCHOR_PAGE, sal_True ) );
            rSet.Put( SfxBoolItem( SID_ANCHOR_CELL, false ) );
        break;

        case SCA_CELL:
        rSet.Put( SfxBoolItem( SID_ANCHOR_PAGE, false ) );
        rSet.Put( SfxBoolItem( SID_ANCHOR_CELL, sal_True ) );
        break;

        default:
        rSet.Put( SfxBoolItem( SID_ANCHOR_PAGE, false ) );
        rSet.Put( SfxBoolItem( SID_ANCHOR_CELL, false ) );
        break;
        }
    }
}

void ScDrawShell::GetDrawFuncState( SfxItemSet& rSet )      // Funktionen disablen
{
    ScDrawView* pView = pViewData->GetScDrawView();

    //  call IsMirrorAllowed first to make sure ForcePossibilities (and thus CheckMarked)
    //  is called before GetMarkCount, so the nMarkCount value is valid for the rest of this method.
    if (!pView->IsMirrorAllowed(sal_True,sal_True))
    {
        rSet.DisableItem( SID_MIRROR_HORIZONTAL );
        rSet.DisableItem( SID_MIRROR_VERTICAL );
    }

    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    sal_uLong nMarkCount = rMarkList.GetMarkCount();

    if ( nMarkCount <= 1 || !pView->IsGroupPossible() )
        rSet.DisableItem( SID_GROUP );
    if ( nMarkCount == 0 || !pView->IsUnGroupPossible() )
        rSet.DisableItem( SID_UNGROUP );
    if ( nMarkCount != 1 || !pView->IsGroupEnterPossible() )
        rSet.DisableItem( SID_ENTER_GROUP );
    if ( !pView->IsGroupEntered() )
        rSet.DisableItem( SID_LEAVE_GROUP );

    if ( nMarkCount <= 1 )                      // nichts oder nur ein Objekt selektiert
    {
            //  Ausrichtung
        rSet.DisableItem( SID_OBJECT_ALIGN_LEFT );      // keine Ausrichtung an der Seite
        rSet.DisableItem( SID_OBJECT_ALIGN_CENTER );
        rSet.DisableItem( SID_OBJECT_ALIGN_RIGHT );
        rSet.DisableItem( SID_OBJECT_ALIGN_UP );
        rSet.DisableItem( SID_OBJECT_ALIGN_MIDDLE );
        rSet.DisableItem( SID_OBJECT_ALIGN_DOWN );

        // pseudo slots for Format menu
        rSet.DisableItem( SID_ALIGN_ANY_LEFT );
        rSet.DisableItem( SID_ALIGN_ANY_HCENTER );
        rSet.DisableItem( SID_ALIGN_ANY_RIGHT );
        rSet.DisableItem( SID_ALIGN_ANY_TOP );
        rSet.DisableItem( SID_ALIGN_ANY_VCENTER );
        rSet.DisableItem( SID_ALIGN_ANY_BOTTOM );
    }

    // do not change layer of form controls
    // #i83729# do not change layer of cell notes (on internal layer)
    if ( !nMarkCount || pView->HasMarkedControl() || pView->HasMarkedInternal() )
    {
        rSet.DisableItem( SID_OBJECT_HEAVEN );
        rSet.DisableItem( SID_OBJECT_HELL );
    }
    else
    {
        if(AreAllObjectsOnLayer(SC_LAYER_FRONT,rMarkList))
        {
            rSet.DisableItem( SID_OBJECT_HEAVEN );
        }
        else if(AreAllObjectsOnLayer(SC_LAYER_BACK,rMarkList))
        {
            rSet.DisableItem( SID_OBJECT_HELL );
        }
    }

    sal_Bool bCanRename = false;
    if ( nMarkCount > 1 )
    {
        // no hypelink options for a selected group
        rSet.DisableItem( SID_DRAW_HLINK_EDIT );
        rSet.DisableItem( SID_DRAW_HLINK_DELETE );
        rSet.DisableItem( SID_OPEN_HYPERLINK );
    }
    else if ( nMarkCount == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();
        ScMacroInfo* pInfo = ScDrawLayer::GetMacroInfo( pObj );
        if ( !pInfo || pInfo->GetHlink().isEmpty() )
        {
            rSet.DisableItem( SID_DRAW_HLINK_DELETE );
            rSet.DisableItem( SID_OPEN_HYPERLINK );
        }
        SdrLayerID nLayerID = pObj->GetLayer();
        if ( nLayerID != SC_LAYER_INTERN )
            bCanRename = sal_True;                          // #i51351# anything except internal objects can be renamed

        // #91929#; don't show original size entry if not possible
        sal_uInt16 nObjType = pObj->GetObjIdentifier();
        if ( nObjType == OBJ_OLE2 )
        {
            SdrOle2Obj* pOleObj = static_cast<SdrOle2Obj*>(rMarkList.GetMark( 0 )->GetMarkedSdrObj());
            if (pOleObj->GetObjRef().is() &&
                ((pOleObj->GetObjRef()->getStatus( pOleObj->GetAspect() ) & embed::EmbedMisc::MS_EMBED_RECOMPOSEONRESIZE) ) )
                //TODO/LATER: why different slots in Draw and Calc?
                rSet.DisableItem(SID_ORIGINALSIZE);
        }
        else if ( nObjType == OBJ_CAPTION )
        {
            if ( nLayerID == SC_LAYER_INTERN )
            {
                // SdrCaptionObj() Notes cannot be cut/copy in isolation from
                // their cells.
                rSet.DisableItem( SID_CUT );
                rSet.DisableItem( SID_COPY );
                // Notes always default to Page anchor.
                rSet.DisableItem( SID_ANCHOR_TOGGLE );
            }
        }
    }
    if ( !bCanRename )
    {
        // #i68101#
        rSet.DisableItem( SID_RENAME_OBJECT );
        rSet.DisableItem( SID_TITLE_DESCRIPTION_OBJECT );
    }

    if ( !nMarkCount )                          // nichts selektiert
    {
            //  Anordnung
        rSet.DisableItem( SID_FRAME_UP );
        rSet.DisableItem( SID_FRAME_DOWN );
        rSet.DisableItem( SID_FRAME_TO_TOP );
        rSet.DisableItem( SID_FRAME_TO_BOTTOM );
            //  Clipboard / loeschen
        rSet.DisableItem( SID_DELETE );
        rSet.DisableItem( SID_DELETE_CONTENTS );
        rSet.DisableItem( SID_CUT );
        rSet.DisableItem( SID_COPY );
            //  sonstiges
        rSet.DisableItem( SID_ANCHOR_TOGGLE );
        rSet.DisableItem( SID_ORIGINALSIZE );
        rSet.DisableItem( SID_ATTR_TRANSFORM );
    }

    if ( rSet.GetItemState( SID_ENABLE_HYPHENATION ) != SFX_ITEM_UNKNOWN )
    {
        SfxItemSet aAttrs( pView->GetModel()->GetItemPool() );
        pView->GetAttributes( aAttrs );
        if( aAttrs.GetItemState( EE_PARA_HYPHENATE ) >= SFX_ITEM_AVAILABLE )
        {
            sal_Bool bValue = ( (const SfxBoolItem&) aAttrs.Get( EE_PARA_HYPHENATE ) ).GetValue();
            rSet.Put( SfxBoolItem( SID_ENABLE_HYPHENATION, bValue ) );
        }
    }

    svx::ExtrusionBar::getState( pView, rSet );
    svx::FontworkBar::getState( pView, rSet );
}

//
//          Attribute fuer Drawing-Objekte
//

void ScDrawShell::GetDrawAttrState( SfxItemSet& rSet )
{
    Point       aMousePos   = pViewData->GetMousePosPixel();
    Window*     pWindow     = pViewData->GetActiveWin();
    ScDrawView* pDrView     = pViewData->GetScDrawView();
    Point       aPos        = pWindow->PixelToLogic(aMousePos);
    sal_Bool        bHasMarked  = pDrView->AreObjectsMarked();

    if( bHasMarked )
    {
        rSet.Put( pDrView->GetAttrFromMarked(false) );

        // Wenn die View selektierte Objekte besitzt, muessen entspr. Items
        // von SFX_ITEM_DEFAULT (_ON) auf SFX_ITEM_DISABLED geaendert werden

        SfxWhichIter aIter( rSet, XATTR_LINE_FIRST, XATTR_FILL_LAST );
        sal_uInt16 nWhich = aIter.FirstWhich();
        while( nWhich )
        {
            if( SFX_ITEM_DEFAULT == rSet.GetItemState( nWhich ) )
                rSet.DisableItem( nWhich );

            nWhich = aIter.NextWhich();
        }
    }
    else
        rSet.Put( pDrView->GetDefaultAttr() );

    SdrPageView* pPV = pDrView->GetSdrPageView();
    if ( pPV )
    {
        // #i52073# when a sheet with an active OLE object is deleted,
        // the slot state is queried without an active page view

        //  Items for position and size (see ScGridWindow::UpdateStatusPosSize, #108137#)

        // #i34458# The SvxSizeItem in SID_TABLE_CELL is no longer needed by
        // SvxPosSizeStatusBarControl, it's enough to have it in SID_ATTR_SIZE.

        sal_Bool bActionItem = false;
        if ( pDrView->IsAction() )              // action rectangle
        {
            Rectangle aRect;
            pDrView->TakeActionRect( aRect );
            if ( !aRect.IsEmpty() )
            {
                pPV->LogicToPagePos(aRect);
                rSet.Put( SfxPointItem( SID_ATTR_POSITION, aRect.TopLeft() ) );
                Size aSize( aRect.Right() - aRect.Left(), aRect.Bottom() - aRect.Top() );
                rSet.Put( SvxSizeItem( SID_ATTR_SIZE, aSize ) );
                bActionItem = sal_True;
            }
        }
        if ( !bActionItem )
        {
            if ( pDrView->AreObjectsMarked() )      // selected objects
            {
                Rectangle aRect = pDrView->GetAllMarkedRect();
                pPV->LogicToPagePos(aRect);
                rSet.Put( SfxPointItem( SID_ATTR_POSITION, aRect.TopLeft() ) );
                Size aSize( aRect.Right() - aRect.Left(), aRect.Bottom() - aRect.Top() );
                rSet.Put( SvxSizeItem( SID_ATTR_SIZE, aSize ) );
            }
            else                                // mouse position
            {
                // aPos is initialized above
                pPV->LogicToPagePos(aPos);
                rSet.Put( SfxPointItem( SID_ATTR_POSITION, aPos ) );
                rSet.Put( SvxSizeItem( SID_ATTR_SIZE, Size( 0, 0 ) ) );
            }
        }
    }
}

void ScDrawShell::GetAttrFuncState(SfxItemSet &rSet)
{
    //  Dialoge fuer Draw-Attribute disablen, wenn noetig

    ScDrawView* pDrView = pViewData->GetScDrawView();
    SfxItemSet aViewSet = pDrView->GetAttrFromMarked(false);

    if ( aViewSet.GetItemState( XATTR_LINESTYLE ) == SFX_ITEM_DEFAULT )
    {
        rSet.DisableItem( SID_ATTRIBUTES_LINE );
        rSet.DisableItem( SID_ATTR_LINEEND_STYLE );     // Tbx-Controller
    }

    if ( aViewSet.GetItemState( XATTR_FILLSTYLE ) == SFX_ITEM_DEFAULT )
        rSet.DisableItem( SID_ATTRIBUTES_AREA );
}

sal_Bool ScDrawShell::AreAllObjectsOnLayer(sal_uInt16 nLayerNo,const SdrMarkList& rMark)
{
    sal_Bool bResult=sal_True;
    sal_uLong nCount = rMark.GetMarkCount();
    for (sal_uLong i=0; i<nCount; i++)
    {
        SdrObject* pObj = rMark.GetMark(i)->GetMarkedSdrObj();
        if ( !pObj->ISA(SdrUnoObj) )
        {
            if(nLayerNo!=pObj->GetLayer())
            {
                bResult=false;
                break;
            }
        }
    }
    return bResult;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
