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


#include <stdio.h>                      // for EOF
#include <rtl/tencinfo.h>
#include <tools/stream.hxx>
#include <tools/debug.hxx>
#include <svtools/rtftoken.h>
#include <svtools/rtfkeywd.hxx>
#include <svtools/parrtf.hxx>
#include <comphelper/string.hxx>

const int MAX_STRING_LEN = 1024;
const int MAX_TOKEN_LEN = 128;

#define RTF_ISDIGIT( c ) comphelper::string::isdigitAscii(c)
#define RTF_ISALPHA( c ) comphelper::string::isalphaAscii(c)

SvRTFParser::SvRTFParser( SvStream& rIn, sal_uInt8 nStackSize )
    : SvParser( rIn, nStackSize ),
    eUNICodeSet( RTL_TEXTENCODING_MS_1252 ),    // default ist ANSI-CodeSet
    nUCharOverread( 1 )
{
    // default ist ANSI-CodeSet
    SetSrcEncoding( RTL_TEXTENCODING_MS_1252 );
    bRTF_InTextRead = false;
}

SvRTFParser::~SvRTFParser()
{
}




int SvRTFParser::_GetNextToken()
{
    int nRet = 0;
    do {
        int bNextCh = true;
        switch( nNextCh )
        {
        case '\\':
            {
                // Steuerzeichen
                switch( nNextCh = GetNextChar() )
                {
                case '{':
                case '}':
                case '\\':
                case '+':       // habe ich in einem RTF-File gefunden
                case '~':       // nonbreaking space
                case '-':       // optional hyphen
                case '_':       // nonbreaking hyphen
                case '\'':      // HexValue
                    nNextCh = '\\';
                    rInput.SeekRel( -1 );
                    ScanText();
                    nRet = RTF_TEXTTOKEN;
                    bNextCh = 0 == nNextCh;
                    break;

                case '*':       // ignoreflag
                    nRet = RTF_IGNOREFLAG;
                    break;
                case ':':       // subentry in an index entry
                    nRet = RTF_SUBENTRYINDEX;
                    break;
                case '|':       // formula-charakter
                    nRet = RTF_FORMULA;
                    break;

                case 0x0a:
                case 0x0d:
                    nRet = RTF_PAR;
                    break;

                default:
                    if( RTF_ISALPHA( nNextCh ) )
                    {
                        aToken = '\\';
                        {
                            String aStrBuffer;
                            sal_Unicode* pStr = aStrBuffer.AllocBuffer(
                                                            MAX_TOKEN_LEN );
                            xub_StrLen nStrLen = 0;
                            do {
                                *(pStr + nStrLen++) = nNextCh;
                                if( MAX_TOKEN_LEN == nStrLen )
                                {
                                    aToken += aStrBuffer;
                                    aToken.GetBufferAccess();  // make unique string!
                                    nStrLen = 0;
                                }
                                nNextCh = GetNextChar();
                            } while( RTF_ISALPHA( nNextCh ) );
                            if( nStrLen )
                            {
                                aStrBuffer.ReleaseBufferAccess( nStrLen );
                                aToken += aStrBuffer;
                            }
                        }

                        // Minus fuer numerischen Parameter
                        int bNegValue = false;
                        if( '-' == nNextCh )
                        {
                            bNegValue = true;
                            nNextCh = GetNextChar();
                        }

                        // evt. Numerischer Parameter
                        if( RTF_ISDIGIT( nNextCh ) )
                        {
                            nTokenValue = 0;
                            do {
                                nTokenValue *= 10;
                                nTokenValue += nNextCh - '0';
                                nNextCh = GetNextChar();
                            } while( RTF_ISDIGIT( nNextCh ) );
                            if( bNegValue )
                                nTokenValue = -nTokenValue;
                            bTokenHasValue=true;
                        }
                        else if( bNegValue )        // das Minus wieder zurueck
                        {
                            nNextCh = '-';
                            rInput.SeekRel( -1 );
                        }
                        if( ' ' == nNextCh )        // Blank gehoert zum Token!
                            nNextCh = GetNextChar();

                        // suche das Token in der Tabelle:
                        if( 0 == (nRet = GetRTFToken( aToken )) )
                            // Unknown Control
                            nRet = RTF_UNKNOWNCONTROL;

                        // bug 76812 - unicode token handled as normal text
                        bNextCh = false;
                        switch( nRet )
                        {
                        case RTF_UC:
                            if( 0 <= nTokenValue )
                            {
                                nUCharOverread = (sal_uInt8)nTokenValue;
                                //cmc: other ifdef breaks #i3584
                                aParserStates.top().
                                    nUCharOverread = nUCharOverread;
                            }
                            aToken.Erase(); // #i47831# erase token to prevent the token from beeing treated as text
                            // read next token
                            nRet = 0;
                            break;

                        case RTF_UPR:
                            if (!_inSkipGroup) {
                            // UPR - overread the group with the ansi
                            //       informations
                            while( '{' != _GetNextToken() )
                                ;
                            SkipGroup();
                            _GetNextToken();  // overread the last bracket
                            nRet = 0;
                            }
                            break;

                        case RTF_U:
                            if( !bRTF_InTextRead )
                            {
                                nRet = RTF_TEXTTOKEN;
                                aToken = (sal_Unicode)nTokenValue;

                                // overread the next n "RTF" characters. This
                                // can be also \{, \}, \'88
                                for( sal_uInt8 m = 0; m < nUCharOverread; ++m )
                                {
                                    sal_Unicode cAnsi = nNextCh;
                                    while( 0xD == cAnsi )
                                        cAnsi = GetNextChar();
                                    while( 0xA == cAnsi )
                                        cAnsi = GetNextChar();

                                    if( '\\' == cAnsi &&
                                        '\'' == ( cAnsi = GetNextChar() ))
                                        // HexValue ueberlesen
                                        cAnsi = GetHexValue();
                                    nNextCh = GetNextChar();
                                }
                                ScanText();
                                bNextCh = 0 == nNextCh;
                            }
                            break;
                        }
                    }
                    else if( SVPAR_PENDING != eState )
                    {
                        // Bug 34631 - "\ " ueberlesen - Blank als Zeichen
                        // eState = SVPAR_ERROR;
                        bNextCh = false;
                    }
                    break;
                }
            }
            break;

        case sal_Unicode(EOF):
            eState = SVPAR_ACCEPTED;
            nRet = nNextCh;
            break;

        case '{':
            {
                if( 0 <= nOpenBrakets )
                {
                    RtfParserState_Impl aState( nUCharOverread, GetSrcEncoding() );
                    aParserStates.push( aState );
                }
                ++nOpenBrakets;
                DBG_ASSERT(
                    static_cast<size_t>(nOpenBrakets) == aParserStates.size(),
                    "ParserStateStack unequal to bracket count" );
                nRet = nNextCh;
            }
            break;

        case '}':
            --nOpenBrakets;
            if( 0 <= nOpenBrakets )
            {
                aParserStates.pop();
                if( !aParserStates.empty() )
                {
                    const RtfParserState_Impl& rRPS =
                            aParserStates.top();
                    nUCharOverread = rRPS.nUCharOverread;
                    SetSrcEncoding( rRPS.eCodeSet );
                }
                else
                {
                    nUCharOverread = 1;
                    SetSrcEncoding( GetCodeSet() );
                }
            }
            DBG_ASSERT(
                static_cast<size_t>(nOpenBrakets) == aParserStates.size(),
                "ParserStateStack unequal to bracket count" );
            nRet = nNextCh;
            break;

        case 0x0d:
        case 0x0a:
            break;

        default:
            // es folgt normaler Text
            ScanText();
            nRet = RTF_TEXTTOKEN;
            bNextCh = 0 == nNextCh;
            break;
        }

        if( bNextCh )
            nNextCh = GetNextChar();

    } while( !nRet && SVPAR_WORKING == eState );
    return nRet;
}


