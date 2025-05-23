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


#include <math.h>

#include <svl/zforlist.hxx>

#include "attrib.hxx"
#include "cell.hxx"
#include "dif.hxx"
#include "docpool.hxx"
#include "document.hxx"
#include "filter.hxx"
#include "fprogressbar.hxx"
#include "ftools.hxx"
#include "patattr.hxx"
#include "scerrors.hxx"
#include "scitems.hxx"

const sal_Unicode pKeyTABLE[]   = { 'T', 'A', 'B', 'L', 'E', 0 };
const sal_Unicode pKeyVECTORS[] = { 'V', 'E', 'C', 'T', 'O', 'R', 'S', 0 };
const sal_Unicode pKeyTUPLES[]  = { 'T', 'U', 'P', 'L', 'E', 'S', 0 };
const sal_Unicode pKeyDATA[]    = { 'D', 'A', 'T', 'A', 0 };
const sal_Unicode pKeyBOT[]     = { 'B', 'O', 'T', 0 };
const sal_Unicode pKeyEOD[]     = { 'E', 'O', 'D', 0 };
const sal_Unicode pKeyERROR[]   = { 'E', 'R', 'R', 'O', 'R', 0 };
const sal_Unicode pKeyTRUE[]    = { 'T', 'R', 'U', 'E', 0 };
const sal_Unicode pKeyFALSE[]   = { 'F', 'A', 'L', 'S', 'E', 0 };
const sal_Unicode pKeyNA[]      = { 'N', 'A', 0 };
const sal_Unicode pKeyV[]       = { 'V', 0 };
const sal_Unicode pKey1_0[]     = { '1', ',', '0', 0 };


