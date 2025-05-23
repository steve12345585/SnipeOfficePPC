/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
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


#include "MultiPropertySetHelper.hxx"
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <comphelper/stl_types.hxx>

// STL includes
#include <algorithm>


using ::com::sun::star::beans::XMultiPropertySet;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::beans::XPropertySetInfo;
using ::com::sun::star::lang::XServiceInfo;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::UNO_QUERY;
using ::comphelper::UStringLess;
using ::rtl::OUString;
using ::std::sort;


MultiPropertySetHelper::MultiPropertySetHelper(
    const sal_Char** pNames ) :
        pPropertyNames( NULL ),
        nLength( 0 ),
        aPropertySequence(),
        pSequenceIndex( NULL ),
        aValues(),
        pValues( NULL )
{
    // first count the elements
    for( const sal_Char** pPtr = pNames; *pPtr != NULL; pPtr++ )
        nLength++;

    // allocate array and create strings
    pPropertyNames = new OUString[nLength];
    for( sal_Int16 i = 0; i < nLength; i++ )
        pPropertyNames[i] = OUString::createFromAscii( pNames[i] );
}


MultiPropertySetHelper::~MultiPropertySetHelper()
{
    pValues = NULL; // memory 'owned' by aValues

    delete[] pSequenceIndex;
    delete[] pPropertyNames;
}


void MultiPropertySetHelper::hasProperties(
    const Reference<XPropertySetInfo> & rInfo )
{
    DBG_ASSERT( rInfo.is(), "I'd really like an XPropertySetInfo here." );

    // allocate sequence index
    if ( NULL == pSequenceIndex )
        pSequenceIndex = new sal_Int16[nLength] ;

    // construct pSequenceIndex
    sal_Int16 nNumberOfProperties = 0;
    sal_Int16 i;

    for( i = 0; i < nLength; i++ )
    {
        // ask for property
        sal_Bool bHasProperty =
            rInfo->hasPropertyByName( pPropertyNames[i] );

        // set index and increment (if appropriate)
        pSequenceIndex[i]= bHasProperty ? nNumberOfProperties : -1;
        if ( bHasProperty )
            nNumberOfProperties++;
    }

    // construct property sequence from index array
    if ( aPropertySequence.getLength() != nNumberOfProperties )
        aPropertySequence.realloc( nNumberOfProperties );
    OUString* pPropertySequence = aPropertySequence.getArray();
    for( i = 0; i < nLength; i ++ )
    {
        sal_Int16 nIndex = pSequenceIndex[i];
        if ( nIndex != -1 )
            pPropertySequence[nIndex] = pPropertyNames[i];
    }
}

sal_Bool MultiPropertySetHelper::checkedProperties()
{
    return (NULL != pSequenceIndex);
}



void MultiPropertySetHelper::getValues(
    const Reference<XMultiPropertySet> & rMultiPropertySet )
{
    DBG_ASSERT( rMultiPropertySet.is(), "We need an XMultiPropertySet." );

    aValues = rMultiPropertySet->getPropertyValues( aPropertySequence );
    pValues = aValues.getConstArray();
}

void MultiPropertySetHelper::getValues(
    const Reference<XPropertySet> & rPropertySet )
{
    DBG_ASSERT( rPropertySet.is(), "We need an XPropertySet." );

    // re-alloc aValues (if necessary) and fill with values from XPropertySet
    sal_Int16 nSupportedPropertiesCount =
        (sal_Int16)aPropertySequence.getLength();
    if ( aValues.getLength() != nSupportedPropertiesCount )
        aValues.realloc( nSupportedPropertiesCount );
    Any* pMutableArray = aValues.getArray();
    for( sal_Int16 i = 0; i < nSupportedPropertiesCount; i++ )
    {
        pMutableArray[i] = rPropertySet->getPropertyValue(
            pPropertyNames[ pSequenceIndex[ i ] ] );
    }

    // re-establish pValues pointer
    pValues = aValues.getConstArray();
}


const Any& MultiPropertySetHelper::getValue( sal_Int16 nIndex,
                     const Reference< XPropertySet> & rPropSet,
                     sal_Bool bTryMulti )
{
    if( !pValues )
    {
        if( bTryMulti )
        {
            Reference < XMultiPropertySet > xMultiPropSet( rPropSet,
                                                           UNO_QUERY );
            if( xMultiPropSet.is() )
                getValues( xMultiPropSet );
            else
                getValues( rPropSet );
        }
        else
        {
            getValues( rPropSet );
        }
    }

    return getValue( nIndex );
}

const Any& MultiPropertySetHelper::getValue( sal_Int16 nIndex,
                     const Reference< XMultiPropertySet> & rMultiPropSet )
{
    if( !pValues )
        getValues( rMultiPropSet );

    return getValue( nIndex );
}

// inline methods defined in header:
// inline Any& MultiPropertySetHelper::getValue( sal_Int16 nIndex )
// inline sal_Bool MultiPropertySetHelper::hasProperty( sal_Int16 nValueNo )

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
