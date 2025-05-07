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

#include <uiconfiguration/graphicnameaccess.hxx>

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#include <comphelper/sequence.hxx>

using namespace ::com::sun::star;

namespace framework
{

GraphicNameAccess::GraphicNameAccess()
{
}

GraphicNameAccess::~GraphicNameAccess()
{
}

void GraphicNameAccess::addElement( const rtl::OUString& rName, const uno::Reference< graphic::XGraphic >& rElement )
{
    m_aNameToElementMap.insert( NameGraphicHashMap::value_type( rName, rElement ));
}

// XNameAccess
uno::Any SAL_CALL GraphicNameAccess::getByName( const ::rtl::OUString& aName )
throw( container::NoSuchElementException,
       lang::WrappedTargetException,
       uno::RuntimeException)
{
    NameGraphicHashMap::const_iterator pIter = m_aNameToElementMap.find( aName );
    if ( pIter != m_aNameToElementMap.end() )
        return uno::makeAny( pIter->second );
    else
        throw container::NoSuchElementException();
}

uno::Sequence< ::rtl::OUString > SAL_CALL GraphicNameAccess::getElementNames()
throw(::com::sun::star::uno::RuntimeException)
{
    if ( m_aSeq.getLength() == 0 )
    {
        uno::Sequence< rtl::OUString > aSeq( m_aNameToElementMap.size() );
        NameGraphicHashMap::const_iterator pIter = m_aNameToElementMap.begin();
        sal_Int32 i( 0);
        while ( pIter != m_aNameToElementMap.end())
        {
            aSeq[i++] = pIter->first;
            ++pIter;
        }
        m_aSeq = aSeq;
    }

    return m_aSeq;
}

sal_Bool SAL_CALL GraphicNameAccess::hasByName( const ::rtl::OUString& aName )
throw(::com::sun::star::uno::RuntimeException)
{
    NameGraphicHashMap::const_iterator pIter = m_aNameToElementMap.find( aName );
    return ( pIter != m_aNameToElementMap.end() );
}

// XElementAccess
sal_Bool SAL_CALL GraphicNameAccess::hasElements()
throw( uno::RuntimeException )
{
    return ( !m_aNameToElementMap.empty() );
}

uno::Type SAL_CALL GraphicNameAccess::getElementType()
throw( uno::RuntimeException )
{
    return ::getCppuType( (const uno::Reference< graphic::XGraphic > *)NULL );
}

} // namespace framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
