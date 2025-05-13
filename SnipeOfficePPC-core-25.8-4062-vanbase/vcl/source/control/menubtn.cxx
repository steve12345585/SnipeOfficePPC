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


#include <tools/rc.h>
#include <vcl/decoview.hxx>
#include <vcl/event.hxx>
#include <vcl/menu.hxx>
#include <vcl/timer.hxx>
#include <vcl/menubtn.hxx>
#include <vcl/svapp.hxx>

// =======================================================================

void MenuButton::ImplInitMenuButtonData()
{
    mnDDStyle       = PUSHBUTTON_DROPDOWN_MENUBUTTON;

    mpMenuTimer     = NULL;
    mpMenu          = NULL;
    mpOwnMenu       = NULL;
    mnCurItemId     = 0;
    mnMenuMode      = 0;
}

// -----------------------------------------------------------------------

void MenuButton::ImplInit( Window* pParent, WinBits nStyle )
{
    if ( !(nStyle & WB_NOTABSTOP) )
        nStyle |= WB_TABSTOP;

    PushButton::ImplInit( pParent, nStyle );
    EnableRTL( Application::GetSettings().GetLayoutRTL() );
}

// -----------------------------------------------------------------------

void MenuButton::ImplExecuteMenu()
{
    Activate();

    if ( mpMenu )
    {
        Point aPos( 0, 1 );
        Size aSize = GetSizePixel();
        Rectangle aRect( aPos, aSize );
        SetPressed( sal_True );
        EndSelection();
        mnCurItemId = mpMenu->Execute( this, aRect, POPUPMENU_EXECUTE_DOWN );
        SetPressed( sal_False );
        if ( mnCurItemId )
        {
            Select();
            mnCurItemId = 0;
        }
    }
}

// -----------------------------------------------------------------------

MenuButton::MenuButton( Window* pParent, WinBits nWinBits )
    : PushButton( WINDOW_MENUBUTTON )
    , mbDisplaySelectedItem(false)
{
    ImplInitMenuButtonData();
    ImplInit( pParent, nWinBits );
}

// -----------------------------------------------------------------------

MenuButton::MenuButton( Window* pParent, const ResId& rResId )
    : PushButton( WINDOW_MENUBUTTON )
    , mbDisplaySelectedItem(false)
{
    ImplInitMenuButtonData();
    rResId.SetRT( RSC_MENUBUTTON );
    WinBits nStyle = ImplInitRes( rResId );
    ImplInit( pParent, nStyle );
    ImplLoadRes( rResId );

    if ( !(nStyle & WB_HIDE) )
        Show();
}

// -----------------------------------------------------------------------

void MenuButton::ImplLoadRes( const ResId& rResId )
{
    Control::ImplLoadRes( rResId );

    sal_uLong nObjMask = ReadLongRes();

    if ( RSCMENUBUTTON_MENU & nObjMask )
    {
        mpOwnMenu = new PopupMenu( ResId( (RSHEADER_TYPE*)GetClassRes(), *rResId.GetResMgr() ) );
        SetPopupMenu( mpOwnMenu );
        IncrementRes( GetObjSizeRes( (RSHEADER_TYPE*)GetClassRes() ) );
    }
}

// -----------------------------------------------------------------------

