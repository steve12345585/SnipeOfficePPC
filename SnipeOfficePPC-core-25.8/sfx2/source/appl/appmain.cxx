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


#define _SDINTERN_HXX

#include <stdio.h>
#include <svtools/ehdl.hxx>
#include <unotools/startoptions.hxx>
#include <svl/itempool.hxx>
#include <svl/urihelper.hxx>
#include <svtools/helpopt.hxx>
#include <framework/sfxhelperfunctions.hxx>
#include <rtl/ustring.hxx>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/Reference.hxx>

#include "sfxtypes.hxx"
#include "appdata.hxx"
#include <sfx2/docfac.hxx>
#include <sfx2/app.hxx>
#include "arrdecl.hxx"
#include <sfx2/dispatch.hxx>
#include "sfx2/sfxresid.hxx"
#include <sfx2/fcontnr.hxx>
#include <sfx2/viewsh.hxx>
#include <sfx2/msgpool.hxx>
#include <sfx2/mnumgr.hxx>
#include <sfx2/appuno.hxx>
#include "app.hrc"
#include <sfx2/docfile.hxx>
#include "workwin.hxx"

DBG_NAME(SfxAppMainNewMenu)
DBG_NAME(SfxAppMainBmkMenu)
DBG_NAME(SfxAppMainWizMenu)
DBG_NAME(SfxAppMainOLEReg)
DBG_NAME(SfxAppMainCHAOSReg)

//===================================================================

TYPEINIT2(SfxApplication,SfxShell,SfxBroadcaster);

//--------------------------------------------------------------------
void SfxApplication::Init
(
)

/*  [Description]

    This virtual method is called from SFx through Application::Main(),
    before Execute() is called and:
    - the Intro is already displayed,
    - the Applications window exists, but it is still hidden,
    - the Bindings already exist (Controller can be registered),
    - the Init and Config-Manager already exists,
    - the Standard-Controller already exists,
    - the SFx-Shells have alredy registered their Interfaces.

    [Cross-reference]

    <SfxApplication::Exit()>
    <SfxApplication::OpenClients()>
*/
{
}

//--------------------------------------------------------------------

void SfxApplication::Exit()

/*  [Description]

    This virtual method is called from SFx through Application::Main(),
    after Execute() has finished and
    - the configuration (SfxConfigManager) was already saved,
    - the window postions etc. in the SfxIniManager were written,
    - the Application widow still exists, but is hidden
    - all Documents and their Views already are closed.
    - Dispatcher, Bindings etc. already destroyed.

    [Cross-reference]
    <SfxApplication::Init(int,char*[])>
*/

{
}

//---------------------------------------------------------------------------

void SfxApplication::PreInit( )
{
}

//-------------------------------------------------------------------------

SfxFilterMatcher& SfxApplication::GetFilterMatcher()
{
    if( !pAppData_Impl->pMatcher )
    {
        pAppData_Impl->pMatcher = new SfxFilterMatcher();
        URIHelper::SetMaybeFileHdl( STATIC_LINK(
            pAppData_Impl->pMatcher, SfxFilterMatcher, MaybeFileHdl_Impl ) );
    }
    return *pAppData_Impl->pMatcher;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
