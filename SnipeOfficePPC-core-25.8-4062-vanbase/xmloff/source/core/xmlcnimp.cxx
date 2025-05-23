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

#include "SvXMLAttrCollection.hxx"
#include <xmloff/xmlcnimp.hxx>
#include <rtl/ustring.hxx>

SvXMLAttrContainerData::SvXMLAttrContainerData() : pimpl( new SvXMLAttrCollection() )
{
}

SvXMLAttrContainerData::SvXMLAttrContainerData(const SvXMLAttrContainerData &rCopy) :
        pimpl( new SvXMLAttrCollection( *(rCopy.pimpl) ) )
{
}

// Need destructor defined (despite it being empty) to avoid "checked_delete"
// compiler errors.
SvXMLAttrContainerData::~SvXMLAttrContainerData()
{
}

int SvXMLAttrContainerData::operator ==( const SvXMLAttrContainerData& rCmp ) const
{
    return ( *(rCmp.pimpl) == *(pimpl) );
}

sal_Bool SvXMLAttrContainerData::AddAttr( const rtl::OUString& rLName,
                                          const rtl::OUString& rValue )
{
    return pimpl->AddAttr(rLName, rValue);
}

sal_Bool SvXMLAttrContainerData::AddAttr( const rtl::OUString& rPrefix,
                                          const rtl::OUString& rNamespace,
                                          const rtl::OUString& rLName,
                                          const rtl::OUString& rValue )
{
    return pimpl->AddAttr(rPrefix, rNamespace, rLName, rValue);
}

sal_Bool SvXMLAttrContainerData::AddAttr( const rtl::OUString& rPrefix,
                                          const rtl::OUString& rLName,
                                          const rtl::OUString& rValue )
{
    return pimpl->AddAttr(rPrefix, rLName, rValue);
}

sal_Bool SvXMLAttrContainerData::SetAt( size_t i,
                                        const rtl::OUString& rLName,
                                        const rtl::OUString& rValue )
{
    return pimpl->SetAt(i, rLName, rValue);
}

sal_Bool SvXMLAttrContainerData::SetAt( size_t i,
                                        const rtl::OUString& rPrefix,
                                        const rtl::OUString& rNamespace,
                                        const rtl::OUString& rLName,
                                        const rtl::OUString& rValue )
{
    return pimpl->SetAt(i, rPrefix, rNamespace, rLName, rValue);
}

sal_Bool SvXMLAttrContainerData::SetAt( size_t i,
                                        const rtl::OUString& rPrefix,
                                        const rtl::OUString& rLName,
                                        const rtl::OUString& rValue )
{
    return pimpl->SetAt(i, rPrefix, rLName, rValue);
}

void SvXMLAttrContainerData::Remove( size_t i )
{
    pimpl->Remove(i);
}

size_t SvXMLAttrContainerData::GetAttrCount() const
{
    return pimpl->GetAttrCount();
}

const rtl::OUString& SvXMLAttrContainerData::GetAttrLName(size_t i) const
{
    return pimpl->GetAttrLName(i);
}

const rtl::OUString& SvXMLAttrContainerData::GetAttrValue(size_t i) const
{
    return pimpl->GetAttrValue(i);
}

const rtl::OUString SvXMLAttrContainerData::GetAttrNamespace( size_t i ) const
{
    return pimpl->GetAttrNamespace(i);
}

const rtl::OUString SvXMLAttrContainerData::GetAttrPrefix( size_t i ) const
{
    return pimpl->GetAttrPrefix(i);
}

const rtl::OUString& SvXMLAttrContainerData::GetNamespace( sal_uInt16 i ) const
{
    return pimpl->GetNamespace(i);
}

const rtl::OUString& SvXMLAttrContainerData::GetPrefix( sal_uInt16 i ) const
{
    return pimpl->GetPrefix(i);
}

sal_uInt16 SvXMLAttrContainerData::GetFirstNamespaceIndex() const
{
    return pimpl->GetFirstNamespaceIndex();
}

sal_uInt16 SvXMLAttrContainerData::GetNextNamespaceIndex( sal_uInt16 nIdx ) const
{
    return pimpl->GetNextNamespaceIndex( nIdx );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
