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

#include <svx/svdmark.hxx>
#include <svx/svdview.hxx>
#include <svx/galbrws.hxx>
#include <svx/gallery.hxx>
#include <svx/hlnkitem.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/request.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/dispatch.hxx>
#include <svl/whiter.hxx>

#include "tabvwsh.hxx"
#include "viewdata.hxx"
#include "tabview.hxx"
#include "drwlayer.hxx"
#include "userdat.hxx"
#include "docsh.hxx"

// forwards -> galwrap.cxx (wg. CLOOKs)

sal_uInt16  GallerySGA_FORMAT_GRAPHIC();
Graphic GalleryGetGraphic       ();
sal_Bool    GalleryIsLinkage        ();
String  GalleryGetFullPath      ();
String  GalleryGetFilterName    ();

// forwards -> imapwrap.cxx (wg. CLOOKs)

class SvxIMapDlg;

sal_uInt16          ScIMapChildWindowId();
SvxIMapDlg*     ScGetIMapDlg();
const void*     ScIMapDlgGetObj( SvxIMapDlg* pDlg );
const ImageMap& ScIMapDlgGetMap( SvxIMapDlg* pDlg );

//------------------------------------------------------------------

void ScTabViewShell::ExecChildWin(SfxRequest& rReq)
{
    sal_uInt16 nSlot = rReq.GetSlot();
    switch(nSlot)
    {
        case SID_GALLERY:
        {
            SfxViewFrame* pThisFrame = GetViewFrame();
            pThisFrame->ToggleChildWindow( GalleryChildWindow::GetChildWindowId() );
            pThisFrame->GetBindings().Invalidate( SID_GALLERY );
            rReq.Ignore();
        }
        break;
    }
}

void ScTabViewShell::GetChildWinState( SfxItemSet& rSet )
{
    if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_GALLERY ) )
    {
        sal_uInt16 nId = GalleryChildWindow::GetChildWindowId();
        rSet.Put( SfxBoolItem( SID_GALLERY, GetViewFrame()->HasChildWindow( nId ) ) );
    }
}

//------------------------------------------------------------------

void ScTabViewShell::ExecGallery( SfxRequest& rReq )
{
    const SfxItemSet* pArgs = rReq.GetArgs();

    if ( pArgs )
    {
        const SfxPoolItem* pItem = NULL;
        SfxItemState eState = pArgs->GetItemState(SID_GALLERY_FORMATS, sal_True, &pItem);
        if ( eState == SFX_ITEM_SET )
        {
            sal_uInt32 nFormats = ((const SfxUInt32Item*)pItem)->GetValue();

            /******************************************************************
            * Graphik einfuegen
            ******************************************************************/
            if ( nFormats & GallerySGA_FORMAT_GRAPHIC() )
            {
                MakeDrawLayer();

                Graphic aGraphic = GalleryGetGraphic();
                Point   aPos     = GetInsertPos();

                String aPath, aFilter;
                if ( GalleryIsLinkage() )           // als Link einfuegen?
                {
                    aPath = GalleryGetFullPath();
                    aFilter = GalleryGetFilterName();
                }

                PasteGraphic( aPos, aGraphic, aPath, aFilter );
            }
            else if ( nFormats & SGA_FORMAT_SOUND )
            {
                //  for sounds (linked or not), insert a hyperlink button,
                //  like in Impress and Writer

                GalleryExplorer* pGal = SVX_GALLERY();
                if ( pGal )
                {
                    const SfxStringItem aMediaURLItem( SID_INSERT_AVMEDIA, pGal->GetURL().GetMainURL( INetURLObject::NO_DECODE ) );
                       GetViewFrame()->GetDispatcher()->Execute( SID_INSERT_AVMEDIA, SFX_CALLMODE_SYNCHRON, &aMediaURLItem, 0L );
                }
            }
        }
    }
}

void ScTabViewShell::GetGalleryState( SfxItemSet& /* rSet */ )
{
}

//------------------------------------------------------------------

ScInputHandler* ScTabViewShell::GetInputHandler() const
{
    return pInputHandler;
}

