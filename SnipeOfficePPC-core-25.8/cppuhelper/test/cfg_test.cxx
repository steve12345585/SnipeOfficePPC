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


// starting the executable:
// -env:UNO_CFG_URL=local;<absolute_path>..\\..\\test\\cfg_data;<absolute_path>\\cfg_update
// -env:UNO_TYPES=cpputest.rdb

#include <sal/main.h>

#include <stdio.h>

#include <rtl/strbuf.hxx>

#include <cppuhelper/implementationentry.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <cppuhelper/implbase2.hxx>

#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XComponent.hpp>

#include <com/sun/star/registry/XImplementationRegistration.hpp>

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )


using namespace ::cppu;
using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace cfg_test
{

//--------------------------------------------------------------------------------------------------
static Sequence< OUString > impl0_getSupportedServiceNames()
{
    OUString str( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.bootstrap.TestComponent0") );
    return Sequence< OUString >( &str, 1 );
}
//--------------------------------------------------------------------------------------------------
static OUString impl0_getImplementationName()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.bootstrap.TestComponent0") );
}
//--------------------------------------------------------------------------------------------------
static Sequence< OUString > impl1_getSupportedServiceNames()
{
    OUString str( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.bootstrap.TestComponent1") );
    return Sequence< OUString >( &str, 1 );
}
//--------------------------------------------------------------------------------------------------
static OUString impl1_getImplementationName()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.bootstrap.TestComponent1") );
}

//==================================================================================================
class ServiceImpl0
    : public WeakImplHelper2< lang::XServiceInfo, lang::XInitialization >
{
    Reference< XComponentContext > m_xContext;

public:
    ServiceImpl0( Reference< XComponentContext > const & xContext ) SAL_THROW(());

    // XInitialization
    virtual void SAL_CALL initialize( const Sequence< Any >& rArgs ) throw (Exception, RuntimeException);

    // XServiceInfo
    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames() throw (RuntimeException);
    virtual OUString SAL_CALL getImplementationName() throw (RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const OUString & rServiceName ) throw (RuntimeException);
};
//__________________________________________________________________________________________________
ServiceImpl0::ServiceImpl0( Reference< XComponentContext > const & xContext ) SAL_THROW(())
    : m_xContext( xContext )
{
    sal_Int32 n;
    OUString val;

    // service properties
    OSL_VERIFY( m_xContext->getValueByName(
        OUSTR("/services/com.sun.star.bootstrap.TestComponent0/context-properties/serviceprop0") ) >>= n );
    OSL_VERIFY( n == 13 );
    OSL_VERIFY( m_xContext->getValueByName(
        OUSTR("/services/com.sun.star.bootstrap.TestComponent0/context-properties/serviceprop1") ) >>= val );
    OSL_VERIFY( val == "value of serviceprop1" );
    // impl properties
    OSL_VERIFY( m_xContext->getValueByName(
        OUSTR("/implementations/com.sun.star.comp.bootstrap.TestComponent0/context-properties/implprop0") ) >>= n );
    OSL_VERIFY( n == 15 );
    OSL_VERIFY( m_xContext->getValueByName(
        OUSTR("/implementations/com.sun.star.comp.bootstrap.TestComponent0/context-properties/implprop1") ) >>= val );
    OSL_VERIFY( val == "value of implprop1" );
}
// XInitialization
//__________________________________________________________________________________________________
void ServiceImpl0::initialize( const Sequence< Any >& rArgs )
    throw (Exception, RuntimeException)
{
    // check args
    OUString val;
    OSL_VERIFY( rArgs.getLength() == 3 );
    OSL_VERIFY( rArgs[ 0 ] >>= val );
    OSL_VERIFY( val == "first argument" );
    OSL_VERIFY( rArgs[ 1 ] >>= val );
    OSL_VERIFY( val == "second argument" );
    OSL_VERIFY( rArgs[ 2 ] >>= val );
    OSL_VERIFY( val == "third argument" );
}
// XServiceInfo
//__________________________________________________________________________________________________
OUString ServiceImpl0::getImplementationName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return impl0_getImplementationName();
}
//__________________________________________________________________________________________________
Sequence< OUString > ServiceImpl0::getSupportedServiceNames()
    throw(::com::sun::star::uno::RuntimeException)
{
    return impl0_getSupportedServiceNames();
}
//__________________________________________________________________________________________________
sal_Bool ServiceImpl0::supportsService( const OUString & rServiceName )
    throw(::com::sun::star::uno::RuntimeException)
{
    const Sequence< OUString > & rSNL = getSupportedServiceNames();
    const OUString * pArray = rSNL.getConstArray();
    for ( sal_Int32 nPos = rSNL.getLength(); nPos--; )
    {
        if (pArray[nPos] == rServiceName)
            return sal_True;
    }
    return sal_False;
}

