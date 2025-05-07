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

// NOT FULLY DECLARED SERVICES
#include <stdlib.h>


namespace csv
{

#ifdef CSV_NO_IOSTREAMS
redirect_out *  redirect_out::pStdOut_ = 0;
redirect_out *  redirect_out::pStdErr_ = 0;
#endif // defined(CSV_NO_IOSTREAMS)


void
Endl( ostream & io_rStream )
{
#ifndef CSV_NO_IOSTREAMS
//    if (NOT bUseRedirect)
        io_rStream << std::endl;
//    else
#endif
//        static_cast< redirect_out& >(io_rStream).re_endl();
}

void
Flush( ostream & io_rStream )
{
#ifndef CSV_NO_IOSTREAMS
//    if (NOT bUseRedirect)
        io_rStream << std::flush;
//    else
#endif
//        static_cast< redirect_out& >(io_rStream).re_flush();
}

void
PerformAssertion(const char * condition, const char * file, unsigned line)
{
    Cout() << "assertion failed: "
         << condition
         << " in file: "
         << file
         << " at line: "
         << line
         << Endl;

    exit(3);
}

}   // namespace csv

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
