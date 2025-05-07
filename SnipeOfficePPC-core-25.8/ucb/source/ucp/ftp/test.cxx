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


/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/

#include "test_ftpurl.hxx"
#include <stdio.h>
#include <unistd.h>



int main(int argc,char* argv[])
{

    typedef int (*INT_FUNC)(void);
    INT_FUNC tests[] = { test_ftpurl,
                   test_ftpparent,
                   test_ftpproperties,
                   test_ftpopen,
                   test_ftplist,
                   0 };  // don't remove ending zero

    int err_level = 0;

    fprintf(stderr,"-------       Testing       ----------\n");

    int i = -1;
    do {
        INT_FUNC f = tests[++i];
        if(f) {
            err_level += (*f)();
            fprintf(stderr,".");
        } else
            break;
    } while(true);

    if(err_level) {
        fprintf(stderr,"number of failed tests: %d\n",err_level);
        fprintf(stderr,"----------------------------------\n");
    } else
        fprintf(stderr,"no errors\n");

    return err_level;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
