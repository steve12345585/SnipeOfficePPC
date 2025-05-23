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

#include "hintids.hxx"

#include <svx/htmlmode.hxx>
#include <svx/svxids.hrc>
#include <sfx2/app.hxx>
#include <vcl/msgbox.hxx>

#include "swtypes.hxx"
#include "wrtsh.hxx"
#include "viewopt.hxx"
#include "macassgn.hxx"
#include "swevent.hxx"
#include "docsh.hxx"
#include "globals.hrc"
#include "view.hxx"
#include <sfx2/viewfrm.hxx>

#include <svx/svxdlg.hxx>

#include <doc.hxx>

using ::com::sun::star::uno::Reference;
using ::com::sun::star::frame::XFrame;

SfxEventNamesItem SwMacroAssignDlg::AddEvents( DlgEventType eType )
{
    SfxEventNamesItem aItem(SID_EVENTCONFIG);

    sal_Bool bHtmlMode = sal_False;
    sal_uInt16 nHtmlMode = ::GetHtmlMode((const SwDocShell*)SfxObjectShell::Current());
    bHtmlMode = nHtmlMode & HTMLMODE_ON ? sal_True : sal_False;

    switch( eType )
    {
    case MACASSGN_AUTOTEXT:
        aItem.AddEvent( String( SW_RES(STR_EVENT_START_INS_GLOSSARY) ), String(),
                            SW_EVENT_START_INS_GLOSSARY );
        aItem.AddEvent( String( SW_RES(STR_EVENT_END_INS_GLOSSARY) ), String(),
                            SW_EVENT_END_INS_GLOSSARY);
        // in order for the new handler to become active!
        break;
    case MACASSGN_ALLFRM:
    case MACASSGN_GRAPHIC:          // graphics
        {
            aItem.AddEvent( String( SW_RES(STR_EVENT_IMAGE_ERROR) ), String(),
                                SVX_EVENT_IMAGE_ERROR);
            aItem.AddEvent( String( SW_RES(STR_EVENT_IMAGE_ABORT) ), String(),
                                SVX_EVENT_IMAGE_ABORT);
            aItem.AddEvent( String( SW_RES(STR_EVENT_IMAGE_LOAD) ), String(),
                                SVX_EVENT_IMAGE_LOAD);
        }
        // no break;
    case MACASSGN_FRMURL:           // Frm - URL-Attributes
        {
            if( !bHtmlMode &&
                (MACASSGN_FRMURL == eType || MACASSGN_ALLFRM == eType))
            {
                aItem.AddEvent( String( SW_RES( STR_EVENT_FRM_KEYINPUT_A ) ), String(),
                                SW_EVENT_FRM_KEYINPUT_ALPHA );
                aItem.AddEvent( String( SW_RES( STR_EVENT_FRM_KEYINPUT_NOA ) ), String(),
                                SW_EVENT_FRM_KEYINPUT_NOALPHA );
                aItem.AddEvent( String( SW_RES( STR_EVENT_FRM_RESIZE ) ), String(),
                                SW_EVENT_FRM_RESIZE );
                aItem.AddEvent( String( SW_RES( STR_EVENT_FRM_MOVE ) ), String(),
                                SW_EVENT_FRM_MOVE );
            }
        }
        // no break;
    case MACASSGN_OLE:              // OLE
        {
            if( !bHtmlMode )
                aItem.AddEvent( String( SW_RES(STR_EVENT_OBJECT_SELECT) ), String(),
                                SW_EVENT_OBJECT_SELECT );
        }
        // no break;
    case MACASSGN_INETFMT:          // INetFmt-Attributes
        {
            aItem.AddEvent( String( SW_RES(STR_EVENT_MOUSEOVER_OBJECT) ), String(),
                                SFX_EVENT_MOUSEOVER_OBJECT );
            aItem.AddEvent( String( SW_RES(STR_EVENT_MOUSECLICK_OBJECT) ), String(),
                                SFX_EVENT_MOUSECLICK_OBJECT);
            aItem.AddEvent( String( SW_RES(STR_EVENT_MOUSEOUT_OBJECT) ), String(),
                                SFX_EVENT_MOUSEOUT_OBJECT);
        }
        break;
    }

    return aItem;
}


sal_Bool SwMacroAssignDlg::INetFmtDlg( Window* pParent, SwWrtShell& rSh,
                                    SvxMacroItem*& rpINetItem )
{
    sal_Bool bRet = sal_False;
    SfxItemSet aSet( rSh.GetAttrPool(), RES_FRMMACRO, RES_FRMMACRO, SID_EVENTCONFIG, SID_EVENTCONFIG, 0 );
    SvxMacroItem aItem( RES_FRMMACRO );
    if( !rpINetItem )
        rpINetItem = new SvxMacroItem( RES_FRMMACRO );
    else
        aItem.SetMacroTable( rpINetItem->GetMacroTable() );

    aSet.Put( aItem );
    aSet.Put( AddEvents( MACASSGN_INETFMT ) );

    SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
    SfxAbstractDialog* pMacroDlg = pFact->CreateSfxDialog( pParent, aSet,
        rSh.GetView().GetViewFrame()->GetFrame().GetFrameInterface(),
        SID_EVENTCONFIG );
    if ( pMacroDlg && pMacroDlg->Execute() == RET_OK )
    {
        const SfxItemSet* pOutSet = pMacroDlg->GetOutputItemSet();
        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET == pOutSet->GetItemState( RES_FRMMACRO, sal_False, &pItem ))
        {
            rpINetItem->SetMacroTable( ((SvxMacroItem*)pItem)->GetMacroTable() );
            bRet = sal_True;
        }
    }
    return bRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
