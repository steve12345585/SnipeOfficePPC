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

#include "sqlcommanddesign.hxx"
#include "formstrings.hxx"
#include "formresid.hrc"
#include "modulepcr.hxx"
#include "unourl.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XTopWindow.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/frame/XTitle.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/lang/NullPointerException.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/frame/XFramesSupplier.hpp>
#include <com/sun/star/sdbc/XConnection.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/sdb/CommandType.hpp>
/** === end UNO includes === **/

#include <svtools/localresaccess.hxx>
#include <tools/diagnose_ex.h>
#include <osl/diagnose.h>

//........................................................................
namespace pcr
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::lang::XMultiComponentFactory;
    using ::com::sun::star::beans::PropertyChangeEvent;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::frame::XFrame;
    using ::com::sun::star::awt::XTopWindow;
    using ::com::sun::star::awt::XWindow;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::uno::UNO_QUERY;
    using ::com::sun::star::beans::PropertyValue;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::lang::XComponent;
    using ::com::sun::star::frame::XComponentLoader;
    using ::com::sun::star::beans::XPropertySet;
    using ::com::sun::star::beans::XPropertySetInfo;
    using ::com::sun::star::frame::XController;
    using ::com::sun::star::frame::XTitle;
    using ::com::sun::star::lang::EventObject;
    using ::com::sun::star::lang::NullPointerException;
    using ::com::sun::star::lang::DisposedException;
    using ::com::sun::star::uno::makeAny;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::frame::XFramesSupplier;
    using ::com::sun::star::frame::XFrames;
    using ::com::sun::star::util::XCloseable;
    using ::com::sun::star::uno::TypeClass_STRING;
    using ::com::sun::star::lang::XMultiServiceFactory;
    using ::com::sun::star::frame::XDispatchProvider;
    using ::com::sun::star::frame::XDispatch;
    using ::com::sun::star::uno::Any;
    /** === end UNO using === **/
    namespace FrameSearchFlag = ::com::sun::star::frame::FrameSearchFlag;
    namespace CommandType = ::com::sun::star::sdb::CommandType;

    //====================================================================
    //= ISQLCommandAdapter
    //====================================================================
    //--------------------------------------------------------------------
    ISQLCommandAdapter::~ISQLCommandAdapter()
    {
    }

    //====================================================================
    //= SQLCommandDesigner
    //====================================================================
    //--------------------------------------------------------------------
    SQLCommandDesigner::SQLCommandDesigner( const Reference< XComponentContext >& _rxContext,
            const ::rtl::Reference< ISQLCommandAdapter >& _rxPropertyAdapter,
            const ::dbtools::SharedConnection& _rConnection, const Link& _rCloseLink )
        :m_xContext( _rxContext )
        ,m_xConnection( _rConnection )
        ,m_xObjectAdapter( _rxPropertyAdapter )
        ,m_aCloseLink( _rCloseLink )
    {
        if ( m_xContext.is() )
            m_xORB = m_xContext->getServiceManager();
        if ( !m_xORB.is() || !_rxPropertyAdapter.is() || !m_xConnection.is() )
            throw NullPointerException();

        impl_doOpenDesignerFrame_nothrow();
    }

    //--------------------------------------------------------------------
    SQLCommandDesigner::~SQLCommandDesigner()
    {
    }

    //--------------------------------------------------------------------
    void SAL_CALL SQLCommandDesigner::propertyChange( const PropertyChangeEvent& Event ) throw (RuntimeException)
    {
        OSL_ENSURE( m_xDesigner.is() && ( Event.Source == m_xDesigner ), "SQLCommandDesigner::propertyChange: where did this come from?" );

        if ( m_xDesigner.is() && ( Event.Source == m_xDesigner ) )
        {
            try
            {
                if ( PROPERTY_ACTIVECOMMAND == Event.PropertyName )
                {
                    ::rtl::OUString sCommand;
                    OSL_VERIFY( Event.NewValue >>= sCommand );
                    m_xObjectAdapter->setSQLCommand( sCommand );
                }
                else if ( PROPERTY_ESCAPE_PROCESSING == Event.PropertyName )
                {
                    sal_Bool bEscapeProcessing( sal_False );
                    OSL_VERIFY( Event.NewValue >>= bEscapeProcessing );
                    m_xObjectAdapter->setEscapeProcessing( bEscapeProcessing );
                }
            }
            catch( const RuntimeException& ) { throw; }
            catch( const Exception& )
            {
                // not allowed to leave, so silence it
                DBG_UNHANDLED_EXCEPTION();
            }
        }
    }

    //--------------------------------------------------------------------
    void SAL_CALL SQLCommandDesigner::disposing( const EventObject& Source ) throw (RuntimeException)
    {
        if ( m_xDesigner.is() && ( Source.Source == m_xDesigner ) )
        {
            impl_designerClosed_nothrow();
            m_xDesigner.clear();
        }
    }

    //--------------------------------------------------------------------
    void SQLCommandDesigner::dispose()
    {
        if ( impl_isDisposed() )
            return;

        if ( isActive() )
            impl_closeDesigner_nothrow();

        m_xConnection.clear();
        m_xContext.clear();
        m_xORB.clear();
    }

    //--------------------------------------------------------------------
    void SQLCommandDesigner::impl_checkDisposed_throw() const
    {
        if ( impl_isDisposed() )
            throw DisposedException();
    }

    //--------------------------------------------------------------------
    void SQLCommandDesigner::raise() const
    {
        impl_checkDisposed_throw();
        impl_raise_nothrow();
    }

    //------------------------------------------------------------------------
    bool SQLCommandDesigner::suspend() const
    {
        impl_checkDisposed_throw();
        return impl_trySuspendDesigner_nothrow();
    }

    //--------------------------------------------------------------------
    void SQLCommandDesigner::impl_raise_nothrow() const
    {
        OSL_PRECOND( isActive(), "SQLCommandDesigner::impl_raise_nothrow: not active!" );
        if ( !isActive() )
            return;

        try
        {
            // activate the frame for this component
            Reference< XFrame >     xFrame( m_xDesigner->getFrame(), UNO_QUERY_THROW );
            Reference< XWindow >    xWindow( xFrame->getContainerWindow(), UNO_QUERY_THROW );
            Reference< XTopWindow > xTopWindow( xWindow, UNO_QUERY_THROW );

            xTopWindow->toFront();
            xWindow->setFocus();
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    //--------------------------------------------------------------------
    void SQLCommandDesigner::impl_doOpenDesignerFrame_nothrow()
    {
        OSL_PRECOND( !isActive(),
            "SQLCommandDesigner::impl_doOpenDesignerFrame_nothrow: already active!" );
        OSL_PRECOND( m_xConnection.is(), "SQLCommandDesigner::impl_doOpenDesignerFrame_nothrow: this will crash!" );
        osl_incrementInterlockedCount(&m_refCount);

        try
        {
            // for various reasons, we don't want the new frame to appear in the desktop's frame list
            // thus, we create a blank frame at the desktop, remove it from the desktop's frame list
            // immediately, and then load the component into this blank (and now parent-less) frame
            Reference< XComponentLoader > xLoader( impl_createEmptyParentlessTask_nothrow(), UNO_QUERY_THROW );
            Sequence< PropertyValue > aArgs( 5 );
            aArgs[0].Name = PROPERTY_ACTIVE_CONNECTION;
            aArgs[0].Value <<= m_xConnection.getTyped();

            aArgs[1].Name  = PROPERTY_COMMAND;
            aArgs[1].Value <<= m_xObjectAdapter->getSQLCommand();
            aArgs[2].Name  = PROPERTY_COMMANDTYPE;
            aArgs[2].Value <<= (sal_Int32)CommandType::COMMAND;
            aArgs[3].Name  = PROPERTY_ESCAPE_PROCESSING;
            aArgs[3].Value <<= m_xObjectAdapter->getEscapeProcessing();

            aArgs[4].Name  = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "GraphicalDesign" ) );
            aArgs[4].Value <<= m_xObjectAdapter->getEscapeProcessing();

            Reference< XComponent > xQueryDesign = xLoader->loadComponentFromURL(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".component:DB/QueryDesign" ) ),
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "_self" ) ),
                FrameSearchFlag::TASKS | FrameSearchFlag::CREATE,
                aArgs
            );

            // remember this newly loaded component - we need to care for it e.g. when we're suspended
            m_xDesigner = m_xDesigner.query( xQueryDesign );
            OSL_ENSURE( m_xDesigner.is() || !xQueryDesign.is(), "SQLCommandDesigner::impl_doOpenDesignerFrame_nothrow: the component is expected to be a controller!" );
            if ( m_xDesigner.is() )
            {
                Reference< XPropertySet > xQueryDesignProps( m_xDesigner, UNO_QUERY );
                OSL_ENSURE( xQueryDesignProps.is(), "SQLCommandDesigner::impl_doOpenDesignerFrame_nothrow: the controller should have properties!" );
                if ( xQueryDesignProps.is() )
                {
                    xQueryDesignProps->addPropertyChangeListener( PROPERTY_ACTIVECOMMAND, this );
                    xQueryDesignProps->addPropertyChangeListener( PROPERTY_ESCAPE_PROCESSING, this );
                }
            }

            // get the frame which we just opened and set it's title
            Reference< XTitle> xTitle(xQueryDesign,UNO_QUERY);
            if ( xTitle.is() )
            {
                ::svt::OLocalResourceAccess aEnumStrings( PcrRes( RID_RSC_ENUM_COMMAND_TYPE ), RSC_RESOURCE );
                ::rtl::OUString sDisplayName = String( PcrRes( CommandType::COMMAND + 1 ) );
                xTitle->setTitle( sDisplayName );
            }
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
            m_xDesigner.clear();
        }
        osl_decrementInterlockedCount(&m_refCount);
    }

    //------------------------------------------------------------------------
    Reference< XFrame > SQLCommandDesigner::impl_createEmptyParentlessTask_nothrow( ) const
    {
        OSL_PRECOND( m_xORB.is(), "SQLCommandDesigner::impl_createEmptyParentlessTask_nothrow: this will crash!" );

        Reference< XFrame > xFrame;
        try
        {
            Reference< XInterface      > xDesktop          ( m_xORB->createInstanceWithContext( SERVICE_DESKTOP, m_xContext ) );
            Reference< XFrame          > xDesktopFrame     ( xDesktop,      UNO_QUERY_THROW );
            Reference< XFramesSupplier > xSuppDesktopFrames( xDesktopFrame, UNO_QUERY_THROW );

            Reference< XFrames > xDesktopFramesCollection( xSuppDesktopFrames->getFrames(), UNO_QUERY_THROW );
            xFrame = xDesktopFrame->findFrame( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "_blank" ) ), FrameSearchFlag::CREATE );
            OSL_ENSURE( xFrame.is(), "SQLCommandDesigner::impl_createEmptyParentlessTask_nothrow: could not create an empty frame!" );
            xDesktopFramesCollection->remove( xFrame );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return xFrame;
    }

    //------------------------------------------------------------------------
    void SQLCommandDesigner::impl_designerClosed_nothrow()
    {
        if ( m_aCloseLink.IsSet() )
            m_aCloseLink.Call( this );
    }

    //------------------------------------------------------------------------
    void SQLCommandDesigner::impl_closeDesigner_nothrow()
    {
        OSL_PRECOND( isActive(), "SQLCommandDesigner::impl_closeDesigner_nothrow: invalid calle!" );
        // close it
        try
        {
            // do not listen anymore ....
            Reference< XPropertySet > xProps( m_xDesigner, UNO_QUERY );
            if ( xProps.is() )
                xProps->removePropertyChangeListener( PROPERTY_ACTIVECOMMAND, this );

            // we need to close the frame via the "user interface", by dispatching a close command,
            // instead of calling XCloseable::close directly. The latter method would also close
            // the frame, but not care for things like shutting down the office when the last
            // frame is gone ...
            const UnoURL aCloseURL( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:CloseDoc" ) ),
                Reference< XMultiServiceFactory >( m_xORB, UNO_QUERY ) );

            Reference< XDispatchProvider > xProvider( m_xDesigner->getFrame(), UNO_QUERY_THROW );
            Reference< XDispatch > xDispatch( xProvider->queryDispatch( aCloseURL, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "_top" ) ), FrameSearchFlag::SELF ) );
            OSL_ENSURE( xDispatch.is(), "SQLCommandDesigner::impl_closeDesigner_nothrow: no dispatcher for the CloseDoc command!" );
            if ( xDispatch.is() )
            {
                xDispatch->dispatch( aCloseURL, Sequence< PropertyValue >( ) );
            }
            else
            {
                // fallback: use the XCloseable::close (with all possible disadvantages)
                Reference< XCloseable > xClose( m_xDesigner->getFrame(), UNO_QUERY );
                if ( xClose.is() )
                    xClose->close( sal_True );
            }
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }

        m_xDesigner.clear();
    }

    //------------------------------------------------------------------------
    bool SQLCommandDesigner::impl_trySuspendDesigner_nothrow() const
    {
        OSL_PRECOND( isActive(), "SQLCommandDesigner::impl_trySuspendDesigner_nothrow: no active designer, this will crash!" );
        sal_Bool bAllow = sal_True;
        try
        {
            bAllow = m_xDesigner->suspend( sal_True );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return bAllow;
    }

//........................................................................
} // namespace pcr
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
