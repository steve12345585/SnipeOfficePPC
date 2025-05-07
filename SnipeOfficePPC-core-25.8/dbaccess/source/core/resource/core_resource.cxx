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


#include "core_resource.hxx"

#include <tools/resmgr.hxx>

// ---- needed as long as we have no contexts for components ---
#include <vcl/svapp.hxx>
//---------------------------------------------------
#include <rtl/instance.hxx>
#include <svl/solar.hrc>

//.........................................................................
namespace dbaccess
{

    //==================================================================
    //= ResourceManager
    //==================================================================
    namespace
    {
        // access safety
        struct theResourceManagerMutex : public rtl::Static< osl::Mutex, theResourceManagerMutex > {};
    }

    sal_Int32       ResourceManager::s_nClients = 0;
    ResMgr*         ResourceManager::m_pImpl = NULL;

    //------------------------------------------------------------------
    void ResourceManager::ensureImplExists()
    {
        if (m_pImpl)
            return;

        ::com::sun::star::lang::Locale aLocale = Application::GetSettings().GetUILocale();

        m_pImpl = ResMgr::CreateResMgr("dba", aLocale);
    }

    //------------------------------------------------------------------
    ::rtl::OUString ResourceManager::loadString(sal_uInt16 _nResId)
    {
        ::rtl::OUString sReturn;

        ensureImplExists();
        if (m_pImpl)
            sReturn = String(ResId(_nResId,*m_pImpl));

        return sReturn;
    }

    //------------------------------------------------------------------
    ::rtl::OUString ResourceManager::loadString( sal_uInt16 _nResId, const sal_Char* _pPlaceholderAscii, const ::rtl::OUString& _rReplace )
    {
        String sString( loadString( _nResId ) );
        sString.SearchAndReplaceAscii( _pPlaceholderAscii, _rReplace );
        return sString;
    }

    //------------------------------------------------------------------
    ::rtl::OUString ResourceManager::loadString( sal_uInt16 _nResId, const sal_Char* _pPlaceholderAscii1, const ::rtl::OUString& _rReplace1,
        const sal_Char* _pPlaceholderAscii2, const ::rtl::OUString& _rReplace2 )
    {
        String sString( loadString( _nResId ) );
        sString.SearchAndReplaceAscii( _pPlaceholderAscii1, _rReplace1 );
        sString.SearchAndReplaceAscii( _pPlaceholderAscii2, _rReplace2 );
        return sString;
    }

    //-------------------------------------------------------------------------
    void ResourceManager::registerClient()
    {
        ::osl::MutexGuard aGuard(theResourceManagerMutex::get());
        ++s_nClients;
    }

    //-------------------------------------------------------------------------
    void ResourceManager::revokeClient()
    {
        ::osl::MutexGuard aGuard(theResourceManagerMutex::get());
        if (!--s_nClients && m_pImpl)
        {
            delete m_pImpl;
            m_pImpl = NULL;
        }
    }
//.........................................................................
}
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
