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


#include <svtools/stdctrl.hxx>

// =======================================================================

FixedInfo::FixedInfo( Window* pParent, WinBits nWinStyle ) :
    FixedText( pParent, nWinStyle | WB_INFO )
{
}

// -----------------------------------------------------------------------

FixedInfo::FixedInfo( Window* pParent, const ResId& rResId ) :
    FixedText( pParent, rResId )
{
    SetStyle( GetStyle() | WB_INFO );
}

namespace svt
{
    // class svt::SelectableFixedText ----------------------------------------

    SelectableFixedText::SelectableFixedText( Window* pParent, const ResId& rResId ) :
        Edit( pParent, rResId )
    {
        Init();
    }

    // -----------------------------------------------------------------------

    SelectableFixedText::~SelectableFixedText()
    {
    }

    // -----------------------------------------------------------------------

    void SelectableFixedText::Init()
    {
        // no border
        SetBorderStyle( WINDOW_BORDER_NOBORDER );
        // read-only
        SetReadOnly();
        // make it transparent
        SetControlBackground();
        SetBackground();
        SetPaintTransparent( sal_True );
    }

    // -----------------------------------------------------------------------

    void SelectableFixedText::LoseFocus()
    {
        Edit::LoseFocus();
        // clear cursor
        Invalidate();
    }

} // namespace svt

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
