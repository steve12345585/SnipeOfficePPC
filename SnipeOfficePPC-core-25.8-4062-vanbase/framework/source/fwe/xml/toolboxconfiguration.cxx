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


#include <framework/toolboxconfiguration.hxx>
#include <xml/toolboxdocumenthandler.hxx>
#include <xml/saxnamespacefilter.hxx>
#include <services.h>

#include <com/sun/star/xml/sax/Parser.hpp>
#include <com/sun/star/xml/sax/Writer.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#include <comphelper/processfactory.hxx>
#include <unotools/streamwrap.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::container;


namespace framework
{

sal_Bool ToolBoxConfiguration::LoadToolBox(
    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
    const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& rInputStream,
    const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >& rToolbarConfiguration )
{
    Reference< XParser > xParser = Parser::create(rxContext);

    // connect stream to input stream to the parser
    InputSource aInputSource;

    aInputSource.aInputStream = rInputStream;

    // create namespace filter and set menudocument handler inside to support xml namespaces
    Reference< XDocumentHandler > xDocHandler( new OReadToolBoxDocumentHandler( rToolbarConfiguration ));
    Reference< XDocumentHandler > xFilter( new SaxNamespaceFilter( xDocHandler ));

    // connect parser and filter
    xParser->setDocumentHandler( xFilter );

    try
    {
        xParser->parseStream( aInputSource );
        return sal_True;
    }
    catch ( const RuntimeException& )
    {
        return sal_False;
    }
    catch( const SAXException& )
    {
        return sal_False;
    }
    catch( const ::com::sun::star::io::IOException& )
    {
        return sal_False;
    }
}


sal_Bool ToolBoxConfiguration::StoreToolBox(
    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
    const ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream >& rOutputStream,
    const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& rToolbarConfiguration )
{
    Reference< XWriter > xWriter = Writer::create(rxContext);
    xWriter->setOutputStream( rOutputStream );

    try
    {
        Reference< XDocumentHandler > xHandler( xWriter, UNO_QUERY_THROW );
        OWriteToolBoxDocumentHandler aWriteToolBoxDocumentHandler( rToolbarConfiguration, xHandler );
        aWriteToolBoxDocumentHandler.WriteToolBoxDocument();
        return sal_True;
    }
    catch ( const RuntimeException& )
    {
        return sal_False;
    }
    catch ( const SAXException& )
    {
        return sal_False;
    }
    catch ( const ::com::sun::star::io::IOException& )
    {
        return sal_False;
    }
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
