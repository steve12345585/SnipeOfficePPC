/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include "comphelper/propertysetinfo.hxx"

using namespace ::rtl;
using namespace ::comphelper;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;

namespace comphelper
{
class PropertyMapImpl
{
public:
    PropertyMapImpl() throw();
    virtual ~PropertyMapImpl() throw();

    void add( PropertyMapEntry* pMap, sal_Int32 nCount = -1 ) throw();
    void remove( const OUString& aName ) throw();

    Sequence< Property > getProperties() throw();

    const PropertyMap* getPropertyMap() const throw();

    Property getPropertyByName( const OUString& aName ) throw( UnknownPropertyException );
    sal_Bool hasPropertyByName( const OUString& aName ) throw();

private:
    PropertyMap maPropertyMap;
    Sequence< Property > maProperties;
};
}

PropertyMapImpl::PropertyMapImpl() throw()
{
}

PropertyMapImpl::~PropertyMapImpl() throw()
{
}

void PropertyMapImpl::add( PropertyMapEntry* pMap, sal_Int32 nCount ) throw()
{
    // nCount < 0   => add all
    // nCount == 0  => add nothing
    // nCount > 0   => add at most nCount entries

    while( pMap->mpName && ( ( nCount < 0) || ( nCount-- > 0 ) ) )
    {
        OUString aName( pMap->mpName, pMap->mnNameLen, RTL_TEXTENCODING_ASCII_US );

#ifdef DBG_UTIL
        PropertyMap::iterator aIter = maPropertyMap.find( aName );
        if( aIter != maPropertyMap.end() )
        {
            OSL_FAIL( "Warning: PropertyMapEntry added twice, possible error!");
        }
#endif
        if( NULL == pMap->mpType )
        {
            OSL_FAIL( "No type in PropertyMapEntry!");
            pMap->mpType = &::getCppuType((const sal_Int32*)0);
        }

        maPropertyMap[aName] = pMap;

        if( maProperties.getLength() )
            maProperties.realloc( 0 );

        pMap = &pMap[1];
    }
}

void PropertyMapImpl::remove( const OUString& aName ) throw()
{
    maPropertyMap.erase( aName );

    if( maProperties.getLength() )
        maProperties.realloc( 0 );
}

Sequence< Property > PropertyMapImpl::getProperties() throw()
{
    // maybe we have to generate the properties after
    // a change in the property map or at first call
    // to getProperties
    if( maProperties.getLength() != (sal_Int32)maPropertyMap.size() )
    {
        maProperties = Sequence< Property >( maPropertyMap.size() );
        Property* pProperties = maProperties.getArray();

        PropertyMap::iterator aIter = maPropertyMap.begin();
        const PropertyMap::iterator aEnd = maPropertyMap.end();
        while( aIter != aEnd )
        {
            PropertyMapEntry* pEntry = (*aIter).second;

            pProperties->Name = OUString( pEntry->mpName, pEntry->mnNameLen, RTL_TEXTENCODING_ASCII_US );
            pProperties->Handle = pEntry->mnHandle;
            pProperties->Type = *pEntry->mpType;
            pProperties->Attributes = pEntry->mnAttributes;

            ++pProperties;
            ++aIter;
        }
    }

    return maProperties;
}

const PropertyMap* PropertyMapImpl::getPropertyMap() const throw()
{
    return &maPropertyMap;
}

Property PropertyMapImpl::getPropertyByName( const OUString& aName ) throw( UnknownPropertyException )
{
    PropertyMap::iterator aIter = maPropertyMap.find( aName );

    if( maPropertyMap.end() == aIter )
        throw UnknownPropertyException( aName, NULL );

    PropertyMapEntry* pEntry = (*aIter).second;

    return Property( aName, pEntry->mnHandle, *pEntry->mpType, pEntry->mnAttributes );
}

sal_Bool PropertyMapImpl::hasPropertyByName( const OUString& aName ) throw()
{
    return maPropertyMap.find( aName ) != maPropertyMap.end();
}

///////////////////////////////////////////////////////////////////////

PropertySetInfo::PropertySetInfo() throw()
{
    mpMap = new PropertyMapImpl();
}

PropertySetInfo::PropertySetInfo( PropertyMapEntry* pMap ) throw()
{
    mpMap = new PropertyMapImpl();
    mpMap->add( pMap );
}

PropertySetInfo::~PropertySetInfo() throw()
{
    delete mpMap;
}

void PropertySetInfo::add( PropertyMapEntry* pMap ) throw()
{
    mpMap->add( pMap );
}

void PropertySetInfo::remove( const rtl::OUString& aName ) throw()
{
    mpMap->remove( aName );
}

Sequence< ::com::sun::star::beans::Property > SAL_CALL PropertySetInfo::getProperties() throw(::com::sun::star::uno::RuntimeException)
{
    return mpMap->getProperties();
}

Property SAL_CALL PropertySetInfo::getPropertyByName( const ::rtl::OUString& aName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException)
{
    return mpMap->getPropertyByName( aName );
}

sal_Bool SAL_CALL PropertySetInfo::hasPropertyByName( const ::rtl::OUString& Name ) throw(::com::sun::star::uno::RuntimeException)
{
    return mpMap->hasPropertyByName( Name );
}

const PropertyMap* PropertySetInfo::getPropertyMap() const throw()
{
    return mpMap->getPropertyMap();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
