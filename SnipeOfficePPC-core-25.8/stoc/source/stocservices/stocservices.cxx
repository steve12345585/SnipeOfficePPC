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


#include "stocservices.hxx"

#include "cppuhelper/factory.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "sal/types.h"
#include "uno/environment.h"
#include "uno/lbnames.h"

using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace cppu;
using namespace osl;
using namespace stoc_services;

#ifdef DISABLE_DYNLOADING

// Do we really to have non-static global variable with the same name
// g_moduleCount in *all* (more or less, it seems) modules even in the
// normal dynamic loading case? Weird. Anyway, in the DISABLE_DYNLOADING
// case we have no use for these, and they can't be the same name.

#define g_moduleCount g_stocservices_moduleCount

#endif

rtl_StandardModuleCount g_moduleCount = MODULE_COUNT_INIT;

static struct ImplementationEntry g_entries[] =
{
    // typeconv
    {
    TypeConverter_Impl_CreateInstance, tcv_getImplementationName,
    tcv_getSupportedServiceNames, createSingleComponentFactory,
    &g_moduleCount.modCnt , 0
    },
    // uriproc
    {
    ExternalUriReferenceTranslator::create,
    ExternalUriReferenceTranslator::getImplementationName,
    ExternalUriReferenceTranslator::getSupportedServiceNames,
    createSingleComponentFactory, 0, 0
    },
    {
    UriReferenceFactory::create,
    UriReferenceFactory::getImplementationName,
    UriReferenceFactory::getSupportedServiceNames,
    createSingleComponentFactory, 0, 0
    },
    {
    UriSchemeParser_vndDOTsunDOTstarDOTexpand::create,
    UriSchemeParser_vndDOTsunDOTstarDOTexpand::getImplementationName,
    UriSchemeParser_vndDOTsunDOTstarDOTexpand::getSupportedServiceNames,
    createSingleComponentFactory, 0, 0
    },
    {
    UriSchemeParser_vndDOTsunDOTstarDOTscript::create,
    UriSchemeParser_vndDOTsunDOTstarDOTscript::getImplementationName,
    UriSchemeParser_vndDOTsunDOTstarDOTscript::getSupportedServiceNames,
    createSingleComponentFactory, 0, 0
    },
    {
    VndSunStarPkgUrlReferenceFactory::create,
    VndSunStarPkgUrlReferenceFactory::getImplementationName,
    VndSunStarPkgUrlReferenceFactory::getSupportedServiceNames,
    createSingleComponentFactory, 0, 0
    },
    { 0, 0, 0, 0, 0, 0 }
};

extern "C"
{

#ifndef DISABLE_DYNLOADING

SAL_DLLPUBLIC_EXPORT sal_Bool SAL_CALL component_canUnload( TimeValue *pTime )
{
    return g_moduleCount.canUnload( &g_moduleCount , pTime );
}

#endif

#ifdef DISABLE_DYNLOADING
#define component_getFactory stocservices_component_getFactory
#endif

SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory(
    const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
    return component_getFactoryHelper( pImplName, pServiceManager, pRegistryKey , g_entries );
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
