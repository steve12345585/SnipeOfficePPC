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

#include "sal/config.h"

#if defined(WNT)
#include <windows.h>
#undef GetObject
#endif

#include <algorithm>
#include <cstddef>
#include <list>
#include <map>
#include <vector>

#include "basic/sbx.hxx"
#include "basic/sbxvar.hxx"
#include "osl/thread.h"
#include "rtl/ref.hxx"
#include "rtl/string.hxx"
#include "rtl/ustring.hxx"
#include "salhelper/simplereferenceobject.hxx"

#undef max

#include "dllmgr.hxx"

/* Open issues:

   Missing support for functions returning structs (see TODO in call()).

   Missing support for additional data types (64 bit integers, Any, ...; would
   trigger OSL_ASSERT(false) in various switches).

   It is assumed that the variables passed into SbiDllMgr::Call to represent
   the arguments and return value have types that exactly match the Declare
   statement; it would be better if this code had access to the function
   signature from the Declare statement, so that it could convert the passed
   variables accordingly.
*/

namespace {

char * address(std::vector< char > & blob) {
    return blob.empty() ? 0 : &blob[0];
}

SbError convert(rtl::OUString const & source, rtl::OString * target) {
    return
        source.convertToString(
            target, osl_getThreadTextEncoding(),
            (RTL_UNICODETOTEXT_FLAGS_UNDEFINED_ERROR |
             RTL_UNICODETOTEXT_FLAGS_INVALID_ERROR))
        ? ERRCODE_NONE : ERRCODE_BASIC_BAD_ARGUMENT;
        //TODO: more specific errcode?
}

SbError convert(char const * source, sal_Int32 length, rtl::OUString * target) {
    return
        rtl_convertStringToUString(
            &target->pData, source, length, osl_getThreadTextEncoding(),
            (RTL_TEXTTOUNICODE_FLAGS_UNDEFINED_ERROR |
             RTL_TEXTTOUNICODE_FLAGS_MBUNDEFINED_ERROR |
             RTL_TEXTTOUNICODE_FLAGS_INVALID_ERROR))
        ? ERRCODE_NONE : ERRCODE_BASIC_BAD_ARGUMENT;
        //TODO: more specific errcode?
}

struct UnmarshalData {
    UnmarshalData(SbxVariable * theVariable, void * theBuffer):
        variable(theVariable), buffer(theBuffer) {}

    SbxVariable * variable;
    void * buffer;
};

struct StringData: public UnmarshalData {
    StringData(SbxVariable * theVariable, void * theBuffer, bool theSpecial):
        UnmarshalData(theVariable, theBuffer), special(theSpecial) {}

    bool special;
};

class MarshalData: private boost::noncopyable {
public:
    std::vector< char > * newBlob() {
        blobs_.push_front(std::vector< char >());
        return &blobs_.front();
    }

    std::vector< UnmarshalData > unmarshal;