FltError ScFormatFilterPluginImpl::ScImportDif( SvStream& rIn, ScDocument* pDoc, const ScAddress& rInsPos,
                        const CharSet eVon, sal_uInt32 nDifOption )
{
    DifParser   aDifParser( rIn, nDifOption, *pDoc, eVon );

    const sal_Bool  bPlain = aDifParser.IsPlain();

    SCTAB       nBaseTab = rInsPos.Tab();

    TOPIC       eTopic = T_UNKNOWN;
    sal_Bool        bSyntErrWarn = false;
    sal_Bool        bOverflowWarn = false;

    rtl::OUString&   aData = aDifParser.aData;
    sal_Bool        bData = false;

    rIn.Seek( 0 );

    ScfStreamProgressBar aPrgrsBar( rIn, pDoc->GetDocumentShell() );

    while( eTopic != T_DATA && eTopic != T_END )
    {
        eTopic = aDifParser.GetNextTopic();

        aPrgrsBar.Progress();

        bData = !aData.isEmpty();

        switch( eTopic )
        {
            case T_TABLE:
            {
                if( aDifParser.nVector != 0 || aDifParser.nVal != 1 )
                    bSyntErrWarn = sal_True;
                if( bData )
                    pDoc->RenameTab( nBaseTab, aData );
            }
                break;
            case T_VECTORS:
            {
                if( aDifParser.nVector != 0 )
                    bSyntErrWarn = true;
            }
                break;
            case T_TUPLES:
            {
                if( aDifParser.nVector != 0 )
                    bSyntErrWarn = true;
            }
                break;
            case T_DATA:
            {
                if( aDifParser.nVector != 0 || aDifParser.nVal != 0 )
                    bSyntErrWarn = sal_True;
            }
                break;
            case T_LABEL:
            case T_COMMENT:
            case T_SIZE:
            case T_PERIODICITY:
            case T_MAJORSTART:
            case T_MINORSTART:
            case T_TRUELENGTH:
            case T_UINITS:
            case T_DISPLAYUNITS:
            case T_END:
            case T_UNKNOWN:
                break;
            default:
                OSL_FAIL( "ScImportDif - missing enum" );
        }

    }


    if( eTopic == T_DATA )
    {   // Ab hier kommen die Daten
        SCCOL               nBaseCol = rInsPos.Col();

        SCCOL               nColCnt = SCCOL_MAX;
        SCROW               nRowCnt = rInsPos.Row();
        DifAttrCache        aAttrCache( bPlain );

        DATASET             eAkt = D_UNKNOWN;

        while( eAkt != D_EOD )
            {
            eAkt = aDifParser.GetNextDataset();

            aPrgrsBar.Progress();

            switch( eAkt )
                {
                case D_BOT:
                    if( nColCnt < SCCOL_MAX )
                        nRowCnt++;
                    nColCnt = nBaseCol;
                    break;
                case D_EOD:
                    break;
                case D_NUMERIC:                 // Numbercell
                    if( nColCnt == SCCOL_MAX )
                        nColCnt = nBaseCol;

                    if( ValidCol(nColCnt) && ValidRow(nRowCnt) )
                    {
                        ScBaseCell*     pCell;
                        if( DifParser::IsV( aData.getStr() ) )
                        {
                            pCell = new ScValueCell( aDifParser.fVal );
                            if( !bPlain )
                                aAttrCache.SetNumFormat( nColCnt, nRowCnt,
                                    aDifParser.nNumFormat );
                        }
                        else if( aData == pKeyTRUE || aData == pKeyFALSE )
                        {
                            pCell = new ScValueCell( aDifParser.fVal );
                            if( bPlain )
                                aAttrCache.SetLogical( nColCnt, nRowCnt );
                            else
                                aAttrCache.SetNumFormat( nColCnt, nRowCnt,
                                    aDifParser.nNumFormat );
                        }
                        else if( aData == pKeyNA || aData == pKeyERROR  )
                            pCell = new ScStringCell( aData );
                        else
                        {
                            String aTmp( RTL_CONSTASCII_USTRINGPARAM( "#IND: " ));
                            aTmp += aData;
                            aTmp += sal_Unicode('?');
                            pCell = new ScStringCell( aTmp );
                        }

                        pDoc->PutCell( nColCnt, nRowCnt, nBaseTab, pCell, true );
                    }
                    else
                        bOverflowWarn = sal_True;

                    nColCnt++;
                    break;
                case D_STRING:                  // Textcell
                    if( nColCnt == SCCOL_MAX )
                        nColCnt = nBaseCol;

                    if( ValidCol(nColCnt) && ValidRow(nRowCnt) )
                    {
                        if (!aData.isEmpty())
                        {
                            pDoc->PutCell( nColCnt, nRowCnt, nBaseTab,
                                ScBaseCell::CreateTextCell( aData, pDoc ), true );
                        }
                    }
                    else
                        bOverflowWarn = sal_True;

                    nColCnt++;
                    break;
                case D_UNKNOWN:
                    break;
                case D_SYNT_ERROR:
                    break;
                default:
                    OSL_FAIL( "ScImportDif - missing enum" );
            }
        }

        aAttrCache.Apply( *pDoc, nBaseTab );
    }
    else
        return eERR_FORMAT;

    if( bSyntErrWarn )
        //###############################################
        // ACHTUNG: Hier fehlt noch die richtige Warnung!
        return eERR_RNGOVRFLW;
        //###############################################
    else if( bOverflowWarn )
        return eERR_RNGOVRFLW;
    else
        return eERR_OK;
}


DifParser::DifParser( SvStream& rNewIn, const sal_uInt32 nOption, ScDocument& rDoc, CharSet e ) :
    rIn( rNewIn )
{
    eCharSet = e;
    if ( rIn.GetStreamCharSet() != eCharSet )
    {
        OSL_FAIL( "CharSet passed overrides and modifies StreamCharSet" );
        rIn.SetStreamCharSet( eCharSet );
    }
    rIn.StartReadingUnicodeText( eCharSet );

    bPlain = ( nOption == SC_DIFOPT_PLAIN );

    if( bPlain )
        pNumFormatter = NULL;
    else
        pNumFormatter = rDoc.GetFormatTable();
}


