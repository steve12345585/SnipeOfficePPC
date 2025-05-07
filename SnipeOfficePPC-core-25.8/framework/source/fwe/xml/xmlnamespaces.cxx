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


#include <xml/xmlnamespaces.hxx>

using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::uno;

namespace framework
{

XMLNamespaces::XMLNamespaces()
    : m_aXMLAttributeNamespace( RTL_CONSTASCII_USTRINGPARAM( "xmlns" ))
{
}

XMLNamespaces::XMLNamespaces( const XMLNamespaces& aXMLNamespaces )
{
    m_aDefaultNamespace = aXMLNamespaces.m_aDefaultNamespace;
    m_aNamespaceMap = aXMLNamespaces.m_aNamespaceMap;
}

XMLNamespaces::~XMLNamespaces()
{
}

void XMLNamespaces::addNamespace( const ::rtl::OUString& aName, const ::rtl::OUString& aValue ) throw( SAXException )
{
    NamespaceMap::iterator p;
    ::rtl::OUString aNamespaceName( aName );
    sal_Int32 nXMLNamespaceLength = m_aXMLAttributeNamespace.getLength();

    // delete preceding "xmlns"
    if ( aNamespaceName.compareTo( m_aXMLAttributeNamespace, nXMLNamespaceLength ) == 0 )
    {
        if ( aNamespaceName.getLength() == nXMLNamespaceLength )
        {
            aNamespaceName = ::rtl::OUString();
        }
        else if ( aNamespaceName.getLength() >= nXMLNamespaceLength+2 )
        {
            aNamespaceName = aNamespaceName.copy( nXMLNamespaceLength+1 );
        }
        else
        {
            // a xml namespace without name is not allowed (e.g. "xmlns:" )
            ::rtl::OUString aErrorMessage( RTL_CONSTASCII_USTRINGPARAM( "A xml namespace without name is not allowed!" ));
            throw SAXException( aErrorMessage, Reference< XInterface >(), Any() );
        }
    }

    if ( aValue.isEmpty() && !aNamespaceName.isEmpty() )
    {
        // namespace should be reseted - as xml draft states this is only allowed
        // for the default namespace - check and throw exception if check fails
        ::rtl::OUString aErrorMessage( RTL_CONSTASCII_USTRINGPARAM( "Clearing xml namespace only allowed for default namespace!" ));
        throw SAXException( aErrorMessage, Reference< XInterface >(), Any() );
    }
    else
    {
        if ( aNamespaceName.isEmpty() )
            m_aDefaultNamespace = aValue;
        else
        {
            p = m_aNamespaceMap.find( aNamespaceName );
            if ( p != m_aNamespaceMap.end() )
            {
                // replace current namespace definition
                m_aNamespaceMap.erase( p );
                m_aNamespaceMap.insert( NamespaceMap::value_type( aNamespaceName, aValue ));
            }
            else
            {
                m_aNamespaceMap.insert( NamespaceMap::value_type( aNamespaceName, aValue ));
            }
        }
    }
}

::rtl::OUString XMLNamespaces::applyNSToAttributeName( const ::rtl::OUString& aName ) const throw( SAXException )
{
    // xml draft: there is no default namespace for attributes!

    int index;
    if (( index = aName.indexOf( ':' )) > 0 )
    {
        if ( aName.getLength() > index+1 )
        {
            ::rtl::OUString aAttributeName = getNamespaceValue( aName.copy( 0, index ) );
            aAttributeName += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("^"));
            aAttributeName += aName.copy( index+1 );
            return aAttributeName;
        }
        else
        {
            // attribute with namespace but without name "namespace:" is not allowed!!
            ::rtl::OUString aErrorMessage( RTL_CONSTASCII_USTRINGPARAM( "Attribute has no name only preceding namespace!" ));
            throw SAXException( aErrorMessage, Reference< XInterface >(), Any() );
        }
    }

    return aName;
}

::rtl::OUString XMLNamespaces::applyNSToElementName( const ::rtl::OUString& aName ) const   throw( SAXException )
{
    // xml draft: element names can have a default namespace

    int         index = aName.indexOf( ':' );
    ::rtl::OUString aNamespace;
    ::rtl::OUString aElementName = aName;

    if ( index > 0 )
        aNamespace = getNamespaceValue( aName.copy( 0, index ) );
    else
        aNamespace = m_aDefaultNamespace;

    if ( !aNamespace.isEmpty() )
    {
        aElementName = aNamespace;
        aElementName += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("^"));
    }
    else
        return aName;

    if ( index > 0 )
    {
        if ( aName.getLength() > index+1 )
            aElementName += aName.copy( index+1 );
        else
        {
            // attribute with namespace but without a name is not allowed (e.g. "cfg:" )
            ::rtl::OUString aErrorMessage( RTL_CONSTASCII_USTRINGPARAM( "Attribute has no name only preceding namespace!" ));
            throw SAXException( aErrorMessage, Reference< XInterface >(), Any() );
        }
    }
    else
        aElementName += aName;

    return aElementName;
}

::rtl::OUString XMLNamespaces::getNamespaceValue( const ::rtl::OUString& aNamespace ) const throw( SAXException )
{
    if ( aNamespace.isEmpty() )
        return m_aDefaultNamespace;
    else
    {
        NamespaceMap::const_iterator p;
        p = m_aNamespaceMap.find( aNamespace );
        if ( p != m_aNamespaceMap.end() )
            return p->second;
        else
        {
            // namespace not defined => throw exception!
            ::rtl::OUString aErrorMessage( RTL_CONSTASCII_USTRINGPARAM( "XML namespace used but not defined!" ));
            throw SAXException( aErrorMessage, Reference< XInterface >(), Any() );
        }
    }
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
