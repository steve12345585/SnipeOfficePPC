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
#ifndef _SVX_FONT_SUBSTITUTION_HXX
#define _SVX_FONT_SUBSTITUTION_HXX

#include <sfx2/tabdlg.hxx>
#include <vcl/fixed.hxx>
#include <svx/simptabl.hxx>
#include <vcl/toolbox.hxx>
#include <svtools/ctrlbox.hxx>

// class SvxFontSubstCheckListBox ------------------------------------------

class SvxFontSubstCheckListBox : public SvxSimpleTable
{
    friend class SvxFontSubstTabPage;
    using SvxSimpleTable::SetTabs;
    using SvTreeListBox::GetCheckButtonState;
    using SvTreeListBox::SetCheckButtonState;

    protected:
        virtual void    SetTabs();
        virtual void    KeyInput( const KeyEvent& rKEvt );

    public:
        SvxFontSubstCheckListBox(SvxSimpleTableContainer& rParent, WinBits nBits = WB_BORDER)
            : SvxSimpleTable(rParent, nBits)
        {
        }

        inline void     *GetUserData(sal_uLong nPos) { return GetEntry(nPos)->GetUserData(); }
        inline void     SetUserData(sal_uLong nPos, void *pData ) { GetEntry(nPos)->SetUserData(pData); }

        sal_Bool            IsChecked(sal_uLong nPos, sal_uInt16 nCol = 0);
        sal_Bool            IsChecked(SvLBoxEntry* pEntry, sal_uInt16 nCol = 0);
        void            CheckEntryPos(sal_uLong nPos, sal_uInt16 nCol, sal_Bool bChecked);
        void            CheckEntry(SvLBoxEntry* pEntry, sal_uInt16 nCol, sal_Bool bChecked);
        SvButtonState   GetCheckButtonState( SvLBoxEntry*, sal_uInt16 nCol ) const;
        void            SetCheckButtonState( SvLBoxEntry*, sal_uInt16 nCol, SvButtonState );
};

// class SvxFontSubstTabPage ----------------------------------------------------
class SvtFontSubstConfig;
class SvxFontSubstTabPage : public SfxTabPage
{
    CheckBox                    aUseTableCB;
    FixedText                   aFont1FT;
    FontNameBox                 aFont1CB;
    FixedText                   aFont2FT;
    FontNameBox                 aFont2CB;
    ToolBox                     aNewDelTBX;
    SvxSimpleTableContainer m_aCheckLBContainer;
    SvxFontSubstCheckListBox    aCheckLB;

    FixedLine                   aSourceViewFontsFL;
    FixedText                   aFontNameFT;
    ListBox                     aFontNameLB;
    CheckBox                    aNonPropFontsOnlyCB;
    FixedText                   aFontHeightFT;
    ListBox                     aFontHeightLB;

    ImageList                   aImageList;
    String                      sAutomatic;

    SvtFontSubstConfig*         pConfig;

    String          sHeader1;
    String          sHeader2;
    String          sHeader3;
    String          sHeader4;

    Color           aTextColor;

    SvLBoxButtonData*   pCheckButtonData;

    DECL_LINK(SelectHdl, Window *pWin = 0);
    DECL_LINK(NonPropFontsHdl, CheckBox* pBox);

    SvLBoxEntry*    CreateEntry(String& rFont1, String& rFont2);
    void            CheckEnable();


    SvxFontSubstTabPage( Window* pParent, const SfxItemSet& rSet );
    ~SvxFontSubstTabPage();

public:
    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rAttrSet);
    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
};


#endif // _SVX_FONT_SUBSTITUTION_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