TOPIC DifParser::GetNextTopic( void )
{
    enum STATE { S_VectorVal, S_Data, S_END, S_START, S_UNKNOWN, S_ERROR_L2 };

    static const sal_Unicode pKeyLABEL[]        = { 'L', 'A', 'B', 'E', 'L', 0 };
    static const sal_Unicode pKeyCOMMENT[]      = { 'C', 'O', 'M', 'M', 'E', 'N', 'T', 0 };
    static const sal_Unicode pKeySIZE[]         = { 'S', 'I', 'Z', 'E', 0 };
    static const sal_Unicode pKeyPERIODICITY[]  = { 'P', 'E', 'R', 'I', 'O', 'D', 'I', 'C', 'I', 'T', 'Y', 0 };
    static const sal_Unicode pKeyMAJORSTART[]   = { 'M', 'A', 'J', 'O', 'R', 'S', 'T', 'A', 'R', 'T', 0 };
    static const sal_Unicode pKeyMINORSTART[]   = { 'M', 'I', 'N', 'O', 'R', 'S', 'T', 'A', 'R', 'T', 0 };
    static const sal_Unicode pKeyTRUELENGTH[]   = { 'T', 'R', 'U', 'E', 'L', 'E', 'N', 'G', 'T', 'H', 0 };
    static const sal_Unicode pKeyUINITS[]       = { 'U', 'I', 'N', 'I', 'T', 'S', 0 };
    static const sal_Unicode pKeyDISPLAYUNITS[] = { 'D', 'I', 'S', 'P', 'L', 'A', 'Y', 'U', 'N', 'I', 'T', 'S', 0 };
    static const sal_Unicode pKeyUNKNOWN[]      = { 0 };

    static const sal_Unicode*   ppKeys[] =
    {
        pKeyTABLE,              // 0
        pKeyVECTORS,
        pKeyTUPLES,
        pKeyDATA,
        pKeyLABEL,
        pKeyCOMMENT,            // 5
        pKeySIZE,
        pKeyPERIODICITY,
        pKeyMAJORSTART,
        pKeyMINORSTART,
        pKeyTRUELENGTH,         // 10
        pKeyUINITS,
        pKeyDISPLAYUNITS,
        pKeyUNKNOWN             // 13
    };

    static const TOPIC      pTopics[] =
    {
        T_TABLE,                // 0
        T_VECTORS,
        T_TUPLES,
        T_DATA,
        T_LABEL,
        T_COMMENT,              // 5
        T_SIZE,
        T_PERIODICITY,
        T_MAJORSTART,
        T_MINORSTART,
        T_TRUELENGTH,           // 10
        T_UINITS,
        T_DISPLAYUNITS,
        T_UNKNOWN               // 13
    };

    STATE                   eS = S_START;
    rtl::OUString           aLine;

    nVector = 0;
    nVal = 0;
    TOPIC eRet = T_UNKNOWN;

    while( eS != S_END )
    {
        if( !ReadNextLine( aLine ) )
        {
            eS = S_END;
            eRet = T_END;
        }

        switch( eS )
        {
            case S_START:
            {
                const sal_Unicode*  pRef;
                sal_uInt16          nCnt = 0;
                sal_Bool            bSearch = sal_True;

                pRef = ppKeys[ nCnt ];

                while( bSearch )
                {
                    if( aLine == pRef )
                    {
                        eRet = pTopics[ nCnt ];
                        bSearch = false;
                    }
                    else
                    {
                        nCnt++;
                        pRef = ppKeys[ nCnt ];
                        if( !*pRef )
                            bSearch = false;
                    }
                }

                if( *pRef )
                    eS = S_VectorVal;
                else
                    eS = S_UNKNOWN;
            }
                break;
            case S_VectorVal:
            {
                const sal_Unicode*      pCur = aLine.getStr();

                pCur = ScanIntVal( pCur, nVector );

                if( pCur && *pCur == ',' )
                {
                    pCur++;
                    ScanIntVal( pCur, nVal );
                    eS = S_Data;
                }
                else
                    eS = S_ERROR_L2;
            }
                break;
            case S_Data:
                OSL_ENSURE( aLine.getLength() >= 2,
                    "+GetNextTopic(): <String> ist zu kurz!" );
                OSL_ENSURE( aLine.getLength() - 2 <= STRING_MAXLEN, "GetNextTopic(): line doesn't fit into data");
                if( aLine.getLength() > 2 )
                    aData = aLine.copy( 1, aLine.getLength() - 2 );
                else
                    aData = OUString();
                eS = S_END;
                break;
            case S_END:
                OSL_FAIL( "DifParser::GetNextTopic - unexpected state" );
                break;
            case S_UNKNOWN:
                // 2 Zeilen ueberlesen
                ReadNextLine( aLine );
            case S_ERROR_L2:                // Fehler in Line 2 aufgetreten
                // eine Zeile ueberlesen
                ReadNextLine( aLine );
                eS = S_END;
                break;
            default:
                OSL_FAIL( "DifParser::GetNextTopic - missing enum" );
        }
    }

    return eRet;
}


