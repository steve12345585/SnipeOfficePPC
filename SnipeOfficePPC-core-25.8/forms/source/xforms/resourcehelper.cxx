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


#include "resourcehelper.hxx"
#include "frm_resource.hxx"

#include <rtl/ustring.hxx>
#include <tools/string.hxx>

using rtl::OUString;

#define OUSTRING(x) rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(x))

namespace xforms
{

OUString getResource( sal_uInt16 nResourceId )
{
    return getResource( nResourceId, OUString(), OUString(), OUString() );
}

OUString getResource( sal_uInt16 nResourceId,
                      const OUString& rInfo1 )
{
    return getResource( nResourceId, rInfo1, OUString(), OUString() );
}

OUString getResource( sal_uInt16 nResourceId,
                      const OUString& rInfo1,
                      const OUString& rInfo2 )
{
    return getResource( nResourceId, rInfo1, rInfo2, OUString() );
}

OUString getResource( sal_uInt16 nResourceId,
                      const OUString& rInfo1,
                      const OUString& rInfo2,
                      const OUString& rInfo3 )
{
    OUString sResource = frm::ResourceManager::loadString( nResourceId );
    OSL_ENSURE( !sResource.isEmpty(), "resource not found?" );

    // use old style String class for search and replace, so we don't have to
    // code this again.
    String sString( sResource );
    sString.SearchAndReplaceAll( String(OUSTRING("$1")), String(rInfo1) );
    sString.SearchAndReplaceAll( String(OUSTRING("$2")), String(rInfo2) );
    sString.SearchAndReplaceAll( String(OUSTRING("$3")), String(rInfo3) );
    return OUString( sString );
}

}   // namespace xforms

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
