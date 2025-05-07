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

#define CSV_USE_CSV_ASSERTIONS
#include <cosv/csv_env.hxx>

#include <cosv/comfunc.hxx>
#include <cosv/string.hxx>
#include <cosv/streamstr.hxx>
#include <cosv/std_outp.hxx>
#include <cosv/tpl/dyn.hxx>
#include <cosv/file.hxx>

// NOT FULLY DECLARED SERVICES


namespace csv
{


File::File( const char *    i_sLocation,
            uintt           i_nMode )
    :   aPath(i_sLocation),
        pStream(0),
        nMode(i_nMode),
        eLastIO(io_none)
{
}

File::File( const String &  i_sLocation,
            uintt           i_nMode )
    :   aPath(i_sLocation),
        pStream(0),
        nMode(i_nMode),
        eLastIO(io_none)
{
}

File::~File()
{
    if ( inq_is_open() )
        close();
}

uintt
File::do_read( void *          out_pDest,
               uintt           i_nNrofBytes )
{
    if ( NOT inq_is_open() )
        return 0;

    if ( eLastIO == io_write )
        ::fseek( pStream, 0, SEEK_CUR );
    uintt ret = position();
    int iRet= ::fread( out_pDest, 1, i_nNrofBytes, pStream );
    if ( iRet < 0  )  {
    fprintf(stderr, "warning: read failed in %s line %d \n", __FILE__, __LINE__);
    }
    ret = position() - ret;

    eLastIO = io_read;
    return ret;
}

bool
File::inq_eod() const
{
    if ( NOT inq_is_open() )
        return true;
    return feof(pStream) != 0;
}

uintt
File::do_write( const void *    i_pSrc,
                uintt           i_nNrofBytes )
{
    if ( NOT inq_is_open() )
        return 0;

    if ( eLastIO == io_write )
        ::fseek( pStream, 0, SEEK_CUR );

    uintt ret = ::fwrite( i_pSrc, 1, i_nNrofBytes, pStream );

    eLastIO = io_write;
    return ret;
}

uintt
File::do_seek( intt     i_nDistance,
               seek_dir i_eStartPoint )
{
    if ( NOT inq_is_open() )
        return uintt(-1);

    static int eSearchDir[3] = { SEEK_SET, SEEK_CUR, SEEK_END };

    ::fseek( pStream,
             intt(i_nDistance),
             eSearchDir[i_eStartPoint] );
    return position();
}

uintt
File::inq_position() const
{
    if ( inq_is_open() )
        return uintt( ::ftell(pStream) );
    else
        return uintt(-1);
}

bool
File::do_open( uintt i_nOpenMode )
{
    if ( inq_is_open() )
    {
        if ( i_nOpenMode == 0 OR i_nOpenMode == nMode )
            return true;
        close();
    }

    if ( i_nOpenMode != 0 )
        nMode = i_nOpenMode;

    const char * sFacadeMode = "";
    switch ( nMode )
    {
         case CFM_RW:        sFacadeMode = "r+b";
                            break;
         case CFM_CREATE:    sFacadeMode = "w+b";
                            break;
         case CFM_READ:      sFacadeMode = "rb";
                            break;
        default:
                            sFacadeMode = "rb";
    }

    pStream = ::fopen( StrPath(), sFacadeMode );
    if ( pStream == 0 AND nMode == CFM_RW )
    {
        sFacadeMode = "w+b";
        pStream = ::fopen( StrPath(), sFacadeMode );
    }

    return pStream != 0;
}

void
File::do_close()
{
    if ( inq_is_open() )
    {
        ::fclose(pStream);
        pStream = 0;
    }
    eLastIO = io_none;
}

bool
File::inq_is_open() const
{
    return pStream != 0;
}

const ploc::Path &
File::inq_MyPath() const
{
     return aPath;
}


}   // namespace csv

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
