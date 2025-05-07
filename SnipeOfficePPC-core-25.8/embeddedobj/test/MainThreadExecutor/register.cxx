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

#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/registry/InvalidRegistryException.hpp>
#include <cppuhelper/factory.hxx>

#include "xexecutor.hxx"

using namespace ::com::sun::star;


extern "C" {

SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory( const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
    void * pRet = 0;

    ::rtl::OUString aImplName( ::rtl::OUString::createFromAscii( pImplName ) );
    uno::Reference< lang::XSingleServiceFactory > xFactory;

    if ( pServiceManager && aImplName.equals( UNOMainThreadExecutor::impl_staticGetImplementationName() ) )
    {
        xFactory= ::cppu::createOneInstanceFactory( reinterpret_cast< lang::XMultiServiceFactory*>( pServiceManager ),
                                            UNOMainThreadExecutor::impl_staticGetImplementationName(),
                                            UNOMainThreadExecutor::impl_staticCreateSelfInstance,
                                            UNOMainThreadExecutor::impl_staticGetSupportedServiceNames() );
    }

    if ( xFactory.is() )
    {
        xFactory->acquire();
        pRet = xFactory.get();
    }

    return pRet;
}

sal_Bool SAL_CALL component_writeInfo( void * pServiceManager, void * pRegistryKey )
{
    if (pRegistryKey)
    {
        try
        {
            uno::Reference< registry::XRegistryKey > xKey( reinterpret_cast< registry::XRegistryKey* >( pRegistryKey ) );

            uno::Reference< registry::XRegistryKey >  xNewKey;

            xNewKey = xKey->createKey( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("/") ) +
                                        UNOMainThreadExecutor::impl_staticGetImplementationName() +
                                        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "/UNO/SERVICES") )  );

            uno::Sequence< ::rtl::OUString > &rServices = UNOMainThreadExecutor::impl_staticGetSupportedServiceNames();
            for( sal_Int32 ind = 0; ind < rServices.getLength(); ind++ )
                xNewKey->createKey( rServices.getConstArray()[ind] );

            return sal_True;
        }
        catch (registry::InvalidRegistryException &)
        {
            OSL_FAIL( "### InvalidRegistryException!" );
        }
    }
    return sal_False;
}

} // extern "C"

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
