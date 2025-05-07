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


#include <rtl/ustrbuf.hxx>
#include <tools/inetdef.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/bootstrap.hxx>
#include <unotools/docinfohelper.hxx>
#include <rtl/bootstrap.hxx>

using namespace ::com::sun::star;

namespace utl
{

::rtl::OUString DocInfoHelper::GetGeneratorString()
{
    rtl::OUStringBuffer aResult;

    // First product: branded name + version
    // version is <product_versions>_<product_extension>$<platform>

    // plain product name
    rtl::OUString aValue( utl::ConfigManager::getProductName() );
    if ( !aValue.isEmpty() )
    {
        aResult.append( aValue.replace( ' ', '_' ) );
        aResult.append( (sal_Unicode)'/' );

        aValue = utl::ConfigManager::getProductVersion();
        if ( !aValue.isEmpty() )
        {
            aResult.append( aValue.replace( ' ', '_' ) );

            aValue = utl::ConfigManager::getProductExtension();
            if ( !aValue.isEmpty() )
            {
                aResult.append( (sal_Unicode)'_' );
                aResult.append( aValue.replace( ' ', '_' ) );
            }
        }

        ::rtl::OUString os( "$_OS" );
        ::rtl::OUString arch( "$_ARCH" );
        ::rtl::Bootstrap::expandMacros(os);
        ::rtl::Bootstrap::expandMacros(arch);
        aResult.append( (sal_Unicode)'$' );
        aResult.append( os );
        aResult.append( (sal_Unicode)'_' );
        aResult.append( arch );
        aResult.append( (sal_Unicode)' ' );
    }

    // second product: LibreOffice_project/<build_information>
    // build_information has '(' and '[' encoded as '$', ')' and ']' ignored
    // and ':' replaced by '-'
    {
        aResult.appendAscii( "LibreOffice_project/" );
        ::rtl::OUString aDefault;
        ::rtl::OUString aBuildId( Bootstrap::getBuildIdData( aDefault ) );
        for( sal_Int32 i=0; i < aBuildId.getLength(); i++ )
        {
            sal_Unicode c = aBuildId[i];
            switch( c )
            {
            case '(':
            case '[':
                aResult.append( (sal_Unicode)'$' );
                break;
            case ')':
            case ']':
                break;
            case ':':
                aResult.append( (sal_Unicode)'-' );
                break;
            default:
                aResult.append( c );
                break;
            }
        }
    }

    return aResult.makeStringAndClear();
}

} // end of namespace utl

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
