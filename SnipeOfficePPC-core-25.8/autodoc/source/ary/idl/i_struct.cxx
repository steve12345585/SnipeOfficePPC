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
#include <ary/idl/i_struct.hxx>
#include <ary/idl/ik_struct.hxx>


// NOT FULLY DECLARED SERVICES
#include <cosv/tpl/processor.hxx>
#include <sci_impl.hxx>
#include "i2s_calculator.hxx"


namespace ary
{
namespace idl
{

Struct::Struct( const String &      i_sName,
                Ce_id               i_nOwner,
                Type_id             i_nBase,
                const String &      i_sTemplateParameter,
                Type_id             i_nTemplateParameterType )
    :   sName(i_sName),
        nOwner(i_nOwner),
        nBase(i_nBase),
        sTemplateParameter(i_sTemplateParameter),
        nTemplateParameterType(i_nTemplateParameterType),
        aElements()
{
}

Struct::~Struct()
{
}

void
Struct::do_Accept( csv::ProcessorIfc & io_processor ) const
{
    csv::CheckedCall(io_processor, *this);
}

ClassId
Struct::get_AryClass() const
{
    return class_id;
}

const String &
Struct::inq_LocalName() const
{
    return sName;
}

Ce_id
Struct::inq_NameRoom() const
{
    return nOwner;
}

Ce_id
Struct::inq_Owner() const
{
    return nOwner;
}

E_SightLevel
Struct::inq_SightLevel() const
{
    return sl_File;
}


namespace ifc_struct
{

inline const Struct &
struct_cast( const CodeEntity &  i_ce )
{
    csv_assert( i_ce.AryClass() == Struct::class_id );
    return static_cast< const Struct& >(i_ce);
}

Type_id
attr::Base( const CodeEntity & i_ce )
{
    return struct_cast(i_ce).nBase;
}

void
attr::Get_Elements( Dyn_CeIterator &    o_result,
                    const CodeEntity &  i_ce )
{
    o_result = new SCI_Vector<Ce_id>( struct_cast(i_ce).aElements );
}


void
xref::Get_Derivations( Dyn_CeIterator &    o_result,
                       const CodeEntity &  i_ce )
{
    o_result = new SCI_Vector<Ce_id>(i_ce.Secondaries().List(struct_2s_Derivations));
}

void
xref::Get_SynonymTypedefs( Dyn_CeIterator &    o_result,
                           const CodeEntity &  i_ce )
{
    o_result = new SCI_Vector<Ce_id>(i_ce.Secondaries().List(struct_2s_SynonymTypedefs));
}

void
xref::Get_AsReturns( Dyn_CeIterator &    o_result,
                     const CodeEntity &  i_ce )
{
    o_result = new SCI_Vector<Ce_id>(i_ce.Secondaries().List(struct_2s_AsReturns));
}

void
xref::Get_AsParameters( Dyn_CeIterator &    o_result,
                        const CodeEntity &  i_ce )
{
    o_result = new SCI_Vector<Ce_id>(i_ce.Secondaries().List(struct_2s_AsParameters));
}

void
xref::Get_AsDataTypes( Dyn_CeIterator &    o_result,
                       const CodeEntity &  i_ce )
{
    o_result = new SCI_Vector<Ce_id>(i_ce.Secondaries().List(struct_2s_AsDataTypes));
}

} // namespace ifc_struct



}   //  namespace   idl
}   //  namespace   ary

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
