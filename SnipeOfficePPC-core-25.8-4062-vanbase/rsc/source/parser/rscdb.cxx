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

/****************** I N C L U D E S **************************************/
// C and C++ Includes.
#include <ctype.h>      // isdigit(), isalpha()
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <tools/fsys.hxx>
#include <tools/rc.h>
#include <rtl/strbuf.hxx>
#include <sal/log.hxx>
#include <sal/macros.h>

// Programmabhaengige Includes.
#include <rsctree.hxx>
#include <rsctop.hxx>
#include <rscmgr.hxx>
#include <rscdb.hxx>
#include <rscrsc.hxx>

using ::rtl::OString;
using ::rtl::OStringBuffer;

/*************************************************************************
|*
|*    RscTypCont :: RscTypCont
|*
*************************************************************************/
RscTypCont :: RscTypCont( RscError * pErrHdl,
                          RSCBYTEORDER_TYPE nOrder,
                          const rtl::OString& rSearchPath,
                          sal_uInt32 nFlagsP )
    :
      nSourceCharSet( RTL_TEXTENCODING_UTF8 ),
      nByteOrder( nOrder ),
      aSearchPath( rSearchPath ),
      aBool( pHS->getID( "sal_Bool" ), RSC_NOTYPE ),
      aShort( pHS->getID( "short" ), RSC_NOTYPE ),
      aUShort( pHS->getID( "sal_uInt16" ), RSC_NOTYPE ),
      aLong( pHS->getID( "long" ), RSC_NOTYPE ),
      aEnumLong( pHS->getID( "enum_long" ), RSC_NOTYPE ),
      aIdUShort( pHS->getID( "IDUSHORT" ), RSC_NOTYPE ),
      aIdNoZeroUShort( pHS->getID( "IDUSHORT" ), RSC_NOTYPE ),
      aNoZeroShort( pHS->getID( "NoZeroShort" ), RSC_NOTYPE ),
      a1to12Short( pHS->getID( "MonthShort" ), RSC_NOTYPE ),
      a0to23Short( pHS->getID( "HourShort" ), RSC_NOTYPE ),
      a1to31Short( pHS->getID( "DayShort" ), RSC_NOTYPE ),
      a0to59Short( pHS->getID( "MinuteShort" ), RSC_NOTYPE ),
      a0to99Short( pHS->getID( "_0to59Short" ), RSC_NOTYPE ),
      a0to9999Short( pHS->getID( "YearShort" ), RSC_NOTYPE ),
      aIdLong( pHS->getID( "IDLONG" ), RSC_NOTYPE ),
      aString( pHS->getID( "Chars" ), RSC_NOTYPE ),
      aStringLiteral( pHS->getID( "Chars" ), RSC_NOTYPE ),
      aWinBits( pHS->getID( "WinBits" ), RSC_NOTYPE ),
      aLangType(),
      aLangString( pHS->getID( "Lang_Chars" ), RSC_NOTYPE, &aString, &aLangType ),
      aLangShort( pHS->getID( "Lang_short" ), RSC_NOTYPE, &aShort, &aLangType ),
      nAcceleratorType( 0 ),
      nFlags( nFlagsP )
{
    nUniqueId = 256;
    nPMId = RSC_VERSIONCONTROL +1; //mindestens einen groesser
    pEH = pErrHdl;
    Init();
}

static sal_uInt32 getLangIdAndShortenLocale( RscTypCont* pTypCont,
                                             rtl::OString& rLang,
                                             rtl::OString& rCountry,
                                             rtl::OString& rVariant )
{
    rtl::OStringBuffer aLangStr( 64 );
    aLangStr.append( rLang.toAsciiLowerCase() );
    if( !rCountry.isEmpty() )
    {
        aLangStr.append( '-' );
        aLangStr.append( rCountry.toAsciiUpperCase() );
    }
    if( !rVariant.isEmpty() )
    {
        aLangStr.append( '-' );
        aLangStr.append( rVariant );
    }
    rtl::OString aL( aLangStr.makeStringAndClear() );
    sal_uInt32 nRet = GetLangId( aL );
    if( nRet == 0 )
    {
        pTypCont->AddLanguage( aL.getStr() );
        nRet = GetLangId( aL );
    }
    if( !rVariant.isEmpty() )
        rVariant = rtl::OString();
    else if( !rCountry.isEmpty() )
        rCountry = rtl::OString();
    else
        rLang = rtl::OString();
#if OSL_DEBUG_LEVEL > 1
        fprintf( stderr, " %s (0x%hx)", aL.getStr(), (int)nRet );
#endif
    return nRet;
}

