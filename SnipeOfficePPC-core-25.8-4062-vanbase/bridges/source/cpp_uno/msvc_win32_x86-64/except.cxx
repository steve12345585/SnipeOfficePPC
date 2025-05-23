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

// Interesting info can be found in:

// MSDN, obviously

// http://www.osronline.com/article.cfm?article=469

// ONTL, "Open NT Native Template Library", a C++ STL-like library
// that can be used even when writing Windows drivers. This is some
// quite badass code. The author has done impressive heavy spelunking
// of MSVCR structures. http://code.google.com/p/ontl/

// Geoff Chappell's pages:
// http://www.geoffchappell.com/studies/msvc/language/index.htm

// The below is from ONTL's ntl/nt/exception.hxx, cleaned up to keep just the _M_X64 parts:

#if 0

/* This information until the corresponding '#endif // 0' is covered
 * by ONTL's license, which is said to be the "zlib/libgng license"
 * below, which as far as I see is permissive enough to allow this
 * information to be included here in this source file. Note that no
 * actual code from ONTL below gets compiled into the object file.
 */

/*
 * Copyright (c) 2011 <copyright holders> (The ONTL main
 * developer(s) don't tell their real name(s) on the ONTL site.)
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 *
 */

typedef uint32_t rva_t;

///\note the calling convention should not matter since this has no arguments
typedef void generic_function_t();

struct ptrtomember // _PMD
{
  typedef __w64 int32_t  mdiff_t;
  mdiff_t member_offset;
  mdiff_t vbtable_offset; // -1 if not a virtual base
  mdiff_t vdisp_offset;   // offset to the displacement value inside the vbtable

  template<typename T>
  T * operator()(T * const thisptr) const
  {
    uintptr_t tp = reinterpret_cast<uintptr_t>(thisptr);
    uintptr_t ptr = tp + member_offset;
    if ( vbtable_offset != -1 ) // !(vbtable_offset < 0)
    {
      ptr += *reinterpret_cast<mdiff_t*>( static_cast<intptr_t>(vdisp_offset + *reinterpret_cast<mdiff_t*>(tp + vbtable_offset)) )
        + vbtable_offset;
    }
    return reinterpret_cast<T*>(ptr);
  }
};

struct eobject
{
  typedef void (* dtor_ptr )(eobject*);
  typedef void (* ctor_ptr )(eobject*, eobject*);
  typedef void (* ctor_ptr2)(eobject*, eobject*, int);
};

struct catchabletype
{
  /** is simple type */
  uint32_t    memmoveable : 1;
  /** catchable as reference only */
  uint32_t    refonly     : 1;
  /** class with virtual base */
  uint32_t    hasvirtbase : 1;
  /** offset to the type descriptor */
  rva_t       typeinfo;

  /** catch type instance location within a thrown object  */
  ptrtomember thiscast;
  /** size of the simple type or offset into buffer of \c this pointer for catch object */
  uint32_t    object_size;

  union
  {
    rva_t               copyctor;
    rva_t               copyctor2;
  };
};

#pragma pack(push, 4)
struct catchabletypearray
{
  uint32_t        size;
  rva_t           type[1];
};
#pragma pack(pop)

#pragma pack(push, 4)
struct throwinfo
{
  typedef exception_disposition __cdecl forwardcompathandler_t(...);

  /* 0x00 */  uint32_t  econst    : 1;
  /* 0x00 */  uint32_t  evolatile : 1;
  /* 0x00 */  uint32_t            : 1;
  /* 0x00 */  uint32_t  e8        : 1;
  /* 0x04 */  rva_t     exception_dtor;
  /* 0x08 */  rva_t     forwardcompathandler;
  /* 0x0C */  rva_t     catchabletypearray; ///< types able to catch the exception.
};
#pragma pack(pop)

/// This type represents the catch clause
struct ehandler
{
  //  union { uint32_t  adjectives; void * ptr; };
  uint32_t isconst      : 1;
  uint32_t isvolatile   : 1;
  uint32_t isunaligned  : 1;// guess it is not used on x86
  uint32_t isreference  : 1;

  uint32_t              :27;
  uint32_t ishz         : 1;

  /** offset to the type descriptor of this catch object */
  /*0x04*/ rva_t        typeinfo;         // dispType
  /*0x08*/ int          eobject_bpoffset; // dispCatchObj
  /** offset to the catch clause funclet */
  /*0x0C*/ rva_t        handler;          // dispOfHandler
  /*0x10*/ uint32_t     frame;            // dispFrame
}

