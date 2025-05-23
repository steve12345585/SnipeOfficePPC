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


/** Attention: stl headers must(!) be included at first. Otherwhise it can make trouble
               with solaris headers ...
*/
#include <vector>

#include <stdio.h>

#include <xml/saxnamespacefilter.hxx>

#include <comphelper/attributelist.hxx>

#include <vcl/svapp.hxx>
#include <rtl/logfile.hxx>

using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::uno;

namespace framework{


SaxNamespaceFilter::SaxNamespaceFilter( Reference< XDocumentHandler >& rSax1DocumentHandler ) :
    ThreadHelpBase( &Application::GetSolarMutex() ),
     m_xLocator( 0 ),
     xDocumentHandler( rSax1DocumentHandler ),
     m_nDepth( 0 ),
     m_aXMLAttributeNamespace( RTL_CONSTASCII_USTRINGPARAM( "xmlns" )),
     m_aXMLAttributeType( RTL_CONSTASCII_USTRINGPARAM( "CDATA" ))
{
}

SaxNamespaceFilter::~SaxNamespaceFilter()
{
}

// XDocumentHandler
void SAL_CALL SaxNamespaceFilter::startDocument(void)
    throw ( SAXException, RuntimeException )
{
}

void SAL_CALL SaxNamespaceFilter::endDocument(void)
    throw(  SAXException, RuntimeException )
{
}

void SAL_CALL SaxNamespaceFilter::startElement(
    const rtl::OUString& rName, const Reference< XAttributeList > &xAttribs )
    throw(  SAXException, RuntimeException )
{
    XMLNamespaces aXMLNamespaces;
    if ( !m_aNamespaceStack.empty() )
        aXMLNamespaces = m_aNamespaceStack.top();

    ::comphelper::AttributeList* pNewList = new ::comphelper::AttributeList();

    // examine all namespaces for this level
    ::std::vector< sal_Int16 > aAttributeIndexes;
    {
        for ( sal_Int16 i=0; i< xAttribs->getLength(); i++ )
        {
            ::rtl::OUString aName = xAttribs->getNameByIndex( i );
            if ( aName.compareTo( m_aXMLAttributeNamespace, m_aXMLAttributeNamespace.getLength() ) == 0 )
                aXMLNamespaces.addNamespace( aName, xAttribs->getValueByIndex( i ));
            else
                aAttributeIndexes.push_back( i );
        }
    }

    // current namespaces for this level
    m_aNamespaceStack.push( aXMLNamespaces );

    try
    {
        // apply namespaces to all remaing attributes
        for ( ::std::vector< sal_Int16 >::const_iterator i(
                  aAttributeIndexes.begin());
              i != aAttributeIndexes.end(); ++i )
        {
            ::rtl::OUString aAttributeName           = xAttribs->getNameByIndex( *i );
            ::rtl::OUString aValue                   = xAttribs->getValueByIndex( *i );
            ::rtl::OUString aNamespaceAttributeName = aXMLNamespaces.applyNSToAttributeName( aAttributeName );
            pNewList->AddAttribute( aNamespaceAttributeName, m_aXMLAttributeType, aValue );
        }
    }
    catch ( SAXException& e )
    {
        e.Message = ::rtl::OUString( getErrorLineString() + e.Message );
        throw;
    }

    ::rtl::OUString aNamespaceElementName;

    try
    {
         aNamespaceElementName = aXMLNamespaces.applyNSToElementName( rName );
    }
    catch ( SAXException& e )
    {
        e.Message = ::rtl::OUString( getErrorLineString() + e.Message );
        throw;
    }

    xDocumentHandler->startElement( aNamespaceElementName, pNewList );
}

void SAL_CALL SaxNamespaceFilter::endElement(const rtl::OUString& aName)
    throw(  SAXException, RuntimeException )
{
    XMLNamespaces& aXMLNamespaces = m_aNamespaceStack.top();
    ::rtl::OUString aNamespaceElementName;

    try
    {
        aNamespaceElementName = aXMLNamespaces.applyNSToElementName( aName );
    }
    catch ( SAXException& e )
    {
        e.Message = ::rtl::OUString( getErrorLineString() + e.Message );
        throw;
    }

    xDocumentHandler->endElement( aNamespaceElementName );
    m_aNamespaceStack.pop();
}

void SAL_CALL SaxNamespaceFilter::characters(const rtl::OUString& aChars)
    throw(  SAXException, RuntimeException )
{
    xDocumentHandler->characters( aChars );
}

void SAL_CALL SaxNamespaceFilter::ignorableWhitespace(const rtl::OUString& aWhitespaces)
    throw(  SAXException, RuntimeException )
{
    xDocumentHandler->ignorableWhitespace( aWhitespaces );
}

void SAL_CALL SaxNamespaceFilter::processingInstruction(
    const rtl::OUString& aTarget, const rtl::OUString& aData)
    throw(  SAXException, RuntimeException )
{
    xDocumentHandler->processingInstruction( aTarget, aData );
}

void SAL_CALL SaxNamespaceFilter::setDocumentLocator(
    const Reference< XLocator > &xLocator)
    throw(  SAXException, RuntimeException )
{
    m_xLocator = xLocator;
    xDocumentHandler->setDocumentLocator( xLocator );
}

::rtl::OUString SaxNamespaceFilter::getErrorLineString()
{
    char buffer[32];

    if ( m_xLocator.is() )
    {
        snprintf( buffer, sizeof(buffer), "Line: %ld - ", static_cast<long>( m_xLocator->getLineNumber() ));
        return ::rtl::OUString::createFromAscii( buffer );
    }
    else
        return ::rtl::OUString();
}

} // namespace

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
