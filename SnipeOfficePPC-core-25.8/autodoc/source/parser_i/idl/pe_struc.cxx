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
#include <s2_luidl/pe_struc.hxx>


// NOT FULLY DECLARED SERVICES
#include <ary/idl/i_gate.hxx>
#include <ary/idl/i_struct.hxx>
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


PE_Struct::PE_Struct()
    // :    aWork,
    //      pStati
{
    pStati = new S_Stati(*this);
}

void
PE_Struct::EstablishContacts( UnoIDL_PE *               io_pParentPE,
                              ary::Repository &         io_rRepository,
                              TokenProcessing_Result &  o_rResult )
{
    UnoIDL_PE::EstablishContacts(io_pParentPE,io_rRepository,o_rResult);
    Work().pPE_Element->EstablishContacts(this,io_rRepository,o_rResult);
    Work().pPE_Type->EstablishContacts(this,io_rRepository,o_rResult);
}

PE_Struct::~PE_Struct()
{
}

void
PE_Struct::ProcessToken( const Token & i_rToken )
{
    i_rToken.Trigger(*Stati().pCurStatus);
}


void
PE_Struct::InitData()
{
    Work().InitData();
    Stati().pCurStatus = &Stati().aWaitForName;
}

void
PE_Struct::TransferData()
{
    if (NOT Work().bIsPreDeclaration)
    {
        csv_assert(Work().sData_Name.size() > 0);
        csv_assert(Work().nCurStruct.IsValid());
    }
    Stati().pCurStatus = &Stati().aNone;
}

void
PE_Struct::ReceiveData()
{
    Stati().pCurStatus->On_SubPE_Left();
}

PE_Struct::S_Work::S_Work()
    :   sData_Name(),
        sData_TemplateParam(),
        bIsPreDeclaration(false),
        nCurStruct(0),
        pPE_Element(0),
        nCurParsed_ElementRef(0),
        pPE_Type(0),
        nCurParsed_Base(0)

{
    pPE_Element = new PE_StructElement(nCurParsed_ElementRef,nCurStruct,sData_TemplateParam);
    pPE_Type = new PE_Type(nCurParsed_Base);
}

void
PE_Struct::S_Work::InitData()
{
    sData_Name.clear();
    sData_TemplateParam.clear();
    bIsPreDeclaration = false;
    nCurStruct = 0;
    nCurParsed_ElementRef = 0;
    nCurParsed_Base = 0;
}

void
PE_Struct::S_Work::Prepare_PE_QualifiedName()
{
    nCurParsed_ElementRef = 0;
}

void
PE_Struct::S_Work::Prepare_PE_Element()
{
    nCurParsed_Base = 0;
}

void
PE_Struct::S_Work::Data_Set_Name( const char * i_sName )
{
    sData_Name = i_sName;
}

void
PE_Struct::S_Work::Data_Set_TemplateParam( const char * i_sTemplateParam )
{
    sData_TemplateParam = i_sTemplateParam;
}

PE_Struct::S_Stati::S_Stati(PE_Struct & io_rStruct)
    :   aNone(io_rStruct),
        aWaitForName(io_rStruct),
        aGotName(io_rStruct),
        aWaitForTemplateParam(io_rStruct),
        aWaitForTemplateEnd(io_rStruct),
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
PE_Struct::PE_StructState::MyPE()
{
    return rStruct;
}


void
PE_Struct::State_WaitForName::Process_Identifier( const TokIdentifier & i_rToken )
{
    Work().Data_Set_Name(i_rToken.Text());
    MoveState( Stati().aGotName );
    SetResult(done,stay);
}

void
PE_Struct::State_GotName::Process_Punctuation( const TokPunctuation & i_rToken )
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
                PE().store_Struct();
                MoveState( Stati().aWaitForElement );
                SetResult(done,stay);
                break;
            case TokPunctuation::Lesser:
                MoveState( Stati().aWaitForTemplateParam );
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
PE_Struct::State_WaitForTemplateParam::Process_Identifier( const TokIdentifier & i_rToken )
{
    Work().Data_Set_TemplateParam(i_rToken.Text());
    MoveState( Stati().aWaitForTemplateEnd );
    SetResult(done,stay);
}

void
PE_Struct::State_WaitForTemplateEnd::Process_Punctuation( const TokPunctuation & )
{
    // Assume:  TokPunctuation::Greater
    MoveState( Stati().aGotName );
    SetResult(done,stay);
}

void
PE_Struct::State_WaitForBase::On_SubPE_Left()
{
    MoveState(Stati().aGotBase);
}

void
PE_Struct::State_GotBase::Process_Punctuation( const TokPunctuation & i_rToken )
{
    if ( i_rToken.Id() == TokPunctuation::CurledBracketOpen )
    {
        PE().store_Struct();
        MoveState( Stati().aWaitForElement );
        SetResult(done,stay);
    }
    else
    {
        SetResult(not_done,pop_failure);
    }
}

void
PE_Struct::State_WaitForElement::Process_Identifier( const TokIdentifier & )
{
    SetResult( not_done, push_sure, Work().pPE_Element.Ptr() );
    Work().Prepare_PE_Element();
}

void
PE_Struct::State_WaitForElement::Process_NameSeparator()
{
    SetResult( not_done, push_sure, Work().pPE_Element.Ptr());
    Work().Prepare_PE_Element();
}

void
PE_Struct::State_WaitForElement::Process_BuiltInType( const TokBuiltInType & )
{
    SetResult( not_done, push_sure, Work().pPE_Element.Ptr());
    Work().Prepare_PE_Element();
}

void
PE_Struct::State_WaitForElement::Process_TypeModifier(const TokTypeModifier & )
{
    SetResult( not_done, push_sure, Work().pPE_Element.Ptr());
    Work().Prepare_PE_Element();
}

void
PE_Struct::State_WaitForElement::Process_Punctuation( const TokPunctuation & i_rToken )
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
PE_Struct::State_WaitForFinish::Process_Punctuation( const TokPunctuation & i_rToken )
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
PE_Struct::store_Struct()
{
    ary::idl::Struct &
        rCe = Gate().Ces().Store_Struct(
                        CurNamespace().CeId(),
                        Work().sData_Name,
                        Work().nCurParsed_Base,
                        Work().sData_TemplateParam );
    PassDocuAt(rCe);
    Work().nCurStruct = rCe.CeId();
}


}   // namespace uidl
}   // namespace csi

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
