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

#include <redline.hxx>
#include <vcl/msgbox.hxx>
#include <svl/eitem.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/ctredlin.hxx>
#include <svx/postattr.hxx>
#include <swtypes.hxx>
#include <wrtsh.hxx>
#include <view.hxx>
#include <swmodule.hxx>
#include <swwait.hxx>
#include <uitool.hxx>

#include <helpid.h>
#include <cmdid.h>
#include <misc.hrc>
#include <redlndlg.hrc>
#include <shells.hrc>

#include <vector>
#include <redlndlg.hxx>
#include "swmodalredlineacceptdlg.hxx"

#include <unomid.h>

SwModalRedlineAcceptDlg::SwModalRedlineAcceptDlg(Window *pParent) :
    SfxModalDialog(pParent, SW_RES(DLG_MOD_REDLINE_ACCEPT))
{
    pImplDlg = new SwRedlineAcceptDlg(this, sal_True);

    pImplDlg->Initialize(GetExtraData());
    pImplDlg->Activate();   // for data's initialisation

    FreeResource();
}

SwModalRedlineAcceptDlg::~SwModalRedlineAcceptDlg()
{
    AcceptAll(sal_False);   // refuse everything remaining
    pImplDlg->FillInfo(GetExtraData());

    delete pImplDlg;
}

void SwModalRedlineAcceptDlg::Activate()
{
}

void SwModalRedlineAcceptDlg::Resize()
{
    pImplDlg->Resize();
    SfxModalDialog::Resize();
}

void SwModalRedlineAcceptDlg::AcceptAll( sal_Bool bAccept )
{
    SvxTPFilter* pFilterTP = pImplDlg->GetChgCtrl()->GetFilterPage();

    if (pFilterTP->IsDate() || pFilterTP->IsAuthor() ||
        pFilterTP->IsRange() || pFilterTP->IsAction())
    {
        pFilterTP->CheckDate(sal_False);    // turn off all filters
        pFilterTP->CheckAuthor(sal_False);
        pFilterTP->CheckRange(sal_False);
        pFilterTP->CheckAction(sal_False);
        pImplDlg->FilterChangedHdl();
    }

    pImplDlg->CallAcceptReject( sal_False, bAccept );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
