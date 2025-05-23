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
#ifndef _ENVFMT_HXX
#define _ENVFMT_HXX

#include <svtools/stdctrl.hxx>
#include <vcl/field.hxx>
#include <vcl/menubtn.hxx>
#include <vcl/group.hxx>

#include "envlop.hxx"

class SwTxtFmtColl;

// class SwEnvFmtPage ---------------------------------------------------------

class SwEnvFmtPage : public SfxTabPage
{

    FixedLine    aAddrFL;
    FixedInfo    aAddrPosInfo;
    FixedText    aAddrLeftText;
    MetricField  aAddrLeftField;
    FixedText    aAddrTopText;
    MetricField  aAddrTopField;
    FixedInfo    aAddrFormatInfo;
    MenuButton   aAddrEditButton;
    FixedLine    aSendFL;
    FixedInfo    aSendPosInfo;
    FixedText    aSendLeftText;
    MetricField  aSendLeftField;
    FixedText    aSendTopText;
    MetricField  aSendTopField;
    FixedInfo    aSendFormatInfo;
    MenuButton   aSendEditButton;
    FixedLine    aSizeFL;
    FixedText    aSizeFormatText;
    ListBox      aSizeFormatBox;
    FixedText    aSizeWidthText;
    MetricField  aSizeWidthField;
    FixedText    aSizeHeightText;
    MetricField  aSizeHeightField;
    SwEnvPreview aPreview;

    std::vector<sal_uInt16>  aIDs;

     SwEnvFmtPage(Window* pParent, const SfxItemSet& rSet);
    ~SwEnvFmtPage();

    DECL_LINK( ModifyHdl, Edit * );
    DECL_LINK( EditHdl, MenuButton * );
    DECL_LINK(FormatHdl, void *);

    void SetMinMax();

    SfxItemSet  *GetCollItemSet(SwTxtFmtColl* pColl, sal_Bool bSender);

    SwEnvDlg    *GetParentSwEnvDlg() {return (SwEnvDlg*) GetParentDialog();}

    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

public:

    static SfxTabPage* Create(Window* pParent, const SfxItemSet& rSet);

    virtual void ActivatePage(const SfxItemSet& rSet);
    virtual int  DeactivatePage(SfxItemSet* pSet = 0);
            void FillItem(SwEnvItem& rItem);
    virtual sal_Bool FillItemSet(SfxItemSet& rSet);
    virtual void Reset(const SfxItemSet& rSet);
};

#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