rtl::OString RscTypCont::ChangeLanguage(const rtl::OString& rNewLang)
{
    rtl::OString aRet = aLanguage;
    aLanguage = rNewLang;

    rtl::OString aLang = aLanguage;
    rtl::OString aLg, aCountry, aVariant;
    sal_Int32 nIndex = 0;
    aLg = aLang.getToken( 0, '-', nIndex );
    if( nIndex != -1 )
        aCountry = aLang.getToken( 0, '-', nIndex );
    if( nIndex != -1 )
        aVariant = aLang.copy( nIndex );

    bool bAppendEnUsFallback =
        ! (rNewLang.equalsIgnoreAsciiCase( "en-US" ) ||
           rNewLang.equalsIgnoreAsciiCase( "x-no-translate" ) );

#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "RscTypCont::ChangeLanguage:" );
#endif
    aLangFallbacks.clear();

    do
    {
        aLangFallbacks.push_back(getLangIdAndShortenLocale( this, aLg, aCountry, aVariant ) );
    } while( !aLg.isEmpty() );

    if( bAppendEnUsFallback )
    {
        aLg = "en";
        aCountry = "US";
        aVariant = rtl::OString();
        aLangFallbacks.push_back( getLangIdAndShortenLocale( this, aLg, aCountry, aVariant ) );
    }

#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "\n" );
#endif

    return aRet;
}

Atom RscTypCont::AddLanguage( const char* pLang )
{
    return aLangType.AddLanguage( pLang, aNmTb );
}


/*************************************************************************
|*
|*    RscTypCont :: ~RscTypCont
|*
*************************************************************************/
void DestroyNode( RscTop * pRscTop, ObjNode * pObjNode ){
    if( pObjNode ){
        DestroyNode( pRscTop, (ObjNode*)pObjNode->Left() );
        DestroyNode( pRscTop, (ObjNode*)pObjNode->Right() );

        if( pObjNode->GetRscObj() ){
            pRscTop->Destroy( RSCINST( pRscTop, pObjNode->GetRscObj() ) );
            rtl_freeMemory( pObjNode->GetRscObj() );
        }
        delete pObjNode;
    };
}

void DestroySubTrees( RscTop * pRscTop ){
    if( pRscTop ){
        DestroySubTrees( (RscTop*)pRscTop->Left() );

        DestroyNode( pRscTop, pRscTop->GetObjNode() );

        DestroySubTrees( (RscTop*)pRscTop->Right() );
    };
}

void DestroyTree( RscTop * pRscTop ){
    if( pRscTop ){
        DestroyTree( (RscTop*)pRscTop->Left() );
        DestroyTree( (RscTop*)pRscTop->Right() );

        delete pRscTop;
    };
}

void Pre_dtorTree( RscTop * pRscTop ){
    if( pRscTop ){
        Pre_dtorTree( (RscTop*)pRscTop->Left() );
        Pre_dtorTree( (RscTop*)pRscTop->Right() );

        pRscTop->Pre_dtor();
    };
}

RscTypCont :: ~RscTypCont(){
    // Alle Unterbaeume loeschen
    aVersion.pClass->Destroy( aVersion );
    rtl_freeMemory( aVersion.pData );
    DestroySubTrees( pRoot );

    // Alle Klassen noch gueltig, jeweilige Instanzen freigeben
    // BasisTypen
    for ( size_t i = 0, n = aBaseLst.size(); i < n; ++i )
        aBaseLst[ i ]->Pre_dtor();
    aBool.Pre_dtor();
    aShort.Pre_dtor();
    aUShort.Pre_dtor();
    aIdUShort.Pre_dtor();
    aIdNoZeroUShort.Pre_dtor();
    aNoZeroShort.Pre_dtor();
    aIdLong.Pre_dtor();
    aString.Pre_dtor();
    aWinBits.Pre_dtor();
    aVersion.pClass->Pre_dtor();
    // Zusammengesetzte Typen
    Pre_dtorTree( pRoot );

    // Klassen zerstoeren
    delete aVersion.pClass;
    DestroyTree( pRoot );

    for ( size_t i = 0, n = aBaseLst.size(); i < n; ++i )
        delete aBaseLst[ i ];
    aBaseLst.clear();

    for ( size_t i = 0, n = aSysLst.size(); i < n; ++i )
        delete aSysLst[ i ];
    aSysLst.clear();
}

