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
#ifndef _NUMPARA_HXX
#define _NUMPARA_HXX


#include <sfx2/tabdlg.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/fixed.hxx>
#include <vcl/field.hxx>
#include <vcl/button.hxx>


/* --- class SwParagraphNumTabPage ----------------------------------------
    with this TabPage numbering settings at the paragraph / paragraph style
    are performed.

 ---------------------------------------------------------------------------*/
class SwParagraphNumTabPage : public SfxTabPage
{
    FixedLine               aOutlineStartFL;
    FixedText               aOutlineLvFT;
    ListBox                 aOutlineLvLB;

    FixedLine               aNewStartFL;
    FixedText               aNumberStyleFT;
    ListBox                 aNumberStyleLB;

    TriStateBox             aNewStartCB;
    TriStateBox             aNewStartNumberCB;
    NumericField            aNewStartNF;

    FixedLine               aCountParaFL;
    TriStateBox             aCountParaCB;
    TriStateBox             aRestartParaCountCB;
    FixedText               aRestartFT;
    NumericField            aRestartNF;

    // --> OD 2008-04-14 #outlinelevel#
    const String msOutlineNumbering;

    sal_Bool                    bModified : 1;
    sal_Bool                    bCurNumrule : 1;

    DECL_LINK(NewStartHdl_Impl, void *);
    DECL_LINK( StyleHdl_Impl, ListBox* );
    DECL_LINK(LineCountHdl_Impl, void *);

protected:
        SwParagraphNumTabPage(Window* pParent, const SfxItemSet& rSet );

public:
        ~SwParagraphNumTabPage();

    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rSet );
    static sal_uInt16*      GetRanges();

    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );

    void                EnableNewStart();
    void                DisableOutline();
    void                DisableNumbering();

    ListBox&            GetStyleBox() {return aNumberStyleLB;};
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
