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
#ifndef _CPPUHELPER_FACTORY_HXX_
#define _CPPUHELPER_FACTORY_HXX_

#include <rtl/ustring.hxx>
#include <uno/dispatcher.h>
#include <rtl/unload.h>

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include "cppuhelperdllapi.h"

//##################################################################################################

#define COMPONENT_GETENV            "component_getImplementationEnvironment"
#define COMPONENT_GETENVEXT         "component_getImplementationEnvironmentExt"
#define COMPONENT_GETDESCRIPTION    "component_getDescription"
#define COMPONENT_WRITEINFO         "component_writeInfo"
#define COMPONENT_GETFACTORY        "component_getFactory"

typedef struct _uno_Environment uno_Environment;

/** Function pointer declaration.
    Function determines the environment of the component implementation, i.e. which compiler
    compiled it. If the environment is NOT session specific (needs no additional context),
    then this function should return the environment type name and leave ppEnv (to 0).

    @param ppEnvTypeName environment type name; string must be constant
    @param ppEnv function returns its environment if the environment is session specific,
                 i.e. has special context
*/
typedef void (SAL_CALL * component_getImplementationEnvironmentFunc)(
    const sal_Char ** ppEnvTypeName, uno_Environment ** ppEnv );

/** Function pointer declaration.
    Function determines the environment of the component implementation, i.e. the compiler.
    If the environment is NOT session specific (needs no additional context),
    then this function should return the environment type name and leave ppEnv (to 0).

    @param ppEnvTypeName environment type name; string must be a constant
    @param ppEnv         function returns an environment if the environment is session specific,
                         i.e. has special context
    @param pImplName
*/
typedef void (SAL_CALL * component_getImplementationEnvironmentExtFunc)(
    sal_Char        const ** ppEnvTypeName,
    uno_Environment       ** ppEnv,
    sal_Char        const  * pImplName,
    uno_Environment        * pTargetEnv
);

/** Function pointer declaration.
    Function retrieves a component description.

    @return an XML formatted string containing a short component description
    @deprecated
*/
typedef const sal_Char * (SAL_CALL * component_getDescriptionFunc)(void);

/** Function pointer declaration.

    @deprecated component_writeInfo should no longer be used in new components

    Function writes component registry info, at least writing the supported service names.

    @param pServiceManager
    a service manager (the type is an XMultiServiceFactory that can be used
    by the environment returned by component_getImplementationEnvironment)
    @param pRegistryKey a registry key
    (the type is XRegistryKey that can be used by the environment
    returned by component_getImplementationEnvironment)
    @return true if everything went fine
*/
typedef sal_Bool (SAL_CALL * component_writeInfoFunc)(
    void * pServiceManager, void * pRegistryKey );

/** Function pointer declaration.
    Retrieves a factory to create component instances.

   @param pImplName
   desired implementation name
   @param pServiceManager
   a service manager (the type is XMultiServiceFactory that can be used by the environment
   returned by component_getImplementationEnvironment)
   @param pRegistryKey
   a registry key (the type is XRegistryKey that can be used by the environment
   returned by component_getImplementationEnvironment)
   @return acquired component factory
   (the type is lang::XSingleComponentFactory or lang::XSingleServiceFactory to be used by the
   environment returned by component_getImplementationEnvironment)
*/
typedef void * (SAL_CALL * component_getFactoryFunc)(
    const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey );

//##################################################################################################

