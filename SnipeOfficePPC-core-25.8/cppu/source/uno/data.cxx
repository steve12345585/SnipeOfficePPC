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


#include <cstddef>
#include <stdio.h>

#include "cppu/macros.hxx"

#include "osl/mutex.hxx"

#include "constr.hxx"
#include "destr.hxx"
#include "copy.hxx"
#include "assign.hxx"
#include "eq.hxx"

#include "boost/static_assert.hpp"


using namespace ::cppu;
using namespace ::rtl;
using namespace ::osl;


namespace cppu
{

// Sequence<>() (default ctor) relies on this being static:
uno_Sequence g_emptySeq = { 1, 0, { 0 } };
typelib_TypeDescriptionReference * g_pVoidType = 0;

//--------------------------------------------------------------------------------------------------
void * binuno_queryInterface( void * pUnoI, typelib_TypeDescriptionReference * pDestType )
{
    // init queryInterface() td
    static typelib_TypeDescription * g_pQITD = 0;
    if (0 == g_pQITD)
    {
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        if (0 == g_pQITD)
        {
            typelib_TypeDescriptionReference * type_XInterface =
                * typelib_static_type_getByTypeClass( typelib_TypeClass_INTERFACE );
            typelib_InterfaceTypeDescription * pTXInterfaceDescr = 0;
            TYPELIB_DANGER_GET( (typelib_TypeDescription **) &pTXInterfaceDescr, type_XInterface );
            OSL_ASSERT( pTXInterfaceDescr->ppAllMembers );
            typelib_typedescriptionreference_getDescription(
                &g_pQITD, pTXInterfaceDescr->ppAllMembers[ 0 ] );
            TYPELIB_DANGER_RELEASE( (typelib_TypeDescription *) pTXInterfaceDescr );
        }
    }

    uno_Any aRet, aExc;
    uno_Any * pExc = &aExc;
    void * aArgs[ 1 ];
    aArgs[ 0 ] = &pDestType;
    (*((uno_Interface *) pUnoI)->pDispatcher)(
        (uno_Interface *) pUnoI, g_pQITD, &aRet, aArgs, &pExc );

    uno_Interface * ret = 0;
    if (0 == pExc)
    {
        typelib_TypeDescriptionReference * ret_type = aRet.pType;
        switch (ret_type->eTypeClass)
        {
        case typelib_TypeClass_VOID: // common case
            typelib_typedescriptionreference_release( ret_type );
            break;
        case typelib_TypeClass_INTERFACE:
            // tweaky... avoiding acquire/ release pair
            typelib_typedescriptionreference_release( ret_type );
            ret = (uno_Interface *) aRet.pReserved; // serving acquired interface
            break;
        default:
            _destructAny( &aRet, 0 );
            break;
        }
    }
    else
    {
#if OSL_DEBUG_LEVEL > 1
        OUStringBuffer buf( 128 );
        buf.appendAscii(
            RTL_CONSTASCII_STRINGPARAM("### exception occurred querying for interface ") );
        buf.append( * reinterpret_cast< OUString const * >( &pDestType->pTypeName ) );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(": [") );
        buf.append( * reinterpret_cast< OUString const * >( &pExc->pType->pTypeName ) );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("] ") );
        // Message is very first member
        buf.append( * reinterpret_cast< OUString const * >( pExc->pData ) );
        OString cstr(
            OUStringToOString( buf.makeStringAndClear(), RTL_TEXTENCODING_ASCII_US ) );
        OSL_FAIL( cstr.getStr() );
#endif
        uno_any_destruct( pExc, 0 );
    }
    return ret;
}

