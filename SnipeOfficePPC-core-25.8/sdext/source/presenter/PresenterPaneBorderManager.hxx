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

#ifndef SDEXT_PRESENTER_PRESENTER_PANE_BORDER_MANAGER_HXX
#define SDEXT_PRESENTER_PRESENTER_PANE_BORDER_MANAGER_HXX

// The body of this file is only used when PresenterWindowManager defines
// the preprocessor symbol ENABLE_PANE_RESIZING, which by default is not the
// case.
#ifdef ENABLE_PANE_RESIZING

#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/compbase3.hxx>
#include <com/sun/star/awt/Point.hpp>
#include <com/sun/star/awt/Size.hpp>
#include <com/sun/star/awt/XGraphics.hpp>
#include <com/sun/star/awt/XMouseListener.hpp>
#include <com/sun/star/awt/XMouseMotionListener.hpp>
#include <com/sun/star/awt/XPointer.hpp>
#include <com/sun/star/awt/XWindowListener.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/drawing/XPresenterHelper.hpp>
#include <com/sun/star/drawing/framework/XPane.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/rendering/XCanvas.hpp>
#include <rtl/ref.hxx>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace css = ::com::sun::star;

namespace sdext { namespace presenter {

class PresenterController;

namespace {
    typedef ::cppu::WeakComponentImplHelper3 <
        css::lang::XInitialization,
        css::awt::XMouseListener,
        css::awt::XMouseMotionListener
    > PresenterPaneBorderManagerInterfaceBase;
}

/** Manage the interactive moving and resizing of panes.
*/
class PresenterPaneBorderManager
    : private ::boost::noncopyable,
      protected ::cppu::BaseMutex,
      public PresenterPaneBorderManagerInterfaceBase
{
public:
    PresenterPaneBorderManager (
        const css::uno::Reference<css::uno::XComponentContext>& rxContext,
        const ::rtl::Reference<PresenterController>& rpPresenterController);
    virtual ~PresenterPaneBorderManager (void);

    virtual void SAL_CALL disposing (void);

    static ::rtl::OUString getImplementationName_static (void);
    static css::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_static (void);
    static css::uno::Reference<css::uno::XInterface> Create(
        const css::uno::Reference<css::uno::XComponentContext>& rxContext)
        SAL_THROW((css::uno::Exception));

    // XInitialization

    virtual void SAL_CALL initialize (const css::uno::Sequence<css::uno::Any>& rArguments)
        throw (css::uno::Exception, css::uno::RuntimeException);

    // XMouseListener

    virtual void SAL_CALL mousePressed (const css::awt::MouseEvent& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL mouseReleased (const css::awt::MouseEvent& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL mouseEntered (const css::awt::MouseEvent& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL mouseExited (const css::awt::MouseEvent& rEvent)
        throw (css::uno::RuntimeException);

    // XMouseMotionListener

    virtual void SAL_CALL mouseMoved (const css::awt::MouseEvent& rEvent)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL mouseDragged (const css::awt::MouseEvent& rEvent)
        throw (css::uno::RuntimeException);

    // lang::XEventListener
    virtual void SAL_CALL disposing (const css::lang::EventObject& rEvent)
        throw (css::uno::RuntimeException);

private:
    enum BorderElement { Top, TopLeft, TopRight, Left, Right, BottomLeft, BottomRight, Bottom,
                         Content, Outside };

    ::rtl::Reference<PresenterController> mpPresenterController;
    css::uno::Reference<css::uno::XComponentContext> mxComponentContext;
    css::uno::Reference<css::drawing::XPresenterHelper> mxPresenterHelper;
    /** The parent window is stored so that it can be invalidated when one
        of its children is resized or moved.  It is assumed to be the parent
        window of all outer windows stored in maWindowList.
    */
    css::uno::Reference<css::awt::XWindow> mxParentWindow;
    typedef ::std::pair<css::uno::Reference<css::awt::XWindow>,
        css::uno::Reference<css::awt::XWindow> > WindowDescriptor;
    typedef ::std::vector<WindowDescriptor> WindowList;
    WindowList maWindowList;

    sal_Int32 mnPointerType;
    css::awt::Point maDragAnchor;
    BorderElement meDragType;
    css::uno::Reference<css::awt::XWindow> mxOuterDragWindow;
    css::uno::Reference<css::awt::XWindow> mxInnerDragWindow;
    css::uno::Reference<css::awt::XPointer> mxPointer;

    BorderElement ClassifyBorderElementUnderMouse (
        const css::uno::Reference<css::awt::XWindow>& rxOuterDragWindow,
        const css::uno::Reference<css::awt::XWindow>& rxInnerDragWindow,
        const css::awt::Point aPosition) const;
    void CreateWindows (const css::uno::Reference<css::awt::XWindow>& rxParentWindow);
    void CaptureMouse (const css::uno::Reference<css::awt::XWindow>& rxWindow);
    void ReleaseMouse (const css::uno::Reference<css::awt::XWindow>& rxWindow);

    /** This method throws a DisposedException when the object has already been
        disposed.
    */
    void ThrowIfDisposed (void)
        throw (css::lang::DisposedException);
};

} } // end of namespace ::sd::presenter

#endif // ENABLE_PANE_RESIZING

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
