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

#include "fuarea.hxx"

#include <svx/svxids.hrc>
#include <vcl/msgbox.hxx>
#include <svl/intitem.hxx>
#include <svl/stritem.hxx>
#include <sfx2/request.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/bindings.hxx>
#include "ViewShell.hxx"

#include "drawdoc.hxx"
#include "View.hxx"
#include "Window.hxx"
#include "app.hrc"
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>

namespace sd {
TYPEINIT1( FuArea, FuPoor );

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuArea::FuArea( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* _pView, SdDrawDocument* pDoc, SfxRequest& rReq)
: FuPoor(pViewSh, pWin, _pView, pDoc, rReq)
{
}

FunctionReference FuArea::Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* _pView, SdDrawDocument* pDoc, SfxRequest& rReq )
{
    FunctionReference xFunc( new FuArea( pViewSh, pWin, _pView, pDoc, rReq ) );
    xFunc->DoExecute(rReq);
    return xFunc;
}

void FuArea::DoExecute( SfxRequest& rReq )
{
    const SfxItemSet* pArgs = rReq.GetArgs();

    if( !pArgs )
    {
        SfxItemSet aNewAttr( mpDoc->GetPool() );
        mpView->GetAttributes( aNewAttr );

        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        AbstractSvxAreaTabDialog * pDlg = pFact ? pFact->CreateSvxAreaTabDialog( NULL,
                                                                        &aNewAttr,
                                                                        mpDoc,
                                                                        mpView) : 0;
        if( pDlg && (pDlg->Execute() == RET_OK) )
        {
            mpView->SetAttributes (*(pDlg->GetOutputItemSet ()));
        }

        // Attribute wurden geaendert, Listboxes in Objectbars muessen aktualisiert werden
        static sal_uInt16 SidArray[] = {
                        SID_ATTR_FILL_STYLE,
                        SID_ATTR_FILL_COLOR,
                        SID_ATTR_FILL_GRADIENT,
                        SID_ATTR_FILL_HATCH,
                        SID_ATTR_FILL_BITMAP,
                        0 };

        mpViewShell->GetViewFrame()->GetBindings().Invalidate( SidArray );

        delete pDlg;
    }

    rReq.Ignore ();

}

void FuArea::Activate()
{
}

void FuArea::Deactivate()
{
}

} // end of namespace sd

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
