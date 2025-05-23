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

#include <viewlayoutctrl.hxx>

#include <vcl/status.hxx>
#include <vcl/image.hxx>
#include <svl/eitem.hxx>
#include <svx/viewlayoutitem.hxx>
#include <utlui.hrc>
#include <swtypes.hxx>  // fuer Pathfinder

// STATIC DATA -----------------------------------------------------------

SFX_IMPL_STATUSBAR_CONTROL( SwViewLayoutControl, SvxViewLayoutItem );

const long nImageWidthSingle = 14;
const long nImageWidthAuto = 24;
const long nImageWidthBook = 22;
const long nImageWidthSum = nImageWidthSingle + nImageWidthAuto + nImageWidthBook;
const long nImageHeight = 10;

struct SwViewLayoutControl::SwViewLayoutControl_Impl
{
    sal_uInt16      mnState; // 0 = single, 1 = auto, 2 = book, 3 = none
    Image       maImageSingleColumn;
    Image       maImageSingleColumn_Active;
    Image       maImageAutomatic;
    Image       maImageAutomatic_Active;
    Image       maImageBookMode;
    Image       maImageBookMode_Active;
};

// class SwViewLayoutControl ------------------------------------------

SwViewLayoutControl::SwViewLayoutControl( sal_uInt16 _nSlotId, sal_uInt16 _nId, StatusBar& rStb ) :
    SfxStatusBarControl( _nSlotId, _nId, rStb ),
    mpImpl( new SwViewLayoutControl_Impl )
{
    mpImpl->mnState = 0;

    mpImpl->maImageSingleColumn         = Image( SW_RES(IMG_VIEWLAYOUT_SINGLECOLUMN) );
    mpImpl->maImageSingleColumn_Active  = Image( SW_RES(IMG_VIEWLAYOUT_SINGLECOLUMN_ACTIVE) );
    mpImpl->maImageAutomatic            = Image( SW_RES(IMG_VIEWLAYOUT_AUTOMATIC) );
    mpImpl->maImageAutomatic_Active     = Image( SW_RES(IMG_VIEWLAYOUT_AUTOMATIC_ACTIVE) );
    mpImpl->maImageBookMode             = Image( SW_RES(IMG_VIEWLAYOUT_BOOKMODE) );
    mpImpl->maImageBookMode_Active      = Image( SW_RES(IMG_VIEWLAYOUT_BOOKMODE_ACTIVE) );
}

SwViewLayoutControl::~SwViewLayoutControl()
{
    delete mpImpl;
}

void SwViewLayoutControl::StateChanged( sal_uInt16 /*nSID*/, SfxItemState eState, const SfxPoolItem* pState )
{
    if ( SFX_ITEM_AVAILABLE != eState || pState->ISA( SfxVoidItem ) )
        GetStatusBar().SetItemText( GetId(), String() );
    else
    {
        OSL_ENSURE( pState->ISA( SvxViewLayoutItem ), "invalid item type" );
        const sal_uInt16 nColumns  = static_cast<const SvxViewLayoutItem*>( pState )->GetValue();
        const bool   bBookMode = static_cast<const SvxViewLayoutItem*>( pState )->IsBookMode();

        // SingleColumn Mode
        if ( 1 == nColumns )
            mpImpl->mnState = 0;
        // Automatic Mode
        else if ( 0 == nColumns )
            mpImpl->mnState = 1;
        // Book Mode
        else if ( bBookMode && 2 == nColumns )
            mpImpl->mnState = 2;
        else
            mpImpl->mnState = 3;
    }

    if ( GetStatusBar().AreItemsVisible() )
        GetStatusBar().SetItemData( GetId(), 0 );    // force repaint
}

void SwViewLayoutControl::Paint( const UserDrawEvent& rUsrEvt )
{
    OutputDevice*       pDev =  rUsrEvt.GetDevice();
    Rectangle           aRect = rUsrEvt.GetRect();

    const Rectangle aControlRect = getControlRect();

    const bool bSingleColumn    = 0 == mpImpl->mnState;
    const bool bAutomatic       = 1 == mpImpl->mnState;
    const bool bBookMode        = 2 == mpImpl->mnState;

    const long nXOffset = (aRect.GetWidth()  - nImageWidthSum)/2;
    const long nYOffset = (aControlRect.GetHeight() - nImageHeight)/2;

    aRect.Left() = aRect.Left() + nXOffset;
    aRect.Top()  = aRect.Top() + nYOffset;

    // draw single column image:
    pDev->DrawImage( aRect.TopLeft(), bSingleColumn ? mpImpl->maImageSingleColumn_Active : mpImpl->maImageSingleColumn );

    // draw automatic image:
    aRect.Left() += nImageWidthSingle;
    pDev->DrawImage( aRect.TopLeft(), bAutomatic ? mpImpl->maImageAutomatic_Active       : mpImpl->maImageAutomatic );

    // draw bookmode image:
    aRect.Left() += nImageWidthAuto;
    pDev->DrawImage( aRect.TopLeft(), bBookMode ? mpImpl->maImageBookMode_Active         : mpImpl->maImageBookMode );
}

sal_Bool SwViewLayoutControl::MouseButtonDown( const MouseEvent & rEvt )
{
    const Rectangle aRect = getControlRect();
    const Point aPoint = rEvt.GetPosPixel();
    const long nXDiff = aPoint.X() - aRect.Left();

    sal_uInt16 nColumns = 1;
    bool bBookMode = false;

    const long nXOffset = (aRect.GetWidth() - nImageWidthSum)/2;

    if ( nXDiff < nXOffset + nImageWidthSingle )
    {
        mpImpl->mnState = 0; // single
        nColumns = 1;
    }
    else if ( nXDiff < nXOffset + nImageWidthSingle + nImageWidthAuto )
    {
        mpImpl->mnState = 1; // auto
        nColumns = 0;
    }
    else
    {
        mpImpl->mnState = 2; // book
        nColumns = 2;
        bBookMode = true;
    }

    // commit state change
    SvxViewLayoutItem aViewLayout( nColumns, bBookMode );

    ::com::sun::star::uno::Any a;
    aViewLayout.QueryValue( a );

    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aArgs( 1 );
    aArgs[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ViewLayout" ));
    aArgs[0].Value = a;

    execute( aArgs );

    return sal_True;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
