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

#include <svl/stritem.hxx>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/util/URLTransformer.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/frame/status/ItemStatus.hpp>
#include <com/sun/star/frame/status/ItemState.hpp>
#include <com/sun/star/awt/MouseButton.hpp>

#include <vcl/status.hxx>

#include <sfx2/app.hxx>
#include "sfx2/stbitem.hxx"
#include "sfxtypes.hxx"
#include <sfx2/msg.hxx>
#include "arrdecl.hxx"
#include <sfx2/bindings.hxx>
#include <sfx2/msgpool.hxx>
#include <sfx2/module.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/unoctitm.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/sfx.hrc>

#include <comphelper/processfactory.hxx>
#include <svl/eitem.hxx>
#include <svl/intitem.hxx>
#include <toolkit/unohlp.hxx>
#include <toolkit/helper/convert.hxx>

using namespace ::com::sun::star;

//--------------------------------------------------------------------

sal_uInt16 SfxStatusBarControl::convertAwtToVCLMouseButtons( sal_Int16 nAwtMouseButtons )
{
    sal_uInt16 nVCLMouseButtons( 0 );

    if ( nAwtMouseButtons & awt::MouseButton::LEFT )
        nVCLMouseButtons |= MOUSE_LEFT;
    if ( nAwtMouseButtons & awt::MouseButton::RIGHT )
        nVCLMouseButtons |= MOUSE_RIGHT;
    if ( nAwtMouseButtons & awt::MouseButton::MIDDLE )
        nVCLMouseButtons |= MOUSE_MIDDLE;

    return nVCLMouseButtons;
}

//--------------------------------------------------------------------

svt::StatusbarController* SAL_CALL SfxStatusBarControllerFactory(
    const uno::Reference< frame::XFrame >& rFrame,
    StatusBar* pStatusBar,
    unsigned short nID,
    const ::rtl::OUString& aCommandURL )
{
    SolarMutexGuard aGuard;

    util::URL aTargetURL;
    aTargetURL.Complete = aCommandURL;
    uno::Reference < util::XURLTransformer > xTrans( util::URLTransformer::create( ::comphelper::getProcessComponentContext() ) );
    xTrans->parseStrict( aTargetURL );

    SfxObjectShell* pObjShell = NULL;
    uno::Reference < frame::XController > xController;
    uno::Reference < frame::XModel > xModel;
    if ( rFrame.is() )
    {
        xController = rFrame->getController();
        if ( xController.is() )
            xModel = xController->getModel();
    }

    if ( xModel.is() )
    {
        // Get tunnel from model to retrieve the SfxObjectShell pointer from it
        ::com::sun::star::uno::Reference < ::com::sun::star::lang::XUnoTunnel > xObj( xModel, uno::UNO_QUERY );
        ::com::sun::star::uno::Sequence < sal_Int8 > aSeq = SvGlobalName( SFX_GLOBAL_CLASSID ).GetByteSequence();
        if ( xObj.is() )
        {
            sal_Int64 nHandle = xObj->getSomething( aSeq );
            if ( nHandle )
                        pObjShell = reinterpret_cast< SfxObjectShell* >( sal::static_int_cast< sal_IntPtr >( nHandle ));
        }
    }

    SfxModule*     pModule   = pObjShell ? pObjShell->GetModule() : NULL;
    SfxSlotPool*   pSlotPool = 0;

    if ( pModule )
        pSlotPool = pModule->GetSlotPool();
    else
        pSlotPool = &(SfxSlotPool::GetSlotPool( NULL ));

    const SfxSlot* pSlot = pSlotPool->GetUnoSlot( aTargetURL.Path );
    if ( pSlot )
    {
        sal_uInt16 nSlotId = pSlot->GetSlotId();
        if ( nSlotId > 0 )
        {
            rtl::OString aCmd(".uno:");
            aCmd += pSlot->GetUnoName();
            pStatusBar->SetHelpId( nSlotId, aCmd );
            return SfxStatusBarControl::CreateControl( nSlotId, nID, pStatusBar, pModule );
        }
    }

    return NULL;
}

