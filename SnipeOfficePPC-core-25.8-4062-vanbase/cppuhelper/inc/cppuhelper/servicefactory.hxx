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
#ifndef _CPPUHELPER_SERVICEFACTORY_HXX_
#define _CPPUHELPER_SERVICEFACTORY_HXX_

#include <rtl/ustring.hxx>

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include "cppuhelperdllapi.h"


namespace cppu
{

/** Deprecated.  Use cppuhelper/bootstrap.hxx functions instead.

    This bootstraps an initial service factory working on a registry. If the first or both
    parameters contain a value then the service factory is initialized with a simple registry
    or a nested registry. Otherwise the service factory must be initialized later with a valid
    registry.

    @param rWriteRegistryFile
    file name of the simple registry or the first registry file of
    the nested registry which will be opened with read/write rights. This
    file will be created if necessary.
    @param rReadRegistryFile
    file name of the second registry file of the nested registry
    which will be opened with readonly rights.
    @param bReadOnly
    flag which specify that the first registry file will be opened with
    readonly rights. Default is FALSE. If this flag is used the registry
    will not be created if not exist.
    @param rBootstrapPath
    specifies a path where the bootstrap components could be find. If this
    parameter is an empty string the compoents will be searched in the system
    path.
    @deprecated
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
SAL_CALL createRegistryServiceFactory(
    const ::rtl::OUString & rWriteRegistryFile,
    const ::rtl::OUString & rReadRegistryFile,
    sal_Bool bReadOnly = sal_False,
    const ::rtl::OUString & rBootstrapPath = ::rtl::OUString() )
    SAL_THROW( (::com::sun::star::uno::Exception) );


/** Deprecated.  Use cppuhelper/bootstrap.hxx functions instead.

    This bootstraps an initial service factory working on a registry file.

    @param rRegistryFile
    file name of the registry to use/ create; if this is an empty
    string, the default registry is used instead
    @param bReadOnly
    flag which specify that the registry file will be opened with
    readonly rights. Default is FALSE. If this flag is used the registry
    will not be created if not exist.
    @param rBootstrapPath
    specifies a path where the bootstrap components could be find. If this
    parameter is an empty string the compoents will be searched in the system
    path.
    @deprecated
*/
inline ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
SAL_CALL createRegistryServiceFactory(
    const ::rtl::OUString & rRegistryFile,
    sal_Bool bReadOnly = sal_False,
    const ::rtl::OUString & rBootstrapPath = ::rtl::OUString() )
    SAL_THROW( (::com::sun::star::uno::Exception) )
{
    return ::cppu::createRegistryServiceFactory(
        rRegistryFile, ::rtl::OUString(), bReadOnly, rBootstrapPath );
}

/** Deprecated.  Use cppuhelper/bootstrap.hxx functions instead.

    This bootstraps a service factory without initialize a registry.

    @param rBootstrapPath
    specifies a path where the bootstrap components could be find. If this
    parameter is an empty string the compoents will be searched in the system
    path.
    @deprecated
*/
inline ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
SAL_CALL createServiceFactory(
    const ::rtl::OUString & rBootstrapPath = ::rtl::OUString() )
    SAL_THROW( (::com::sun::star::uno::Exception) )
{
    return ::cppu::createRegistryServiceFactory(
        ::rtl::OUString(), ::rtl::OUString(), sal_False, rBootstrapPath );
}

} // end namespace cppu

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
