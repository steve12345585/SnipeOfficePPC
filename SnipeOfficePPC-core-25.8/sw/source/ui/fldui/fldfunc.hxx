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
#ifndef _SWFLDFUNC_HXX
#define _SWFLDFUNC_HXX

#include <sfx2/tabdlg.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/button.hxx>
#include <vcl/group.hxx>
#include <vcl/edit.hxx>

#include "condedit.hxx"
#include "fldpage.hxx"
#include <actctrl.hxx>

class SwFldFuncPage : public SwFldPage
{
    FixedText       aTypeFT;
    ListBox         aTypeLB;
    FixedText       aSelectionFT;
    ListBox         aSelectionLB;
    FixedText       aFormatFT;
    ListBox         aFormatLB;
    FixedText       aNameFT;
    ConditionEdit   aNameED;
    FixedText       aValueFT;
    Edit            aValueED;
    FixedText       aCond1FT;
    ConditionEdit   aCond1ED;
    FixedText       aCond2FT;
    ConditionEdit   aCond2ED;
    PushButton      aMacroBT;

    //controls of "Input list"
    FixedText       aListItemFT;
    ReturnActionEdit aListItemED;
    PushButton      aListAddPB;
    FixedText       aListItemsFT;
    ListBox         aListItemsLB;
    PushButton      aListRemovePB;
    PushButton      aListUpPB;
    PushButton      aListDownPB;
    FixedText       aListNameFT;
    Edit            aListNameED;

    String          sOldValueFT;
    String          sOldNameFT;

    sal_uLong           nOldFormat;
    bool            bDropDownLBChanged;

    DECL_LINK(TypeHdl, void *);
    DECL_LINK(SelectHdl, void * = 0);
    DECL_LINK(InsertMacroHdl, void *);
    DECL_LINK(ModifyHdl, void * = 0);
    DECL_LINK( ListModifyHdl, Control*);
    DECL_LINK( ListEnableHdl, void*);

    // select Macro
    DECL_LINK( MacroHdl, Button * );

    void                UpdateSubType();
    String              TurnMacroString(const String &rMacro);

protected:
    virtual sal_uInt16      GetGroup();

public:
                        SwFldFuncPage(Window* pParent, const SfxItemSet& rSet);

                        ~SwFldFuncPage();

    static SfxTabPage*  Create(Window* pParent, const SfxItemSet& rAttrSet);

    virtual sal_Bool        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );

    virtual void        FillUserData();
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
