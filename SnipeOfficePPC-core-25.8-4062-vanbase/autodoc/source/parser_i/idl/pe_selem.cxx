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

#include <precomp.h>
#include <s2_luidl/pe_selem.hxx>

// NOT FULLY DECLARED SERVICES
#include <ary/idl/i_gate.hxx>
#include <ary/idl/i_struct.hxx>
#include <ary/idl/i_structelem.hxx>
#include <ary/idl/ip_ce.hxx>
#include <ary/doc/d_oldidldocu.hxx>
#include <s2_luidl/pe_type2.hxx>
#include <s2_luidl/tk_ident.hxx>
#include <s2_luidl/tk_punct.hxx>


namespace csi
{
namespace uidl
{

namespace
{
    const String  C_sNone;
}

PE_StructElement::PE_StructElement( RStructElement &    o_rResult,
                                    const RStruct &     i_rCurStruct,
                                    const String &      i_rCurStructTemplateParam )
    :   eState(e_none),
        pResult(&o_rResult),
        pCurStruct(&i_rCurStruct),
        bIsExceptionElement(false),
        pPE_Type(0),
        nType(0),
        sName(),
        pCurStructTemplateParam(&i_rCurStructTemplateParam)
{
    pPE_Type = new PE_Type(nType);
}

PE_StructElement::PE_StructElement( RStructElement &    o_rResult,
                                    const RStruct &     i_rCurExc )
    :   eState(e_none),
        pResult(&o_rResult),
        pCurStruct(&i_rCurExc),
        bIsExceptionElement(true),
        pPE_Type(0),
        nType(0),
        sName(),
        pCurStructTemplateParam(&C_sNone)
{
    pPE_Type = new PE_Type(nType);
}

void
PE_StructElement::EstablishContacts( UnoIDL_PE *                io_pParentPE,
                                     ary::Repository &      io_rRepository,
                                     TokenProcessing_Result &   o_rResult )
{
    UnoIDL_PE::EstablishContacts(io_pParentPE,io_rRepository,o_rResult);
    pPE_Type->EstablishContacts(this,io_rRepository,o_rResult);
}

PE_StructElement::~PE_StructElement()
{
}

void
PE_StructElement::ProcessToken( const Token & i_rToken )
{
    i_rToken.Trigger(*this);
}

void
PE_StructElement::Process_Default()
{
    if (eState == expect_type)
    {
        SetResult( not_done, push_sure, pPE_Type.Ptr() );
        eState = expect_name;
    }
    else {
        csv_assert(false);
    }
}

void
PE_StructElement::Process_Identifier( const TokIdentifier & i_rToken )
{
    csv_assert(*i_rToken.Text() != 0);

    if (eState == expect_type)
    {
        if ( *pCurStructTemplateParam == i_rToken.Text() )
        {
            nType = lhf_FindTemplateParamType();
               SetResult( done, stay );
            eState = expect_name;
        }
        else    // No template parameter type existing, or not matching:
        {
               SetResult( not_done, push_sure, pPE_Type.Ptr() );
            eState = expect_name;
        }
    }
    else if (eState == expect_name)
    {
        sName = i_rToken.Text();
        SetResult( done, stay );
        eState = expect_finish;
    }
    else {
        csv_assert(false);
    }
}

void
PE_StructElement::Process_Punctuation( const TokPunctuation &)
{
    csv_assert(eState == expect_finish);

    SetResult( done, pop_success );
}

void
PE_StructElement::InitData()
{
    eState = expect_type;

    nType = 0;
    sName = "";
}

void
PE_StructElement::TransferData()
{
    csv_assert(pResult != 0 AND pCurStruct != 0);

    ary::idl::StructElement *
        pCe = 0;
    if (bIsExceptionElement)
    {
        pCe = & Gate().Ces().Store_ExceptionMember(
                                            *pCurStruct,
                                            sName,
                                            nType );
    }
    else
    {
        pCe = & Gate().Ces().Store_StructMember(
                                            *pCurStruct,
                                            sName,
                                            nType );
    }
    *pResult = pCe->CeId();
    PassDocuAt(*pCe);

    eState = e_none;
}

UnoIDL_PE &
PE_StructElement::MyPE()
{
    return *this;
}

ary::idl::Type_id
PE_StructElement::lhf_FindTemplateParamType() const
{
    const ary::idl::CodeEntity &
        rCe = Gate().Ces().Find_Ce(*pCurStruct);
    const ary::idl::Struct &
        rStruct = static_cast< const ary::idl::Struct& >(rCe);
    return rStruct.TemplateParameterType();
}


}   // namespace uidl
}   // namespace csi

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
