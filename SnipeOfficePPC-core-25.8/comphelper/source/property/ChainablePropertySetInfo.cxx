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

#include <comphelper/ChainablePropertySetInfo.hxx>
#include <comphelper/TypeGeneration.hxx>

using ::rtl::OUString;
using ::comphelper::PropertyInfo;
using ::comphelper::GenerateCppuType;
using ::comphelper::ChainablePropertySetInfo;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Type;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::XInterface;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::beans::Property;
using ::com::sun::star::beans::XPropertySetInfo;
using ::com::sun::star::beans::UnknownPropertyException;

ChainablePropertySetInfo::ChainablePropertySetInfo( PropertyInfo* pMap )
    throw()
{
    add ( pMap );
}

ChainablePropertySetInfo::~ChainablePropertySetInfo()
    throw()
{
}

void ChainablePropertySetInfo::add( PropertyInfo* pMap, sal_Int32 nCount )
    throw()
{
    // nCount < 0   => add all
    // nCount == 0  => add nothing
    // nCount > 0   => add at most nCount entries
    if( maProperties.getLength() )
        maProperties.realloc( 0 );

    while( pMap->mpName && ( ( nCount < 0) || ( nCount-- > 0 ) ) )
    {
        OUString aName( pMap->mpName, pMap->mnNameLen, RTL_TEXTENCODING_ASCII_US );

#ifdef DBG_UTIL
        PropertyInfoHash::iterator aIter = maMap.find( aName );
        if( aIter != maMap.end() )
            OSL_FAIL( "Warning: PropertyInfo added twice, possible error!");
#endif
        maMap[aName] = pMap++;
    }
}

void ChainablePropertySetInfo::remove( const rtl::OUString& aName )
    throw()
{
    maMap.erase ( aName );
    if ( maProperties.getLength() )
         maProperties.realloc( 0 );
}

Sequence< ::Property > SAL_CALL ChainablePropertySetInfo::getProperties()
    throw(::com::sun::star::uno::RuntimeException)
{
    sal_Int32 nSize = maMap.size();
    if( maProperties.getLength() != nSize )
    {
        maProperties.realloc ( nSize );
        Property* pProperties = maProperties.getArray();

        PropertyInfoHash::iterator aIter = maMap.begin();
        const PropertyInfoHash::iterator aEnd = maMap.end();
        for ( ; aIter != aEnd; ++aIter, ++pProperties)
        {
            PropertyInfo* pInfo = (*aIter).second;

            pProperties->Name = OUString( pInfo->mpName, pInfo->mnNameLen, RTL_TEXTENCODING_ASCII_US );
            pProperties->Handle = pInfo->mnHandle;
            const Type* pType;
            GenerateCppuType ( pInfo->meCppuType, pType);
            pProperties->Type = *pType;
            pProperties->Attributes = pInfo->mnAttributes;
        }
    }
    return maProperties;
}

Property SAL_CALL ChainablePropertySetInfo::getPropertyByName( const ::rtl::OUString& rName )
    throw(::UnknownPropertyException, ::com::sun::star::uno::RuntimeException)
{
    PropertyInfoHash::iterator aIter = maMap.find( rName );

    if ( maMap.end() == aIter )
        throw UnknownPropertyException( rName, *this );

    PropertyInfo *pInfo = (*aIter).second;
    Property aProperty;
    aProperty.Name   = OUString( pInfo->mpName, pInfo->mnNameLen, RTL_TEXTENCODING_ASCII_US );
    aProperty.Handle = pInfo->mnHandle;
    const Type* pType = &aProperty.Type;
    GenerateCppuType ( pInfo->meCppuType, pType );
    aProperty.Type = *pType;
    aProperty.Attributes = pInfo->mnAttributes;
    return aProperty;
}

sal_Bool SAL_CALL ChainablePropertySetInfo::hasPropertyByName( const ::rtl::OUString& rName )
    throw(::com::sun::star::uno::RuntimeException)
{
    return static_cast < sal_Bool > ( maMap.find ( rName ) != maMap.end() );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
