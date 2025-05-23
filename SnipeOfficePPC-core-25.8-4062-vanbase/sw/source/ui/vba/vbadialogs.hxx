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
#ifndef SW_VBA_DIALOGS_HXX
#define SW_VBA_DIALOGS_HXX

#include <com/sun/star/uno/XComponentContext.hpp>
#include <ooo/vba/word/XDialogs.hpp>
#include <ooo/vba/XCollection.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <vbahelper/vbadialogsbase.hxx>
#include <cppuhelper/implbase1.hxx>

typedef cppu::ImplInheritanceHelper1< VbaDialogsBase, ov::word::XDialogs > SwVbaDialogs_BASE;

class SwVbaDialogs : public SwVbaDialogs_BASE
{
public:
    SwVbaDialogs( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > &xContext, const css::uno::Reference< css::frame::XModel >& xModel ): SwVbaDialogs_BASE( xParent, xContext, xModel ) {}
    virtual ~SwVbaDialogs() {}

    // XCollection
    virtual css::uno::Any SAL_CALL Item( const css::uno::Any& Index ) throw (css::uno::RuntimeException);

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif /* SW_VBA_DIALOGS_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
