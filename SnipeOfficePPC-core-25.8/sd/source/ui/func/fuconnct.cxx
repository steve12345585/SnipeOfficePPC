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




#include "fuconnct.hxx"
#include <sfx2/request.hxx>
#include <vcl/msgbox.hxx>
#include "View.hxx"
#include "ViewShell.hxx"
#include "drawdoc.hxx"
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>

namespace sd {

TYPEINIT1( FuConnectionDlg, FuPoor );

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuConnectionDlg::FuConnectionDlg (
    ViewShell* pViewSh,
    ::sd::Window* pWin,
    ::sd::View* pView,
    SdDrawDocument* pDoc,
    SfxRequest& rReq)
    : FuPoor(pViewSh, pWin, pView, pDoc, rReq)
{
}

FunctionReference FuConnectionDlg::Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq )
{
    FunctionReference xFunc( new FuConnectionDlg( pViewSh, pWin, pView, pDoc, rReq ) );
    xFunc->DoExecute(rReq);
    return xFunc;
}

void FuConnectionDlg::DoExecute( SfxRequest& rReq )
{
    SfxItemSet aNewAttr( mpDoc->GetPool() );
    mpView->GetAttributes( aNewAttr );

    const SfxItemSet* pArgs = rReq.GetArgs();

    if( !pArgs )
    {
        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        ::std::auto_ptr<SfxAbstractDialog> pDlg( pFact ? pFact->CreateSfxDialog( NULL, aNewAttr, mpView, RID_SVXPAGE_CONNECTION) : 0);

        if( pDlg.get() && (pDlg->Execute() == RET_OK) )
        {
            rReq.Done( *pDlg->GetOutputItemSet() );
            pArgs = rReq.GetArgs();
        }
    }
    if( pArgs )
        mpView->SetAttributes( *pArgs );
}

} // end of namespace sd

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
