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

#ifndef _SV_MENUBTN_HXX
#define _SV_MENUBTN_HXX

#include <tools/solar.h>
#include <vcl/dllapi.h>
#include <vcl/button.hxx>

class Timer;
class PopupMenu;
class VclBuilder;
class VclSimpleEvent;

// --------------------
// - MenuButton-Types -
// --------------------

#define MENUBUTTON_MENUMODE_TIMED       ((sal_uInt16)0x0001)

// --------------
// - MenuButton -
// --------------

class VCL_DLLPUBLIC MenuButton : public PushButton
{
private:
    friend class VclBuilder;

    Rectangle       maFocusRect;
    Timer*          mpMenuTimer;
    PopupMenu*      mpOwnMenu;
    PopupMenu*      mpMenu;
    sal_uInt16      mnCurItemId;
    sal_uInt16      mnMenuMode;
    bool            mbDisplaySelectedItem;
    Link            maActivateHdl;
    Link            maSelectHdl;

    SAL_DLLPRIVATE void    ImplInitMenuButtonData();
    SAL_DLLPRIVATE void    ImplExecuteMenu();
    DECL_DLLPRIVATE_LINK(  ImplMenuTimeoutHdl, void* );

    SAL_DLLPRIVATE void    updateText();

    // Copy assignment is forbidden and not implemented.
    SAL_DLLPRIVATE         MenuButton( const MenuButton & );
    SAL_DLLPRIVATE         MenuButton& operator=( const MenuButton & );

    DECL_LINK(MenuEventListener, VclSimpleEvent*);

protected:
    using Window::ImplInit;
    SAL_DLLPRIVATE void    ImplInit( Window* pParent, WinBits nStyle );
    SAL_DLLPRIVATE void    ImplLoadRes( const ResId& rResId );

public:
                    MenuButton( Window* pParent, WinBits nStyle = 0 );
                    MenuButton( Window* pParent, const ResId& rResId );
                    ~MenuButton();

    virtual void    MouseButtonDown( const MouseEvent& rMEvt );
    virtual void    KeyInput( const KeyEvent& rKEvt );

    virtual void    Activate();
    virtual void    Select();

    void            SetMenuMode( sal_uInt16 nMode );
    sal_uInt16          GetMenuMode() const { return mnMenuMode; }

    void            SetPopupMenu( PopupMenu* pNewMenu );
    PopupMenu*      GetPopupMenu() const { return mpMenu; }

    sal_uInt16      GetCurItemId() const { return mnCurItemId; }
    void            SetCurItemId( sal_uInt16 nItemId ) { mnCurItemId = nItemId; }

    void            SetActivateHdl( const Link& rLink ) { maActivateHdl = rLink; }
    const Link&     GetActivateHdl() const              { return maActivateHdl; }
    void            SetSelectHdl( const Link& rLink )   { maSelectHdl = rLink; }
    const Link&     GetSelectHdl() const                { return maSelectHdl; }

    void            SetShowDisplaySelectedItem(bool bShow);

    virtual Size    GetOptimalSize(WindowSizeType eType) const;
};

#endif  // _SV_MENUBTN_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
