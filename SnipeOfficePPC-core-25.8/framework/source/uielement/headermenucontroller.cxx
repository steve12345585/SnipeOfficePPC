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

#include <uielement/headermenucontroller.hxx>

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________
#include <threadhelp/resetableguard.hxx>
#include "services.h"

#include <classes/resource.hrc>
#include <classes/fwlresid.hxx>

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/awt/XDevice.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/awt/MenuItemStyle.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

//_________________________________________________________________________________________________________________
//  includes of other projects
//_________________________________________________________________________________________________________________

#include <vcl/menu.hxx>
#include <vcl/svapp.hxx>
#include <vcl/i18nhelp.hxx>
#include <rtl/ustrbuf.hxx>
#include <osl/mutex.hxx>

//_________________________________________________________________________________________________________________
//  Defines
//_________________________________________________________________________________________________________________

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::frame;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;
using namespace com::sun::star::style;
using namespace com::sun::star::container;

const sal_uInt16 ALL_MENUITEM_ID = 1;

namespace framework
{

DEFINE_XSERVICEINFO_MULTISERVICE        (   HeaderMenuController                    ,
                                            OWeakObject                             ,
                                            SERVICENAME_POPUPMENUCONTROLLER         ,
                                            IMPLEMENTATIONNAME_HEADERMENUCONTROLLER
                                        )

DEFINE_INIT_SERVICE                     (   HeaderMenuController, {} )

HeaderMenuController::HeaderMenuController( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager,bool _bFooter ) :
    svt::PopupMenuControllerBase( xServiceManager )
    ,m_bFooter(_bFooter)
{
}

HeaderMenuController::~HeaderMenuController()
{
}

// private function
void HeaderMenuController::fillPopupMenu( const Reference< ::com::sun::star::frame::XModel >& rModel, Reference< css::awt::XPopupMenu >& rPopupMenu )
{
    VCLXPopupMenu*                                     pPopupMenu        = (VCLXPopupMenu *)VCLXMenu::GetImplementation( rPopupMenu );
    PopupMenu*                                         pVCLPopupMenu     = 0;

    SolarMutexGuard aSolarMutexGuard;

    resetPopupMenu( rPopupMenu );
    if ( pPopupMenu )
        pVCLPopupMenu = (PopupMenu *)pPopupMenu->GetMenu();

    Reference< XStyleFamiliesSupplier > xStyleFamiliesSupplier( rModel, UNO_QUERY );
    if ( pVCLPopupMenu && xStyleFamiliesSupplier.is())
    {
        Reference< XNameAccess > xStyleFamilies = xStyleFamiliesSupplier->getStyleFamilies();

        rtl::OUString aCmd( RTL_CONSTASCII_USTRINGPARAM( ".uno:InsertPageHeader" ));
        rtl::OUString aHeaderFooterIsOnStr(RTL_CONSTASCII_USTRINGPARAM( "HeaderIsOn" ));
        if ( m_bFooter )
        {
            aCmd = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:InsertPageFooter" ));
            aHeaderFooterIsOnStr = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "FooterIsOn" ));
        }
        const rtl::OUString aIsPhysicalStr( RTL_CONSTASCII_USTRINGPARAM( "IsPhysical" ));
        const rtl::OUString aDisplayNameStr( RTL_CONSTASCII_USTRINGPARAM( "DisplayName" ));

