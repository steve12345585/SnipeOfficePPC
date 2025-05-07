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

#include "java/lang/String.hxx"
#include "java/tools.hxx"
using namespace connectivity;
//**************************************************************
//************ Class: java.lang.String
//**************************************************************

jclass java_lang_String::theClass = 0;

java_lang_String::~java_lang_String()
{}

jclass java_lang_String::getMyClass() const
{
    return st_getMyClass();
}
jclass java_lang_String::st_getMyClass()
{
    // the class needs to be fetched only once, that is why it is static
    if( !theClass )
        theClass = findMyClass("java/lang/String");
    return theClass;
}

//--------------------------------------------------------------------------
java_lang_String::operator ::rtl::OUString()
{
    SDBThreadAttach t;
    if(!t.pEnv)
        return ::rtl::OUString();
    return JavaString2String(t.pEnv,(jstring)object);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
