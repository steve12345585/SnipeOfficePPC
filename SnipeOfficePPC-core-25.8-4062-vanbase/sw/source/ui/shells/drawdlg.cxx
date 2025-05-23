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

#include <svx/svxids.hrc>
#include <vcl/msgbox.hxx>
#include <sfx2/request.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/svdview.hxx>
#include <svx/drawitem.hxx>

#include <svx/xtable.hxx>
#include "view.hxx"
#include "wrtsh.hxx"
#include "docsh.hxx"
#include "cmdid.h"

#include "drawsh.hxx"
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>

void SwDrawShell::ExecDrawDlg(SfxRequest& rReq)
{
    SwWrtShell*     pSh     = &GetShell();
    SdrView*        pView   = pSh->GetDrawView();
    SdrModel*       pDoc    = pView->GetModel();
    sal_Bool            bChanged = pDoc->IsChanged();
    pDoc->SetChanged(sal_False);

    SfxItemSet aNewAttr( pDoc->GetItemPool() );
    pView->GetAttributes( aNewAttr );

    GetView().NoRotate();

    switch (rReq.GetSlot())
    {
        case FN_DRAWTEXT_ATTR_DLG:
        {
            SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
            if ( pFact )
            {
                SfxAbstractTabDialog *pDlg = pFact->CreateTextTabDialog( NULL, &aNewAttr, pView );
                sal_uInt16 nResult = pDlg->Execute();

                if (nResult == RET_OK)
                {
                    if (pView->AreObjectsMarked())
                    {
                        pSh->StartAction();
                        pView->SetAttributes(*pDlg->GetOutputItemSet());
                        rReq.Done(*(pDlg->GetOutputItemSet()));
                        pSh->EndAction();
                    }
                }

                delete( pDlg );
            }
        }
        break;

        case SID_ATTRIBUTES_AREA:
        {
            sal_Bool bHasMarked = pView->AreObjectsMarked();

            SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
            AbstractSvxAreaTabDialog * pDlg = pFact->CreateSvxAreaTabDialog( NULL,
                                                                            &aNewAttr,
                                                                            pDoc,
                                                                            pView);
            if (pDlg->Execute() == RET_OK)
            {
                pSh->StartAction();
                if (bHasMarked)
                    pView->SetAttributes(*pDlg->GetOutputItemSet());
                else
                    pView->SetDefaultAttr(*pDlg->GetOutputItemSet(), sal_False);
                pSh->EndAction();

                static sal_uInt16 aInval[] =
                {
                    SID_ATTR_FILL_STYLE, SID_ATTR_FILL_COLOR, 0
                };
                SfxBindings &rBnd = GetView().GetViewFrame()->GetBindings();
                rBnd.Invalidate(aInval);
                rBnd.Update(SID_ATTR_FILL_STYLE);
                rBnd.Update(SID_ATTR_FILL_COLOR);
            }
            delete pDlg;
        }
        break;

        case SID_ATTRIBUTES_LINE:
        {
            sal_Bool bHasMarked = pView->AreObjectsMarked();

            const SdrObject* pObj = NULL;
            const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
            if( rMarkList.GetMarkCount() == 1 )
                pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();

            SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
            OSL_ENSURE(pFact, "Dialogdiet Factory fail!");
            SfxAbstractTabDialog * pDlg = pFact->CreateSvxLineTabDialog( NULL,
                    &aNewAttr,
                pDoc,
                pObj,
                bHasMarked);
            OSL_ENSURE(pDlg, "Dialogdiet fail!");
            if (pDlg->Execute() == RET_OK)
            {
                pSh->StartAction();
                if(bHasMarked)
                    pView->SetAttrToMarked(*pDlg->GetOutputItemSet(), sal_False);
                else
                    pView->SetDefaultAttr(*pDlg->GetOutputItemSet(), sal_False);
                pSh->EndAction();

                static sal_uInt16 aInval[] =
                {
                    SID_ATTR_LINE_STYLE, SID_ATTR_LINE_WIDTH,
                    SID_ATTR_LINE_COLOR, 0
                };

                GetView().GetViewFrame()->GetBindings().Invalidate(aInval);
            }
            delete pDlg;
        }
        break;

        default:
            break;
    }


    if (pDoc->IsChanged())
        GetShell().SetModified();
    else
        if (bChanged)
            pDoc->SetChanged(sal_True);
}

void SwDrawShell::ExecDrawAttrArgs(SfxRequest& rReq)
{
    SwWrtShell* pSh   = &GetShell();
    SdrView*    pView = pSh->GetDrawView();
    const SfxItemSet* pArgs = rReq.GetArgs();
    sal_Bool        bChanged = pView->GetModel()->IsChanged();
    pView->GetModel()->SetChanged(sal_False);

    GetView().NoRotate();

    if (pArgs)
    {
        if(pView->AreObjectsMarked())
            pView->SetAttrToMarked(*rReq.GetArgs(), sal_False);
        else
            pView->SetDefaultAttr(*rReq.GetArgs(), sal_False);
    }
    else
    {
        SfxDispatcher* pDis = pSh->GetView().GetViewFrame()->GetDispatcher();
        switch (rReq.GetSlot())
        {
            case SID_ATTR_FILL_STYLE:
            case SID_ATTR_FILL_COLOR:
            case SID_ATTR_FILL_GRADIENT:
            case SID_ATTR_FILL_HATCH:
            case SID_ATTR_FILL_BITMAP:
                pDis->Execute(SID_ATTRIBUTES_AREA, sal_False);
                break;
            case SID_ATTR_LINE_STYLE:
            case SID_ATTR_LINE_DASH:
            case SID_ATTR_LINE_WIDTH:
            case SID_ATTR_LINE_COLOR:
                pDis->Execute(SID_ATTRIBUTES_LINE, sal_False);
                break;
        }
    }
    if (pView->GetModel()->IsChanged())
        GetShell().SetModified();
    else
        if (bChanged)
            pView->GetModel()->SetChanged(sal_True);
}

void SwDrawShell::GetDrawAttrState(SfxItemSet& rSet)
{
    SdrView* pSdrView = GetShell().GetDrawView();

    if (pSdrView->AreObjectsMarked())
    {
        sal_Bool bDisable = Disable( rSet );

        if( !bDisable )
            pSdrView->GetAttributes( rSet );
    }
    else
        rSet.Put(pSdrView->GetDefaultAttr());
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
