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
#ifndef VBA_GLOBAL_BASE_HXX
#define VBA_GLOBAL_BASE_HXX

#include "vbahelperinterface.hxx"
#include <ooo/vba/XGlobalsBase.hpp>

typedef InheritedHelperInterfaceImpl1< ov::XGlobalsBase > Globals_BASE;
class VBAHELPER_DLLPUBLIC VbaGlobalsBase : public Globals_BASE
{
protected:
    rtl::OUString msDocCtxName;
    rtl::OUString msApplication;

    bool hasServiceName( const rtl::OUString& serviceName );
    void init(  const css::uno::Sequence< css::beans::PropertyValue >& aInitArgs );

public:
    VbaGlobalsBase( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const rtl::OUString& sDocCtxName );
    virtual ~VbaGlobalsBase();
    // XMultiServiceFactory
    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstance( const ::rtl::OUString& aServiceSpecifier ) throw (css::uno::Exception, css::uno::RuntimeException);
    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstanceWithArguments( const ::rtl::OUString& ServiceSpecifier, const css::uno::Sequence< css::uno::Any >& Arguments ) throw (css::uno::Exception, css::uno::RuntimeException);
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getAvailableServiceNames(  ) throw (css::uno::RuntimeException);
};
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