void RscTypCont::ClearSysNames()
{
    for ( size_t i = 0, n = aSysLst.size(); i < n; ++i )
        delete aSysLst[ i ];
    aSysLst.clear();
}

//=======================================================================
RscTop * RscTypCont::SearchType( Atom nId )
/*  [Beschreibung]

    Sucht eine Basistyp nId;
*/
{
    if( nId == InvalidAtom )
        return NULL;

#define ELSE_IF( a )                \
    else if( a.GetId() == nId ) \
        return &a;                  \

    if( aBool.GetId() == nId )
        return &aBool;
    ELSE_IF( aShort )
    ELSE_IF( aUShort )
    ELSE_IF( aLong )
    ELSE_IF( aEnumLong )
    ELSE_IF( aIdUShort )
    ELSE_IF( aIdNoZeroUShort )
    ELSE_IF( aNoZeroShort )
    ELSE_IF( a1to12Short )
    ELSE_IF( a0to23Short )
    ELSE_IF( a1to31Short )
    ELSE_IF( a0to59Short )
    ELSE_IF( a0to99Short )
    ELSE_IF( a0to9999Short )
    ELSE_IF( aIdLong )
    ELSE_IF( aString )
    ELSE_IF( aWinBits )
    ELSE_IF( aLangType )
    ELSE_IF( aLangString )
    ELSE_IF( aLangShort )

    for ( size_t i = 0, n = aBaseLst.size(); i < n; ++i )
    {
        RscTop* pEle = aBaseLst[ i ];
        if( pEle->GetId() == nId )
            return pEle;
    }
    return NULL;
}

/*************************************************************************
|*
|*    RscTypCont :: PutSysName()
|*
*************************************************************************/
sal_uInt32 RscTypCont :: PutSysName( sal_uInt32 nRscTyp, char * pFileName,
                                 sal_uInt32 nConst, sal_uInt32 nId, sal_Bool bFirst )
{
    RscSysEntry *pSysEntry;
    RscSysEntry *pFoundEntry = NULL;
    sal_Bool            bId1 = sal_False;

    for ( size_t i = 0, n = aSysLst.size(); i < n; ++i )
    {
        pSysEntry = aSysLst[ i ];
        if( pSysEntry->nKey == 1 )
            bId1 = sal_True;
        if( !strcmp( pSysEntry->aFileName.getStr(), pFileName ) )
            if(  pSysEntry->nRscTyp == nRscTyp
              && pSysEntry->nTyp    == nConst
              && pSysEntry->nRefId  == nId
                 ) {
                pFoundEntry = pSysEntry;
                break;
            }
    }
    pSysEntry = pFoundEntry;

    if ( !pSysEntry || (bFirst && !bId1) )
    {
        pSysEntry = new RscSysEntry;
        pSysEntry->nKey = nUniqueId++;
        pSysEntry->nRscTyp = nRscTyp;
        pSysEntry->nTyp = nConst;
        pSysEntry->nRefId = nId;
        pSysEntry->aFileName = (const char*)pFileName;
        if( bFirst && !bId1 )
        {
            pSysEntry->nKey = 1;
            aSysLst.insert( aSysLst.begin(), pSysEntry );
        }
        else
            aSysLst.push_back( pSysEntry );
    }

    return pSysEntry->nKey;
}

/*************************************************************************
|*
|*    RscTypCont :: WriteInc
|*
*************************************************************************/
void RscTypCont :: WriteInc( FILE * fOutput, sal_uLong lFileKey )
{

    if( NOFILE_INDEX == lFileKey )
    {
        sal_uIntPtr aIndex = aFileTab.FirstIndex();
        while( aIndex != UNIQUEINDEX_ENTRY_NOTFOUND )
        {
            RscFile   * pFName = aFileTab.Get( aIndex );
            if( pFName->IsIncFile() )
            {
                fprintf( fOutput, "#include " );
                fprintf( fOutput, "\"%s\"\n",
                                  pFName->aFileName.getStr() );
            }
            aIndex = aFileTab.NextIndex( aIndex );
        }
    }
    else
    {
        RscDepend *     pDep;
        RscFile   *     pFile;

        RscFile   *     pFName = aFileTab.Get( lFileKey );
        if( pFName )
        {
            for ( size_t i = 0, n = pFName->aDepLst.size(); i < n; ++i )
            {
                pDep = pFName->aDepLst[ i ];
                if( pDep->GetFileKey() != lFileKey )
                {
                    pFile = aFileTab.GetFile( pDep->GetFileKey() );
                    if( pFile )
                    {
                        fprintf( fOutput, "#include " );
                        fprintf( fOutput, "\"%s\"\n",
                                 pFile->aFileName.getStr() );
                    }
                }
            };
        };
    };
}

