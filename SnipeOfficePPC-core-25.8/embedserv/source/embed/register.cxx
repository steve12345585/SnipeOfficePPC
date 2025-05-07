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
#if defined(_MSC_VER) && (_MSC_VER > 1310)
#pragma warning(disable : 4917 4555)
#endif

#ifdef __MINGW32__
#define INITGUID
#endif
#include "servprov.hxx"
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/registry/InvalidRegistryException.hpp>
#include <rtl/ustring.h>
#include <cppuhelper/factory.hxx>


using namespace ::com::sun::star;


uno::Reference<uno::XInterface> SAL_CALL EmbedServer_createInstance(
    const uno::Reference<lang::XMultiServiceFactory> & xSMgr)
throw (uno::Exception)
{
    uno::Reference<uno::XInterface > xService = *new EmbedServer_Impl( xSMgr );
    return xService;
}

::rtl::OUString SAL_CALL EmbedServer_getImplementationName() throw()
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.ole.EmbedServer") );

}

uno::Sequence< ::rtl::OUString > SAL_CALL EmbedServer_getSupportedServiceNames() throw()
{
    uno::Sequence< ::rtl::OUString > aServiceNames( 1 );
    aServiceNames[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.OleEmbeddedServerRegistration" ) );
    return aServiceNames;
}

extern "C" {

SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory( const sal_Char * pImplName, void * pServiceManager, void * /*pRegistryKey*/ )
{
    void * pRet = 0;

    ::rtl::OUString aImplName( ::rtl::OUString::createFromAscii( pImplName ) );
    uno::Reference< lang::XSingleServiceFactory > xFactory;

    if(pServiceManager && aImplName.equals( EmbedServer_getImplementationName() ) )
    {
        xFactory= ::cppu::createOneInstanceFactory( reinterpret_cast< lang::XMultiServiceFactory*>(pServiceManager),
                                            EmbedServer_getImplementationName(),
                                            EmbedServer_createInstance,
                                            EmbedServer_getSupportedServiceNames() );
    }

    if (xFactory.is())
    {
        xFactory->acquire();
        pRet = xFactory.get();
    }

    return pRet;
}

} // extern "C"

// Fix strange warnings about some
// ATL::CAxHostWindow::QueryInterface|AddRef|Releae functions.
// warning C4505: 'xxx' : unreferenced local function has been removed
#if defined(_MSC_VER)
#pragma warning(disable: 4505)
#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