//------------------------------------------------------------------

String ScTabViewShell::GetDescription() const
{
    return rtl::OUString(" ** Test ** ");
}

void ScTabViewShell::ExecImageMap( SfxRequest& rReq )
{
    sal_uInt16 nSlot = rReq.GetSlot();
    switch(nSlot)
    {
        case SID_IMAP:
        {
            SfxViewFrame* pThisFrame = GetViewFrame();
            sal_uInt16 nId = ScIMapChildWindowId();
            pThisFrame->ToggleChildWindow( nId );
            GetViewFrame()->GetBindings().Invalidate( SID_IMAP );

            if ( pThisFrame->HasChildWindow( nId ) )
            {
                SvxIMapDlg* pDlg = ScGetIMapDlg();
                if ( pDlg )
                {
                    SdrView* pDrView = GetSdrView();
                    if ( pDrView )
                    {
                        const SdrMarkList& rMarkList = pDrView->GetMarkedObjectList();
                        if ( rMarkList.GetMarkCount() == 1 )
                            UpdateIMap( rMarkList.GetMark( 0 )->GetMarkedSdrObj() );
                    }
                }
            }

            rReq.Ignore();
        }
        break;

        case SID_IMAP_EXEC:
        {
            SdrView* pDrView = GetSdrView();
            SdrMark* pMark = pDrView ? pDrView->GetMarkedObjectList().GetMark(0) : 0;

            if ( pMark )
            {
                SdrObject*  pSdrObj = pMark->GetMarkedSdrObj();
                SvxIMapDlg* pDlg = ScGetIMapDlg();

                if ( ScIMapDlgGetObj(pDlg) == (void*) pSdrObj )
                {
                    const ImageMap& rImageMap = ScIMapDlgGetMap(pDlg);
                    ScIMapInfo*     pIMapInfo = ScDrawLayer::GetIMapInfo( pSdrObj );

                    if ( !pIMapInfo )
                        pSdrObj->AppendUserData( new ScIMapInfo( rImageMap ) );
                    else
                        pIMapInfo->SetImageMap( rImageMap );

                    GetViewData()->GetDocShell()->SetDrawModified();
                }
            }
        }
        break;
    }
}

void ScTabViewShell::GetImageMapState( SfxItemSet& rSet )
{
    SfxWhichIter aIter(rSet);
    sal_uInt16 nWhich = aIter.FirstWhich();
    while ( nWhich )
    {
        switch ( nWhich )
        {
            case SID_IMAP:
                {
                    //  Disabled wird nicht mehr...

                    sal_Bool bThere = false;
                    SfxViewFrame* pThisFrame = GetViewFrame();
                    sal_uInt16 nId = ScIMapChildWindowId();
                    if ( pThisFrame->KnowsChildWindow(nId) )
                        if ( pThisFrame->HasChildWindow(nId) )
                            bThere = sal_True;

                    ObjectSelectionType eType=GetCurObjectSelectionType();
                    sal_Bool bEnable=(eType==OST_OleObject) ||(eType==OST_Graphic);
                    if(!bThere && !bEnable)
                    {
                       rSet.DisableItem( nWhich );
                    }
                    else
                    {
                        rSet.Put( SfxBoolItem( nWhich, bThere ) );
                    }
                }
                break;

            case SID_IMAP_EXEC:
                {
                    sal_Bool bDisable = sal_True;

                    SdrView* pDrView = GetSdrView();
                    if ( pDrView )
                    {
                        const SdrMarkList& rMarkList = pDrView->GetMarkedObjectList();
                        if ( rMarkList.GetMarkCount() == 1 )
                            if ( ScIMapDlgGetObj(ScGetIMapDlg()) ==
                                        (void*) rMarkList.GetMark(0)->GetMarkedSdrObj() )
                                bDisable = false;
                    }

                    rSet.Put( SfxBoolItem( SID_IMAP_EXEC, bDisable ) );
                }
                break;
        }

        nWhich = aIter.NextWhich();
    }
}




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