namespace cppu
{

/** Function pointer declaration.
    Function creates component instance passing the component context to be used.

    @param xContext component context to be used
    @return component instance
*/
typedef ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >(
    SAL_CALL * ComponentFactoryFunc)(
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & xContext )
    SAL_THROW( (::com::sun::star::uno::Exception) );

/** Creates a single component factory supporting the XSingleComponentFactory interface.

    @param fptr function pointer for instanciating the object
    @param rImplementationName implementation name of service
    @param rServiceNames supported services
    @param pModCount for future extension (library unloading concept).
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory >
SAL_CALL createSingleComponentFactory(
    ComponentFactoryFunc fptr,
    ::rtl::OUString const & rImplementationName,
    ::com::sun::star::uno::Sequence< ::rtl::OUString > const & rServiceNames,
    rtl_ModuleCount * pModCount = 0 )
    SAL_THROW(());

/** Creates a single service factory which holds the instance created only once.

    @param fptr function pointer for instanciating the object
    @param rImplementationName implementation name of service
    @param rServiceNames supported services
    @param pModCount for future extension (library unloading concept).

    @see createSingleComponentFactory
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory > SAL_CALL
createOneInstanceComponentFactory(
    ComponentFactoryFunc fptr,
    ::rtl::OUString const & rImplementationName,
    ::com::sun::star::uno::Sequence< ::rtl::OUString > const & rServiceNames,
    rtl_ModuleCount * pModCount = 0 )
    SAL_THROW(());

/** Deprecated.  The type of the instanciate function used as argument of the create*Fcatory functions.

    @see createSingleFactory
    @see createOneInstanceFactory
    @deprecated
*/
typedef ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >(SAL_CALL * ComponentInstantiation)(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager );

/** Deprecated.  Creates a single service factory.

    @param rServiceManager      the service manager used by the implementation.
    @param rImplementationName  the implementation name. An empty string is possible.
    @param pCreateFunction      the function pointer to create an object.
    @param rServiceNames        the service supported by the implementation.
    @param pModCount            for future extension (library unloading concept).
    @return a factory that support the interfaces XServiceProvider, XServiceInfo
    XSingleServiceFactory and XComponent.

    @see createOneInstanceFactory
    @deprecated
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > SAL_CALL
createSingleFactory(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager,
    const ::rtl::OUString & rImplementationName,
    ComponentInstantiation pCreateFunction,
    const ::com::sun::star::uno::Sequence< ::rtl::OUString > & rServiceNames,
    rtl_ModuleCount * pModCount = 0  )
    SAL_THROW(());

/** Deprecated.  Creates a factory wrapping another one.
    This means the methods of the interfaces XServiceProvider, XServiceInfo and
    XSingleServiceFactory are forwarded.
    @attention
    The XComponent interface is not supported!

    @param rServiceManager      the service manager used by the implementation.
    @param rFactory             the wrapped service factory.
    @return a factory that support the interfaces XServiceProvider, XServiceInfo
    XSingleServiceFactory.

    @see createSingleFactory
    @deprecated
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > SAL_CALL
createFactoryProxy(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager,
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > & rFactory )
    SAL_THROW(());

/** Deprecated.  Creates a single service factory which holds the instance created only once.

    @param rServiceManager      the service manager used by the implementation.
    @param rComponentName       the implementation name. An empty string is possible.
    @param pCreateFunction      the function pointer to create an object.
    @param rServiceNames        the service supported by the implementation.
    @param pModCount            for future extension (library unloading concept).
    @return a factory that support the interfaces XServiceProvider, XServiceInfo
    XSingleServiceFactory and XComponent.

    @see createSingleFactory
    @deprecated
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > SAL_CALL
createOneInstanceFactory(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager,
    const ::rtl::OUString & rComponentName,
    ComponentInstantiation pCreateFunction,
    const ::com::sun::star::uno::Sequence< ::rtl::OUString > & rServiceNames,
    rtl_ModuleCount * pModCount = 0  )
    SAL_THROW(());

/** Deprecated.  Creates a single service factory based on a registry.

    @param rServiceManager      the service manager used by the implementation.
    @param rImplementationName  the implementation name. An empty string is possible.
    @param rImplementationKey   the registry key of the implementation section.
    @return a factory that support the interfaces XServiceProvider, XServiceInfo
    XSingleServiceFactory and XComponent.
    @deprecated
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > SAL_CALL
createSingleRegistryFactory(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager,
    const ::rtl::OUString & rImplementationName,
    const ::com::sun::star::uno::Reference< ::com::sun::star::registry::XRegistryKey > & rImplementationKey )
    SAL_THROW(());

/** Deprecated.  Creates a single service factory which holds the instance created only once
    based on a registry.

    @param rServiceManager      the service manager used by the implementation.
    @param rComponentName       the implementation name. An empty string is possible.
    @param rImplementationKey   the registry key of the implementation section.
    @return a factory that support the interfaces XServiceProvider, XServiceInfo
    XSingleServiceFactory and XComponent.

    @see createSingleRegistryFactory
    @deprecated
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > SAL_CALL
createOneInstanceRegistryFactory(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rServiceManager,
    const ::rtl::OUString & rComponentName,
    const ::com::sun::star::uno::Reference< ::com::sun::star::registry::XRegistryKey > & rImplementationKey )
    SAL_THROW(());

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
