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

#include "ids.hrc"
#include "openlocked.hxx"

OpenLockedQueryBox::OpenLockedQueryBox( Window* pParent, ResMgr* pResMgr, const String& aMessage ) :
    MessBox(pParent, 0,
            ResId(STR_OPENLOCKED_TITLE, *pResMgr).toString(),
            aMessage )
{
    SetImage( QueryBox::GetStandardImage() );

    AddButton(ResId(STR_OPENLOCKED_OPENREADONLY_BTN, *pResMgr).toString(), RET_YES,
            BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_OKBUTTON | BUTTONDIALOG_FOCUSBUTTON);

    AddButton(ResId(STR_OPENLOCKED_OPENCOPY_BTN, *pResMgr).toString(), RET_NO, 0);

    AddButton( BUTTON_CANCEL, RET_CANCEL, BUTTONDIALOG_CANCELBUTTON );
    SetButtonHelpText( RET_YES, String() );
    SetButtonHelpText( RET_NO, String() );

#ifdef WNT
    // bnc#656566
    // Yes, it is silly to do this only for this dialog but not the
    // other similar ones. But hey, it was about this dialog that the
    // customer complained. You who read this and feel the itch, feel
    // free to fix the problem in a better way.
    EnableAlwaysOnTop( true );
#endif
}

OpenLockedQueryBox::~OpenLockedQueryBox()
{
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
