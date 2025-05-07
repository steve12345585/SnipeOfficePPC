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
#ifndef _OPTPAGE_HXX
#define _OPTPAGE_HXX
#include <sfx2/tabdlg.hxx>

#include <vcl/group.hxx>

#include <vcl/button.hxx>

#include <vcl/lstbox.hxx>

#include <vcl/field.hxx>

#include <vcl/fixed.hxx>
#include <svtools/ctrlbox.hxx>
#include <svx/fntctrl.hxx>
#include <fontcfg.hxx>
class SfxPrinter;
class SwStdFontConfig;
class SwWrtShell;
class FontList;
class SwCompareConfig;

class SwContentOptPage : public SfxTabPage
{
    //visual aids
    FixedLine   aLineFL;
    CheckBox    aCrossCB;

    //view
    FixedLine   aWindowFL;
    CheckBox    aHScrollBox;
    CheckBox    aVScrollBox;
    CheckBox    aAnyRulerCB;
    CheckBox    aHRulerCBox;
    ListBox     aHMetric;
    CheckBox    aVRulerCBox;
    CheckBox    aVRulerRightCBox;
    ListBox     aVMetric;
    CheckBox    aSmoothCBox;

    //display
    FixedLine   aDispFL;
    CheckBox    aGrfCB;
    CheckBox    aTblCB;
    CheckBox    aDrwCB;
    CheckBox    aFldNameCB;
    CheckBox    aPostItCB;

    FixedLine   aSettingsFL;
    FixedText   aMetricFT;
    ListBox     aMetricLB;

    DECL_LINK(VertRulerHdl, CheckBox*);
    DECL_LINK(AnyRulerHdl, CheckBox*);
public:
                        SwContentOptPage( Window* pParent,
                                           const SfxItemSet& rSet );
                        ~SwContentOptPage();

    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet);

    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );

};

/*--------------------------------------------------------
 TabPage printer settings additions
--------------------------------------------------------- */
class SwAddPrinterTabPage : public SfxTabPage
{
    FixedLine       aFL1;
    CheckBox        aGrfCB;
//  CheckBox        aTabCB;
//  CheckBox        aDrawCB;
    CheckBox        aCtrlFldCB;
    CheckBox        aBackgroundCB;
    CheckBox        aBlackFontCB;
    CheckBox        aPrintHiddenTextCB;
    CheckBox        aPrintTextPlaceholderCB;

    FixedLine       aSeparatorLFL;

    FixedLine       aFL2;
    CheckBox        aLeftPageCB;
    CheckBox        aRightPageCB;
//  CheckBox        aReverseCB;
    CheckBox        aProspectCB;
    CheckBox        aProspectCB_RTL;

    FixedLine       aSeparatorRFL;

    FixedLine        aFL3;
    RadioButton     aNoRB;
    RadioButton     aOnlyRB;
    RadioButton     aEndRB;
    RadioButton     aEndPageRB;
    FixedLine        aFL4;
    CheckBox        aPrintEmptyPagesCB;
//    CheckBox        aSingleJobsCB;
    CheckBox        aPaperFromSetupCB;
    FixedText       aFaxFT;
    ListBox         aFaxLB;
    String          sNone;

    sal_Bool        bAttrModified;
    sal_Bool        bPreview;

    void        Init();
                DECL_LINK(AutoClickHdl, void *);
                DECL_LINK(SelectHdl, void *);


                SwAddPrinterTabPage( Window* pParent,
                                           const SfxItemSet& rSet );
public:

    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet );

    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
    void                SetFax( const std::vector<String>& );
    void                SelectFax( const String& );
    void                SetPreview(sal_Bool bPrev);
    virtual void        PageCreated (SfxAllItemSet aSet);

};

class SwStdFontTabPage : public SfxTabPage
{
    FixedLine       aStdChrFL  ;

    FixedText       aTypeFT;

    FixedText       aStandardLbl;
    ComboBox        aStandardBox;

    FixedText       aHeightFT;
    FontSizeBox     aStandardHeightLB;

    FixedText       aTitleLbl   ;
    ComboBox        aTitleBox   ;
    FontSizeBox     aTitleHeightLB;

    FixedText       aListLbl    ;
    ComboBox        aListBox    ;
    FontSizeBox     aListHeightLB;

    FixedText       aLabelLbl   ;
    ComboBox        aLabelBox   ;
    FontSizeBox     aLabelHeightLB;

    FixedText       aIdxLbl     ;
    ComboBox        aIdxBox     ;
    FontSizeBox     aIndexHeightLB;

    CheckBox        aDocOnlyCB  ;
    PushButton      aStandardPB;

    String          sShellStd;
    String          sShellTitle;
    String          sShellList;
    String          sShellLabel;
    String          sShellIndex;

