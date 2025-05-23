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


#include "comphelper/servicedecl.hxx"
#include "osl/diagnose.h"
#include "rtl/string.hxx"
#include "rtl/ustrbuf.hxx"
#include "cppuhelper/implbase2.hxx"
#include "comphelper/sequence.hxx"
#include "com/sun/star/lang/XSingleComponentFactory.hpp"
#include <vector>

using namespace com::sun::star;

namespace comphelper {
namespace service_decl {

class ServiceDecl::Factory :
        public cppu::WeakImplHelper2<lang::XSingleComponentFactory,
                                     lang::XServiceInfo>,
        private boost::noncopyable
{
public:
    explicit Factory( ServiceDecl const& rServiceDecl )
        : m_rServiceDecl(rServiceDecl) {}

    // XServiceInfo:
    virtual rtl::OUString SAL_CALL getImplementationName()
        throw (uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( rtl::OUString const& name )
        throw (uno::RuntimeException);
    virtual uno::Sequence<rtl::OUString> SAL_CALL getSupportedServiceNames()
        throw (uno::RuntimeException);
    // XSingleComponentFactory:
    virtual uno::Reference<uno::XInterface> SAL_CALL createInstanceWithContext(
        uno::Reference<uno::XComponentContext> const& xContext )
        throw (uno::Exception);
    virtual uno::Reference<uno::XInterface> SAL_CALL
    createInstanceWithArgumentsAndContext(
    uno::Sequence<uno::Any> const& args,
    uno::Reference<uno::XComponentContext> const& xContext )
        throw (uno::Exception);

private:
    virtual ~Factory();

    ServiceDecl const& m_rServiceDecl;
};

ServiceDecl::Factory::~Factory()
{
}

// XServiceInfo:
rtl::OUString ServiceDecl::Factory::getImplementationName()
    throw (uno::RuntimeException)
{
    return m_rServiceDecl.getImplementationName();
}

sal_Bool ServiceDecl::Factory::supportsService( rtl::OUString const& name )
    throw (uno::RuntimeException)
{
    return m_rServiceDecl.supportsService(name);
}

uno::Sequence<rtl::OUString> ServiceDecl::Factory::getSupportedServiceNames()
    throw (uno::RuntimeException)
{
    return m_rServiceDecl.getSupportedServiceNames();
}

// XSingleComponentFactory:
uno::Reference<uno::XInterface> ServiceDecl::Factory::createInstanceWithContext(
    uno::Reference<uno::XComponentContext> const& xContext )
    throw (uno::Exception)
{
    return m_rServiceDecl.m_createFunc(
        m_rServiceDecl, uno::Sequence<uno::Any>(), xContext );
}

uno::Reference<uno::XInterface>
ServiceDecl::Factory::createInstanceWithArgumentsAndContext(
    uno::Sequence<uno::Any > const& args,
    uno::Reference<uno::XComponentContext> const& xContext )
    throw (uno::Exception)
{
    return m_rServiceDecl.m_createFunc(
        m_rServiceDecl, args, xContext );
}

void * ServiceDecl::getFactory( sal_Char const* pImplName ) const
{
    if (rtl_str_compare(m_pImplName, pImplName) == 0) {
        lang::XSingleComponentFactory * const pFac( new Factory(*this) );
        pFac->acquire();
        return pFac;
    }
    return 0;
}

uno::Sequence<rtl::OUString> ServiceDecl::getSupportedServiceNames() const
{
    std::vector<rtl::OUString> vec;

    rtl::OString const str(m_pServiceNames);
    sal_Int32 nIndex = 0;
    do {
        rtl::OString const token( str.getToken( 0, m_cDelim, nIndex ) );
        vec.push_back( rtl::OUString( token.getStr(), token.getLength(),
                                      RTL_TEXTENCODING_ASCII_US ) );
    }
    while (nIndex >= 0);

    return comphelper::containerToSequence(vec);
}

bool ServiceDecl::supportsService( ::rtl::OUString const& name ) const
{
    rtl::OString const str(m_pServiceNames);
    sal_Int32 nIndex = 0;
    do {
        rtl::OString const token( str.getToken( 0, m_cDelim, nIndex ) );
        if (name.equalsAsciiL( token.getStr(), token.getLength() ))
            return true;
    }
    while (nIndex >= 0);
    return false;
}

rtl::OUString ServiceDecl::getImplementationName() const
{
    return rtl::OUString::createFromAscii(m_pImplName);
}

} // namespace service_decl
} // namespace comphelper

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
