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


#include "doceventnotifier.hxx"
#include "scriptdocument.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/document/XEventBroadcaster.hpp>
/** === end UNO includes === **/

#include <vcl/svapp.hxx>

#include <tools/diagnose_ex.h>

#include <comphelper/componentcontext.hxx>
#include <comphelper/processfactory.hxx>

#include <osl/mutex.hxx>
#include <sal/macros.h>

#include <cppuhelper/compbase1.hxx>
#include <cppuhelper/basemutex.hxx>

//........................................................................
namespace basctl
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::document::XEventBroadcaster;
    using ::com::sun::star::document::XEventListener;
    using ::com::sun::star::document::EventObject;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::frame::XModel;
    using ::com::sun::star::uno::UNO_QUERY;
    /** === end UNO using === **/
    namespace csslang = ::com::sun::star::lang;

    //====================================================================
    //= DocumentEventNotifier_Impl
    //====================================================================
    typedef ::cppu::WeakComponentImplHelper1    <   XEventListener
                                                >   DocumentEventNotifier_Impl_Base;

    enum ListenerAction
    {
        RegisterListener,
        RemoveListener
    };

    /** impl class for DocumentEventNotifier
    */
    class DocumentEventNotifier_Impl    :public ::boost::noncopyable
                                        ,public ::cppu::BaseMutex
                                        ,public DocumentEventNotifier_Impl_Base
    {
    public:
        DocumentEventNotifier_Impl( DocumentEventListener& _rListener, const Reference< XModel >& _rxDocument );

        // document::XEventListener
        virtual void SAL_CALL notifyEvent( const EventObject& Event ) throw (RuntimeException);

        // lang::XEventListener
        virtual void SAL_CALL disposing( const csslang::EventObject& Event ) throw (RuntimeException);

        // ComponentHelper
        virtual void SAL_CALL disposing();

    protected:
        ~DocumentEventNotifier_Impl();

    private:
        /// determines whether the instance is already disposed
        bool    impl_isDisposed_nothrow() const { return m_pListener == NULL; }

        /// disposes the instance
        void    impl_dispose_nothrow();

        /// registers or revokes the instance as listener at the global event broadcaster
        void    impl_listenerAction_nothrow( ListenerAction _eAction );

    private:
        DocumentEventListener*  m_pListener;
        Reference< XModel >     m_xModel;
    };

    //--------------------------------------------------------------------
    DocumentEventNotifier_Impl::DocumentEventNotifier_Impl( DocumentEventListener& _rListener, const Reference< XModel >& _rxDocument )
        :DocumentEventNotifier_Impl_Base( m_aMutex )
        ,m_pListener( &_rListener )
        ,m_xModel( _rxDocument )
    {
        osl_incrementInterlockedCount( &m_refCount );
        impl_listenerAction_nothrow( RegisterListener );
        osl_decrementInterlockedCount( &m_refCount );
    }

    //--------------------------------------------------------------------
    DocumentEventNotifier_Impl::~DocumentEventNotifier_Impl()
    {
        if ( !impl_isDisposed_nothrow() )
        {
            acquire();
            dispose();
        }
    }

    //--------------------------------------------------------------------
    void SAL_CALL DocumentEventNotifier_Impl::notifyEvent( const EventObject& _rEvent ) throw (RuntimeException)
    {
        ::osl::ClearableMutexGuard aGuard( m_aMutex );

        OSL_PRECOND( !impl_isDisposed_nothrow(), "DocumentEventNotifier_Impl::notifyEvent: disposed, but still getting events?" );
        if ( impl_isDisposed_nothrow() )
            return;

        Reference< XModel > xDocument( _rEvent.Source, UNO_QUERY );
        OSL_ENSURE( xDocument.is(), "DocumentEventNotifier_Impl::notifyEvent: illegal source document!" );
        if ( !xDocument.is() )
            return;

        struct EventEntry
        {
            const sal_Char* pEventName;
            void (DocumentEventListener::*listenerMethod)( const ScriptDocument& _rDocument );
        };
        EventEntry aEvents[] = {
            { "OnNew",          &DocumentEventListener::onDocumentCreated },
            { "OnLoad",         &DocumentEventListener::onDocumentOpened },
            { "OnSave",         &DocumentEventListener::onDocumentSave },
            { "OnSaveDone",     &DocumentEventListener::onDocumentSaveDone },
            { "OnSaveAs",       &DocumentEventListener::onDocumentSaveAs },
            { "OnSaveAsDone",   &DocumentEventListener::onDocumentSaveAsDone },
            { "OnUnload",       &DocumentEventListener::onDocumentClosed },
            { "OnTitleChanged", &DocumentEventListener::onDocumentTitleChanged },
            { "OnModeChanged",  &DocumentEventListener::onDocumentModeChanged }
        };

        for ( size_t i=0; i < SAL_N_ELEMENTS( aEvents ); ++i )
        {
            if ( !_rEvent.EventName.equalsAscii( aEvents[i].pEventName ) )
                continue;

            ScriptDocument aDocument( xDocument );
            {
                // the listener implementations usually require the SolarMutex, so lock it here.
                // But ensure the proper order of locking the solar and the own mutex
                aGuard.clear();
                SolarMutexGuard aSolarGuard;
                ::osl::MutexGuard aGuard2( m_aMutex );

                if ( impl_isDisposed_nothrow() )
                    // somebody took the chance to dispose us -> bail out
                    return;

                (m_pListener->*aEvents[i].listenerMethod)( aDocument );
            }
            break;
        }
    }

    //--------------------------------------------------------------------
    void SAL_CALL DocumentEventNotifier_Impl::disposing( const csslang::EventObject& /*Event*/ ) throw (RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( m_aMutex );

        if ( !impl_isDisposed_nothrow() )
            impl_dispose_nothrow();
    }

    //--------------------------------------------------------------------
    void SAL_CALL DocumentEventNotifier_Impl::disposing()
    {
        impl_listenerAction_nothrow( RemoveListener );
        impl_dispose_nothrow();
    }

    //--------------------------------------------------------------------
    void DocumentEventNotifier_Impl::impl_dispose_nothrow()
    {
        m_pListener = NULL;
        m_xModel.clear();
    }

    //--------------------------------------------------------------------
    void DocumentEventNotifier_Impl::impl_listenerAction_nothrow( ListenerAction _eAction )
    {
        try
        {
            Reference< XEventBroadcaster > xBroadcaster;
            if ( m_xModel.is() )
                xBroadcaster.set( m_xModel, UNO_QUERY_THROW );
            else
            {
                ::comphelper::ComponentContext aContext( ::comphelper::getProcessServiceFactory() );
                xBroadcaster.set(
                    aContext.createComponent( "com.sun.star.frame.GlobalEventBroadcaster" ),
                    UNO_QUERY_THROW );
            }

            void ( SAL_CALL XEventBroadcaster::*listenerAction )( const Reference< XEventListener >& ) =
                ( _eAction == RegisterListener ) ? &XEventBroadcaster::addEventListener : &XEventBroadcaster::removeEventListener;
            (xBroadcaster.get()->*listenerAction)( this );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    //====================================================================
    //= DocumentEventNotifier
    //====================================================================
    //--------------------------------------------------------------------
    DocumentEventNotifier::DocumentEventNotifier( DocumentEventListener& _rListener, const Reference< XModel >& _rxDocument )
        :m_pImpl( new DocumentEventNotifier_Impl( _rListener, _rxDocument ) )
    {
    }

    //--------------------------------------------------------------------
    DocumentEventNotifier::DocumentEventNotifier( DocumentEventListener& _rListener )
        :m_pImpl( new DocumentEventNotifier_Impl( _rListener, Reference< XModel >() ) )
    {
    }

    //--------------------------------------------------------------------
    DocumentEventNotifier::~DocumentEventNotifier()
    {
    }

    //--------------------------------------------------------------------
    void DocumentEventNotifier::dispose()
    {
        m_pImpl->dispose();
    }

    //====================================================================
    //= DocumentEventListener
    //====================================================================
    DocumentEventListener::~DocumentEventListener()
    {
    }

//........................................................................
} // namespace basctl
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
