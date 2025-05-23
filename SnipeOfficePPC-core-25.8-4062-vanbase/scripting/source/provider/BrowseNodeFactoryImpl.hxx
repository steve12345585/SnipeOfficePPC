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

#include <rtl/ustring.hxx>
#include <cppuhelper/implbase2.hxx>

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>

#include <com/sun/star/script/browse/XBrowseNode.hpp>
#include <com/sun/star/script/browse/BrowseNodeTypes.hpp>
#include <com/sun/star/script/browse/XBrowseNodeFactory.hpp>

namespace browsenodefactory
{

class BrowseNodeFactoryImpl :
    public ::cppu::WeakImplHelper2 <
        css::script::browse::XBrowseNodeFactory,
        css::lang::XServiceInfo >
{
private:
    css::uno::Reference< css::uno::XComponentContext > m_xComponentContext;
    css::uno::Reference< css::script::browse::XBrowseNode > m_xSelectorBrowseNode;

protected:
    virtual ~BrowseNodeFactoryImpl();

public:
    BrowseNodeFactoryImpl(
        css::uno::Reference< css::uno::XComponentContext > const & xComponentContext );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName()
        throw ( css::uno::RuntimeException );

    virtual sal_Bool SAL_CALL
        supportsService( ::rtl::OUString const & serviceName )
            throw ( css::uno::RuntimeException );

    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL
        getSupportedServiceNames()
            throw ( css::uno::RuntimeException );

    // XBrowseNodeFactory
    virtual css::uno::Reference< css::script::browse::XBrowseNode > SAL_CALL
        createView( sal_Int16 viewType )
            throw ( css::uno::RuntimeException );
    private:
    css::uno::Reference< css::script::browse::XBrowseNode >
        getSelectorHierarchy()
            throw ( css::uno::RuntimeException );

    css::uno::Reference< css::script::browse::XBrowseNode >
        getOrganizerHierarchy()
            throw ( css::uno::RuntimeException );
};


} // namespace browsenodefactory

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