//==================================================================================================
class ServiceImpl1 : public ServiceImpl0
{
public:
    inline ServiceImpl1( Reference< XComponentContext > const & xContext ) SAL_THROW(())
        : ServiceImpl0( xContext )
        {}

    // XServiceInfo
    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames() throw (RuntimeException);
    virtual OUString SAL_CALL getImplementationName() throw (RuntimeException);
};
//__________________________________________________________________________________________________
OUString ServiceImpl1::getImplementationName()
    throw(::com::sun::star::uno::RuntimeException)
{
    return impl1_getImplementationName();
}
//__________________________________________________________________________________________________
Sequence< OUString > ServiceImpl1::getSupportedServiceNames()
    throw(::com::sun::star::uno::RuntimeException)
{
    return impl1_getSupportedServiceNames();
}

//==================================================================================================
static Reference< XInterface > SAL_CALL ServiceImpl0_create(
    Reference< XComponentContext > const & xContext )
    SAL_THROW( (Exception) )
{
    return (OWeakObject *)new ServiceImpl0( xContext );
}
//==================================================================================================
static Reference< XInterface > SAL_CALL ServiceImpl1_create(
    Reference< XComponentContext > const & xContext )
    SAL_THROW( (Exception) )
{
    return (OWeakObject *)new ServiceImpl1( xContext );
}

} // namespace cfg_test

static struct ImplementationEntry g_entries[] =
{
    {
        ::cfg_test::ServiceImpl0_create, ::cfg_test::impl0_getImplementationName,
        ::cfg_test::impl0_getSupportedServiceNames, createSingleComponentFactory,
        0, 0
    },
    {
        ::cfg_test::ServiceImpl1_create, ::cfg_test::impl1_getImplementationName,
        ::cfg_test::impl1_getSupportedServiceNames, createSingleComponentFactory,
        0, 0
    },
    { 0, 0, 0, 0, 0, 0 }
};

// component exports
extern "C"
{
//==================================================================================================
sal_Bool SAL_CALL component_writeInfo(
    void * pServiceManager, void * pRegistryKey )
{
    return component_writeInfoHelper(
        pServiceManager, pRegistryKey, g_entries );
}
//==================================================================================================
SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory(
    const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
    return component_getFactoryHelper(
        pImplName, pServiceManager, pRegistryKey , g_entries );
}
}


//##################################################################################################
//##################################################################################################
//##################################################################################################

SAL_IMPLEMENT_MAIN()
{
    try
    {
        Reference< XComponentContext > xContext( defaultBootstrap_InitialComponentContext() );
        Reference< lang::XMultiComponentFactory > xMgr( xContext->getServiceManager() );

        // show what is in context
        xContext->getValueByName( OUSTR("dump_maps") );

        sal_Int32 n(0);
        OSL_VERIFY( xContext->getValueByName( OUSTR("/global-context-properties/TestValue") ) >>= n );
        ::fprintf( stderr, "> n=%d\n", n );

        Reference< XInterface > x;
        OSL_VERIFY( !(xContext->getValueByName( OUSTR("/singletons/my_converter") ) >>= x) );
        OSL_VERIFY( xContext->getValueByName( OUSTR("/singletons/com.sun.star.script.theConverter") ) >>= x );
        OSL_VERIFY( xContext->getValueByName( OUSTR("/singletons/com.sun.star.bootstrap.theTestComponent0") ) >>= x );

        ::fprintf( stderr, "> registering service...\n");
#if defined(SAL_W32)
        OUString libName( OUSTR("cfg_test.dll") );
#elif defined(SAL_UNX)
        OUString libName( OUSTR("libcfg_test.so") );
#endif
        Reference< registry::XImplementationRegistration > xImplReg( xMgr->createInstanceWithContext(
            OUSTR("com.sun.star.registry.ImplementationRegistration"), xContext ), UNO_QUERY );
        OSL_ENSURE( xImplReg.is(), "### no impl reg!" );
        xImplReg->registerImplementation(
            OUSTR("com.sun.star.loader.SharedLibrary"), libName,
            Reference< registry::XSimpleRegistry >() );

        OSL_VERIFY( (x = xMgr->createInstanceWithContext( OUSTR("com.sun.star.bootstrap.TestComponent0"), xContext )).is() );
        OSL_VERIFY( (x = xMgr->createInstanceWithContext( OUSTR("com.sun.star.bootstrap.TestComponent1"), xContext )).is() );

        Reference< lang::XComponent > xComp( xContext, UNO_QUERY );
        if (xComp.is())
        {
            xComp->dispose();
        }
        return 0;
    }
    catch (Exception & exc)
    {
        OString str( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
        ::fprintf( stderr, "# caught exception: %s\n", str.getStr() );
        return 1;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