    std::vector< StringData > unmarshalStrings;

private:
    std::list< std::vector< char > > blobs_;
};

std::size_t align(std::size_t address, std::size_t alignment) {
    // alignment = 2^k for some k >= 0
    return (address + (alignment - 1)) & ~(alignment - 1);
}

char * align(
    std::vector< char > & blob, std::size_t alignment, std::size_t offset,
    std::size_t add)
{
    std::vector< char >::size_type n = blob.size();
    n = align(n - offset, alignment) + offset; //TODO: overflow in align()
    blob.resize(n + add); //TODO: overflow
    return address(blob) + n;
}

template< typename T > void add(
    std::vector< char > & blob, T const & data, std::size_t alignment,
    std::size_t offset)
{
    *reinterpret_cast< T * >(align(blob, alignment, offset, sizeof (T))) = data;
}

std::size_t alignment(SbxVariable * variable) {
    OSL_ASSERT(variable != 0);
    if ((variable->GetType() & SbxARRAY) == 0) {
        switch (variable->GetType()) {
        case SbxINTEGER:
            return 2;
        case SbxLONG:
        case SbxSINGLE:
        case SbxSTRING:
            return 4;
        case SbxDOUBLE:
            return 8;
        case SbxOBJECT:
            {
                std::size_t n = 1;
                SbxArray * props = PTR_CAST(SbxObject, variable->GetObject())->
                    GetProperties();
                for (sal_uInt16 i = 0; i < props->Count(); ++i) {
                    n = std::max(n, alignment(props->Get(i)));
                }
                return n;
            }
        case SbxBOOL:
        case SbxBYTE:
            return 1;
        default:
            OSL_ASSERT(false);
            return 1;
        }
    } else {
        SbxDimArray * arr = PTR_CAST(SbxDimArray, variable->GetObject());
        int dims = arr->GetDims();
        std::vector< sal_Int32 > low(dims);
        for (int i = 0; i < dims; ++i) {
            sal_Int32 up;
            arr->GetDim32(i + 1, low[i], up);
        }
        return alignment(arr->Get32(&low[0]));
    }
}

SbError marshal(
    bool outer, SbxVariable * variable, bool special,
    std::vector< char > & blob, std::size_t offset, MarshalData & data);

SbError marshalString(
    SbxVariable * variable, bool special, MarshalData & data, void ** buffer)
{
    OSL_ASSERT(variable != 0 && buffer != 0);
    rtl::OString str;
    SbError e = convert(variable->GetOUString(), &str);
    if (e != ERRCODE_NONE) {
        return e;
    }
    std::vector< char > * blob = data.newBlob();
    blob->insert(blob->begin(), str.getStr(), str.getStr() + str.getLength() + 1);
    *buffer = address(*blob);
    data.unmarshalStrings.push_back(StringData(variable, *buffer, special));
    return ERRCODE_NONE;
}

SbError marshalStruct(
    SbxVariable * variable, std::vector< char > & blob, std::size_t offset,
    MarshalData & data)
{
    OSL_ASSERT(variable != 0);
    SbxArray * props = PTR_CAST(SbxObject, variable->GetObject())->
        GetProperties();
    for (sal_uInt16 i = 0; i < props->Count(); ++i) {
        SbError e = marshal(false, props->Get(i), false, blob, offset, data);
        if (e != ERRCODE_NONE) {
            return e;
        }
    }
    return ERRCODE_NONE;
}

SbError marshalArray(
    SbxVariable * variable, std::vector< char > & blob, std::size_t offset,
    MarshalData & data)
{
    OSL_ASSERT(variable != 0);
    SbxDimArray * arr = PTR_CAST(SbxDimArray, variable->GetObject());
    int dims = arr->GetDims();
    std::vector< sal_Int32 > low(dims);
    std::vector< sal_Int32 > up(dims);
    for (int i = 0; i < dims; ++i) {
        arr->GetDim32(i + 1, low[i], up[i]);
    }
    for (std::vector< sal_Int32 > idx = low;;) {
        SbError e = marshal(
            false, arr->Get32(&idx[0]), false, blob, offset, data);
        if (e != ERRCODE_NONE) {
            return e;
        }
        int i = dims - 1;
        while (idx[i] == up[i]) {
            idx[i] = low[i];
            if (i == 0) {
                return ERRCODE_NONE;
            }
            --i;
        }
        ++idx[i];
    }
}

// 8-aligned structs are only 4-aligned on stack, so alignment of members in
// such structs must take that into account via "offset"
SbError marshal(
    bool outer, SbxVariable * variable, bool special,
    std::vector< char > & blob, std::size_t offset, MarshalData & data)
{
    OSL_ASSERT(variable != 0);
    if ((variable->GetFlags() & SBX_REFERENCE) == 0) {
        if ((variable->GetType() & SbxARRAY) == 0) {
            switch (variable->GetType()) {
            case SbxINTEGER:
                add(blob, variable->GetInteger(), outer ? 8 : 2, offset);
                break;
            case SbxLONG:
                add(blob, variable->GetLong(), outer ? 8 : 4, offset);
                break;
            case SbxSINGLE:
                add(blob, variable->GetSingle(), outer ? 8 : 4, offset);
                break;
            case SbxDOUBLE:
                add(blob, variable->GetDouble(), 8, offset);
                break;
            case SbxSTRING:
                {
                    void * p;
                    SbError e = marshalString(variable, special, data, &p);
                    if (e != ERRCODE_NONE) {
                        return e;
                    }
                    add(blob, p, 8, offset);
                    break;
                }
            case SbxOBJECT:
                {
                    align(blob, outer ? 8 : alignment(variable), offset, 0);
                    SbError e = marshalStruct(variable, blob, offset, data);
                    if (e != ERRCODE_NONE) {
                        return e;
                    }
                    break;
                }
            case SbxBOOL:
                add(blob, variable->GetBool(), outer ? 8 : 1, offset);
                break;
            case SbxBYTE:
                add(blob, variable->GetByte(), outer ? 8 : 1, offset);
                break;
            default:
                OSL_ASSERT(false);
                break;
            }
        } else {
            SbError e = marshalArray(variable, blob, offset, data);
            if (e != ERRCODE_NONE) {
                return e;
            }
        }
    } else {
        if ((variable->GetType() & SbxARRAY) == 0) {
            switch (variable->GetType()) {
            case SbxINTEGER:
            case SbxLONG:
            case SbxSINGLE:
            case SbxDOUBLE:
            case SbxBOOL:
            case SbxBYTE:
                add(blob, variable->data(), 8, offset);
                break;
            case SbxSTRING:
                {
                    std::vector< char > * blob2 = data.newBlob();
                    void * p;
                    SbError e = marshalString(variable, special, data, &p);
                    if (e != ERRCODE_NONE) {
                        return e;
                    }
                    add(*blob2, p, 8, 0);
                    add(blob, address(*blob2), 8, offset);
                    break;
                }
            case SbxOBJECT:
                {
                    std::vector< char > * blob2 = data.newBlob();
                    SbError e = marshalStruct(variable, *blob2, 0, data);
                    if (e != ERRCODE_NONE) {
                        return e;
                    }
                    void * p = address(*blob2);
                    if (outer) {
                        data.unmarshal.push_back(UnmarshalData(variable, p));
                    }
                    add(blob, p, 8, offset);
                    break;
                }
            default:
                OSL_ASSERT(false);
                break;
            }
        } else {
            std::vector< char > * blob2 = data.newBlob();
            SbError e = marshalArray(variable, *blob2, 0, data);
            if (e != ERRCODE_NONE) {
                return e;
            }
            void * p = address(*blob2);
            if (outer) {
                data.unmarshal.push_back(UnmarshalData(variable, p));
            }
            add(blob, p, 8, offset);
        }
    }
    return ERRCODE_NONE;
}

template< typename T > T read(void const ** pointer) {
    T const * p = static_cast< T const * >(*pointer);
    *pointer = static_cast< void const * >(p + 1);
    return *p;
}

void const * unmarshal(SbxVariable * variable, void const * data) {
    OSL_ASSERT(variable != 0);
    if ((variable->GetType() & SbxARRAY) == 0) {
        switch (variable->GetType()) {
        case SbxINTEGER:
            variable->PutInteger(read< sal_Int16 >(&data));
            break;
        case SbxLONG:
            variable->PutLong(read< sal_Int32 >(&data));
            break;
        case SbxSINGLE:
            variable->PutSingle(read< float >(&data));
            break;
        case SbxDOUBLE:
            variable->PutDouble(read< double >(&data));
            break;
        case SbxSTRING:
            read< char * >(&data); // handled by unmarshalString
            break;
        case SbxOBJECT:
            {
                data = reinterpret_cast< void const * >(
                    align(
                        reinterpret_cast< sal_uIntPtr >(data),
                        alignment(variable)));
                SbxArray * props = PTR_CAST(SbxObject, variable->GetObject())->
                    GetProperties();
                for (sal_uInt16 i = 0; i < props->Count(); ++i) {
                    data = unmarshal(props->Get(i), data);
                }
                break;
            }
        case SbxBOOL:
            variable->PutBool(read< sal_Bool >(&data));
            break;
        case SbxBYTE:
            variable->PutByte(read< sal_uInt8 >(&data));
            break;
        default:
            OSL_ASSERT(false);
            break;
        }
    } else {
        SbxDimArray * arr = PTR_CAST(SbxDimArray, variable->GetObject());
        int dims = arr->GetDims();
        std::vector< sal_Int32 > low(dims);
        std::vector< sal_Int32 > up(dims);
        for (int i = 0; i < dims; ++i) {
            arr->GetDim32(i + 1, low[i], up[i]);
        }
        for (std::vector< sal_Int32 > idx = low;;) {
            data = unmarshal(arr->Get32(&idx[0]), data);
            int i = dims - 1;
            while (idx[i] == up[i]) {
                idx[i] = low[i];
                if (i == 0) {
                    goto done;
                }
                --i;
            }
            ++idx[i];
        }
    done:;
    }
    return data;
}

SbError unmarshalString(StringData const & data, SbxVariable & result) {
    rtl::OUString str;
    if (data.buffer != 0) {
        char const * p = static_cast< char const * >(data.buffer);
        sal_Int32 len;
        if (data.special) {
            len = static_cast< sal_Int32 >(result.GetULong());
            if (len < 0) { // i.e., DWORD result >= 2^31
                return ERRCODE_BASIC_BAD_ARGUMENT;
                    //TODO: more specific errcode?
            }
        } else {
            len = rtl_str_getLength(p);
        }
        SbError e = convert(p, len, &str);
        if (e != ERRCODE_NONE) {
            return e;
        }
    }
    data.variable->PutString(str);
    return ERRCODE_NONE;
}

struct ProcData {
    rtl::OString name;
    FARPROC proc;
};

SbError call(
    rtl::OUString const & dll, ProcData const & proc, SbxArray * arguments,
    SbxVariable & result)
{
    if (arguments->Count() > 20)
        return ERRCODE_BASIC_NOT_IMPLEMENTED;

    std::vector< char > stack;
    MarshalData data;

    // For DWORD GetLogicalDriveStringsA(DWORD nBufferLength, LPSTR lpBuffer)
    // from kernel32, upon return, filled lpBuffer length is result DWORD, which
    // requires special handling in unmarshalString; other functions might
    // require similar treatment, too:
    bool special =
        dll.equalsIgnoreAsciiCaseAsciiL(
            RTL_CONSTASCII_STRINGPARAM("KERNEL32.DLL")) &&
        (proc.name ==
         rtl::OString(RTL_CONSTASCII_STRINGPARAM("GetLogicalDriveStringsA")));
    for (int i = 1; i < (arguments == 0 ? 0 : arguments->Count()); ++i) {
        SbError e = marshal(
            true, arguments->Get(i), special && i == 2, stack, stack.size(),
            data);
        if (e != ERRCODE_NONE) {
            return e;
        }
        align(stack, 8, 0, 0);
    }

    stack.resize(20*8);

    // We fake all calls as being to a varargs function,
    // as this means any floating-point argument among the first four
    // ones will end up in a XMM register where the callee expects it.
    sal_Int32 (*proc_i)(double d, ...) = (sal_Int32 (*)(double, ...)) proc.proc;
    double (*proc_d)(double d, ...) = (double (*)(double, ...)) proc.proc;

    sal_Int64 iRetVal;
    double dRetVal;

    switch (result.GetType()) {
    case SbxEMPTY:
    case SbxINTEGER:
    case SbxLONG:
    case SbxSTRING:
    case SbxOBJECT:
    case SbxBOOL:
    case SbxBYTE:
        iRetVal =
            proc_i(*(double *)&stack[0*8],
                   *(double *)&stack[1*8],
                   *(double *)&stack[2*8],
                   *(double *)&stack[3*8],
                   *(sal_uInt64 *)&stack[4*8],
                   *(sal_uInt64 *)&stack[5*8],
                   *(sal_uInt64 *)&stack[6*8],
                   *(sal_uInt64 *)&stack[7*8],
                   *(sal_uInt64 *)&stack[8*8],
                   *(sal_uInt64 *)&stack[9*8],
                   *(sal_uInt64 *)&stack[10*8],
                   *(sal_uInt64 *)&stack[11*8],
                   *(sal_uInt64 *)&stack[12*8],
                   *(sal_uInt64 *)&stack[13*8],
                   *(sal_uInt64 *)&stack[14*8],
                   *(sal_uInt64 *)&stack[15*8],
                   *(sal_uInt64 *)&stack[16*8],
                   *(sal_uInt64 *)&stack[17*8],
                   *(sal_uInt64 *)&stack[18*8],
                   *(sal_uInt64 *)&stack[19*8]);
        break;
    case SbxSINGLE:
    case SbxDOUBLE:
        dRetVal =
            proc_d(*(double *)&stack[0*8],
                   *(double *)&stack[1*8],
                   *(double *)&stack[2*8],
                   *(double *)&stack[3*8],
                   *(sal_uInt64 *)&stack[4*8],
                   *(sal_uInt64 *)&stack[5*8],
                   *(sal_uInt64 *)&stack[6*8],
                   *(sal_uInt64 *)&stack[7*8],
                   *(sal_uInt64 *)&stack[8*8],
                   *(sal_uInt64 *)&stack[9*8],
                   *(sal_uInt64 *)&stack[10*8],
                   *(sal_uInt64 *)&stack[11*8],
                   *(sal_uInt64 *)&stack[12*8],
                   *(sal_uInt64 *)&stack[13*8],
                   *(sal_uInt64 *)&stack[14*8],
                   *(sal_uInt64 *)&stack[15*8],
                   *(sal_uInt64 *)&stack[16*8],
                   *(sal_uInt64 *)&stack[17*8],
                   *(sal_uInt64 *)&stack[18*8],
                   *(sal_uInt64 *)&stack[19*8]);
    }

    switch (result.GetType()) {
    case SbxEMPTY:
        break;
    case SbxINTEGER:
        result.PutInteger(static_cast< sal_Int16 >(iRetVal));
        break;
    case SbxLONG:
        result.PutLong(static_cast< sal_Int32 >(iRetVal));
        break;
    case SbxSINGLE:
        result.PutSingle(static_cast< float >(dRetVal));
        break;
    case SbxDOUBLE:
        result.PutDouble(dRetVal);
        break;
    case SbxSTRING:
        {
            char const * s1 = reinterpret_cast< char const * >(iRetVal);
            rtl::OUString s2;
            SbError e = convert(s1, rtl_str_getLength(s1), &s2);
            if (e != ERRCODE_NONE) {
                return e;
            }
            result.PutString(s2);
            break;
        }
    case SbxOBJECT:
        //TODO
        break;
    case SbxBOOL:
        result.PutBool(static_cast< sal_Bool >(iRetVal));
        break;
    case SbxBYTE:
        result.PutByte(static_cast< sal_uInt8 >(iRetVal));
        break;
    default:
        OSL_ASSERT(false);
        break;
    }
    for (int i = 1; i < (arguments == 0 ? 0 : arguments->Count()); ++i) {
        arguments->Get(i)->ResetFlag(SBX_REFERENCE);
            //TODO: skipped for errors?!?
    }
    for (std::vector< UnmarshalData >::iterator i(data.unmarshal.begin());
         i != data.unmarshal.end(); ++i)
    {
        unmarshal(i->variable, i->buffer);
    }
    for (std::vector< StringData >::iterator i(data.unmarshalStrings.begin());
         i != data.unmarshalStrings.end(); ++i)
    {
        SbError e = unmarshalString(*i, result);
        if (e != ERRCODE_NONE) {
            return e;
        }
    }
    return ERRCODE_NONE;
}

SbError getProcData(HMODULE handle, rtl::OUString const & name, ProcData * proc)
{
    OSL_ASSERT(proc != 0);
    if (name.getLength() != 0 && name[0] == '@') { //TODO: "@" vs. "#"???
        sal_Int32 n = name.copy(1).toInt32(); //TODO: handle bad input
        if (n <= 0 || n > 0xFFFF) {
            return ERRCODE_BASIC_BAD_ARGUMENT; //TODO: more specific errcode?
        }
        FARPROC p = GetProcAddress(handle, reinterpret_cast< LPCSTR >(n));
        if (p != 0) {
            proc->name = rtl::OString(RTL_CONSTASCII_STRINGPARAM("#")) +
                rtl::OString::valueOf(n);
            proc->proc = p;
            return ERRCODE_NONE;
        }
    } else {
        rtl::OString name8;
        SbError e = convert(name, &name8);
        if (e != ERRCODE_NONE) {
            return e;
        }
        FARPROC p = GetProcAddress(handle, name8.getStr());
        if (p != 0) {
            proc->name = name8;
            proc->proc = p;
            return ERRCODE_NONE;
        }
        sal_Int32 i = name8.indexOf('#');
        if (i != -1) {
            name8 = name8.copy(0, i);
            p = GetProcAddress(handle, name8.getStr());
            if (p != 0) {
                proc->name = name8;
                proc->proc = p;
                return ERRCODE_NONE;
            }
        }
        rtl::OString real(
            rtl::OString(RTL_CONSTASCII_STRINGPARAM("_")) + name8);
        p = GetProcAddress(handle, real.getStr());
        if (p != 0) {
            proc->name = real;
            proc->proc = p;
            return ERRCODE_NONE;
        }
        real = name8 + rtl::OString(RTL_CONSTASCII_STRINGPARAM("A"));
        p = GetProcAddress(handle, real.getStr());
        if (p != 0) {
            proc->name = real;
            proc->proc = p;
            return ERRCODE_NONE;
        }
    }
    return ERRCODE_BASIC_PROC_UNDEFINED;
}

struct Dll: public salhelper::SimpleReferenceObject {
private:
    typedef std::map< rtl::OUString, ProcData > Procs;

