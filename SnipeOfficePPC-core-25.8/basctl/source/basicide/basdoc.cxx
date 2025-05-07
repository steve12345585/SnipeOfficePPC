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

#include <sfx2/app.hxx>
#include <sfx2/docfac.hxx>
#include <sfx2/printer.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/objsh.hxx>
#include <svl/itemset.hxx>

#include "unomodel.hxx"

#include <basdoc.hxx>
#define BasicDocShell     // This CANNOT come before basdoc apparently
#include <basslots.hxx>
#include <sfx2/sfxmodelfactory.hxx>
#include <svx/svxids.hrc>

TYPEINIT1(BasicDocShell, SfxObjectShell);
DBG_NAME(BasicDocShell);

SFX_IMPL_OBJECTFACTORY( BasicDocShell, SvGlobalName(), SFXOBJECTSHELL_STD_NORMAL, "sbasic" )

SFX_IMPL_INTERFACE( BasicDocShell, SfxObjectShell, IDEResId( 0 ) )
{
    SFX_STATUSBAR_REGISTRATION( IDEResId( SID_BASICIDE_STATUSBAR ) );
}

BasicDocShell::BasicDocShell()
    :SfxObjectShell( SFXMODEL_DISABLE_EMBEDDED_SCRIPTS | SFXMODEL_DISABLE_DOCUMENT_RECOVERY )
{
    pPrinter = 0;
    SetPool( &SFX_APP()->GetPool() );
    SetBaseModel( new SIDEModel(this) );
}

BasicDocShell::~BasicDocShell()
{
    delete pPrinter;
}

SfxPrinter* BasicDocShell::GetPrinter( sal_Bool bCreate )
{
    if ( !pPrinter && bCreate )
        pPrinter = new SfxPrinter( new SfxItemSet( GetPool(), SID_PRINTER_NOTFOUND_WARN , SID_PRINTER_NOTFOUND_WARN ) );

    return pPrinter;
}

void BasicDocShell::SetPrinter( SfxPrinter* pPr )
{
    if ( pPr != pPrinter )
    {
        delete pPrinter;
        pPrinter = pPr;
    }
}

void BasicDocShell::FillClass( SvGlobalName*, sal_uInt32*, String*, String*, String*, sal_Int32, sal_Bool bTemplate) const
{
    (void)bTemplate;
    DBG_ASSERT( bTemplate == sal_False, "No template for Basic" );
}

void BasicDocShell::Draw( OutputDevice *, const JobSetup &, sal_uInt16 )
{}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
