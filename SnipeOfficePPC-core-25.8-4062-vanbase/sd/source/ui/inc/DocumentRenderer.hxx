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

#ifndef SD_DOCUMENT_RENDERER_HXX
#define SD_DOCUMENT_RENDERER_HXX

#include "ViewShellBase.hxx"

#include <com/sun/star/view/XRenderable.hpp>
#include <cppuhelper/compbase1.hxx>
#include <cppuhelper/basemutex.hxx>
#include <boost/scoped_ptr.hpp>

namespace sd {

namespace {
    typedef ::cppu::WeakComponentImplHelper1 <
        ::com::sun::star::view::XRenderable
        > DocumentRendererInterfaceBase;
}


class DocumentRenderer
    : protected ::cppu::BaseMutex,
      public DocumentRendererInterfaceBase
{
public:
    DocumentRenderer (ViewShellBase& rBase);
    virtual ~DocumentRenderer (void);

    // XRenderable
    virtual sal_Int32 SAL_CALL getRendererCount (
        const css::uno::Any& aSelection,
        const css::uno::Sequence<css::beans::PropertyValue >& xOptions)
        throw (css::lang::IllegalArgumentException, css::uno::RuntimeException);

    virtual css::uno::Sequence<css::beans::PropertyValue> SAL_CALL getRenderer (
        sal_Int32 nRenderer,
        const css::uno::Any& rSelection,
        const css::uno::Sequence<css::beans::PropertyValue>& rxOptions)
        throw (css::lang::IllegalArgumentException, css::uno::RuntimeException);

    virtual void SAL_CALL render (
        sal_Int32 nRenderer,
        const css::uno::Any& rSelection,
        const css::uno::Sequence<css::beans::PropertyValue>& rxOptions)
        throw (css::lang::IllegalArgumentException, css::uno::RuntimeException);

private:
    class Implementation;
    ::boost::scoped_ptr<Implementation> mpImpl;
};

} // end of namespace sd

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