sal_Unicode SvRTFParser::GetHexValue()
{
    // Hex-Wert sammeln
    register int n;
    register sal_Unicode nHexVal = 0;

    for( n = 0; n < 2; ++n )
    {
        nHexVal *= 16;
        nNextCh = GetNextChar();
        if( nNextCh >= '0' && nNextCh <= '9' )
            nHexVal += (nNextCh - 48);
        else if( nNextCh >= 'a' && nNextCh <= 'f' )
            nHexVal += (nNextCh - 87);
        else if( nNextCh >= 'A' && nNextCh <= 'F' )
            nHexVal += (nNextCh - 55);
    }
    return nHexVal;
}

void SvRTFParser::ScanText( const sal_Unicode cBreak )
{
    String aStrBuffer;
    int bWeiter = true;
    while( bWeiter && IsParserWorking() && aStrBuffer.Len() < MAX_STRING_LEN)
    {
        int bNextCh = true;
        switch( nNextCh )
        {
        case '\\':
            {
                switch (nNextCh = GetNextChar())
                {
                case '\'':
                    {

                        rtl::OStringBuffer aByteString;
                        while (1)
                        {
                            char c = (char)GetHexValue();
                            /*
                             * Note: \'00 is a valid internal character in  a
                             * string in RTF. rtl::OStringBuffer supports
                             * appending nulls fine
                             */
                            aByteString.append(c);

                            bool bBreak = false;
                            sal_Char nSlash = '\\';
                            while (!bBreak)
                            {
                                wchar_t __next=GetNextChar();
                                if (__next>0xFF) // fix for #i43933# and #i35653#
                                {
                                    if (aByteString.getLength())
                                        aStrBuffer.Append(String(rtl::OStringToOUString(aByteString.makeStringAndClear(), GetSrcEncoding())));
                                    aStrBuffer.Append((sal_Unicode)__next);

                                    continue;
                                }
                                nSlash = (sal_Char)__next;
                                while (nSlash == 0xD || nSlash == 0xA)
                                    nSlash = (sal_Char)GetNextChar();

                                switch (nSlash)
                                {
                                    case '{':
                                    case '}':
                                    case '\\':
                                        bBreak = true;
                                        break;
                                    default:
                                        aByteString.append(nSlash);
                                        break;
                                }
                            }

                            nNextCh = GetNextChar();

                            if (nSlash != '\\' || nNextCh != '\'')
                            {
                                rInput.SeekRel(-1);
                                nNextCh = nSlash;
                                break;
                            }
                        }

                        bNextCh = false;

                        if (aByteString.getLength())
                            aStrBuffer.Append(String(rtl::OStringToOUString(aByteString.makeStringAndClear(), GetSrcEncoding())));
                    }
                    break;
                case '\\':
                case '}':
                case '{':
                case '+':       // habe ich in einem RTF-File gefunden
                    aStrBuffer.Append(nNextCh);
                    break;
                case '~':       // nonbreaking space
                    aStrBuffer.Append(static_cast< sal_Unicode >(0xA0));
                    break;
                case '-':       // optional hyphen
                    aStrBuffer.Append(static_cast< sal_Unicode >(0xAD));
                    break;
                case '_':       // nonbreaking hyphen
                    aStrBuffer.Append(static_cast< sal_Unicode >(0x2011));
                    break;

                case 'u':
                    // UNI-Code Zeichen lesen
                    {
                        nNextCh = GetNextChar();
                        rInput.SeekRel( -2 );

                        if( '-' == nNextCh || RTF_ISDIGIT( nNextCh ) )
                        {
                            bRTF_InTextRead = true;

                            String sSave( aToken );
                            nNextCh = '\\';
                            #ifdef DBG_UTIL
                            int nToken =
                            #endif
                                _GetNextToken();
                            DBG_ASSERT( RTF_U == nToken, "doch kein UNI-Code Zeichen" );
                            // dont convert symbol chars
                            aStrBuffer.Append(
                                static_cast< sal_Unicode >(nTokenValue));

                            // overread the next n "RTF" characters. This
                            // can be also \{, \}, \'88
                            for( sal_uInt8 m = 0; m < nUCharOverread; ++m )
                            {
                                sal_Unicode cAnsi = nNextCh;
                                while( 0xD == cAnsi )
                                    cAnsi = GetNextChar();
                                while( 0xA == cAnsi )
                                    cAnsi = GetNextChar();

                                if( '\\' == cAnsi &&
                                    '\'' == ( cAnsi = GetNextChar() ))
                                    // HexValue ueberlesen
                                    cAnsi = GetHexValue();
                                nNextCh = GetNextChar();
                            }
                            bNextCh = false;
                            aToken = sSave;
                            bRTF_InTextRead = false;
                        }
                        else
                        {
                            nNextCh = '\\';
                            bWeiter = false;        // Abbrechen, String zusammen
                        }
                    }
                    break;

                default:
                    rInput.SeekRel( -1 );
                    nNextCh = '\\';
                    bWeiter = false;        // Abbrechen, String zusammen
                    break;
                }
            }
            break;

        case sal_Unicode(EOF):
                eState = SVPAR_ERROR;
                // weiter
        case '{':
        case '}':
            bWeiter = false;
            break;

        case 0x0a:
        case 0x0d:
            break;

        default:
            if( nNextCh == cBreak || aStrBuffer.Len() >= MAX_STRING_LEN)
                bWeiter = false;
            else
            {
                do {
                    // alle anderen Zeichen kommen in den Text
                    aStrBuffer.Append(nNextCh);

                    if (sal_Unicode(EOF) == (nNextCh = GetNextChar()))
                    {
                        if (aStrBuffer.Len())
                            aToken += aStrBuffer;
                        return;
                    }
                } while
                (
                    (RTF_ISALPHA(nNextCh) || RTF_ISDIGIT(nNextCh)) &&
                    (aStrBuffer.Len() < MAX_STRING_LEN)
                );
                bNextCh = false;
            }
        }

        if( bWeiter && bNextCh )
            nNextCh = GetNextChar();
    }

    if (aStrBuffer.Len())
        aToken += aStrBuffer;
}


