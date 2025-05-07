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

#ifndef _FORMULA_CORE_RESOURCE_HXX_
#define _FORMULA_CORE_RESOURCE_HXX_

#include <rtl/ustring.hxx>
#include <osl/mutex.hxx>

class ResMgr;
//.........................................................................
namespace formula
{
    //==================================================================
    //= ResourceManager
    //= handling ressources within the FORMULA-Core library
    //==================================================================
    class ResourceManager
    {
        friend class OModuleClient;
        static sal_Int32    s_nClients;     /// number of registered clients
        static ResMgr*  m_pImpl;

    private:
        // no instantiation allowed
        ResourceManager() { }
        ~ResourceManager() { }

    protected:
        static void ensureImplExists();
        /// register a client for the module
        static void registerClient();
        /// revoke a client for the module
        static void revokeClient();

    public:

        static ResMgr*  getResManager();
    };

    //=========================================================================
    //= OModuleClient
    //=========================================================================
    /** base class for objects which uses any global module-specific ressources
    */
    class OModuleClient
    {
    public:
        OModuleClient()     { ResourceManager::registerClient(); }
        ~OModuleClient()    { ResourceManager::revokeClient(); }
    };


//.........................................................................
} // formula
//.........................................................................

#endif // _FORMULA_CORE_RESOURCE_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
