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

#include <vcl/msgbox.hxx>
#include <unotools/localedatawrapper.hxx>

#include "redcom.hxx"
#include "docsh.hxx"
#include "tabvwsh.hxx"
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>
//------------------------------------------------------------------------

ScRedComDialog::ScRedComDialog( Window* pParent, const SfxItemSet& rCoreSet,
                    ScDocShell *pShell,ScChangeAction *pAction,sal_Bool bPrevNext)
{
    SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
    if(pFact)
    {
        pDlg = pFact->CreateSvxPostItDialog( pParent, rCoreSet, bPrevNext, true );
        OSL_ENSURE(pDlg, "Dialog creation failed!");
        pDocShell=pShell;
        pDlg->DontChangeAuthor();
        pDlg->HideAuthor();

        pDlg->SetPrevHdl(LINK( this, ScRedComDialog, PrevHdl));
        pDlg->SetNextHdl(LINK( this, ScRedComDialog, NextHdl));

        ReInit(pAction);
    }
}

ScRedComDialog::~ScRedComDialog()
{
    delete pDlg;
}

ScChangeAction *ScRedComDialog::FindPrev(ScChangeAction *pAction)
{
    if(pAction!=NULL && pDocShell !=NULL)
    {
        ScDocument* pDoc = pDocShell->GetDocument();
        ScChangeViewSettings* pSettings = pDoc->GetChangeViewSettings();

        pAction=pAction->GetPrev();

        while(pAction!=NULL)
        {
            if( pAction->GetState()==SC_CAS_VIRGIN &&
                pAction->IsDialogRoot() &&
                ScViewUtil::IsActionShown(*pAction,*pSettings,*pDoc)) break;

            pAction=pAction->GetPrev();
        }
    }
    return pAction;
}

ScChangeAction *ScRedComDialog::FindNext(ScChangeAction *pAction)
{
    if(pAction!=NULL && pDocShell !=NULL)
    {
        ScDocument* pDoc = pDocShell->GetDocument();
        ScChangeViewSettings* pSettings = pDoc->GetChangeViewSettings();

        pAction=pAction->GetNext();

        while(pAction!=NULL)
        {
            if( pAction->GetState()==SC_CAS_VIRGIN &&
                pAction->IsDialogRoot() &&
                ScViewUtil::IsActionShown(*pAction,*pSettings,*pDoc)) break;

            pAction=pAction->GetNext();
        }
    }
    return pAction;
}

void ScRedComDialog::ReInit(ScChangeAction *pAction)
{
    pChangeAction=pAction;
    if(pChangeAction!=NULL && pDocShell !=NULL)
    {
        rtl::OUString aTitle;
        pChangeAction->GetDescription( aTitle, pDocShell->GetDocument());
        pDlg->SetText(aTitle);
        aComment=pChangeAction->GetComment();

        sal_Bool bNext=FindNext(pChangeAction)!=NULL;
        sal_Bool bPrev=FindPrev(pChangeAction)!=NULL;
        pDlg->EnableTravel(bNext,bPrev);

        String aAuthor = pChangeAction->GetUser();

        DateTime aDT = pChangeAction->GetDateTime();
        String aDate = ScGlobal::pLocaleData->getDate( aDT );
        aDate += ' ';
        aDate += ScGlobal::pLocaleData->getTime( aDT, false, false );

        pDlg->ShowLastAuthor(aAuthor, aDate);
        pDlg->SetNote(aComment);
    }
}

short ScRedComDialog::Execute()
{
    short nRet=pDlg->Execute();

    if(nRet== RET_OK )
    {
        if ( pDocShell!=NULL && pDlg->GetNote() != aComment )
            pDocShell->SetChangeComment( pChangeAction, pDlg->GetNote());
    }

    return nRet;
}

void ScRedComDialog::SelectCell()
{
    if(pChangeAction!=NULL)
    {
        const ScChangeAction* pAction=pChangeAction;
        const ScBigRange& rRange = pAction->GetBigRange();

        if(rRange.IsValid(pDocShell->GetDocument()))
        {
            ScViewData* pViewData=pDocShell->GetViewData();
            ScRange aRef=rRange.MakeRange();
            ScTabView* pTabView=pViewData->GetView();
            pTabView->MarkRange(aRef);
        }
    }
}

IMPL_LINK(ScRedComDialog, PrevHdl, AbstractSvxPostItDialog*, pDlgP )
{
    if (pDocShell!=NULL && pDlgP->GetNote() != aComment )
        pDocShell->SetChangeComment( pChangeAction, pDlgP->GetNote());

    ReInit(FindPrev(pChangeAction));
    SelectCell();

    return 0;
}

IMPL_LINK(ScRedComDialog, NextHdl, AbstractSvxPostItDialog*, pDlgP )
{
    if ( pDocShell!=NULL && pDlgP->GetNote() != aComment )
        pDocShell->SetChangeComment( pChangeAction, pDlgP->GetNote());

    ReInit(FindNext(pChangeAction));
    SelectCell();

    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
