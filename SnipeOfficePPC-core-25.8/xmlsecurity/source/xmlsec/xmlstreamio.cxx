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


/*
 * Implementation of the I/O interfaces based on stream and URI binding
 */
#include "xmlstreamio.hxx"
#include <rtl/ustring.hxx>
#include "rtl/uri.hxx"

#include <libxml/uri.h>
#include <sal/types.h>
//For reasons that escape me, this is what xmlsec does when size_t is not 4
#if SAL_TYPES_SIZEOFPOINTER != 4
#    define XMLSEC_NO_SIZE_T
#endif
#include <xmlsec/io.h>

#define XMLSTREAMIO_INITIALIZED 0x01
#define XMLSTREAMIO_REGISTERED  0x02

/* Global variables */
/*-
 * Enable stream I/O or not.
 */
static char enableXmlStreamIO = 0x00 ;

::com::sun::star::uno::Reference< ::com::sun::star::xml::crypto::XUriBinding > m_xUriBinding ;

extern "C"
int xmlStreamMatch( const char* uri )
{
    ::com::sun::star::uno::Reference< com::sun::star::io::XInputStream > xInputStream ;

    if( ( enableXmlStreamIO & XMLSTREAMIO_INITIALIZED ) &&
        ( enableXmlStreamIO & XMLSTREAMIO_REGISTERED ) ) {
        if( uri == NULL || !m_xUriBinding.is() )
            return 0 ;
        //XMLSec first unescapes the uri and  calls this function. For example, we pass the Uri
        //ObjectReplacements/Object%201 then XMLSec passes ObjectReplacements/Object 1
        //first. If this failed it would try this
        //again with the original escaped string. However, it does not get this far, because there
        //is another callback registered by libxml which claims to be able to handle this uri.
        ::rtl::OUString sUri =
            ::rtl::Uri::encode( ::rtl::OUString::createFromAscii( uri ),
            rtl_UriCharClassUric, rtl_UriEncodeKeepEscapes, RTL_TEXTENCODING_UTF8);
        xInputStream = m_xUriBinding->getUriBinding( sUri ) ;
        if (!xInputStream.is())
        {
            //Try the the passed in uri directly.
            //For old documents prior OOo 3.0. We did not use URIs then.
            xInputStream = m_xUriBinding->getUriBinding(
                ::rtl::OUString::createFromAscii(uri));
        }
    }
    if (xInputStream.is())
        return 1;
    else
        return 0 ;
}

extern "C"
void* xmlStreamOpen( const char* uri )
{
    ::com::sun::star::uno::Reference< com::sun::star::io::XInputStream > xInputStream ;
    ::com::sun::star::io::XInputStream* pInputStream ;

    if( ( enableXmlStreamIO & XMLSTREAMIO_INITIALIZED ) &&
        ( enableXmlStreamIO & XMLSTREAMIO_REGISTERED ) ) {
        if( uri == NULL || !m_xUriBinding.is() )
            return NULL ;

        //see xmlStreamMatch
        ::rtl::OUString sUri =
            ::rtl::Uri::encode( ::rtl::OUString::createFromAscii( uri ),
            rtl_UriCharClassUric, rtl_UriEncodeKeepEscapes, RTL_TEXTENCODING_UTF8);
        xInputStream = m_xUriBinding->getUriBinding( sUri ) ;
        if (!xInputStream.is())
        {
            //For old documents.
            //try the the passed in uri directly.
            xInputStream = m_xUriBinding->getUriBinding(
                ::rtl::OUString::createFromAscii(uri));
        }

        if( xInputStream.is() ) {
            pInputStream = xInputStream.get() ;
            pInputStream->acquire() ;
            return ( void* )pInputStream ;
        }
    }

    return NULL ;
}

extern "C"
int xmlStreamRead( void* context, char* buffer, int len )
{
    int numbers ;
    ::com::sun::star::uno::Reference< com::sun::star::io::XInputStream > xInputStream ;
    ::com::sun::star::uno::Sequence< sal_Int8 > outSeqs( len ) ;

    numbers = 0 ;
    if( ( enableXmlStreamIO & XMLSTREAMIO_INITIALIZED ) &&
        ( enableXmlStreamIO & XMLSTREAMIO_REGISTERED ) ) {
        if( context != NULL ) {
            xInputStream = ( com::sun::star::io::XInputStream* )context ;
            if( !xInputStream.is() )
                return 0 ;

            numbers = xInputStream->readBytes( outSeqs, len ) ;
            const sal_Int8* readBytes = ( const sal_Int8* )outSeqs.getArray() ;
            for( int i = 0 ; i < numbers ; i ++ )
                *( buffer + i ) = *( readBytes + i ) ;
        }
    }

    return numbers ;
}

extern "C"
int xmlStreamClose( void * context )
{
    ::com::sun::star::io::XInputStream* pInputStream ;

    if( ( enableXmlStreamIO & XMLSTREAMIO_INITIALIZED ) &&
        ( enableXmlStreamIO & XMLSTREAMIO_REGISTERED ) ) {
        if( context != NULL ) {
            pInputStream = ( ::com::sun::star::io::XInputStream* )context ;
            pInputStream->release() ;
        }
    }

    return 0 ;
}

int xmlEnableStreamInputCallbacks()
{

    if( !( enableXmlStreamIO & XMLSTREAMIO_INITIALIZED ) ) {
        //Register the callbacks into xmlSec
        //In order to make the xmlsec io finding the callbacks firstly,
        //I put the callbacks at the very begining.

        //Cleanup the older callbacks.
        //Notes: all none default callbacks will lose.
        xmlSecIOCleanupCallbacks() ;

        //Register my classbacks.
        int cbs = xmlSecIORegisterCallbacks(
                    xmlStreamMatch,
                    xmlStreamOpen,
                    xmlStreamRead,
                    xmlStreamClose ) ;
        if( cbs < 0 ) {
            return -1 ;
        }

        //Register the default callbacks.
        //Notes: the error will cause xmlsec working problems.
        cbs = xmlSecIORegisterDefaultCallbacks() ;
        if( cbs < 0 ) {
            return -1 ;
        }

        enableXmlStreamIO |= XMLSTREAMIO_INITIALIZED ;
    }

    return 0 ;
}

int xmlRegisterStreamInputCallbacks(
    ::com::sun::star::uno::Reference< ::com::sun::star::xml::crypto::XUriBinding >& aUriBinding
) {
    if( !( enableXmlStreamIO & XMLSTREAMIO_INITIALIZED ) ) {
        if( xmlEnableStreamInputCallbacks() < 0 )
            return -1 ;
    }

    if( !( enableXmlStreamIO & XMLSTREAMIO_REGISTERED ) ) {
        enableXmlStreamIO |= XMLSTREAMIO_REGISTERED ;
    }

    m_xUriBinding = aUriBinding ;

    return 0 ;
}

int xmlUnregisterStreamInputCallbacks( void )
{
    if( ( enableXmlStreamIO & XMLSTREAMIO_REGISTERED ) ) {
        //Clear the uir-stream binding
        m_xUriBinding.clear() ;

        //disable the registered flag
        enableXmlStreamIO &= ~XMLSTREAMIO_REGISTERED ;
    }

    return 0 ;
}

void xmlDisableStreamInputCallbacks() {
    xmlUnregisterStreamInputCallbacks() ;
    enableXmlStreamIO &= ~XMLSTREAMIO_INITIALIZED ;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