/*************************************************************************
|*
|*    RscTypCont :: Methoden die ueber all Knoten laufen
|*
*************************************************************************/

class RscEnumerateObj
{
friend class RscEnumerateRef;
private:
    ERRTYPE     aError;     // Enthaelt den ersten Fehler
    RscTypCont* pTypCont;
    FILE *      fOutput;    // AusgabeDatei
    sal_uLong       lFileKey;   // Welche src-Datei
    RscTop *    pClass;

    DECL_LINK( CallBackWriteRc, ObjNode * );
    DECL_LINK( CallBackWriteSrc, ObjNode * );
    DECL_LINK( CallBackWriteCxx, ObjNode * );
    DECL_LINK( CallBackWriteHxx, ObjNode * );

    ERRTYPE WriteRc( RscTop * pCl, ObjNode * pRoot )
    {
        pClass = pCl;
        if( pRoot )
            pRoot->EnumNodes( LINK( this, RscEnumerateObj, CallBackWriteRc ) );
        return aError;
    }
    ERRTYPE WriteSrc( RscTop * pCl, ObjNode * pRoot ){
        pClass = pCl;
        if( pRoot )
            pRoot->EnumNodes( LINK( this, RscEnumerateObj, CallBackWriteSrc ) );
        return aError;
    }
    ERRTYPE WriteCxx( RscTop * pCl, ObjNode * pRoot ){
        pClass = pCl;
        if( pRoot )
            pRoot->EnumNodes( LINK( this, RscEnumerateObj, CallBackWriteCxx ) );
        return aError;
    }
    ERRTYPE WriteHxx( RscTop * pCl, ObjNode * pRoot ){
        pClass = pCl;
        if( pRoot )
            pRoot->EnumNodes( LINK( this, RscEnumerateObj, CallBackWriteHxx ) );
        return aError;
    }
public:
    void WriteRcFile( RscWriteRc & rMem, FILE * fOutput );
};

/*************************************************************************
|*
|*    RscEnumerateObj :: CallBackWriteRc
|*
*************************************************************************/
IMPL_LINK( RscEnumerateObj, CallBackWriteRc, ObjNode *, pObjNode )
{
    RscWriteRc      aMem( pTypCont->GetByteOrder() );

    aError = pClass->WriteRcHeader( RSCINST( pClass, pObjNode->GetRscObj() ),
                                     aMem, pTypCont,
                                     pObjNode->GetRscId(), 0, sal_True );
    if( aError.IsError() || aError.IsWarning() )
        pTypCont->pEH->Error( aError, pClass, pObjNode->GetRscId() );

    WriteRcFile( aMem, fOutput );
    return 0;
}

/*************************************************************************
|*
|*    RscEnumerateObj :: CallBackWriteSrc
|*
*************************************************************************/
IMPL_LINK_INLINE_START( RscEnumerateObj, CallBackWriteSrc, ObjNode *, pObjNode )
{
    if( pObjNode->GetFileKey() == lFileKey ){
        pClass->WriteSrcHeader( RSCINST( pClass, pObjNode->GetRscObj() ),
                                fOutput, pTypCont, 0,
                                pObjNode->GetRscId(), "" );
        fprintf( fOutput, ";\n" );
    }
    return 0;
}
IMPL_LINK_INLINE_END( RscEnumerateObj, CallBackWriteSrc, ObjNode *, pObjNode )

/*************************************************************************
|*
|*    RscEnumerateObj :: CallBackWriteCxx
|*
*************************************************************************/
IMPL_LINK_INLINE_START( RscEnumerateObj, CallBackWriteCxx, ObjNode *, pObjNode )
{
    if( pClass->IsCodeWriteable() && pObjNode->GetFileKey() == lFileKey )
        aError = pClass->WriteCxxHeader(
                              RSCINST( pClass, pObjNode->GetRscObj() ),
                              fOutput, pTypCont, pObjNode->GetRscId() );
    return 0;
}
IMPL_LINK_INLINE_END( RscEnumerateObj, CallBackWriteCxx, ObjNode *, pObjNode )

