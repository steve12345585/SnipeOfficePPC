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


#pragma warning( disable : 4237 )
#include <boost/unordered_map.hpp>
#include <sal/config.h>
#include <malloc.h>
#include <new.h>
#include <typeinfo.h>
#include <signal.h>

#include "rtl/alloc.h"
#include "rtl/strbuf.hxx"
#include "rtl/ustrbuf.hxx"

#include "com/sun/star/uno/Any.hxx"

#include "msci.hxx"


#pragma pack(push, 8)

using namespace ::com::sun::star::uno;
using namespace ::std;
using namespace ::osl;
using namespace ::rtl;

namespace CPPU_CURRENT_NAMESPACE
{

//==================================================================================================
static inline OUString toUNOname( OUString const & rRTTIname ) throw ()
{
    OUStringBuffer aRet( 64 );
    OUString aStr( rRTTIname.copy( 4, rRTTIname.getLength()-4-2 ) ); // filter .?AUzzz@yyy@xxx@@
    sal_Int32 nPos = aStr.getLength();
    while (nPos > 0)
    {
        sal_Int32 n = aStr.lastIndexOf( '@', nPos );
        aRet.append( aStr.copy( n +1, nPos -n -1 ) );
        if (n >= 0)
        {
            aRet.append( (sal_Unicode)'.' );
        }
        nPos = n;
    }
    return aRet.makeStringAndClear();
}
//==================================================================================================
static inline OUString toRTTIname( OUString const & rUNOname ) throw ()
{
    OUStringBuffer aRet( 64 );
    aRet.appendAscii( RTL_CONSTASCII_STRINGPARAM(".?AV") ); // class ".?AV"; struct ".?AU"
    sal_Int32 nPos = rUNOname.getLength();
    while (nPos > 0)
    {
        sal_Int32 n = rUNOname.lastIndexOf( '.', nPos );
        aRet.append( rUNOname.copy( n +1, nPos -n -1 ) );
        aRet.append( (sal_Unicode)'@' );
        nPos = n;
    }
    aRet.append( (sal_Unicode)'@' );
    return aRet.makeStringAndClear();
}


//##################################################################################################
//#### RTTI simulation #############################################################################
//##################################################################################################


typedef boost::unordered_map< OUString, void *, OUStringHash, equal_to< OUString > > t_string2PtrMap;

//==================================================================================================
class RTTInfos
{
    Mutex               _aMutex;
    t_string2PtrMap     _allRTTI;

    static OUString toRawName( OUString const & rUNOname ) throw ();
public:
    type_info * getRTTI( OUString const & rUNOname ) throw ();

    RTTInfos();
    ~RTTInfos();
};

//==================================================================================================
class __type_info
{
    friend type_info * RTTInfos::getRTTI( OUString const & ) throw ();
    friend int msci_filterCppException(
        LPEXCEPTION_POINTERS, uno_Any *, uno_Mapping * );

public:
    virtual ~__type_info() throw ();

    inline __type_info( void * m_data, const char * m_d_name ) throw ()
        : _m_data( m_data )
        { ::strcpy( _m_d_name, m_d_name ); } // #100211# - checked

private:
    void * _m_data;
    char _m_d_name[1];
};
//__________________________________________________________________________________________________
__type_info::~__type_info() throw ()
{
}
//__________________________________________________________________________________________________
type_info * RTTInfos::getRTTI( OUString const & rUNOname ) throw ()
{
    // a must be
    OSL_ENSURE( sizeof(__type_info) == sizeof(type_info), "### type info structure size differ!" );

    MutexGuard aGuard( _aMutex );
    t_string2PtrMap::const_iterator const iFind( _allRTTI.find( rUNOname ) );

    // check if type is already available
    if (iFind == _allRTTI.end())
    {
        // insert new type_info
        OString aRawName( OUStringToOString( toRTTIname( rUNOname ), RTL_TEXTENCODING_ASCII_US ) );
        __type_info * pRTTI = new( ::rtl_allocateMemory( sizeof(__type_info) + aRawName.getLength() ) )
            __type_info( NULL, aRawName.getStr() );

        // put into map
        pair< t_string2PtrMap::iterator, bool > insertion(
            _allRTTI.insert( t_string2PtrMap::value_type( rUNOname, pRTTI ) ) );
        OSL_ENSURE( insertion.second, "### rtti insertion failed?!" );

        return (type_info *)pRTTI;
    }
    else
    {
        return (type_info *)iFind->second;
    }
}
//__________________________________________________________________________________________________
RTTInfos::RTTInfos() throw ()
{
}
//__________________________________________________________________________________________________
RTTInfos::~RTTInfos() throw ()
{
#if OSL_DEBUG_LEVEL > 1
    OSL_TRACE( "> freeing generated RTTI infos... <" );
#endif

    MutexGuard aGuard( _aMutex );
    for ( t_string2PtrMap::const_iterator iPos( _allRTTI.begin() );
          iPos != _allRTTI.end(); ++iPos )
    {
        __type_info * pType = (__type_info *)iPos->second;
        pType->~__type_info(); // obsolete, but good style...
        ::rtl_freeMemory( pType );
    }
}


//##################################################################################################
//#### Exception raising ###########################################################################
//##################################################################################################


//==================================================================================================
struct ObjectFunction
{
    char somecode[12];
    typelib_TypeDescription * _pTypeDescr; // type of object

