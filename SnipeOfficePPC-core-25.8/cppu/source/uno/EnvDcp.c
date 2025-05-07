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

#include "uno/EnvDcp.h"


CPPU_DLLPUBLIC void uno_EnvDcp_getTypeName(rtl_uString const * pEnvDcp, rtl_uString ** ppEnvTypeName)
{
    sal_Int32 colIdx = rtl_ustr_indexOfChar_WithLength(pEnvDcp->buffer, pEnvDcp->length, ':');
    if (colIdx >= 0)
        rtl_uString_newFromStr_WithLength(ppEnvTypeName, pEnvDcp->buffer, colIdx);

    else
        rtl_uString_newFromStr(ppEnvTypeName, pEnvDcp->buffer);
}

CPPU_DLLPUBLIC void uno_EnvDcp_getPurpose(rtl_uString const * pEnvDcp, rtl_uString ** ppEnvPurpose)
{
    sal_Int32 colIdx = rtl_ustr_indexOfChar_WithLength(pEnvDcp->buffer, pEnvDcp->length, ':');
    if (colIdx >= 0)
        rtl_uString_newFromStr_WithLength(ppEnvPurpose, pEnvDcp->buffer + colIdx, pEnvDcp->length - colIdx);

    else
        rtl_uString_new(ppEnvPurpose);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
