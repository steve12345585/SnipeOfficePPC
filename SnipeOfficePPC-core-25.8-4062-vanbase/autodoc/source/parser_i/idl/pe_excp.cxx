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
#include <s2_luidl/pe_excp.hxx>


// NOT FULLY DECLARED SERVICES
#include <ary/idl/i_exception.hxx>
#include <ary/idl/i_gate.hxx>
#include <ary/idl/i_structelem.hxx>
#include <ary/idl/ip_ce.hxx>
#include <ary/doc/d_oldidldocu.hxx>
#include <s2_luidl/tk_ident.hxx>
#include <s2_luidl/tk_punct.hxx>
#include <s2_luidl/tk_keyw.hxx>
#include <s2_luidl/pe_type2.hxx>
#include <s2_luidl/pe_selem.hxx>



namespace csi
{
namespace uidl
{


PE_Exception::PE_Exception()
    // :    aWork,
    //      pStati
{
    pStati = new S_Stati(*this);
}

void
PE_Exception::EstablishContacts( UnoIDL_PE *                io_pParentPE,
                              ary::Repository &         io_rRepository,
                              TokenProcessing_Result &  o_rResult )
{
    UnoIDL_PE::EstablishContacts(io_pParentPE,io_rRepository,o_rResult);
    Work().pPE_Element->EstablishContacts(this,io_rRepository,o_rResult);
    Work().pPE_Type->EstablishContacts(this,io_rRepository,o_rResult);
}

PE_Exception::~PE_Exception()
{
}

void
PE_Exception::ProcessToken( const Token & i_rToken )
{
    i_rToken.Trigger(*Stati().pCurStatus);
}


void
PE_Exception::InitData()
{
    Work().InitData();
    Stati().pCurStatus = &Stati().aWaitForName;
}

void
PE_Exception::TransferData()
{
    if (NOT Work().bIsPreDeclaration)
    {
        csv_assert(Work().sData_Name.size() > 0);
        csv_assert(Work().nCurStruct.IsValid());
    }
    Stati().pCurStatus = &Stati().aNone;
}

void
PE_Exception::ReceiveData()
{
    Stati().pCurStatus->On_SubPE_Left();
}

PE_Exception::S_Work::S_Work()
    :   sData_Name(),
        bIsPreDeclaration(false),
        nCurStruct(0),
        pPE_Element(0),
        nCurParsed_ElementRef(0),
        pPE_Type(0),
        nCurParsed_Base(0)

{
    pPE_Element = new PE_StructElement(nCurParsed_ElementRef,nCurStruct);
    pPE_Type = new PE_Type(nCurParsed_Base);
}

void
PE_Exception::S_Work::InitData()
{
    sData_Name.clear();
    bIsPreDeclaration = false;
    nCurStruct = 0;

    nCurParsed_ElementRef = 0;
    nCurParsed_Base = 0;
}

void
PE_Exception::S_Work::Prepare_PE_QualifiedName()
{
    nCurParsed_ElementRef = 0;
}

void
PE_Exception::S_Work::Prepare_PE_Element()
{
    nCurParsed_Base = 0;
}

void
PE_Exception::S_Work::Data_Set_Name( const char * i_sName )
{
    sData_Name = i_sName;
}

PE_Exception::S_Stati::S_Stati(PE_Exception & io_rStruct)
    :   aNone(io_rStruct),
        aWaitForName(io_rStruct),
        aGotName(io_rStruct),
        aWaitForBase(io_rStruct),
        aGotBase(io_rStruct),
        aWaitForElement(io_rStruct),
        aWaitForFinish(io_rStruct),
        pCurStatus(0)
{
    pCurStatus = &aNone;
}


//***********************       Stati       ***************************//


UnoIDL_PE &
PE_Exception::PE_StructState::MyPE()
{
    return rStruct;
}


void
PE_Exception::State_WaitForName::Process_Identifier( const TokIdentifier & i_rToken )
{
    Work().Data_Set_Name(i_rToken.Text());
    MoveState( Stati().aGotName );
    SetResult(done,stay);
}

void
PE_Exception::State_GotName::Process_Punctuation( const TokPunctuation & i_rToken )
{
    if ( i_rToken.Id() != TokPunctuation::Semicolon )
    {
        switch (i_rToken.Id())
        {
            case TokPunctuation::Colon:
                MoveState( Stati().aWaitForBase );
                SetResult(done,push_sure,Work().pPE_Type.Ptr());
                Work().Prepare_PE_QualifiedName();
                break;
            case TokPunctuation::CurledBracketOpen:
                PE().store_Exception();
                MoveState( Stati().aWaitForElement );
                SetResult(done,stay);
                break;
            default:
                SetResult(not_done,pop_failure);
        }   // end switch
    }
    else
    {
        Work().sData_Name.clear();
        SetResult(done,pop_success);
    }
}

void
PE_Exception::State_WaitForBase::On_SubPE_Left()
{
    MoveState(Stati().aGotBase);
}

void
PE_Exception::State_GotBase::Process_Punctuation( const TokPunctuation & i_rToken )
{
    if ( i_rToken.Id() == TokPunctuation::CurledBracketOpen )
    {
        PE().store_Exception();
        MoveState( Stati().aWaitForElement );
        SetResult(done,stay);
    }
    else
    {
        SetResult(not_done,pop_failure);
    }
}

void
PE_Exception::State_WaitForElement::Process_Identifier( const TokIdentifier & )
{
    SetResult( not_done, push_sure, Work().pPE_Element.Ptr() );
    Work().Prepare_PE_Element();
}

void
PE_Exception::State_WaitForElement::Process_NameSeparator()
{
    SetResult( not_done, push_sure, Work().pPE_Element.Ptr());
    Work().Prepare_PE_Element();
}

void
PE_Exception::State_WaitForElement::Process_BuiltInType( const TokBuiltInType & )
{
    SetResult( not_done, push_sure, Work().pPE_Element.Ptr());
    Work().Prepare_PE_Element();
}

void
PE_Exception::State_WaitForElement::Process_TypeModifier(const TokTypeModifier & )
{
    SetResult( not_done, push_sure, Work().pPE_Element.Ptr());
    Work().Prepare_PE_Element();
}

void
PE_Exception::State_WaitForElement::Process_Punctuation( const TokPunctuation & i_rToken )
{
    if ( i_rToken.Id() == TokPunctuation::CurledBracketClose )
    {
        MoveState( Stati().aWaitForFinish );
        SetResult( done, stay );
    }
    else
    {
        SetResult( not_done, pop_failure );
    }
}

void
PE_Exception::State_WaitForFinish::Process_Punctuation( const TokPunctuation & i_rToken )
{
    if (i_rToken.Id() == TokPunctuation::Semicolon)
    {
        MoveState( Stati().aNone );
        SetResult( done, pop_success );
    }
    else
    {
        SetResult( not_done, pop_failure );
    }
}

void
PE_Exception::store_Exception()
{
    ary::idl::Exception &
        rCe = Gate().Ces().Store_Exception(
                            CurNamespace().CeId(),
                            Work().sData_Name,
                            Work().nCurParsed_Base );
    PassDocuAt(rCe);
    Work().nCurStruct = rCe.Id();
}


}   // namespace uidl
}   // namespace csi

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
