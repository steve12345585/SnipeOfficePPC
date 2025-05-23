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

#ifndef SDEXT_PRESENTER_PRESENTER_BUTTON_HXX
#define SDEXT_PRESENTER_PRESENTER_BUTTON_HXX

#include "PresenterBitmapContainer.hxx"
#include "PresenterTheme.hxx"
#include <com/sun/star/awt/Rectangle.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindowListener.hpp>
#include <com/sun/star/awt/XPaintListener.hpp>
#include <com/sun/star/awt/XMouseListener.hpp>
#include <com/sun/star/awt/XMouseMotionListener.hpp>
#include <com/sun/star/rendering/XCanvas.hpp>
#include <com/sun/star/rendering/XBitmap.hpp>
#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/compbase4.hxx>
#include <boost/noncopyable.hpp>
#include <rtl/ref.hxx>

namespace sdext { namespace presenter {

class PresenterController;

namespace {
    typedef ::cppu::WeakComponentImplHelper4 <
        css::awt::XWindowListener,
        css::awt::XPaintListener,
        css::awt::XMouseListener,
        css::awt::XMouseMotionListener
    > PresenterButtonInterfaceBase;
}

/** Button for the presenter screen.  It displays a text surrounded by a
    frame.
*/
class PresenterButton
    : private ::boost::noncopyable,
      private ::cppu::BaseMutex,
      public PresenterButtonInterfaceBase
{
public:
    static ::rtl::Reference<PresenterButton> Create (
        const css::uno::Reference<css::uno::XComponentContext>& rxComponentContext,
        const ::rtl::Reference<PresenterController>& rpPresenterController,
        const ::boost::shared_ptr<PresenterTheme>& rpTheme,
        const css::uno::Reference<css::awt::XWindow>& rxParentWindow,
        const css::uno::Reference<css::rendering::XCanvas>& rxParentCanvas,
        const ::rtl::OUString& rsConfigurationName);
    ~PresenterButton (void);

    virtual void SAL_CALL disposing (void);

    void SetCenter (const css::geometry::RealPoint2D& rLocation);
    void SetCanvas (
        const css::uno::Reference<css::rendering::XCanvas>& rxParentCanvas,
        const css::uno::Reference<css::awt::XWindow>& rxParentWindow);
    css::geometry::IntegerSize2D GetSize (void);

    // XWindowListener

    virtual void SAL_CALL windowResized (const css::awt::WindowEvent& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL windowMoved (const css::awt::WindowEvent& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL windowShown (const css::lang::EventObject& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL windowHidden (const css::lang::EventObject& rEvent)
        throw (css::uno::RuntimeException);

    // XPaintListener

    virtual void SAL_CALL windowPaint (const css::awt::PaintEvent& rEvent)
        throw (css::uno::RuntimeException);

    // XMouseListener

    virtual void SAL_CALL mousePressed (const css::awt::MouseEvent& rEvent)
        throw(css::uno::RuntimeException);

    virtual void SAL_CALL mouseReleased (const css::awt::MouseEvent& rEvent)
        throw(css::uno::RuntimeException);

    virtual void SAL_CALL mouseEntered (const css::awt::MouseEvent& rEvent)
        throw(css::uno::RuntimeException);

    virtual void SAL_CALL mouseExited (const css::awt::MouseEvent& rEvent)
        throw(css::uno::RuntimeException);

    // XMouseMotionListener

    virtual void SAL_CALL mouseMoved (const css::awt::MouseEvent& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL mouseDragged (const css::awt::MouseEvent& rEvent)
        throw (css::uno::RuntimeException);

    // lang::XEventListener
    virtual void SAL_CALL disposing (const css::lang::EventObject& rEvent)
        throw (css::uno::RuntimeException);

private:
    ::rtl::Reference<PresenterController> mpPresenterController;
    ::boost::shared_ptr<PresenterTheme> mpTheme;
    css::uno::Reference<css::awt::XWindow> mxWindow;
    css::uno::Reference<css::rendering::XCanvas> mxCanvas;
    css::uno::Reference<css::drawing::XPresenterHelper> mxPresenterHelper;
    const ::rtl::OUString msText;
    const PresenterTheme::SharedFontDescriptor mpFont;
    const PresenterTheme::SharedFontDescriptor mpMouseOverFont;
    const ::rtl::OUString msAction;
    css::geometry::RealPoint2D maCenter;
    css::geometry::IntegerSize2D maButtonSize;
    PresenterBitmapDescriptor::Mode meState;
    css::uno::Reference<css::rendering::XBitmap> mxNormalBitmap;
    css::uno::Reference<css::rendering::XBitmap> mxMouseOverBitmap;

    PresenterButton (
        const css::uno::Reference<css::uno::XComponentContext>& rxComponentContext,
        const ::rtl::Reference<PresenterController>& rpPresenterController,
        const ::boost::shared_ptr<PresenterTheme>& rpTheme,
        const css::uno::Reference<css::awt::XWindow>& rxParentWindow,
        const PresenterTheme::SharedFontDescriptor& rFont,
        const PresenterTheme::SharedFontDescriptor& rMouseOverFont,
        const ::rtl::OUString& rxText,
        const ::rtl::OUString& rxAction);
    void RenderButton (
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const css::geometry::IntegerSize2D& rSize,
        const PresenterTheme::SharedFontDescriptor& rFont,
        const PresenterBitmapDescriptor::Mode eMode,
        const SharedBitmapDescriptor& rpLeft,
        const SharedBitmapDescriptor& rpCenter,
        const SharedBitmapDescriptor& rpRight);
    css::geometry::IntegerSize2D CalculateButtonSize (void);
    void Invalidate (void);
    css::uno::Reference<css::rendering::XBitmap> GetBitmap (
        const SharedBitmapDescriptor& mpIcon,
        const PresenterBitmapDescriptor::Mode eMode);
    void SetupButtonBitmaps (void);
    static css::uno::Reference<css::beans::XPropertySet> GetConfigurationProperties (
        const css::uno::Reference<css::uno::XComponentContext>& rxComponentContext,
        const ::rtl::OUString& rsConfgurationName);

    void ThrowIfDisposed (void) const
        throw (::com::sun::star::lang::DisposedException);
};

} }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
