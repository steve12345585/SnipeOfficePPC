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

#include "xmloff/xmlerror.hxx"
#include <tools/debug.hxx>
#include <rtl/ustring.hxx>
#include <com/sun/star/xml/sax/XLocator.hpp>
#include <com/sun/star/xml/sax/SAXParseException.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Sequence.hxx>


#include <rtl/ustrbuf.hxx>



using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::xml::sax::XLocator;
using ::com::sun::star::xml::sax::SAXParseException;


//
/// ErrorRecord: contains all information for one error
//

class ErrorRecord
{
public:

    ErrorRecord( sal_Int32 nId,
                 const Sequence<OUString>& rParams,
                 const OUString& rExceptionMessage,
                 sal_Int32 nRow,
                 sal_Int32 nColumn,
                 const OUString& rPublicId,
                 const OUString& rSystemId);
    ~ErrorRecord();

    sal_Int32 nId;  /// error ID

    OUString sExceptionMessage;/// message of original exception (if available)

    // XLocator information:
    sal_Int32 nRow;     /// row number where error occurred (or -1 for unknown)
    sal_Int32 nColumn;  /// column number where error occurred (or -1)
    OUString sPublicId; /// public identifier
    OUString sSystemId; /// public identifier

    /// message Parameters
    Sequence<OUString> aParams;
};


ErrorRecord::ErrorRecord( sal_Int32 nID, const Sequence<OUString>& rParams,
    const OUString& rExceptionMessage, sal_Int32 nRowNumber, sal_Int32 nCol,
    const OUString& rPublicId, const OUString& rSystemId) :
        nId(nID),
        sExceptionMessage(rExceptionMessage),
        nRow(nRowNumber),
        nColumn(nCol),
        sPublicId(rPublicId),
        sSystemId(rSystemId),
        aParams(rParams)
{
}

ErrorRecord::~ErrorRecord()
{
}




XMLErrors::XMLErrors()
{
}

XMLErrors::~XMLErrors()
{
}

void XMLErrors::AddRecord(
    sal_Int32 nId,
    const Sequence<OUString> & rParams,
    const OUString& rExceptionMessage,
    sal_Int32 nRow,
    sal_Int32 nColumn,
    const OUString& rPublicId,
    const OUString& rSystemId )
{
    aErrors.push_back( ErrorRecord( nId, rParams, rExceptionMessage,
                                    nRow, nColumn, rPublicId, rSystemId ) );

#ifdef DBG_UTIL

    // give detailed assertion on this message

    OUStringBuffer sMessage;

    sMessage.appendAscii( "An error or a warning has occurred during XML import/export!\n" );

    // ID & flags
    sMessage.appendAscii( "Error-Id: 0x");
    sMessage.append( nId, 16 );
    sMessage.appendAscii( "\n    Flags: " );
    sal_Int32 nFlags = (nId & XMLERROR_MASK_FLAG);
    sMessage.append( nFlags >> 28, 16 );
    if( (nFlags & XMLERROR_FLAG_WARNING) != 0 )
        sMessage.appendAscii( " WARNING" );
    if( (nFlags & XMLERROR_FLAG_ERROR) != 0 )
        sMessage.appendAscii( " ERRROR" );
    if( (nFlags & XMLERROR_FLAG_SEVERE) != 0 )
        sMessage.appendAscii( " SEVERE" );
    sMessage.appendAscii( "\n    Class: " );
    sal_Int32 nClass = (nId & XMLERROR_MASK_CLASS);
    sMessage.append( nClass >> 16, 16 );
    if( (nClass & XMLERROR_CLASS_IO) != 0 )
        sMessage.appendAscii( " IO" );
    if( (nClass & XMLERROR_CLASS_FORMAT) != 0 )
        sMessage.appendAscii( " FORMAT" );
    if( (nClass & XMLERROR_CLASS_API) != 0 )
        sMessage.appendAscii( " API" );
    if( (nClass & XMLERROR_CLASS_OTHER) != 0 )
        sMessage.appendAscii( " OTHER" );
    sMessage.appendAscii( "\n    Number: " );
    sal_Int32 nNumber = (nId & XMLERROR_MASK_NUMBER);
    sMessage.append( nNumber, 16 );
    sMessage.appendAscii( "\n");

    // the parameters
    sMessage.appendAscii( "Parameters:\n" );
    sal_Int32 nLength = rParams.getLength();
    const OUString* pParams = rParams.getConstArray();
    for( sal_Int32 i = 0; i < nLength; i++ )
    {
        sMessage.appendAscii( "    " );
        sMessage.append( i );
        sMessage.appendAscii( ": " );
        sMessage.append( pParams[i] );
        sMessage.appendAscii( "\n" );
    }

    // the exception message
    sMessage.appendAscii( "Exception-Message: " );
    sMessage.append( rExceptionMessage );
    sMessage.appendAscii( "\n" );

    // position (if given)
    if( (nRow != -1) || (nColumn != -1) )
    {
        sMessage.appendAscii( "Position:\n    Public Identifier: " );
        sMessage.append( rPublicId );
        sMessage.appendAscii( "\n    System Identifier: " );
        sMessage.append( rSystemId );
        sMessage.appendAscii( "\n    Row, Column: " );
        sMessage.append( nRow );
        sMessage.appendAscii( "," );
        sMessage.append( nColumn );
        sMessage.appendAscii( "\n" );
    }

    // convert to byte string and signal the error
    rtl::OString aError(rtl::OUStringToOString(sMessage.makeStringAndClear(),
        RTL_TEXTENCODING_ASCII_US));
    OSL_FAIL( aError.getStr() );
#endif
}

void XMLErrors::AddRecord(
    sal_Int32 nId,
    const Sequence<OUString> & rParams,
    const OUString& rExceptionMessage,
    const Reference<XLocator> & rLocator)
{
    if ( rLocator.is() )
    {
        AddRecord( nId, rParams, rExceptionMessage,
                   rLocator->getLineNumber(), rLocator->getColumnNumber(),
                   rLocator->getPublicId(), rLocator->getSystemId() );
    }
    else
    {
        OUString sEmpty;
        AddRecord( nId, rParams, rExceptionMessage,
                   -1, -1, sEmpty, sEmpty );
    }
}

void XMLErrors::ThrowErrorAsSAXException(sal_Int32 nIdMask)
    throw( SAXParseException )
{
    // search first error/warning that matches the nIdMask
    for( ErrorList::iterator aIter = aErrors.begin();
         aIter != aErrors.end();
         ++aIter )
    {
        if ( (aIter->nId & nIdMask) != 0 )
        {
            // we throw the error
            ErrorRecord& rErr = aErrors[0];
            Any aAny;
            aAny <<= rErr.aParams;
            throw SAXParseException(
                rErr.sExceptionMessage, NULL, aAny,
                rErr.sPublicId, rErr.sSystemId, rErr.nRow, rErr.nColumn );
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
