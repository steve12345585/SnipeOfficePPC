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


#include <com/sun/star/lang/XSingleServiceFactory.hpp>

#include <cppuhelper/factory.hxx>


#include <documentdigitalsignatures.hxx>
#include <certificatecontainer.hxx>

using namespace ::com::sun::star;

extern "C"
{
SAL_DLLPUBLIC_EXPORT void* SAL_CALL xmlsecurity_component_getFactory( const sal_Char * pImplName, void * pServiceManager, void * /*pRegistryKey*/ )
{
    void* pRet = 0;
    uno::Reference< XInterface > xFactory;

    //Decryptor
    rtl::OUString implName = rtl::OUString::createFromAscii( pImplName );

    if ( pServiceManager && implName.equals( DocumentDigitalSignatures::GetImplementationName() ) )
    {
        // DocumentDigitalSignatures
        xFactory = cppu::createSingleComponentFactory(
            DocumentDigitalSignatures_CreateInstance,
            rtl::OUString::createFromAscii( pImplName ),
            DocumentDigitalSignatures::GetSupportedServiceNames() );
    }
    else if ( pServiceManager && implName.equals( CertificateContainer::impl_getStaticImplementationName() ))
    {
        // CertificateContainer
        xFactory = cppu::createOneInstanceFactory(
            reinterpret_cast< lang::XMultiServiceFactory * >( pServiceManager ),
            rtl::OUString::createFromAscii( pImplName ),
            CertificateContainer::impl_createInstance,
            CertificateContainer::impl_getStaticSupportedServiceNames() );
    }

     if (xFactory.is())
    {
        xFactory->acquire();
        pRet = xFactory.get();
    }
    return pRet;
}

}   // extern "C"





/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
