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


#include <string.h>

#include "aqua/saldata.hxx"
#include "aqua/salobj.h"
#include "aqua/salframe.h"

// =======================================================================

AquaSalObject::AquaSalObject( AquaSalFrame* pFrame ) :
    mpFrame( pFrame ),
    mnClipX( -1 ),
    mnClipY( -1 ),
    mnClipWidth( -1 ),
    mnClipHeight( -1 ),
    mbClip( false ),
    mnX( 0 ),
    mnY( 0 ),
    mnWidth( 20 ),
    mnHeight( 20 )
{
    maSysData.nSize = sizeof( maSysData );
    maSysData.pView = NULL;

    NSRect aInitFrame = { { 0, 0 }, { 20, 20 } };
    mpClipView = [[NSClipView alloc] initWithFrame: aInitFrame ];
    if( mpClipView )
    {
        [mpFrame->getView() addSubview: mpClipView];
        [mpClipView setHidden: YES];
    }
    maSysData.pView = [[NSView alloc] initWithFrame: aInitFrame];
    if( maSysData.pView )
    {
        if( mpClipView )
            [mpClipView setDocumentView: maSysData.pView];
    }
}

// -----------------------------------------------------------------------

AquaSalObject::~AquaSalObject()
{
    if( maSysData.pView )
    {
        NSView *pView = maSysData.pView;
        [pView removeFromSuperview];
        [pView release];
    }
    if( mpClipView )
    {
        [mpClipView removeFromSuperview];
        [mpClipView release];
    }
}

/*
   sadly there seems to be no way to impose clipping on a child view,
   especially a QTMovieView which seems to ignore the current context
   completely. Also there is no real way to shape a window; on Aqua a
   similar effect to non-rectangular windows is achieved by using a
   non-opaque window and not painting where one wants the background
   to shine through.

   With respect to SalObject this leaves us to having an NSClipView
   containing the child view. Even a QTMovieView respects the boundaries of
   that, which gives us a clip "region" consisting of one rectangle.
   This is gives us an 80% solution only, though.
*/

// -----------------------------------------------------------------------

void AquaSalObject::ResetClipRegion()
{
    mbClip = false;
    setClippedPosSize();
}

// -----------------------------------------------------------------------

sal_uInt16 AquaSalObject::GetClipRegionType()
{
    return SAL_OBJECT_CLIP_INCLUDERECTS;
}

// -----------------------------------------------------------------------

void AquaSalObject::BeginSetClipRegion( sal_uLong )
{
    mbClip = false;
}

// -----------------------------------------------------------------------

void AquaSalObject::UnionClipRegion( long nX, long nY, long nWidth, long nHeight )
{
    if( mbClip )
    {
        if( nX < mnClipX )
        {
            mnClipWidth += mnClipX - nX;
            mnClipX = nX;
        }
        if( nX + nWidth > mnClipX + mnClipWidth )
            mnClipWidth = nX + nWidth - mnClipX;
        if( nY < mnClipY )
        {
            mnClipHeight += mnClipY - nY;
            mnClipY = nY;
        }
        if( nY + nHeight > mnClipY + mnClipHeight )
            mnClipHeight = nY + nHeight - mnClipY;
    }
    else
    {
        mnClipX = nX;
        mnClipY = nY;
        mnClipWidth = nWidth;
        mnClipHeight = nHeight;
        mbClip = true;
    }
}

// -----------------------------------------------------------------------

void AquaSalObject::EndSetClipRegion()
{
    setClippedPosSize();
}

// -----------------------------------------------------------------------

void AquaSalObject::SetPosSize( long nX, long nY, long nWidth, long nHeight )
{
    mnX = nX;
    mnY = nY;
    mnWidth = nWidth;
    mnHeight = nHeight;
    setClippedPosSize();
}

// -----------------------------------------------------------------------

void AquaSalObject::setClippedPosSize()
{
    NSRect aViewRect = { { 0, 0 }, { mnWidth, mnHeight } };
    if( maSysData.pView )
    {
        NSView *pView = maSysData.pView;
        [pView setFrame: aViewRect];
    }

    NSRect aClipViewRect = { { mnX, mnY }, { mnWidth, mnHeight } };
    NSPoint aClipPt = { 0, 0 };
    if( mbClip )
    {
        aClipViewRect.origin.x += mnClipX;
        aClipViewRect.origin.y += mnClipY;
        aClipViewRect.size.width = mnClipWidth;
        aClipViewRect.size.height = mnClipHeight;
        aClipPt.x = mnClipX;
        if( mnClipY == 0 )
            aClipPt.y = mnHeight - mnClipHeight;
    }

    mpFrame->VCLToCocoa( aClipViewRect, false );
    [mpClipView setFrame: aClipViewRect];

    [mpClipView scrollToPoint: aClipPt];
}

// -----------------------------------------------------------------------

void AquaSalObject::Show( sal_Bool bVisible )
{
    if( mpClipView )
        [mpClipView setHidden: (bVisible ? NO : YES)];
}

// -----------------------------------------------------------------------

const SystemEnvData* AquaSalObject::GetSystemData() const
{
    return &maSysData;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
