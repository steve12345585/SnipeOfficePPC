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


#include "registry/registry.h"
#include "fileurl.hxx"

#include "rtl/ustring.hxx"

#include <stdio.h>
#include <string.h>

using rtl::OUString;
using namespace registry::tools;

#if (defined UNX)
int main( int argc, char * argv[] )
#else
int __cdecl main( int argc, char * argv[] )
#endif
{
    RegHandle       hReg;
    RegKeyHandle    hRootKey, hKey;

    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "using: regview registryfile [keyName]\n");
        exit(1);
    }

    OUString regName( convertToFileUrl(argv[1], strlen(argv[1])) );
    if (reg_openRegistry(regName.pData, &hReg, REG_READONLY))
    {
        fprintf(stderr, "open registry \"%s\" failed\n", argv[1]);
        exit(1);
    }

    if (!reg_openRootKey(hReg, &hRootKey))
    {
        if (argc == 3)
        {
            OUString keyName( OUString::createFromAscii(argv[2]) );
            if (!reg_openKey(hRootKey, keyName.pData, &hKey))
            {
                if (reg_dumpRegistry(hKey))
                {
                    fprintf(stderr, "dumping registry \"%s\" failed\n", argv[1]);
                }

                if (reg_closeKey(hKey))
                {
                    fprintf(stderr, "closing key \"%s\" of registry \"%s\" failed\n",
                            argv[2], argv[1]);
                }
            }
            else
            {
                fprintf(stderr, "key \"%s\" not exists in registry \"%s\"\n",
                        argv[2], argv[1]);
            }
        }
        else
        {
            if (reg_dumpRegistry(hRootKey))
            {
                fprintf(stderr, "dumping registry \"%s\" failed\n", argv[1]);
            }
        }

        if (reg_closeKey(hRootKey))
        {
            fprintf(stderr, "closing root key of registry \"%s\" failed\n", argv[1]);
        }
    }
    else
    {
        fprintf(stderr, "open root key of registry \"%s\" failed\n", argv[1]);
    }

    if (reg_closeRegistry(hReg))
    {
        fprintf(stderr, "closing registry \"%s\" failed\n", argv[1]);
        exit(1);
    }

    return(0);
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
