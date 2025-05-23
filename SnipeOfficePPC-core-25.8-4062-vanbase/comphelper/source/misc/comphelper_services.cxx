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


#include "comphelper_module.hxx"

#include <rtl/instance.hxx>

//--------------------------------------------------------------------
extern void createRegistryInfo_OPropertyBag();
extern void createRegistryInfo_SequenceOutputStream();
extern void createRegistryInfo_SequenceInputStream();
extern void createRegistryInfo_UNOMemoryStream();
extern void createRegistryInfo_IndexedPropertyValuesContainer();
extern void createRegistryInfo_NamedPropertyValuesContainer();
extern void createRegistryInfo_AnyCompareFactory();
extern void createRegistryInfo_OfficeInstallationDirectories();
extern void createRegistryInfo_OInstanceLocker();
extern void createRegistryInfo_Map();
extern void createRegistryInfo_OSimpleLogRing();
extern void createRegistryInfo_OOfficeRestartManager();

//........................................................................
namespace comphelper { namespace module
{
//........................................................................

    namespace
    {
        class doInitialize
        {
        public:
            doInitialize()
            {
                createRegistryInfo_OPropertyBag();
                createRegistryInfo_SequenceOutputStream();
                createRegistryInfo_SequenceInputStream();
                createRegistryInfo_UNOMemoryStream();
                createRegistryInfo_IndexedPropertyValuesContainer();
                createRegistryInfo_NamedPropertyValuesContainer();
                createRegistryInfo_AnyCompareFactory();
                createRegistryInfo_OfficeInstallationDirectories();
                createRegistryInfo_OInstanceLocker();
                createRegistryInfo_Map();
                createRegistryInfo_OSimpleLogRing();
                createRegistryInfo_OOfficeRestartManager();
            }
        };

        struct theInitializer : public rtl::Static< doInitialize, theInitializer > {};
    }

    static void initializeModule()
    {
        theInitializer::get();
    }

//........................................................................
} } // namespace comphelper::module
//........................................................................

extern "C" SAL_DLLPUBLIC_EXPORT void* SAL_CALL comphelp_component_getFactory(
    const sal_Char* pImplementationName, SAL_UNUSED_PARAMETER void*,
    SAL_UNUSED_PARAMETER void* )
{
    ::comphelper::module::initializeModule();
    return ::comphelper::module::ComphelperModule::getInstance().getComponentFactory( pImplementationName );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
