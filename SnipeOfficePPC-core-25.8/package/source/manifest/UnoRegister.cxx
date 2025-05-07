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

#include <ManifestReader.hxx>
#include <ManifestWriter.hxx>
#include <cppuhelper/factory.hxx>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include <osl/diagnose.h>
#include <ZipPackage.hxx>

#include <zipfileaccess.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::registry;
using namespace ::com::sun::star::packages;
using namespace ::com::sun::star::packages::manifest;

using rtl::OUString;

/**
 * This function is called to get service factories for an implementation.
 * @param pImplName name of implementation
 * @param pServiceManager generic uno interface providing a service manager to instantiate components
 * @param pRegistryKey registry data key to read and write component persistent data
 * @return a component factory (generic uno interface)
 */
extern "C" SAL_DLLPUBLIC_EXPORT void * SAL_CALL package2_component_getFactory(
    const sal_Char * pImplName, void * pServiceManager, void * /*pRegistryKey*/ )
{
    void * pRet = 0;
    uno::Reference< XMultiServiceFactory > xSMgr(
            reinterpret_cast< XMultiServiceFactory * >( pServiceManager ) );
    uno::Reference< XSingleServiceFactory > xFactory;

    if (ManifestReader::static_getImplementationName().compareToAscii( pImplName ) == 0)
        xFactory = ManifestReader::createServiceFactory ( xSMgr );
    else if (ManifestWriter::static_getImplementationName().compareToAscii( pImplName ) == 0)
        xFactory = ManifestWriter::createServiceFactory ( xSMgr );
    else if (ZipPackage::static_getImplementationName().compareToAscii( pImplName ) == 0)
        xFactory = ZipPackage::createServiceFactory ( xSMgr );
    else if ( OZipFileAccess::impl_staticGetImplementationName().compareToAscii( pImplName ) == 0 )
        xFactory = ::cppu::createSingleFactory( xSMgr,
                                           OZipFileAccess::impl_staticGetImplementationName(),
                                           OZipFileAccess::impl_staticCreateSelfInstance,
                                           OZipFileAccess::impl_staticGetSupportedServiceNames() );

    if ( xFactory.is() )
    {
        xFactory->acquire();
        pRet = xFactory.get();
    }
    return pRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
