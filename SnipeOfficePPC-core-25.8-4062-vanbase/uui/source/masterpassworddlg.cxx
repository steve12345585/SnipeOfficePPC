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

#include <ids.hrc>
#include <masterpassworddlg.hrc>
#include <masterpassworddlg.hxx>

// MasterPasswordDialog---------------------------------------------------

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(MasterPasswordDialog, OKHdl_Impl)
{
    EndDialog( RET_OK );
    return 1;
}

// -----------------------------------------------------------------------

MasterPasswordDialog::MasterPasswordDialog
(
    Window*                                     pParent,
    ::com::sun::star::task::PasswordRequestMode aDialogMode,
    ResMgr*                                     pResMgr
) :

    ModalDialog( pParent, ResId( DLG_UUI_MASTERPASSWORD, *pResMgr ) ),

    aFTMasterPassword       ( this, ResId( FT_MASTERPASSWORD, *pResMgr ) ),
    aEDMasterPassword       ( this, ResId( ED_MASTERPASSWORD, *pResMgr ) ),
    aFL ( this, ResId( FL_FIXED_LINE, *pResMgr ) ),
    aOKBtn                  ( this, ResId( BTN_MASTERPASSWORD_OK, *pResMgr ) ),
    aCancelBtn              ( this, ResId( BTN_MASTERPASSWORD_CANCEL, *pResMgr ) ),
    aHelpBtn                ( this, ResId( BTN_MASTERPASSWORD_HELP, *pResMgr ) ),
    nDialogMode             ( aDialogMode ),
    pResourceMgr            ( pResMgr )
{
    if( nDialogMode == ::com::sun::star::task::PasswordRequestMode_PASSWORD_REENTER )
    {
        String aErrorMsg( ResId( STR_ERROR_MASTERPASSWORD_WRONG, *pResourceMgr ));
        ErrorBox aErrorBox( pParent, WB_OK, aErrorMsg );
        aErrorBox.Execute();
    }

    FreeResource();

    aOKBtn.SetClickHdl( LINK( this, MasterPasswordDialog, OKHdl_Impl ) );
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
