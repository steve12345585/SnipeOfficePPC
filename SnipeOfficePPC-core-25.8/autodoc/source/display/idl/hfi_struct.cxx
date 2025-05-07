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
#include "hfi_struct.hxx"


// NOT FULLY DEFINED SERVICES
#include <ary/idl/i_ce.hxx>
#include <ary/idl/i_struct.hxx>
#include <ary/idl/ik_exception.hxx>
#include <ary/idl/ik_struct.hxx>
#include <toolkit/hf_docentry.hxx>
#include <toolkit/hf_linachain.hxx>
#include <toolkit/hf_navi_sub.hxx>
#include <toolkit/hf_title.hxx>
#include "hfi_navibar.hxx"
#include "hfi_property.hxx"
#include "hfi_typetext.hxx"
#include "hi_linkhelper.hxx"


extern const String
    C_sCePrefix_Struct("struct");
extern const String
    C_sCePrefix_Exception("exception");


namespace
{

const String
    C_sBaseStruct("Base Hierarchy");
const String
    C_sBaseException("Base Hierarchy");

const String
    C_sList_Elements("Elements' Summary");
const String
    C_sList_Elements_Label("Elements");

const String
    C_sList_ElementDetails("Elements' Details");
const String
    C_sList_ElementDetails_Label("ElementDetails");

enum E_SubListIndices
{
    sli_ElementsSummary = 0,
    sli_ElementsDetails = 1
};

}   // anonymous namespace



HF_IdlStruct::HF_IdlStruct( Environment &         io_rEnv,
                            Xml::Element &        o_rOut,
                            bool                  i_bIsException )
    :   HtmlFactory_Idl(io_rEnv, &o_rOut),
        bIsException(i_bIsException)
{
}

HF_IdlStruct::~HF_IdlStruct()
{
}

void
HF_IdlStruct::Produce_byData( const client & i_ce ) const
{
    const ary::idl::Struct *
        pStruct =
            bIsException
                ?   0
                :   static_cast< const ary::idl::Struct* >(&i_ce);
    bool bIsTemplate =
            pStruct != 0
                ?   pStruct->TemplateParameterType().IsValid()
                :   false;

    Dyn<HF_NaviSubRow>
        pNaviSubRow( &make_Navibar(i_ce) );

    HF_TitleTable
        aTitle(CurOut());
    HF_LinkedNameChain
        aNameChain(aTitle.Add_Row());

    aNameChain.Produce_CompleteChain(Env().CurPosition(), nameChainLinker);

    // Title:
    StreamLock
        slAnnotations(200);
    get_Annotations(slAnnotations(), i_ce);

    StreamLock rTitle(200);
    if (bIsTemplate)
        rTitle() << "template ";
    rTitle()
        << (bIsException
            ?   C_sCePrefix_Exception
            :   C_sCePrefix_Struct)
        << " "
        << i_ce.LocalName();
    if (bIsTemplate)
    {
        csv_assert(pStruct != 0);
        rTitle()
            << "<"
            << pStruct->TemplateParameter()
            << ">";
    }
    aTitle.Produce_Title(slAnnotations().c_str(), rTitle().c_str());

    // Bases:
    produce_Bases( aTitle.Add_Row(),
                   i_ce,
                   bIsException
                    ?   C_sBaseException
                    :   C_sBaseStruct );

    // Docu:
    write_Docu(aTitle.Add_Row(), i_ce);
    CurOut() << new Html::HorizontalLine();

    // Elements:
    dyn_ce_list
        dpElements;
    if (bIsException)
        ary::idl::ifc_exception::attr::Get_Elements(dpElements, i_ce);
    else
        ary::idl::ifc_struct::attr::Get_Elements(dpElements, i_ce);

    if ( (*dpElements).operator bool() )
    {
        produce_Members( *dpElements,
                         C_sList_Elements,
                         C_sList_Elements_Label,
                         C_sList_ElementDetails,
                         C_sList_ElementDetails_Label );
        pNaviSubRow->SwitchOn(sli_ElementsSummary);
        pNaviSubRow->SwitchOn(sli_ElementsDetails);
    }
    pNaviSubRow->Produce_Row();
}

HtmlFactory_Idl::type_id
HF_IdlStruct::inq_BaseOf( const client & i_ce ) const
{
    return bIsException
                ?   ary::idl::ifc_exception::attr::Base(i_ce)
                :   ary::idl::ifc_struct::attr::Base(i_ce);
}

HF_NaviSubRow &
HF_IdlStruct::make_Navibar( const client & i_ce ) const
{
    HF_IdlNavigationBar
        aNaviBar(Env(), CurOut());
    aNaviBar.Produce_CeMainRow(i_ce);

    DYN HF_NaviSubRow &
        ret = aNaviBar.Add_SubRow();
    ret.AddItem(C_sList_Elements, C_sList_Elements_Label, false);
    ret.AddItem(C_sList_ElementDetails, C_sList_ElementDetails_Label, false);

    CurOut() << new Html::HorizontalLine();
    return ret;
}

void
HF_IdlStruct::produce_MemberDetails( HF_SubTitleTable &  o_table,
                                     const client &      i_ce) const
{
    HF_IdlStructElement
        aElement( Env(), o_table );
    aElement.Produce_byData(i_ce);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
