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

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>

typedef int   gboolean;
typedef char  gchar;
typedef struct _GError GError;

struct _GError
{
  int domain;
  int code;
  char *message;
};

typedef enum {
  GNOME_VFS_OK
} GnomeVFSResult;

/*
 * Wrapper function which extracs gnome_url_show from libgnome
 */

gboolean gnome_url_show (const char *url, GError **error)
{
    void* handle = dlopen("libgnomevfs-2.so.0", RTLD_LAZY);
    gboolean ret = 0;

    (void)error; /* avoid warning due to unused parameter */

    if( NULL != handle )
    {
        gboolean (* init) (void) =
            (gboolean (*) (void)) dlsym(handle, "gnome_vfs_init");

        if( NULL != init && init() )
        {
            GnomeVFSResult (* func) (const char *url) =
                (GnomeVFSResult (*) (const char *)) dlsym(handle, "gnome_vfs_url_show");

            if( NULL != func )
                ret = (GNOME_VFS_OK == func(url));
        }

        dlclose(handle);
    }

    return ret;
}

/*
 * The intended use of this tool is to pass the argument to
 * the gnome_show_url function of libgnome2.
 */

int main(int argc, char *argv[] )
{
    GError *error = NULL;
    char *fallback;
    char *idx;
    int retcode = -1;

    if( argc != 2 )
    {
        fprintf( stderr, "Usage: gnome-open-url <uri>\n" );
        return -1;
    }

    if( gnome_url_show(argv[1], &error) )
    {
        return 0;
    }

    /*
     * launch open-url command by replacing gnome-open-url from
     * the command line. This is the fallback when running on
     * remote machines with no GNOME installed.
     */

    fallback = strdup(argv[0]);
    idx = strstr(fallback, "gnome-open-url");
    if ( NULL != idx )
    {
        char *args[3];
        strncpy(idx, "open-url", 9);
        args[0] = fallback;
        args[1] = argv[1];
        args[2] = NULL;
        retcode = execv(fallback, args);
    }
    free(fallback);

    return retcode;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
