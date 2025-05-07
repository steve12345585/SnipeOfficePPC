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




#include <cmdid.h>
#include <swtypes.hxx>
#include <sfx2/basedlgs.hxx>
#include <sfx2/dispatch.hxx>
#include <vcl/msgbox.hxx>
#include <svx/htmlmode.hxx>
#include <viewopt.hxx>
#include <docsh.hxx>
#include <fldwrap.hxx>
#include <wrtsh.hxx>
#include <view.hxx>
#include <swmodule.hxx>

#include <helpid.h>
#include <fldui.hrc>
#include <globals.hrc>
#include <fldtdlg.hrc>
#include "swabstdlg.hxx"

SFX_IMPL_CHILDWINDOW(SwFldDlgWrapper, FN_INSERT_FIELD)

SwChildWinWrapper::SwChildWinWrapper(Window *pParentWindow, sal_uInt16 nId) :
        SfxChildWindow(pParentWindow, nId),
        m_pDocSh(0)
{
    // avoid flickering of buttons:
    m_aUpdateTimer.SetTimeout(200);
    m_aUpdateTimer.SetTimeoutHdl(LINK(this, SwChildWinWrapper, UpdateHdl));
}

IMPL_LINK_NOARG(SwChildWinWrapper, UpdateHdl)
{
    GetWindow()->Activate();    // update dialog

    return 0;
}

/*--------------------------------------------------------------------
    Description: newly initialise dialog after Doc switch
 --------------------------------------------------------------------*/
sal_Bool SwChildWinWrapper::ReInitDlg(SwDocShell *)
{
    sal_Bool bRet = sal_False;

    if (m_pDocSh != GetOldDocShell())
    {
        m_aUpdateTimer.Stop();
        bRet = sal_True;            // immediate Update
    }
    else
        m_aUpdateTimer.Start();

    return bRet;
}

SfxChildWinInfo SwFldDlgWrapper::GetInfo() const
{
    SfxChildWinInfo aInfo = SfxChildWindow::GetInfo();
    aInfo.aPos = GetWindow()->OutputToAbsoluteScreenPixel(aInfo.aPos);
    return aInfo;
}

SwFldDlgWrapper::SwFldDlgWrapper( Window* _pParent, sal_uInt16 nId,
                                    SfxBindings* pB,
                                    SfxChildWinInfo*  )
    : SwChildWinWrapper( _pParent, nId )
{
    SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
    OSL_ENSURE(pFact, "SwAbstractDialogFactory fail!");

    AbstractSwFldDlg* pDlg = pFact->CreateSwFldDlg(pB, this, _pParent, DLG_FLD_INSERT );
    OSL_ENSURE(pDlg, "Dialogdiet fail!");
    pDlgInterface = pDlg;
    pWindow = pDlg->GetWindow();
    pDlg->Start();
    eChildAlignment = SFX_ALIGN_NOALIGNMENT;
}

/*--------------------------------------------------------------------
    Description: newly initialise dialog after Doc switch
 --------------------------------------------------------------------*/
sal_Bool SwFldDlgWrapper::ReInitDlg(SwDocShell *pDocSh)
{
    sal_Bool bRet;

    if ((bRet = SwChildWinWrapper::ReInitDlg(pDocSh)) == sal_True)  // update immediately, Doc switch
    {
        pDlgInterface->ReInitDlg();
    }

    return bRet;
}

void SwFldDlgWrapper::ShowPage(sal_uInt16 nPage)
{
    pDlgInterface->ShowPage(nPage ? nPage : TP_FLD_REF);
}

SFX_IMPL_CHILDWINDOW(SwFldDataOnlyDlgWrapper, FN_INSERT_FIELD_DATA_ONLY)

SfxChildWinInfo SwFldDataOnlyDlgWrapper::GetInfo() const
{
    SfxChildWinInfo aInfo = SfxChildWindow::GetInfo();
// prevent instatiation of dialog other than by calling
// the mail merge dialog
    aInfo.bVisible = sal_False;
    return aInfo;
}

SwFldDataOnlyDlgWrapper::SwFldDataOnlyDlgWrapper( Window* _pParent, sal_uInt16 nId,
                                    SfxBindings* pB,
                                    SfxChildWinInfo* pInfo )
    : SwChildWinWrapper( _pParent, nId )
{
    SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
    OSL_ENSURE(pFact, "SwAbstractDialogFactory fail!");

    AbstractSwFldDlg* pDlg = pFact->CreateSwFldDlg(pB, this, _pParent, DLG_FLD_INSERT );
    OSL_ENSURE(pDlg, "Dialogdiet fail!");
    pDlgInterface = pDlg;

    pWindow = pDlg->GetWindow();
    pDlg->ActivateDatabasePage();
    pDlg->Start();
    pDlg->Initialize( pInfo );
    eChildAlignment = SFX_ALIGN_NOALIGNMENT;
}

/* --------------------------------------------------
 * re-init after doc activation
 * --------------------------------------------------*/
sal_Bool SwFldDataOnlyDlgWrapper::ReInitDlg(SwDocShell *pDocSh)
{
    sal_Bool bRet;
    if ((bRet = SwChildWinWrapper::ReInitDlg(pDocSh)) == sal_True)  // update immediately, Doc switch
    {
        pDlgInterface->ReInitDlg();
    }

    return bRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
