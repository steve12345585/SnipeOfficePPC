/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#include "sal/config.h"

#include "export.hxx"
#include <stdio.h>
#include <osl/time.h>
#include <osl/process.h>
#include <rtl/strbuf.hxx>
#include <rtl/ustring.hxx>
#include <sal/macros.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <stdlib.h>

//flags for handleArguments()
#define STATE_NON       0x0001
#define STATE_INPUT     0x0002
#define STATE_OUTPUT    0x0003
#define STATE_PRJ       0x0004
#define STATE_ROOT      0x0005
#define STATE_MERGESRC  0x0006
#define STATE_LANGUAGES 0x0007

//
// class ResData();
//

/*****************************************************************************/
ResData::~ResData()
/*****************************************************************************/
{
    if ( pStringList ) {
        // delete existing res. of type StringList
        for ( size_t i = 0; i < pStringList->size(); i++ ) {
            ExportListEntry* test = (*pStringList)[ i ];
            if( test != NULL ) delete test;
        }
        delete pStringList;
    }
    if ( pFilterList ) {
        // delete existing res. of type FilterList
        for ( size_t i = 0; i < pFilterList->size(); i++ ) {
            ExportListEntry* test = (*pFilterList)[ i ];
            delete test;
        }
        delete pFilterList;
    }
    if ( pItemList ) {
        // delete existing res. of type ItemList
        for ( size_t i = 0; i < pItemList->size(); i++ ) {
            ExportListEntry* test = (*pItemList)[ i ];
            delete test;
        }
        delete pItemList;
    }
    if ( pUIEntries ) {
        // delete existing res. of type UIEntries
        for ( size_t i = 0; i < pUIEntries->size(); i++ ) {
            ExportListEntry* test = (*pUIEntries)[ i ];
            delete test;
        }
        delete pUIEntries;
    }
}

//
// class Export
//

/*****************************************************************************/
rtl::OString Export::sLanguages;
rtl::OString Export::sForcedLanguages;
/*****************************************************************************/

bool Export::handleArguments(
    int argc, char * argv[], HandledArgs& o_aHandledArgs)
{
    o_aHandledArgs = HandledArgs();
    sLanguages = "";
    sal_uInt16 nState = STATE_NON;

    for( int i = 1; i < argc; i++ )
    {
        if ( OString( argv[ i ] ).toAsciiUpperCase() == "-I" )
        {
            nState = STATE_INPUT; // next token specifies source file
        }
        else if ( OString( argv[ i ] ).toAsciiUpperCase() == "-O" )
        {
            nState = STATE_OUTPUT; // next token specifies the dest file
        }
        else if ( OString( argv[ i ] ).toAsciiUpperCase() == "-P" )
        {
            nState = STATE_PRJ; // next token specifies the cur. project
        }
        else if ( OString( argv[ i ] ).toAsciiUpperCase() == "-R" )
        {
            nState = STATE_ROOT; // next token specifies path to project root
        }
        else if ( OString( argv[ i ] ).toAsciiUpperCase() == "-M" )
        {
            nState = STATE_MERGESRC; // next token specifies the merge database
        }
        else if ( OString( argv[ i ] ).toAsciiUpperCase() == "-L" )
        {
            nState = STATE_LANGUAGES;
        }
        else
        {
            switch ( nState )
            {
                case STATE_NON:
                {
                    return false;    // no valid command line
                }
                case STATE_INPUT:
                {
                    o_aHandledArgs.m_sInputFile = OString( argv[i] );
                }
                break;
                case STATE_OUTPUT:
                {
                    o_aHandledArgs.m_sOutputFile = OString( argv[i] );
                }
                break;
                case STATE_PRJ:
                {
                    o_aHandledArgs.m_sPrj = OString( argv[i] );
                }
                break;
                case STATE_ROOT:
                {
                    o_aHandledArgs.m_sPrjRoot = OString( argv[i] );
                }
                break;
                case STATE_MERGESRC:
                {
                    o_aHandledArgs.m_sMergeSrc = OString( argv[i] );
                    o_aHandledArgs.m_bMergeMode = true;
                }
                break;
                case STATE_LANGUAGES:
                {
                    sLanguages = OString( argv[i] );
                }
                break;
            }
        }
    }
    if( !o_aHandledArgs.m_sInputFile.isEmpty() &&
        !o_aHandledArgs.m_sOutputFile.isEmpty() )
    {
        return true;
    }
    else
    {
        o_aHandledArgs = HandledArgs();
        return false;
    }
}

void Export::writeUsage(const OString& rName, const OString& rFileType)
{
    std::cout
        << "Syntax: " << rName.getStr()
        << " [-p Prj] [-r PrjRoot] -i FileIn -o FileOut"
        << " [-m DataBase] [-l l1,l2,...]\n"
        << " Prj:      Project\n"
        << " PrjRoot:  Path to project root (../.. etc.)\n"
        << " FileIn:   Source files (*." << rFileType.getStr() << ")\n"
        << " FileOut:  Destination file (*.*)\n"
        << " DataBase: Mergedata (*.po)\n"
        << " -l: Restrict the handled languages; l1, l2, ... are elements of"
        << " (de, en-US, ...)\n";
}

