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

#include <cmdid.h>
#include <hintids.hxx>
#include <vcl/msgbox.hxx>
#include <svl/stritem.hxx>
#include <svl/whiter.hxx>
#include <svl/urihelper.hxx>
#include <sfx2/dispatch.hxx>
#include <editeng/sizeitem.hxx>
#include <editeng/protitem.hxx>
#include <sfx2/request.hxx>
#include <svl/srchitem.hxx>
#include <svx/htmlmode.hxx>
#include <svx/sdgluitm.hxx>
#include <svx/sdgcoitm.hxx>
#include <svx/sdggaitm.hxx>
#include <svx/sdgtritm.hxx>
#include <svx/sdginitm.hxx>
#include <svx/sdgmoitm.hxx>
#include <editeng/brshitem.hxx>
#include <svx/grfflt.hxx>
#include <fmturl.hxx>
#include <view.hxx>
#include <wrtsh.hxx>
#include <viewopt.hxx>
#include <swmodule.hxx>
#include <frmatr.hxx>
#include <swundo.hxx>
#include <uitool.hxx>
#include <docsh.hxx>
#include <mediash.hxx>
#include <frmmgr.hxx>
#include <frmdlg.hxx>
#include <frmfmt.hxx>
#include <grfatr.hxx>
#include <usrpref.hxx>
#include <edtwin.hxx>
#include <swwait.hxx>
#include <shells.hrc>
#include <popup.hrc>

#include <sfx2/objface.hxx>
#include <svx/svdomedia.hxx>
#include <svx/sdr/contact/viewcontactofsdrmediaobj.hxx>
#include <avmedia/mediaitem.hxx>

#define SwMediaShell
#include <sfx2/msg.hxx>
#include "swslots.hxx"
#include "swabstdlg.hxx"

SFX_IMPL_INTERFACE(SwMediaShell, SwBaseShell, SW_RES(STR_SHELLNAME_MEDIA))
{
    SFX_POPUPMENU_REGISTRATION(SW_RES(MN_MEDIA_POPUPMENU));
    SFX_OBJECTBAR_REGISTRATION(SFX_OBJECTBAR_OBJECT, SW_RES(RID_MEDIA_TOOLBOX));
}

void SwMediaShell::ExecMedia(SfxRequest &rReq)
{
    SwWrtShell* pSh = &GetShell();
    SdrView*    pSdrView = pSh->GetDrawView();

    if( pSdrView )
    {
        const SfxItemSet*   pArgs = rReq.GetArgs();
        sal_uInt16              nSlotId = rReq.GetSlot();
        sal_Bool                bChanged = pSdrView->GetModel()->IsChanged();

        pSdrView->GetModel()->SetChanged( sal_False );

        switch( nSlotId )
        {
            case SID_DELETE:
            {
                if( pSh->IsObjSelected() )
                {
                    pSh->SetModified();
                    pSh->DelSelectedObj();

                    if( pSh->IsSelFrmMode() )
                        pSh->LeaveSelFrmMode();

                    GetView().AttrChangedNotify( pSh );
                }
            }
            break;

            case( SID_AVMEDIA_TOOLBOX ):
            {
                if( pSh->IsObjSelected() )
                {
                    const SfxPoolItem*  pItem;

                    if( !pArgs || ( SFX_ITEM_SET != pArgs->GetItemState( SID_AVMEDIA_TOOLBOX, sal_False, &pItem ) ) )
                        pItem = NULL;

                    if( pItem )
                    {
                        SdrMarkList* pMarkList = new SdrMarkList( pSdrView->GetMarkedObjectList() );

                        if( 1 == pMarkList->GetMarkCount() )
                        {
                            SdrObject* pObj = pMarkList->GetMark( 0 )->GetMarkedSdrObj();

                            if( pObj && pObj->ISA( SdrMediaObj ) )
                            {
                                static_cast< sdr::contact::ViewContactOfSdrMediaObj& >( pObj->GetViewContact() ).executeMediaItem(
                                    static_cast< const ::avmedia::MediaItem& >( *pItem ) );
                            }
                        }

                        delete pMarkList;
                    }
                }
            }
            break;

            default:
            break;
        }

        if( pSdrView->GetModel()->IsChanged() )
            GetShell().SetModified();
        else if( bChanged )
            pSdrView->GetModel()->SetChanged(sal_True);
    }
}

void SwMediaShell::GetMediaState(SfxItemSet &rSet)
{
    SfxWhichIter    aIter( rSet );
    sal_uInt16          nWhich = aIter.FirstWhich();

    while( nWhich )
    {
        if( SID_AVMEDIA_TOOLBOX == nWhich )
        {
            SwWrtShell& rSh = GetShell();
            SdrView*    pView = rSh.GetDrawView();

            if( pView )
            {
                bool bDisable = true;
                SdrMarkList* pMarkList = new SdrMarkList( pView->GetMarkedObjectList() );

                if( 1 == pMarkList->GetMarkCount() )
                {
                    SdrObject* pObj = pMarkList->GetMark( 0 )->GetMarkedSdrObj();

                    if( pObj && pObj->ISA( SdrMediaObj ) )
                    {
                        ::avmedia::MediaItem aItem( SID_AVMEDIA_TOOLBOX );

                        static_cast< sdr::contact::ViewContactOfSdrMediaObj& >( pObj->GetViewContact() ).updateMediaItem( aItem );
                        rSet.Put( aItem );
                        bDisable = false;
                    }
                }

                if( bDisable )
                    rSet.DisableItem( SID_AVMEDIA_TOOLBOX );

                delete pMarkList;
            }
        }

        nWhich = aIter.NextWhich();
    }
}

SwMediaShell::SwMediaShell(SwView &_rView) :
    SwBaseShell(_rView)

{
    SetName(rtl::OUString("Media Playback"));
    SetHelpId(SW_MEDIASHELL);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
