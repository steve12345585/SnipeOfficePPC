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


#include <xmlsecurity/biginteger.hxx>

#include <sal/types.h>
//For reasons that escape me, this is what xmlsec does when size_t is not 4
#if SAL_TYPES_SIZEOFPOINTER != 4
#    define XMLSEC_NO_SIZE_T
#endif
#include <xmlsec/xmlsec.h>
#include <xmlsec/bn.h>
#include <com/sun/star/uno/Sequence.hxx>

using namespace ::com::sun::star::uno ;
using ::rtl::OUString ;

Sequence< sal_Int8 > numericStringToBigInteger ( OUString numeral )
{
    if( numeral.getStr() != NULL )
    {
        xmlChar* chNumeral ;
        const xmlSecByte* bnInteger ;
        xmlSecSize length ;
        xmlSecBn bn ;

        rtl::OString onumeral = rtl::OUStringToOString( numeral , RTL_TEXTENCODING_ASCII_US ) ;

        chNumeral = xmlStrndup( ( const xmlChar* )onumeral.getStr(), ( int )onumeral.getLength() ) ;

        if( xmlSecBnInitialize( &bn, 0 ) < 0 ) {
            xmlFree( chNumeral ) ;
            return Sequence< sal_Int8 >();
        }

        if( xmlSecBnFromDecString( &bn, chNumeral ) < 0 ) {
            xmlFree( chNumeral ) ;
            xmlSecBnFinalize( &bn ) ;
            return Sequence< sal_Int8 >();
        }

        xmlFree( chNumeral ) ;

        length = xmlSecBnGetSize( &bn ) ;
        if( length <= 0 ) {
            xmlSecBnFinalize( &bn ) ;
            return Sequence< sal_Int8 >();
        }

        bnInteger = xmlSecBnGetData( &bn ) ;
        if( bnInteger == NULL ) {
            xmlSecBnFinalize( &bn ) ;
            return Sequence< sal_Int8 >();
        }

        Sequence< sal_Int8 > integer( length ) ;
        for( unsigned int i = 0 ; i < length ; i ++ )
        {
            integer[i] = *( bnInteger + i ) ;
        }

        xmlSecBnFinalize( &bn ) ;
        return integer ;
    }

    return Sequence< sal_Int8 >();
}

OUString bigIntegerToNumericString ( Sequence< sal_Int8 > integer )
{
    OUString aRet ;

    if( integer.getLength() ) {
        xmlSecBn bn ;
        xmlChar* chNumeral ;

        if( xmlSecBnInitialize( &bn, 0 ) < 0 )
            return aRet ;

        if( xmlSecBnSetData( &bn, ( const unsigned char* )&integer[0], integer.getLength() ) < 0 ) {
            xmlSecBnFinalize( &bn ) ;
            return aRet ;
        }

        chNumeral = xmlSecBnToDecString( &bn ) ;
        if( chNumeral == NULL ) {
            xmlSecBnFinalize( &bn ) ;
            return aRet ;
        }

        aRet = OUString::createFromAscii( ( const char* )chNumeral ) ;

        xmlSecBnFinalize( &bn ) ;
        xmlFree( chNumeral ) ;
    }

    return aRet ;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
