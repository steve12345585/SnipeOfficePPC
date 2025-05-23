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
#ifndef _CHART2_TP_SERIESTOAXIS_HXX
#define _CHART2_TP_SERIESTOAXIS_HXX

// header for SfxTabPage
#include <sfx2/tabdlg.hxx>
// header for FixedText
#include <vcl/fixed.hxx>
// header for CheckBox
#include <vcl/button.hxx>
// header for MetricField
#include <vcl/field.hxx>

//.............................................................................
namespace chart
{
//.............................................................................

class SchOptionTabPage : public SfxTabPage
{
public:
    SchOptionTabPage(Window* pParent, const SfxItemSet& rInAttrs);
    virtual ~SchOptionTabPage();

    static SfxTabPage* Create(Window* pParent, const SfxItemSet& rInAttrs);
    virtual sal_Bool FillItemSet(SfxItemSet& rOutAttrs);
    virtual void Reset(const SfxItemSet& rInAttrs);

    void Init( bool bProvidesSecondaryYAxis, bool bProvidesOverlapAndGapWidth, bool bProvidesBarConnectors );

private: //methods
    void AdaptControlPositionsAndVisibility();

private: //member
    FixedLine   aGrpAxis;
    RadioButton aRbtAxis1;
    RadioButton aRbtAxis2;

    FixedLine   aGrpBar;
    FixedText   aFTGap;
    MetricField aMTGap;
    FixedText   aFTOverlap;
    MetricField aMTOverlap;
    CheckBox    aCBConnect;
    CheckBox    aCBAxisSideBySide;

    FixedLine   m_aFL_PlotOptions;
    FixedText   m_aFT_MissingValues;
    RadioButton m_aRB_DontPaint;
    RadioButton m_aRB_AssumeZero;
    RadioButton m_aRB_ContinueLine;

    CheckBox    m_aCBIncludeHiddenCells;

    DECL_LINK(EnableHdl, void * );

    sal_Int32   m_nAllSeriesAxisIndex;

    bool m_bProvidesSecondaryYAxis;
    bool m_bProvidesOverlapAndGapWidth;
    bool m_bProvidesBarConnectors;
};

//.............................................................................
} //namespace chart
//.............................................................................

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
