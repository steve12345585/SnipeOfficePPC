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

/****************** I N C L U D E S **************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Solar Definitionen
#include <tools/solar.h>
#include <rsctools.hxx>

#include <rtl/textcvt.h>
#include <rtl/textenc.h>
#include <rtl/alloc.h>

/*************************************************************************
|*
|*    RscChar::MakeChar()
|*
|*    Beschreibung      Der String wird nach C-Konvention umgesetzt
|*
*************************************************************************/
char * RscChar::MakeUTF8( char * pStr, sal_uInt16 nTextEncoding )
{
    sal_Size nMaxUniCodeBuf = strlen( pStr ) + 1;
    if( nMaxUniCodeBuf * 6 > 0x0FFFFF )
        RscExit( 10 );

    char *          pOrgStr = new char[ nMaxUniCodeBuf ];
    sal_uInt32      nOrgLen = 0;

    char cOld = '1';
    while( cOld != 0 )
    {
        char c;

        if( *pStr == '\\' )
        {
            ++pStr;
            switch( *pStr )
            {
                case 'a':
                    c = '\a';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'v':
                    c = '\v';
                    break;
                case '\\':
                    c = '\\';
                    break;
                case '?':
                    c = '\?';
                    break;
                case '\'':
                    c = '\'';
                    break;
                case '\"':
                    c = '\"';
                    break;
                default:
                {
                    if( '0' <= *pStr && '7' >= *pStr )
                    {
                        sal_uInt16  nChar = 0;
                        int  i = 0;
                        while( '0' <= *pStr && '7' >= *pStr && i != 3 )
                        {
                            nChar = nChar * 8 + (sal_uInt8)*pStr - (sal_uInt8)'0';
                            ++pStr;
                            i++;
                        }
                        if( nChar > 255 )
                        {
                            // Wert zu gross, oder kein 3 Ziffern
                            delete [] pOrgStr;
                            return( NULL );
                        }
                        c = (char)nChar;
                        pStr--;
                    }
                    else if( 'x' == *pStr )
                    {
                        sal_uInt16  nChar = 0;
                        int  i = 0;
                        ++pStr;
                        while( isxdigit( *pStr ) && i != 2 )
                        {
                            if( isdigit( *pStr ) )
                                nChar = nChar * 16 + (sal_uInt8)*pStr - (sal_uInt8)'0';
                            else if( isupper( *pStr ) )
                                nChar = nChar * 16 + (sal_uInt8)*pStr - (sal_uInt8)'A' +10;
                            else
                                nChar = nChar * 16 + (sal_uInt8)*pStr - (sal_uInt8)'a' +10;
                            ++pStr;
                            i++;
                        }
                        c = (char)nChar;
                        pStr--;
                    }
                    else
                        c = *pStr;
                };
            }
        }
        else
            c = *pStr;
        pOrgStr[ nOrgLen++ ] = c;
        cOld = *pStr;
        pStr++;
    }

    sal_Unicode *   pUniCode = new sal_Unicode[ nMaxUniCodeBuf ];
    rtl_TextToUnicodeConverter hConv = rtl_createTextToUnicodeConverter( nTextEncoding );

    sal_uInt32 nInfo;
    sal_Size   nSrcCvtBytes;
    sal_Size nUniSize = rtl_convertTextToUnicode( hConv, 0,
                                                pOrgStr, nOrgLen,
                                                pUniCode, nMaxUniCodeBuf,
                                                RTL_TEXTTOUNICODE_FLAGS_UNDEFINED_DEFAULT
                                                | RTL_TEXTTOUNICODE_FLAGS_MBUNDEFINED_DEFAULT
                                                | RTL_TEXTTOUNICODE_FLAGS_INVALID_DEFAULT
                                                | RTL_TEXTTOUNICODE_FLAGS_FLUSH,
                                                &nInfo,
                                                &nSrcCvtBytes );

    rtl_destroyTextToUnicodeConverter( hConv );
    delete[] pOrgStr, pOrgStr = 0;

    hConv = rtl_createUnicodeToTextConverter( RTL_TEXTENCODING_UTF8 );
    // factor fo 6 is the maximum size of an UNICODE character as utf8
    char * pUtf8 = (char *)rtl_allocateMemory( nUniSize * 6 );
    rtl_convertUnicodeToText( hConv, 0,
                            pUniCode, nUniSize,
                            pUtf8, nUniSize * 6,
                            RTL_UNICODETOTEXT_FLAGS_UNDEFINED_DEFAULT
                            | RTL_UNICODETOTEXT_FLAGS_INVALID_DEFAULT
                            | RTL_UNICODETOTEXT_FLAGS_FLUSH,
                            &nInfo,
                            &nSrcCvtBytes );

    rtl_destroyTextToUnicodeConverter( hConv );
    delete[] pUniCode, pUniCode = 0;

    return pUtf8;
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
