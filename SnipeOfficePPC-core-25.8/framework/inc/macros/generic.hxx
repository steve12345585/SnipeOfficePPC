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

#ifndef __FRAMEWORK_MACROS_GENERIC_HXX_
#define __FRAMEWORK_MACROS_GENERIC_HXX_

//_________________________________________________________________________________________________________________
//  includes
//_________________________________________________________________________________________________________________

#include <rtl/ustring.hxx>
#include <rtl/textenc.h>

//*****************************************************************************************************************
//  generic macros
//*****************************************************************************************************************

/*_________________________________________________________________________________________________________________
    DECLARE_ASCII( SASCIIVALUE )

    Use it to declare a constant ascii value at compile time in code.
    zB. OUSting sTest = DECLARE_ASCII( "Test" )
_________________________________________________________________________________________________________________*/

#define DECLARE_ASCII( SASCIIVALUE )                                                                            \
    ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( SASCIIVALUE ) )

/*_________________________________________________________________________________________________________________
    U2B( SUNICODEVALUE )
    B2U( SASCIIVALUE )

    Use it to convert unicode strings to ascii values and reverse ...
    We use UTF8 as default textencoding.
_________________________________________________________________________________________________________________*/

#define U2B( SUNICODEVALUE )                                                                                    \
    ::rtl::OUStringToOString( SUNICODEVALUE, RTL_TEXTENCODING_UTF8 )

#define B2U( SASCIIVALUE )                                                                                      \
    ::rtl::OStringToOUString( SASCIIVALUE, RTL_TEXTENCODING_UTF8 )

#endif  //  #ifndef __FRAMEWORK_MACROS_GENERIC_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
