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

#include "sal/config.h"
#include "sal/precppunit.hxx"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "rtl/string.h"
#include "rtl/ustring.hxx"

namespace test { namespace oustring {

class Compare: public CppUnit::TestFixture
{
private:
    void equalsIgnoreAsciiCaseAscii();

CPPUNIT_TEST_SUITE(Compare);
CPPUNIT_TEST(equalsIgnoreAsciiCaseAscii);
CPPUNIT_TEST_SUITE_END();
};

} }

CPPUNIT_TEST_SUITE_REGISTRATION(test::oustring::Compare);

void test::oustring::Compare::equalsIgnoreAsciiCaseAscii()
{
    CPPUNIT_ASSERT(!rtl::OUString().equalsIgnoreAsciiCaseAscii("abc"));
    CPPUNIT_ASSERT(!rtl::OUString().equalsIgnoreAsciiCaseAsciiL(
                       RTL_CONSTASCII_STRINGPARAM("abc")));
    CPPUNIT_ASSERT(!rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("abc")).
                   equalsIgnoreAsciiCaseAscii(""));
    CPPUNIT_ASSERT(!rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("abc")).
                   equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("")));

    CPPUNIT_ASSERT(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("abc")).
                   equalsIgnoreAsciiCaseAscii("abc"));
    CPPUNIT_ASSERT(!rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("abcd")).
                   equalsIgnoreAsciiCaseAscii("abc"));
    CPPUNIT_ASSERT(!rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("abc")).
                   equalsIgnoreAsciiCaseAscii("abcd"));
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
