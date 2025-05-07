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
#ifndef _TOOLS_RESMGR_HXX
#define _TOOLS_RESMGR_HXX

#include "tools/toolsdllapi.h"
#include <i18npool/lang.h>
#include <tools/string.hxx>
#include <tools/resid.hxx>
#include <com/sun/star/lang/Locale.hpp>

#include <vector>

class SvStream;
class InternalResMgr;

// -----------------
// - RSHEADER_TYPE -
// -----------------

// Definition der Struktur, aus denen die Resource aufgebaut ist
struct RSHEADER_TYPE
{
private:
    sal_uInt32              nId;        // Identifier der Resource
    RESOURCE_TYPE           nRT;        // Resource Typ
    sal_uInt32              nGlobOff;   // Globaler Offset
    sal_uInt32              nLocalOff;  // Lokaler Offset

public:
    inline sal_uInt32       GetId();       // Identifier der Resource
    inline RESOURCE_TYPE    GetRT();       // Resource Typ
    inline sal_uInt32       GetGlobOff();   // Globaler Offset
    inline sal_uInt32       GetLocalOff();  // Lokaler Offset
};

// ----------
// - ResMgr -
// ----------

typedef rtl::OUString (*ResHookProc)( const rtl::OUString& rStr );

// ----------
// - ResMgr -
// ----------

// Initialisierung
#define RC_NOTYPE               0x00
// Globale Resource
#define RC_GLOBAL               0x01
#define RC_AUTORELEASE          0x02
#define RC_NOTFOUND             0x04
#define RC_FALLBACK_DOWN        0x08
#define RC_FALLBACK_UP          0x10

class Resource;
class ResMgr;
struct ImpRCStack
{
    // pResource and pClassRes equal NULL: resource was not loaded
    RSHEADER_TYPE * pResource;          // pointer to resource
    void          * pClassRes;          // pointer to class specified init data
    short           Flags;              // resource status
    void *          aResHandle;         // Resource-Identifier from InternalResMgr
    const Resource* pResObj;            // pointer to Resource object
    sal_uInt32      nId;                // ResId used for error message
    ResMgr*         pResMgr;            // ResMgr for Resource pResObj

    void            Clear();
    void            Init( ResMgr * pMgr, const Resource * pObj, sal_uInt32 nId );
};

class TOOLS_DLLPUBLIC ResMgr
{
private:
    InternalResMgr*            pImpRes;
    std::vector< ImpRCStack >  aStack;  // resource context stack
    int                        nCurStack;
    ResMgr*         pFallbackResMgr;    // fallback ResMgr in case the Resource
                                        // was not contained in this ResMgr
    ResMgr*         pOriginalResMgr;    // the res mgr that fell back to this
                                        // stack level

    TOOLS_DLLPRIVATE void         incStack();
    TOOLS_DLLPRIVATE void         decStack();

    TOOLS_DLLPRIVATE const ImpRCStack * StackTop( sal_uInt32 nOff = 0 ) const
    {
        return (((int)nOff >= nCurStack) ? NULL : &aStack[nCurStack-nOff]);
    }
    TOOLS_DLLPRIVATE void               Init( const rtl::OUString& rFileName );

    TOOLS_DLLPRIVATE ResMgr( InternalResMgr * pImp );

    #ifdef DBG_UTIL
    TOOLS_DLLPRIVATE static void RscError_Impl( const sal_Char* pMessage, ResMgr* pResMgr,
                               RESOURCE_TYPE nRT, sal_uInt32 nId,
                               std::vector< ImpRCStack >& rResStack, int nDepth );
    #endif

    // called from within GetResource() if a resource could not be found
    TOOLS_DLLPRIVATE ResMgr* CreateFallbackResMgr( const ResId& rId, const Resource* pResource );
    // creates a 1k sized buffer set to zero for unfound resources
    // used in case RC_NOTFOUND
    static void* pEmptyBuffer;
    TOOLS_DLLPRIVATE static void* getEmptyBuffer();

    // the next two methods are needed to prevent the string hook called
    // with the res mgr mutex locked
    // like GetString, but doesn't call the string hook
    TOOLS_DLLPRIVATE static sal_uInt32  GetStringWithoutHook( UniString& rStr, const sal_uInt8* pStr );
    // like ReadString but doesn't call the string hook
    TOOLS_DLLPRIVATE UniString          ReadStringWithoutHook();