/*****************************************************************************/
void Export::SetLanguages( std::vector<rtl::OString> val ){
/*****************************************************************************/
    aLanguages = val;
    isInitialized = true;
}
/*****************************************************************************/
std::vector<rtl::OString> Export::GetLanguages(){
/*****************************************************************************/
    return aLanguages;
}
/*****************************************************************************/
std::vector<rtl::OString> Export::GetForcedLanguages(){
/*****************************************************************************/
    return aForcedLanguages;
}
std::vector<rtl::OString> Export::aLanguages       = std::vector<rtl::OString>();
std::vector<rtl::OString> Export::aForcedLanguages = std::vector<rtl::OString>();

/*****************************************************************************/
rtl::OString Export::QuoteHTML( rtl::OString const &rString )
/*****************************************************************************/
{
    rtl::OStringBuffer sReturn;
    for ( sal_Int32 i = 0; i < rString.getLength(); i++ ) {
        rtl::OString sTemp = rString.copy( i );
        if ( sTemp.match( "<Arg n=" ) ) {
            while ( i < rString.getLength() && rString[i] != '>' ) {
                 sReturn.append(rString[i]);
                i++;
            }
            if ( rString[i] == '>' ) {
                sReturn.append('>');
                i++;
            }
        }
        if ( i < rString.getLength()) {
            switch ( rString[i]) {
                case '<':
                    sReturn.append("&lt;");
                break;

                case '>':
                    sReturn.append("&gt;");
                break;

                case '\"':
                    sReturn.append("&quot;");
                break;

                case '\'':
                    sReturn.append("&apos;");
                break;

                case '&':
                    if ((( i + 4 ) < rString.getLength()) &&
                        ( rString.copy( i, 5 ) == "&amp;" ))
                            sReturn.append(rString[i]);
                    else
                        sReturn.append("&amp;");
                break;

                default:
                    sReturn.append(rString[i]);
                break;
            }
        }
    }
    return sReturn.makeStringAndClear();
}

void Export::RemoveUTF8ByteOrderMarker( rtl::OString &rString )
{
    if( hasUTF8ByteOrderMarker( rString ) )
        rString = rString.copy(3);
}

bool Export::hasUTF8ByteOrderMarker( const rtl::OString &rString )
{
    return rString.getLength() >= 3 && rString[0] == '\xEF' &&
           rString[1] == '\xBB' && rString[2] == '\xBF' ;
}

/*****************************************************************************/
rtl::OString Export::UnquoteHTML( rtl::OString const &rString )
/*****************************************************************************/
{
    rtl::OStringBuffer sReturn;
    for (sal_Int32 i = 0; i != rString.getLength();) {
        if (rString.match("&amp;", i)) {
            sReturn.append('&');
            i += RTL_CONSTASCII_LENGTH("&amp;");
        } else if (rString.match("&lt;", i)) {
            sReturn.append('<');
            i += RTL_CONSTASCII_LENGTH("&lt;");
        } else if (rString.match("&gt;", i)) {
            sReturn.append('>');
            i += RTL_CONSTASCII_LENGTH("&gt;");
        } else if (rString.match("&quot;", i)) {
            sReturn.append('"');
            i += RTL_CONSTASCII_LENGTH("&quot;");
        } else if (rString.match("&apos;", i)) {
            sReturn.append('\'');
            i += RTL_CONSTASCII_LENGTH("&apos;");
        } else {
            sReturn.append(rString[i]);
            ++i;
        }
    }
    return sReturn.makeStringAndClear();
}

bool Export::isSourceLanguage(const rtl::OString &rLanguage)
{
    return !isAllowed(rLanguage);
}

bool Export::isAllowed(const rtl::OString &rLanguage)
{
    return !rLanguage.equalsIgnoreAsciiCaseL(RTL_CONSTASCII_STRINGPARAM("en-US"));
}

bool Export::isInitialized = false;

/*****************************************************************************/
void Export::InitLanguages( bool bMergeMode ){
/*****************************************************************************/
    if( !isInitialized )
    {
        rtl::OString sTmp;
        OStringBoolHashMap aEnvLangs;

        sal_Int32 nIndex = 0;
        do
        {
            rtl::OString aToken = sLanguages.getToken(0, ',', nIndex);
            sTmp = aToken.getToken(0, '=').trim();
            if( bMergeMode && !isAllowed( sTmp ) ){}
            else if( !( (sTmp[0]=='x' || sTmp[0]=='X') && sTmp[1]=='-' ) ){
                aLanguages.push_back( sTmp );
            }
        }
        while ( nIndex >= 0 );

        InitForcedLanguages( bMergeMode );
        isInitialized = true;
    }
}
/*****************************************************************************/
void Export::InitForcedLanguages( bool bMergeMode ){
/*****************************************************************************/
    rtl::OString sTmp;
    OStringBoolHashMap aEnvLangs;

    sal_Int32 nIndex = 0;
    do
    {
        rtl::OString aToken = sForcedLanguages.getToken(0, ',', nIndex);

        sTmp = aToken.getToken(0, '=').trim();
        if( bMergeMode && isAllowed( sTmp ) ){}
        else if( !( (sTmp[0]=='x' || sTmp[0]=='X') && sTmp[1]=='-' ) )
            aForcedLanguages.push_back( sTmp );
    }
    while ( nIndex >= 0 );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
