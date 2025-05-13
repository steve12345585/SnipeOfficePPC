/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#define CSV_USE_CSV_ASSERTIONS
#include <cosv/csv_env.hxx>

#include <cosv/comfunc.hxx>
#include <cosv/string.hxx>
#include <cosv/streamstr.hxx>
#include <cosv/std_outp.hxx>
#include <cosv/tpl/dyn.hxx>

// NOT FULLY DECLARED SERVICES
#include <ut.hxx>

using csv::SimpleString;


/** @file
    UnitTests for class String.
*/

inline bool
check_value( const String & a, const char * b )
{
     return strcmp( a.c_str(), b ) == 0;
}


bool
classtest_String( csv::SimpleString & rSimpleString )
{
    bool ret = true;

    String x1;
    UT_CHECK( String(), check_value(x1,"") )

    const char * s2a = "";
    String x2a(s2a);
    UT_CHECK( String(const char*), check_value(x2a,s2a) )

    const char * s2b = "_zluoadninger  prrg chi��i(/%$##@\\\"'''fh  kl";
    String x2b(s2b);
    UT_CHECK( String(const char*), check_value(x2b,s2b) )


    bool ret = ftest_Read( rSimpleString );
    ret = ftest_Write( rSimpleString ) AND ret;
    ret = ftest_SeekBack( rSimpleString ) AND ret;
    ret = ftest_SeekRelative( rSimpleString ) AND ret;

    rSimpleString.Close();

    return ret;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
