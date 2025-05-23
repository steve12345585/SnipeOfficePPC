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

#ifndef _SV_SALMENU_HXX
#define _SV_SALMENU_HXX

#include <tools/solar.h>
#include "vcl/dllapi.h"
#include <vcl/menu.hxx>
#include <vcl/keycod.hxx>
#include <vcl/image.hxx>

struct SystemMenuData;
class FloatingWindow;
class SalFrame;

struct SalItemParams
{
    sal_uInt16      nId;                    // item Id
    MenuItemType    eType;                  // MenuItem-Type
    MenuItemBits    nBits;                  // MenuItem-Bits
    Menu*           pMenu;                  // Pointer to Menu
    rtl::OUString   aText;                  // Menu-Text
    Image           aImage;                 // Image
};


struct SalMenuButtonItem
{
    sal_uInt16          mnId;
    Image               maImage;
    rtl::OUString       maToolTipText;

    SalMenuButtonItem() : mnId( 0 ) {}
    SalMenuButtonItem( sal_uInt16 i_nId, const Image& rImg, const rtl::OUString& i_rTTText = rtl::OUString() )
    : mnId( i_nId ), maImage( rImg ), maToolTipText( i_rTTText ) {}
};

class VCL_PLUGIN_PUBLIC SalMenuItem
{
public:
    SalMenuItem() {}
    virtual ~SalMenuItem();
};

class VCL_PLUGIN_PUBLIC SalMenu
{
public:
    SalMenu() {}
    virtual ~SalMenu();

    virtual sal_Bool VisibleMenuBar() = 0;  // must return sal_True to actually DISPLAY native menu bars
                                            // otherwise only menu messages are processed (eg, OLE on Windows)

    virtual void InsertItem( SalMenuItem* pSalMenuItem, unsigned nPos ) = 0;
    virtual void RemoveItem( unsigned nPos ) = 0;
    virtual void SetSubMenu( SalMenuItem* pSalMenuItem, SalMenu* pSubMenu, unsigned nPos ) = 0;
    virtual void SetFrame( const SalFrame* pFrame ) = 0;
    virtual void CheckItem( unsigned nPos, sal_Bool bCheck ) = 0;
    virtual void EnableItem( unsigned nPos, sal_Bool bEnable ) = 0;
    virtual void SetItemText( unsigned nPos, SalMenuItem* pSalMenuItem, const rtl::OUString& rText )= 0;
    virtual void SetItemImage( unsigned nPos, SalMenuItem* pSalMenuItem, const Image& rImage ) = 0;
    virtual void SetAccelerator( unsigned nPos, SalMenuItem* pSalMenuItem, const KeyCode& rKeyCode, const rtl::OUString& rKeyName ) = 0;
    virtual void GetSystemMenuData( SystemMenuData* pData ) = 0;
    virtual bool ShowNativePopupMenu(FloatingWindow * pWin, const Rectangle& rRect, sal_uLong nFlags);
    virtual bool AddMenuBarButton( const SalMenuButtonItem& ); // return false if not implemented or failure
    virtual void RemoveMenuBarButton( sal_uInt16 nId );

    // TODO: implement show/hide for the Win/Mac VCL native backends
    virtual void ShowItem( unsigned nPos, sal_Bool bShow ) { EnableItem( nPos, bShow ); }

    // return an empty rectangle if not implemented
    // return Rectangle( Point( -1, -1 ), Size( 1, 1 ) ) if menu bar buttons implemented
    // but rectangle cannot be determined
    virtual Rectangle GetMenuBarButtonRectPixel( sal_uInt16 i_nItemId, SalFrame* i_pReferenceFrame );
};


#endif // _SV_SALMENU_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