// ___BuildCatchObject
/// 15.3/16 When the exception-declaration specifies a class type, a copy
///         constructor is used to initialize either the object declared
///         in the exception-declaration or,
///         if the exception-declaration does not specify a name,
///         a temporary object of that type.
///\note    This is the question may we optimize out the last case.
///\warning If the copy constructor throws an exception, std::unexpected would be called
void
  constructcatchobject(
  cxxregistration *             cxxreg,
  const ehandler *        const catchblock,
  catchabletype *         const convertable,
  const dispatcher_context* const dispatch
  )
  const
{
  _EH_TRACE_ENTER();
  // build helper
  __try {
    struct typeinfo_t { void* vtbl; void* spare; char name[1]; };
    enum catchable_info { cidefault, cicomplex, civirtual } cinfo = cidefault;

    const typeinfo_t* ti = catchblock->typeinfo ? dispatch->va<typeinfo_t*>(catchblock->typeinfo) : NULL;
    if(ti && *ti->name && (catchblock->eobject_bpoffset || catchblock->ishz)){
      eobject** objplace = catchblock->ishz
        ? reinterpret_cast<eobject**>(cxxreg)
        : reinterpret_cast<eobject**>(catchblock->eobject_bpoffset + cxxreg->fp.FramePointers);
      if(catchblock->isreference){
        // just ref/pointer
        *objplace = adjust_pointer(get_object(), convertable);
      }else if(convertable->memmoveable){
        // POD
        std::memcpy(objplace, get_object(), convertable->object_size);
        if(convertable->object_size == sizeof(void*) && *objplace)
          *objplace = adjust_pointer((void*)*objplace, convertable);
      }else{
        // if copy ctor exists, call it; binary copy otherwise
        if(convertable->copyctor){
          cinfo = convertable->hasvirtbase ? civirtual : cicomplex;
        }else{
          std::memcpy(objplace, (const void*)adjust_pointer(get_object(), convertable), convertable->object_size);
        }
      }
    }
    // end of build helper
    if(cinfo != cidefault){
      eobject* objthis = catchblock->ishz
        ? reinterpret_cast<eobject*>(cxxreg)
        : reinterpret_cast<eobject*>(catchblock->eobject_bpoffset + cxxreg->fp.FramePointers);
      void* copyctor = thrown_va(convertable->copyctor);
      eobject* copyarg = adjust_pointer(get_object(), convertable);
      if(cinfo == cicomplex)
        (eobject::ctor_ptr (copyctor))(objthis, copyarg);
      else
        (eobject::ctor_ptr2(copyctor))(objthis, copyarg, 1);
    }
  }
  __except(cxxregistration::unwindfilter(static_cast<nt::ntstatus>(_exception_code())))
  {
    nt::exception::inconsistency();
  }
  _EH_TRACE_LEAVE();
}

#endif // 0


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

#include "mscx.hxx"

#pragma pack(push, 8)

using namespace ::com::sun::star::uno;
using namespace ::std;
using namespace ::osl;
using namespace ::rtl;

