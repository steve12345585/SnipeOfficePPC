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
#ifndef _SWTABLEPG_HXX
#define _SWTABLEPG_HXX
#include <sfx2/tabdlg.hxx>
#include <vcl/fixed.hxx>
#include <vcl/button.hxx>
#include <vcl/lstbox.hxx>
#include <actctrl.hxx>
#include "prcntfld.hxx"
#include "swtypes.hxx"
#include "textcontrolcombo.hxx"

class SwWrtShell;
class SwTableRep;

struct TColumn
{
    SwTwips nWidth;
    sal_Bool    bVisible;
};

class SwFormatTablePage : public SfxTabPage
{
    FixedLine        aOptionsFL;
    FixedText       aNameFT;
    TableNameEdit   aNameED;
    FixedText       aWidthFT;
    PercentField    aWidthMF;
    CheckBox        aRelWidthCB;

    FixedLine        aPosFL;
    RadioButton     aFullBtn;
    RadioButton     aLeftBtn;
    RadioButton     aFromLeftBtn;
    RadioButton     aRightBtn;
    RadioButton     aCenterBtn;
    RadioButton     aFreeBtn;

    FixedLine       aDistFL;
    FixedText       aLeftFT;
    PercentField    aLeftMF;
    FixedText       aRightFT;
    PercentField    aRightMF;
    FixedText       aTopFT;
    MetricField     aTopMF;
    FixedText       aBottomFT;
    MetricField     aBottomMF;

    FixedLine       aPropertiesFL;
    FixedText       aTextDirectionFT;
    ListBox         aTextDirectionLB;

    SwTableRep*     pTblData;
    SwTwips         nSaveWidth;
    SwTwips         nMinTableWidth;
    sal_uInt16          nOldAlign;
    sal_Bool            bModified;
    sal_Bool            bFull:1;
    sal_Bool            bHtmlMode : 1;

    void        Init();
    void        ModifyHdl( Edit* pEdit );

    DECL_LINK( AutoClickHdl, CheckBox * );
    DECL_LINK( RelWidthClickHdl, CheckBox * );
    void RightModify();
    DECL_LINK( UpDownLoseFocusHdl, MetricField * );

    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

public:
    SwFormatTablePage( Window* pParent, const SfxItemSet& rSet );

    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rAttrSet);
    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
    virtual void        ActivatePage( const SfxItemSet& rSet );
    virtual int         DeactivatePage( SfxItemSet* pSet = 0 );
};

/*-------------------------------------------------------
 TabPage Format/Tabelle/Spalten
--------------------------------------------------------- */
#define MET_FIELDS 6 //Anzahl der verwendeten MetricFields

class SwTableColumnPage : public SfxTabPage
{
    CheckBox        aModifyTableCB;
    CheckBox        aProportionalCB;
    FixedText       aSpaceFT;
    MetricField     aSpaceED;
     FixedLine        aColFL;
    ImageButton     aUpBtn;
    FixedText       aFT1;
    PercentField    aMF1;
    FixedText       aFT2;
    PercentField    aMF2;
    FixedText       aFT3;
    PercentField    aMF3;
    FixedText       aFT4;
    PercentField    aMF4;
    FixedText       aFT5;
    PercentField    aMF5;
    FixedText       aFT6;
    PercentField    aMF6;
    ImageButton     aDownBtn;

    SwTableRep*     pTblData;
    PercentField*   pFieldArr[MET_FIELDS];
    FixedText*      pTextArr[MET_FIELDS];
    SwTwips         nTableWidth;
    SwTwips         nMinWidth;
    sal_uInt16          nNoOfCols;
    sal_uInt16          nNoOfVisibleCols;
    //Breite merken, wenn auf autom. Ausrichtung gestellt wird
    sal_uInt16          aValueTbl[MET_FIELDS];//primaere Zuordnung der MetricFields
    sal_Bool            bModified:1;
    sal_Bool            bModifyTbl:1;
    sal_Bool            bPercentMode:1;

    void        Init(sal_Bool bWeb);
    DECL_LINK( AutoClickHdl, CheckBox * );
    void        ModifyHdl( PercentField* pEdit );
    DECL_LINK( UpHdl, PercentField * );
    DECL_LINK( DownHdl, PercentField * );
    DECL_LINK( LoseFocusHdl, PercentField * );
    DECL_LINK( ModeHdl, CheckBox * );
    void        UpdateCols( sal_uInt16 nAktPos );
    SwTwips     GetVisibleWidth(sal_uInt16 nPos);
    void        SetVisibleWidth(sal_uInt16 nPos, SwTwips nNewWidth);

    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

public:
    SwTableColumnPage( Window* pParent, const SfxItemSet& rSet );
    ~SwTableColumnPage();

    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rAttrSet);
    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
    virtual void        ActivatePage( const SfxItemSet& rSet );
    virtual int         DeactivatePage( SfxItemSet* pSet = 0 );

};

class SwTextFlowPage : public SfxTabPage
{
    FixedLine       aFlowFL;
    CheckBox        aPgBrkCB;
    RadioButton     aPgBrkRB;
    RadioButton     aColBrkRB;
    RadioButton     aPgBrkBeforeRB;
    RadioButton     aPgBrkAfterRB;
    CheckBox        aPageCollCB;
    ListBox         aPageCollLB;
    FixedText       aPageNoFT;
    NumericField    aPageNoNF;
    CheckBox        aSplitCB;
    TriStateBox     aSplitRowCB;
    CheckBox        aKeepCB;
    CheckBox        aHeadLineCB;
    FixedText       aRepeatHeaderFT;    // "dummy" to build before and after FT
    FixedText       aRepeatHeaderBeforeFT;
    NumericField    aRepeatHeaderNF;
    FixedText       aRepeatHeaderAfterFT;
    TextControlCombo    aRepeatHeaderCombo;
    FixedText       aTextDirectionFT;
    ListBox         aTextDirectionLB;

    FixedLine       aVertOrientFL;
    FixedText       aVertOrientFT;
    ListBox         aVertOrientLB;

    SwWrtShell*     pShell;

    sal_Bool            bPageBreak;
    sal_Bool            bHtmlMode;


    DECL_LINK(PageBreakHdl_Impl, void *);
    DECL_LINK(ApplyCollClickHdl_Impl, void *);
    DECL_LINK( PageBreakPosHdl_Impl, RadioButton* );
    DECL_LINK( PageBreakTypeHdl_Impl, RadioButton* );
    DECL_LINK( SplitHdl_Impl, CheckBox* );
    DECL_LINK( SplitRowHdl_Impl, TriStateBox* );
    DECL_LINK( HeadLineCBClickHdl, void* p = 0 );


    SwTextFlowPage( Window* pParent, const SfxItemSet& rSet );
    ~SwTextFlowPage();

public:
    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rAttrSet);
    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );

    void                SetShell(SwWrtShell* pSh);

    void                DisablePageBreak();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
