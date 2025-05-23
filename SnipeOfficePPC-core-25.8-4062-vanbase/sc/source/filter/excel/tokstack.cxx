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

#include <string.h>

#include "compiler.hxx"
#include "tokstack.hxx"
#include "global.hxx"
#include "scmatrix.hxx"

#include <stdio.h> // printf

const sal_uInt16    TokenPool::nScTokenOff = 8192;


TokenStack::TokenStack( sal_uInt16 nNewSize )
{
    pStack = new TokenId[ nNewSize ];

    Reset();
    nSize = nNewSize;
}


TokenStack::~TokenStack()
{
    delete[] pStack;
}




//------------------------------------------------------------------------

// !ACHTUNG!: nach Aussen hin beginnt die Nummerierung mit 1!
// !ACHTUNG!: SC-Token werden mit einem Offset nScTokenOff abgelegt
//              -> Unterscheidung von anderen Token


TokenPool::TokenPool( void )
{
    sal_uInt16  nLauf = nScTokenOff;

    // Sammelstelle fuer Id-Folgen
    nP_Id = 256;
    pP_Id = new sal_uInt16[ nP_Id ];

    // Sammelstelle fuer Ids
    nElement = 32;
    pElement = new sal_uInt16[ nElement ];
    pType = new E_TYPE[ nElement ];
    pSize = new sal_uInt16[ nElement ];
    nP_IdLast = 0;

    // Sammelstelle fuer Strings
    nP_Str = 4;
    ppP_Str = new String *[ nP_Str ];
    for( nLauf = 0 ; nLauf < nP_Str ; nLauf++ )
        ppP_Str[ nLauf ] = NULL;

    // Sammelstelle fuer double
    nP_Dbl = 8;
    pP_Dbl = new double[ nP_Dbl ];

    // Sammelstelle fuer error codes
    nP_Err = 8;
    pP_Err = new sal_uInt16[ nP_Err ];

    // Sammelstellen fuer Referenzen
    nP_RefTr = 32;
    ppP_RefTr = new ScSingleRefData *[ nP_RefTr ];
    for( nLauf = 0 ; nLauf < nP_RefTr ; nLauf++ )
        ppP_RefTr[ nLauf ] = NULL;

    nP_Ext = 32;
    ppP_Ext = new EXTCONT*[ nP_Ext ];
    memset( ppP_Ext, 0, sizeof( EXTCONT* ) * nP_Ext );

    nP_Nlf = 16;
    ppP_Nlf = new NLFCONT*[ nP_Nlf ];
    memset( ppP_Nlf, 0, sizeof( NLFCONT* ) * nP_Nlf );

    nP_Matrix = 16;
    ppP_Matrix = new ScMatrix*[ nP_Matrix ];
    memset( ppP_Matrix, 0, sizeof( ScMatrix* ) * nP_Matrix );

    pScToken = new ScTokenArray;

    Reset();
}


TokenPool::~TokenPool()
{
    sal_uInt16 n;

    delete[] pP_Id;
    delete[] pElement;
    delete[] pType;
    delete[] pSize;
    delete[] pP_Dbl;
    delete[] pP_Err;

    for( n = 0 ; n < nP_RefTr ; n++ )
        delete ppP_RefTr[ n ];
    delete[] ppP_RefTr;

    for( n = 0 ; n < nP_Str ; n++ )
        delete ppP_Str[ n ];
    delete[] ppP_Str;

    for( n = 0 ; n < nP_Ext ; n++ )
        delete ppP_Ext[ n ];
    delete[] ppP_Ext;

    for( n = 0 ; n < nP_Nlf ; n++ )
        delete ppP_Nlf[ n ];
    delete[] ppP_Nlf;

    for( n = 0 ; n < nP_Matrix ; n++ )
    {
        if( ppP_Matrix[ n ] )
            ppP_Matrix[ n ]->DecRef( );
    }
    delete[] ppP_Matrix;

    delete pScToken;
}


