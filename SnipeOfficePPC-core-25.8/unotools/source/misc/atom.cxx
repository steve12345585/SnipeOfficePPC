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


#include <unotools/atom.hxx>

using namespace utl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;

AtomProvider::AtomProvider()
{
    m_nAtoms = 1;
}

AtomProvider::~AtomProvider()
{
}

int AtomProvider::getAtom( const ::rtl::OUString& rString, sal_Bool bCreate )
{
    ::boost::unordered_map< ::rtl::OUString, int, ::rtl::OUStringHash >::iterator it = m_aAtomMap.find( rString );
    if( it != m_aAtomMap.end() )
        return it->second;
    if( ! bCreate )
        return INVALID_ATOM;
    m_aAtomMap[ rString ] = m_nAtoms;
    m_aStringMap[ m_nAtoms ] = rString;
    m_nAtoms++;
    return m_nAtoms-1;
}

const ::rtl::OUString& AtomProvider::getString( int nAtom ) const
{
    static ::rtl::OUString aEmpty;
    ::boost::unordered_map< int, ::rtl::OUString, ::boost::hash< int > >::const_iterator it = m_aStringMap.find( nAtom );

    return it == m_aStringMap.end() ? aEmpty : it->second;
}

// -----------------------------------------------------------------------

MultiAtomProvider::MultiAtomProvider()
{
}

MultiAtomProvider::~MultiAtomProvider()
{
    for( ::boost::unordered_map< int, AtomProvider*, ::boost::hash< int > >::iterator it = m_aAtomLists.begin(); it != m_aAtomLists.end(); ++it )
        delete it->second;
}

int MultiAtomProvider::getAtom( int atomClass, const ::rtl::OUString& rString, sal_Bool bCreate )
{
    ::boost::unordered_map< int, AtomProvider*, ::boost::hash< int > >::iterator it =
          m_aAtomLists.find( atomClass );
    if( it != m_aAtomLists.end() )
        return it->second->getAtom( rString, bCreate );

    if( bCreate )
    {
        AtomProvider* pNewClass;
        m_aAtomLists[ atomClass ] = pNewClass = new AtomProvider();
        return pNewClass->getAtom( rString, bCreate );
    }
    return INVALID_ATOM;
}

const ::rtl::OUString& MultiAtomProvider::getString( int atomClass, int atom ) const
{
    ::boost::unordered_map< int, AtomProvider*, ::boost::hash< int > >::const_iterator it =
          m_aAtomLists.find( atomClass );
    if( it != m_aAtomLists.end() )
        return it->second->getString( atom );

    static ::rtl::OUString aEmpty;
    return aEmpty;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