    inline static void * operator new ( size_t nSize );
    inline static void operator delete ( void * pMem );

    ObjectFunction( typelib_TypeDescription * pTypeDescr, void * fpFunc ) throw ();
    ~ObjectFunction() throw ();
};

inline void * ObjectFunction::operator new ( size_t nSize )
{
    void * pMem = rtl_allocateMemory( nSize );
    if (pMem != 0)
    {
        DWORD old_protect;
#if OSL_DEBUG_LEVEL > 0
        BOOL success =
#endif
        VirtualProtect( pMem, nSize, PAGE_EXECUTE_READWRITE, &old_protect );
        OSL_ENSURE( success, "VirtualProtect() failed!" );
    }
    return pMem;
}

inline void ObjectFunction::operator delete ( void * pMem )
{
    rtl_freeMemory( pMem );
}

//__________________________________________________________________________________________________
ObjectFunction::ObjectFunction( typelib_TypeDescription * pTypeDescr, void * fpFunc ) throw ()
    : _pTypeDescr( pTypeDescr )
{
    ::typelib_typedescription_acquire( _pTypeDescr );

    unsigned char * pCode = (unsigned char *)somecode;
    // a must be!
    OSL_ENSURE( (void *)this == (void *)pCode, "### unexpected!" );

    // push ObjectFunction this
    *pCode++ = 0x68;
    *(void **)pCode = this;
    pCode += sizeof(void *);
    // jmp rel32 fpFunc
    *pCode++ = 0xe9;
    *(sal_Int32 *)pCode = ((unsigned char *)fpFunc) - pCode - sizeof(sal_Int32);
}
//__________________________________________________________________________________________________
ObjectFunction::~ObjectFunction() throw ()
{
    ::typelib_typedescription_release( _pTypeDescr );
}

//==================================================================================================
static void * __cdecl __copyConstruct( void * pExcThis, void * pSource, ObjectFunction * pThis )
    throw ()
{
    ::uno_copyData( pExcThis, pSource, pThis->_pTypeDescr, cpp_acquire );
    return pExcThis;
}
//==================================================================================================
static void * __cdecl __destruct( void * pExcThis, ObjectFunction * pThis )
    throw ()
{
    ::uno_destructData( pExcThis, pThis->_pTypeDescr, cpp_release );
    return pExcThis;
}

// these are non virtual object methods; there is no this ptr on stack => ecx supplies _this_ ptr

//==================================================================================================
static __declspec(naked) void copyConstruct() throw ()
{
    __asm
    {
        // ObjectFunction this already on stack
        push [esp+8]  // source exc object this
        push ecx      // exc object
        call __copyConstruct
        add  esp, 12  // + ObjectFunction this
        ret  4
    }
}
//==================================================================================================
static __declspec(naked) void destruct() throw ()
{
    __asm
    {
        // ObjectFunction this already on stack
        push ecx    // exc object
        call __destruct
        add  esp, 8 // + ObjectFunction this
        ret
    }
}

//==================================================================================================
struct ExceptionType
{
    sal_Int32           _n0;
    type_info *         _pTypeInfo;
    sal_Int32           _n1, _n2, _n3, _n4;
    ObjectFunction *    _pCopyCtor;
    sal_Int32           _n5;

