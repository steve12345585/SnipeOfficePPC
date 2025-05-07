/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
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


#include "ctp_panel.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/drawing/framework/XPane.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/WindowClass.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/awt/XDevice.hpp>
#include <com/sun/star/awt/XGraphics.hpp>
/** === end UNO includes === **/

#include <tools/diagnose_ex.h>

//......................................................................................................................
namespace sd { namespace colortoolpanel
{
//......................................................................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::XInterface;
    using ::com::sun::star::uno::UNO_QUERY;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::uno::UNO_SET_THROW;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::uno::makeAny;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::uno::Type;
    using ::com::sun::star::drawing::framework::XConfigurationController;
    using ::com::sun::star::drawing::framework::XResourceId;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::drawing::framework::XPane;
    using ::com::sun::star::awt::XWindow;
    using ::com::sun::star::rendering::XCanvas;
    using ::com::sun::star::lang::DisposedException;
    using ::com::sun::star::awt::XWindowPeer;
    using ::com::sun::star::lang::XMultiComponentFactory;
    using ::com::sun::star::awt::XToolkit;
    using ::com::sun::star::awt::WindowDescriptor;
    using ::com::sun::star::awt::WindowClass_SIMPLE;
    using ::com::sun::star::awt::Rectangle;
    using ::com::sun::star::awt::PaintEvent;
    using ::com::sun::star::lang::EventObject;
    using ::com::sun::star::awt::XDevice;
    using ::com::sun::star::awt::XGraphics;
    using ::com::sun::star::accessibility::XAccessible;
    /** === end UNO using === **/
    namespace WindowAttribute = ::com::sun::star::awt::WindowAttribute;
    namespace PosSize = ::com::sun::star::awt::PosSize;

    //==================================================================================================================
    //= helpers
    //==================================================================================================================
    namespace
    {
        Reference< XWindow > lcl_createPlainWindow_nothrow( const Reference< XComponentContext >& i_rContext,
            const Reference< XWindowPeer >& i_rParentWindow )
        {
            try
            {
                ENSURE_OR_THROW( i_rContext.is(), "illegal component context" );
                Reference< XMultiComponentFactory > xFactory( i_rContext->getServiceManager(), UNO_SET_THROW );
                Reference< XToolkit > xToolkit( xFactory->createInstanceWithContext(
                    ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.Toolkit" ) ),
                    i_rContext
                ), UNO_QUERY_THROW );

                WindowDescriptor aWindow;
                aWindow.Type = WindowClass_SIMPLE;
                aWindow.WindowServiceName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "window" ) );
                aWindow.Parent = i_rParentWindow;
                aWindow.WindowAttributes = WindowAttribute::BORDER;

