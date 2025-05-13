/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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


#include "uielement/buttontoolbarcontroller.hxx"

#include "uielement/toolbar.hxx"

#include <com/sun/star/util/URLTransformer.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include "com/sun/star/util/XMacroExpander.hpp"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/beans/XPropertySet.hpp"

#include <rtl/uri.hxx>
#include <osl/mutex.hxx>
#include <comphelper/processfactory.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <vcl/svapp.hxx>
#include <vcl/mnemonic.hxx>
#include <vcl/window.hxx>
#include <vcl/graph.hxx>
#include <vcl/bitmap.hxx>
#include <vcl/graphicfilter.hxx>
#include <svtools/miscopt.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::util;

namespace framework
{

ButtonToolbarController::ButtonToolbarController(
    const uno::Reference< lang::XMultiServiceFactory >& rServiceManager,
    ToolBox*                                            pToolBar,
    const rtl::OUString&                                aCommand ) :
    cppu::OWeakObject(),
    m_bInitialized( sal_False ),
    m_bDisposed( sal_False ),
    m_aCommandURL( aCommand ),
    m_xServiceManager( rServiceManager ),
    m_pToolbar( pToolBar )
{
}

ButtonToolbarController::~ButtonToolbarController()
{
}

        // XInterface
uno::Any SAL_CALL ButtonToolbarController::queryInterface( const uno::Type& rType )
throw (::com::sun::star::uno::RuntimeException)
{
    Any a = ::cppu::queryInterface(
                rType ,
                static_cast< frame::XStatusListener* >( this ),
                static_cast< frame::XToolbarController* >( this ),
                static_cast< lang::XInitialization* >( this ),
                static_cast< lang::XComponent* >( this ),
                static_cast< util::XUpdatable* >( this ));

    if ( a.hasValue() )
        return a;

    return cppu::OWeakObject::queryInterface( rType );
}

void SAL_CALL ButtonToolbarController::acquire() throw ()
{
    cppu::OWeakObject::acquire();
}

void SAL_CALL ButtonToolbarController::release() throw ()
{
    cppu::OWeakObject::release();
}

// XInitialization
void SAL_CALL ButtonToolbarController::initialize(
    const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments )
throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
{
    bool bInitialized( true );

    {
        SolarMutexGuard aSolarMutexGuard;

        if ( m_bDisposed )
            throw DisposedException();

        bInitialized = m_bInitialized;
    }

    if ( !bInitialized )
    {
        SolarMutexGuard aSolarMutexGuard;
        m_bInitialized = sal_True;

        PropertyValue aPropValue;
        for ( int i = 0; i < aArguments.getLength(); i++ )
        {
            if ( aArguments[i] >>= aPropValue )
            {
                if ( aPropValue.Name == "Frame" )
                    m_xFrame.set(aPropValue.Value,UNO_QUERY);
                else if ( aPropValue.Name == "CommandURL" )
                    aPropValue.Value >>= m_aCommandURL;
                else if ( aPropValue.Name == "ServiceManager" )
                    m_xServiceManager.set(aPropValue.Value,UNO_QUERY);
            }
        }
    }
}

// XComponent
void SAL_CALL ButtonToolbarController::dispose() throw (::com::sun::star::uno::RuntimeException)
{
    Reference< XComponent > xThis( static_cast< OWeakObject* >(this), UNO_QUERY );

    {
        SolarMutexGuard aSolarMutexGuard;
        if ( m_bDisposed )
            throw DisposedException();

        m_xServiceManager.clear();
        m_xURLTransformer.clear();
        m_xFrame.clear();
        m_pToolbar = 0;
        m_bDisposed = sal_True;
    }
}

void SAL_CALL ButtonToolbarController::addEventListener(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& )
throw (::com::sun::star::uno::RuntimeException)
{
    // do nothing
}

void SAL_CALL ButtonToolbarController::removeEventListener(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& )
throw (::com::sun::star::uno::RuntimeException)
{
    // do nothing
}

// XUpdatable
void SAL_CALL ButtonToolbarController::update()
throw (::com::sun::star::uno::RuntimeException)
{
    SolarMutexGuard aSolarMutexGuard;
    if ( m_bDisposed )
        throw DisposedException();
}

// XEventListener
void SAL_CALL ButtonToolbarController::disposing(
    const com::sun::star::lang::EventObject& Source )
throw ( ::com::sun::star::uno::RuntimeException )
{
    uno::Reference< uno::XInterface > xSource( Source.Source );

    SolarMutexGuard aSolarMutexGuard;

    if ( m_bDisposed )
        return;

    uno::Reference< uno::XInterface > xIfac( m_xFrame, uno::UNO_QUERY );
    if ( xIfac == xSource )
        m_xFrame.clear();
}

void SAL_CALL ButtonToolbarController::statusChanged( const ::com::sun::star::frame::FeatureStateEvent& )
throw ( ::com::sun::star::uno::RuntimeException )
{
    // do nothing
    if ( m_bDisposed )
        throw DisposedException();
}

// XToolbarController
void SAL_CALL ButtonToolbarController::execute( sal_Int16 KeyModifier )
throw (::com::sun::star::uno::RuntimeException)
{
    uno::Reference< frame::XDispatch >      xDispatch;
    uno::Reference< frame::XFrame >         xFrame;
    uno::Reference< util::XURLTransformer > xURLTransformer;
    rtl::OUString                           aCommandURL;
    ::com::sun::star::util::URL             aTargetURL;

    {
        SolarMutexGuard aSolarMutexGuard;

        if ( m_bDisposed )
            throw DisposedException();

        if ( m_bInitialized &&
             m_xFrame.is() &&
             m_xServiceManager.is() &&
             !m_aCommandURL.isEmpty() )
        {
            if ( !m_xURLTransformer.is() )
            {
                m_xURLTransformer = util::URLTransformer::create( ::comphelper::getComponentContext(m_xServiceManager) );
            }

            xFrame          = m_xFrame;
            aCommandURL     = m_aCommandURL;
            xURLTransformer = m_xURLTransformer;
        }
    }

    uno::Reference< frame::XDispatchProvider > xDispatchProvider( xFrame, uno::UNO_QUERY );
    if ( xDispatchProvider.is() )
    {
        aTargetURL.Complete = aCommandURL;
        xURLTransformer->parseStrict( aTargetURL );
        xDispatch = xDispatchProvider->queryDispatch( aTargetURL, ::rtl::OUString(), 0 );
    }

    if ( xDispatch.is() )
    {
        try
        {
            Sequence<PropertyValue>   aArgs( 1 );

            // Provide key modifier information to dispatch function
            aArgs[0].Name   = rtl::OUString( "KeyModifier" );
            aArgs[0].Value  <<= KeyModifier;

            xDispatch->dispatch( aTargetURL, aArgs );
        }
        catch ( const DisposedException& )
        {
        }
    }
}

void SAL_CALL ButtonToolbarController::click()
throw (::com::sun::star::uno::RuntimeException)
{
    SolarMutexGuard aSolarMutexGuard;

    if ( m_bDisposed )
        throw DisposedException();

    sal_Int16   nKeyModifier( (sal_Int16)m_pToolbar->GetModifier() );
    execute( nKeyModifier );
}

void SAL_CALL ButtonToolbarController::doubleClick()
throw (::com::sun::star::uno::RuntimeException)
{
    // do nothing
    if ( m_bDisposed )
        throw DisposedException();
}

uno::Reference< awt::XWindow > SAL_CALL ButtonToolbarController::createPopupWindow()
throw (::com::sun::star::uno::RuntimeException)
{
    if ( m_bDisposed )
        throw DisposedException();

    return uno::Reference< awt::XWindow >();
}

uno::Reference< awt::XWindow > SAL_CALL ButtonToolbarController::createItemWindow(
    const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& )
throw (::com::sun::star::uno::RuntimeException)
{
    if ( m_bDisposed )
        throw DisposedException();

    return uno::Reference< awt::XWindow >();
}

} // namespace

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