namespace CPPU_CURRENT_NAMESPACE
{

static inline OUString toUNOname(
    OUString const & rRTTIname )
    throw ()
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

static inline OUString toRTTIname(
    OUString const & rUNOname )
    throw ()
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

//RTTI simulation

typedef boost::unordered_map< OUString, void *, OUStringHash, equal_to< OUString > > t_string2PtrMap;

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

class __type_info
{
    friend type_info * RTTInfos::getRTTI( OUString const & ) throw ();
    friend int mscx_filterCppException(
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

__type_info::~__type_info() throw ()
{
}

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

RTTInfos::RTTInfos() throw ()
{
}

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

void * __cdecl copyConstruct(
    void * pExcThis,
    void * pSource,
    typelib_TypeDescription * pTD ) throw ()
{
    ::uno_copyData( pExcThis, pSource, pTD, cpp_acquire );
    return pExcThis;
}

void * __cdecl destruct(
    void * pExcThis,
    typelib_TypeDescription * pTD ) throw ()
{
    ::uno_destructData( pExcThis, pTD, cpp_release );
    return pExcThis;
}

const int codeSnippetSize = 40;

void GenerateConstructorTrampoline(
    unsigned char * code,
    typelib_TypeDescription * pTD ) throw ()
{
    unsigned char *p = code;

    // mov r8, pTD
    *p++ = 0x49; *p++ = 0xB8;
    *((void **)p) = pTD; p += 8;

    // mov r11, copyConstruct
    *p++ = 0x49; *p++ = 0xBB;
    *((void **)p) = &copyConstruct; p += 8;

    // jmp r11
    *p++ = 0x41; *p++ = 0xFF; *p++ = 0xE3;

    OSL_ASSERT( p < code + codeSnippetSize );
}

void GenerateDestructorTrampoline(
    unsigned char * code,
    typelib_TypeDescription * pTD ) throw ()
{
    unsigned char *p = code;

    // mov rdx, pTD
    *p++ = 0x48; *p++ = 0xBA;
    *((void **)p) = pTD; p += 8;

    // mov r11, destruct
    *p++ = 0x49; *p++ = 0xBB;
    *((void **)p) = &destruct; p += 8;

    // jmp r11
    *p++ = 0x41; *p++ = 0xFF; *p++ = 0xE3;

    OSL_ASSERT( p < code + codeSnippetSize );
}

// This looks like it is the struct catchabletype above

struct ExceptionType
{
    sal_Int32   _n0;            // flags
    sal_uInt32  _pTypeInfo;     // typeinfo
    sal_Int32   _n1, _n2, _n3;  // thiscast
    sal_Int32   _n4;            // object_size
    sal_uInt32  _pCopyCtor;     // copyctor

    inline ExceptionType(
        sal_uChar * pCode,
        sal_uInt64 pCodeBase,
        typelib_TypeDescription * pTD ) throw ()
        : _n0( 0 )
        , _n1( 0 )
        , _n2( -1 )
        , _n3( 0 )
        , _n4( pTD->nSize )
        {
            // As _n0 is always initialized to zero, that means the
            // hasvirtbase flag (see the ONTL catchabletype struct) is
            // off, and thus the copyctor is of the ctor_ptr kind.
            _pTypeInfo = (sal_uInt32) ((sal_uInt64) mscx_getRTTI( pTD->pTypeName ) - pCodeBase);
            GenerateConstructorTrampoline( pCode, pTD );
            _pCopyCtor = (sal_uInt32) ((sal_uInt64) pCode - pCodeBase);
        }
    inline ~ExceptionType() throw ()
        {
        }
};

struct RaiseInfo;

class ExceptionInfos
{
    Mutex           _aMutex;
    t_string2PtrMap _allRaiseInfos;

public:
    static RaiseInfo * getRaiseInfo( typelib_TypeDescription * pTD ) throw ();

    static DWORD allocationGranularity;

    ExceptionInfos() throw ();
    ~ExceptionInfos() throw ();
};

DWORD ExceptionInfos::allocationGranularity = 0;

// This corresponds to the struct throwinfo described above.

struct RaiseInfo
{
    sal_Int32           _n0;
    sal_uInt32          _pDtor;
    sal_Int32           _n2;
    sal_uInt32          _types;

    // Additional fields
    typelib_TypeDescription * _pTD;
    sal_uChar *        _code;
    sal_uInt64         _codeBase;

    RaiseInfo( typelib_TypeDescription * pTD ) throw ();

    ~RaiseInfo() throw ();
};

RaiseInfo::RaiseInfo( typelib_TypeDescription * pTD )throw ()
    : _n0( 0 )
    , _n2( 0 )
    , _pTD( pTD )
{
    typelib_CompoundTypeDescription * pCompTD;

    // Count how many trampolines we need
    int codeSize = codeSnippetSize;

    // Info count
    int nLen = 0;
    for ( pCompTD = (typelib_CompoundTypeDescription*)pTD;
          pCompTD; pCompTD = pCompTD->pBaseTypeDescription )
    {
        ++nLen;
        codeSize += codeSnippetSize;
    }

    sal_uChar * pCode = _code = (sal_uChar *)::rtl_allocateMemory( codeSize );

    _codeBase = (sal_uInt64)pCode & ~(ExceptionInfos::allocationGranularity-1);

    DWORD old_protect;
#if OSL_DEBUG_LEVEL > 0
    BOOL success =
#endif
        VirtualProtect( pCode, codeSize, PAGE_EXECUTE_READWRITE, &old_protect );
        OSL_ENSURE( success, "VirtualProtect() failed!" );

    ::typelib_typedescription_acquire( pTD );

    GenerateDestructorTrampoline( pCode, pTD );
    _pDtor = (sal_Int32)((sal_uInt64)pCode - _codeBase);
    pCode += codeSnippetSize;

    // Info count accompanied by type info ptrs: type, base type, base base type, ...
    _types = (sal_Int32)((sal_uInt64)::rtl_allocateMemory( 4 + 4* nLen) - _codeBase);
    *(sal_Int32 *)_types = nLen;

    ExceptionType ** ppTypes = (ExceptionType **)((sal_Int32 *)_types + 1);

    int nPos = 0;
    for ( pCompTD = (typelib_CompoundTypeDescription*)pTD;
          pCompTD; pCompTD = pCompTD->pBaseTypeDescription )
    {
        ppTypes[nPos++] =
            new ExceptionType( pCode, _codeBase,
                               (typelib_TypeDescription *)pCompTD );
        pCode += codeSnippetSize;
    }
}

RaiseInfo::~RaiseInfo() throw ()
{
    sal_uInt32 * pTypes =
        (sal_uInt32 *)(_codeBase + _types) + 1;

    for ( int nTypes = *(sal_uInt32 *)(_codeBase + _types); nTypes--; )
    {
        delete (ExceptionType *) (_codeBase + pTypes[nTypes]);
    }
    ::rtl_freeMemory( (void*)(_codeBase +_types) );
    ::rtl_freeMemory( _code );

    ::typelib_typedescription_release( _pTD );
}

ExceptionInfos::ExceptionInfos() throw ()
{
}

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

RaiseInfo * ExceptionInfos::getRaiseInfo( typelib_TypeDescription * pTD ) throw ()
{
    static ExceptionInfos * s_pInfos = 0;
    if (! s_pInfos)
    {
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        if (! s_pInfos)
        {
            SYSTEM_INFO systemInfo;
            GetSystemInfo( &systemInfo );
            allocationGranularity = systemInfo.dwAllocationGranularity;

#ifdef LEAK_STATIC_DATA
            s_pInfos = new ExceptionInfos();
#else
            static ExceptionInfos s_allExceptionInfos;
            s_pInfos = &s_allExceptionInfos;
#endif
        }
    }

    OSL_ASSERT( pTD &&
                (pTD->eTypeClass == typelib_TypeClass_STRUCT ||
                 pTD->eTypeClass == typelib_TypeClass_EXCEPTION) );

    RaiseInfo * pRaiseInfo;

    OUString const & rTypeName = *reinterpret_cast< OUString * >( &pTD->pTypeName );
    MutexGuard aGuard( s_pInfos->_aMutex );
    t_string2PtrMap::const_iterator const iFind(
        s_pInfos->_allRaiseInfos.find( rTypeName ) );
    if (iFind == s_pInfos->_allRaiseInfos.end())
    {
        pRaiseInfo = new RaiseInfo( pTD );

        // Put into map
        pair< t_string2PtrMap::iterator, bool > insertion(
            s_pInfos->_allRaiseInfos.insert( t_string2PtrMap::value_type( rTypeName, (void *)pRaiseInfo ) ) );
        OSL_ENSURE( insertion.second, "### raise info insertion failed?!" );
    }
    else
    {
        // Reuse existing info
        pRaiseInfo = (RaiseInfo *)iFind->second;
    }

    return pRaiseInfo;
}

type_info * mscx_getRTTI(
    OUString const & rUNOname )
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

void mscx_raiseException(
    uno_Any * pUnoExc,
    uno_Mapping * pUno2Cpp )
{
    // no ctor/dtor in here: this leads to dtors called twice upon RaiseException()!
    // thus this obj file will be compiled without opt, so no inlining of
    // ExceptionInfos::getRaiseInfo()

    // construct cpp exception object
    typelib_TypeDescription * pTD = NULL;
    TYPELIB_DANGER_GET( &pTD, pUnoExc->pType );

    void * pCppExc = alloca( pTD->nSize );
    ::uno_copyAndConvertData( pCppExc, pUnoExc->pData, pTD, pUno2Cpp );

    ULONG_PTR arFilterArgs[4];
    arFilterArgs[0] = MSVC_magic_number;
    arFilterArgs[1] = (ULONG_PTR)pCppExc;
    arFilterArgs[2] = (ULONG_PTR)ExceptionInfos::getRaiseInfo( pTD );
    arFilterArgs[3] = ((RaiseInfo *)arFilterArgs[2])->_codeBase;

    // Destruct uno exception
    ::uno_any_destruct( pUnoExc, 0 );
    TYPELIB_DANGER_RELEASE( pTD );

    // last point to release anything not affected by stack unwinding
    RaiseException( MSVC_ExceptionCode, EXCEPTION_NONCONTINUABLE, 3, arFilterArgs );
}

int mscx_filterCppException(
    EXCEPTION_POINTERS * pPointers,
    uno_Any * pUnoExc,
    uno_Mapping * pCpp2Uno )
{
    if (pPointers == 0)
        return EXCEPTION_CONTINUE_SEARCH;

    EXCEPTION_RECORD * pRecord = pPointers->ExceptionRecord;

    // Handle only C++ exceptions:
    if (pRecord == 0 || pRecord->ExceptionCode != MSVC_ExceptionCode)
        return EXCEPTION_CONTINUE_SEARCH;

    bool rethrow = __CxxDetectRethrow( &pRecord );
    OSL_ASSERT( pRecord == pPointers->ExceptionRecord );

    if (rethrow && pRecord == pPointers->ExceptionRecord)
    {
        // Hack to get msvcrt internal _curexception field:
        pRecord = *reinterpret_cast< EXCEPTION_RECORD ** >(
            reinterpret_cast< char * >( __pxcptinfoptrs() ) +
            // As long as we don't demand MSVCR source as build prerequisite,
            // we have to code those offsets here.
            //
            // MSVS9/crt/src/mtdll.h:
            // offsetof (_tiddata, _curexception) -
            // offsetof (_tiddata, _tpxcptinfoptrs):
#if _MSC_VER < 1600
            0x48 // msvcr90.dll
#else
            error, please find value for this compiler version
#endif
            );
    }

    // Rethrow: handle only C++ exceptions:
    if (pRecord == 0 || pRecord->ExceptionCode != MSVC_ExceptionCode)
        return EXCEPTION_CONTINUE_SEARCH;

    if (pRecord->NumberParameters == 4 &&
        pRecord->ExceptionInformation[0] == MSVC_magic_number &&
        pRecord->ExceptionInformation[1] != 0 &&
        pRecord->ExceptionInformation[2] != 0 &&
        pRecord->ExceptionInformation[3] != 0)
    {
        // ExceptionInformation[1] is the address of the thrown object
        // (or the address of a pointer to it, in most cases when it
        // is a C++ class, obviously).

        // [2] is the throwinfo pointer

        // [3] is the image base address which is added the 32-bit
        // rva_t fields in throwinfo to get actual 64-bit addresses

        void * types =
            (void *) (pRecord->ExceptionInformation[3] +
                      ((RaiseInfo *)pRecord->ExceptionInformation[2])->_types);

        if (types != 0 && *(DWORD *)types > 0)
        {
            DWORD pType = *((DWORD *)types + 1);
            if (pType != 0 &&
                ((ExceptionType *)(pRecord->ExceptionInformation[3]+pType))->_pTypeInfo != 0)
            {
                OUString aRTTIname(
                    OStringToOUString(
                        reinterpret_cast< __type_info * >(
                            ((ExceptionType *)(pRecord->ExceptionInformation[3]+pType))->_pTypeInfo )->_m_d_name,
                        RTL_TEXTENCODING_ASCII_US ) );
                OUString aUNOname( toUNOname( aRTTIname ) );

                typelib_TypeDescription * pExcTD = 0;
                typelib_typedescription_getByName(
                    &pExcTD, aUNOname.pData );
                if (pExcTD == NULL)
                {
                    OUStringBuffer buf;
                    buf.appendAscii(
                        RTL_CONSTASCII_STRINGPARAM(
                            "[mscx_uno bridge error] UNO type of "
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
                }
                else
                {
                    // construct uno exception any
                    uno_any_constructAndConvert(
                        pUnoExc, (void *) pRecord->ExceptionInformation[1],
                        pExcTD, pCpp2Uno );
                    typelib_typedescription_release( pExcTD );
                }

                return EXCEPTION_EXECUTE_HANDLER;
            }
        }
    }
    // though this unknown exception leaks now, no user-defined exception
    // is ever thrown thru the binary C-UNO dispatcher call stack.
    RuntimeException exc(
        OUString( RTL_CONSTASCII_USTRINGPARAM(
                      "[mscx_uno bridge error] unexpected "
                      "C++ exception occurred!") ),
        Reference< XInterface >() );
    uno_type_any_constructAndConvert(
        pUnoExc, &exc, ::getCppuType( &exc ).getTypeLibType(), pCpp2Uno );
    return EXCEPTION_EXECUTE_HANDLER;
}

}

#pragma pack(pop)

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
