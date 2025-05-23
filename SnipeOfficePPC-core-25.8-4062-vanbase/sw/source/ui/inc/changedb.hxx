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
#ifndef _CHANGEDB_HXX
#define _CHANGEDB_HXX


#include <vcl/bitmap.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <svtools/stdctrl.hxx>
#include <svtools/treelistbox.hxx>
#include <svx/stddlg.hxx>
#include "dbtree.hxx"

class SwFldMgr;
class SwView;
class SwWrtShell;
struct SwDBData;

/*--------------------------------------------------------------------
     Description: exchange database at fields
 --------------------------------------------------------------------*/
class SwChangeDBDlg: public SvxStandardDialog
{
    FixedLine       aDBListFL;
    FixedText       aUsedDBFT;
    FixedText       aAvailDBFT;
    SvTreeListBox   aUsedDBTLB;
    SwDBTreeList    aAvailDBTLB;
    PushButton      aAddDBPB;
    FixedInfo       aDescFT;
    FixedText       aDocDBTextFT;
    FixedText       aDocDBNameFT;
    OKButton        aOKBT;
    CancelButton    aCancelBT;
    HelpButton      aHelpBT;

    ImageList       aImageList;

    SwWrtShell      *pSh;
    SwFldMgr        *pMgr;

    DECL_LINK(TreeSelectHdl, void * = 0);
    DECL_LINK(ButtonHdl, void *);
    DECL_LINK(AddDBHdl, void *);

    virtual void    Apply();
    void            UpdateFlds();
    void            FillDBPopup();
    SvTreeListEntry*    Insert(const String& rDBName);
    void            ShowDBName(const SwDBData& rDBData);

public:
    SwChangeDBDlg(SwView& rVw);
    ~SwChangeDBDlg();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
