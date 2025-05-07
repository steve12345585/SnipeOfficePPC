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

#ifndef INCLUDED_uno_EnvDcp_h
#define INCLUDED_uno_EnvDcp_h

#include <cppu/cppudllapi.h>
#include "rtl/ustring.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** Get the OBI type part of an environment descriptor.
    (http://wiki.services.openoffice.org/wiki/Uno/Binary/Spec/Environment_Descriptor)

    @param pEnvDcp        the Environment Descriptor
    @param ppEnvTypeName  the OBI type
    @since UDK 3.2.7
*/
CPPU_DLLPUBLIC void uno_EnvDcp_getTypeName(rtl_uString const * pEnvDcp, rtl_uString ** ppEnvTypeName);


/** Get the purpose part of an environment descriptor.
    (http://wiki.services.openoffice.org/wiki/Uno/Binary/Spec/Environment_Descriptor)

    @param pEnvDcp        the Environment Descriptor
    @param ppEnvPurpose   the purpose
    @since UDK 3.2.7
*/
CPPU_DLLPUBLIC void uno_EnvDcp_getPurpose (rtl_uString const * pEnvDcp, rtl_uString ** ppEnvPurpose);


#ifdef __cplusplus
}
#endif


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