/** Returns the new number of elements, or 0 if overflow. */
static sal_uInt16 lcl_canGrow( sal_uInt16 nOld, sal_uInt16 nByMin = 1 )
{
    if (!nOld)
        return nByMin ? nByMin : 1;
    if (nOld == SAL_MAX_UINT16)
        return 0;
    sal_uInt32 nNew = ::std::max( static_cast<sal_uInt32>(nOld) * 2,
            static_cast<sal_uInt32>(nOld) + nByMin);
    if (nNew > SAL_MAX_UINT16)
        nNew = SAL_MAX_UINT16;
    if (nNew - nByMin < nOld)
        nNew = 0;
    return static_cast<sal_uInt16>(nNew);
}


bool TokenPool::GrowString( void )
{
    sal_uInt16 nP_StrNew = lcl_canGrow( nP_Str);
    if (!nP_StrNew)
        return false;

    sal_uInt16      nL;

    String** ppP_StrNew = new (::std::nothrow) String *[ nP_StrNew ];
    if (!ppP_StrNew)
        return false;

    for( nL = 0 ; nL < nP_Str ; nL++ )
        ppP_StrNew[ nL ] = ppP_Str[ nL ];
    for( nL = nP_Str ; nL < nP_StrNew ; nL++ )
        ppP_StrNew[ nL ] = NULL;

    nP_Str = nP_StrNew;

    delete[]    ppP_Str;
    ppP_Str = ppP_StrNew;
    return true;
}


bool TokenPool::GrowDouble( void )
{
    sal_uInt16 nP_DblNew = lcl_canGrow( nP_Dbl);
    if (!nP_DblNew)
        return false;


    double* pP_DblNew = new (::std::nothrow) double[ nP_DblNew ];
    if (!pP_DblNew)
        return false;

    for( sal_uInt16 nL = 0 ; nL < nP_Dbl ; nL++ )
        pP_DblNew[ nL ] = pP_Dbl[ nL ];

    nP_Dbl = nP_DblNew;

    delete[] pP_Dbl;
    pP_Dbl = pP_DblNew;
    return true;
}


/* TODO: in case we had FormulaTokenArray::AddError() */
#if 0
void TokenPool::GrowError( void )
{
    sal_uInt16 nP_ErrNew = lcl_canGrow( nP_Err);
    if (!nP_ErrNew)
        return false;


    sal_uInt16*     pP_ErrNew = new (::std::nothrow) sal_uInt16[ nP_ErrNew ];
    if (!pP_ErrNew)
        return false;

    for( sal_uInt16 nL = 0 ; nL < nP_Err ; nL++ )
        pP_ErrNew[ nL ] = pP_Err[ nL ];

    nP_Err = nP_ErrNew;

    delete[] pP_Err;
    pP_Err = pP_ErrNew;
    return true;
}
#endif


bool TokenPool::GrowTripel( sal_uInt16 nByMin )
{
    sal_uInt16 nP_RefTrNew = lcl_canGrow( nP_RefTr, nByMin);
    if (!nP_RefTrNew)
        return false;

    sal_uInt16          nL;

    ScSingleRefData** ppP_RefTrNew = new (::std::nothrow) ScSingleRefData *[ nP_RefTrNew ];
    if (!ppP_RefTrNew)
        return false;

    for( nL = 0 ; nL < nP_RefTr ; nL++ )
        ppP_RefTrNew[ nL ] = ppP_RefTr[ nL ];
    for( nL = nP_RefTr ; nL < nP_RefTrNew ; nL++ )
        ppP_RefTrNew[ nL ] = NULL;

    nP_RefTr = nP_RefTrNew;

    delete[] ppP_RefTr;
    ppP_RefTr = ppP_RefTrNew;
    return true;
}


bool TokenPool::GrowId( void )
{
    sal_uInt16 nP_IdNew = lcl_canGrow( nP_Id);
    if (!nP_IdNew)
        return false;


    sal_uInt16* pP_IdNew = new (::std::nothrow) sal_uInt16[ nP_IdNew ];
    if (!pP_IdNew)
        return false;

    for( sal_uInt16 nL = 0 ; nL < nP_Id ; nL++ )
        pP_IdNew[ nL ] = pP_Id[ nL ];

    nP_Id = nP_IdNew;

    delete[] pP_Id;
    pP_Id = pP_IdNew;
    return true;
}


