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
#ifndef _SWANNOTSH_HXX
#define _SWANNOTSH_HXX

#include <sfx2/shell.hxx>
#include "shellid.hxx"
#include "swmodule.hxx"
#include <unotools/caserotate.hxx>

class SwView;
class SwAnnotationShell: public SfxShell
{
    SwView&     rView;
    RotateTransliteration m_aRotateCase;

public:
    SFX_DECL_INTERFACE(SW_ANNOTATIONSHELL)
    TYPEINFO();

                SwAnnotationShell(SwView&);
    virtual     ~SwAnnotationShell();

    void        StateDisableItems(SfxItemSet &);
    void        Exec(SfxRequest &);

    void        GetState(SfxItemSet &);
    void        StateInsert(SfxItemSet &rSet);

    void        NoteExec(SfxRequest &);
    void        GetNoteState(SfxItemSet &);

    void        ExecLingu(SfxRequest &rReq);
    void        GetLinguState(SfxItemSet &);

    void        ExecClpbrd(SfxRequest &rReq);
    void        StateClpbrd(SfxItemSet &rSet);

    void        ExecTransliteration(SfxRequest &);
    void                ExecRotateTransliteration(SfxRequest &);

    void        ExecUndo(SfxRequest &rReq);
    void        StateUndo(SfxItemSet &rSet);

    void        StateStatusLine(SfxItemSet &rSet);

    void        InsertSymbol(SfxRequest& rReq);

    void        ExecSearch(SfxRequest&, sal_Bool bNoMessage = sal_False);
    void            StateSearch(SfxItemSet &);


    virtual ::svl::IUndoManager*
                GetUndoManager();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