//--------------------------------------------------------------------

SfxStatusBarControl::SfxStatusBarControl
(
    sal_uInt16      nSlotID,            /* Slot-Id which is connected to this
                                       instance. If a Slot-Id is set to != 0 at
                                       registration it will always be set there.
                                    */


    sal_uInt16      nCtrlID,            /* ID of this controller in the status bar */

    StatusBar&  rBar                /* Reference to the StatusBar,for which
                                       this Control was created. */
)


/*  [Description]

    Constructor of the SfxStatusBarControl Class. The Subclasses are
    created at the Factory if necessary.

    Instances of this base class are created for all StatusBar-fields
    for which no specific ones have been registered.
*/

:   svt::StatusbarController(),
    nSlotId( nSlotID ),
    nId( nCtrlID ),
    pBar( &rBar )
{
}

//--------------------------------------------------------------------

SfxStatusBarControl::~SfxStatusBarControl()

/*  [Description]

    Destructor of the SfxStatusBarControl Class. The Class and its Subclasses
    are destroyed by SFx.
*/

{}

//--------------------------------------------------------------------
// XInterface
uno::Any SAL_CALL SfxStatusBarControl::queryInterface( const uno::Type & rType )
throw( uno::RuntimeException)
{
    return svt::StatusbarController::queryInterface( rType );
}

void SAL_CALL SfxStatusBarControl::acquire() throw()
{
    OWeakObject::acquire();
}

void SAL_CALL SfxStatusBarControl::release() throw()
{
    OWeakObject::release();
}

//--------------------------------------------------------------------
// XEventListener
void SAL_CALL SfxStatusBarControl::disposing( const lang::EventObject& aEvent )
throw( uno::RuntimeException )
{
    svt::StatusbarController::disposing( aEvent );
}

//--------------------------------------------------------------------
// XComponent
void SAL_CALL SfxStatusBarControl::dispose()
throw (uno::RuntimeException)
{
    svt::StatusbarController::dispose();
}