bool TokenPool::GrowElement( void )
{
    sal_uInt16 nElementNew = lcl_canGrow( nElement);
    if (!nElementNew)
        return false;


    sal_uInt16* pElementNew = new (::std::nothrow) sal_uInt16[ nElementNew ];
    E_TYPE* pTypeNew = new (::std::nothrow) E_TYPE[ nElementNew ];
    sal_uInt16* pSizeNew = new (::std::nothrow) sal_uInt16[ nElementNew ];
    if (!pElementNew || !pTypeNew || !pSizeNew)
    {
        delete [] pElementNew;
        delete [] pTypeNew;
        delete [] pSizeNew;
        return false;
    }

    for( sal_uInt16 nL = 0 ; nL < nElement ; nL++ )
    {
        pElementNew[ nL ] = pElement[ nL ];
        pTypeNew[ nL ] = pType[ nL ];
        pSizeNew[ nL ] = pSize[ nL ];
    }

    nElement = nElementNew;

    delete[] pElement;
    delete[] pType;
    delete[] pSize;
    pElement = pElementNew;
    pType = pTypeNew;
    pSize = pSizeNew;
    return true;
}


bool TokenPool::GrowExt( void )
{
    sal_uInt16 nNewSize = lcl_canGrow( nP_Ext);
    if (!nNewSize)
        return false;

    EXTCONT** ppNew = new (::std::nothrow) EXTCONT*[ nNewSize ];
    if (!ppNew)
        return false;

    memset( ppNew, 0, sizeof( EXTCONT* ) * nNewSize );
    memcpy( ppNew, ppP_Ext, sizeof( EXTCONT* ) * nP_Ext );

    delete[] ppP_Ext;
    ppP_Ext = ppNew;
    nP_Ext = nNewSize;
    return true;
}


bool TokenPool::GrowNlf( void )
{
    sal_uInt16 nNewSize = lcl_canGrow( nP_Nlf);
    if (!nNewSize)
        return false;

    NLFCONT** ppNew = new (::std::nothrow) NLFCONT*[ nNewSize ];
    if (!ppNew)
        return false;

    memset( ppNew, 0, sizeof( NLFCONT* ) * nNewSize );
    memcpy( ppNew, ppP_Nlf, sizeof( NLFCONT* ) * nP_Nlf );

    delete[] ppP_Nlf;
    ppP_Nlf = ppNew;
    nP_Nlf = nNewSize;
    return true;
}


bool TokenPool::GrowMatrix( void )
{
    sal_uInt16 nNewSize = lcl_canGrow( nP_Matrix);
    if (!nNewSize)
        return false;

    ScMatrix**  ppNew = new (::std::nothrow) ScMatrix*[ nNewSize ];
    if (!ppNew)
        return false;

    memset( ppNew, 0, sizeof( ScMatrix* ) * nNewSize );
    memcpy( ppNew, ppP_Matrix, sizeof( ScMatrix* ) * nP_Matrix );

    delete[] ppP_Matrix;
    ppP_Matrix = ppNew;
    nP_Matrix = nNewSize;
    return true;
}

