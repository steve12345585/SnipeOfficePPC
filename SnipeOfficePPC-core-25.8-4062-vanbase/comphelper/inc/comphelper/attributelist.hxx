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

#ifndef _COMPHELPER_ATTRIBUTE_LIST_HXX
#define _COMPHELPER_ATTRIBUTE_LIST_HXX

#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <cppuhelper/implbase1.hxx>
#include <comphelper/comphelperdllapi.h>


namespace comphelper
{

struct AttributeList_Impl;

class COMPHELPER_DLLPUBLIC AttributeList : public ::cppu::WeakImplHelper1
<
    ::com::sun::star::xml::sax::XAttributeList
>
{
    AttributeList_Impl *m_pImpl;
public:
    AttributeList();
    virtual ~AttributeList();

    // methods that are not contained in any interface
    void AddAttribute( const ::rtl::OUString &sName , const ::rtl::OUString &sType , const ::rtl::OUString &sValue );

    // ::com::sun::star::xml::sax::XAttributeList
    virtual sal_Int16 SAL_CALL getLength(void)
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL getNameByIndex(sal_Int16 i)
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL getTypeByIndex(sal_Int16 i)
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL getTypeByName(const ::rtl::OUString& aName)
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL getValueByIndex(sal_Int16 i)
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL getValueByName(const ::rtl::OUString& aName)
        throw( ::com::sun::star::uno::RuntimeException );

};

} // namespace comphelper

#endif // _COMPHELPER_ATTRIBUTE_LIST_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
