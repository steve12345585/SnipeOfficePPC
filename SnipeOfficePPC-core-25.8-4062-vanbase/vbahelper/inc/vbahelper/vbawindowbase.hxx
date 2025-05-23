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

#ifndef VBA_WINDOWBASE_HXX
#define VBA_WINDOWBASE_HXX

#include <ooo/vba/XWindowBase.hpp>
#include <com/sun/star/awt/XWindow2.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <vbahelper/vbahelperinterface.hxx>

typedef InheritedHelperInterfaceImpl1< ov::XWindowBase > WindowBaseImpl_BASE;

class VBAHELPER_DLLPUBLIC VbaWindowBase : public WindowBaseImpl_BASE
{
public:
    VbaWindowBase(
        const css::uno::Reference< ov::XHelperInterface >& xParent,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const css::uno::Reference< css::frame::XModel >& xModel,
        const css::uno::Reference< css::frame::XController >& xController )
        throw (css::uno::RuntimeException);
    VbaWindowBase(
        css::uno::Sequence< css::uno::Any > const& aArgs,
        css::uno::Reference< css::uno::XComponentContext > const& xContext )
        throw (css::uno::RuntimeException);

    // XWindowBase
    virtual sal_Int32 SAL_CALL getHeight() throw (css::uno::RuntimeException) ;
    virtual void SAL_CALL setHeight( sal_Int32 _height ) throw (css::uno::RuntimeException) ;
    virtual sal_Int32 SAL_CALL getLeft() throw (css::uno::RuntimeException) ;
    virtual void SAL_CALL setLeft( sal_Int32 _left ) throw (css::uno::RuntimeException) ;
    virtual sal_Int32 SAL_CALL getTop() throw (css::uno::RuntimeException) ;
    virtual void SAL_CALL setTop( sal_Int32 _top ) throw (css::uno::RuntimeException) ;
    virtual sal_Bool SAL_CALL getVisible() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setVisible( sal_Bool _visible ) throw (css::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getWidth() throw (css::uno::RuntimeException) ;
    virtual void SAL_CALL setWidth( sal_Int32 _width ) throw (css::uno::RuntimeException) ;

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();

protected:
    css::uno::Reference< css::frame::XController > getController() throw (css::uno::RuntimeException);
    css::uno::Reference< css::awt::XWindow > getWindow() throw (css::uno::RuntimeException);
    css::uno::Reference< css::awt::XWindow2 > getWindow2() throw (css::uno::RuntimeException);

    css::uno::Reference< css::frame::XModel > m_xModel;

private:
    void construct( const css::uno::Reference< css::frame::XController >& xController ) throw (css::uno::RuntimeException);

    css::uno::WeakReference< css::frame::XController > m_xController;
    css::uno::WeakReference< css::awt::XWindow > m_xWindow;
};

#endif //VBA_WINDOWBASE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
