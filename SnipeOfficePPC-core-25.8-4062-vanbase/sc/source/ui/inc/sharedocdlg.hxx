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

#ifndef SC_SHAREDOCDLG_HXX
#define SC_SHAREDOCDLG_HXX

#include <vcl/button.hxx>
#include <vcl/dialog.hxx>
#include <vcl/fixed.hxx>
#include <svx/simptabl.hxx>

class ScViewData;
class ScDocShell;


//=============================================================================
// class ScShareDocumentDlg
//=============================================================================

class ScShareDocumentDlg : public ModalDialog
{
private:
    CheckBox            maCbShare;
    FixedText           maFtWarning;
    FixedLine           maFlUsers;
    FixedText           maFtUsers;
    SvxSimpleTableContainer m_aLbUsersContainer;
    SvxSimpleTable      maLbUsers;
    FixedLine           maFlEnd;
    HelpButton          maBtnHelp;
    OKButton            maBtnOK;
    CancelButton        maBtnCancel;

    String              maStrTitleName;
    String              maStrTitleAccessed;
    String              maStrNoUserData;
    String              maStrUnkownUser;
    String              maStrExclusiveAccess;

    ScViewData*         mpViewData;
    ScDocShell*         mpDocShell;

    DECL_LINK( ToggleHandle, void* );

public:
                        ScShareDocumentDlg( Window* pParent, ScViewData* pViewData );
                        ~ScShareDocumentDlg();

    bool                IsShareDocumentChecked() const;
    void                UpdateView();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
