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

#include <AnnotationMenuButton.hxx>

#include <annotation.hrc>
#include <app.hrc>
#include <access.hrc>

#include <unotools/useroptions.hxx>

#include <vcl/svapp.hxx>
#include <vcl/menu.hxx>
#include <vcl/decoview.hxx>
#include <vcl/gradient.hxx>

#include <cmdid.h>
#include <SidebarWin.hxx>

namespace sw { namespace annotation {

Color ColorFromAlphaColor( const sal_uInt8 aTransparency,
                           const Color &aFront,
                           const Color &aBack )
{
    return Color((sal_uInt8)(aFront.GetRed()    * aTransparency/(double)255 + aBack.GetRed()    * (1-aTransparency/(double)255)),
                 (sal_uInt8)(aFront.GetGreen()  * aTransparency/(double)255 + aBack.GetGreen()  * (1-aTransparency/(double)255)),
                 (sal_uInt8)(aFront.GetBlue()   * aTransparency/(double)255 + aBack.GetBlue()   * (1-aTransparency/(double)255)));
}

AnnotationMenuButton::AnnotationMenuButton( sw::sidebarwindows::SwSidebarWin& rSidebarWin )
    : MenuButton( &rSidebarWin )
    , mrSidebarWin( rSidebarWin )
{
    AddEventListener( LINK( &mrSidebarWin, sw::sidebarwindows::SwSidebarWin, WindowEventListener ) );

    SetAccessibleName( SW_RES( STR_ACCESS_ANNOTATION_BUTTON_NAME ) );
    SetAccessibleDescription( SW_RES( STR_ACCESS_ANNOTATION_BUTTON_DESC ) );
    SetQuickHelpText( GetAccessibleDescription() );
}

AnnotationMenuButton::~AnnotationMenuButton()
{
    RemoveEventListener( LINK( &mrSidebarWin, sw::sidebarwindows::SwSidebarWin, WindowEventListener ) );
}

void AnnotationMenuButton::Select()
{
    mrSidebarWin.ExecuteCommand( GetCurItemId() );
}

void AnnotationMenuButton::MouseButtonDown( const MouseEvent& rMEvt )
{
    PopupMenu* pButtonPopup( GetPopupMenu() );
    if ( mrSidebarWin.IsReadOnly() )
    {
        pButtonPopup->EnableItem( FN_REPLY, false );
        pButtonPopup->EnableItem( FN_DELETE_COMMENT, false );
        pButtonPopup->EnableItem( FN_DELETE_NOTE_AUTHOR, false );
        pButtonPopup->EnableItem( FN_DELETE_ALL_NOTES, false );
    }
    else
    {
        pButtonPopup->EnableItem( FN_DELETE_COMMENT, !mrSidebarWin.IsProtected() );
        pButtonPopup->EnableItem( FN_DELETE_NOTE_AUTHOR, true );
        pButtonPopup->EnableItem( FN_DELETE_ALL_NOTES, true );
    }

    if ( mrSidebarWin.IsProtected() )
    {
        pButtonPopup->EnableItem( FN_REPLY, false );
    }
    else
    {
        SvtUserOptions aUserOpt;
        String sAuthor;
        if ( !(sAuthor = aUserOpt.GetFullName()).Len() )
        {
            if ( !(sAuthor = aUserOpt.GetID()).Len() )
            {
                sAuthor = String( SW_RES( STR_REDLINE_UNKNOWN_AUTHOR ));
            }
        }
        // do not allow to reply to ourself and no answer possible if this note is in a protected section
        if ( sAuthor == mrSidebarWin.GetAuthor() )
        {
            pButtonPopup->EnableItem( FN_REPLY, false );
        }
        else
        {
            pButtonPopup->EnableItem( FN_REPLY, true );
        }
    }

    MenuButton::MouseButtonDown( rMEvt );
}

void AnnotationMenuButton::Paint( const Rectangle& /*rRect*/ )
{
    if ( Application::GetSettings().GetStyleSettings().GetHighContrastMode() )
        SetFillColor(COL_BLACK);
    else
        SetFillColor( mrSidebarWin.ColorDark() );
    SetLineColor();
    const Rectangle aRect( Rectangle( Point( 0, 0 ), PixelToLogic( GetSizePixel() ) ) );
    DrawRect( aRect );

    if ( Application::GetSettings().GetStyleSettings().GetHighContrastMode())
    {
        //draw rect around button
        SetFillColor(COL_BLACK);
        SetLineColor(COL_WHITE);
    }
    else
    {
        //draw button
        Gradient aGradient;
        if ( IsMouseOver() )
            aGradient = Gradient( GradientStyle_LINEAR,
                                  ColorFromAlphaColor( 80, mrSidebarWin.ColorAnchor(), mrSidebarWin.ColorDark() ),
                                  ColorFromAlphaColor( 15, mrSidebarWin.ColorAnchor(), mrSidebarWin.ColorDark() ));
        else
            aGradient = Gradient( GradientStyle_LINEAR,
                                  ColorFromAlphaColor( 15, mrSidebarWin.ColorAnchor(), mrSidebarWin.ColorDark() ),
                                  ColorFromAlphaColor( 80, mrSidebarWin.ColorAnchor(), mrSidebarWin.ColorDark() ));
        DrawGradient( aRect, aGradient );

        //draw rect around button
        SetFillColor();
        SetLineColor( ColorFromAlphaColor( 90, mrSidebarWin.ColorAnchor(), mrSidebarWin.ColorDark() ));
    }
    DrawRect( aRect );

    if ( mrSidebarWin.IsPreview() )
    {
        Font aOldFont( mrSidebarWin.GetFont() );
        Font aFont(aOldFont);
        Color aCol( COL_BLACK);
        aFont.SetColor( aCol );
        aFont.SetHeight(200);
        aFont.SetWeight(WEIGHT_MEDIUM);
        SetFont( aFont );
        DrawText(  aRect ,
                   rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Edit Note")),
                   TEXT_DRAW_CENTER );
        SetFont( aOldFont );
    }
    else
    {
        Rectangle aSymbolRect( aRect );
        // 25% distance to the left and right button border
        const long nBorderDistanceLeftAndRight = ((aSymbolRect.GetWidth()*250)+500)/1000;
        aSymbolRect.Left()+=nBorderDistanceLeftAndRight;
        aSymbolRect.Right()-=nBorderDistanceLeftAndRight;
        // 40% distance to the top button border
        const long nBorderDistanceTop = ((aSymbolRect.GetHeight()*400)+500)/1000;
        aSymbolRect.Top()+=nBorderDistanceTop;
        // 15% distance to the bottom button border
        const long nBorderDistanceBottom = ((aSymbolRect.GetHeight()*150)+500)/1000;
        aSymbolRect.Bottom()-=nBorderDistanceBottom;
        DecorationView aDecoView( this );
        aDecoView.DrawSymbol( aSymbolRect, SYMBOL_SPIN_DOWN,
                              ( Application::GetSettings().GetStyleSettings().GetHighContrastMode()
                                ? Color( COL_WHITE )
                                : Color( COL_BLACK ) ) );
    }
}

void AnnotationMenuButton::KeyInput( const KeyEvent& rKeyEvt )
{
    const KeyCode& rKeyCode = rKeyEvt.GetKeyCode();
    const sal_uInt16 nKey = rKeyCode.GetCode();
    if ( nKey == KEY_TAB )
    {
        mrSidebarWin.ActivatePostIt();
        mrSidebarWin.GrabFocus();
    }
    else
    {
        MenuButton::KeyInput( rKeyEvt );
    }
}

} } // end of namespace sw::annotation

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