static void lcl_DeEscapeQuotesDif( OUString& rString )
{
    //  Special handling for DIF import: Escaped (duplicated) quotes are resolved.
    //  Single quote characters are left in place because older versions didn't
    //  escape quotes in strings (and Excel doesn't when using the clipboard).
    //  The quotes around the string are removed before this function is called.

    rString = rString.replaceAll("\"\"", "\"");
}

// Determine if passed in string is numeric data and set fVal/nNumFormat if so
DATASET DifParser::GetNumberDataset( const sal_Unicode* pPossibleNumericData )
{
    DATASET eRet = D_SYNT_ERROR;
    if( bPlain )
    {
        if( ScanFloatVal( pPossibleNumericData ) )
            eRet = D_NUMERIC;
        else
            eRet = D_SYNT_ERROR;
    }
    else
    {   // ...und zur Strafe mit'm Numberformatter...
        OSL_ENSURE( pNumFormatter, "-DifParser::GetNextDataset(): No Formatter, more fun!" );
        rtl::OUString aTestVal( pPossibleNumericData );
        sal_uInt32 nFormat = 0;
        double fTmpVal;
        if( pNumFormatter->IsNumberFormat( aTestVal, nFormat, fTmpVal ) )
        {
            fVal = fTmpVal;
            nNumFormat = nFormat;
            eRet = D_NUMERIC;
        }
        else
            eRet = D_SYNT_ERROR;
    }
    return eRet;
}

bool DifParser::ReadNextLine( rtl::OUString& rStr )
{
    if( aLookAheadLine.isEmpty() )
    {
        return rIn.ReadUniOrByteStringLine( rStr, rIn.GetStreamCharSet() );
    }
    else
    {
        rStr = aLookAheadLine;
        aLookAheadLine = rtl::OUString();
        return true;
    }
}

// Look ahead in the stream to determine if the next line is the first line of
// a valid data record structure
bool DifParser::LookAhead()
{
    const sal_Unicode* pAktBuffer;
    bool bValidStructure = false;

    OSL_ENSURE( aLookAheadLine.isEmpty(), "*DifParser::LookAhead(): LookAhead called twice in a row" );
    rIn.ReadUniOrByteStringLine( aLookAheadLine, rIn.GetStreamCharSet() );

    pAktBuffer = aLookAheadLine.getStr();

    switch( *pAktBuffer )
    {
        case '-':                   // Special Datatype
            pAktBuffer++;

            if( Is1_0( pAktBuffer ) )
            {
                bValidStructure = true;
            }
            break;
        case '0':                   // Numeric Data
            pAktBuffer++;
            if( *pAktBuffer == ',' )
            {
                pAktBuffer++;
                bValidStructure = ( GetNumberDataset(pAktBuffer) != D_SYNT_ERROR );
            }
            break;
        case '1':                   // String Data
            if( Is1_0( aLookAheadLine.getStr() ) )
            {
                bValidStructure = true;
            }
            break;
    }
    return bValidStructure;
}

