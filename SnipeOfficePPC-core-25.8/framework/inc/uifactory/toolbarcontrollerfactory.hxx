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

#ifndef __FRAMEWORK_UIFACTORY_TOOLBARCONTROLLERFACTORY_HXX_
#define __FRAMEWORK_UIFACTORY_TOOLBARCONTROLLERFACTORY_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#include <threadhelp/threadhelpbase.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>
#include <stdtypes.h>

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/frame/XUIControllerRegistration.hpp>

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/implbase3.hxx>
#include <rtl/ustring.hxx>

namespace framework
{

class ConfigurationAccess_ControllerFactory;
class ToolbarControllerFactory :  protected ThreadHelpBase                                          ,   // Struct for right initalization of mutex member! Must be first of baseclasses.
                                  public ::cppu::WeakImplHelper3<   com::sun::star::lang::XServiceInfo,
                                                                    com::sun::star::lang::XMultiComponentFactory,
                                                                    com::sun::star::frame::XUIControllerRegistration>
{
    public:
        ToolbarControllerFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager );
        virtual ~ToolbarControllerFactory();

        //  XInterface, XTypeProvider, XServiceInfo
        DECLARE_XSERVICEINFO

        // XMultiComponentFactory
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL createInstanceWithContext( const ::rtl::OUString& aServiceSpecifier, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& Context ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL createInstanceWithArgumentsAndContext( const ::rtl::OUString& ServiceSpecifier, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& Arguments, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& Context ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getAvailableServiceNames() throw (::com::sun::star::uno::RuntimeException);

        // XUIControllerRegistration
        virtual sal_Bool SAL_CALL hasController( const ::rtl::OUString& aCommandURL, const rtl::OUString& aModuleName ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL registerController( const ::rtl::OUString& aCommandURL, const rtl::OUString& aModuleName, const ::rtl::OUString& aControllerImplementationName ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL deregisterController( const ::rtl::OUString& aCommandURL, const rtl::OUString& aModuleName ) throw (::com::sun::star::uno::RuntimeException);

    protected:
        ToolbarControllerFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager,bool  );
        sal_Bool                                                                         m_bConfigRead;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceManager;
        ConfigurationAccess_ControllerFactory*                                           m_pConfigAccess;
};

} // namespace framework

#endif // __FRAMEWORK_SERVICES_TOOLBARCONTROLLERFACTORY_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
