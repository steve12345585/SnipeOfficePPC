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

#ifndef _SVARRAY_HXX
#define _SVARRAY_HXX

/***********************************************************************
*
*   Hier folgt die Beschreibung fuer die exportierten Makros:
*
*       SV_DECL_VARARR(nm, AE, IS, GS)
*       SV_IMPL_VARARR( nm, AE )
*           definiere/implementiere ein Array das einfache Objecte
*           enthaelt. (Sie werden im Speicher verschoben, koennen also
*           z.B. keine String sein)
*
*       SV_DECL_PTRARR(nm, AE, IS, GS)
*       SV_IMPL_PTRARR(nm, AE)
*           definiere/implementiere ein Array das Pointer haelt. Diese
*           werden von aussen angelegt und zerstoert. Das IMPL-Makro
*           wird nur benoetigt, wenn die DeleteAndDestroy Methode genutzt
*           wird, diese loescht dann die Pointer und ruft deren Destruktoren
*
*       SV_DECL_PTRARR_DEL(nm, AE, IS, GS)
*       SV_IMPL_PTRARR(nm, AE)
*           definiere/implementiere ein Array das Pointer haelt. Diese
*           werden von aussen angelegt und im Destructor zerstoert.
*
*
*       SV_DECL_PTRARR_SORT(nm, AE, IS, GS)
*       SV_IMPL_PTRARR_SORT( nm,AE )
*           defieniere/implementiere ein Sort-Array mit Pointern, das nach
*           Pointern sortiert ist. Basiert auf einem PTRARR
*
*       SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, GS)
*       SV_IMPL_PTRARR_SORT( nm,AE )
*           defieniere/implementiere ein Sort-Array mit Pointern, das nach
*           Pointern sortiert ist. Basiert auf einem PTRARR_DEL
*
*       SV_DECL_PTRARR_SORT(nm, AE, IS, GS)
*       SV_IMPL_OP_PTRARR_SORT( nm,AE )
*           defieniere/implementiere ein Sort-Array mit Pointern, das nach
*           Objecten sortiert ist. Basiert auf einem PTRARR.
*           Sortierung mit Hilfe der Object-operatoren "<" und "=="
*
*       SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, GS)
*       SV_IMPL_OP_PTRARR_SORT( nm,AE )
*           defieniere/implementiere ein Sort-Array mit Pointern, das nach
*           Objecten sortiert ist. Basiert auf einem PTRARR_DEL.
*           Sortierung mit Hilfe der Object-operatoren "<" und "=="
*
*       SV_DECL_VARARR_SORT(nm, AE, IS, GS)
*       SV_IMPL_VARARR_SORT( nm,AE )
*           defieniere/implementiere ein Sort-Array mit einfachen Objecten.
*           Basiert auf einem VARARR.
*           Sortierung mit Hilfe der Object-operatoren "<" und "=="
*
* JP 09.10.96:  vordefinierte Arrays:
*   SortArr:    SvStringsSort, SvStringsSortDtor,
*               SvStringsISort, SvStringsISortDtor
***********************************************************************/

#include "svl/svldllapi.h"

#ifndef INCLUDED_STRING_H
#include <string.h>     // memmove()
#define INCLUDED_STRING_H
#endif

#ifndef INCLUDED_LIMITS_H
#include <limits.h>     // USHRT_MAX
#define INCLUDED_LIMITS_H
#endif
#include <rtl/alloc.h>
#include <tools/solar.h>

class String;

#ifndef CONCAT
#define CONCAT(x,y) x##y
#endif

class DummyType;
inline void* operator new( size_t, DummyType* pPtr )
{
    return pPtr;
}
inline void operator delete( void*, DummyType* ) {}

#ifndef DBG_UTIL

#define _SVVARARR_DEF_GET_OP_INLINE( nm, ArrElem ) \
ArrElem& operator[](sal_uInt16 nP) const { return *(pData+nP); }\
\
void Insert( const nm * pI, sal_uInt16 nP,\
             sal_uInt16 nS = 0, sal_uInt16 nE = USHRT_MAX )\
{\
    if( USHRT_MAX == nE ) \
        nE = pI->nA; \
    if( nS < nE ) \
        Insert( (const ArrElem*)pI->pData+nS, (sal_uInt16)nE-nS, nP );\
}