        try
        {
            Reference< XNameContainer > xNameContainer;
            if ( xStyleFamilies->getByName( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PageStyles" ))) >>= xNameContainer )
            {
                Sequence< rtl::OUString > aSeqNames = xNameContainer->getElementNames();

                sal_uInt16   nId = 2;
                sal_uInt16  nCount = 0;
                sal_Bool bAllOneState( sal_True );
                sal_Bool bLastCheck( sal_True );
                sal_Bool bFirstChecked( sal_False );
                sal_Bool bFirstItemInserted( sal_False );
                for ( sal_Int32 n = 0; n < aSeqNames.getLength(); n++ )
                {
                    rtl::OUString aName = aSeqNames[n];
                    Reference< XPropertySet > xPropSet( xNameContainer->getByName( aName ), UNO_QUERY );
                    if ( xPropSet.is() )
                    {
                        sal_Bool bIsPhysical( sal_False );
                        if (( xPropSet->getPropertyValue( aIsPhysicalStr ) >>= bIsPhysical ) && bIsPhysical )
                        {
                            rtl::OUString aDisplayName;
                            sal_Bool      bHeaderIsOn( sal_False );
                            xPropSet->getPropertyValue( aDisplayNameStr ) >>= aDisplayName;
                            xPropSet->getPropertyValue( aHeaderFooterIsOnStr ) >>= bHeaderIsOn;

                            rtl::OUStringBuffer aStrBuf( aCmd );
                            aStrBuf.appendAscii( "?PageStyle:string=");
                            aStrBuf.append( aDisplayName );
                            aStrBuf.appendAscii( "&On:bool=" );
                            if ( !bHeaderIsOn )
                                aStrBuf.appendAscii( "true" );
                            else
                                aStrBuf.appendAscii( "false" );
                            rtl::OUString aCommand( aStrBuf.makeStringAndClear() );
                            pVCLPopupMenu->InsertItem( nId, aDisplayName, MIB_CHECKABLE );
                            if ( !bFirstItemInserted )
                            {
                                bFirstItemInserted = sal_True;
                                bFirstChecked      = bHeaderIsOn;
                            }

                            pVCLPopupMenu->SetItemCommand( nId, aCommand );

                            if ( bHeaderIsOn )
                                pVCLPopupMenu->CheckItem( nId, sal_True );
                            ++nId;

                            // Check if all entries have the same state
                            if( bAllOneState && n && bHeaderIsOn != bLastCheck )
                                bAllOneState = sal_False;
                            bLastCheck = bHeaderIsOn;
                            ++nCount;
                        }
                    }
                }

                if ( bAllOneState && ( nCount > 1 ))
                {
                    // Insert special item for all command
                    pVCLPopupMenu->InsertItem( ALL_MENUITEM_ID, String( FwlResId( STR_MENU_HEADFOOTALL )), 0, 0 );

                    rtl::OUStringBuffer aStrBuf( aCmd );
                    aStrBuf.appendAscii( "?On:bool=" );

                    // Command depends on check state of first menu item entry
                    if ( !bFirstChecked )
                        aStrBuf.appendAscii( "true" );
                    else
                        aStrBuf.appendAscii( "false" );

                    pVCLPopupMenu->SetItemCommand( 1, aStrBuf.makeStringAndClear() );
                    pVCLPopupMenu->InsertSeparator( 1 );
                }
            }
        }
        catch ( const com::sun::star::container::NoSuchElementException& )
        {
        }
    }
}

// XEventListener
void SAL_CALL HeaderMenuController::disposing( const EventObject& ) throw ( RuntimeException )
{
    Reference< css::awt::XMenuListener > xHolder(( OWeakObject *)this, UNO_QUERY );

    osl::MutexGuard aLock( m_aMutex );
    m_xFrame.clear();
    m_xDispatch.clear();
    m_xServiceManager.clear();

    if ( m_xPopupMenu.is() )
        m_xPopupMenu->removeMenuListener( Reference< css::awt::XMenuListener >(( OWeakObject *)this, UNO_QUERY ));
    m_xPopupMenu.clear();
}

// XStatusListener
void SAL_CALL HeaderMenuController::statusChanged( const FeatureStateEvent& Event ) throw ( RuntimeException )
{
    Reference< com::sun::star::frame::XModel > xModel;

    if ( Event.State >>= xModel )
    {
        osl::MutexGuard aLock( m_aMutex );
        m_xModel = xModel;
        if ( m_xPopupMenu.is() )
            fillPopupMenu( xModel, m_xPopupMenu );
    }
}

// XMenuListener
void HeaderMenuController::impl_select(const Reference< XDispatch >& _xDispatch,const ::com::sun::star::util::URL& aTargetURL)
{
    Sequence<PropertyValue>      aArgs;
    OSL_ENSURE(_xDispatch.is(),"HeaderMenuController::impl_select: No dispatch");
    if ( _xDispatch.is() )
        _xDispatch->dispatch( aTargetURL, aArgs );
}

void SAL_CALL HeaderMenuController::updatePopupMenu() throw (::com::sun::star::uno::RuntimeException)
{
    osl::ResettableMutexGuard aLock( m_aMutex );

    throwIfDisposed();

    Reference< com::sun::star::frame::XModel > xModel( m_xModel );
    aLock.clear();

    if ( !xModel.is() )
        svt::PopupMenuControllerBase::updatePopupMenu();

    aLock.reset();
    if ( m_xPopupMenu.is() && m_xModel.is() )
        fillPopupMenu( m_xModel, m_xPopupMenu );
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
