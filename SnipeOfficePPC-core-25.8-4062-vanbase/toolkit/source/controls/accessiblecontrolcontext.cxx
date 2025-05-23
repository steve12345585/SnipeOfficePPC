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

#include <toolkit/controls/accessiblecontrolcontext.hxx>
#include <unotools/accessiblestatesethelper.hxx>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <vcl/svapp.hxx>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <toolkit/helper/vclunohelper.hxx>
#include <vcl/window.hxx>

//........................................................................
namespace toolkit
{
//........................................................................

    using ::comphelper::OContextEntryGuard;
    using namespace ::com::sun::star;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::accessibility;

    //====================================================================
    //= OAccessibleControlContext
    //====================================================================
    //--------------------------------------------------------------------
    OAccessibleControlContext::OAccessibleControlContext()
        :OAccessibleControlContext_Base( )
    {
        // nothing to do here, we have a late ctor
    }

    //--------------------------------------------------------------------
    OAccessibleControlContext::~OAccessibleControlContext()
    {
        ensureDisposed();
    }

    //--------------------------------------------------------------------
    IMPLEMENT_FORWARD_XINTERFACE3( OAccessibleControlContext, OAccessibleControlContext_Base, OAccessibleImplementationAccess, OAccessibleControlContext_IBase )
    IMPLEMENT_FORWARD_XTYPEPROVIDER3( OAccessibleControlContext, OAccessibleControlContext_Base, OAccessibleImplementationAccess, OAccessibleControlContext_IBase )
        // (order matters: the first is the class name, the second is the class doing the ref counting)

    //--------------------------------------------------------------------
    void OAccessibleControlContext::Init( const Reference< XAccessible >& _rxCreator ) SAL_THROW( ( Exception ) )
    {
        OContextEntryGuard aGuard( this );

        // retrieve the model of the control
        OSL_ENSURE( !m_xControlModel.is(), "OAccessibleControlContext::Init: already know a control model....!???" );

        Reference< awt::XControl > xControl( _rxCreator, UNO_QUERY );
        if ( xControl.is() )
            m_xControlModel = m_xControlModel.query( xControl->getModel() );
        OSL_ENSURE( m_xControlModel.is(), "OAccessibleControlContext::Init: invalid creator (no control, or control without model!" );
        if ( !m_xControlModel.is() )
            throw DisposedException();  // caught by the caller (the create method)

        // start listening at the model
        startModelListening();

        // announce the XAccessible to our base class
        OAccessibleControlContext_Base::lateInit( _rxCreator );
    }