    inline ExceptionType( typelib_TypeDescription * pTypeDescr ) throw ()
        : _n0( 0 )
        , _n1( 0 )
        , _n2( -1 )
        , _n3( 0 )
        , _n4( pTypeDescr->nSize )
        , _pCopyCtor( new ObjectFunction( pTypeDescr, copyConstruct ) )
        , _n5( 0 )
        { _pTypeInfo = msci_getRTTI( pTypeDescr->pTypeName ); }
    inline ~ExceptionType() throw ()
        { delete _pCopyCtor; }
};
//==================================================================================================
struct RaiseInfo
{
    sal_Int32           _n0;
    ObjectFunction *    _pDtor;
    sal_Int32           _n2;
    void *              _types;
    sal_Int32           _n3, _n4;

    RaiseInfo( typelib_TypeDescription * pTypeDescr ) throw ();
    ~RaiseInfo() throw ();
};
//__________________________________________________________________________________________________
RaiseInfo::RaiseInfo( typelib_TypeDescription * pTypeDescr ) throw ()
    : _n0( 0 )
    , _pDtor( new ObjectFunction( pTypeDescr, destruct ) )
    , _n2( 0 )
    , _n3( 0 )
    , _n4( 0 )
{
    // a must be
    OSL_ENSURE( sizeof(sal_Int32) == sizeof(ExceptionType *), "### pointer size differs from sal_Int32!" );

    typelib_CompoundTypeDescription * pCompTypeDescr;

    // info count
    sal_Int32 nLen = 0;
    for ( pCompTypeDescr = (typelib_CompoundTypeDescription*)pTypeDescr;
          pCompTypeDescr; pCompTypeDescr = pCompTypeDescr->pBaseTypeDescription )
    {
        ++nLen;
    }

    // info count accompanied by type info ptrs: type, base type, base base type, ...
    _types = ::rtl_allocateMemory( sizeof(sal_Int32) + (sizeof(ExceptionType *) * nLen) );
    *(sal_Int32 *)_types = nLen;

    ExceptionType ** ppTypes = (ExceptionType **)((sal_Int32 *)_types + 1);

    sal_Int32 nPos = 0;
    for ( pCompTypeDescr = (typelib_CompoundTypeDescription*)pTypeDescr;
          pCompTypeDescr; pCompTypeDescr = pCompTypeDescr->pBaseTypeDescription )
    {
        ppTypes[nPos++] = new ExceptionType( (typelib_TypeDescription *)pCompTypeDescr );
    }
}
//__________________________________________________________________________________________________
RaiseInfo::~RaiseInfo() throw ()
{
    ExceptionType ** ppTypes = (ExceptionType **)((sal_Int32 *)_types + 1);
    for ( sal_Int32 nTypes = *(sal_Int32 *)_types; nTypes--; )
    {
        delete ppTypes[nTypes];
    }
    ::rtl_freeMemory( _types );

    delete _pDtor;
}

//==================================================================================================
class ExceptionInfos
{
    Mutex           _aMutex;
    t_string2PtrMap _allRaiseInfos;

public:
    static void * getRaiseInfo( typelib_TypeDescription * pTypeDescr ) throw ();

