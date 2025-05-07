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


#include "classes/fwkresid.hxx"
#include <osl/mutex.hxx>
#include <vcl/svapp.hxx>

#include <rtl/strbuf.hxx>

namespace framework
{

ResMgr* FwkResId::GetResManager()
{
    static ResMgr*  pResMgr = NULL;

    if ( !pResMgr )
    {
        SolarMutexGuard aSolarGuard;
        pResMgr = ResMgr::CreateResMgr("fwe", Application::GetSettings().GetUILocale());
    }

    return pResMgr;
}

// -----------------------------------------------------------------------

FwkResId::FwkResId( sal_uInt16 nId ) :
    ResId( nId, *FwkResId::GetResManager() )
{
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
