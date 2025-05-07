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
#include <rtl/string.hxx>
#include <typeinfo>

typedef struct _uno_Any uno_Any;
typedef struct _uno_Mapping uno_Mapping;

// private C50 structures and functions
namespace __Crun
{
    struct static_type_info
    {
        std::ptrdiff_t m_pClassName;
        int         m_nSkip1; // must be 0
        void*       m_pMagic; // points to some magic data
        int         m_nMagic[ 4 ];
        int         m_nSkip2[2]; // must be 0
    };
    void* ex_alloc(unsigned);
    void ex_throw( void*, const static_type_info*, void(*)(void*));
    void* ex_get();
    void ex_rethrow_q() throw();
}

namespace __Cimpl
{
    const char* ex_name();
}

extern "C" void _ex_register( void*, int );

namespace CPPU_CURRENT_NAMESPACE
{

inline char* adjustPointer( char* pIn, typelib_TypeDescription* pType )
{
    switch( pType->nSize )
    {
        case 1: return pIn + 3;
        case 2: return pIn + 2;
        case 3: return pIn + 1;
            // Huh ? perhaps a char[3] ? Though that would be a pointer
            // well, we have it anyway for symmetry
    }
    return pIn;
}

//##################################################################################################
//#### exceptions ##################################################################################
//##################################################################################################

void cc50_solaris_sparc_raiseException(
    uno_Any * pUnoExc, uno_Mapping * pUno2Cpp );

void cc50_solaris_sparc_fillUnoException(
    void*, const char*,
    uno_Any*, uno_Mapping * pCpp2Uno );

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
