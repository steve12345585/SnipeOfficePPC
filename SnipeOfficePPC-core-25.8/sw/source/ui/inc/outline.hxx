/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef _OUTLINE_HXX
#define _OUTLINE_HXX

#include <sfx2/tabdlg.hxx>


#include <vcl/menu.hxx>

#include <vcl/button.hxx>
#include <svtools/stdctrl.hxx>

#include <vcl/fixed.hxx>

#include <vcl/lstbox.hxx>

#include <vcl/edit.hxx>

#include <vcl/field.hxx>

#include "swtypes.hxx"      //for MAXLEVEL
#include <numprevw.hxx>
#include <numberingtypelistbox.hxx>

class SwWrtShell;
class SwNumRule;
class SwChapterNumRules;

class SwOutlineTabDialog : public SfxTabDialog
{
    static     sal_uInt16    nNumLevel;

    String              aNullStr;
    String              aCollNames[MAXLEVEL];
    PopupMenu           aFormMenu;

    SwWrtShell&         rWrtSh;
    SwNumRule*          pNumRule;
    SwChapterNumRules*  pChapterNumRules;

    sal_Bool                bModified : 1;

    protected:
    DECL_LINK(CancelHdl, void *);
    DECL_LINK( FormHdl, Button * );
    DECL_LINK( MenuSelectHdl, Menu * );

        virtual void    PageCreated(sal_uInt16 nPageId, SfxTabPage& rPage);
        virtual short   Ok();

    public:
        SwOutlineTabDialog(Window* pParent,
                    const SfxItemSet* pSwItemSet,
                    SwWrtShell &);
        ~SwOutlineTabDialog();

    SwNumRule*          GetNumRule() {return pNumRule;}
    sal_uInt16              GetLevel(const String &rFmtName) const;
    String*             GetCollNames() {return aCollNames;}

    static sal_uInt16       GetActNumLevel() {return nNumLevel;}
    static void         SetActNumLevel(sal_uInt16 nSet) {nNumLevel = nSet;}
};

class SwOutlineSettingsTabPage : public SfxTabPage
{
    FixedLine       aLevelFL;
    ListBox         aLevelLB;

    FixedLine        aNumberFL;
    FixedText       aCollLbl;
    ListBox         aCollBox;
    FixedText       aNumberLbl;
    SwNumberingTypeListBox  aNumberBox;
    FixedText       aCharFmtFT;
    ListBox         aCharFmtLB;
    FixedText       aAllLevelFT;
    NumericField    aAllLevelNF;
    FixedText       aDelim;
    FixedText       aPrefixFT;
    Edit            aPrefixED;
    FixedText       aSuffixFT;
    Edit            aSuffixED;
    FixedText       aStartLbl;
    NumericField    aStartEdit;
    NumberingPreview aPreviewWIN;

    String              aNoFmtName;
    String              aSaveCollNames[MAXLEVEL];
    SwWrtShell*         pSh;
    SwNumRule*          pNumRule;
    String*             pCollNames;
    sal_uInt16              nActLevel;

    DECL_LINK( LevelHdl, ListBox * );
    DECL_LINK( ToggleComplete, NumericField * );
    DECL_LINK( CollSelect, ListBox * );
    DECL_LINK(CollSelectGetFocus, void *);
    DECL_LINK( NumberSelect, SwNumberingTypeListBox * );
    DECL_LINK(DelimModify, void *);
    DECL_LINK( StartModified, NumericField * );
    DECL_LINK(CharFmtHdl, void *);

    void    Update();

    void    SetModified(){aPreviewWIN.Invalidate();}
    void    CheckForStartValue_Impl(sal_uInt16 nNumberingType);

    using SfxTabPage::ActivatePage;
    using SfxTabPage::DeactivatePage;

public:
    SwOutlineSettingsTabPage(Window* pParent, const SfxItemSet& rSet);
    ~SwOutlineSettingsTabPage();

    void SetWrtShell(SwWrtShell* pShell);

    virtual void        ActivatePage(const SfxItemSet& rSet);
    virtual int         DeactivatePage(SfxItemSet *pSet);

    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
