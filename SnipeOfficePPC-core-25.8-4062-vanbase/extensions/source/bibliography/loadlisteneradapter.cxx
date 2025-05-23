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

#include "loadlisteneradapter.hxx"
#include <osl/diagnose.h>
#include <rtl/ref.hxx>

//.........................................................................
namespace bib
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::form;

    //=====================================================================
    //= OComponentListener
    //=====================================================================
    //---------------------------------------------------------------------
    OComponentListener::~OComponentListener()
    {
        {
            ::osl::MutexGuard aGuard( m_rMutex );
            if ( m_pAdapter )
                m_pAdapter->dispose();
        }
    }

    //---------------------------------------------------------------------
    void OComponentListener::_disposing( const EventObject& /*_rSource*/ ) throw( RuntimeException)
    {
        // nothing to do here, overrride if you're interested in
    }

    //---------------------------------------------------------------------
    void OComponentListener::setAdapter( OComponentAdapterBase* pAdapter )
    {
        {
            ::osl::MutexGuard aGuard( m_rMutex );
            if ( m_pAdapter )
            {
                m_pAdapter->release();
                m_pAdapter = NULL;
            }
        }

        if ( pAdapter )
        {
            ::osl::MutexGuard aGuard( m_rMutex );
            m_pAdapter = pAdapter;
            m_pAdapter->acquire();
        }
    }

    //=====================================================================
    //= OComponentAdapterBase
    //=====================================================================
    //---------------------------------------------------------------------
    OComponentAdapterBase::OComponentAdapterBase( const Reference< XComponent >& _rxComp, sal_Bool _bAutoRelease )
        :m_xComponent( _rxComp )
        ,m_pListener( NULL )
        ,m_nLockCount( 0 )
        ,m_bListening( sal_False )
        ,m_bAutoRelease( _bAutoRelease )
    {
        OSL_ENSURE( m_xComponent.is(), "OComponentAdapterBase::OComponentAdapterBase: invalid component!" );
    }

    //---------------------------------------------------------------------
    void OComponentAdapterBase::Init( OComponentListener* _pListener )
    {
        OSL_ENSURE( !m_pListener, "OComponentAdapterBase::Init: already initialized!" );
        OSL_ENSURE( _pListener, "OComponentAdapterBase::Init: invalid listener!" );

        m_pListener = _pListener;
        if ( m_pListener )
            m_pListener->setAdapter( this );

        startComponentListening( );
        m_bListening = sal_True;
    }

    //---------------------------------------------------------------------
    OComponentAdapterBase::~OComponentAdapterBase()
    {
    }

    //---------------------------------------------------------------------
    void OComponentAdapterBase::dispose()
    {
        if ( m_bListening )
        {
            ::rtl::Reference< OComponentAdapterBase > xPreventDelete(this);

            disposing();

            m_pListener->setAdapter(NULL);

            m_pListener = NULL;
            m_bListening = sal_False;

            if (m_bAutoRelease)
                m_xComponent = NULL;
        }
    }

    // XEventListener

    //---------------------------------------------------------------------
    void SAL_CALL OComponentAdapterBase::disposing( const EventObject& _rSource ) throw( RuntimeException )
    {
        if ( m_pListener )
        {
             // tell the listener
            if ( !locked() )
                m_pListener->_disposing( _rSource );

            // disconnect the listener
            if ( m_pListener )  // may have been reset whilest calling into _disposing
                m_pListener->setAdapter( NULL );
        }

        m_pListener = NULL;
        m_bListening = sal_False;

        if ( m_bAutoRelease )
            m_xComponent = NULL;
    }

    //=====================================================================
    //= OLoadListenerAdapter
    //=====================================================================
    //---------------------------------------------------------------------
    OLoadListenerAdapter::OLoadListenerAdapter( const Reference< XLoadable >& _rxLoadable, sal_Bool _bAutoRelease )
        :OComponentAdapterBase( Reference< XComponent >( _rxLoadable, UNO_QUERY ), _bAutoRelease )
    {
    }

    //---------------------------------------------------------------------
    void OLoadListenerAdapter::startComponentListening()
    {
        Reference< XLoadable > xLoadable( getComponent(), UNO_QUERY );
        OSL_ENSURE( xLoadable.is(), "OLoadListenerAdapter::OLoadListenerAdapter: invalid object!" );
        if ( xLoadable.is() )
            xLoadable->addLoadListener( this );
    }

    //---------------------------------------------------------------------
    void SAL_CALL OLoadListenerAdapter::acquire(  ) throw ()
    {
        OLoadListenerAdapter_Base::acquire();
    }

    //---------------------------------------------------------------------
    void SAL_CALL OLoadListenerAdapter::release(  ) throw ()
    {
        OLoadListenerAdapter_Base::release();
    }

    //---------------------------------------------------------------------
    void SAL_CALL OLoadListenerAdapter::disposing( const  EventObject& _rSource ) throw( RuntimeException)
    {
        OComponentAdapterBase::disposing( _rSource );
    }

    //---------------------------------------------------------------------
    void OLoadListenerAdapter::disposing()
    {
        Reference< XLoadable > xLoadable( getComponent(), UNO_QUERY );
        if ( xLoadable.is() )
            xLoadable->removeLoadListener( this );
    }

    //---------------------------------------------------------------------
    void SAL_CALL OLoadListenerAdapter::loaded( const EventObject& _rEvent ) throw (RuntimeException)
    {
        if ( !locked() && getLoadListener( ) )
            getLoadListener( )->_loaded( _rEvent );
    }

    //---------------------------------------------------------------------
    void SAL_CALL OLoadListenerAdapter::unloading( const EventObject& _rEvent ) throw (RuntimeException)
    {
        if ( !locked() && getLoadListener( ) )
            getLoadListener( )->_unloading( _rEvent );
    }

    //---------------------------------------------------------------------
    void SAL_CALL OLoadListenerAdapter::unloaded( const EventObject& _rEvent ) throw (RuntimeException)
    {
        if ( !locked() && getLoadListener( ) )
            getLoadListener( )->_unloaded( _rEvent );
    }

    //---------------------------------------------------------------------
    void SAL_CALL OLoadListenerAdapter::reloading( const EventObject& _rEvent ) throw (RuntimeException)
    {
        if ( !locked() && getLoadListener( ) )
            getLoadListener( )->_reloading( _rEvent );
    }

    //---------------------------------------------------------------------
    void SAL_CALL OLoadListenerAdapter::reloaded( const EventObject& _rEvent ) throw (RuntimeException)
    {
        if ( !locked() && getLoadListener( ) )
            getLoadListener( )->_reloaded( _rEvent );
    }

//.........................................................................
}   // namespace bib
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