/*************************************************************************
|*
|*    RscEnumerateObj :: CallBackWriteHxx
|*
*************************************************************************/
IMPL_LINK_INLINE_START( RscEnumerateObj, CallBackWriteHxx, ObjNode *, pObjNode )
{
    if( pClass->IsCodeWriteable() && pObjNode->GetFileKey() == lFileKey )
        aError = pClass->WriteHxxHeader(
                              RSCINST( pClass, pObjNode->GetRscObj() ),
                              fOutput, pTypCont, pObjNode->GetRscId() );
    return 0;
}
IMPL_LINK_INLINE_END( RscEnumerateObj, CallBackWriteHxx, ObjNode *, pObjNode )

/*************************************************************************
|*
|*    RscEnumerateObj :: WriteRcFile
|*
*************************************************************************/
void RscEnumerateObj :: WriteRcFile( RscWriteRc & rMem, FILE * fOut )
{
    // Definition der Struktur, aus denen die Resource aufgebaut ist
    /*
    struct RSHEADER_TYPE{
        sal_uInt32          nId;        // Identifier der Resource
        sal_uInt32          nRT;        // Resource Typ
        sal_uInt32          nGlobOff;   // Globaler Offset
        sal_uInt32          nLocalOff;  // Lokaler Offset
    } aHeader;
    */

    sal_uInt32 nId = rMem.GetLong( 0 );
    sal_uInt32 nRT = rMem.GetLong( 4 );

    // Tabelle wird entsprechend gefuellt
    pTypCont->PutTranslatorKey( (sal_uInt64(nRT) << 32) + sal_uInt64(nId) );

    if( nRT == RSC_VERSIONCONTROL )
    { // kommt immmer als letztes
        sal_Int32 nCount = pTypCont->aIdTranslator.size();
        // groesse der Tabelle
        sal_uInt32 nSize = (nCount * (sizeof(sal_uInt64)+sizeof(sal_Int32))) + sizeof(sal_Int32);

        rMem.Put( nCount ); //Anzahl speichern
        for( std::map< sal_uInt64, sal_uLong >::const_iterator it =
             pTypCont->aIdTranslator.begin(); it != pTypCont->aIdTranslator.end(); ++it )
        {
            // Schluessel schreiben
            rMem.Put( it->first );
            // Objekt Id oder Position schreiben
            rMem.Put( (sal_Int32)it->second );
        }
        rMem.Put( nSize ); // Groesse hinten Speichern
    }

    //Dateioffset neu setzen
    pTypCont->IncFilePos( rMem.Size() );


    //Position wurde vorher in Tabelle geschrieben
    bool bSuccess = (1 == fwrite( rMem.GetBuffer(), rMem.Size(), 1, fOut ));
    SAL_WARN_IF(!bSuccess, "rsc", "short write");
};

class RscEnumerateRef
{
private:
    RscTop *        pRoot;

    DECL_LINK( CallBackWriteRc, RscTop * );
    DECL_LINK( CallBackWriteSrc, RscTop * );
    DECL_LINK( CallBackWriteCxx, RscTop * );
    DECL_LINK( CallBackWriteHxx, RscTop * );
    DECL_LINK( CallBackWriteSyntax, RscTop * );
    DECL_LINK( CallBackWriteRcCtor, RscTop * );
public:
    RscEnumerateObj aEnumObj;

            RscEnumerateRef( RscTypCont * pTC, RscTop * pR,
                             FILE * fOutput )
            {
                aEnumObj.pTypCont = pTC;
                aEnumObj.fOutput  = fOutput;
                pRoot             = pR;
            }
    ERRTYPE WriteRc()
    {
        aEnumObj.aError.Clear();
        pRoot->EnumNodes( LINK( this, RscEnumerateRef, CallBackWriteRc ) );
        return aEnumObj.aError;
    };

    ERRTYPE WriteSrc( sal_uLong lFileKey )
    {
        aEnumObj.lFileKey = lFileKey;

        aEnumObj.aError.Clear();
        pRoot->EnumNodes( LINK( this, RscEnumerateRef, CallBackWriteSrc ) );
        return aEnumObj.aError;
    }

    ERRTYPE WriteCxx( sal_uLong lFileKey )
    {
        aEnumObj.lFileKey = lFileKey;

        aEnumObj.aError.Clear();
        pRoot->EnumNodes( LINK( this, RscEnumerateRef, CallBackWriteCxx ) );
        return aEnumObj.aError;
    }

    ERRTYPE WriteHxx(  sal_uLong lFileKey )
    {
        aEnumObj.lFileKey = lFileKey;

        aEnumObj.aError.Clear();
        pRoot->EnumNodes( LINK( this, RscEnumerateRef, CallBackWriteHxx ) );
        return aEnumObj.aError;
    }

