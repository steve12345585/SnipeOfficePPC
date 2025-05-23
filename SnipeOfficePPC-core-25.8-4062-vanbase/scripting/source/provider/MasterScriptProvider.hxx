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

#ifndef _FRAMEWORK_SCRIPT_PROVIDER_XFUNCTIONPROVIDER_HXX_
#define _FRAMEWORK_SCRIPT_PROVIDER_XFUNCTIONPROVIDER_HXX_

#include <rtl/ustring.hxx>

#include <cppuhelper/implbase5.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/document/XScriptInvocationContext.hpp>

#include <com/sun/star/lang/XInitialization.hpp>

#include <com/sun/star/script/provider/XScriptProvider.hpp>
#include <com/sun/star/script/browse/XBrowseNode.hpp>

#include "ProviderCache.hxx"

namespace func_provider
{

 typedef ::cppu::WeakImplHelper5<
     css::script::provider::XScriptProvider,
     css::script::browse::XBrowseNode, css::lang::XServiceInfo,
     css::lang::XInitialization,
     css::container::XNameContainer > t_helper;

class MasterScriptProvider :
            public t_helper
{
public:
    MasterScriptProvider(
        const css::uno::Reference< css::uno::XComponentContext >
        & xContext ) throw( css::uno::RuntimeException );
    ~MasterScriptProvider();

    // XServiceInfo implementation
    virtual ::rtl::OUString SAL_CALL getImplementationName( )
        throw( css::uno::RuntimeException );

    // XBrowseNode implementation
    virtual ::rtl::OUString SAL_CALL getName()
        throw ( css::uno::RuntimeException );
    virtual css::uno::Sequence< css::uno::Reference< css::script::browse::XBrowseNode > > SAL_CALL getChildNodes()
        throw ( css::uno::RuntimeException );
    virtual sal_Bool SAL_CALL hasChildNodes()
        throw ( css::uno::RuntimeException );
    virtual sal_Int16 SAL_CALL getType()
        throw ( css::uno::RuntimeException );
    // XNameContainer
    virtual void SAL_CALL insertByName( const ::rtl::OUString& aName, const css::uno::Any& aElement ) throw ( css::lang::IllegalArgumentException, css::container::ElementExistException, css::lang::WrappedTargetException, css::uno::RuntimeException);
    virtual void SAL_CALL removeByName( const ::rtl::OUString& Name ) throw ( css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException);

    // XNameReplace
    virtual void SAL_CALL replaceByName( const ::rtl::OUString& aName, const css::uno::Any& aElement ) throw ( css::lang::IllegalArgumentException, css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException);
    // XNameAccess
    virtual css::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw ( css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException);
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  ) throw ( css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw (::com::sun::star::uno::RuntimeException);

    // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType(  ) throw ( css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements(  ) throw ( css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
        throw( css::uno::RuntimeException );
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames( )
        throw( css::uno::RuntimeException );

    // XScriptProvider implementation
    virtual css::uno::Reference < css::script::provider::XScript > SAL_CALL
        getScript( const ::rtl::OUString& scriptURI )
        throw( css::script::provider::ScriptFrameworkErrorException,
               css::uno::RuntimeException );

    /**
     *  XInitialise implementation
     *
     * @param args expected to contain a single ::rtl::OUString
     * containing the URI
     */
    virtual void SAL_CALL initialize( const css::uno::Sequence < css::uno::Any > & args )
        throw ( css::uno::Exception, css::uno::RuntimeException);

    // Public method to return all Language Providers in this MasterScriptProviders
    // context.
    css::uno::Sequence< css::uno::Reference< css::script::provider::XScriptProvider > > SAL_CALL
        getAllProviders() throw ( css::uno::RuntimeException );

    bool isPkgProvider() { return m_bIsPkgMSP; }
    css::uno::Reference< css::script::provider::XScriptProvider > getPkgProvider() { return m_xMSPPkg; }
    // returns context string for this provider, eg
    ::rtl::OUString getContextString() { return m_sCtxString; }

private:
    ::rtl::OUString parseLocationName( const ::rtl::OUString& location );
    void  createPkgProvider();
    bool  isValid();
    ::rtl::OUString getURLForModel();
    const css::uno::Sequence< ::rtl::OUString >& getProviderNames();

    ProviderCache* providerCache();
    /* to obtain other services if needed */
    css::uno::Reference< css::uno::XComponentContext >              m_xContext;
    css::uno::Reference< css::lang::XMultiComponentFactory >        m_xMgr;
    css::uno::Reference< css::frame::XModel >                       m_xModel;
    css::uno::Reference< css::document::XScriptInvocationContext >  m_xInvocationContext;
    css::uno::Sequence< css::uno::Any >                             m_sAargs;
    ::rtl::OUString                                                 m_sNodeName;

    // This component supports XInitialization, it can be created
    // using createInstanceXXX() or createInstanceWithArgumentsXXX using
    // the service Mangager.
    // Need to detect proper initialisation and validity
    // for the object, so m_bIsValid indicates that the object is valid is set in ctor
    // in case of createInstanceWithArgumentsXXX() called m_bIsValid is set to reset
    // and then set to true when initialisation is complete
    bool m_bIsValid;
    // m_bInitialised ensure initialisation only takes place once.
    bool m_bInitialised;
    bool m_bIsPkgMSP;
    css::uno::Reference< css::script::provider::XScriptProvider > m_xMSPPkg;
    ProviderCache* m_pPCache;
    osl::Mutex m_mutex;
    ::rtl::OUString m_sCtxString;
};
} // namespace func_provider
#endif //_FRAMEWORK_SCRIPT_PROVIDER_XFUNCTIONPROVIDER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
