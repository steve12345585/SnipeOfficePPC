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
#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#define _CPPUHELPER_INTERFACECONTAINER_HXX_

#include <cppuhelper/interfacecontainer.h>


namespace cppu
{

template< class key , class hashImpl , class equalImpl >
inline OMultiTypeInterfaceContainerHelperVar< key , hashImpl , equalImpl >::OMultiTypeInterfaceContainerHelperVar( ::osl::Mutex & rMutex_ )
    SAL_THROW(())
    : rMutex( rMutex_ )
{
    m_pMap = new InterfaceMap;
}

//===================================================================
template< class key , class hashImpl , class equalImpl >
inline OMultiTypeInterfaceContainerHelperVar< key , hashImpl , equalImpl >::~OMultiTypeInterfaceContainerHelperVar()
    SAL_THROW(())
{
    typename InterfaceMap::iterator iter = m_pMap->begin();
    typename InterfaceMap::iterator end = m_pMap->end();

    while( iter != end )
    {
        delete (OInterfaceContainerHelper*)(*iter).second;
        (*iter).second = 0;
        ++iter;
    }
    delete m_pMap;
}

//===================================================================
template< class key , class hashImpl , class equalImpl >
inline ::com::sun::star::uno::Sequence< key > OMultiTypeInterfaceContainerHelperVar< key , hashImpl , equalImpl >::getContainedTypes() const
    SAL_THROW(())
{
    ::osl::MutexGuard aGuard( rMutex );
    typename InterfaceMap::size_type nSize = m_pMap->size();
    if( nSize != 0 )
    {
        ::com::sun::star::uno::Sequence< key > aInterfaceTypes( nSize );
        key * pArray = aInterfaceTypes.getArray();

        typename InterfaceMap::iterator iter = m_pMap->begin();
        typename InterfaceMap::iterator end = m_pMap->end();

        sal_uInt32 i = 0;
        while( iter != end )
        {
            // are interfaces added to this container?
            if( ((OInterfaceContainerHelper*)(*iter).second)->getLength() )
                // yes, put the type in the array
                pArray[i++] = (*iter).first;
            iter++;
        }
        if( i != nSize ) {
            // may be empty container, reduce the sequence to the right size
            aInterfaceTypes = ::com::sun::star::uno::Sequence<key>( pArray, i );
        }
        return aInterfaceTypes;
    }
    return ::com::sun::star::uno::Sequence<key>();
}

//===================================================================
template< class key , class hashImpl , class equalImpl >
OInterfaceContainerHelper * OMultiTypeInterfaceContainerHelperVar< key , hashImpl , equalImpl >::getContainer(
    const key & rKey ) const SAL_THROW(())
{
    ::osl::MutexGuard aGuard( rMutex );

     typename InterfaceMap::iterator iter = find( rKey );
    if( iter != m_pMap->end() )
            return (OInterfaceContainerHelper*) (*iter).second;
    return 0;
}

//===================================================================
template< class key , class hashImpl , class equalImpl >
sal_Int32 OMultiTypeInterfaceContainerHelperVar< key , hashImpl , equalImpl >::addInterface(
    const key & rKey,
    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > & rListener )
    SAL_THROW(())
{
    ::osl::MutexGuard aGuard( rMutex );
    typename InterfaceMap::iterator iter = find( rKey );
    if( iter == m_pMap->end() )
    {
        OInterfaceContainerHelper * pLC = new OInterfaceContainerHelper( rMutex );
        m_pMap->push_back(std::pair<key, void*>(rKey, pLC));
        return pLC->addInterface( rListener );
    }
    else
        return ((OInterfaceContainerHelper*)(*iter).second)->addInterface( rListener );
}

//===================================================================
template< class key , class hashImpl , class equalImpl >
inline sal_Int32 OMultiTypeInterfaceContainerHelperVar< key , hashImpl , equalImpl >::removeInterface(
    const key & rKey,
    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > & rListener )
    SAL_THROW(())
{
    ::osl::MutexGuard aGuard( rMutex );

    // search container with id nUik
    typename InterfaceMap::iterator iter = find( rKey );
    // container found?
    if( iter != m_pMap->end() )
        return ((OInterfaceContainerHelper*)(*iter).second)->removeInterface( rListener );

    // no container with this id. Always return 0
    return 0;
}

//===================================================================
template< class key , class hashImpl , class equalImpl >
void OMultiTypeInterfaceContainerHelperVar< key , hashImpl , equalImpl >::disposeAndClear(
    const ::com::sun::star::lang::EventObject & rEvt )
    SAL_THROW(())
{
    typename InterfaceMap::size_type nSize = 0;
    OInterfaceContainerHelper ** ppListenerContainers = NULL;
    {
        ::osl::MutexGuard aGuard( rMutex );
        nSize = m_pMap->size();
        if( nSize )
        {
            typedef OInterfaceContainerHelper* ppp;
            ppListenerContainers = new ppp[nSize];

            typename InterfaceMap::iterator iter = m_pMap->begin();
            typename InterfaceMap::iterator end = m_pMap->end();

            typename InterfaceMap::size_type i = 0;
            while( iter != end )
            {
                ppListenerContainers[i++] = (OInterfaceContainerHelper*)(*iter).second;
                ++iter;
            }
        }
    }

    // create a copy, because do not fire event in a guarded section
    for( typename InterfaceMap::size_type i = 0; i < nSize; i++ )
    {
        if( ppListenerContainers[i] )
            ppListenerContainers[i]->disposeAndClear( rEvt );
    }

    delete [] ppListenerContainers;
}

//===================================================================
template< class key , class hashImpl , class equalImpl >
void OMultiTypeInterfaceContainerHelperVar< key , hashImpl , equalImpl >::clear() SAL_THROW(())
{
    ::osl::MutexGuard aGuard( rMutex );
    typename InterfaceMap::iterator iter = m_pMap->begin();
    typename InterfaceMap::iterator end = m_pMap->end();

    while( iter != end )
    {
        ((OInterfaceContainerHelper*)(*iter).second)->clear();
        ++iter;
    }
}


}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