//--------------------------------------------------------------------
// XStatusListener
void SAL_CALL SfxStatusBarControl::statusChanged( const frame::FeatureStateEvent& rEvent )
throw ( ::com::sun::star::uno::RuntimeException )
{
    SfxViewFrame* pViewFrame = NULL;
    uno::Reference < frame::XController > xController;

    SolarMutexGuard aGuard;
    if ( m_xFrame.is() )
        xController = m_xFrame->getController();

    uno::Reference < frame::XDispatchProvider > xProvider( xController, uno::UNO_QUERY );
    if ( xProvider.is() )
    {
        uno::Reference < frame::XDispatch > xDisp = xProvider->queryDispatch( rEvent.FeatureURL, ::rtl::OUString(), 0 );
        if ( xDisp.is() )
        {
            uno::Reference< lang::XUnoTunnel > xTunnel( xDisp, uno::UNO_QUERY );
            SfxOfficeDispatch* pDisp = NULL;
            if ( xTunnel.is() )
            {
                sal_Int64 nImplementation = xTunnel->getSomething(SfxOfficeDispatch::impl_getStaticIdentifier());
                pDisp = reinterpret_cast< SfxOfficeDispatch* >(sal::static_int_cast< sal_IntPtr >( nImplementation ));
            }

            if ( pDisp )
                pViewFrame = pDisp->GetDispatcher_Impl()->GetFrame();
        }
    }

    sal_uInt16 nSlotID = 0;
    SfxSlotPool& rPool = SfxSlotPool::GetSlotPool( pViewFrame );
    const SfxSlot* pSlot = rPool.GetUnoSlot( rEvent.FeatureURL.Path );
    if ( pSlot )
        nSlotID = pSlot->GetSlotId();

    if ( nSlotID > 0 )
    {
        if ( rEvent.Requery )
            svt::StatusbarController::statusChanged( rEvent );
        else
        {
            SfxItemState eState = SFX_ITEM_DISABLED;
            SfxPoolItem* pItem = NULL;
            if ( rEvent.IsEnabled )
            {
                eState = SFX_ITEM_AVAILABLE;
                uno::Type pType = rEvent.State.getValueType();

                if ( pType == ::getVoidCppuType() )
                {
                    pItem = new SfxVoidItem( nSlotID );
                    eState = SFX_ITEM_UNKNOWN;
                }
                else if ( pType == ::getBooleanCppuType() )
                {
                    sal_Bool bTemp = 0;
                    rEvent.State >>= bTemp ;
                    pItem = new SfxBoolItem( nSlotID, bTemp );
                }
                else if ( pType == ::getCppuType((const sal_uInt16*)0) )
                {
                    sal_uInt16 nTemp = 0;
                    rEvent.State >>= nTemp ;
                    pItem = new SfxUInt16Item( nSlotID, nTemp );
                }
                else if ( pType == ::getCppuType((const sal_uInt32*)0) )
                {
                    sal_uInt32 nTemp = 0;
                    rEvent.State >>= nTemp ;
                    pItem = new SfxUInt32Item( nSlotID, nTemp );
                }
                else if ( pType == ::getCppuType((const ::rtl::OUString*)0) )
                {
                    ::rtl::OUString sTemp ;
                    rEvent.State >>= sTemp ;
                    pItem = new SfxStringItem( nSlotID, sTemp );
                }
                else if ( pType == ::getCppuType((const ::com::sun::star::frame::status::ItemStatus*)0) )
                {
                    frame::status::ItemStatus aItemStatus;
                    rEvent.State >>= aItemStatus;
                    eState = aItemStatus.State;
                    pItem = new SfxVoidItem( nSlotID );
                }
                else
                {
                    if ( pSlot )
                        pItem = pSlot->GetType()->CreateItem();
                    if ( pItem )
                    {
                        pItem->SetWhich( nSlotID );
                        pItem->PutValue( rEvent.State );
                    }
                    else
                        pItem = new SfxVoidItem( nSlotID );
                }
            }

            StateChanged( nSlotID, eState, pItem );
            delete pItem;
        }
    }
}

//--------------------------------------------------------------------
// XStatusbarController

::sal_Bool SAL_CALL SfxStatusBarControl::mouseButtonDown(
    const awt::MouseEvent& rMouseEvent )
throw ( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    ::Point aPos( rMouseEvent.X, rMouseEvent.Y );

    ::MouseEvent aMouseEvent( aPos,
                              (sal_uInt16)rMouseEvent.ClickCount,
                              0,
                              convertAwtToVCLMouseButtons( rMouseEvent.Buttons ),
                              0 );

    return MouseButtonDown( aMouseEvent );
}

//--------------------------------------------------------------------

::sal_Bool SAL_CALL SfxStatusBarControl::mouseMove(
    const awt::MouseEvent& rMouseEvent )
throw (uno::RuntimeException)
{
    SolarMutexGuard aGuard;
    ::Point aPos( rMouseEvent.X, rMouseEvent.Y );

    ::MouseEvent aMouseEvent( aPos,
                              (sal_uInt16)rMouseEvent.ClickCount,
                              0,
                              convertAwtToVCLMouseButtons( rMouseEvent.Buttons ),
                              0 );
    return MouseMove( aMouseEvent );
}

//--------------------------------------------------------------------

::sal_Bool SAL_CALL SfxStatusBarControl::mouseButtonUp(
    const ::awt::MouseEvent& rMouseEvent )
throw ( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    ::Point aPos( rMouseEvent.X, rMouseEvent.Y );

    ::MouseEvent aMouseEvent( aPos,
                              (sal_uInt16)rMouseEvent.ClickCount,
                              0,
                              convertAwtToVCLMouseButtons( rMouseEvent.Buttons ),
                              0 );
    return MouseButtonUp( aMouseEvent );
}

//--------------------------------------------------------------------

void SAL_CALL SfxStatusBarControl::command(
    const awt::Point& rPos,
    ::sal_Int32 nCommand,
    ::sal_Bool /*bMouseEvent*/,
    const ::com::sun::star::uno::Any& /*aData*/ )