    //--------------------------------------------------------------------
    OAccessibleControlContext* OAccessibleControlContext::create( const Reference< XAccessible >& _rxCreator ) SAL_THROW( ( ) )
    {
        OAccessibleControlContext* pNew = NULL;
        try
        {
            pNew = new OAccessibleControlContext;
            pNew->Init( _rxCreator );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "OAccessibleControlContext::create: caught an exception from the late ctor!" );
        }
        return pNew;
    }

    //--------------------------------------------------------------------
    void OAccessibleControlContext::startModelListening( ) SAL_THROW( ( Exception ) )
    {
        Reference< XComponent > xModelComp( m_xControlModel, UNO_QUERY );
        OSL_ENSURE( xModelComp.is(), "OAccessibleControlContext::startModelListening: invalid model!" );
        if ( xModelComp.is() )
            xModelComp->addEventListener( this );
    }

    //--------------------------------------------------------------------
    void OAccessibleControlContext::stopModelListening( ) SAL_THROW( ( Exception ) )
    {
        Reference< XComponent > xModelComp( m_xControlModel, UNO_QUERY );
        OSL_ENSURE( xModelComp.is(), "OAccessibleControlContext::stopModelListening: invalid model!" );
        if ( xModelComp.is() )
            xModelComp->removeEventListener( this );
    }

    //--------------------------------------------------------------------
    sal_Int32 SAL_CALL OAccessibleControlContext::getAccessibleChildCount(  ) throw (RuntimeException)
    {
        // we do not have children
        return 0;
    }

    //--------------------------------------------------------------------
    Reference< XAccessible > SAL_CALL OAccessibleControlContext::getAccessibleChild( sal_Int32 ) throw (IndexOutOfBoundsException, RuntimeException)
    {
        // we do not have children
        throw IndexOutOfBoundsException();
    }

    //--------------------------------------------------------------------
    Reference< XAccessible > SAL_CALL OAccessibleControlContext::getAccessibleParent(  ) throw (RuntimeException)
    {
        OContextEntryGuard aGuard( this );
        OSL_ENSURE( implGetForeignControlledParent().is(), "OAccessibleControlContext::getAccessibleParent: somebody forgot to set a parent!" );
            // this parent of us is foreign controlled - somebody has to set it using the OAccessibleImplementationAccess
            // class, before integrating our instance into an AccessibleDocumentModel
        return implGetForeignControlledParent();
    }

    //--------------------------------------------------------------------
    sal_Int16 SAL_CALL OAccessibleControlContext::getAccessibleRole(  ) throw (RuntimeException)
    {
        return AccessibleRole::SHAPE;
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL OAccessibleControlContext::getAccessibleDescription(  ) throw (RuntimeException)
    {
        OContextEntryGuard aGuard( this );
        return getModelStringProperty( "HelpText" );
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL OAccessibleControlContext::getAccessibleName(  ) throw (RuntimeException)
    {
        OContextEntryGuard aGuard( this );
        return getModelStringProperty( "Name" );
    }

    //--------------------------------------------------------------------
    Reference< XAccessibleRelationSet > SAL_CALL OAccessibleControlContext::getAccessibleRelationSet(  ) throw (RuntimeException)
    {
        return NULL;
    }

    //--------------------------------------------------------------------
    Reference< XAccessibleStateSet > SAL_CALL OAccessibleControlContext::getAccessibleStateSet(  ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( GetMutex() );
            // no OContextEntryGuard here, as we do not want to throw an exception in case we're not alive anymore

        ::utl::AccessibleStateSetHelper* pStateSet = NULL;
        if ( isAlive() )
        {
            // no own states, only the ones which are foreign controlled
            pStateSet = new ::utl::AccessibleStateSetHelper( implGetForeignControlledStates() );
        }
        else
        {   // only the DEFUNC state if we're already disposed
            pStateSet = new ::utl::AccessibleStateSetHelper;
            pStateSet->AddState( AccessibleStateType::DEFUNC );
        }
        return pStateSet;
    }

    //--------------------------------------------------------------------
    void SAL_CALL OAccessibleControlContext::disposing( const EventObject&
    #if OSL_DEBUG_LEVEL > 0
    _rSource
    #endif
    ) throw ( RuntimeException )
    {
        OSL_ENSURE( Reference< XPropertySet >( _rSource.Source, UNO_QUERY ).get() == m_xControlModel.get(),
            "OAccessibleControlContext::disposing: where did this come from?" );

        stopModelListening( );
        m_xControlModel.clear();
        m_xModelPropsInfo.clear();

        OAccessibleControlContext_Base::disposing();
    }

    //--------------------------------------------------------------------
    ::rtl::OUString OAccessibleControlContext::getModelStringProperty( const sal_Char* _pPropertyName )
    {
        ::rtl::OUString sReturn;
        try
        {
            if ( !m_xModelPropsInfo.is() && m_xControlModel.is() )
                m_xModelPropsInfo = m_xControlModel->getPropertySetInfo();

            ::rtl::OUString sPropertyName( ::rtl::OUString::createFromAscii( _pPropertyName ) );
            if ( m_xModelPropsInfo.is() && m_xModelPropsInfo->hasPropertyByName( sPropertyName ) )
                m_xControlModel->getPropertyValue( sPropertyName ) >>= sReturn;
        }
        catch( const Exception& )
        {
            OSL_FAIL( "OAccessibleControlContext::getModelStringProperty: caught an exception!" );
        }
        return sReturn;
    }

    //--------------------------------------------------------------------
    Window* OAccessibleControlContext::implGetWindow( Reference< awt::XWindow >* _pxUNOWindow ) const
    {
        Reference< awt::XControl > xControl( getAccessibleCreator(), UNO_QUERY );
        Reference< awt::XWindow > xWindow;
        if ( xControl.is() )
            xWindow = xWindow.query( xControl->getPeer() );

        Window* pWindow = xWindow.is() ? VCLUnoHelper::GetWindow( xWindow ) : NULL;

        if ( _pxUNOWindow )
            *_pxUNOWindow = xWindow;
        return pWindow;
    }

    //--------------------------------------------------------------------
    awt::Rectangle SAL_CALL OAccessibleControlContext::implGetBounds(  ) throw (RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
            // want to do some VCL stuff here ...
        OContextEntryGuard aGuard( this );

        OSL_FAIL( "OAccessibleControlContext::implGetBounds: performance issue: forced to calc the size myself!" );
        // In design mode (and this is what this class is for), the surrounding shape (if any) should handle this call
        // The problem is that in design mode, our size may not be correct (in the drawing layer, controls are
        // positioned/sized for painting only), and that calculation of our position is expensive

        // what we know (or can obtain from somewhere):
        // * the PosSize of our peer, relative to it's parent window
        // * the parent window which the PosSize is relative to
        // * our foreign controlled accessible parent
        // from this info, we can determine the the position of our peer relative to the foreign parent

        // our control
        Reference< awt::XWindow > xWindow;
        Window* pVCLWindow = implGetWindow( &xWindow );

        awt::Rectangle aBounds( 0, 0, 0, 0 );
        if ( xWindow.is() )
        {
            // ugly, but .... though the XWindow has a getPosSize, it is impossible to determine the
            // parent which this position/size is relative to. This means we must tunnel UNO and ask the
            // implementation
            Window* pVCLParent = pVCLWindow ? pVCLWindow->GetParent() : NULL;

            // the relative location of the window
            ::Point aWindowRelativePos( 0, 0);
            if ( pVCLWindow )
                aWindowRelativePos = pVCLWindow->GetPosPixel();

            // the screnn position of the "window parent" of the control
            ::Point aVCLParentScreenPos( 0, 0 );
            if ( pVCLParent )
                aVCLParentScreenPos = pVCLParent->GetPosPixel();

            // the screen position of the "accessible parent" of the control
            Reference< XAccessible > xParentAcc( implGetForeignControlledParent() );
            Reference< XAccessibleComponent > xParentAccComponent;
            if ( xParentAcc.is() )
                xParentAccComponent = xParentAccComponent.query( xParentAcc->getAccessibleContext() );
            awt::Point aAccParentScreenPos( 0, 0 );
            if ( xParentAccComponent.is() )
                aAccParentScreenPos = xParentAccComponent->getLocationOnScreen();

            // now the size of the control
            aBounds = xWindow->getPosSize();

            // correct the pos
            aBounds.X = aWindowRelativePos.X() + aVCLParentScreenPos.X() - aAccParentScreenPos.X;
            aBounds.Y = aWindowRelativePos.Y() + aVCLParentScreenPos.Y() - aAccParentScreenPos.Y;
        }

        return aBounds;
    }

    //--------------------------------------------------------------------
    Reference< XAccessible > SAL_CALL OAccessibleControlContext::getAccessibleAtPoint( const awt::Point& /* _rPoint */ ) throw (RuntimeException)
    {
        // no children at all
        return NULL;
    }

    //--------------------------------------------------------------------
    void SAL_CALL OAccessibleControlContext::grabFocus(  ) throw (RuntimeException)
    {
        OSL_FAIL( "OAccessibleControlContext::grabFocus: !isFocusTraversable, but grabFocus!" );
    }

    //--------------------------------------------------------------------
    Any SAL_CALL OAccessibleControlContext::getAccessibleKeyBinding(  ) throw (RuntimeException)
    {
        // we do not have any key bindings to activate a UNO control in design mode
        return Any();
    }

    //--------------------------------------------------------------------
    sal_Int32 SAL_CALL OAccessibleControlContext::getForeground(  ) throw (::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
            // want to do some VCL stuff here ...
        OContextEntryGuard aGuard( this );

        Window* pWindow = implGetWindow( );
        sal_Int32 nColor = 0;
        if ( pWindow )
        {
            if ( pWindow->IsControlForeground() )
                nColor = pWindow->GetControlForeground().GetColor();
            else
            {
                Font aFont;
                if ( pWindow->IsControlFont() )
                    aFont = pWindow->GetControlFont();
                else
                    aFont = pWindow->GetFont();
                nColor = aFont.GetColor().GetColor();
            }
        }
        return nColor;
    }

    //--------------------------------------------------------------------
    sal_Int32 SAL_CALL OAccessibleControlContext::getBackground(  ) throw (::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
            // want to do some VCL stuff here ...
        OContextEntryGuard aGuard( this );

        Window* pWindow = implGetWindow( );
        sal_Int32 nColor = 0;
        if ( pWindow )
        {
            if ( pWindow->IsControlBackground() )
                nColor = pWindow->GetControlBackground().GetColor();
            else
                nColor = pWindow->GetBackground().GetColor().GetColor();
        }

        return nColor;
    }

//........................................................................
}   //namespace toolkit
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