bool TokenPool::GetElement( const sal_uInt16 nId )
{
    OSL_ENSURE( nId < nElementAkt, "*TokenPool::GetElement(): Id too large!?" );
    if (nId >= nElementAkt)
        return false;

    bool bRet = true;
    if( pType[ nId ] == T_Id )
        bRet = GetElementRek( nId );
    else
    {
        switch( pType[ nId ] )
        {
            case T_Str:
                {
                    sal_uInt16 n = pElement[ nId ];
                    String* p = ( n < nP_Str )? ppP_Str[ n ] : NULL;
                    if (p)
                        pScToken->AddString( *p );
                    else
                        bRet = false;
                }
                break;
            case T_D:
                {
                    sal_uInt16 n = pElement[ nId ];
                    if (n < nP_Dbl)
                        pScToken->AddDouble( pP_Dbl[ n ] );
                    else
                        bRet = false;
                }
                break;
            case T_Err:
/* TODO: in case we had FormulaTokenArray::AddError() */
#if 0
                {
                    sal_uInt16 n = pElement[ nId ];
                    if (n < nP_Err)
                        pScToken->AddError( pP_Err[ n ] );
                    else
                        bRet = false;
                }
#endif
                break;
            case T_RefC:
                {
                    sal_uInt16 n = pElement[ nId ];
                    ScSingleRefData* p = ( n < nP_RefTr )? ppP_RefTr[ n ] : NULL;
                    if (p)
                        pScToken->AddSingleReference( *p );
                    else
                        bRet = false;
                }
                break;
            case T_RefA:
                {
                    sal_uInt16 n = pElement[ nId ];
                    if (n < nP_RefTr && ppP_RefTr[ n ] && n+1 < nP_RefTr && ppP_RefTr[ n + 1 ])
                    {
                        ScComplexRefData aScComplexRefData;
                        aScComplexRefData.Ref1 = *ppP_RefTr[ n ];
                        aScComplexRefData.Ref2 = *ppP_RefTr[ n + 1 ];
                        pScToken->AddDoubleReference( aScComplexRefData );
                    }
                    else
                        bRet = false;
                }
                break;
            case T_RN:
            {
                sal_uInt16 n = pElement[nId];
                if (n < maRangeNames.size())
                {
                    const RangeName& r = maRangeNames[n];
                    pScToken->AddRangeName(r.mnIndex, r.mbGlobal);
                }
            }
            break;
            case T_Ext:
                {
                    sal_uInt16      n = pElement[ nId ];
                    EXTCONT*        p = ( n < nP_Ext )? ppP_Ext[ n ] : NULL;

                    if( p )
                    {
                        if( p->eId == ocEuroConvert )
                            pScToken->AddOpCode( p->eId );
                        else
                            pScToken->AddExternal( p->aText, p->eId );
                    }
                    else
                        bRet = false;
                }
                break;
            case T_Nlf:
                {
                    sal_uInt16      n = pElement[ nId ];
                    NLFCONT*        p = ( n < nP_Nlf )? ppP_Nlf[ n ] : NULL;

                    if( p )
                        pScToken->AddColRowName( p->aRef );
                    else
                        bRet = false;
                }
                break;
            case T_Matrix:
                {
                    sal_uInt16      n = pElement[ nId ];
                    ScMatrix*       p = ( n < nP_Matrix )? ppP_Matrix[ n ] : NULL;

                    if( p )
                        pScToken->AddMatrix( p );
                    else
                        bRet = false;
                }
                break;
            case T_ExtName:
            {
                sal_uInt16 n = pElement[nId];
                if (n < maExtNames.size())
                {
                    const ExtName& r = maExtNames[n];
                    pScToken->AddExternalName(r.mnFileId, r.maName);
                }
                else
                    bRet = false;
            }
            break;
            case T_ExtRefC:
            {
                sal_uInt16 n = pElement[nId];
                if (n < maExtCellRefs.size())
                {
                    const ExtCellRef& r = maExtCellRefs[n];
                    pScToken->AddExternalSingleReference(r.mnFileId, r.maTabName, r.maRef);
                }
                else
                    bRet = false;
            }
            break;
            case T_ExtRefA:
            {
                sal_uInt16 n = pElement[nId];
                if (n < maExtAreaRefs.size())
                {
                    const ExtAreaRef& r = maExtAreaRefs[n];
                    pScToken->AddExternalDoubleReference(r.mnFileId, r.maTabName, r.maRef);
                }
                else
                    bRet = false;
            }
            break;
            default:
                OSL_FAIL("-TokenPool::GetElement(): undefined state!?");
                bRet = false;
        }
    }
    return bRet;
}


