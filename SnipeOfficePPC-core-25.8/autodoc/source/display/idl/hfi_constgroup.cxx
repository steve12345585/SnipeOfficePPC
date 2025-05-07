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

#include <precomp.h>
#include "hfi_constgroup.hxx"


// NOT FULLY DEFINED SERVICES
#include <ary/idl/i_ce.hxx>
#include <ary/idl/ik_constgroup.hxx>
#include <toolkit/hf_linachain.hxx>
#include <toolkit/hf_navi_sub.hxx>
#include <toolkit/hf_title.hxx>
#include "hfi_navibar.hxx"
#include "hfi_property.hxx"
#include "hi_linkhelper.hxx"


extern const String
    C_sCePrefix_Constants("constants group");


namespace
{

const String
    C_sList_Constants("Constants");
const String
    C_sList_Constants_Label("Constants");
const String
    C_sList_ConstantDetails("Constants' Details");
const String
    C_sList_ConstantDetails_Label("ConstantDetails");

enum E_SubListIndices
{
    sli_ConstantsSummary = 0,
    sli_ConstantDetails = 1
};


}   // anonymous namespace



HF_IdlConstGroup::HF_IdlConstGroup( Environment &   io_rEnv,
                                    Xml::Element &  o_rOut )
    :   HtmlFactory_Idl(io_rEnv, &o_rOut)
{
}

HF_IdlConstGroup::~HF_IdlConstGroup()
{
}

void
HF_IdlConstGroup::Produce_byData( const client & i_ce ) const
{
    Dyn<HF_NaviSubRow>
        pNaviSubRow( &make_Navibar(i_ce) );

    HF_TitleTable
        aTitle(CurOut());
    HF_LinkedNameChain
        aNameChain(aTitle.Add_Row());

    aNameChain.Produce_CompleteChain(Env().CurPosition(), nameChainLinker);
    produce_Title(aTitle, C_sCePrefix_Constants, i_ce);

    write_Docu(aTitle.Add_Row(), i_ce);
    CurOut() << new Html::HorizontalLine();

    dyn_ce_list
        dpConstants;
    ary::idl::ifc_constgroup::attr::Get_Constants(dpConstants, i_ce);

    if ( (*dpConstants).operator bool() )
    {
        produce_Members( *dpConstants,
                         C_sList_Constants,
                         C_sList_Constants_Label,
                         C_sList_ConstantDetails,
                         C_sList_ConstantDetails_Label );
        pNaviSubRow->SwitchOn(sli_ConstantsSummary);
        pNaviSubRow->SwitchOn(sli_ConstantDetails);
    }
    pNaviSubRow->Produce_Row();
}

HF_NaviSubRow &
HF_IdlConstGroup::make_Navibar( const client & i_ce ) const
{
    HF_IdlNavigationBar
        aNaviBar(Env(), CurOut());
    aNaviBar.Produce_CeMainRow(i_ce,true);  // true := avoid link to Use-page.

    DYN HF_NaviSubRow &
        ret = aNaviBar.Add_SubRow();
    ret.AddItem(C_sList_Constants, C_sList_Constants_Label, false);
    ret.AddItem(C_sList_ConstantDetails, C_sList_ConstantDetails_Label, false);

    CurOut() << new Html::HorizontalLine();
    return ret;
}

void
HF_IdlConstGroup::produce_MemberDetails( HF_SubTitleTable &  o_table,
                                         const client &      i_ce ) const
{
    HF_IdlConstant
        aElement( Env(), o_table );
    aElement.Produce_byData(i_ce);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
