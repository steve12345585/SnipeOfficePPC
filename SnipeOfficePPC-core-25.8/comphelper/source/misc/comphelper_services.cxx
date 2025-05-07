/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
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