#define _SVVARARR_IMPL_GET_OP_INLINE( nm, ArrElem )

#else // DBG_UTIL

#define _SVVARARR_DEF_GET_OP_INLINE( nm,ArrElem )\
ArrElem& operator[](sal_uInt16 nP) const;\
void Insert( const nm *pI, sal_uInt16 nP,\
                sal_uInt16 nS = 0, sal_uInt16 nE = USHRT_MAX );

#define _SVVARARR_IMPL_GET_OP_INLINE( nm, ArrElem )\
ArrElem& nm::operator[](sal_uInt16 nP) const\
{\
    OSL_ENSURE( pData && nP < nA,"Op[]");\
    return *(pData+nP);\
}\
void nm::Insert( const nm *pI, sal_uInt16 nP, sal_uInt16 nStt, sal_uInt16 nE)\
{\
    OSL_ENSURE(nP<=nA,"Ins,Ar[Start.End]");\
    if( USHRT_MAX == nE ) \
        nE = pI->nA; \
    if( nStt < nE ) \
        Insert( (const ArrElem*)pI->pData+nStt, (sal_uInt16)nE-nStt, nP );\
}

#endif // DBG_UTIL

#define _SV_DECL_VARARR_GEN(nm, AE, IS, AERef, vis )\
typedef sal_Bool (*FnForEach_##nm)( const AERef, void* );\
class vis nm\
{\
protected:\
    AE    *pData;\
    sal_uInt16 nFree;\
    sal_uInt16 nA;\
\
    void _resize(size_t n);\
\
public:\
    nm( sal_uInt16= IS );\
    ~nm() { rtl_freeMemory( pData ); }\
\
    _SVVARARR_DEF_GET_OP_INLINE(nm, AE )\
    AERef GetObject(sal_uInt16 nP) const { return (*this)[nP]; } \
\
    void Insert( const AERef aE, sal_uInt16 nP );\
    void Insert( const AE *pE, sal_uInt16 nL, sal_uInt16 nP );\
    void Remove( sal_uInt16 nP, sal_uInt16 nL = 1 );\
    void Replace( const AERef aE, sal_uInt16 nP );\
    void Replace( const AE *pE, sal_uInt16 nL, sal_uInt16 nP );\
    sal_uInt16 Count() const { return nA; }\
    const AE* GetData() const { return (const AE*)pData; }\
\
    void ForEach( CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( 0, nA, fnForEach, pArgs );\
    }\
    void ForEach( sal_uInt16 nS, sal_uInt16 nE, \
                    CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( nS, nE, fnForEach, pArgs );\
    }\
\
    void _ForEach( sal_uInt16 nStt, sal_uInt16 nE, \
            CONCAT( FnForEach_, nm ) fnCall, void* pArgs = 0 );\
\

#define SV_DECL_VARARR_GEN(nm, AE, IS, AERef, vis )\
_SV_DECL_VARARR_GEN(nm, AE, IS, AERef, vis )\
private:\
nm( const nm& );\
nm& operator=( const nm& );\
};

#define SV_DECL_VARARR(nm, AE, IS) \
SV_DECL_VARARR_GEN(nm, AE, IS, AE &, )

#define SV_DECL_VARARR_VISIBILITY(nm, AE, IS, vis ) \
SV_DECL_VARARR_GEN(nm, AE, IS, AE &, vis )

#define SV_IMPL_VARARR_GEN( nm, AE, AERef )\
nm::nm( sal_uInt16 nInit )\
    : pData (0),\
      nFree (nInit),\
      nA    (0)\
{\
    if( nInit )\
    {\
        pData = (AE*)(rtl_allocateMemory(sizeof(AE) * nInit));\
        OSL_ENSURE( pData, "CTOR, allocate");\
    }\
}\
\
void nm::_resize (size_t n)\
{\
    sal_uInt16 nL = ((n < USHRT_MAX) ? sal_uInt16(n) : USHRT_MAX);\
    AE* pE = (AE*)(rtl_reallocateMemory (pData, sizeof(AE) * nL));\
    if ((pE != 0) || (nL == 0))\
    {\
        pData = pE;\
        nFree = nL - nA;\
    }\
}\
\
void nm::Insert( const AERef aE, sal_uInt16 nP )\
{\
    OSL_ENSURE(nP <= nA && nA < USHRT_MAX, "Ins 1");\
    if (nFree < 1)\
        _resize (nA + ((nA > 1) ? nA : 1));\
    if( pData && nP < nA )\
        memmove( pData+nP+1, pData+nP, (nA-nP) * sizeof( AE ));\
    *(pData+nP) = (AE&)aE;\
    ++nA; --nFree;\
}\
\
void nm::Insert( const AE* pE, sal_uInt16 nL, sal_uInt16 nP )\
{\
    OSL_ENSURE(nP<=nA && ((long)nA+nL)<USHRT_MAX,"Ins n");\
    if (nFree < nL)\
        _resize (nA + ((nA > nL) ? nA : nL));\
    if( pData && nP < nA )\
        memmove( pData+nP+nL, pData+nP, (nA-nP) * sizeof( AE ));\
    if( pE )\
        memcpy( pData+nP, pE, nL * sizeof( AE ));\
    nA = nA + nL; nFree = nFree - nL;\
}\
\
void nm::Replace( const AERef aE, sal_uInt16 nP )\
{\
    if( nP < nA )\
        *(pData+nP) = (AE&)aE;\
}\
\
void nm::Replace( const AE *pE, sal_uInt16 nL, sal_uInt16 nP )\
{\
    if( pE && nP < nA )\
    {\
        if( nP + nL < nA )\
            memcpy( pData + nP, pE, nL * sizeof( AE ));\
        else if( nP + nL < nA + nFree )\
        {\
            memcpy( pData + nP, pE, nL * sizeof( AE ));\
            nP = nP + (nL - nA); \
            nFree = nP;\
        }\
        else \
        {\
            sal_uInt16 nTmpLen = nA + nFree - nP; \
            memcpy( pData + nP, pE, nTmpLen * sizeof( AE ));\
            nA = nA + nFree; \
            nFree = 0; \
            Insert( pE + nTmpLen, nL - nTmpLen, nA );\
        }\
    }\
}\
\
void nm::Remove( sal_uInt16 nP, sal_uInt16 nL )\
{\
    if( !nL )\
        return;\
    OSL_ENSURE( nP < nA && nP + nL <= nA,"Del");\
    if( pData && nP+1 < nA )\
        memmove( pData+nP, pData+nP+nL, (nA-nP-nL) * sizeof( AE ));\
    nA = nA - nL; nFree = nFree + nL;\
    if (nFree > nA)\
        _resize (nA);\
}\
\
void nm::_ForEach( sal_uInt16 nStt, sal_uInt16 nE, \
            CONCAT( FnForEach_, nm ) fnCall, void* pArgs )\
{\
    if( nStt >= nE || nE > nA )\
        return;\
    for( ; nStt < nE && (*fnCall)( *(const AE*)(pData+nStt), pArgs ); nStt++)\
        ;\
}\
\
_SVVARARR_IMPL_GET_OP_INLINE(nm, AE )\

#define SV_IMPL_VARARR( nm, AE ) \
SV_IMPL_VARARR_GEN( nm, AE, AE & )

#define _SV_DECL_PTRARR_DEF_GEN( nm, AE, IS, AERef, vis )\
_SV_DECL_VARARR_GEN( nm, AE, IS, AERef, vis)\
sal_uInt16 GetPos( const AERef aE ) const;\
};