MenuButton::~MenuButton()
{
    delete mpMenuTimer;
    if (mbDisplaySelectedItem && mpMenu)
        mpMenu->RemoveEventListener(LINK(this, MenuButton, MenuEventListener));
    delete mpOwnMenu;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(MenuButton, ImplMenuTimeoutHdl)
{
    // Abfragen, ob Button-Benutzung noch aktiv ist, da diese ja auch
    // vorher abgebrochen wurden sein koennte
    if ( IsTracking() )
    {
        if ( !(GetStyle() & WB_NOPOINTERFOCUS) )
            GrabFocus();
        ImplExecuteMenu();
    }

    return 0;
}

// -----------------------------------------------------------------------

void MenuButton::MouseButtonDown( const MouseEvent& rMEvt )
{
    bool bExecute = true;
    if ( mnMenuMode & MENUBUTTON_MENUMODE_TIMED )
    {
        // if the separated dropdown symbol is not hit, delay the popup execution
        if( mnDDStyle != PUSHBUTTON_DROPDOWN_MENUBUTTON || // no separator at all
            rMEvt.GetPosPixel().X() <= ImplGetSeparatorX() )
        {
            if ( !mpMenuTimer )
            {
                mpMenuTimer = new Timer;
                mpMenuTimer->SetTimeoutHdl( LINK( this, MenuButton, ImplMenuTimeoutHdl ) );
            }

            mpMenuTimer->SetTimeout( GetSettings().GetMouseSettings().GetActionDelay() );
            mpMenuTimer->Start();

            PushButton::MouseButtonDown( rMEvt );
            bExecute = false;
        }
    }
    if( bExecute )
    {
        if ( PushButton::ImplHitTestPushButton( this, rMEvt.GetPosPixel() ) )
        {
            if ( !(GetStyle() & WB_NOPOINTERFOCUS) )
                GrabFocus();
            ImplExecuteMenu();
        }
    }
}

// -----------------------------------------------------------------------

void MenuButton::KeyInput( const KeyEvent& rKEvt )
{
    KeyCode aKeyCode = rKEvt.GetKeyCode();
    sal_uInt16 nCode = aKeyCode.GetCode();
    if ( (nCode == KEY_DOWN) && aKeyCode.IsMod2() )
        ImplExecuteMenu();
    else if ( !(mnMenuMode & MENUBUTTON_MENUMODE_TIMED) &&
              !aKeyCode.GetModifier() &&
              ((nCode == KEY_RETURN) || (nCode == KEY_SPACE)) )
        ImplExecuteMenu();
    else
        PushButton::KeyInput( rKEvt );
}

// -----------------------------------------------------------------------

void MenuButton::Activate()
{
    maActivateHdl.Call( this );
}

// -----------------------------------------------------------------------

void MenuButton::Select()
{
    updateText();
    maSelectHdl.Call( this );
}

void MenuButton::updateText()
{
    if (mbDisplaySelectedItem)
    {
        if (mpMenu)
            SetText(mpMenu->GetItemText(mpMenu->GetCurItemId()));
        else
            SetText(OUString());
    }
}

Size MenuButton::GetOptimalSize(WindowSizeType eType) const
{
    Size aRet = PushButton::GetOptimalSize(eType);
    if (mbDisplaySelectedItem && mpMenu)
    {
        Size aMenuSize(mpMenu->ImplCalcSize(const_cast<MenuButton*>(this)));
        if (aMenuSize.Width() > aRet.Width())
            aRet.Width() = aMenuSize.Width();
    }
    return aRet;
}

// -----------------------------------------------------------------------

void MenuButton::SetMenuMode( sal_uInt16 nMode )
{
    // Fuer die 5.1-Auslieferung besser noch nicht inline, ansonsten kann
    // diese Funktion zur 6.0 inline werden
    mnMenuMode = nMode;
}

void MenuButton::SetShowDisplaySelectedItem(bool bShow)
{
    if (mbDisplaySelectedItem == bShow)
        return;
    if (mbDisplaySelectedItem && mpMenu)
        mpMenu->RemoveEventListener(LINK(this, MenuButton, MenuEventListener));
    mbDisplaySelectedItem = bShow;
    if (mbDisplaySelectedItem && mpMenu)
        mpMenu->AddEventListener(LINK(this, MenuButton, MenuEventListener));
}

void MenuButton::SetPopupMenu( PopupMenu* pNewMenu )
{
    if (pNewMenu == mpMenu)
        return;
    if (mbDisplaySelectedItem && mpMenu)
        mpMenu->RemoveEventListener(LINK(this, MenuButton, MenuEventListener));
    // Fuer die 5.1-Auslieferung besser noch nicht inline, ansonsten kann
    // diese Funktion zur 6.0 inline werden
    mpMenu = pNewMenu;
    updateText();
    if (mbDisplaySelectedItem && mpMenu)
        mpMenu->AddEventListener(LINK(this, MenuButton, MenuEventListener));
}

IMPL_LINK(MenuButton, MenuEventListener, VclSimpleEvent*, pEvent)
{
    if (pEvent && pEvent->ISA(VclMenuEvent))
    {
        VclMenuEvent* pMenuEvent = (VclMenuEvent*)pEvent;
        if (pMenuEvent->GetMenu() == mpMenu)
        {
            switch (pMenuEvent->GetId())
            {
                case VCLEVENT_MENU_INSERTITEM:
                case VCLEVENT_MENU_REMOVEITEM:
                case VCLEVENT_MENU_ITEMTEXTCHANGED:
                    queue_resize();
                    break;
                case VCLEVENT_MENU_SELECT:
                    updateText();
                    break;
            }
        }
    }
    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