bool TokenPool::GetElementRek( const sal_uInt16 nId )
{
#ifdef DBG_UTIL
    m_nRek++;
    OSL_ENSURE(m_nRek <= nP_Id, "*TokenPool::GetElement(): recursion loops!?");
#endif

    OSL_ENSURE( nId < nElementAkt, "*TokenPool::GetElementRek(): nId >= nElementAkt" );

    if (nId >= nElementAkt)
    {
        DBG_ERRORFILE( "*TokenPool::GetElementRek(): nId >= nElementAkt" );
#ifdef DBG_UTIL
        m_nRek--;
#endif
        return false;
    }

    if (pType[ nId ] != T_Id)
    {
        DBG_ERRORFILE( "-TokenPool::GetElementRek(): pType[ nId ] != T_Id" );
#ifdef DBG_UTIL
        m_nRek--;
#endif
        return false;
    }


    bool bRet = true;
    sal_uInt16      nAnz = pSize[ nId ];
    sal_uInt16 nFirstId = pElement[ nId ];
    if (nFirstId >= nP_Id)
    {
        DBG_ERRORFILE( "TokenPool::GetElementRek: nFirstId >= nP_Id");
        nAnz = 0;
        bRet = false;
    }
    sal_uInt16* pAkt = nAnz ? &pP_Id[ nFirstId ] : NULL;
    if (nAnz > nP_Id - nFirstId)
    {
        DBG_ERRORFILE( "TokenPool::GetElementRek: nAnz > nP_Id - nFirstId");
        nAnz = nP_Id - nFirstId;
        bRet = false;
    }
    for( ; nAnz > 0 ; nAnz--, pAkt++ )
    {
        if( *pAkt < nScTokenOff )
        {// Rekursion oder nicht?
            if (*pAkt >= nElementAkt)
            {
                DBG_ERRORFILE( "TokenPool::GetElementRek: *pAkt >= nElementAkt");
                bRet = false;
            }
            else
            {
                if (pType[ *pAkt ] == T_Id)
                    bRet = GetElementRek( *pAkt );
                else
                    bRet = GetElement( *pAkt );
            }
        }
        else    // elementarer SC_Token
            pScToken->AddOpCode( ( DefTokenId ) ( *pAkt - nScTokenOff ) );
    }


#ifdef DBG_UTIL
    m_nRek--;
#endif
    return bRet;
}


void TokenPool::operator >>( TokenId& rId )
{
    rId = ( TokenId ) ( nElementAkt + 1 );

    if( nElementAkt >= nElement )
        if (!GrowElement())
            return;

    pElement[ nElementAkt ] = nP_IdLast;    // Start der Token-Folge
    pType[ nElementAkt ] = T_Id;            // Typinfo eintragen
    pSize[ nElementAkt ] = nP_IdAkt - nP_IdLast;
        // von nP_IdLast bis nP_IdAkt-1 geschrieben -> Laenge der Folge

    nElementAkt++;          // Startwerte fuer naechste Folge
    nP_IdLast = nP_IdAkt;
}