//==================================================================================================
void defaultConstructStruct(
    void * pMem,
    typelib_CompoundTypeDescription * pCompType )
    SAL_THROW(())
{
    _defaultConstructStruct( pMem, pCompType );
}
//==================================================================================================
void copyConstructStruct(
    void * pDest, void * pSource,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_AcquireFunc acquire, uno_Mapping * mapping )
    SAL_THROW(())
{
    _copyConstructStruct( pDest, pSource, pTypeDescr, acquire, mapping );
}
//==================================================================================================
void destructStruct(
    void * pValue,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_ReleaseFunc release )
    SAL_THROW(())
{
    _destructStruct( pValue, pTypeDescr, release );
}
//==================================================================================================
sal_Bool equalStruct(
    void * pDest, void *pSource,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW(())
{
    return _equalStruct( pDest, pSource, pTypeDescr, queryInterface, release );
}
//==================================================================================================
sal_Bool assignStruct(
    void * pDest, void * pSource,
    typelib_CompoundTypeDescription * pTypeDescr,
    uno_QueryInterfaceFunc queryInterface, uno_AcquireFunc acquire, uno_ReleaseFunc release )
    SAL_THROW(())
{
    return _assignStruct( pDest, pSource, pTypeDescr, queryInterface, acquire, release );
}

//==============================================================================
uno_Sequence * copyConstructSequence(
    uno_Sequence * pSource,
    typelib_TypeDescriptionReference * pElementType,
    uno_AcquireFunc acquire, uno_Mapping * mapping )
{
    return icopyConstructSequence( pSource, pElementType, acquire, mapping );
}

//==============================================================================
void destructSequence(
    uno_Sequence * pSequence,
    typelib_TypeDescriptionReference * pType,
    typelib_TypeDescription * pTypeDescr,
    uno_ReleaseFunc release )
{
    idestructSequence( pSequence, pType, pTypeDescr, release );
}

//==================================================================================================
sal_Bool equalSequence(
    uno_Sequence * pDest, uno_Sequence * pSource,
    typelib_TypeDescriptionReference * pElementType,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW(())
{
    return _equalSequence( pDest, pSource, pElementType, queryInterface, release );
}

extern "C"
{
//##################################################################################################
CPPU_DLLPUBLIC void SAL_CALL uno_type_constructData(
    void * pMem, typelib_TypeDescriptionReference * pType )
    SAL_THROW_EXTERN_C()
{
    _defaultConstructData( pMem, pType, 0 );
}
//##################################################################################################
CPPU_DLLPUBLIC void SAL_CALL uno_constructData(
    void * pMem, typelib_TypeDescription * pTypeDescr )
    SAL_THROW_EXTERN_C()
{
    _defaultConstructData( pMem, pTypeDescr->pWeakRef, pTypeDescr );
}
//##################################################################################################
CPPU_DLLPUBLIC void SAL_CALL uno_type_destructData(
    void * pValue, typelib_TypeDescriptionReference * pType,
    uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    _destructData( pValue, pType, 0, release );
}
//##################################################################################################
CPPU_DLLPUBLIC void SAL_CALL uno_destructData(
    void * pValue,
    typelib_TypeDescription * pTypeDescr,
    uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    _destructData( pValue, pTypeDescr->pWeakRef, pTypeDescr, release );
}
//##################################################################################################
CPPU_DLLPUBLIC void SAL_CALL uno_type_copyData(
    void * pDest, void * pSource,
    typelib_TypeDescriptionReference * pType,
    uno_AcquireFunc acquire )
    SAL_THROW_EXTERN_C()
{
    _copyConstructData( pDest, pSource, pType, 0, acquire, 0 );
}
//##################################################################################################
CPPU_DLLPUBLIC void SAL_CALL uno_copyData(
    void * pDest, void * pSource,
    typelib_TypeDescription * pTypeDescr,
    uno_AcquireFunc acquire )
    SAL_THROW_EXTERN_C()
{
    _copyConstructData( pDest, pSource, pTypeDescr->pWeakRef, pTypeDescr, acquire, 0 );
}
//##################################################################################################
CPPU_DLLPUBLIC void SAL_CALL uno_type_copyAndConvertData(
    void * pDest, void * pSource,
    typelib_TypeDescriptionReference * pType,
    uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    _copyConstructData( pDest, pSource, pType, 0, 0, mapping );
}
//##################################################################################################
CPPU_DLLPUBLIC void SAL_CALL uno_copyAndConvertData(
    void * pDest, void * pSource,
    typelib_TypeDescription * pTypeDescr,
    uno_Mapping * mapping )
    SAL_THROW_EXTERN_C()
{
    _copyConstructData( pDest, pSource, pTypeDescr->pWeakRef, pTypeDescr, 0, mapping );
}
//##################################################################################################
CPPU_DLLPUBLIC sal_Bool SAL_CALL uno_type_equalData(
    void * pVal1, typelib_TypeDescriptionReference * pVal1Type,
    void * pVal2, typelib_TypeDescriptionReference * pVal2Type,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    return _equalData(
        pVal1, pVal1Type, 0,
        pVal2, pVal2Type,
        queryInterface, release );
}
//##################################################################################################
CPPU_DLLPUBLIC sal_Bool SAL_CALL uno_equalData(
    void * pVal1, typelib_TypeDescription * pVal1TD,
    void * pVal2, typelib_TypeDescription * pVal2TD,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    return _equalData(
        pVal1, pVal1TD->pWeakRef, pVal1TD,
        pVal2, pVal2TD->pWeakRef,
        queryInterface, release );
}
//##################################################################################################
CPPU_DLLPUBLIC sal_Bool SAL_CALL uno_type_assignData(
    void * pDest, typelib_TypeDescriptionReference * pDestType,
    void * pSource, typelib_TypeDescriptionReference * pSourceType,
    uno_QueryInterfaceFunc queryInterface, uno_AcquireFunc acquire, uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    return _assignData(
        pDest, pDestType, 0,
        pSource, pSourceType, 0,
        queryInterface, acquire, release );
}
//##################################################################################################
CPPU_DLLPUBLIC sal_Bool SAL_CALL uno_assignData(
    void * pDest, typelib_TypeDescription * pDestTD,
    void * pSource, typelib_TypeDescription * pSourceTD,
    uno_QueryInterfaceFunc queryInterface, uno_AcquireFunc acquire, uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    return _assignData(
        pDest, pDestTD->pWeakRef, pDestTD,
        pSource, pSourceTD->pWeakRef, pSourceTD,
        queryInterface, acquire, release );
}
//##################################################################################################
CPPU_DLLPUBLIC sal_Bool SAL_CALL uno_type_isAssignableFromData(
    typelib_TypeDescriptionReference * pAssignable,
    void * pFrom, typelib_TypeDescriptionReference * pFromType,
    uno_QueryInterfaceFunc queryInterface, uno_ReleaseFunc release )
    SAL_THROW_EXTERN_C()
{
    if (::typelib_typedescriptionreference_isAssignableFrom( pAssignable, pFromType ))
        return sal_True;
    if (typelib_TypeClass_INTERFACE != pFromType->eTypeClass ||
        typelib_TypeClass_INTERFACE != pAssignable->eTypeClass)
    {
        return sal_False;
    }

    // query
    if (0 == pFrom)
        return sal_False;
    void * pInterface = *(void **)pFrom;
    if (0 == pInterface)
        return sal_False;

    if (0 == queryInterface)
        queryInterface = binuno_queryInterface;
    void * p = (*queryInterface)( pInterface, pAssignable );
    _release( p, release );
    return (0 != p);
}
}


//##################################################################################################
//##################################################################################################
//##################################################################################################


#if OSL_DEBUG_LEVEL > 1

#if defined( SAL_W32)
#pragma pack(push, 8)
#endif

// Why hardcode like this instead of using the (generated)
// <sal/typesizes.h> ?

#if (defined(INTEL) \
    && (defined(__GNUC__) && (defined(LINUX) || defined(FREEBSD) || defined(NETBSD) || defined(OPENBSD)) \
        || defined(MACOSX) || defined(DRAGONFLY) || (defined(__SUNPRO_CC) && defined(SOLARIS)))) \
    || defined(IOS)
#define MAX_ALIGNMENT_4
#endif

#define OFFSET_OF( s, m ) reinterpret_cast< std::size_t >((char *)&((s *)16)->m -16)

#define BINTEST_VERIFY( c ) \
    if (! (c)) { fprintf( stderr, "### binary compatibility test failed: %s [line %d]!!!\n", #c, __LINE__ ); abort(); }
#define BINTEST_VERIFYOFFSET( s, m, n ) \
    if (OFFSET_OF(s, m) != n) { fprintf( stderr, "### OFFSET_OF(" #s ", "  #m ") = %" SAL_PRI_SIZET "u instead of expected %" SAL_PRI_SIZET "u!!!\n", OFFSET_OF(s, m), n ); abort(); }

#define BINTEST_VERIFYSIZE( s, n ) \
    if (sizeof(s) != n) { fprintf( stderr, "### sizeof(" #s ") = %" SAL_PRI_SIZET "u instead of expected %" SAL_PRI_SIZET "u!!!\n", sizeof(s), n ); abort(); }

struct C1
{
    sal_Int16 n1;
};
struct C2 : public C1
{
    sal_Int32 n2 CPPU_GCC3_ALIGN( C1 );
};
struct C3 : public C2
{
    double d3;
    sal_Int32 n3;
};
struct C4 : public C3
{
    sal_Int32 n4 CPPU_GCC3_ALIGN( C3 );
    double d4;
};
struct C5 : public C4
{
    sal_Int64 n5;
    sal_Bool b5;
};
struct C6 : public C1
{
    C5 c6 CPPU_GCC3_ALIGN( C1 );
    sal_Bool b6;
};

struct D
{
    sal_Int16 d;
    sal_Int32 e;
};
struct E
{
    sal_Bool a;
    sal_Bool b;
    sal_Bool c;
    sal_Int16 d;
    sal_Int32 e;
};

struct M
{
    sal_Int32   n;
    sal_Int16   o;
};

struct N : public M
{
    sal_Int16   p CPPU_GCC3_ALIGN( M );
};
struct N2
{
    M m;
    sal_Int16   p;
};

struct O : public M
{
    double  p;
    sal_Int16 q;
};
struct O2 : public O
{
    sal_Int16 p2 CPPU_GCC3_ALIGN( O );
};

struct P : public N
{
    double  p2;
};

struct empty
{
};
struct second : public empty
{
    int a;
};

struct AlignSize_Impl
{
    sal_Int16   nInt16;
    double      dDouble;
};

struct Char1
{
    char c1;
};
struct Char2 : public Char1
{
    char c2 CPPU_GCC3_ALIGN( Char1 );
};
struct Char3 : public Char2
{
    char c3 CPPU_GCC3_ALIGN( Char2 );
};
struct Char4
{
    Char3 chars;
    char c;
};
class Ref
{
    void * p;
};
enum Enum
{
    v = SAL_MAX_ENUM
};


class BinaryCompatible_Impl
{
public:
    BinaryCompatible_Impl();
};
BinaryCompatible_Impl::BinaryCompatible_Impl()
{
    BOOST_STATIC_ASSERT( ((sal_Bool) true) == sal_True &&
                         (1 != 0) == sal_True );
    BOOST_STATIC_ASSERT( ((sal_Bool) false) == sal_False &&
                         (1 == 0) == sal_False );
#ifdef MAX_ALIGNMENT_4
    // max alignment is 4
    BINTEST_VERIFYOFFSET( AlignSize_Impl, dDouble, std::size_t(4) );
    BINTEST_VERIFYSIZE( AlignSize_Impl, std::size_t(12) );
#else
    // max alignment is 8
    BINTEST_VERIFYOFFSET( AlignSize_Impl, dDouble, std::size_t(8) );
    BINTEST_VERIFYSIZE( AlignSize_Impl, std::size_t(16) );
#endif

    // sequence
    BINTEST_VERIFY( (SAL_SEQUENCE_HEADER_SIZE % 8) == 0 );
    // enum
    BINTEST_VERIFY( sizeof( Enum ) == sizeof( sal_Int32 ) );
    // any
    BINTEST_VERIFY( sizeof(void *) >= sizeof(sal_Int32) );
    BINTEST_VERIFY( sizeof( uno_Any ) == sizeof( void * ) * 3 );
    BINTEST_VERIFYOFFSET( uno_Any, pType, std::size_t(0) );
    BINTEST_VERIFYOFFSET( uno_Any, pData, 1 * sizeof (void *) );
    BINTEST_VERIFYOFFSET( uno_Any, pReserved, 2 * sizeof (void *) );
    // interface
    BINTEST_VERIFY( sizeof( Ref ) == sizeof( void * ) );
    // string
    BINTEST_VERIFY( sizeof( OUString ) == sizeof( rtl_uString * ) );
    // struct
    BINTEST_VERIFYSIZE( M, std::size_t(8) );
    BINTEST_VERIFYOFFSET( M, o, std::size_t(4) );
    BINTEST_VERIFYSIZE( N, std::size_t(12) );
    BINTEST_VERIFYOFFSET( N, p, std::size_t(8) );
    BINTEST_VERIFYSIZE( N2, std::size_t(12) );
    BINTEST_VERIFYOFFSET( N2, p, std::size_t(8) );
#ifdef MAX_ALIGNMENT_4
    BINTEST_VERIFYSIZE( O, std::size_t(20) );
#else
    BINTEST_VERIFYSIZE( O, std::size_t(24) );
#endif
    BINTEST_VERIFYSIZE( D, std::size_t(8) );
    BINTEST_VERIFYOFFSET( D, e, std::size_t(4) );
    BINTEST_VERIFYOFFSET( E, d, std::size_t(4) );
    BINTEST_VERIFYOFFSET( E, e, std::size_t(8) );

    BINTEST_VERIFYSIZE( C1, std::size_t(2) );
    BINTEST_VERIFYSIZE( C2, std::size_t(8) );
    BINTEST_VERIFYOFFSET( C2, n2, std::size_t(4) );

#ifdef MAX_ALIGNMENT_4
    BINTEST_VERIFYSIZE( C3, std::size_t(20) );
    BINTEST_VERIFYOFFSET( C3, d3, std::size_t(8) );
    BINTEST_VERIFYOFFSET( C3, n3, std::size_t(16) );
    BINTEST_VERIFYSIZE( C4, std::size_t(32) );
    BINTEST_VERIFYOFFSET( C4, n4, std::size_t(20) );
    BINTEST_VERIFYOFFSET( C4, d4, std::size_t(24) );
    BINTEST_VERIFYSIZE( C5, std::size_t(44) );
    BINTEST_VERIFYOFFSET( C5, n5, std::size_t(32) );
    BINTEST_VERIFYOFFSET( C5, b5, std::size_t(40) );
    BINTEST_VERIFYSIZE( C6, std::size_t(52) );
    BINTEST_VERIFYOFFSET( C6, c6, std::size_t(4) );
    BINTEST_VERIFYOFFSET( C6, b6, std::size_t(48) );

    BINTEST_VERIFYSIZE( O2, std::size_t(24) );
    BINTEST_VERIFYOFFSET( O2, p2, std::size_t(20) );
#else
    BINTEST_VERIFYSIZE( C3, std::size_t(24) );
    BINTEST_VERIFYOFFSET( C3, d3, std::size_t(8) );
    BINTEST_VERIFYOFFSET( C3, n3, std::size_t(16) );
    BINTEST_VERIFYSIZE( C4, std::size_t(40) );
    BINTEST_VERIFYOFFSET( C4, n4, std::size_t(24) );
    BINTEST_VERIFYOFFSET( C4, d4, std::size_t(32) );
    BINTEST_VERIFYSIZE( C5, std::size_t(56) );
    BINTEST_VERIFYOFFSET( C5, n5, std::size_t(40) );
    BINTEST_VERIFYOFFSET( C5, b5, std::size_t(48) );
    BINTEST_VERIFYSIZE( C6, std::size_t(72) );
    BINTEST_VERIFYOFFSET( C6, c6, std::size_t(8) );
    BINTEST_VERIFYOFFSET( C6, b6, std::size_t(64) );

    BINTEST_VERIFYSIZE( O2, std::size_t(32) );
    BINTEST_VERIFYOFFSET( O2, p2, std::size_t(24) );
#endif

    BINTEST_VERIFYSIZE( Char3, std::size_t(3) );
    BINTEST_VERIFYOFFSET( Char4, c, std::size_t(3) );

#ifdef MAX_ALIGNMENT_4
    // max alignment is 4
    BINTEST_VERIFYSIZE( P, std::size_t(20) );
#else
    // alignment of P is 8, because of P[] ...
    BINTEST_VERIFYSIZE( P, std::size_t(24) );
    BINTEST_VERIFYSIZE( second, sizeof( int ) );
#endif
}

#ifdef SAL_W32
#   pragma pack(pop)
#endif

static BinaryCompatible_Impl aTest;

#endif

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
