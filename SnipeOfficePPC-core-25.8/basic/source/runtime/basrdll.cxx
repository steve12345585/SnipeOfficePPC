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

#include <tools/shl.hxx>
#include <vcl/svapp.hxx>
#include <svl/solar.hrc>
#include <tools/debug.hxx>
#include <vcl/msgbox.hxx>

#include <basic/sbstar.hxx>
#include <basic/basrdll.hxx>
#include <basrid.hxx>
#include <sb.hrc>

BasResId::BasResId( sal_uInt32 nId ) :
    ResId( nId, *(BASIC_DLL()->GetBasResMgr()) )
{
}

BasicDLL::BasicDLL()
{
    BASIC_DLL() = this;
    ::com::sun::star::lang::Locale aLocale = Application::GetSettings().GetUILocale();
    pBasResMgr = ResMgr::CreateResMgr("sb", aLocale );
    bDebugMode = sal_False;
    bBreakEnabled = sal_True;
}

BasicDLL::~BasicDLL()
{
    delete pBasResMgr;
}

void BasicDLL::EnableBreak( sal_Bool bEnable )
{
    BasicDLL* pThis = BASIC_DLL();
    DBG_ASSERT( pThis, "BasicDLL::EnableBreak: Noch keine Instanz!" );
    if ( pThis )
        pThis->bBreakEnabled = bEnable;
}

void BasicDLL::SetDebugMode( sal_Bool bDebugMode )
{
    BasicDLL* pThis = BASIC_DLL();
    DBG_ASSERT( pThis, "BasicDLL::EnableBreak: Noch keine Instanz!" );
    if ( pThis )
        pThis->bDebugMode = bDebugMode;
}


void BasicDLL::BasicBreak()
{
    // bJustStopping: if there's someone pressing STOP like crazy umpteen times,
    // but the Basic doesn't stop early enough, the box might appear more often...
    static sal_Bool bJustStopping = sal_False;

    BasicDLL* pThis = BASIC_DLL();
    DBG_ASSERT( pThis, "BasicDLL::EnableBreak: Noch keine Instanz!" );
    if ( pThis )
    {
        if ( StarBASIC::IsRunning() && !bJustStopping && ( pThis->bBreakEnabled || pThis->bDebugMode ) )
        {
            bJustStopping = sal_True;
            StarBASIC::Stop();
            String aMessageStr( BasResId( IDS_SBERR_TERMINATED ) );
            InfoBox( 0, aMessageStr ).Execute();
            bJustStopping = sal_False;
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