throw (::com::sun::star::uno::RuntimeException)
{
    SolarMutexGuard aGuard;
    ::Point aPos( rPos.X, rPos.Y );
    CommandEvent aCmdEvent( aPos, (sal_uInt16)nCommand, sal_True, NULL );

    Command( aCmdEvent );
}

//--------------------------------------------------------------------

void SAL_CALL SfxStatusBarControl::paint(
    const uno::Reference< awt::XGraphics >& xGraphics,
    const awt::Rectangle& rOutputRectangle,
    ::sal_Int32 nItemId,
    ::sal_Int32 nStyle )
throw ( ::uno::RuntimeException )
{
    SolarMutexGuard aGuard;

    OutputDevice* pOutDev = VCLUnoHelper::GetOutputDevice( xGraphics );
    if ( pOutDev )
    {
        ::Rectangle aRect = VCLRectangle( rOutputRectangle );
        UserDrawEvent aUserDrawEvent( pOutDev, aRect, (sal_uInt16)nItemId, (sal_uInt16)nStyle );
        Paint( aUserDrawEvent );
    }
}

//--------------------------------------------------------------------

void SAL_CALL SfxStatusBarControl::click()
throw ( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    Click();
}

//--------------------------------------------------------------------

void SAL_CALL SfxStatusBarControl::doubleClick()
throw ( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    DoubleClick();
}

//--------------------------------------------------------------------
// old sfx2 interface
//--------------------------------------------------------------------

void SfxStatusBarControl::StateChanged
(
    sal_uInt16              nSID,
    SfxItemState        eState,
    const SfxPoolItem*  pState  /* Pointer to SfxPoolItem, is only valid
                                   withih this Method call. This can be a
                                   Null-Pointer, a Pointer to SfxVoidItem
                                   or of this Type found registered by the
                                   Subclass of SfxStatusBarControl.
                                */
)

/*  [Description]

    The base implementation includes items of type SfxStringItem
    where the text is entered in the status row field and
    SfxVoidItem, where the field is emptied. The base implementation
    should not be called in overloaded methods.
*/

{
    DBG_ASSERT( pBar != 0, "setting state to dangling StatusBar" );

    const SfxStringItem* pStr = PTR_CAST( SfxStringItem, pState );
    if ( eState == SFX_ITEM_AVAILABLE && pStr )
        pBar->SetItemText( nSID, pStr->GetValue() );
    else
    {
        DBG_ASSERT( eState != SFX_ITEM_AVAILABLE || pState->ISA(SfxVoidItem),
                    "wrong SfxPoolItem subclass in SfxStatusBarControl" );
        pBar->SetItemText( nSID, String() );
    }
}

//--------------------------------------------------------------------

sal_Bool SfxStatusBarControl::MouseButtonDown( const MouseEvent & )

/*  [Description]

    This virtual method forwards the Event MouseButtonDown() of the
    StatusBar if the mouse position is within the range of the items,
    or if the mouse was captured by <SfxStatusBarControl::CaptureMouse()>

    The default implementation is empty and returns FALSE.

    [Return value]

    sal_Bool                TRUE
           The event has been processed and is not intended to
           be forwarded to StatusBar

                        FALSE
           The event was not processed and is to be
           be forwarded to StatusBar
*/

{
    return sal_False;
}

//--------------------------------------------------------------------

sal_Bool SfxStatusBarControl::MouseMove( const MouseEvent & )

/*  [Description]

    This virtual method forwards the Event MouseMove() of the
    StatusBar if the mouse position is within the range of the items,
    or if the mouse was captured by <SfxStatusBarControl::CaptureMouse()>

    The default implementation is empty and returns FALSE.

    [Return value]

    sal_Bool                TRUE
           The event has been processed and is not intended to
           be forwarded to StatusBar

                        FALSE
           The event was not processed and is to be
           be forwarded to StatusBar
*/

{
    return sal_False;
}

//--------------------------------------------------------------------

sal_Bool SfxStatusBarControl::MouseButtonUp( const MouseEvent & )

