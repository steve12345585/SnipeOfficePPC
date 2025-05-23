/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 *  The Contents of this file are made available subject to the terms of
 *  the BSD license.
 *
 *  Copyright 2000, 2010 Oracle and/or its affiliates.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of Sun Microsystems, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 *  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************/

#include <stdio.h>
#include <rtl/ustring.hxx>
#include <cppuhelper/queryinterface.hxx> // helper for queryInterface() impl
#include <cppuhelper/factory.hxx> // helper for component factory
// generated c++ interfaces
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>

// include our specific addon header to get access to functions and definitions
#include <addon.hxx>

using namespace ::rtl;
using namespace ::osl;
using namespace ::cppu;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::registry;

//##################################################################################################
//#### EXPORTED ####################################################################################
//##################################################################################################


/**
 * This function creates an implementation section in the registry and another subkey
 *
 * for each supported service.
 * @param pServiceManager   the service manager
 * @param pRegistryKey      the registry key
 */
// This method not longer necessary since OOo 3.4 where the component registration was
// was changed to passive component registration. For more details see
// http://wiki.services.openoffice.org/wiki/Passive_Component_Registration
//
// extern "C" SAL_DLLPUBLIC_EXPORT sal_Bool SAL_CALL component_writeInfo(void * pServiceManager, void * pRegistryKey)
// {
//  sal_Bool result = sal_False;

//  if (pRegistryKey)
//  {
//      try
//      {
//          Reference< XRegistryKey > xNewKey(
//              reinterpret_cast< XRegistryKey * >( pRegistryKey )->createKey(
//                  OUString( RTL_CONSTASCII_USTRINGPARAM("/" IMPLEMENTATION_NAME "/UNO/SERVICES") ) ) );

//          const Sequence< OUString > & rSNL =
//              Addon_getSupportedServiceNames();
//          const OUString * pArray = rSNL.getConstArray();
//          for ( sal_Int32 nPos = rSNL.getLength(); nPos--; )
//              xNewKey->createKey( pArray[nPos] );

//          return sal_True;
//      }
//      catch (InvalidRegistryException &)
//      {
//          // we should not ignore exceptions
//      }
//  }
//  return result;
// }

/**
 * This function is called to get service factories for an implementation.
 *
 * @param pImplName       name of implementation
 * @param pServiceManager a service manager, need for component creation
 * @param pRegistryKey    the registry key for this component, need for persistent data
 * @return a component factory
 */
extern "C" SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory(const sal_Char * pImplName, void * /*pServiceManager*/, void * pRegistryKey)
{
    void * pRet = 0;

    if (rtl_str_compare( pImplName, IMPLEMENTATION_NAME ) == 0)
    {
        Reference< XSingleComponentFactory > xFactory( createSingleComponentFactory(
            Addon_createInstance,
            OUString( RTL_CONSTASCII_USTRINGPARAM( IMPLEMENTATION_NAME ) ),
            Addon_getSupportedServiceNames() ) );

        if (xFactory.is())
        {
            xFactory->acquire();
            pRet = xFactory.get();
        }
    }

    return pRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
