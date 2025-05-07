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

#ifndef __FRAMEWORK_SERVICES_CONTENTHANDLERFACTORY_HXX_
#define __FRAMEWORK_SERVICES_CONTENTHANDLERFACTORY_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#include <classes/filtercache.hxx>
#include <macros/generic.hxx>
#include <macros/debug.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>
#include <threadhelp/threadhelpbase.hxx>

#include <threadhelp/transactionbase.hxx>
#include <general.h>

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/WrappedTargetException.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/container/ElementExistException.hpp>
#include <com/sun/star/container/NoSuchElementException.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/container/NoSuchElementException.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/util/XFlushable.hpp>

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/interfacecontainer.hxx>
#include <cppuhelper/weak.hxx>

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//  exported const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  exported definitions
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
    @short      factory to create handler-objects
    @descr      These class can be used to create new handler for specified contents.
                We use cached values of our configuration to lay down, which handler match
                a given URL or handlername. (use service TypeDetection to do that)
                With a detected type name you can search a registered handler and create it with these factory.

    @implements XInterface
                XTypeProvider
                XServiceInfo
                XMultiServiceFactory
                XNameContainer
                XNameReplace
                XNameAccess
                XElementAccess
                XFlushable

    @base       ThreadHelpBase
                OWeakObject

    @devstatus  ready to use
    @threadsafe yes
*//*-*************************************************************************************************************/

class ContentHandlerFactory :   // interfaces
                        public  css::lang::XTypeProvider            ,
                        public  css::lang::XServiceInfo             ,
                        public  css::lang::XMultiServiceFactory     ,
                        public  css::container::XNameContainer      ,       // => XNameReplace => XNameAccess => XElementAccess
                        public  css::util::XFlushable               ,
                        // base classes
                        // Order is neccessary for right initialization of it!
                        private ThreadHelpBase                      ,
                        private TransactionBase                     ,
                        public  ::cppu::OWeakObject
{
    //-------------------------------------------------------------------------------------------------------------
    //  public methods
    //-------------------------------------------------------------------------------------------------------------

    public:

        //---------------------------------------------------------------------------------------------------------
        //  constructor / destructor
        //---------------------------------------------------------------------------------------------------------
                 ContentHandlerFactory( const css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory );
        virtual ~ContentHandlerFactory(                                                                        );

        //---------------------------------------------------------------------------------------------------------
        //  XInterface, XTypeProvider, XServiceInfo
        //---------------------------------------------------------------------------------------------------------
        DECLARE_XINTERFACE
        DECLARE_XTYPEPROVIDER
        DECLARE_XSERVICEINFO

        //---------------------------------------------------------------------------------------------------------
        //  XMultiServiceFactory
        //---------------------------------------------------------------------------------------------------------
        virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstance             ( const ::rtl::OUString&                        sTypeName   ) throw( css::uno::Exception        ,
                                                                                                                                                                     css::uno::RuntimeException );
        virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstanceWithArguments( const ::rtl::OUString&                        sTypeName   ,
                                                                                                  const css::uno::Sequence< css::uno::Any >&    lArguments  ) throw( css::uno::Exception, css::uno::RuntimeException );
        virtual css::uno::Sequence< ::rtl::OUString >       SAL_CALL getAvailableServiceNames   (                                                           ) throw( css::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        //  XNameContainer
        //---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL insertByName( const ::rtl::OUString&  sHandlerName        ,
                                            const css::uno::Any&    aHandlerProperties  ) throw( css::lang::IllegalArgumentException     ,
                                                                                                 css::container::ElementExistException   ,
                                                                                                 css::lang::WrappedTargetException       ,
                                                                                                 css::uno::RuntimeException              );
        virtual void SAL_CALL removeByName( const ::rtl::OUString&  sHandlerName        ) throw( css::container::NoSuchElementException  ,
                                                                                                 css::lang::WrappedTargetException       ,
                                                                                                 css::uno::RuntimeException              );

        //---------------------------------------------------------------------------------------------------------
        //  XNameReplace
        //---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL replaceByName( const ::rtl::OUString& sHandlerName       ,
                                             const css::uno::Any&   aHandlerProperties ) throw( css::lang::IllegalArgumentException    ,
                                                                                                css::container::NoSuchElementException ,
                                                                                                css::lang::WrappedTargetException      ,
                                                                                                css::uno::RuntimeException             );

        //---------------------------------------------------------------------------------------------------------
        //  XNameAccess
        //---------------------------------------------------------------------------------------------------------
        virtual css::uno::Any                         SAL_CALL getByName      ( const ::rtl::OUString& sName ) throw( css::container::NoSuchElementException ,
                                                                                                                      css::lang::WrappedTargetException      ,
                                                                                                                      css::uno::RuntimeException             );
        virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(                              ) throw( css::uno::RuntimeException             );
        virtual sal_Bool                              SAL_CALL hasByName      ( const ::rtl::OUString& sName ) throw( css::uno::RuntimeException             );

        //---------------------------------------------------------------------------------------------------------
        //  XElementAccess
        //---------------------------------------------------------------------------------------------------------
        virtual css::uno::Type SAL_CALL getElementType() throw( css::uno::RuntimeException );
        virtual sal_Bool       SAL_CALL hasElements   () throw( css::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        //  XFlushable
        //---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL flush              (                                                                   ) throw ( css::uno::RuntimeException );
        virtual void SAL_CALL addFlushListener   ( const css::uno::Reference< css::util::XFlushListener >& xListener ) throw ( css::uno::RuntimeException );
        virtual void SAL_CALL removeFlushListener( const css::uno::Reference< css::util::XFlushListener >& xListener ) throw ( css::uno::RuntimeException );

    //-------------------------------------------------------------------------------------------------------------
    //  protected methods
    //-------------------------------------------------------------------------------------------------------------

    protected:

    //-------------------------------------------------------------------------------------------------------------
    //  debug methods
    //  (should be private everyway!)
    //-------------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      debug-method to check incoming parameter of some other mehods of this class
            @descr      The following methods are used to check parameters for other methods
                        of this class. The return value is used directly for an ASSERT(...).

            @seealso    ASSERTs in implementation!

            @param      references to checking variables
            @return     sal_False ,on invalid parameter
            @return     sal_True  ,otherwise

            @onerror    -
        *//*-*****************************************************************************************************/

    #ifdef ENABLE_ASSERTIONS

    private:

        static sal_Bool implcp_ContentHandlerFactory        (   const   css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory            );
        static sal_Bool implcp_createInstanceWithArguments  (   const   ::rtl::OUString&                                        sTypeName           ,
                                                                  const css::uno::Sequence< css::uno::Any >&                    lArguments          );
        static sal_Bool implcp_getByName                    (   const   ::rtl::OUString&                                        sName               );
        static sal_Bool implcp_hasByName                    (   const   ::rtl::OUString&                                        sName               );
        static sal_Bool implcp_removeByName                 (   const   ::rtl::OUString&                                        sHandlerName        );

    #endif  //  #ifdef ENABLE_ASSERTIONS

    //-------------------------------------------------------------------------------------------------------------
    //  private variables
    //  (should be private everyway!)
    //-------------------------------------------------------------------------------------------------------------

    private:

        css::uno::Reference< css::lang::XMultiServiceFactory >      m_xFactory              ;
        FilterCache                                                 m_aCache                ;

};      //  class ContentHandlerFactory

}       //  namespace framework

#endif  //  #ifndef __FRAMEWORK_SERVICES_CONTENTHANDLERFACTORY_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
