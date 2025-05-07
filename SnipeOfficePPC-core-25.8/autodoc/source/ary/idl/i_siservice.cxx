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
#include <ary/idl/i_siservice.hxx>
#include <ary/idl/ik_siservice.hxx>


// NOT FULLY DECLARED SERVICES
#include <cosv/tpl/processor.hxx>
#include <sci_impl.hxx>


namespace ary
{
namespace idl
{

SglIfcService::SglIfcService( const String &      i_sName,
                              Ce_id               i_nOwner,
                              Type_id             i_nBaseInterface )
    :   sName(i_sName),
        nOwner(i_nOwner),
        nBaseInterface(i_nBaseInterface),
        aConstructors()
{
}

SglIfcService::~SglIfcService()
{
}

void
SglIfcService::do_Accept( csv::ProcessorIfc & io_processor ) const
{
    csv::CheckedCall(io_processor, *this);
}

ClassId
SglIfcService::get_AryClass() const
{
    return class_id;
}

const String &
SglIfcService::inq_LocalName() const
{
    return sName;
}

Ce_id
SglIfcService::inq_NameRoom() const
{
    return nOwner;
}

Ce_id
SglIfcService::inq_Owner() const
{
    return nOwner;
}

E_SightLevel
SglIfcService::inq_SightLevel() const
{
    return sl_File;
}


namespace ifc_sglifcservice
{

inline const SglIfcService &
sglifcservice_cast( const CodeEntity &  i_ce )
{
    csv_assert( i_ce.AryClass() == SglIfcService::class_id );
    return static_cast< const SglIfcService& >(i_ce);
}

Type_id
attr::BaseInterface( const CodeEntity & i_ce )
{
    return sglifcservice_cast(i_ce).nBaseInterface;
}

void
attr::Get_Constructors( Dyn_CeIterator &    o_result,
                        const CodeEntity &  i_ce )
{
    o_result = new SCI_Vector<Ce_id>( sglifcservice_cast(i_ce).aConstructors );
}

} // namespace ifc_sglifcservice


}   //  namespace   idl
}   //  namespace   ary

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
