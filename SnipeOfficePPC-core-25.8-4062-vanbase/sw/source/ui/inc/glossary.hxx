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
#ifndef _GLOSSARY_HXX
#define _GLOSSARY_HXX

#include <vcl/edit.hxx>
#include <svtools/treelistbox.hxx>
#include <svx/stddlg.hxx>

#include <vcl/button.hxx>
#include <vcl/fixed.hxx>

#include <vcl/combobox.hxx>

#include <vcl/menubtn.hxx>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XContentEnumerationAccess.hpp>
#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/container/XElementAccess.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>

#include <actctrl.hxx>

class SwGlossaryHdl;
class SwNewGlosNameDlg;
class SwWrtShell;
class SfxViewFrame;
class PopupMenu;
class Menu;

const short RET_EDIT = 100;

//------------------------------------------------------------------

class SwGlTreeListBox : public SvTreeListBox
{
    const String    sReadonly;

    SvTreeListEntry*  pDragEntry;

    virtual DragDropMode NotifyStartDrag( TransferDataContainer& rContainer,
                                            SvTreeListEntry* );
    virtual sal_Bool    NotifyAcceptDrop( SvTreeListEntry* );

    virtual sal_Bool    NotifyMoving(   SvTreeListEntry*  pTarget,
                                    SvTreeListEntry*  pEntry,
                                    SvTreeListEntry*& rpNewParent,
                                    sal_uLong&        rNewChildPos
                                );
    virtual sal_Bool    NotifyCopying(  SvTreeListEntry*  pTarget,
                                    SvTreeListEntry*  pEntry,
                                    SvTreeListEntry*& rpNewParent,
                                    sal_uLong&        rNewChildPos);
public:
    SwGlTreeListBox(Window* pParent, const ResId& rResId);

    virtual void    RequestHelp( const HelpEvent& rHEvt );
    void            Clear();
};

//------------------------------------------------------------------
class SwOneExampleFrame;
class SwGlossaryDlg : public SvxStandardDialog
{
    friend class SwNewGlosNameDlg;
    friend class SwGlTreeListBox;

    CheckBox        aInsertTipCB;
    FixedText       aNameLbl;
    Edit            aNameED;
    FixedText       aShortNameLbl;
    NoSpaceEdit     aShortNameEdit;
    SwGlTreeListBox aCategoryBox;
    FixedLine       aRelativeFL;
    CheckBox        aFileRelCB;
    CheckBox        aNetRelCB;
    Window          aExampleWIN;
    Window          aExampleDummyWIN;
    CheckBox        aShowExampleCB;
    OKButton        aInsertBtn;
    CancelButton    aCloseBtn;
    HelpButton      aHelpBtn;
    MenuButton      aEditBtn;
    PushButton      aBibBtn;
    PushButton      aPathBtn;

    String          sReadonlyPath;

    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >        _xAutoText;
    SwOneExampleFrame*  pExampleFrame;

    PopupMenu*      pMenu;
    SwGlossaryHdl*  pGlossaryHdl;

    String          sResumeGroup;
    String          sResumeShortName;
    sal_Bool            bResume;


    const sal_Bool      bSelection : 1;
    sal_Bool            bReadOnly : 1;
    sal_Bool            bIsOld : 1;
    sal_Bool            bIsDocReadOnly:1;

    SwWrtShell*     pSh;

    void EnableShortName(sal_Bool bOn = sal_True);

    DECL_LINK( NameModify, Edit * );
    DECL_LINK( NameDoubleClick, SvTreeListBox * );
    DECL_LINK( GrpSelect, SvTreeListBox * );
    DECL_LINK( MenuHdl, Menu * );
    DECL_LINK( EnableHdl, Menu * );
    DECL_LINK(BibHdl, void *);
    DECL_LINK(EditHdl, void *);
    DECL_LINK( PathHdl, Button * );
    DECL_LINK( CheckBoxHdl, CheckBox * );
    DECL_LINK( ShowPreviewHdl, CheckBox * );
    DECL_LINK( PreviewLoadedHdl, void * );


    virtual void    Apply();
    void            Init();
    SvTreeListEntry*    DoesBlockExist(const String& sBlock, const String& rShort);
    void            ShowAutoText(const String& rGroup, const String& rShortName);
    void            ResumeShowAutoText();

    sal_Bool            GetResumeData(String& rGroup, String& rShortName)
                        {rGroup = sResumeGroup; rShortName = sResumeShortName; return bResume;}
    void            SetResumeData(const String& rGroup, const String& rShortName)
                        {sResumeGroup = rGroup; sResumeShortName = rShortName; bResume = sal_True;}
    void            ResetResumeData() {bResume = sal_False;}
public:
    SwGlossaryDlg(SfxViewFrame* pViewFrame, SwGlossaryHdl* pGlosHdl, SwWrtShell *pWrtShell);
    ~SwGlossaryDlg();
    String          GetCurrGrpName() const;
    inline String   GetCurrLongName() const;
    inline String   GetCurrShortName() const;
    static String   GetCurrGroup();
    static void     SetActGroup(const String& rNewGroup);
    static String   GetExtension();
};

inline String SwGlossaryDlg::GetCurrLongName() const
{
    return aNameED.GetText();
}
inline String SwGlossaryDlg::GetCurrShortName() const
{
    return aShortNameEdit.GetText();
}


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
