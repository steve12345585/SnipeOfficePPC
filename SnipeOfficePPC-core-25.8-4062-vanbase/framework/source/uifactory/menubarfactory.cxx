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

#include <uifactory/menubarfactory.hxx>

#include <threadhelp/resetableguard.hxx>
#include "services.h"
#include <uielement/menubarwrapper.hxx>

#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/frame/ModuleManager.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/ui/XModuleUIConfigurationManagerSupplier.hpp>

#include <com/sun/star/ui/XUIConfigurationManagerSupplier.hpp>

#include <vcl/menu.hxx>
#include <vcl/svapp.hxx>
#include <rtl/ustrbuf.hxx>
#include <rtl/logfile.hxx>

//_________________________________________________________________________________________________________________
//  Defines
//_________________________________________________________________________________________________________________

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::frame;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;
using namespace ::com::sun::star::ui;

namespace framework
{

//*****************************************************************************************************************
//  XInterface, XTypeProvider, XServiceInfo
//*****************************************************************************************************************
DEFINE_XSERVICEINFO_ONEINSTANCESERVICE  (   MenuBarFactory                                  ,
                                            ::cppu::OWeakObject                             ,
                                            SERVICENAME_MENUBARFACTORY                      ,
                                            IMPLEMENTATIONNAME_MENUBARFACTORY
                                        )

DEFINE_INIT_SERVICE                     (   MenuBarFactory, {} )

MenuBarFactory::MenuBarFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager ) :
    ThreadHelpBase()
    , m_xServiceManager( xServiceManager )
    , m_xModuleManager( ModuleManager::create( comphelper::getComponentContext(xServiceManager) ) )
{
}
MenuBarFactory::MenuBarFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager,bool ) :
    ThreadHelpBase(&Application::GetSolarMutex())
    , m_xServiceManager( xServiceManager )
    , m_xModuleManager( ModuleManager::create( comphelper::getComponentContext(xServiceManager) ) )
{
}

MenuBarFactory::~MenuBarFactory()
{
}

// XUIElementFactory
Reference< XUIElement > SAL_CALL MenuBarFactory::createUIElement(
    const ::rtl::OUString& ResourceURL,
    const Sequence< PropertyValue >& Args )
throw ( ::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException )
{
    // SAFE
    ResetableGuard aLock( m_aLock );
    MenuBarWrapper* pMenuBarWrapper = new MenuBarWrapper( m_xServiceManager );
    Reference< ::com::sun::star::ui::XUIElement > xMenuBar( (OWeakObject *)pMenuBarWrapper, UNO_QUERY );
    Reference< ::com::sun::star::frame::XModuleManager2 > xModuleManager = m_xModuleManager;
    aLock.unlock();
    CreateUIElement(ResourceURL,Args,"MenuOnly","private:resource/menubar/",xMenuBar,xModuleManager,m_xServiceManager);
    return xMenuBar;
}
void MenuBarFactory::CreateUIElement(const ::rtl::OUString& ResourceURL
                                     , const Sequence< PropertyValue >& Args
                                     ,const char* _pExtraMode
                                     ,const char* _pAsciiName
                                     ,const Reference< ::com::sun::star::ui::XUIElement >& _xMenuBar
                                     ,const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModuleManager2 >& _xModuleManager
                                     ,const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _xServiceManager)
{
    Reference< XUIConfigurationManager > xCfgMgr;
    Reference< XUIConfigurationManager > xConfigSource;
    Reference< XFrame >                  xFrame;
    rtl::OUString                        aResourceURL( ResourceURL );
    sal_Bool                             bPersistent( sal_True );
    sal_Bool                             bExtraMode( sal_False );

    for ( sal_Int32 n = 0; n < Args.getLength(); n++ )
    {
        if ( Args[n].Name == "ConfigurationSource" )
            Args[n].Value >>= xConfigSource;
        else if ( Args[n].Name == "Frame" )
            Args[n].Value >>= xFrame;
        else if ( Args[n].Name == "ResourceURL" )
            Args[n].Value >>= aResourceURL;
        else if ( Args[n].Name == "Persistent" )
            Args[n].Value >>= bPersistent;
        else if ( _pExtraMode && Args[n].Name.equalsAscii( _pExtraMode ))
            Args[n].Value >>= bExtraMode;
    }
    if ( aResourceURL.indexOf( rtl::OUString::createFromAscii(_pAsciiName)) != 0 )
        throw IllegalArgumentException();

    // Identify frame and determine document based ui configuration manager/module ui configuration manager
    if ( xFrame.is() && !xConfigSource.is() )
    {
        bool bHasSettings( false );
        Reference< XModel > xModel;

        Reference< XController > xController = xFrame->getController();
        if ( xController.is() )
            xModel = xController->getModel();

        if ( xModel.is() )
        {
            Reference< XUIConfigurationManagerSupplier > xUIConfigurationManagerSupplier( xModel, UNO_QUERY );
            if ( xUIConfigurationManagerSupplier.is() )
            {
                xCfgMgr = xUIConfigurationManagerSupplier->getUIConfigurationManager();
                bHasSettings = xCfgMgr->hasSettings( aResourceURL );
            }
        }

        if ( !bHasSettings )
        {
            rtl::OUString aModuleIdentifier = _xModuleManager->identify( Reference< XInterface >( xFrame, UNO_QUERY ));
            if ( !aModuleIdentifier.isEmpty() )
            {
                Reference< ::com::sun::star::ui::XModuleUIConfigurationManagerSupplier > xModuleCfgSupplier(
                    _xServiceManager->createInstance( SERVICENAME_MODULEUICONFIGURATIONMANAGERSUPPLIER ), UNO_QUERY );
                xCfgMgr = xModuleCfgSupplier->getUIConfigurationManager( aModuleIdentifier );
                bHasSettings = xCfgMgr->hasSettings( aResourceURL );
            }
        }
    }

    PropertyValue aPropValue;
    Sequence< Any > aPropSeq( _pExtraMode ? 5 : 4);
    aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Frame" ));
    aPropValue.Value <<= xFrame;
    aPropSeq[0] <<= aPropValue;
    aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ConfigurationSource" ));
    aPropValue.Value <<= xCfgMgr;
    aPropSeq[1] <<= aPropValue;
    aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ResourceURL" ));
    aPropValue.Value <<= aResourceURL;
    aPropSeq[2] <<= aPropValue;
    aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Persistent" ));
    aPropValue.Value <<= bPersistent;
    aPropSeq[3] <<= aPropValue;
    if ( _pExtraMode )
    {
        aPropValue.Name = rtl::OUString::createFromAscii(_pExtraMode);
        aPropValue.Value <<= bExtraMode;
        aPropSeq[4] <<= aPropValue;
    }

    SolarMutexGuard aGuard;
    Reference< XInitialization > xInit( _xMenuBar, UNO_QUERY );
    xInit->initialize( aPropSeq );
}

} // namespace framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