                Reference< XWindowPeer > xWindow( xToolkit->createWindow( aWindow ), UNO_SET_THROW );
                return Reference< XWindow >( xWindow, UNO_QUERY_THROW );
            }
            catch( const Exception& )
            {
                DBG_UNHANDLED_EXCEPTION();
            }
            return NULL;
        }
    }
    //==================================================================================================================
    //= class SingleColorPanel
    //==================================================================================================================
    //------------------------------------------------------------------------------------------------------------------
    SingleColorPanel::SingleColorPanel( const Reference< XComponentContext >& i_rContext,
            const Reference< XConfigurationController >& i_rConfigController, const Reference< XResourceId >& i_rResourceId )
        :SingleColorPanel_Base( m_aMutex )
        ,m_xContext( i_rContext )
        ,m_xResourceId( i_rResourceId )
        ,m_xWindow()
    {
        ENSURE_OR_THROW( i_rConfigController.is(), "invalid configuration controller" );
        ENSURE_OR_THROW( m_xResourceId.is(), "invalid resource id" );

        // retrieve the parent window for our to-be-created pane window
        Reference< XWindow > xParentWindow;
        Reference< XWindowPeer > xParentPeer;
        try
        {
            Reference< XResource > xAnchor( i_rConfigController->getResource( m_xResourceId->getAnchor() ), UNO_SET_THROW );
            Reference< XPane > xAnchorPane( xAnchor, UNO_QUERY_THROW );
            xParentWindow.set( xAnchorPane->getWindow(), UNO_SET_THROW );
            xParentPeer.set( xParentWindow, UNO_QUERY_THROW );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        osl_incrementInterlockedCount( &m_refCount );
        if ( xParentWindow.is() )
        {
            m_xWindow = lcl_createPlainWindow_nothrow( m_xContext, xParentPeer );
            m_xWindow->addPaintListener( this );
            if ( m_xWindow.is() )
            {
                const Rectangle aPanelAnchorSize( xParentWindow->getPosSize() );
                m_xWindow->setPosSize( 0, 0, aPanelAnchorSize.Width, aPanelAnchorSize.Height, PosSize::POSSIZE );
                m_xWindow->setVisible( sal_True );
            }
        }
        osl_decrementInterlockedCount( &m_refCount );
    }

    //------------------------------------------------------------------------------------------------------------------
    SingleColorPanel::~SingleColorPanel()
    {
    }

    //------------------------------------------------------------------------------------------------------------------
    Reference< XWindow > SAL_CALL SingleColorPanel::getWindow(  ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !m_xWindow.get() )
            throw DisposedException( ::rtl::OUString(), *this );
        return m_xWindow;
    }

    //------------------------------------------------------------------------------------------------------------------
    Reference< XAccessible > SAL_CALL SingleColorPanel::createAccessible( const Reference< XAccessible >& i_rParentAccessible ) throw (RuntimeException)
    {
        (void)i_rParentAccessible;
        return Reference< XAccessible >( m_xWindow, UNO_QUERY );
        // TODO: this is, strictly, not correct, as we ignore i_ParentAccessible here. If you are not doing a sample
        // extension only, you'll want to do this correctly ....
    }

    //------------------------------------------------------------------------------------------------------------------
    Reference< XResourceId > SAL_CALL SingleColorPanel::getResourceId(  ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !m_xWindow.is() )
            throw DisposedException( ::rtl::OUString(), *this );
        return m_xResourceId;
    }

    //------------------------------------------------------------------------------------------------------------------
    ::sal_Bool SAL_CALL SingleColorPanel::isAnchorOnly(  ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !m_xWindow.is() )
            throw DisposedException( ::rtl::OUString(), *this );
        return sal_False;
    }

    //------------------------------------------------------------------------------------------------------------------
    void SAL_CALL SingleColorPanel::windowPaint( const PaintEvent& i_rEvent ) throw (RuntimeException)
    {
        try
        {
            const Reference< XDevice > xDevice( i_rEvent.Source, UNO_QUERY_THROW );
            const Reference< XGraphics > xGraphics( xDevice->createGraphics(), UNO_SET_THROW );
            xGraphics->setFillColor( 0x80 << 8 );
            xGraphics->setLineColor( 0x80 << 16 );

            const Reference< XWindow > xWindow( i_rEvent.Source, UNO_QUERY_THROW );
            const Rectangle aWindowRect( xWindow->getPosSize() );
            xGraphics->drawRect( 0, 0, aWindowRect.Width - 1, aWindowRect.Height - 1 );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    void SAL_CALL SingleColorPanel::disposing( const EventObject& i_rSource ) throw (RuntimeException)
    {
        (void)i_rSource;
    }

    //------------------------------------------------------------------------------------------------------------------
    void SAL_CALL SingleColorPanel::disposing()
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !m_xWindow.is() )
            // already disposed
            return;
        m_xWindow->removePaintListener( this );
        try
        {
            Reference< XComponent > xWindowComp( m_xWindow, UNO_QUERY_THROW );
            xWindowComp->dispose();
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        m_xWindow.clear();
    }

//......................................................................................................................
} } // namespace sd::colortoolpanel
//......................................................................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
