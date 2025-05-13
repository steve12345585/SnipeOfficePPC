/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#ifndef SC_TPUSRLST_HXX
#define SC_TPUSRLST_HXX

#include <sfx2/tabdlg.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>
#include <svtools/svmedit.hxx>

//========================================================================

class ScUserList;
class ScDocument;
class ScViewData;
class ScRangeUtil;

class ScTpUserLists : public SfxTabPage
{
public:
    static  SfxTabPage* Create          ( Window*               pParent,
                                          const SfxItemSet&     rAttrSet );
    virtual sal_Bool        FillItemSet     ( SfxItemSet& rCoreAttrs );
    virtual void        Reset           ( const SfxItemSet& rCoreAttrs );
    using SfxTabPage::DeactivatePage;
    virtual int         DeactivatePage  ( SfxItemSet* pSet = NULL );

private:
            ScTpUserLists( Window*              pParent,
                           const SfxItemSet&    rArgSet );
            ~ScTpUserLists();

private:
    FixedText       aFtLists;
    ListBox         aLbLists;
    FixedText       aFtEntries;
    MultiLineEdit   aEdEntries;
    FixedText       aFtCopyFrom;
    Edit            aEdCopyFrom;
    PushButton      aBtnNew;
    PushButton      aBtnAdd;
    PushButton      aBtnRemove;
    PushButton      aBtnCopy;

    const String    aStrQueryRemove;
    const String    aStrNew;
    const String    aStrCancel;
    const String    aStrAdd;
    const String    aStrModify;
    const String    aStrCopyList;
    const String    aStrCopyFrom;
    const String    aStrCopyErr;

    const sal_uInt16    nWhichUserLists;
    ScUserList*     pUserLists;

    ScDocument*     pDoc;
    ScViewData*     pViewData;
    ScRangeUtil*    pRangeUtil;
    String          aStrSelectedArea;

    sal_Bool            bModifyMode;
    sal_Bool            bCancelMode;
    sal_Bool            bCopyDone;
    sal_uInt16          nCancelPos;

#ifdef _TPUSRLST_CXX
private:
    void    Init                ();
    sal_uInt16  UpdateUserListBox   ();
    void    UpdateEntries       ( size_t nList );
    void    MakeListStr         ( String& rListStr );
    void    AddNewList          ( const String& rEntriesStr );
    void    RemoveList          ( size_t nList );
    void    ModifyList          ( sal_uInt16        nSelList,
                                  const String& rEntriesStr );
    void    CopyListFromArea    ( const ScRefAddress& rStartPos,
                                  const ScRefAddress& rEndPos );

    // Handler:
    DECL_LINK( LbSelectHdl,     ListBox* );
    DECL_LINK( BtnClickHdl,     PushButton* );
    DECL_LINK( EdEntriesModHdl, MultiLineEdit* );
#endif
};



#endif // SC_TPUSRLST_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
