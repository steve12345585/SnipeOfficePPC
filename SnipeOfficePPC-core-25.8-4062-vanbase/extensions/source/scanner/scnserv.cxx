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

#include <osl/diagnose.h>
#include <cppuhelper/factory.hxx>
#include <uno/mapping.hxx>
#include "scanner.hxx"

#include <com/sun/star/registry/XRegistryKey.hpp>

using namespace com::sun::star::registry;


extern "C" SAL_DLLPUBLIC_EXPORT void* SAL_CALL scn_component_getFactory( const sal_Char* pImplName, void* pServiceManager, void* /*pRegistryKey*/ )
{
    REF( ::com::sun::star::lang::XSingleServiceFactory ) xFactory;
    void*                                                pRet = 0;

    if( ::rtl::OUString::createFromAscii( pImplName ) == ScannerManager::getImplementationName_Static() )
    {
        xFactory = REF( ::com::sun::star::lang::XSingleServiceFactory )( ::cppu::createSingleFactory(
                        static_cast< ::com::sun::star::lang::XMultiServiceFactory* >( pServiceManager ),
                        ScannerManager::getImplementationName_Static(),
                        ScannerManager_CreateInstance,
                        ScannerManager::getSupportedServiceNames_Static() ) );
    }

    if( xFactory.is() )
    {
        xFactory->acquire();
        pRet = xFactory.get();
    }

    return pRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
