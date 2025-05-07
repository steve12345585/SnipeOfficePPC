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
#ifndef _SFXMINSTACK_HXX
#define _SFXMINSTACK_HXX

#include <sfx2/minarray.hxx>

#define DECL_PTRSTACK( ARR, T, nI, nG ) \
DECL_PTRARRAY( ARR##arr_, T, nI, nG ) \
class ARR: private ARR##arr_ \
{ \
public: \
    ARR( sal_uInt8 nInitSize = nI, sal_uInt8 nGrowSize = nG ): \
        ARR##arr_( nInitSize, nGrowSize ) \
    {} \
\
    ARR( const ARR& rOrig ): \
        ARR##arr_( rOrig ) \
    {} \
\
    sal_uInt16      Count() const { return ARR##arr_::Count(); } \
    void        Push( T rElem ) { Append( rElem ); } \
    T           Top( sal_uInt16 nLevel = 0 ) const \
                { return (*this)[Count()-nLevel-1]; } \
    T           Bottom() const { return (*this)[0]; } \
    T           Pop() \
                {   T aRet = (*this)[Count()-1]; \
                    Remove( Count()-1, 1 ); \
                    return aRet; \
                } \
    T*       operator*() \
                { return &(*this)[Count()-1]; } \
    void        Clear() { ARR##arr_::Clear(); } \
    sal_Bool        Contains( const T pItem ) const \
                { return ARR##arr_::Contains( pItem ); } \
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
