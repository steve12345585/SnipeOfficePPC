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

#include <salhelper/dynload.hxx>
#include <rtl/ustrbuf.hxx>

namespace salhelper
{

typedef void*   (SAL_CALL *ApiInitFunction) (void);

ORealDynamicLoader::ORealDynamicLoader(ORealDynamicLoader ** ppSetToZeroInDestructor_,
                       const rtl::OUString& moduleName,
                       const rtl::OUString& initFunction,
                       void* pApi,
                       oslModule pModule)
    : m_pApi(pApi)
    , m_refCount(1)
    , m_pModule(pModule)
    , m_strModuleName(moduleName)
    , m_strInitFunction(initFunction)
    , ppSetToZeroInDestructor( ppSetToZeroInDestructor_ )
{
}

ORealDynamicLoader* ORealDynamicLoader::newInstance(ORealDynamicLoader ** ppSetToZeroInDestructor,
                                  const rtl::OUString& moduleName,
                                  const rtl::OUString& initFunction)
{
    ApiInitFunction initFunc;
    oslModule pModule = osl_loadModule(moduleName.pData, SAL_LOADMODULE_DEFAULT);

    if ( !pModule )
    {
        return NULL;
    }

    initFunc = (ApiInitFunction)osl_getFunctionSymbol(
        pModule, initFunction.pData);

    if ( !initFunc )
    {
        osl_unloadModule(pModule);
        return NULL;
    }

    return(new ORealDynamicLoader(ppSetToZeroInDestructor, moduleName,
                                 initFunction,
                                 initFunc(),
                                 pModule));
}

ORealDynamicLoader::~ORealDynamicLoader()
{
    // set the address to zero
    if( ppSetToZeroInDestructor )
        *ppSetToZeroInDestructor = 0;

    if (m_pModule)
    {
        osl_unloadModule(m_pModule);
        m_pModule = NULL;
    }
}

sal_uInt32 ORealDynamicLoader::acquire()
{
    return ++m_refCount;
}

sal_uInt32 ORealDynamicLoader::release()
{
    sal_uInt32 nRet = --m_refCount;
    if( nRet == 0 )
        delete this;
    return nRet;
}


void* ORealDynamicLoader::getApi() const
{
    return m_pApi;
}

} // namespace salhelper

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