    ExceptionInfos() throw ();
    ~ExceptionInfos() throw ();
};
//__________________________________________________________________________________________________
ExceptionInfos::ExceptionInfos() throw ()
{
}
//__________________________________________________________________________________________________
ExceptionInfos::~ExceptionInfos() throw ()
{
#if OSL_DEBUG_LEVEL > 1
    OSL_TRACE( "> freeing exception infos... <" );
#endif

    MutexGuard aGuard( _aMutex );
    for ( t_string2PtrMap::const_iterator iPos( _allRaiseInfos.begin() );
          iPos != _allRaiseInfos.end(); ++iPos )
    {
        delete (RaiseInfo *)iPos->second;
    }
}
//__________________________________________________________________________________________________
void * ExceptionInfos::getRaiseInfo( typelib_TypeDescription * pTypeDescr ) throw ()
{
    static ExceptionInfos * s_pInfos = 0;
    if (! s_pInfos)
    {
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        if (! s_pInfos)
        {
#ifdef LEAK_STATIC_DATA
            s_pInfos = new ExceptionInfos();
#else
            static ExceptionInfos s_allExceptionInfos;
            s_pInfos = &s_allExceptionInfos;
#endif
        }
    }

    OSL_ASSERT( pTypeDescr &&
                (pTypeDescr->eTypeClass == typelib_TypeClass_STRUCT ||
                 pTypeDescr->eTypeClass == typelib_TypeClass_EXCEPTION) );

    void * pRaiseInfo;

    OUString const & rTypeName = *reinterpret_cast< OUString * >( &pTypeDescr->pTypeName );
    MutexGuard aGuard( s_pInfos->_aMutex );
    t_string2PtrMap::const_iterator const iFind(
        s_pInfos->_allRaiseInfos.find( rTypeName ) );
    if (iFind == s_pInfos->_allRaiseInfos.end())
    {
        pRaiseInfo = new RaiseInfo( pTypeDescr );
        // put into map
        pair< t_string2PtrMap::iterator, bool > insertion(
            s_pInfos->_allRaiseInfos.insert( t_string2PtrMap::value_type( rTypeName, pRaiseInfo ) ) );
        OSL_ENSURE( insertion.second, "### raise info insertion failed?!" );
    }
    else
    {
        // reuse existing info
        pRaiseInfo = iFind->second;
    }

    return pRaiseInfo;
}


//##################################################################################################
//#### exported ####################################################################################
//##################################################################################################


//##################################################################################################
type_info * msci_getRTTI( OUString const & rUNOname )
{
    static RTTInfos * s_pRTTIs = 0;
    if (! s_pRTTIs)
    {
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        if (! s_pRTTIs)
        {
#ifdef LEAK_STATIC_DATA
            s_pRTTIs = new RTTInfos();
#else
            static RTTInfos s_aRTTIs;
            s_pRTTIs = &s_aRTTIs;
#endif
        }
    }
    return s_pRTTIs->getRTTI( rUNOname );
}

//##################################################################################################
void msci_raiseException( uno_Any * pUnoExc, uno_Mapping * pUno2Cpp )
{
    // no ctor/dtor in here: this leads to dtors called twice upon RaiseException()!
    // thus this obj file will be compiled without opt, so no inling of
    // ExceptionInfos::getRaiseInfo()

    // construct cpp exception object
    typelib_TypeDescription * pTypeDescr = 0;
    TYPELIB_DANGER_GET( &pTypeDescr, pUnoExc->pType );

    void * pCppExc = alloca( pTypeDescr->nSize );
    ::uno_copyAndConvertData( pCppExc, pUnoExc->pData, pTypeDescr, pUno2Cpp );

    // a must be
    OSL_ENSURE(
        sizeof(sal_Int32) == sizeof(void *),
        "### pointer size differs from sal_Int32!" );
    DWORD arFilterArgs[3];
    arFilterArgs[0] = MSVC_magic_number;
    arFilterArgs[1] = (DWORD)pCppExc;
    arFilterArgs[2] = (DWORD)ExceptionInfos::getRaiseInfo( pTypeDescr );

    // destruct uno exception
    ::uno_any_destruct( pUnoExc, 0 );
    TYPELIB_DANGER_RELEASE( pTypeDescr );

    // last point to release anything not affected by stack unwinding
    RaiseException( MSVC_ExceptionCode, EXCEPTION_NONCONTINUABLE, 3, arFilterArgs );
}

//##############################################################################
int msci_filterCppException(
    EXCEPTION_POINTERS * pPointers, uno_Any * pUnoExc, uno_Mapping * pCpp2Uno )
{
    if (pPointers == 0)
        return EXCEPTION_CONTINUE_SEARCH;
    EXCEPTION_RECORD * pRecord = pPointers->ExceptionRecord;
    // handle only C++ exceptions:
    if (pRecord == 0 || pRecord->ExceptionCode != MSVC_ExceptionCode)
        return EXCEPTION_CONTINUE_SEARCH;

    bool rethrow = __CxxDetectRethrow( &pRecord );
    OSL_ASSERT( pRecord == pPointers->ExceptionRecord );

    if (rethrow && pRecord == pPointers->ExceptionRecord)
    {
        // hack to get msvcrt internal _curexception field:
        pRecord = *reinterpret_cast< EXCEPTION_RECORD ** >(
            reinterpret_cast< char * >( __pxcptinfoptrs() ) +
            // as long as we don't demand msvcr source as build prerequisite
            // (->platform sdk), we have to code those offsets here.
            //
            // crt\src\mtdll.h:
            // offsetof (_tiddata, _curexception) -
            // offsetof (_tiddata, _tpxcptinfoptrs):
            0x28 // msvcr80.dll (and later?)
            );
    }
    // rethrow: handle only C++ exceptions:
    if (pRecord == 0 || pRecord->ExceptionCode != MSVC_ExceptionCode)
        return EXCEPTION_CONTINUE_SEARCH;

    if (pRecord->NumberParameters == 3 &&
//          pRecord->ExceptionInformation[ 0 ] == MSVC_magic_number &&
        pRecord->ExceptionInformation[ 1 ] != 0 &&
        pRecord->ExceptionInformation[ 2 ] != 0)
    {
        void * types = reinterpret_cast< RaiseInfo * >(
            pRecord->ExceptionInformation[ 2 ] )->_types;
        if (types != 0 && *reinterpret_cast< DWORD * >( types ) > 0) // count
        {
            ExceptionType * pType = *reinterpret_cast< ExceptionType ** >(
                reinterpret_cast< DWORD * >( types ) + 1 );
            if (pType != 0 && pType->_pTypeInfo != 0)
            {
                OUString aRTTIname(
                    OStringToOUString(
                        reinterpret_cast< __type_info * >(
                            pType->_pTypeInfo )->_m_d_name,
                        RTL_TEXTENCODING_ASCII_US ) );
                OUString aUNOname( toUNOname( aRTTIname ) );

                typelib_TypeDescription * pExcTypeDescr = 0;
                typelib_typedescription_getByName(
                    &pExcTypeDescr, aUNOname.pData );
                if (pExcTypeDescr == 0)
                {
                    OUStringBuffer buf;
                    buf.appendAscii(
                        RTL_CONSTASCII_STRINGPARAM(
                            "[msci_uno bridge error] UNO type of "
                            "C++ exception unknown: \"") );
                    buf.append( aUNOname );
                    buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(
                                         "\", RTTI-name=\"") );
                    buf.append( aRTTIname );
                    buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("\"!") );
                    RuntimeException exc(
                        buf.makeStringAndClear(), Reference< XInterface >() );
                    uno_type_any_constructAndConvert(
                        pUnoExc, &exc,
                        ::getCppuType( &exc ).getTypeLibType(), pCpp2Uno );
                    // msvcr80.dll cleans up, different from former msvcrs
                    // if (! rethrow):
                    // though this unknown exception leaks now, no user-defined
                    // exception is ever thrown thru the binary C-UNO dispatcher
                    // call stack.
                }
                else
                {
                    // construct uno exception any
                    uno_any_constructAndConvert(
                        pUnoExc, (void *) pRecord->ExceptionInformation[1],
                        pExcTypeDescr, pCpp2Uno );
                    typelib_typedescription_release( pExcTypeDescr );
                }

                return EXCEPTION_EXECUTE_HANDLER;
            }
        }
    }
    // though this unknown exception leaks now, no user-defined exception
    // is ever thrown thru the binary C-UNO dispatcher call stack.
    RuntimeException exc(
        OUString( RTL_CONSTASCII_USTRINGPARAM(
                      "[msci_uno bridge error] unexpected "
                      "C++ exception occurred!") ),
        Reference< XInterface >() );
    uno_type_any_constructAndConvert(
        pUnoExc, &exc, ::getCppuType( &exc ).getTypeLibType(), pCpp2Uno );
    return EXCEPTION_EXECUTE_HANDLER;
}

}

#pragma pack(pop)

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