    virtual ~Dll();

public:
    Dll(): handle(0) {}

    SbError getProc(rtl::OUString const & name, ProcData * proc);

    HMODULE handle;
    Procs procs;
};

Dll::~Dll() {
    if (handle != 0 && !FreeLibrary(handle)) {
        OSL_TRACE("FreeLibrary(%p) failed with %u", handle, GetLastError());
    }
}

SbError Dll::getProc(rtl::OUString const & name, ProcData * proc) {
    Procs::iterator i(procs.find(name));
    if (i != procs.end()) {
        *proc = i->second;
        return ERRCODE_NONE;
    }
    SbError e = getProcData(handle, name, proc);
    if (e == ERRCODE_NONE) {
        procs.insert(Procs::value_type(name, *proc));
    }
    return e;
}

rtl::OUString fullDllName(rtl::OUString const & name) {
    rtl::OUString full(name);
    if (full.indexOf('.') == -1) {
        full += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(".DLL"));
    }
    return full;
}

}

struct SbiDllMgr::Impl: private boost::noncopyable {
private:
    typedef std::map< rtl::OUString, rtl::Reference< Dll > > Dlls;

public:
    Dll * getDll(rtl::OUString const & name);

    Dlls dlls;
};

Dll * SbiDllMgr::Impl::getDll(rtl::OUString const & name) {
    Dlls::iterator i(dlls.find(name));
    if (i == dlls.end()) {
        i = dlls.insert(Dlls::value_type(name, new Dll)).first;
        HMODULE h = LoadLibraryW(reinterpret_cast<LPCWSTR>(name.getStr()));
        if (h == 0) {
            dlls.erase(i);
            return 0;
        }
        i->second->handle = h;
    }
    return i->second.get();
}

SbError SbiDllMgr::Call(
    rtl::OUString const & function, rtl::OUString const & library,
    SbxArray * arguments, SbxVariable & result, bool cdeclConvention)
{
    if (cdeclConvention) {
        return ERRCODE_BASIC_NOT_IMPLEMENTED;
    }
    rtl::OUString dllName(fullDllName(library));
    Dll * dll = impl_->getDll(dllName);
    if (dll == 0) {
        return ERRCODE_BASIC_BAD_DLL_LOAD;
    }
    ProcData proc;
    SbError e = dll->getProc(function, &proc);
    if (e != ERRCODE_NONE) {
        return e;
    }
    return call(dllName, proc, arguments, result);
}

void SbiDllMgr::FreeDll(rtl::OUString const & library) {
    impl_->dlls.erase(library);
}

SbiDllMgr::SbiDllMgr(): impl_(new Impl) {}

SbiDllMgr::~SbiDllMgr() {}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
