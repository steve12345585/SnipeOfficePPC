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

// INCLUDE ---------------------------------------------------------------

#include <sfx2/app.hxx>
#include "sfx2/viewfac.hxx"
#include <rtl/ustrbuf.hxx>

// STATIC DATA -----------------------------------------------------------

DBG_NAME(SfxViewFactory)

SfxViewShell *SfxViewFactory::CreateInstance(SfxViewFrame *pFrame, SfxViewShell *pOldSh )
{
    DBG_CHKTHIS(SfxViewFactory, 0);
    return (*fnCreate)(pFrame, pOldSh);
}

String SfxViewFactory::GetLegacyViewName() const
{
    ::rtl::OUStringBuffer aViewName;
    aViewName.appendAscii(RTL_CONSTASCII_STRINGPARAM("view"));
    aViewName.append( sal_Int32( GetOrdinal() ) );
    return aViewName.makeStringAndClear();
}

String SfxViewFactory::GetAPIViewName() const
{
    if ( m_sViewName.Len() > 0 )
        return m_sViewName;

    if ( GetOrdinal() == 0 )
        return String::CreateFromAscii( "Default" );

    return GetLegacyViewName();
}

// CTOR / DTOR -----------------------------------------------------------

SfxViewFactory::SfxViewFactory( SfxViewCtor fnC, SfxViewInit fnI,
                                sal_uInt16 nOrdinal, const sal_Char* asciiViewName ):
    fnCreate(fnC),
    fnInit(fnI),
    nOrd(nOrdinal),
    m_sViewName( String::CreateFromAscii( asciiViewName ) )
{
    DBG_CTOR(SfxViewFactory, 0);
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
