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
#ifndef _INPDLG_HXX
#define _INPDLG_HXX

#include <svx/stddlg.hxx>

#include <svtools/svmedit.hxx>
#include <vcl/fixed.hxx>
#include <vcl/button.hxx>

class SwInputField;
class SwSetExpField;
class SwUserFieldType;
class SwField;
class SwWrtShell;

/*--------------------------------------------------------------------
     Description: insert fields
 --------------------------------------------------------------------*/

class SwFldInputDlg: public SvxStandardDialog
{
    virtual void    Apply();
    virtual void    StateChanged( StateChangedType );

    SwWrtShell         &rSh;
    SwInputField*       pInpFld;
    SwSetExpField*      pSetFld;
    SwUserFieldType*    pUsrType;

    Edit                aLabelED;

    MultiLineEdit       aEditED;
    FixedLine           aEditFL;

    OKButton            aOKBT;
    CancelButton        aCancelBT;
    PushButton          aNextBT;
    HelpButton          aHelpBT;

    DECL_LINK(NextHdl, void *);
public:
    SwFldInputDlg(  Window *pParent, SwWrtShell &rSh,
                    SwField* pField, sal_Bool bNextButton = sal_False );
    ~SwFldInputDlg();
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
