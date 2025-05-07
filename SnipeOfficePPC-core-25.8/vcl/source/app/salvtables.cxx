/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


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