    void    WriteSyntax()
            {
                pRoot->EnumNodes( LINK( this, RscEnumerateRef,
                                        CallBackWriteSyntax ) );
            }

    void    WriteRcCtor()
            {
                pRoot->EnumNodes( LINK( this, RscEnumerateRef,
                                        CallBackWriteRcCtor ) );
            }
};

/*************************************************************************
|*
|*    RscRscEnumerateRef :: CallBack...
|*
*************************************************************************/
IMPL_LINK_INLINE_START( RscEnumerateRef, CallBackWriteRc, RscTop *, pRef )
{
    aEnumObj.WriteRc( pRef, pRef->GetObjNode() );
    return 0;
}
IMPL_LINK_INLINE_END( RscEnumerateRef, CallBackWriteRc, RscTop *, pRef )
IMPL_LINK_INLINE_START( RscEnumerateRef, CallBackWriteSrc, RscTop *, pRef )
{
    aEnumObj.WriteSrc( pRef, pRef->GetObjNode() );
    return 0;
}
IMPL_LINK_INLINE_END( RscEnumerateRef, CallBackWriteSrc, RscTop *, pRef )
IMPL_LINK_INLINE_START( RscEnumerateRef, CallBackWriteCxx, RscTop *, pRef )
{
    if( pRef->IsCodeWriteable() )
        aEnumObj.WriteCxx( pRef, pRef->GetObjNode() );
    return 0;
}
IMPL_LINK_INLINE_END( RscEnumerateRef, CallBackWriteCxx, RscTop *, pRef )
IMPL_LINK_INLINE_START( RscEnumerateRef, CallBackWriteHxx, RscTop *, pRef )
{
    if( pRef->IsCodeWriteable() )
        aEnumObj.WriteHxx( pRef, pRef->GetObjNode() );
    return 0;
}
IMPL_LINK_INLINE_END( RscEnumerateRef, CallBackWriteHxx, RscTop *, pRef )
IMPL_LINK_INLINE_START( RscEnumerateRef, CallBackWriteSyntax, RscTop *, pRef )
{
    pRef->WriteSyntaxHeader( aEnumObj.fOutput, aEnumObj.pTypCont );
    return 0;
}
IMPL_LINK_INLINE_END( RscEnumerateRef, CallBackWriteSyntax, RscTop *, pRef )
IMPL_LINK_INLINE_START( RscEnumerateRef, CallBackWriteRcCtor, RscTop *, pRef )
{
    pRef->WriteRcCtor( aEnumObj.fOutput, aEnumObj.pTypCont );
    return 0;
}
IMPL_LINK_INLINE_END( RscEnumerateRef, CallBackWriteRcCtor, RscTop *, pRef )

/*************************************************************************
|*
|*    RscTypCont :: WriteRc
|*
*************************************************************************/

ERRTYPE RscTypCont::WriteRc( WriteRcContext& rContext )
{
    ERRTYPE       aError;
    RscEnumerateRef aEnumRef( this, pRoot, rContext.fOutput );

    aIdTranslator.clear();
    nFilePos = 0;
    nPMId = RSCVERSION_ID +1; //mindestens einen groesser

    aError = aEnumRef.WriteRc();

    // version control
    RscWriteRc aMem( nByteOrder );
    aVersion.pClass->WriteRcHeader( aVersion, aMem, this, RscId( RSCVERSION_ID ), 0, sal_True );
    aEnumRef.aEnumObj.WriteRcFile( aMem, rContext.fOutput );

    return aError;
}

/*************************************************************************
|*
|*    RscTypCont :: WriteSrc
|*
*************************************************************************/
void RscTypCont :: WriteSrc( FILE * fOutput, sal_uLong nFileKey,
                             sal_Bool bName )
{
    RscFile     *   pFName;
    RscEnumerateRef aEnumRef( this, pRoot, fOutput );

    unsigned char aUTF8BOM[3] = { 0xef, 0xbb, 0xbf };
    size_t nItems = SAL_N_ELEMENTS(aUTF8BOM);
    bool bSuccess = (nItems == fwrite(aUTF8BOM, 1, nItems, fOutput));
    SAL_WARN_IF(!bSuccess, "rsc", "short write");
    if( bName )
    {
        WriteInc( fOutput, nFileKey );

        if( NOFILE_INDEX == nFileKey )
        {
            sal_uIntPtr aIndex = aFileTab.FirstIndex();
            while( aIndex != UNIQUEINDEX_ENTRY_NOTFOUND ) {
                pFName = aFileTab.Get( aIndex );
                if( !pFName->IsIncFile() )
                    pFName->aDefLst.WriteAll( fOutput );
                aEnumRef.WriteSrc( aIndex );
                aIndex = aFileTab.NextIndex( aIndex );
            };
        }
        else
        {
            pFName = aFileTab.Get( nFileKey );
            if( pFName ){
                pFName->aDefLst.WriteAll( fOutput );
                aEnumRef.WriteSrc( nFileKey );
            }
        }
    }
    else
    {
        RscId::SetNames( sal_False );
        if( NOFILE_INDEX == nFileKey )
        {
            sal_uIntPtr aIndex = aFileTab.FirstIndex();
            while( aIndex != UNIQUEINDEX_ENTRY_NOTFOUND )
            {
                aEnumRef.WriteSrc( aIndex );
                aIndex = aFileTab.NextIndex( aIndex );
            };
        }
        else
             aEnumRef.WriteSrc( nFileKey );
        RscId::SetNames();
    };
}

