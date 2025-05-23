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

#ifndef _OSL_MODULE_HXX_
#define _OSL_MODULE_HXX_

#include <rtl/ustring.hxx>
#include <osl/module.h>

namespace osl
{

class Module
{
    Module( const Module&);
    Module& operator = ( const Module&);

public:
    static sal_Bool getUrlFromAddress(void * addr, ::rtl::OUString & libraryUrl) {
        return osl_getModuleURLFromAddress(addr, &libraryUrl.pData);
    }

    /** Get module URL from the specified function address in the module.

        Similar to getUrlFromAddress, but use a function address to get URL of the Module.
        Use Function pointer as symbol address to conceal type conversion.

        @param addr
        [in] function address in oslGenericFunction format.

        @param libraryUrl
        [in|out] receives the URL of the module.

        @return
        <dl>
        <dt>sal_True</dt>
        <dd>on success</dd>
        <dt>sal_False</dt>
        <dd>can not get the URL from the specified function address or the parameter is invalid.</dd>
        </dl>

        @see getUrlFromAddress
    */
    static sal_Bool getUrlFromAddress( oslGenericFunction addr, ::rtl::OUString & libraryUrl){
        return osl_getModuleURLFromFunctionAddress( addr, &libraryUrl.pData );
    }

    Module(): m_Module(0){}

#ifndef DISABLE_DYNLOADING

    Module( const ::rtl::OUString& strModuleName, sal_Int32 nRtldMode = SAL_LOADMODULE_DEFAULT) : m_Module(0)
    {
        load( strModuleName, nRtldMode);
    }

#endif

    ~Module()
    {
#ifndef DISABLE_DYNLOADING
        osl_unloadModule(m_Module);
#endif
    }

#ifndef DISABLE_DYNLOADING

    sal_Bool SAL_CALL load( const ::rtl::OUString& strModuleName,
        sal_Int32 nRtldMode = SAL_LOADMODULE_DEFAULT)
    {
        unload();
        m_Module= osl_loadModule( strModuleName.pData, nRtldMode );
        return is();
    }

    /// @since UDK 3.2.8
    sal_Bool SAL_CALL loadRelative(
        ::oslGenericFunction baseModule, ::rtl::OUString const & relativePath,
        ::sal_Int32 mode = SAL_LOADMODULE_DEFAULT)
    {
        unload();
        m_Module = osl_loadModuleRelative(baseModule, relativePath.pData, mode);
        return is();
    }

    /// @Since SnipeOffice 3.5
    sal_Bool SAL_CALL loadRelative(
        oslGenericFunction baseModule, char const * relativePath,
        sal_Int32 mode = SAL_LOADMODULE_DEFAULT)
    {
        unload();
        m_Module = osl_loadModuleRelativeAscii(baseModule, relativePath, mode);
        return is();
    }

    void SAL_CALL unload()
    {
        if (m_Module)
        {
            osl_unloadModule(m_Module);
            m_Module = 0;
        }
    }

#endif

    sal_Bool SAL_CALL is() const
    {
           return m_Module != NULL;
    }

    void* SAL_CALL getSymbol( const ::rtl::OUString& strSymbolName)
    {
    return ( osl_getSymbol( m_Module, strSymbolName.pData ) );
    }

    /** Get function address by the function name in the module.

        getFunctionSymbol is an alternative function for getSymbol.
        Use Function pointer as symbol address to conceal type conversion.

        @param ustrFunctionSymbolName
        [in] Function name to be looked up.

        @return
        <dl>
        <dt>oslGenericFunction format function address</dt>
        <dd>on success</dd>
        <dt>NULL</dt>
        <dd>lookup failed or parameter is somewhat invalid</dd>
        </dl>

        @see getSymbol
    */
    oslGenericFunction SAL_CALL getFunctionSymbol( const ::rtl::OUString& ustrFunctionSymbolName ) const
    {
        return ( osl_getFunctionSymbol( m_Module, ustrFunctionSymbolName.pData ) );
    }

    /// @Since SnipeOffice 3.5
    oslGenericFunction SAL_CALL getFunctionSymbol(char const * name) const {
        return osl_getAsciiFunctionSymbol(m_Module, name);
    }

    operator oslModule() const
    {
        return m_Module;
    }

private:
    oslModule m_Module;

};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