DATASET DifParser::GetNextDataset( void )
{
    DATASET             eRet = D_UNKNOWN;
    rtl::OUString       aLine;
    const sal_Unicode*      pAktBuffer;

    ReadNextLine( aLine );

    pAktBuffer = aLine.getStr();

    switch( *pAktBuffer )
    {
        case '-':                   // Special Datatype
            pAktBuffer++;

            if( Is1_0( pAktBuffer ) )
            {
                ReadNextLine( aLine );
                if( IsBOT( aLine.getStr() ) )
                    eRet = D_BOT;
                else if( IsEOD( aLine.getStr() ) )
                    eRet = D_EOD;
            }
            break;
        case '0':                   // Numeric Data
            pAktBuffer++;           // Wert in fVal, 2. Zeile in aData
            if( *pAktBuffer == ',' )
            {
                pAktBuffer++;
                eRet = GetNumberDataset(pAktBuffer);
                rtl::OUString aTmpLine;
                ReadNextLine( aTmpLine );
                if ( eRet == D_SYNT_ERROR )
                {   // for broken records write "#ERR: data" to cell
                    OUStringBuffer aTmp("#ERR: ");
                    aTmp.append(pAktBuffer).append(" (");
                    aTmp.append(aTmpLine).append(')');
                    aData = aTmp.makeStringAndClear();
                    eRet = D_STRING;
                }
                else
                {
                    aData = aTmpLine;
                }
            }
            break;
        case '1':                   // String Data
            if( Is1_0( aLine.getStr() ) )
            {
                ReadNextLine( aLine );
                sal_Int32 nLineLength = aLine.getLength();
                const sal_Unicode* pLine = aLine.getStr();

                if( nLineLength >= 1 && *pLine == '"' )
                {
                    // Quotes are not always escaped (duplicated), see lcl_DeEscapeQuotesDif
                    // A look ahead into the next line is needed in order to deal with
                    // multiline strings containing quotes
                    if( LookAhead() )
                    {
                        // Single line string
                        if( nLineLength >= 2 && pLine[nLineLength - 1] == '"' )
                        {
                            aData = aLine.copy( 1, nLineLength - 2 );
                            lcl_DeEscapeQuotesDif( aData );
                            eRet = D_STRING;
                        }
                    }
                    else
                    {
                        // Multiline string
                        aData = aLine.copy( 1 );
                        bool bContinue = true;
                        while ( bContinue )
                        {
                            aData = aData + "\n";
                            bContinue = !rIn.IsEof() && ReadNextLine( aLine );
                            if( bContinue )
                            {
                                nLineLength = aLine.getLength();
                                if( nLineLength >= 1 )
                                {
                                    pLine = aLine.getStr();
                                    bContinue = !LookAhead();
                                    if( bContinue )
                                    {
                                        aData = aData + aLine;
                                    }
                                    else if( pLine[nLineLength - 1] == '"' )
                                    {
                                        aData = aData + aLine.copy(0, nLineLength -1 );
                                        lcl_DeEscapeQuotesDif( aData );
                                        eRet = D_STRING;
                                    }
                                }
                            }
                        };
                    }
                }
            }
            break;
    }

    if( eRet == D_UNKNOWN )
        ReadNextLine( aLine );

    if( rIn.IsEof() )
        eRet = D_EOD;

    return eRet;
}


const sal_Unicode* DifParser::ScanIntVal( const sal_Unicode* pStart, sal_uInt32& rRet )
{
    // eat leading whitespace, not specified, but seen in the wild
    while (*pStart == ' ' || *pStart == '\t')
        ++pStart;

    sal_Unicode     cAkt = *pStart;

    if( IsNumber( cAkt ) )
        rRet = ( sal_uInt32 ) ( cAkt - '0' );
    else
        return NULL;

    pStart++;
    cAkt = *pStart;

    while( IsNumber( cAkt ) && rRet < ( 0xFFFFFFFF / 10 ) )
    {
        rRet *= 10;
        rRet += ( sal_uInt32 ) ( cAkt - '0' );

        pStart++;
        cAkt = *pStart;
    }

    return pStart;
}


