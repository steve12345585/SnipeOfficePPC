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
