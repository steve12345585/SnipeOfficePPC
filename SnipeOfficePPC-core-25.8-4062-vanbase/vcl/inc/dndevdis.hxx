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

#ifndef _DNDEVDIS_HXX_
#define _DNDEVDIS_HXX_

#include <com/sun/star/datatransfer/dnd/XDropTargetListener.hpp>
#include <com/sun/star/datatransfer/dnd/XDropTargetDragContext.hpp>

#include <com/sun/star/datatransfer/dnd/XDragGestureRecognizer.hpp>
#include <cppuhelper/implbase3.hxx>
#include <vcl/window.hxx>

class DNDEventDispatcher: public ::cppu::WeakImplHelper3<
    ::com::sun::star::datatransfer::dnd::XDropTargetListener,
    ::com::sun::star::datatransfer::dnd::XDropTargetDragContext,
    ::com::sun::star::datatransfer::dnd::XDragGestureListener >
{
    Window * m_pTopWindow;
    Window * m_pCurrentWindow;

    ::osl::Mutex m_aMutex;
    ::com::sun::star::uno::Sequence< ::com::sun::star::datatransfer::DataFlavor > m_aDataFlavorList;

    Window* findTopLevelWindow(Point location);
    /*
     * fire the events on the dnd listener container of the specified window
     */

    sal_Int32 fireDragEnterEvent( Window *pWindow, const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetDragContext >& xContext,
        const sal_Int8 nDropAction, const Point& rLocation, const sal_Int8 nSourceAction,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::datatransfer::DataFlavor >& aFlavorList ) throw(::com::sun::star::uno::RuntimeException);

    sal_Int32 fireDragOverEvent( Window *pWindow, const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetDragContext >& xContext,
        const sal_Int8 nDropAction, const Point& rLocation, const sal_Int8 nSourceAction ) throw(::com::sun::star::uno::RuntimeException);

    sal_Int32 fireDragExitEvent( Window *pWindow ) throw(::com::sun::star::uno::RuntimeException);

    sal_Int32 fireDropActionChangedEvent( Window *pWindow, const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetDragContext >& xContext,
        const sal_Int8 nDropAction, const Point& rLocation, const sal_Int8 nSourceAction ) throw(::com::sun::star::uno::RuntimeException);

    sal_Int32 fireDropEvent( Window *pWindow, const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetDropContext >& xContext,
        const sal_Int8 nDropAction, const Point& rLocation, const sal_Int8 nSourceAction,
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable >& xTransferable ) throw(::com::sun::star::uno::RuntimeException);

    sal_Int32 fireDragGestureEvent( Window *pWindow, const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDragSource >& xSource,
        const ::com::sun::star::uno::Any event, const Point& rOrigin, const sal_Int8 nDragAction )throw(::com::sun::star::uno::RuntimeException);

public:

    DNDEventDispatcher( Window * pTopWindow );
    virtual ~DNDEventDispatcher();

    /*
     * XDropTargetDragContext
     */

    virtual void SAL_CALL acceptDrag( sal_Int8 dropAction ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL rejectDrag() throw(::com::sun::star::uno::RuntimeException);

    /*
     * XDropTargetListener
     */

    virtual void SAL_CALL drop( const ::com::sun::star::datatransfer::dnd::DropTargetDropEvent& dtde ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL dragEnter( const ::com::sun::star::datatransfer::dnd::DropTargetDragEnterEvent& dtdee ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL dragExit( const ::com::sun::star::datatransfer::dnd::DropTargetEvent& dte ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL dragOver( const ::com::sun::star::datatransfer::dnd::DropTargetDragEvent& dtde ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL dropActionChanged( const ::com::sun::star::datatransfer::dnd::DropTargetDragEvent& dtde ) throw(::com::sun::star::uno::RuntimeException);

    /*
     * XDragGestureListener
     */

    virtual void SAL_CALL dragGestureRecognized( const ::com::sun::star::datatransfer::dnd::DragGestureEvent& dge ) throw(::com::sun::star::uno::RuntimeException);


    /*
     * XEventListener
     */

    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& eo ) throw(::com::sun::star::uno::RuntimeException);
};

//==================================================================================================
//
//==================================================================================================

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
