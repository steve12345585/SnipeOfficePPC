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

#ifndef __FRAMEWORK_UIFACTORY_ADDONSTOOLBOXFACTORY_HXX_
#define __FRAMEWORK_UIFACTORY_ADDONSTOOLBOXFACTORY_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#include <stdtypes.h>
#include <threadhelp/threadhelpbase.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>
#include <services.h>

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/ui/XUIElementFactory.hpp>
#include <com/sun/star/frame/XModuleManager.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XModel.hpp>

//_________________________________________________________________________________________________________________
//  includes of other projects
//_________________________________________________________________________________________________________________
#include <cppuhelper/implbase2.hxx>
#include <rtl/ustring.hxx>

namespace framework
{

class AddonsToolBoxFactory :  protected ThreadHelpBase                                  ,   // Struct for right initalization of mutex member! Must be first of baseclasses.
                              public ::cppu::WeakImplHelper2< com::sun::star::lang::XServiceInfo        ,
                                                              com::sun::star::ui::XUIElementFactory >
{
    public:
        AddonsToolBoxFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager );
        virtual ~AddonsToolBoxFactory();

        //  XInterface, XTypeProvider, XServiceInfo
        DECLARE_XSERVICEINFO

        // XUIElementFactory
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIElement > SAL_CALL createUIElement( const ::rtl::OUString& ResourceURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& Args ) throw ( ::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );

        sal_Bool hasButtonsInContext( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > >& rPropSeq,
                                      const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame );

    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceManager;
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModuleManager > m_xModuleManager;
};

}

#endif // __FRAMEWORK_UIFACTORY_ADDONSTOOLBOXFACTORY_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