#define _SV_DECL_PTRARR_DEF( nm, AE, IS, vis )\
_SV_DECL_PTRARR_DEF_GEN( nm, AE, IS, AE &, vis )

#define SV_DECL_PTRARR_GEN(nm, AE, IS, Base, AERef, VPRef, vis )\
typedef sal_Bool (*FnForEach_##nm)( const AERef, void* );\
class vis nm: public Base \
{\
public:\
    nm( sal_uInt16 nIni=IS )\
        : Base(nIni) {}\
    void Insert( const nm *pI, sal_uInt16 nP, \
            sal_uInt16 nS = 0, sal_uInt16 nE = USHRT_MAX ) {\
        Base::Insert((const Base*)pI, nP, nS, nE);\
    }\
    void Insert( const AERef aE, sal_uInt16 nP ) {\
        Base::Insert( (const VPRef )aE, nP );\
    }\
    void Insert( const AE *pE, sal_uInt16 nL, sal_uInt16 nP ) {\
        Base::Insert( (const VoidPtr*)pE, nL, nP );\
    }\
    void Replace( const AERef aE, sal_uInt16 nP ) {\
        Base::Replace( (const VPRef)aE, nP );\
    }\
    void Replace( const AE *pE, sal_uInt16 nL, sal_uInt16 nP ) {\
        Base::Replace( (const VoidPtr*)pE, nL, nP );\
    }\
    void Remove( sal_uInt16 nP, sal_uInt16 nL = 1) {\
        Base::Remove(nP,nL);\
    }\
    const AE* GetData() const {\
        return (const AE*)Base::GetData();\
    }\
    void ForEach( CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( 0, nA, (FnForEach_##Base)fnForEach, pArgs );\
    }\
    void ForEach( sal_uInt16 nS, sal_uInt16 nE, \
                    CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( nS, nE, (FnForEach_##Base)fnForEach, pArgs );\
    }\
    AE operator[]( sal_uInt16 nP )const  { \
        return (AE)Base::operator[](nP); }\
    AE GetObject(sal_uInt16 nP) const { \
        return (AE)Base::GetObject(nP); }\
    \
    sal_uInt16 GetPos( const AERef aE ) const { \
        return Base::GetPos((const VPRef)aE);\
    }\
    void DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL=1 );\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR(nm, AE, IS)\
SV_DECL_PTRARR_GEN(nm, AE, IS, SvPtrarr, AE &, VoidPtr &, )

#define SV_DECL_PTRARR_VISIBILITY(nm, AE, IS, vis)\
SV_DECL_PTRARR_GEN(nm, AE, IS, SvPtrarr, AE &, VoidPtr &, vis )

#define SV_DECL_PTRARR_DEL_GEN(nm, AE, IS, Base, AERef, VPRef, vis )\
typedef sal_Bool (*FnForEach_##nm)( const AERef, void* );\
class vis nm: public Base \
{\
public:\
    nm( sal_uInt16 nIni=IS )\
        : Base(nIni) {}\
    ~nm() { DeleteAndDestroy( 0, Count() ); }\
    void Insert( const nm *pI, sal_uInt16 nP, \
            sal_uInt16 nS = 0, sal_uInt16 nE = USHRT_MAX ) {\
        Base::Insert((const Base*)pI, nP, nS, nE);\
    }\
    void Insert( const AERef aE, sal_uInt16 nP ) {\
        Base::Insert((const VPRef)aE, nP );\
    }\
    void Insert( const AE *pE, sal_uInt16 nL, sal_uInt16 nP ) {\
        Base::Insert( (const VoidPtr *)pE, nL, nP );\
    }\
    void Replace( const AERef aE, sal_uInt16 nP ) {\
        Base::Replace( (const VPRef)aE, nP );\
    }\
    void Replace( const AE *pE, sal_uInt16 nL, sal_uInt16 nP ) {\
        Base::Replace( (const VoidPtr*)pE, nL, nP );\
    }\
    void Remove( sal_uInt16 nP, sal_uInt16 nL = 1) {\
        Base::Remove(nP,nL);\
    }\
    const AE* GetData() const {\
        return (const AE*)Base::GetData();\
    }\
    void ForEach( CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( 0, nA, (FnForEach_##Base)fnForEach, pArgs );\
    }\
    void ForEach( sal_uInt16 nS, sal_uInt16 nE, \
                    CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( nS, nE, (FnForEach_##Base)fnForEach, pArgs );\
    }\
    AE operator[]( sal_uInt16 nP )const  { \
        return (AE)Base::operator[](nP); }\
    AE GetObject( sal_uInt16 nP )const  { \
        return (AE)Base::GetObject(nP); }\
    \
    sal_uInt16 GetPos( const AERef aE ) const { \
        return Base::GetPos((const VPRef)aE);\
    } \
    void DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL=1 );\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR_DEL(nm, AE, IS)\
SV_DECL_PTRARR_DEL_GEN(nm, AE, IS, SvPtrarr, AE &, VoidPtr &, )

#define SV_DECL_PTRARR_DEL_VISIBILITY(nm, AE, IS, vis)\
SV_DECL_PTRARR_DEL_GEN(nm, AE, IS, SvPtrarr, AE &, VoidPtr &, vis)

#define SV_IMPL_PTRARR_GEN(nm, AE, Base)\
void nm::DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL )\
{ \
    if( nL ) {\
        OSL_ENSURE( nP < nA && nP + nL <= nA,"Del");\
        for( sal_uInt16 n=nP; n < nP + nL; n++ ) \
            delete *((AE*)pData+n); \
        Base::Remove( nP, nL ); \
    } \
}

#define SV_IMPL_PTRARR(nm, AE )\
SV_IMPL_PTRARR_GEN(nm, AE, SvPtrarr )

typedef void* VoidPtr;
_SV_DECL_PTRARR_DEF( SvPtrarr, VoidPtr, 0, SVL_DLLPUBLIC )

// SORTARR - Begin

#define _SORT_CLASS_DEF(nm, AE, IS, vis)\
typedef sal_Bool (*FnForEach_##nm)( const AE&, void* );\
class vis nm : private nm##_SAR \
{\
public:\
    nm(sal_uInt16 nSize = IS)\
        : nm##_SAR(nSize) {}\
    void Insert( const nm *pI, sal_uInt16 nS=0, sal_uInt16 nE=USHRT_MAX );\
    sal_Bool Insert( const AE& aE );\
    sal_Bool Insert( const AE& aE, sal_uInt16& rP );\
    void Insert( const AE *pE, sal_uInt16 nL );\
    void Remove( sal_uInt16 nP, sal_uInt16 nL = 1 );\
    void Remove( const AE& aE, sal_uInt16 nL = 1 );\
    sal_uInt16 Count() const  {   return nm##_SAR::Count(); }\
    const AE* GetData() const { return (const AE*)pData; }\
\
/* Das Ende stehe im DECL-Makro !!! */

#define _SV_SEEK_PTR(nm,AE)\
sal_Bool nm::Seek_Entry( const AE aE, sal_uInt16* pP ) const\
{\
    register sal_uInt16 nO  = nm##_SAR::Count(),\
            nM, \
            nU = 0;\
    if( nO > 0 )\
    {\
        nO--;\
        register long rCmp = (long)aE;\
        while( nU <= nO )\
        {\
            nM = nU + ( nO - nU ) / 2;\
            if( (long)*(pData + nM) == rCmp )\
            {\
                if( pP ) *pP = nM;\
                return sal_True;\
            }\
            else if( (long)*(pData+ nM) < (long)aE  )\
                nU = nM + 1;\
            else if( nM == 0 )\
            {\
                if( pP ) *pP = nU;\
                return sal_False;\
            }\
            else\
                nO = nM - 1;\
        }\
    }\
    if( pP ) *pP = nU;\
    return sal_False;\
}

#define _SV_SEEK_PTR_TO_OBJECT( nm,AE )\
sal_Bool nm::Seek_Entry( const AE aE, sal_uInt16* pP ) const\
{\
    register sal_uInt16 nO  = nm##_SAR::Count(),\
            nM, \
            nU = 0;\
    if( nO > 0 )\
    {\
        nO--;\
        while( nU <= nO )\
        {\
            nM = nU + ( nO - nU ) / 2;\
            if( *(*((AE*)pData + nM)) == *(aE) )\
            {\
                if( pP ) *pP = nM;\
                return sal_True;\
            }\
            else if( *(*((AE*)pData + nM)) < *(aE) )\
                nU = nM + 1;\
            else if( nM == 0 )\
            {\
                if( pP ) *pP = nU;\
                return sal_False;\
            }\
            else\
                nO = nM - 1;\
        }\
    }\
    if( pP ) *pP = nU;\
    return sal_False;\
}

#define _SV_SEEK_OBJECT( nm,AE )\
sal_Bool nm::Seek_Entry( const AE & aE, sal_uInt16* pP ) const\
{\
    register sal_uInt16 nO  = nm##_SAR::Count(),\
            nM, \
            nU = 0;\
    if( nO > 0 )\
    {\
        nO--;\
        while( nU <= nO )\
        {\
            nM = nU + ( nO - nU ) / 2;\
            if( *(pData + nM) == aE )\
            {\
                if( pP ) *pP = nM;\
                return sal_True;\
            }\
            else if( *(pData + nM) < aE )\
                nU = nM + 1;\
            else if( nM == 0 )\
            {\
                if( pP ) *pP = nU;\
                return sal_False;\
            }\
            else\
                nO = nM - 1;\
        }\
    }\
    if( pP ) *pP = nU;\
    return sal_False;\
}

#define _SV_IMPL_SORTAR_ALG(nm, AE)\
void nm::Insert( const nm * pI, sal_uInt16 nS, sal_uInt16 nE )\
{\
    if( USHRT_MAX == nE )\
        nE = pI->Count();\
    sal_uInt16 nP;\
    const AE * pIArr = pI->GetData();\
    for( ; nS < nE; ++nS )\
    {\
        if( ! Seek_Entry( *(pIArr+nS), &nP) )\
                nm##_SAR::Insert( *(pIArr+nS), nP );\
        if( ++nP >= Count() )\
        {\
            nm##_SAR::Insert( pI, nP, nS+1, nE );\
            nS = nE;\
        }\
    }\
}\
\
sal_Bool nm::Insert( const AE & aE )\
{\
    sal_uInt16 nP;\
    sal_Bool bExist;\
    bExist = Seek_Entry( aE, &nP );\
    if( ! bExist )\
        nm##_SAR::Insert( aE, nP );\
    return !bExist;\
}\
sal_Bool nm::Insert( const AE & aE, sal_uInt16& rP )\
{\
    sal_Bool bExist;\
    bExist = Seek_Entry( aE, &rP );\
    if( ! bExist )\
        nm##_SAR::Insert( aE, rP );\
    return !bExist;\
}\
void nm::Insert( const AE* pE, sal_uInt16 nL)\
{\
    sal_uInt16 nP;\
    for( sal_uInt16 n = 0; n < nL; ++n )\
        if( ! Seek_Entry( *(pE+n), &nP ))\
            nm##_SAR::Insert( *(pE+n), nP );\
}\
void nm::Remove( sal_uInt16 nP, sal_uInt16 nL )\
{\
    if( nL )\
        nm##_SAR::Remove( nP, nL);\
}\
\
void nm::Remove( const AE &aE, sal_uInt16 nL )\
{\
    sal_uInt16 nP;\
    if( nL && Seek_Entry( aE, &nP ) )   \
        nm##_SAR::Remove( nP, nL);\
}\

#define _SORTARR_BLC_CASTS(nm, AE )\
    sal_uInt16 GetPos( const AE& aE ) const { \
        return SvPtrarr::GetPos((const VoidPtr&)aE);\
    }

#define _SV_DECL_PTRARR_SORT_ALG(nm, AE, IS, vis)\
SV_DECL_PTRARR_VISIBILITY(nm##_SAR, AE, IS, vis)\
_SORT_CLASS_DEF(nm, AE, IS, vis)\
    AE operator[](sal_uInt16 nP) const {\
        return nm##_SAR::operator[]( nP );\
    }\
    AE GetObject(sal_uInt16 nP) const {\
        return nm##_SAR::GetObject( nP );\
    }\
    sal_Bool Seek_Entry( const AE aE, sal_uInt16* pP = 0 ) const;\
    void ForEach( CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( 0, nA, (FnForEach_SvPtrarr)fnForEach, pArgs );\
    }\
    void ForEach( sal_uInt16 nS, sal_uInt16 nE, \
                    CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( nS, nE, (FnForEach_SvPtrarr)fnForEach, pArgs );\
    }\
    void DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL=1 ); \
    _SORTARR_BLC_CASTS(nm, AE )\
\
/* Das Ende stehe im DECL-Makro !!! */

#define _SV_DECL_PTRARR_SORT(nm, AE, IS, vis)\
_SV_DECL_PTRARR_SORT_ALG(nm, AE, IS, vis)\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR_SORT(nm, AE, IS)\
_SV_DECL_PTRARR_SORT(nm, AE, IS,)

#define SV_DECL_PTRARR_SORT_VISIBILITY(nm, AE, IS, vis)\
_SV_DECL_PTRARR_SORT(nm, AE, IS, vis)


#define _SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, vis)\
_SV_DECL_PTRARR_SORT_ALG(nm, AE, IS, vis)\
    ~nm() { DeleteAndDestroy( 0, Count() ); }\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR_SORT_DEL(nm, AE, IS)\
_SV_DECL_PTRARR_SORT_DEL(nm, AE, IS,)

#define SV_DECL_PTRARR_SORT_DEL_VISIBILITY(nm, AE, IS, vis)\
_SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, vis)

#define _SV_DECL_VARARR_SORT(nm, AE, IS, vis)\
SV_DECL_VARARR_VISIBILITY(nm##_SAR, AE, IS, vis)\
_SORT_CLASS_DEF(nm, AE, IS, vis) \
    const AE& operator[](sal_uInt16 nP) const {\
        return nm##_SAR::operator[]( nP );\
    }\
    const AE& GetObject(sal_uInt16 nP) const {\
        return nm##_SAR::GetObject( nP );\
    }\
    sal_Bool Seek_Entry( const AE & aE, sal_uInt16* pP = 0 ) const;\
    void ForEach( CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( 0, nA, (FnForEach_##nm##_SAR)fnForEach, pArgs );\
    }\
    void ForEach( sal_uInt16 nS, sal_uInt16 nE, \
                    CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( nS, nE, (FnForEach_##nm##_SAR)fnForEach, pArgs );\
    }\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_VARARR_SORT(nm, AE, IS)\
_SV_DECL_VARARR_SORT(nm, AE, IS,)

#define SV_DECL_VARARR_SORT_VISIBILITY(nm, AE, IS, vis)\
_SV_DECL_VARARR_SORT(nm, AE, IS, vis)

#define SV_IMPL_PTRARR_SORT( nm,AE )\
_SV_IMPL_SORTAR_ALG( nm,AE )\
    void nm::DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL ) { \
        if( nL ) {\
            OSL_ENSURE( nP < nA && nP + nL <= nA, "ERR_VAR_DEL" );\
            for( sal_uInt16 n=nP; n < nP + nL; n++ ) \
                delete *((AE*)pData+n); \
            SvPtrarr::Remove( nP, nL ); \
        } \
    } \
_SV_SEEK_PTR( nm, AE )

#define SV_IMPL_OP_PTRARR_SORT( nm,AE )\
_SV_IMPL_SORTAR_ALG( nm,AE )\
    void nm::DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL ) { \
        if( nL ) {\
            OSL_ENSURE( nP < nA && nP + nL <= nA, "ERR_VAR_DEL" );\
            for( sal_uInt16 n=nP; n < nP + nL; n++ ) \
                delete *((AE*)pData+n); \
            SvPtrarr::Remove( nP, nL ); \
        } \
    } \
_SV_SEEK_PTR_TO_OBJECT( nm,AE )

#define SV_IMPL_VARARR_SORT( nm,AE )\
SV_IMPL_VARARR(nm##_SAR, AE)\
_SV_IMPL_SORTAR_ALG( nm,AE )\
_SV_SEEK_OBJECT( nm,AE )

#if defined(ICC) || defined(GCC) || (defined(WNT) && _MSC_VER >= 1400)
#define C40_INSERT( c, p, n ) Insert( (c const *&) p, n )
#define C40_PTR_INSERT( c, p ) Insert( (c const *&) p )
#define C40_REPLACE( c, p, n ) Replace( (c const *&) p, n )
#define C40_GETPOS( c, r) GetPos( (c const *&) r )
#else
#define C40_INSERT( c, p, n ) Insert( p, n )
#define C40_PTR_INSERT( c, p ) Insert( p )
#define C40_REPLACE( c, p, n ) Replace( p, n )
#define C40_GETPOS( c, r) GetPos( r )
#endif

#endif  //_SVARRAY_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
