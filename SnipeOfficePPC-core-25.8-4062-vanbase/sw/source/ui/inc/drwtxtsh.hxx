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
#ifndef _SWDRWTXTSH_HXX
#define _SWDRWTXTSH_HXX

#include <sfx2/shell.hxx>
#include "shellid.hxx"
#include <unotools/caserotate.hxx>

class SdrView;
class SwView;
class SwWrtShell;

class SwDrawTextShell: public SfxShell
{
    SwView      &rView;
    RotateTransliteration m_aRotateCase;

    SdrView     *pSdrView;

    sal_Bool        bRotate : 1;
    sal_Bool        bSelMove: 1;

    void        SetAttrToMarked(const SfxItemSet& rAttr);
    void        InsertSymbol(SfxRequest& rReq);
    sal_Bool        IsTextEdit();
public:
    SFX_DECL_INTERFACE(SW_DRWTXTSHELL)
    TYPEINFO();

    SwView     &GetView() { return rView; }
    SwWrtShell &GetShell();

                SwDrawTextShell(SwView &rView);
    virtual     ~SwDrawTextShell();

    virtual ::svl::IUndoManager*
                GetUndoManager();

    void        StateDisableItems(SfxItemSet &);

    void        Execute(SfxRequest &);
    void        ExecDraw(SfxRequest &);
    void        GetState(SfxItemSet &);
    void        GetDrawTxtCtrlState(SfxItemSet&);

    void        ExecFontWork(SfxRequest& rReq);
    void        StateFontWork(SfxItemSet& rSet);
    void        ExecFormText(SfxRequest& rReq);
    void        GetFormTextState(SfxItemSet& rSet);
    void        ExecDrawLingu(SfxRequest &rReq);
    void        ExecUndo(SfxRequest &rReq);
    void        StateUndo(SfxItemSet &rSet);
    void        ExecClpbrd(SfxRequest &rReq);
    void        StateClpbrd(SfxItemSet &rSet);
    void        StateInsert(SfxItemSet &rSet);
    void        ExecTransliteration(SfxRequest &);
    void                ExecRotateTransliteration(SfxRequest &);

    void        Init();
    void        StateStatusline(SfxItemSet &rSet);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