/*************************************************************************
|*
|*    RscTypCont :: WriteHxx
|*
*************************************************************************/
ERRTYPE RscTypCont :: WriteHxx( FILE * fOutput, sal_uLong nFileKey )
{
    fprintf( fOutput, "#include <tools/rc.hxx>\n" );
    fprintf( fOutput, "#include <tools/resid.hxx>\n" );
    fprintf( fOutput, "#include <vcl/accel.hxx>\n" );
    fprintf( fOutput, "#include <vcl/bitmap.hxx>\n" );
    fprintf( fOutput, "#include <vcl/button.hxx>\n" );
    fprintf( fOutput, "#include <tools/color.hxx>\n" );
    fprintf( fOutput, "#include <vcl/combobox.hxx>\n" );
    fprintf( fOutput, "#include <vcl/ctrl.hxx>\n" );
    fprintf( fOutput, "#include <vcl/dialog.hxx>\n" );
    fprintf( fOutput, "#include <vcl/edit.hxx>\n" );
    fprintf( fOutput, "#include <vcl/field.hxx>\n" );
    fprintf( fOutput, "#include <vcl/fixed.hxx>\n" );
    fprintf( fOutput, "#include <vcl/group.hxx>\n" );
    fprintf( fOutput, "#include <vcl/image.hxx>\n" );
    fprintf( fOutput, "#include <vcl/button.hxx>\n" );
    fprintf( fOutput, "#include <vcl/keycod.hxx>\n" );
    fprintf( fOutput, "#include <vcl/lstbox.hxx>\n" );
    fprintf( fOutput, "#include <vcl/mapmod.hxx>\n" );
    fprintf( fOutput, "#include <vcl/menu.hxx>\n" );
    fprintf( fOutput, "#include <vcl/menubtn.hxx>\n" );
    fprintf( fOutput, "#include <vcl/morebtn.hxx>\n" );
    fprintf( fOutput, "#include <vcl/msgbox.hxx>\n" );
    fprintf( fOutput, "#include <vcl/scrbar.hxx>\n" );
    fprintf( fOutput, "#include <vcl/spin.hxx>\n" );
    fprintf( fOutput, "#include <vcl/spinfld.hxx>\n" );
    fprintf( fOutput, "#include <vcl/splitwin.hxx>\n" );
    fprintf( fOutput, "#include <vcl/status.hxx>\n" );
    fprintf( fOutput, "#include <vcl/tabctrl.hxx>\n" );
    fprintf( fOutput, "#include <vcl/tabdlg.hxx>\n" );
    fprintf( fOutput, "#include <vcl/tabpage.hxx>\n" );
    fprintf( fOutput, "#include <vcl/toolbox.hxx>\n" );
    fprintf( fOutput, "#include <vcl/window.hxx>\n" );
    fprintf( fOutput, "#include <vcl/wrkwin.hxx>\n" );
    fprintf( fOutput, "#include <svtools/svmedit.hxx>\n" );

    RscEnumerateRef aEnumRef( this, pRoot, fOutput );
    ERRTYPE         aError;

    if( NOFILE_INDEX == nFileKey )
    {
        sal_uIntPtr aIndex = aFileTab.FirstIndex();
        while( aIndex != UNIQUEINDEX_ENTRY_NOTFOUND )
        {
            aError = aEnumRef.WriteHxx( aIndex );
            aIndex = aFileTab.NextIndex( aIndex );
        };
    }
    else
        aError = aEnumRef.WriteHxx( nFileKey );

    return aError;
}

