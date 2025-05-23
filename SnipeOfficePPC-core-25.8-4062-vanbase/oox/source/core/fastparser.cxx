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

#include "oox/core/fastparser.hxx"

#include "oox/core/fasttokenhandler.hxx"
#include "oox/helper/containerhelper.hxx"
#include "oox/helper/helper.hxx"
#include "oox/helper/storagebase.hxx"
#include "oox/token/namespacemap.hxx"

namespace oox {
namespace core {

// ============================================================================

using namespace ::com::sun::star::io;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;

using ::rtl::OUString;

// ============================================================================

namespace {

class InputStreamCloseGuard
{
public:
    explicit            InputStreamCloseGuard( const Reference< XInputStream >& rxInStream, bool bCloseStream );
                        ~InputStreamCloseGuard();
private:
    Reference< XInputStream > mxInStream;
    bool                mbCloseStream;
};

InputStreamCloseGuard::InputStreamCloseGuard( const Reference< XInputStream >& rxInStream, bool bCloseStream ) :
    mxInStream( rxInStream ),
    mbCloseStream( bCloseStream )
{
}

InputStreamCloseGuard::~InputStreamCloseGuard()
{
    if( mxInStream.is() && mbCloseStream ) try { mxInStream->closeInput(); } catch( Exception& ) {}
}

} // namespace

// ============================================================================

FastParser::FastParser( const Reference< XComponentContext >& rxContext ) throw( RuntimeException ) :
    mrNamespaceMap( StaticNamespaceMap::get() )
{
    // create a fast parser instance
    Reference< XMultiComponentFactory > xFactory( rxContext->getServiceManager(), UNO_SET_THROW );
    mxParser.set( xFactory->createInstanceWithContext( CREATE_OUSTRING( "com.sun.star.xml.sax.FastParser" ), rxContext ), UNO_QUERY_THROW );

    // create the fast tokenhandler
    mxTokenHandler.set( new FastTokenHandler );

    // create the fast token handler based on the OOXML token list
    mxParser->setTokenHandler( mxTokenHandler );
}

FastParser::~FastParser()
{
}

void FastParser::registerNamespace( sal_Int32 nNamespaceId ) throw( IllegalArgumentException, RuntimeException )
{
    if( !mxParser.is() )
        throw RuntimeException();

    const OUString* pNamespaceUrl = ContainerHelper::getMapElement( mrNamespaceMap, nNamespaceId );
    if( !pNamespaceUrl )
        throw IllegalArgumentException();

    mxParser->registerNamespace( *pNamespaceUrl, nNamespaceId );
}

void FastParser::setDocumentHandler( const Reference< XFastDocumentHandler >& rxDocHandler ) throw( RuntimeException )
{
    if( !mxParser.is() )
        throw RuntimeException();
    mxParser->setFastDocumentHandler( rxDocHandler );
}

void FastParser::parseStream( const InputSource& rInputSource, bool bCloseStream ) throw( SAXException, IOException, RuntimeException )
{
    // guard closing the input stream also when exceptions are thrown
    InputStreamCloseGuard aGuard( rInputSource.aInputStream, bCloseStream );
    if( !mxParser.is() )
        throw RuntimeException();
    mxParser->parseStream( rInputSource );
}

void FastParser::parseStream( const Reference< XInputStream >& rxInStream, const OUString& rStreamName, bool bCloseStream ) throw( SAXException, IOException, RuntimeException )
{
    InputSource aInputSource;
    aInputSource.sSystemId = rStreamName;
    aInputSource.aInputStream = rxInStream;
    parseStream( aInputSource, bCloseStream );
}

void FastParser::parseStream( StorageBase& rStorage, const OUString& rStreamName, bool bCloseStream ) throw( SAXException, IOException, RuntimeException )
{
    parseStream( rStorage.openInputStream( rStreamName ), rStreamName, bCloseStream );
}

OUString FastParser::getNamespaceURL( const OUString& rPrefix ) throw( IllegalArgumentException, RuntimeException )
{
    if( !mxParser.is() )
        throw RuntimeException();
    return mxParser->getNamespaceURL( rPrefix );
}

sal_Int32 FastParser::getNamespaceId( const OUString& rUrl )
{
    for( NamespaceMap::const_iterator aIt = mrNamespaceMap.begin(), aEnd = mrNamespaceMap.end(); aIt != aEnd; ++aIt )
        if( rUrl  == aIt->second )
            return aIt->first;
    return 0;
}

// ============================================================================

} // namespace core
} // namespace oox

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