    SfxPrinter*         pPrt;
    FontList*           pFontList;
    SwStdFontConfig*    pFontConfig;
    SwWrtShell*         pWrtShell;
    LanguageType        eLanguage;
    // only defaults were there? they were signed with the boxes
    sal_Bool    bListDefault    :1;
    sal_Bool    bSetListDefault :1;
    sal_Bool    bLabelDefault   :1;
    sal_Bool    bSetLabelDefault :1;
    sal_Bool    bIdxDefault     :1;
    sal_Bool    bSetIdxDefault  :1;
    sal_Bool    bDeletePrinter :1;

    sal_Bool    bListHeightDefault    :1;
    sal_Bool    bSetListHeightDefault :1;
    sal_Bool    bLabelHeightDefault   :1;
    sal_Bool    bSetLabelHeightDefault :1;
    sal_Bool    bIndexHeightDefault     :1;
    sal_Bool    bSetIndexHeightDefault  :1;

    sal_uInt8 nFontGroup; //fontcfg.hxx: FONT_GROUP_[STANDARD|CJK|CTL]

    String sScriptWestern;
    String sScriptAsian;
    String sScriptComplex;

    DECL_LINK(StandardHdl, void *);
    DECL_LINK( ModifyHdl, ComboBox * );
    DECL_LINK( ModifyHeightHdl, FontSizeBox * );
    DECL_LINK( LoseFocusHdl, ComboBox * );

            SwStdFontTabPage( Window* pParent,
                                       const SfxItemSet& rSet );
            ~SwStdFontTabPage();

public:
    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet );

    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );

    void    SetFontMode(sal_uInt8 nGroup) {nFontGroup = nGroup;}
    virtual void        PageCreated (SfxAllItemSet aSet);
};

class SwTableOptionsTabPage : public SfxTabPage
{
    FixedLine   aTableFL;
    CheckBox    aHeaderCB;
    CheckBox    aRepeatHeaderCB;
    CheckBox    aDontSplitCB;
    CheckBox    aBorderCB;

    FixedLine   aSeparatorFL;

    FixedLine   aTableInsertFL;
    CheckBox    aNumFormattingCB;
    CheckBox    aNumFmtFormattingCB;
    CheckBox    aNumAlignmentCB;

    FixedLine   aMoveFL;
    FixedText   aMoveFT;
    FixedText   aRowMoveFT;
    MetricField aRowMoveMF;
    FixedText   aColMoveFT;
    MetricField aColMoveMF;

    FixedText   aInsertFT;
    FixedText   aRowInsertFT;
    MetricField aRowInsertMF;
    FixedText   aColInsertFT;
    MetricField aColInsertMF;

    FixedText   aHandlingFT;
    RadioButton aFixRB;
    RadioButton aFixPropRB;
    RadioButton aVarRB;
    FixedText   aFixFT;
    FixedText   aFixPropFT;
    FixedText   aVarFT;

    SwWrtShell* pWrtShell;
    sal_Bool        bHTMLMode;

    DECL_LINK(CheckBoxHdl, void *);


                SwTableOptionsTabPage( Window* pParent,
                                           const SfxItemSet& rSet );
                ~SwTableOptionsTabPage();

public:

    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet );

    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );

    void SetWrtShell(SwWrtShell* pSh) {pWrtShell = pSh;}
    virtual void        PageCreated (SfxAllItemSet aSet);

};

/*--------------------------------------------------
 TabPage for ShadowCrsr
--------------------------------------------------*/
class SwShdwCrsrOptionsTabPage : public SfxTabPage
{
    //nonprinting characters
    FixedLine   aUnprintFL;
    CheckBox    aParaCB;
    CheckBox    aSHyphCB;
    CheckBox    aSpacesCB;
    CheckBox    aHSpacesCB;
    CheckBox    aTabCB;
    CheckBox    aBreakCB;
    CheckBox    aCharHiddenCB;
    CheckBox    aFldHiddenCB;
    CheckBox    aFldHiddenParaCB;

    FixedLine   aSeparatorFL;

    FixedLine       aFlagFL;
    CheckBox        aOnOffCB;

    FixedText       aFillModeFT;
    RadioButton     aFillMarginRB;
    RadioButton     aFillIndentRB;
    RadioButton     aFillTabRB;
    RadioButton     aFillSpaceRB;

    FixedLine       aCrsrOptFL;
    CheckBox        aCrsrInProtCB;

    FixedLine       m_aLayoutOptionsFL;
    CheckBox        m_aMathBaselineAlignmentCB;

    SwWrtShell *    m_pWrtShell;


    SwShdwCrsrOptionsTabPage( Window* pParent, const SfxItemSet& rSet );
    ~SwShdwCrsrOptionsTabPage();

public:

    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rAttrSet );

    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );

    void    SetWrtShell( SwWrtShell * pSh ) { m_pWrtShell = pSh; }
    virtual void        PageCreated( SfxAllItemSet aSet );
};