/*  [Description]

    This virtual method forwards the Event MouseButtonUp() of the
    StatusBar if the mouse position is within the range of the items,
    or if the mouse was captured by <SfxStatusBarControl::CaptureMouse()>

    The default implementation is empty and returns FALSE.

    [Return value]

    sal_Bool                TRUE
           The event has been processed and is not intended to
           be forwarded to StatusBar

                        FALSE
           The event was not processed and is to be
           be forwarded to StatusBar
*/

{
    return sal_False;
}

//--------------------------------------------------------------------

void SfxStatusBarControl::Command( const CommandEvent& )

/*  [Description]

    This virtual method is called when a CommandEvent is recived by
    SfxStatusBarControl.

    The default implementation is empty.
*/

{
}

//--------------------------------------------------------------------

void SfxStatusBarControl::Click()

/*  [Description]

    This virtual method is called when the user clicks on the
    field in the status row that belongs to this control.

    The default implementation is empty.
*/

{
}

//--------------------------------------------------------------------

void SfxStatusBarControl::DoubleClick()

/*  [Description]

    This virtual method is called when the user double-clicks on the
    field in the status row that belongs to this control.
*/

{
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aArgs;
    execute( aArgs );
}

//--------------------------------------------------------------------

void SfxStatusBarControl::Paint
(
    const UserDrawEvent& /* Reference to an UserDrawEvent */
)

/*  [Description]

    This virtual method is called to paint the contents if the field
    at hand is marked with SIB_USERDRAW. The output must be obtained
    within the Rectangle of rUDEvt.GetRect() by the OutputDevice
    given by rUDEvt.GetDevice().

    The default implementation is empty.
*/

{
}

//--------------------------------------------------------------------

void SfxStatusBarControl::CaptureMouse()
{
}

//--------------------------------------------------------------------

void SfxStatusBarControl::ReleaseMouse()
{
}

//--------------------------------------------------------------------

SfxStatusBarControl* SfxStatusBarControl::CreateControl
(
    sal_uInt16     nSlotID,
    sal_uInt16     nStbId,
    StatusBar* pBar,
    SfxModule* pMod
)
{
    SolarMutexGuard aGuard;
    SfxApplication *pApp = SFX_APP();

    SfxSlotPool *pSlotPool;
    if ( pMod )
        pSlotPool = pMod->GetSlotPool();
    else
        pSlotPool = &SfxSlotPool::GetSlotPool();

    TypeId aSlotType = pSlotPool->GetSlotType(nSlotID);
    if ( aSlotType )
    {
        if ( pMod )
        {
            SfxStbCtrlFactArr_Impl *pFactories = pMod->GetStbCtrlFactories_Impl();
            if ( pFactories )
            {
                SfxStbCtrlFactArr_Impl &rFactories = *pFactories;
                for ( sal_uInt16 nFactory = 0; nFactory < rFactories.size(); ++nFactory )
                if ( rFactories[nFactory]->nTypeId == aSlotType &&
                     ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                     ( rFactories[nFactory]->nSlotId == nSlotID) ) )
                    return rFactories[nFactory]->pCtor( nSlotID, nStbId, *pBar );
            }
        }

        SfxStbCtrlFactArr_Impl &rFactories = pApp->GetStbCtrlFactories_Impl();
        for ( sal_uInt16 nFactory = 0; nFactory < rFactories.size(); ++nFactory )
        if ( rFactories[nFactory]->nTypeId == aSlotType &&
             ( ( rFactories[nFactory]->nSlotId == 0 ) ||
             ( rFactories[nFactory]->nSlotId == nSlotID) ) )
            return rFactories[nFactory]->pCtor( nSlotID, nStbId, *pBar );
    }

    return NULL;
}

//--------------------------------------------------------------------
void SfxStatusBarControl::RegisterStatusBarControl(SfxModule* pMod, SfxStbCtrlFactory* pFact)
{
    SFX_APP()->RegisterStatusBarControl_Impl( pMod, pFact );
}
//--------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
