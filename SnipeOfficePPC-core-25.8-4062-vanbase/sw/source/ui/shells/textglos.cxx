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

#include <sfx2/request.hxx>
#include <svl/eitem.hxx>
#include <svl/stritem.hxx>

#include "view.hxx"
#include "cmdid.h"
#include "textsh.hxx"
#include "initui.hxx"
#include "gloshdl.hxx"
#include "glosdoc.hxx"
#include "gloslst.hxx"
#include "swabstdlg.hxx"
#include <misc.hrc>

// STATIC DATA -----------------------------------------------------------
void SwTextShell::ExecGlossary(SfxRequest &rReq)
{
    sal_uInt16 nSlot = rReq.GetSlot();
    ::GetGlossaries()->UpdateGlosPath(!rReq.IsAPI() ||
                                        FN_GLOSSARY_DLG == nSlot );
    SwGlossaryHdl* pGlosHdl = GetView().GetGlosHdl();
    // SwGlossaryList updaten?
    sal_Bool bUpdateList = sal_False;

    const SfxItemSet *pArgs = rReq.GetArgs();
    const SfxPoolItem* pItem = 0;
    if(pArgs)
       pArgs->GetItemState(nSlot, sal_False, &pItem );

    switch( nSlot )
    {
        case FN_GLOSSARY_DLG:
            pGlosHdl->GlossaryDlg();
            bUpdateList = sal_True;
            rReq.Ignore();
            break;
        case FN_EXPAND_GLOSSARY:
        {
            sal_Bool bReturn;
            bReturn = pGlosHdl->ExpandGlossary();
            rReq.SetReturnValue( SfxBoolItem( nSlot, bReturn ) );
            rReq.Done();
        }
        break;
        case FN_NEW_GLOSSARY:
            if(pItem && pArgs->Count() == 3 )
            {
                String aGroup = (( const SfxStringItem *)pItem)->GetValue();
                String aName;
                if(SFX_ITEM_SET ==  pArgs->GetItemState(FN_PARAM_1, sal_False, &pItem ))
                    aName = (( const SfxStringItem *)pItem)->GetValue();
                String aShortName;
                if(SFX_ITEM_SET ==  pArgs->GetItemState(FN_PARAM_2, sal_False, &pItem ))
                    aShortName = (( const SfxStringItem *)pItem)->GetValue();

                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                OSL_ENSURE(pFact, "Dialogdiet fail!");
                ::GlossarySetActGroup fnSetActGroup = pFact->SetGlossaryActGroupFunc( DLG_RENAME_GLOS );
                if ( fnSetActGroup )
                    (*fnSetActGroup)( aGroup );
                pGlosHdl->SetCurGroup(aGroup, sal_True);
                //eingestellte Gruppe muss in NewGlossary ggf. erzeugt werden!
                pGlosHdl->NewGlossary( aName, aShortName, sal_True );
                rReq.Done();
            }
            bUpdateList = sal_True;
        break;
        case FN_SET_ACT_GLOSSARY:
            if(pItem)
            {
                String aGroup = (( const SfxStringItem *)pItem)->GetValue();
                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                OSL_ENSURE(pFact, "Dialogdiet fail!");
                ::GlossarySetActGroup fnSetActGroup = pFact->SetGlossaryActGroupFunc( DLG_RENAME_GLOS );
                if ( fnSetActGroup )
                    (*fnSetActGroup)( aGroup );
                rReq.Done();
            }
        break;
        case FN_INSERT_GLOSSARY:
        {
            if(pItem && pArgs->Count() > 1)
            {
                String aGroup = (( const SfxStringItem *)pItem)->GetValue();
                String aName;
                if(SFX_ITEM_SET ==  pArgs->GetItemState(FN_PARAM_1, sal_False, &pItem ))
                    aName = (( const SfxStringItem *)pItem)->GetValue();
                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                OSL_ENSURE(pFact, "Dialogdiet fail!");
                ::GlossarySetActGroup fnSetActGroup = pFact->SetGlossaryActGroupFunc( DLG_RENAME_GLOS );
                if ( fnSetActGroup )
                    (*fnSetActGroup)( aGroup );
                pGlosHdl->SetCurGroup(aGroup, sal_True);
                rReq.SetReturnValue(SfxBoolItem(nSlot, pGlosHdl->InsertGlossary( aName ) ? sal_True : sal_False));
                rReq.Done();
            }
        }
        break;
        default:
            OSL_FAIL("wrong dispatcher");
            return;
    }
    if(bUpdateList)
    {
        SwGlossaryList* pList = ::GetGlossaryList();
        if(pList->IsActive())
            pList->Update();
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