    static ResMgr* ImplCreateResMgr( InternalResMgr* pImpl ) { return new ResMgr( pImpl ); }

    //No copying
    ResMgr(const ResMgr&);
    ResMgr& operator=(const ResMgr&);
public:
    static void         DestroyAllResMgr();  // Wird gerufen, wenn App beendet wird

    ~ResMgr();

                        // Sprachabhaengige Ressource Library
    static const sal_Char*  GetLang( LanguageType& eLanguage, sal_uInt16 nPrio = 0 ); //depricated! see "tools/source/rc/resmgr.cxx"
    static ResMgr*      SearchCreateResMgr( const sal_Char* pPrefixName,
                                            com::sun::star::lang::Locale& rLocale );
     static ResMgr*     CreateResMgr( const sal_Char* pPrefixName,
                                      com::sun::star::lang::Locale aLocale = com::sun::star::lang::Locale( rtl::OUString(),
                                                                                                           rtl::OUString(),
                                                                                                           rtl::OUString()));

    // Testet ob Resource noch da ist
    void                TestStack( const Resource * );

    // ist Resource verfuegbar
    sal_Bool                IsAvailable( const ResId& rId,
                                     const Resource* = NULL) const;

    // Resource suchen und laden
    sal_Bool                GetResource( const ResId& rId, const Resource * = NULL );
    static void *       GetResourceSkipHeader( const ResId& rResId, ResMgr ** ppResMgr );
    // Kontext freigeben
    void                PopContext( const Resource* = NULL );

    // Resourcezeiger erhoehen
    void*               Increment( sal_uInt32 nSize );

    // Groesse ein Objektes in der Resource
    static sal_uInt32   GetObjSize( RSHEADER_TYPE* pHT )
                            { return( pHT->GetGlobOff() ); }

    // returns a string and its length out of the resource
    static sal_uInt32   GetString( UniString& rStr, const sal_uInt8* pStr );
    // returns a byte string and its length out of the resource
    static sal_uInt32   GetByteString( rtl::OString& rStr, const sal_uInt8* pStr );

    // Groesse eines Strings in der Resource
    static sal_uInt32   GetStringSize( sal_uInt32 nLen )
                            { nLen++; return (nLen + nLen%2); }
    static sal_uInt32   GetStringSize( const sal_uInt8* pStr, sal_uInt32& nLen );

    // return a int64
    static sal_uInt64   GetUInt64( void* pDatum );
    // Gibt einen long zurueck
    static sal_Int32        GetLong( void * pLong );
    // return a short
    static sal_Int16        GetShort( void * pShort );

    // Gibt einen Zeiger auf die Resource zurueck
    void *              GetClass();

    RSHEADER_TYPE *     CreateBlock( const ResId & rId );

    // Gibt die verbleibende Groesse zurueck
    sal_uInt32          GetRemainSize();

    const rtl::OUString&GetFileName() const;

    sal_Int16               ReadShort();
    sal_Int32               ReadLong();
    UniString           ReadString();
    rtl::OString        ReadByteString();

    // generate auto help id for current resource stack
    rtl::OString        GetAutoHelpId();

    static void         SetReadStringHook( ResHookProc pProc );
    static ResHookProc  GetReadStringHook();
    static void         SetDefaultLocale( const com::sun::star::lang::Locale& rLocale );
};

inline sal_uInt32 RSHEADER_TYPE::GetId()
{
    return (sal_uInt32)ResMgr::GetLong( &nId );
}
inline RESOURCE_TYPE RSHEADER_TYPE::GetRT()
{
    return (RESOURCE_TYPE)ResMgr::GetLong( &nRT );
}
inline sal_uInt32 RSHEADER_TYPE::GetGlobOff()
{
    return (sal_uInt32)ResMgr::GetLong( &nGlobOff );
}
inline sal_uInt32 RSHEADER_TYPE::GetLocalOff()
{
    return (sal_uInt32)ResMgr::GetLong( &nLocalOff );
}

#endif // _SV_RESMGR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