short SvRTFParser::_inSkipGroup=0;

void SvRTFParser::SkipGroup()
{
short nBrackets=1;
if (_inSkipGroup>0)
    return;
_inSkipGroup++;
//#i16185# fecking \bin keyword
    do
    {
        switch (nNextCh)
        {
            case '{':
                ++nBrackets;
                break;
            case '}':
                if (!--nBrackets) {
                    _inSkipGroup--;
                    return;
                }
                break;
        }
        int nToken = _GetNextToken();
        if (nToken == RTF_BIN)
        {
            rInput.SeekRel(-1);
            rInput.SeekRel(nTokenValue);
            nNextCh = GetNextChar();
        }
        while (nNextCh==0xa || nNextCh==0xd)
        {
            nNextCh = GetNextChar();
        }
    } while (sal_Unicode(EOF) != nNextCh && IsParserWorking());

    if( SVPAR_PENDING != eState && '}' != nNextCh )
        eState = SVPAR_ERROR;
    _inSkipGroup--;
}

void SvRTFParser::ReadUnknownData() { SkipGroup(); }
void SvRTFParser::ReadBitmapData()  { SkipGroup(); }
void SvRTFParser::ReadOLEData()     { SkipGroup(); }


SvParserState SvRTFParser::CallParser()
{
    sal_Char cFirstCh;
    nNextChPos = rInput.Tell();
    rInput >> cFirstCh; nNextCh = cFirstCh;
    eState = SVPAR_WORKING;
    nOpenBrakets = 0;
    SetSrcEncoding( eCodeSet = RTL_TEXTENCODING_MS_1252 );
    eUNICodeSet = RTL_TEXTENCODING_MS_1252;     // default ist ANSI-CodeSet

    // die 1. beiden Token muessen '{' und \\rtf sein !!
    if( '{' == GetNextToken() && RTF_RTF == GetNextToken() )
    {
        AddRef();
        Continue( 0 );
        if( SVPAR_PENDING != eState )
            ReleaseRef();       // dann brauchen wir den Parser nicht mehr!
    }
    else
        eState = SVPAR_ERROR;

    return eState;
}

