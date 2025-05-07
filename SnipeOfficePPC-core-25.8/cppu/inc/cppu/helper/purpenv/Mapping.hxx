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

#ifndef INCLUDED_cppu_helper_purpenv_Mapping_hxx
#define INCLUDED_cppu_helper_purpenv_Mapping_hxx

#include "com/sun/star/uno/Any.h"

#include <cppu/cppudllapi.h>
#include "uno/environment.h"
#include "uno/mapping.h"


namespace cppu { namespace helper { namespace purpenv {

/** C++ helper for implementing Purpose Environments.
    (http://wiki.services.openoffice.org/wiki/Uno/Cpp/Spec/Purpose_Bridge_Implementation_Helper)

    @since UDK 3.2.7
*/

typedef void ProbeFun(
    bool                                pre,
    void                              * pThis,
    void                              * pContext,
    typelib_TypeDescriptionReference  * pReturnTypeRef,
    typelib_MethodParameter           * pParams,
    sal_Int32                           nParams,
    typelib_TypeDescription     const * pMemberType,
    void                              * pReturn,
    void                              * pArgs[],
    uno_Any                          ** ppException );



PURPENV_DLLPUBLIC  void createMapping(uno_Mapping     ** ppMapping,
                   uno_Environment  * pFrom,
                   uno_Environment  * pTo,
                   ProbeFun         * probeFun  = NULL,
                   void             * pContext  = NULL
    );

}}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
