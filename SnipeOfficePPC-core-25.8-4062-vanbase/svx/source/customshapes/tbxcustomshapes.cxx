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


#include <string>

#include <svx/svxids.hrc>
#include <tools/shl.hxx>
#include <svl/eitem.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/viewsh.hxx>
#include <sfx2/viewfrm.hxx>
#include <vcl/toolbox.hxx>
#include <osl/mutex.hxx>

#include <sfx2/imagemgr.hxx>
#include <vcl/svapp.hxx>
#include "svx/tbxcustomshapes.hxx"

SFX_IMPL_TOOLBOX_CONTROL(SvxTbxCtlCustomShapes, SfxBoolItem);

SvxTbxCtlCustomShapes::SvxTbxCtlCustomShapes( sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx ) :
    SfxToolBoxControl( nSlotId, nId, rTbx ),
    m_aSubTbxResName( RTL_CONSTASCII_USTRINGPARAM( "private:resource/toolbar/" ) )
{
    switch( nSlotId )
    {
        default :
        {
            DBG_ASSERT( false, "SvxTbxCtlCustomShapes: unknown slot executed. ?" );
        }
        case SID_DRAWTBX_CS_BASIC :
        {
            m_aCommand = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:BasicShapes.diamond" ) );
            m_aSubTbName = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "basicshapes" ) );
        }
        break;

        case SID_DRAWTBX_CS_SYMBOL :
        {
            m_aCommand = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:SymbolShapes.smiley" ) );
            m_aSubTbName = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "symbolshapes" ) );
        }
        break;

        case SID_DRAWTBX_CS_ARROW :
        {
            m_aCommand = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:ArrowShapes.left-right-arrow" ) );
            m_aSubTbName = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "arrowshapes" ) );
        }
        break;
        case SID_DRAWTBX_CS_FLOWCHART :
        {
            m_aCommand = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:FlowChartShapes.flowchart-internal-storage" ) );
            m_aSubTbName = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "flowchartshapes" ) );
        }
        break;
        case SID_DRAWTBX_CS_CALLOUT :
        {
            m_aCommand = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:CalloutShapes.round-rectangular-callout" ) );
            m_aSubTbName = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "calloutshapes" ) );
        }
        break;
        case SID_DRAWTBX_CS_STAR :
        {
            m_aCommand = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:StarShapes.star5" ) );
            m_aSubTbName = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "starshapes" ) );
        }
        break;
    }
    m_aSubTbxResName += m_aSubTbName;
    rTbx.SetItemBits( nId, TIB_DROPDOWN | rTbx.GetItemBits( nId ) );
    rTbx.Invalidate();
}

/*************************************************************************
|*
|* Notification when the application status has changed
|*
\************************************************************************/

void SvxTbxCtlCustomShapes::StateChanged( sal_uInt16 nSID, SfxItemState eState,
                                  const SfxPoolItem* pState )
{
    SfxToolBoxControl::StateChanged( nSID, eState, pState );
}

/*************************************************************************
|*
|* when one wants to create a popup window
|*
\************************************************************************/

SfxPopupWindowType SvxTbxCtlCustomShapes::GetPopupWindowType() const
{
    return( m_aCommand.isEmpty() ? SFX_POPUPWINDOW_ONCLICK : SFX_POPUPWINDOW_ONTIMEOUT);
}

/*************************************************************************
|*
|* Here is the window created
|* The location of the Toolbox is queried through GetToolBox()
|* rItemRect are the screen coordinates
|*
\************************************************************************/

SfxPopupWindow* SvxTbxCtlCustomShapes::CreatePopupWindow()
{
    createAndPositionSubToolBar( m_aSubTbxResName );
    return NULL;
}

// -----------------------------------------------------------------------

void SvxTbxCtlCustomShapes::Select( sal_Bool /*bMod1*/ )
{
     if ( !m_aCommand.isEmpty() )
    {
        com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue > aParamSeq( 0 );
        Dispatch( m_aCommand, aParamSeq );
    }
}


::sal_Bool SAL_CALL SvxTbxCtlCustomShapes::opensSubToolbar() throw (::com::sun::star::uno::RuntimeException)
{
    // We control a sub-toolbar therefore we have to return true.
    return sal_True;
}

::rtl::OUString SAL_CALL SvxTbxCtlCustomShapes::getSubToolbarName() throw (::com::sun::star::uno::RuntimeException)
{
    // Provide the controlled sub-toolbar name, so we are notified whenever
    // this toolbar executes a function.
    return m_aSubTbName;
}

void SAL_CALL SvxTbxCtlCustomShapes::functionSelected( const ::rtl::OUString& rCommand ) throw (::com::sun::star::uno::RuntimeException)
{
    // remind the new command
    m_aCommand = rCommand;
    // Our sub-toolbar wants to execute a function.
    // We have to change the image of our toolbar button to reflect the new function.
    SolarMutexGuard aGuard;
    if ( !m_bDisposed )
    {
        if ( !m_aCommand.isEmpty() )
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > xFrame( getFrameInterface());
            Image aImage = GetImage( xFrame, m_aCommand, hasBigImages() );
            if ( !!aImage )
                GetToolBox().SetItemImage( GetId(), aImage );
        }
    }
}

void SAL_CALL SvxTbxCtlCustomShapes::updateImage(  ) throw (::com::sun::star::uno::RuntimeException)
{
    // We should update the button image of our parent (toolbar).
    // Use the stored command to set the correct current image.
    SolarMutexGuard aGuard;
    if ( !m_aCommand.isEmpty() )
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > xFrame( getFrameInterface());
        Image aImage = GetImage( xFrame, m_aCommand, hasBigImages() );
        if ( !!aImage )
            GetToolBox().SetItemImage( GetId(), aImage );
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
