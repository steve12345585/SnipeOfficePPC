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

#include <unotools/sharedunocomponent.hxx>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <cppuhelper/implbase1.hxx>
#include <tools/debug.hxx>

//............................................................................
namespace utl
{
//............................................................................

    using ::com::sun::star::uno::XInterface;
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::UNO_QUERY;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::lang::XComponent;
    using ::com::sun::star::lang::EventObject;
    using ::com::sun::star::util::XCloseable;
    using ::com::sun::star::util::XCloseListener;
    using ::com::sun::star::util::CloseVetoException;

    //========================================================================
    //= DisposableComponent
    //========================================================================
    //------------------------------------------------------------------------
    DisposableComponent::DisposableComponent( const Reference< XInterface >& _rxComponent )
        :m_xComponent( _rxComponent, UNO_QUERY )
    {
        DBG_ASSERT( m_xComponent.is() || !_rxComponent.is(), "DisposableComponent::DisposableComponent: should be an XComponent!" );
    }

    //------------------------------------------------------------------------
    DisposableComponent::~DisposableComponent()
    {
        if ( m_xComponent.is() )
        {
            try
            {
                m_xComponent->dispose();
            }
            catch( const Exception& )
            {
                OSL_FAIL( "DisposableComponent::~DisposableComponent: caught an exception!" );
            }
            m_xComponent.clear();
        }
    }

    //========================================================================
    //= CloseableComponentImpl
    //========================================================================
    DBG_NAME( CloseableComponentImpl )
    typedef ::cppu::WeakImplHelper1 <   XCloseListener
                                    >   CloseableComponentImpl_Base;
    class CloseableComponentImpl : public CloseableComponentImpl_Base
    {
    private:
        Reference< XCloseable > m_xCloseable;

    public:
        CloseableComponentImpl( const Reference< XInterface >& _rxComponent );

        /** closes the component

            @nofail
        */
        void    nf_closeComponent();

    protected:
        virtual ~CloseableComponentImpl();

        // XCloseListener overridables
        virtual void SAL_CALL queryClosing( const EventObject& Source, ::sal_Bool GetsOwnership ) throw (CloseVetoException, RuntimeException);
        virtual void SAL_CALL notifyClosing( const EventObject& Source ) throw (RuntimeException);

        // XEventListener overridables
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);

    private:
        /** starts or stops being a CloseListener at the component

            Only to be called upon construction of the instance, or when the component
            is to be closed.

        @nofail
        */
        void    impl_nf_switchListening( bool _bListen );


    private:
        CloseableComponentImpl();                                           // never implemented
        CloseableComponentImpl( const CloseableComponentImpl& );            // never implemented
        CloseableComponentImpl& operator=( const CloseableComponentImpl& ); // never implemented
    };

    //------------------------------------------------------------------------
    CloseableComponentImpl::CloseableComponentImpl( const Reference< XInterface >& _rxComponent )
        :m_xCloseable( _rxComponent, UNO_QUERY )
    {
        DBG_CTOR( CloseableComponentImpl, NULL );
        DBG_ASSERT( m_xCloseable.is() || !_rxComponent.is(), "CloseableComponentImpl::CloseableComponentImpl: component is not an XCloseable!" );
        impl_nf_switchListening( true );
    }
    //------------------------------------------------------------------------
    CloseableComponentImpl::~CloseableComponentImpl()
    {
        nf_closeComponent();
        DBG_DTOR( CloseableComponentImpl, NULL );
    }

    //------------------------------------------------------------------------
    void CloseableComponentImpl::nf_closeComponent()
    {
        if ( !m_xCloseable.is() )
            // nothing to do
            return;

        // stop listening
        impl_nf_switchListening( false );

        // close
        try
        {
            m_xCloseable->close( sal_True );
        }
        catch( const CloseVetoException& ) { /* fine */ }
        catch( const Exception& )
        {
            OSL_FAIL( "CloseableComponentImpl::nf_closeComponent: caught an unexpected exception!" );
        }

        // reset
        m_xCloseable.clear();
    }

    //------------------------------------------------------------------------
    void CloseableComponentImpl::impl_nf_switchListening( bool _bListen )
    {
        if ( !m_xCloseable.is() )
            return;

        try
        {
            if ( _bListen )
                m_xCloseable->addCloseListener( this );
            else
                m_xCloseable->removeCloseListener( this );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "CloseableComponentImpl::impl_nf_switchListening: caught an exception!" );
        }
    }

    //--------------------------------------------------------------------
    void SAL_CALL CloseableComponentImpl::queryClosing( const EventObject&
    #ifdef DBG_UTIL
    Source
    #endif
    , ::sal_Bool /*GetsOwnership*/ ) throw (CloseVetoException, RuntimeException)
    {
        // as long as we live, somebody wants to keep the object alive. So, veto the
        // closing
        DBG_ASSERT( Source.Source == m_xCloseable, "CloseableComponentImpl::queryClosing: where did this come from?" );
        throw CloseVetoException();
    }

    //--------------------------------------------------------------------
    void SAL_CALL CloseableComponentImpl::notifyClosing( const EventObject&
    #ifdef DBG_UTIL
    Source
    #endif
    ) throw (RuntimeException)
    {
        DBG_ASSERT( Source.Source == m_xCloseable, "CloseableComponentImpl::notifyClosing: where did this come from?" );

        // this should be unreachable: As long as we're a CloseListener, we veto the closing. If we're going
        // to close the component ourself, then we revoke ourself as listener *before* the close call. So,
        // if this here fires, something went definately wrong.
        OSL_FAIL( "CloseableComponentImpl::notifyClosing: unreachable!" );
    }

    //--------------------------------------------------------------------
    void SAL_CALL CloseableComponentImpl::disposing( const EventObject&
    #ifdef DBG_UTIL
    Source
    #endif
    ) throw (RuntimeException)
    {
        DBG_ASSERT( Source.Source == m_xCloseable, "CloseableComponentImpl::disposing: where did this come from?" );
        OSL_FAIL( "CloseableComponentImpl::disposing: unreachable!" );
            // same reasoning for this assertion as in ->notifyClosing
    }

    //========================================================================
    //= CloseableComponentImpl
    //========================================================================
    DBG_NAME( CloseableComponent )
    //------------------------------------------------------------------------
    CloseableComponent::CloseableComponent( const Reference< XInterface >& _rxComponent )
        :m_pImpl( new CloseableComponentImpl( _rxComponent ) )
    {
        DBG_CTOR( CloseableComponent, NULL );
    }

    //------------------------------------------------------------------------
    CloseableComponent::~CloseableComponent()
    {
        // close the component, deliver ownership to anybody who wants to veto the close
        m_pImpl->nf_closeComponent();
        DBG_DTOR( CloseableComponent, NULL );
    }

//............................................................................
}   // namespace utl
//............................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