/*************************************************************************
|*
|*    RscTypCont :: WriteCxx
|*
*************************************************************************/
ERRTYPE RscTypCont::WriteCxx( FILE * fOutput, sal_uLong nFileKey,
                              const rtl::OString& rHxxName )
{
    RscEnumerateRef aEnumRef( this, pRoot, fOutput );
    ERRTYPE         aError;
    fprintf( fOutput, "#include <string.h>\n" );
    WriteInc( fOutput, nFileKey );
    if( !rHxxName.isEmpty() )
        fprintf( fOutput, "#include \"%s\"\n", rHxxName.getStr() );
    fprintf( fOutput, "\n\n" );

    if( NOFILE_INDEX == nFileKey )
    {
        sal_uIntPtr aIndex = aFileTab.FirstIndex();
        while( aIndex != UNIQUEINDEX_ENTRY_NOTFOUND )
        {
            aError = aEnumRef.WriteCxx( aIndex );
            aIndex = aFileTab.NextIndex( aIndex );
        };
    }
    else
        aError = aEnumRef.WriteCxx( nFileKey );

    return aError;
}

/*************************************************************************
|*
|*    RscTypCont :: WriteSyntax
|*
*************************************************************************/
void RscTypCont::WriteSyntax( FILE * fOutput )
{
    for( size_t i = 0; i < aBaseLst.size(); i++ )
        aBaseLst[ i ]->WriteSyntaxHeader( fOutput, this );
    RscEnumerateRef aEnumRef( this, pRoot, fOutput );
    aEnumRef.WriteSyntax();
}

//=======================================================================
void RscTypCont::WriteRcCtor
(
    FILE * fOutput
)
{
    RscEnumerateRef aEnumRef( this, pRoot, fOutput );
    aEnumRef.WriteRcCtor();
}

/*************************************************************************
|*
|*    RscTypCont :: Delete()
|*
*************************************************************************/
class RscDel
{
    sal_uLong lFileKey;
    DECL_LINK( Delete, RscTop * );
public:
    RscDel( RscTop * pRoot, sal_uLong lKey );
};


inline RscDel::RscDel( RscTop * pRoot, sal_uLong lKey )
{
    lFileKey = lKey;
    pRoot->EnumNodes( LINK( this, RscDel, Delete ) );
}

IMPL_LINK_INLINE_START( RscDel, Delete, RscTop *, pNode )
{
    if( pNode->GetObjNode() )
        pNode->pObjBiTree = pNode->GetObjNode()->DelObjNode( pNode, lFileKey );
    return 0;
}
IMPL_LINK_INLINE_END( RscDel, Delete, RscTop *, pNode )

void RscTypCont :: Delete( sal_uLong lFileKey ){
    // Resourceinstanzen loeschen
    RscDel aDel( pRoot, lFileKey );
    // Defines loeschen
    aFileTab.DeleteFileContext( lFileKey );
}

sal_Bool IsInstConsistent( ObjNode * pObjNode, RscTop * pRscTop )
{
    sal_Bool bRet = sal_True;

    if( pObjNode ){
        RSCINST aTmpI;

        if( ! IsInstConsistent( (ObjNode*)pObjNode->Left(), pRscTop ) )
            bRet = sal_False;

        aTmpI.pClass = pRscTop;
        aTmpI.pData = pObjNode->GetRscObj();
        if( ! aTmpI.pClass->IsConsistent( aTmpI ) )
            bRet = sal_False;

        if( ! IsInstConsistent( (ObjNode*)pObjNode->Right(), pRscTop ) )
            bRet = sal_False;
    };

    return( bRet );
}

sal_Bool MakeConsistent( RscTop * pRscTop )
{
    sal_Bool bRet = sal_True;

    if( pRscTop ){
        if( ! ::MakeConsistent( (RscTop*)pRscTop->Left() ) )
            bRet = sal_False;

        if( pRscTop->GetObjNode() ){
            if( ! pRscTop->GetObjNode()->IsConsistent() ){
                pRscTop->GetObjNode()->OrderTree();
                if( ! pRscTop->GetObjNode()->IsConsistent() )
                    bRet = sal_False;
            }
            if( ! IsInstConsistent( pRscTop->GetObjNode(), pRscTop ) )
                bRet = sal_False;
        }

        if( ! ::MakeConsistent( (RscTop*)pRscTop->Right() ) )
            bRet = sal_False;
    };

    return bRet;
}

sal_uInt32 RscTypCont::PutTranslatorKey( sal_uInt64 nKey )
{
    aIdTranslator[ nKey ] = nFilePos;
    return nPMId++;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