sal_Bool DifParser::ScanFloatVal( const sal_Unicode* pStart )
    {
    double                  fNewVal = 0.0;
    sal_Bool                    bNeg = false;
    double                  fFracPos = 1.0;
    sal_Int32                   nExp = 0;
    sal_Bool                    bExpNeg = false;
    sal_Bool                    bExpOverflow = false;
    static const sal_uInt16     nExpLimit = 4096;   // ACHTUNG: muss genauer ermittelt werden!

    sal_Unicode             cAkt;
    sal_Bool                    bRet = false;

    enum STATE { S_FIRST, S_PRE, S_POST, S_EXP_FIRST, S_EXP, S_END, S_FINDEND };

    STATE   eS = S_FIRST;

    fNewVal = 0.0;

    while( eS != S_END )
    {
        cAkt = *pStart;
        switch( eS )
        {
            case S_FIRST:
                if( IsNumber( cAkt ) )
                {
                    fNewVal *= 10;
                    fNewVal += cAkt - '0';
                    eS = S_PRE;
                }
                else
                {
                    switch( cAkt )
                    {
                        case ' ':
                        case '\t':
                        case '+':
                            break;
                        case '-':
                            bNeg = !bNeg;
                            break;
                        case '.':
                        case ',':                   //!
                            eS = S_POST;
                            fFracPos = 0.1;
                            break;
                        default:
                            eS = S_END;
                    }
                }
                break;
            case S_PRE:
                if( IsNumber( cAkt ) )
                {
                    fNewVal *= 10;
                    fNewVal += cAkt - '0';
                }
                else
                {
                    switch( cAkt )
                    {
                        case '.':
                        case ',':                   //!
                            eS = S_POST;
                            fFracPos = 0.1;
                            break;
                        case 'e':
                        case 'E':
                            eS = S_EXP;
                            break;
                        case 0x00:              // IsNumberEnding( cAkt )
                            bRet = sal_True;        // no
                        default:                // break!
                            eS = S_END;
                    }
                }
                break;
            case S_POST:
                if( IsNumber( cAkt ) )
                {
                    fNewVal += fFracPos * ( cAkt - '0' );
                    fFracPos /= 10.0;
                }
                else
                {
                    switch( cAkt )
                    {
                        case 'e':
                        case 'E':
                            eS = S_EXP_FIRST;
                            break;
                        case 0x00:              // IsNumberEnding( cAkt )
                            bRet = sal_True;        // no
                        default:                // break!
                            eS = S_END;
                    }
                }
                break;
            case S_EXP_FIRST:
                if( IsNumber( cAkt ) )
                {
                    if( nExp < nExpLimit )
                    {
                        nExp *= 10;
                        nExp += ( sal_uInt16 ) ( cAkt - '0' );
                    }
                    eS = S_EXP;
                }
                else
                {
                    switch( cAkt )
                    {
                        case '+':
                            break;
                        case '-':
                            bExpNeg = !bExpNeg;
                            break;
                        default:
                            eS = S_END;
                    }
                }
                break;
            case S_EXP:
                if( IsNumber( cAkt ) )
                {
                    if( nExp < ( 0xFFFF / 10 ) )
                    {
                        nExp *= 10;
                        nExp += ( sal_uInt16 ) ( cAkt - '0' );
                    }
                    else
                    {
                        bExpOverflow = sal_True;
                        eS = S_FINDEND;
                    }
                }
                else
                {
                    bRet = IsNumberEnding( cAkt );
                    eS = S_END;
                }
                break;
            case S_FINDEND:
                if( IsNumberEnding( cAkt ) )
                {
                    bRet = sal_True;        // damit sinnvoll weitergeparst werden kann
                    eS = S_END;
                }
                break;
            case S_END:
                OSL_FAIL( "DifParser::ScanFloatVal - unexpected state" );
                break;
            default:
                OSL_FAIL( "DifParser::ScanFloatVal - missing enum" );
        }
        pStart++;
    }

    if( bRet )
    {
        if( bExpOverflow )
            return false;       // ACHTUNG: hier muss noch differenziert werden

        if( bNeg )
            fNewVal *= 1.0;

        if( bExpNeg )
            nExp *= -1;

        if( nExp != 0 )
            fNewVal *= pow( 10.0, ( double ) nExp );
        fVal = fNewVal;
    }

    return bRet;
}

DifColumn::DifColumn ()
    : pAkt(NULL)
{
}

void DifColumn::SetLogical( SCROW nRow )
{
    OSL_ENSURE( ValidRow(nRow), "*DifColumn::SetLogical(): Row zu gross!" );

    if( pAkt )
    {
        OSL_ENSURE( nRow > 0, "*DifColumn::SetLogical(): weitere koennen nicht 0 sein!" );

        nRow--;

        if( pAkt->nEnd == nRow )
            pAkt->nEnd++;
        else
            pAkt = NULL;
    }
    else
    {
        pAkt = new ENTRY;
        pAkt->nStart = pAkt->nEnd = nRow;

        aEntries.push_back(pAkt);
    }
}


void DifColumn::SetNumFormat( SCROW nRow, const sal_uInt32 nNumFormat )
{
    OSL_ENSURE( ValidRow(nRow), "*DifColumn::SetNumFormat(): Row zu gross!" );

    if( nNumFormat > 0 )
    {
        if(pAkt)
        {
            OSL_ENSURE( nRow > 0,
                "*DifColumn::SetNumFormat(): weitere koennen nicht 0 sein!" );
            OSL_ENSURE( nRow > pAkt->nEnd,
                "*DifColumn::SetNumFormat(): Noch 'mal von vorne?" );

            if( pAkt->nNumFormat == nNumFormat && pAkt->nEnd == nRow - 1 )
                pAkt->nEnd = nRow;
            else
                NewEntry( nRow, nNumFormat );
        }
        else
            NewEntry(nRow,nNumFormat );
    }
    else
        pAkt = NULL;
}