/*-----------------------------------------------------------------------
    Description: mark preview
 -----------------------------------------------------------------------*/
class SwMarkPreview : public Window
{
    Color           m_aBgCol;           // background
    Color           m_aTransCol;        // transparency
    Color           m_aMarkCol;         // marks
    Color           m_aLineCol;         // general lines
    Color           m_aShadowCol;       // shadow
    Color           m_aTxtCol;          // text
    Color           m_aPrintAreaCol;    // frame for print area

    Rectangle       aPage;
    Rectangle       aLeftPagePrtArea;
    Rectangle       aRightPagePrtArea;

    sal_uInt16          nMarkPos;

    using OutputDevice::DrawRect;
    void            DrawRect(const Rectangle &rRect, const Color &rFillColor, const Color &rLineColor);

    void            Paint(const Rectangle&);
    void            PaintPage(const Rectangle &rRect);
    void            InitColors( void );

protected:
    virtual void    DataChanged( const DataChangedEvent& rDCEvt );

public:
                    SwMarkPreview(Window* pParent, const ResId& rResID);
    virtual         ~SwMarkPreview();

    inline void     SetColor(const Color& rCol) { m_aMarkCol = rCol; }
    inline void     SetMarkPos(sal_uInt16 nPos) { nMarkPos = nPos; }
};

/*-----------------------------------------------------------------------
    Description: redlining options
 -----------------------------------------------------------------------*/
class SwRedlineOptionsTabPage : public SfxTabPage
{
    FixedLine           aInsertFL;

    FixedText           aInsertFT;
    FixedText           aInsertAttrFT;
    ListBox             aInsertLB;
    FixedText           aInsertColorFT;
    ColorListBox        aInsertColorLB;
    SvxFontPrevWindow   aInsertedPreviewWN;

    FixedText           aDeletedFT;
    FixedText           aDeletedAttrFT;
    ListBox             aDeletedLB;
    FixedText           aDeletedColorFT;
    ColorListBox        aDeletedColorLB;
    SvxFontPrevWindow   aDeletedPreviewWN;

    FixedText           aChangedFT;
    FixedText           aChangedAttrFT;
    ListBox             aChangedLB;
    FixedText           aChangedColorFT;
    ColorListBox        aChangedColorLB;
    SvxFontPrevWindow   aChangedPreviewWN;

    FixedLine           aChangedFL;

    FixedText           aMarkPosFT;
    ListBox             aMarkPosLB;
    FixedText           aMarkColorFT;
    ColorListBox        aMarkColorLB;
    SwMarkPreview       aMarkPreviewWN;

    String              sAuthor;
    String              sNone;

    SwRedlineOptionsTabPage( Window* pParent, const SfxItemSet& rSet );
    ~SwRedlineOptionsTabPage();

    DECL_LINK( AttribHdl, ListBox *pLB );
    DECL_LINK(ChangedMaskPrevHdl, void * = 0);
    DECL_LINK( ColorHdl, ColorListBox *pColorLB );

    void                InitFontStyle(SvxFontPrevWindow& rExampleWin);

public:

    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rAttrSet );

    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
};

/*-------------------------------------------------------
 TabPage test settings for SW
--------------------------------------------------------- */

#ifdef DBG_UTIL

class SwTestTabPage : public SfxTabPage
{
public:
                        SwTestTabPage( Window* pParent,
                                           const SfxItemSet& rSet );

    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet );

    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );

private:
    FixedLine aTestFL;
    CheckBox aTest1CBox;
    CheckBox aTest2CBox;
    CheckBox aTest3CBox;
    CheckBox aTest4CBox;
    CheckBox aTest5CBox;
    CheckBox aTest6CBox;
    CheckBox aTest7CBox;
    CheckBox aTest8CBox;
    CheckBox aTest9CBox;
    CheckBox aTest10CBox;

    sal_Bool        bAttrModified;

    void        Init();
    DECL_LINK(AutoClickHdl, void *);

};
#endif // DBG_UTIL

class SwCompareOptionsTabPage : public SfxTabPage
{
    FixedLine aComparisonFL;
    RadioButton aAutoRB;
    RadioButton aWordRB;
    RadioButton aCharRB;

    FixedLine aSettingsFL;
    CheckBox aRsidCB;
    CheckBox aIgnoreCB;
    NumericField aLenNF;

    SwCompareOptionsTabPage( Window* pParent, const SfxItemSet& rSet );
    ~SwCompareOptionsTabPage();

    DECL_LINK(ComparisonHdl, void *);
    DECL_LINK(IgnoreHdl, void *);

public:

    static SfxTabPage*	Create( Window* pParent, const SfxItemSet& rAttrSet );

    virtual	sal_Bool    FillItemSet( SfxItemSet& rSet );
    virtual	void 		Reset( const SfxItemSet& rSet );
};

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