const TokenId TokenPool::Store( const double& rDouble )
{
    if( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;

    if( nP_DblAkt >= nP_Dbl )
        if (!GrowDouble())
            return (const TokenId) nElementAkt+1;

    pElement[ nElementAkt ] = nP_DblAkt;    // Index in Double-Array
    pType[ nElementAkt ] = T_D;             // Typinfo Double eintragen

    pP_Dbl[ nP_DblAkt ] = rDouble;

    pSize[ nElementAkt ] = 1;           // eigentlich Banane

    nElementAkt++;
    nP_DblAkt++;

    return ( const TokenId ) nElementAkt; // Ausgabe von altem Wert + 1!
}


const TokenId TokenPool::Store( const sal_uInt16 nIndex )
{
    return StoreName(nIndex, true);
}


const TokenId TokenPool::Store( const String& rString )
{
    // weitgehend nach Store( const sal_Char* ) kopiert, zur Vermeidung
    //  eines temporaeren Strings in "
    if( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;


    if( nP_StrAkt >= nP_Str )
        if (!GrowString())
            return (const TokenId) nElementAkt+1;


    pElement[ nElementAkt ] = nP_StrAkt;    // Index in String-Array
    pType[ nElementAkt ] = T_Str;           // Typinfo String eintragen

    // String anlegen
    if( !ppP_Str[ nP_StrAkt ] )
        //...aber nur, wenn noch nicht vorhanden
        ppP_Str[ nP_StrAkt ] = new (::std::nothrow) String( rString );
    else
        //...ansonsten nur kopieren
        *ppP_Str[ nP_StrAkt ] = rString;

    if (ppP_Str[ nP_StrAkt ])
    {
        DBG_ASSERT( sizeof( xub_StrLen ) <= 2, "*TokenPool::Store(): StrLen doesn't match!" );
        pSize[ nElementAkt ] = ( sal_uInt16 ) ppP_Str[ nP_StrAkt ]->Len();
    }

    nElementAkt++;
    nP_StrAkt++;

    return ( const TokenId ) nElementAkt; // Ausgabe von altem Wert + 1!
}


const TokenId TokenPool::Store( const ScSingleRefData& rTr )
{
    if( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;

    if( nP_RefTrAkt >= nP_RefTr )
        if (!GrowTripel())
            return (const TokenId) nElementAkt+1;

    pElement[ nElementAkt ] = nP_RefTrAkt;
    pType[ nElementAkt ] = T_RefC;          // Typinfo Cell-Reff eintragen

    if( !ppP_RefTr[ nP_RefTrAkt ] )
        ppP_RefTr[ nP_RefTrAkt ] = new ScSingleRefData( rTr );
    else
        *ppP_RefTr[ nP_RefTrAkt ] = rTr;

    nElementAkt++;
    nP_RefTrAkt++;

    return ( const TokenId ) nElementAkt; // Ausgabe von altem Wert + 1!
}


const TokenId TokenPool::Store( const ScComplexRefData& rTr )
{
    if( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;

    if( nP_RefTrAkt + 1 >= nP_RefTr )
        if (!GrowTripel( 2))
            return (const TokenId) nElementAkt+1;

    pElement[ nElementAkt ] = nP_RefTrAkt;
    pType[ nElementAkt ] = T_RefA;          // Typinfo Area-Reff eintragen

    if( !ppP_RefTr[ nP_RefTrAkt ] )
        ppP_RefTr[ nP_RefTrAkt ] = new ScSingleRefData( rTr.Ref1 );
    else
        *ppP_RefTr[ nP_RefTrAkt ] = rTr.Ref1;
    nP_RefTrAkt++;

    if( !ppP_RefTr[ nP_RefTrAkt ] )
        ppP_RefTr[ nP_RefTrAkt ] = new ScSingleRefData( rTr.Ref2 );
    else
        *ppP_RefTr[ nP_RefTrAkt ] = rTr.Ref2;
    nP_RefTrAkt++;

    nElementAkt++;

    return ( const TokenId ) nElementAkt; // Ausgabe von altem Wert + 1!
}


const TokenId TokenPool::Store( const DefTokenId e, const String& r )
{
    if( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;

    if( nP_ExtAkt >= nP_Ext )
        if (!GrowExt())
            return (const TokenId) nElementAkt+1;

    pElement[ nElementAkt ] = nP_ExtAkt;
    pType[ nElementAkt ] = T_Ext;           // Typinfo String eintragen

    if( ppP_Ext[ nP_ExtAkt ] )
    {
        ppP_Ext[ nP_ExtAkt ]->eId = e;
        ppP_Ext[ nP_ExtAkt ]->aText = r;
    }
    else
        ppP_Ext[ nP_ExtAkt ] = new EXTCONT( e, r );

    nElementAkt++;
    nP_ExtAkt++;

    return ( const TokenId ) nElementAkt; // Ausgabe von altem Wert + 1!
}


const TokenId TokenPool::StoreNlf( const ScSingleRefData& rTr )
{
    if( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;

    if( nP_NlfAkt >= nP_Nlf )
        if (!GrowNlf())
            return (const TokenId) nElementAkt+1;

    pElement[ nElementAkt ] = nP_NlfAkt;
    pType[ nElementAkt ] = T_Nlf;

    if( ppP_Nlf[ nP_NlfAkt ] )
    {
        ppP_Nlf[ nP_NlfAkt ]->aRef = rTr;
    }
    else
        ppP_Nlf[ nP_NlfAkt ] = new NLFCONT( rTr );

    nElementAkt++;
    nP_NlfAkt++;

    return ( const TokenId ) nElementAkt;
}

const TokenId TokenPool::StoreMatrix()
{
    ScMatrix* pM;

    if( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;

    if( nP_MatrixAkt >= nP_Matrix )
        if (!GrowMatrix())
            return (const TokenId) nElementAkt+1;

    pElement[ nElementAkt ] = nP_MatrixAkt;
    pType[ nElementAkt ] = T_Matrix;

    pM = new ScMatrix( 0, 0 );
    pM->IncRef( );
    ppP_Matrix[ nP_MatrixAkt ] = pM;

    nElementAkt++;
    nP_MatrixAkt++;

    return ( const TokenId ) nElementAkt;
}

const TokenId TokenPool::StoreName( sal_uInt16 nIndex, bool bGlobal )
{
    if ( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;

    pElement[nElementAkt] = static_cast<sal_uInt16>(maRangeNames.size());
    pType[nElementAkt] = T_RN;

    maRangeNames.push_back(RangeName());
    RangeName& r = maRangeNames.back();
    r.mnIndex = nIndex;
    r.mbGlobal = bGlobal;

    ++nElementAkt;

    return static_cast<const TokenId>(nElementAkt);
}

const TokenId TokenPool::StoreExtName( sal_uInt16 nFileId, const String& rName )
{
    if ( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;

    pElement[nElementAkt] = static_cast<sal_uInt16>(maExtNames.size());
    pType[nElementAkt] = T_ExtName;

    maExtNames.push_back(ExtName());
    ExtName& r = maExtNames.back();
    r.mnFileId = nFileId;
    r.maName = rName;

    ++nElementAkt;

    return static_cast<const TokenId>(nElementAkt);
}

const TokenId TokenPool::StoreExtRef( sal_uInt16 nFileId, const String& rTabName, const ScSingleRefData& rRef )
{
    if ( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;

    pElement[nElementAkt] = static_cast<sal_uInt16>(maExtCellRefs.size());
    pType[nElementAkt] = T_ExtRefC;

    maExtCellRefs.push_back(ExtCellRef());
    ExtCellRef& r = maExtCellRefs.back();
    r.mnFileId = nFileId;
    r.maTabName = rTabName;
    r.maRef = rRef;

    ++nElementAkt;

    return static_cast<const TokenId>(nElementAkt);
}

const TokenId TokenPool::StoreExtRef( sal_uInt16 nFileId, const String& rTabName, const ScComplexRefData& rRef )
{
    if ( nElementAkt >= nElement )
        if (!GrowElement())
            return (const TokenId) nElementAkt+1;

    pElement[nElementAkt] = static_cast<sal_uInt16>(maExtAreaRefs.size());
    pType[nElementAkt] = T_ExtRefA;

    maExtAreaRefs.push_back(ExtAreaRef());
    ExtAreaRef& r = maExtAreaRefs.back();
    r.mnFileId = nFileId;
    r.maTabName = rTabName;
    r.maRef = rRef;

    ++nElementAkt;

    return static_cast<const TokenId>(nElementAkt);
}

void TokenPool::Reset( void )
{
    nP_IdAkt = nP_IdLast = nElementAkt = nP_StrAkt = nP_DblAkt = nP_ErrAkt = nP_RefTrAkt = nP_ExtAkt = nP_NlfAkt = nP_MatrixAkt = 0;
    maRangeNames.clear();
    maExtNames.clear();
    maExtCellRefs.clear();
    maExtAreaRefs.clear();
}


sal_Bool TokenPool::IsSingleOp( const TokenId& rId, const DefTokenId eId ) const
{
    sal_uInt16 nId = (sal_uInt16) rId;
    if( nId && nId <= nElementAkt )
    {// existent?
        nId--;
        if( T_Id == pType[ nId ] )
        {// Tokenfolge?
            if( pSize[ nId ] == 1 )
            {// GENAU 1 Token
                sal_uInt16 nPid = pElement[ nId ];
                if (nPid < nP_Id)
                {
                    sal_uInt16 nSecId = pP_Id[ nPid ];
                    if( nSecId >= nScTokenOff )
                    {// Default-Token?
                        return ( DefTokenId ) ( nSecId - nScTokenOff ) == eId;  // Gesuchter?
                    }
                }
            }
        }
    }

    return false;
}

const String* TokenPool::GetExternal( const TokenId& rId ) const
{
    const String*   p = NULL;
    sal_uInt16 n = (sal_uInt16) rId;
    if( n && n <= nElementAkt )
    {
        n--;
        if( pType[ n ] == T_Ext )
        {
            sal_uInt16 nExt = pElement[ n ];
            if ( nExt < nP_Ext && ppP_Ext[ nExt ] )
                p = &ppP_Ext[ nExt ]->aText;
        }
    }

    return p;
}

ScMatrix* TokenPool::GetMatrix( unsigned int n ) const
{
    if( n < nP_MatrixAkt )
        return ppP_Matrix[ n ];
    else
        printf ("GETMATRIX %d >= %d\n", n, nP_MatrixAkt);
    return NULL;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
