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


#include <salframe.hxx>
#include <salinst.hxx>
#include <salvd.hxx>
#include <salprn.hxx>
#include <saltimer.hxx>
#include <salimestatus.hxx>
#include <salsys.hxx>
#include <salbmp.hxx>
#include <salobj.hxx>
#include <salmenu.hxx>

// this file contains the virtual destructors of the sal interface
// compilers ususally put their vtables where the destructor is

SalFrame::~SalFrame()
{
}

// -----------------------------------------------------------------------

// default to full-frame flushes
// on ports where partial-flushes are much cheaper this method should be overridden
void SalFrame::Flush( const Rectangle& )
{
    Flush();
}

// -----------------------------------------------------------------------

void SalFrame::SetRepresentedURL( const rtl::OUString& )
{
    // currently this is Mac only functionality
}

// -----------------------------------------------------------------------

SalInstance::~SalInstance()
{
}

void SalInstance::FillFontPathList( std::list< rtl::OString >& )
{
    // do nothing
}

SalMenu* SalInstance::CreateMenu( sal_Bool, Menu* )
{
    // default: no native menus
    return NULL;
}

void SalInstance::DestroyMenu( SalMenu* pMenu )
{
    (void)pMenu;
    OSL_ENSURE( pMenu == 0, "DestroyMenu called with non-native menus" );
}

SalMenuItem* SalInstance::CreateMenuItem( const SalItemParams* )
{
    return NULL;
}

void SalInstance::DestroyMenuItem( SalMenuItem* pItem )
{
    (void)pItem;
    OSL_ENSURE( pItem == 0, "DestroyMenu called with non-native menus" );
}

SalTimer::~SalTimer()
{
}

SalBitmap::~SalBitmap()
{
}

SalI18NImeStatus::~SalI18NImeStatus()
{
}

SalSystem::~SalSystem()
{
}

SalPrinter::~SalPrinter()
{
}

sal_Bool SalPrinter::StartJob( const rtl::OUString*, const rtl::OUString&, const rtl::OUString&,
                           ImplJobSetup*, vcl::PrinterController& )
{
    return sal_False;
}

SalInfoPrinter::~SalInfoPrinter()
{
}

SalVirtualDevice::~SalVirtualDevice()
{
}

SalObject::~SalObject()
{
}

SalMenu::~SalMenu()
{
}

bool SalMenu::ShowNativePopupMenu(FloatingWindow *, const Rectangle&, sal_uLong )
{
    return false;
}

bool SalMenu::AddMenuBarButton( const SalMenuButtonItem& )
{
    return false;
}

void SalMenu::RemoveMenuBarButton( sal_uInt16 )
{
}

Rectangle SalMenu::GetMenuBarButtonRectPixel( sal_uInt16, SalFrame* )
{
    return Rectangle();
}

SalMenuItem::~SalMenuItem()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
