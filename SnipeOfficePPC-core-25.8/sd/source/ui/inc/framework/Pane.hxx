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

#ifndef SD_FRAMEWORK_PANE_HXX
#define SD_FRAMEWORK_PANE_HXX

#include "MutexOwner.hxx"

#include <com/sun/star/drawing/framework/XPane.hpp>
#include <com/sun/star/drawing/framework/XPane2.hpp>
#include <com/sun/star/drawing/framework/TabBarButton.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <cppuhelper/compbase3.hxx>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

class Window;

namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;

namespace {

typedef ::cppu::WeakComponentImplHelper3 <
    ::com::sun::star::drawing::framework::XPane,
    ::com::sun::star::drawing::framework::XPane2,
      ::com::sun::star::lang::XUnoTunnel
    > PaneInterfaceBase;

} // end of anonymous namespace.

namespace sd { namespace framework {

/** A pane is a wrapper for a window and possibly for a tab bar (for view
    switching).  Panes are unique resources.

    This class has two responsibilities:
    1. It implements the XPane interface.  This is the most important
    interface of this class for API based views (of which there not that
    many yet) because it gives access to the XWindow.
    2. It gives access to the underlying VCL Window by implementing the
    XUnoTunnel interface.  This is necessary at the moment and in the
    foreseeable future because many parts of the Draw and Impress views rely
    on direct access on the Window class.
*/
class Pane
    : protected MutexOwner,
      public PaneInterfaceBase
{
public:
    /** Create a new Pane object that wrapps the given window.
        @param rsPaneURL
            The URL that is used by the configuration to identify the pane.
            The given URL has to be valid.
        @param pWindow
            The VCL Window (usually this really is an sd::Window) that is
            wrapped by the new Pane object.  The given pointer must not be
            NULL.
    */
    Pane (
        const ::com::sun::star::uno::Reference<
            com::sun::star::drawing::framework::XResourceId>& rxPaneId,
        ::Window* pWindow)
        throw ();
    virtual ~Pane (void) throw();

    virtual void SAL_CALL disposing (void);

    static const ::com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId (void);

    /** This method is typically used together with the XUnoTunnel to obtain
        a Window pointer from an XPane object.
    */
    virtual ::Window* GetWindow (void);


    //----- XPane -------------------------------------------------------------

    /** For a UNO API based implementation of a view this may the most
        important method of this class because the view is only interested
        in the window of the pane.
    */
    virtual cssu::Reference<css::awt::XWindow>
        SAL_CALL getWindow (void)
        throw (cssu::RuntimeException);

    virtual cssu::Reference<css::rendering::XCanvas>
        SAL_CALL getCanvas (void)
        throw (cssu::RuntimeException);


    //----- XPane2 -------------------------------------------------------------

    virtual sal_Bool SAL_CALL isVisible (void)
        throw (cssu::RuntimeException);

    virtual void SAL_CALL setVisible (sal_Bool bIsVisible)
        throw (cssu::RuntimeException);

    virtual cssu::Reference<css::accessibility::XAccessible> SAL_CALL getAccessible (void)
        throw (cssu::RuntimeException);

    virtual void SAL_CALL setAccessible (
        const cssu::Reference<css::accessibility::XAccessible>& rxAccessible)
        throw (cssu::RuntimeException);


    //----- XResource ---------------------------------------------------------

    virtual ::com::sun::star::uno::Reference<com::sun::star::drawing::framework::XResourceId>
        SAL_CALL getResourceId (void)
        throw (::com::sun::star::uno::RuntimeException);

    /** For the typical pane it makes no sense to be dislayed without a
        view.  Therefore this default implementation returns always <TRUE/>.
    */
    virtual sal_Bool SAL_CALL isAnchorOnly (void)
        throw (com::sun::star::uno::RuntimeException);


    //----- XUnoTunnel --------------------------------------------------------

    virtual sal_Int64 SAL_CALL getSomething (const com::sun::star::uno::Sequence<sal_Int8>& rId)
        throw (com::sun::star::uno::RuntimeException);


protected:
    ::com::sun::star::uno::Reference<com::sun::star::drawing::framework::XResourceId> mxPaneId;
    ::Window* mpWindow;
    ::com::sun::star::uno::Reference<com::sun::star::awt::XWindow> mxWindow;
    ::com::sun::star::uno::Reference<com::sun::star::rendering::XCanvas> mxCanvas;

    /** Overload this method, not getCanvas(), when you want to provide a
        different canvas.
    */
    virtual ::com::sun::star::uno::Reference<com::sun::star::rendering::XCanvas>
        CreateCanvas (void)
        throw (::com::sun::star::uno::RuntimeException);

    /** Throw DisposedException when the object has already been disposed or
        is currently being disposed.  Otherwise this method returns
        normally.
    */
    void ThrowIfDisposed (void) const
        throw (::com::sun::star::lang::DisposedException);
};

} } // end of namespace sd::framework

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
