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

#include "java/lang/Throwable.hxx"
#include "java/tools.hxx"
using namespace connectivity;
//**************************************************************
//************ Class: java.lang.Throwable
//**************************************************************

jclass java_lang_Throwable::theClass = 0;

java_lang_Throwable::~java_lang_Throwable()
{}

jclass java_lang_Throwable::getMyClass() const
{
    return st_getMyClass();
}
jclass java_lang_Throwable::st_getMyClass()
{
    // the class needs to be fetched only once, that is why it is static
    if( !theClass )
        theClass = findMyClass("java/lang/Throwable");
    return theClass;
}
// -----------------------------------------------------------------------------

::rtl::OUString java_lang_Throwable::getMessage() const
{
    static jmethodID mID(NULL);
    return callStringMethod("getMessage",mID);
}
// -----------------------------------------------------------------------------

::rtl::OUString java_lang_Throwable::getLocalizedMessage() const
{
    static jmethodID mID(NULL);
    return callStringMethod("getLocalizedMessage",mID);
}
// -----------------------------------------------------------------------------


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
