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

#include "dp_misc.h"
#include "dp_parceldesc.hxx"



using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

using ::rtl::OUString;

namespace dp_registry
{
namespace backend
{
namespace sfwk
{


// XDocumentHandler
void SAL_CALL
ParcelDescDocHandler::startDocument()
throw ( xml::sax::SAXException, RuntimeException )
{
    m_bIsParsed = false;
}

void SAL_CALL
ParcelDescDocHandler::endDocument()
throw ( xml::sax::SAXException, RuntimeException )
{
    m_bIsParsed = true;
}

void SAL_CALL
ParcelDescDocHandler::characters( const OUString & )
    throw ( xml::sax::SAXException, RuntimeException )
{
}

void SAL_CALL
ParcelDescDocHandler::ignorableWhitespace( const OUString & )
    throw ( xml::sax::SAXException, RuntimeException )
{
}

void SAL_CALL
ParcelDescDocHandler::processingInstruction(
    const OUString &, const OUString & )
        throw ( xml::sax::SAXException, RuntimeException )
{
}

void SAL_CALL
ParcelDescDocHandler::setDocumentLocator(
    const Reference< xml::sax::XLocator >& )
        throw ( xml::sax::SAXException, RuntimeException )
{
}

void SAL_CALL
ParcelDescDocHandler::startElement( const OUString& aName,
    const Reference< xml::sax::XAttributeList > & xAttribs )
        throw ( xml::sax::SAXException,
            RuntimeException )
{

    dp_misc::TRACE(OUSTR("ParcelDescDocHandler::startElement() for ") +
        aName + OUSTR("\n"));
    if ( !skipIndex )
    {
        if ( aName == "parcel" )
        {
            m_sLang = xAttribs->getValueByName( OUString("language" ) );
        }
        ++skipIndex;
    }
    else
    {
        dp_misc::TRACE(OUSTR("ParcelDescDocHandler::startElement() skipping for ")
            + aName + OUSTR("\n"));
    }

}

void SAL_CALL ParcelDescDocHandler::endElement( const OUString & aName )
   throw ( xml::sax::SAXException, RuntimeException )
{
    if ( skipIndex )
    {
        --skipIndex;
        dp_misc::TRACE(OUSTR("ParcelDescDocHandler::endElement() skipping for ")
            + aName + OUSTR("\n"));
    }
}


}
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
