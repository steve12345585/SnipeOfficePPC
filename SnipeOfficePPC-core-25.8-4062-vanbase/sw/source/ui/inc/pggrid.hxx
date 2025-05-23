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
#ifndef _PGGRID_HXX
#define _PGGRID_HXX

#include <sfx2/tabdlg.hxx>
#include <colex.hxx>
#include <vcl/field.hxx>
#include <vcl/fixed.hxx>
#include <svtools/ctrlbox.hxx>

/*--------------------------------------------------------------------
    Description:   TabPage Format/(Styles/)Page/Text grid
 --------------------------------------------------------------------*/
class SwTextGridPage: public SfxTabPage
{
    FixedLine       aGridTypeFL;
    RadioButton     aNoGridRB;
    RadioButton     aLinesGridRB;
    RadioButton     aCharsGridRB;
    CheckBox        aSnapToCharsCB;

    SwPageGridExample   aExampleWN;

    FixedLine       aLayoutFL;

    FixedText       aLinesPerPageFT;
    NumericField    aLinesPerPageNF;
    FixedText       aLinesRangeFT;

    FixedText       aTextSizeFT;
    MetricField     aTextSizeMF;

    FixedText       aCharsPerLineFT;
    NumericField    aCharsPerLineNF;
    FixedText       aCharsRangeFT;

    FixedText       aCharWidthFT;
    MetricField     aCharWidthMF;

    FixedText       aRubySizeFT;
    MetricField     aRubySizeMF;

    CheckBox        aRubyBelowCB;

    FixedLine       aDisplayFL;

    CheckBox        aDisplayCB;
    CheckBox        aPrintCB;
    FixedText       aColorFT;
    ColorListBox    aColorLB;

    Window*         aControls[20];

    sal_Int32       m_nRubyUserValue;
    sal_Bool        m_bRubyUserValue;
    Size            m_aPageSize;
    sal_Bool        m_bVertical;
    sal_Bool        m_bSquaredMode;
    sal_Bool        m_bHRulerChanged;
    sal_Bool        m_bVRulerChanged;

    SwTextGridPage(Window *pParent, const SfxItemSet &rSet);
    ~SwTextGridPage();

    void UpdatePageSize(const SfxItemSet& rSet);
    void PutGridItem(SfxItemSet& rSet);
    void SetLinesOrCharsRanges(FixedText & rField, const sal_Int32 nValue );

    DECL_LINK(GridTypeHdl, RadioButton*);
    DECL_LINK(CharorLineChangedHdl, SpinField*);
    DECL_LINK(TextSizeChangedHdl, SpinField*);
    DECL_LINK(GridModifyHdl, void*);
    DECL_LINK(DisplayGridHdl, void *);

    using SfxTabPage::ActivatePage;
    using SfxTabPage::DeactivatePage;

public:
    static SfxTabPage *Create(Window *pParent, const SfxItemSet &rSet);
    static sal_uInt16* GetRanges();

    virtual sal_Bool    FillItemSet(SfxItemSet &rSet);
    virtual void    Reset(const SfxItemSet &rSet);

    virtual void    ActivatePage( const SfxItemSet& rSet );
    virtual int     DeactivatePage( SfxItemSet* pSet = 0 );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
