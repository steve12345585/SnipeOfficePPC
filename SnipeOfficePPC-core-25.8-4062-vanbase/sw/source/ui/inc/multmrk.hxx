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

#ifndef _MULTMRK_HXX
#define _MULTMRK_HXX


#include <svx/stddlg.hxx>
#include <vcl/fixed.hxx>
#include <svtools/stdctrl.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/button.hxx>

class SwTOXMgr;

/*--------------------------------------------------------------------
     Description:   insert mark for index entry
 --------------------------------------------------------------------*/

class SwMultiTOXMarkDlg : public SvxStandardDialog
{
    DECL_LINK( SelectHdl, ListBox * );

    FixedLine           aTOXFL;
    FixedText           aEntryFT;
    FixedInfo           aTextFT;
    FixedText           aTOXFT;
    ListBox             aTOXLB;
    OKButton            aOkBT;
    CancelButton        aCancelBT;

    SwTOXMgr           &rMgr;
    sal_uInt16              nPos;

    void                Apply();
public:
    SwMultiTOXMarkDlg( Window* pParent, SwTOXMgr &rTOXMgr );
    ~SwMultiTOXMarkDlg();
};


#endif // _MULTMRK_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