void SvRTFParser::Continue( int nToken )
{
//  DBG_ASSERT( SVPAR_CS_DONTKNOW == GetCharSet(),
//              "Zeichensatz wurde geaendert." );

    if( !nToken )
        nToken = GetNextToken();

    while( IsParserWorking() )
    {
        SaveState( nToken );
        switch( nToken )
        {
        case '}':
            if( nOpenBrakets )
                goto NEXTTOKEN;
            eState = SVPAR_ACCEPTED;
            break;

        case '{':
            // eine unbekannte Gruppe ?
            {
                if( RTF_IGNOREFLAG != GetNextToken() )
                    nToken = SkipToken( -1 );
                else if( RTF_UNKNOWNCONTROL != GetNextToken() )
                    nToken = SkipToken( -2 );
                else
                {
                    // gleich herausfiltern
                    ReadUnknownData();
                    nToken = GetNextToken();
                    if( '}' != nToken )
                        eState = SVPAR_ERROR;
                    break;      // auf zum naechsten Token!!
                }
            }
            goto NEXTTOKEN;

        case RTF_UNKNOWNCONTROL:
            break;      // unbekannte Token ueberspringen
        case RTF_NEXTTYPE:
        case RTF_ANSITYPE:
            SetSrcEncoding( eCodeSet = RTL_TEXTENCODING_MS_1252 );
            break;
        case RTF_MACTYPE:
            SetSrcEncoding( eCodeSet = RTL_TEXTENCODING_APPLE_ROMAN );
            break;
        case RTF_PCTYPE:
            SetSrcEncoding( eCodeSet = RTL_TEXTENCODING_IBM_437 );
            break;
        case RTF_PCATYPE:
            SetSrcEncoding( eCodeSet = RTL_TEXTENCODING_IBM_850 );
            break;
        case RTF_ANSICPG:
            eCodeSet = rtl_getTextEncodingFromWindowsCodePage(nTokenValue);
            SetSrcEncoding(eCodeSet);
            break;
        default:
NEXTTOKEN:
            NextToken( nToken );
            break;
        }
        if( IsParserWorking() )
            SaveState( 0 );         // bis hierhin abgearbeitet,
                                    // weiter mit neuem Token!
        nToken = GetNextToken();
    }
    if( SVPAR_ACCEPTED == eState && 0 < nOpenBrakets )
        eState = SVPAR_ERROR;
}

void SvRTFParser::SetEncoding( rtl_TextEncoding eEnc )
{
    if (eEnc == RTL_TEXTENCODING_DONTKNOW)
        eEnc = GetCodeSet();

    if (!aParserStates.empty())
        aParserStates.top().eCodeSet = eEnc;
    SetSrcEncoding(eEnc);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