void DifColumn::NewEntry( const SCROW nPos, const sal_uInt32 nNumFormat )
{
    pAkt = new ENTRY;
    pAkt->nStart = pAkt->nEnd = nPos;
    pAkt->nNumFormat = nNumFormat;

    aEntries.push_back(pAkt);
}


void DifColumn::Apply( ScDocument& rDoc, const SCCOL nCol, const SCTAB nTab, const ScPatternAttr& rPattAttr )
{
    for (boost::ptr_vector<ENTRY>::const_iterator it = aEntries.begin(); it != aEntries.end(); ++it)
        rDoc.ApplyPatternAreaTab( nCol, it->nStart, nCol, it->nEnd, nTab, rPattAttr );
}


void DifColumn::Apply( ScDocument& rDoc, const SCCOL nCol, const SCTAB nTab )
{
    ScPatternAttr aAttr( rDoc.GetPool() );
    SfxItemSet &rItemSet = aAttr.GetItemSet();

    for (boost::ptr_vector<ENTRY>::const_iterator it = aEntries.begin(); it != aEntries.end(); ++it)
    {
        OSL_ENSURE( it->nNumFormat > 0,
            "+DifColumn::Apply(): Numberformat darf hier nicht 0 sein!" );

        rItemSet.Put( SfxUInt32Item( ATTR_VALUE_FORMAT, it->nNumFormat ) );

        rDoc.ApplyPatternAreaTab( nCol, it->nStart, nCol, it->nEnd, nTab, aAttr );

        rItemSet.ClearItem();
    }
}


DifAttrCache::DifAttrCache( const sal_Bool bNewPlain )
{
    bPlain = bNewPlain;
    ppCols = new DifColumn *[ MAXCOL + 1 ];
    for( SCCOL nCnt = 0 ; nCnt <= MAXCOL ; nCnt++ )
        ppCols[ nCnt ] = NULL;
}


DifAttrCache::~DifAttrCache()
{
    for( SCCOL nCnt = 0 ; nCnt <= MAXCOL ; nCnt++ )
    {
        if( ppCols[ nCnt ] )
            delete ppCols[ nCnt ];
    }
}

void DifAttrCache::SetLogical( const SCCOL nCol, const SCROW nRow )
{
    OSL_ENSURE( ValidCol(nCol), "-DifAttrCache::SetLogical(): Col zu gross!" );
    OSL_ENSURE( bPlain, "*DifAttrCache::SetLogical(): muss Plain sein!" );

    if( !ppCols[ nCol ] )
        ppCols[ nCol ] = new DifColumn;

    ppCols[ nCol ]->SetLogical( nRow );
}

void DifAttrCache::SetNumFormat( const SCCOL nCol, const SCROW nRow, const sal_uInt32 nNumFormat )
{
    OSL_ENSURE( ValidCol(nCol), "-DifAttrCache::SetNumFormat(): Col zu gross!" );
    OSL_ENSURE( !bPlain, "*DifAttrCache::SetNumFormat(): sollte nicht Plain sein!" );

    if( !ppCols[ nCol ] )
        ppCols[ nCol ] = new DifColumn;

    ppCols[ nCol ]->SetNumFormat( nRow, nNumFormat );
}


void DifAttrCache::Apply( ScDocument& rDoc, SCTAB nTab )
{
    if( bPlain )
    {
        ScPatternAttr*  pPatt = NULL;

        for( SCCOL nCol = 0 ; nCol <= MAXCOL ; nCol++ )
        {
            if( ppCols[ nCol ] )
            {
                if( !pPatt )
                {
                    pPatt = new ScPatternAttr( rDoc.GetPool() );
                    pPatt->GetItemSet().Put( SfxUInt32Item( ATTR_VALUE_FORMAT,
                        rDoc.GetFormatTable()->GetStandardFormat( NUMBERFORMAT_LOGICAL ) ) );
                }

                ppCols[ nCol ]->Apply( rDoc, nCol, nTab, *pPatt );
            }
        }

        if( pPatt )
            delete pPatt;
    }
    else
    {
        for( SCCOL nCol = 0 ; nCol <= MAXCOL ; nCol++ )
        {
            if( ppCols[ nCol ] )
                ppCols[ nCol ]->Apply( rDoc, nCol, nTab );
        }
    }
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
