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



#include <stdio.h>

#include "unx/saldisp.hxx"
#include "unx/saldata.hxx"
#include "unx/i18n_xkb.hxx"

SalI18N_KeyboardExtension::SalI18N_KeyboardExtension( Display* pDisplay )
    : mbUseExtension( sal_True ),
      mnDefaultGroup( 0 )
{
    mpDisplay = pDisplay;

    // allow user to set the default keyboard group idx or to disable the usage
    // of x keyboard extension at all:
    //      setenv SAL_XKEYBOARDGROUP       disables keyboard extension
    //      setenv SAL_XKEYBOARDGROUP 2     sets the keyboard group index to 2
    // keyboard group index must be in [1,4], may be specified in hex or decimal
    static char *pUseKeyboardExtension = getenv( "SAL_XKEYBOARDGROUP" );
    if ( pUseKeyboardExtension != NULL )
    {
        mbUseExtension = pUseKeyboardExtension[0] != '\0' ;
        if ( mbUseExtension )
            mnDefaultGroup = strtol( pUseKeyboardExtension, NULL, 0 );
        if ( mnDefaultGroup > XkbMaxKbdGroup )
            mnDefaultGroup = 0;
    }

    // query XServer support for XKB Extension,
    // do not call XQueryExtension() / XInitExtension() due to possible version
    // clashes !
    if ( mbUseExtension )
    {
        int nMajorExtOpcode;
        int nExtMajorVersion = XkbMajorVersion;
        int nExtMinorVersion = XkbMinorVersion;

        mbUseExtension = (sal_Bool)XkbQueryExtension( mpDisplay,
            &nMajorExtOpcode, (int*)&mnEventBase, (int*)&mnErrorBase,
            &nExtMajorVersion, &nExtMinorVersion );
    }

    // query notification for changes of the keyboard group
    if ( mbUseExtension )
    {
        #define XkbGroupMask (  XkbGroupStateMask | XkbGroupBaseMask \
                                | XkbGroupLatchMask | XkbGroupLockMask )

        mbUseExtension = XkbSelectEventDetails( mpDisplay,
            XkbUseCoreKbd, XkbStateNotify, XkbGroupMask, XkbGroupMask );
    }

    // query initial keyboard group
    if ( mbUseExtension )
    {
        XkbStateRec aStateRecord;
        XkbGetState( mpDisplay, XkbUseCoreKbd, &aStateRecord );
        mnGroup = aStateRecord.group;
    }
}

void
SalI18N_KeyboardExtension::Dispatch( XEvent* pEvent )
{
    // must the event be handled?
    if (   !mbUseExtension
        || (pEvent->type != mnEventBase) )
        return;

    // only handle state notify events for now, and only interested
    // in group details
    sal_uInt32 nXKBType = ((XkbAnyEvent*)pEvent)->xkb_type;
    switch ( nXKBType )
    {
        case XkbStateNotify:

            mnGroup = ((XkbStateNotifyEvent*)pEvent)->group;
            break;

        default:

            #if OSL_DEBUG_LEVEL > 1
            fprintf(stderr, "Got unrequested XkbAnyEvent %#x/%i\n",
                    static_cast<unsigned int>(nXKBType), static_cast<int>(nXKBType) );
            #endif
            break;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
