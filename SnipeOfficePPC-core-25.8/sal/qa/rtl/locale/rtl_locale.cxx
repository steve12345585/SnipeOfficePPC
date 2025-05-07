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

#include <osl/thread.h>
#include <rtl/locale.hxx>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/plugin/TestPlugIn.h>

namespace rtl_locale
{
    // default locale for test purpose
    void setDefaultLocale()
    {
        rtl::OLocale::setDefault(rtl::OUString("de"), rtl::OUString("DE"), /* rtl::OUString() */ rtl::OUString("hochdeutsch") );
    }

class getDefault : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
        // start message
        rtl_locale::setDefaultLocale();
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void getDefault_000()
    {
        // this is demonstration code
        // CPPUNIT_ASSERT_MESSAGE("a message", 1 == 1);

        // due to the fact, we set the default locale at first, this test is no longer possible
       // ::rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        // CPPUNIT_ASSERT_MESSAGE("locale must be null", aLocale.getData() == NULL);

    }

    void getDefault_001()
    {
        // rtl::OLocale::setDefault(rtl::OUString("de"), rtl::OUString("DE"), rtl::OUString());
       rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        CPPUNIT_ASSERT_MESSAGE("locale must not null", aLocale.getData() != NULL);
    }

    // Change the following lines only, if you add, remove or rename
    // member functions of the current class,
    // because these macros are need by auto register mechanism.

    SAL_CPPUNIT_TEST_SUITE(getDefault);
    CPPUNIT_TEST(getDefault_000);
    CPPUNIT_TEST(getDefault_001);
    SAL_CPPUNIT_TEST_SUITE_END();
}; // class getDefault


class setDefault : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
        // start message
        rtl_locale::setDefaultLocale();
    }

    void tearDown()
    {
        setDefaultLocale();
    }

    // insert your test code here.
    void setDefault_001()
    {
        rtl::OLocale::setDefault(rtl::OUString("en"), rtl::OUString("US"), rtl::OUString());
       rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        CPPUNIT_ASSERT_MESSAGE("locale must not null", aLocale.getData() != NULL);

        // be sure to not GPF
    }

    // Change the following lines only, if you add, remove or rename
    // member functions of the current class,
    // because these macros are need by auto register mechanism.

    SAL_CPPUNIT_TEST_SUITE(setDefault);
    CPPUNIT_TEST(setDefault_001);
    SAL_CPPUNIT_TEST_SUITE_END();
}; // class setDefault


class getLanguage : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
        // start message
        rtl_locale::setDefaultLocale();
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void getLanguage_001()
    {
        rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        rtl::OUString suLanguage = aLocale.getLanguage();
        CPPUNIT_ASSERT_MESSAGE( "locale language must be 'de'", suLanguage == "de" );
    }
    void getLanguage_002()
    {
        rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        rtl::OUString suLanguage(rtl_locale_getLanguage(aLocale.getData()), SAL_NO_ACQUIRE);
        CPPUNIT_ASSERT_MESSAGE( "locale language must be 'de'", suLanguage == "de" );
    }

    // Change the following lines only, if you add, remove or rename
    // member functions of the current class,
    // because these macros are need by auto register mechanism.

    SAL_CPPUNIT_TEST_SUITE(getLanguage);
    CPPUNIT_TEST(getLanguage_001);
    CPPUNIT_TEST(getLanguage_002);
    SAL_CPPUNIT_TEST_SUITE_END();
}; // class getLanguage


class getCountry : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
        // start message
        rtl_locale::setDefaultLocale();
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void getCountry_001()
    {
        rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        rtl::OUString suCountry = aLocale.getCountry();
        CPPUNIT_ASSERT_MESSAGE( "locale country must be 'DE'", suCountry == "DE" );
    }
    void getCountry_002()
    {
        rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        rtl::OUString suCountry(rtl_locale_getCountry(aLocale.getData()), SAL_NO_ACQUIRE);
        CPPUNIT_ASSERT_MESSAGE( "locale country must be 'DE'", suCountry == "DE" );
    }

    // Change the following lines only, if you add, remove or rename
    // member functions of the current class,
    // because these macros are need by auto register mechanism.

    SAL_CPPUNIT_TEST_SUITE(getCountry);
    CPPUNIT_TEST(getCountry_001);
    CPPUNIT_TEST(getCountry_002);
    SAL_CPPUNIT_TEST_SUITE_END();
}; // class getCountry


class getVariant : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
        // start message
        rtl_locale::setDefaultLocale();
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void getVariant_001()
    {
        rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        rtl::OUString suVariant = aLocale.getVariant();
        CPPUNIT_ASSERT_MESSAGE( "locale variant must be 'hochdeutsch'", suVariant == "hochdeutsch" );
    }
    void getVariant_002()
    {
        rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        rtl::OUString suVariant(rtl_locale_getVariant(aLocale.getData()), SAL_NO_ACQUIRE);
        CPPUNIT_ASSERT_MESSAGE( "locale variant must be 'hochdeutsch'", suVariant == "hochdeutsch" );
    }

    // Change the following lines only, if you add, remove or rename
    // member functions of the current class,
    // because these macros are need by auto register mechanism.

    SAL_CPPUNIT_TEST_SUITE(getVariant);
    CPPUNIT_TEST(getVariant_001);
    CPPUNIT_TEST(getVariant_002);
    SAL_CPPUNIT_TEST_SUITE_END();
}; // class getVariant


class hashCode : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
        // start message
        rtl_locale::setDefaultLocale();
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void hashCode_001()
    {
        rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        sal_Int32 nHashCode = aLocale.hashCode();
        CPPUNIT_ASSERT_MESSAGE("locale hashcode must be 3831", nHashCode != 0);
    }
    void hashCode_002()
    {
        rtl::OLocale aLocale = ::rtl::OLocale::getDefault();
        sal_Int32 nHashCode = rtl_locale_hashCode(aLocale.getData());
        CPPUNIT_ASSERT_MESSAGE("locale hashcode must be 3831", nHashCode != 0);
    }

    // Change the following lines only, if you add, remove or rename
    // member functions of the current class,
    // because these macros are need by auto register mechanism.

    SAL_CPPUNIT_TEST_SUITE(hashCode);
    CPPUNIT_TEST(hashCode_001);
    CPPUNIT_TEST(hashCode_002);
    SAL_CPPUNIT_TEST_SUITE_END();
}; // class hashCode


class equals : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
        // start message
        rtl_locale::setDefaultLocale();
    }

    void tearDown()
    {
    }

    // insert your test code here.
    void equals_001()
    {
        rtl::OLocale aLocale1 = rtl::OLocale::registerLocale(rtl::OUString("en"), rtl::OUString("US"), rtl::OUString());
        rtl::OLocale aLocale2 = rtl::OLocale::registerLocale(rtl::OUString("en"), rtl::OUString("US"));

        sal_Bool bLocaleAreEqual = sal_False;
        bLocaleAreEqual = (aLocale1 == aLocale2);

        CPPUNIT_ASSERT_MESSAGE("check operator ==()", bLocaleAreEqual == sal_True);
    }

    void equals_002()
    {
        rtl::OLocale aLocale1 = rtl::OLocale::registerLocale(rtl::OUString("en"), rtl::OUString("US"), rtl::OUString());
        rtl::OLocale aLocale2 = rtl::OLocale::registerLocale(rtl::OUString("en"), rtl::OUString("US"));

        sal_Int32 nEqual = rtl_locale_equals(aLocale1.getData(), aLocale2.getData());
        CPPUNIT_ASSERT(nEqual != 0);
    }

    // Change the following lines only, if you add, remove or rename
    // member functions of the current class,
    // because these macros are need by auto register mechanism.

    SAL_CPPUNIT_TEST_SUITE(equals);
    CPPUNIT_TEST(equals_001);
    CPPUNIT_TEST(equals_002);
    SAL_CPPUNIT_TEST_SUITE_END();
}; // class equals

// -----------------------------------------------------------------------------
CPPUNIT_TEST_SUITE_REGISTRATION(rtl_locale::getDefault);
CPPUNIT_TEST_SUITE_REGISTRATION(rtl_locale::setDefault);
CPPUNIT_TEST_SUITE_REGISTRATION(rtl_locale::getLanguage);
CPPUNIT_TEST_SUITE_REGISTRATION(rtl_locale::getCountry);
CPPUNIT_TEST_SUITE_REGISTRATION(rtl_locale::getVariant);
CPPUNIT_TEST_SUITE_REGISTRATION(rtl_locale::hashCode);
CPPUNIT_TEST_SUITE_REGISTRATION(rtl_locale::equals);
} // namespace rtl_locale


// -----------------------------------------------------------------------------

// this macro creates an empty function, which will called by the RegisterAllFunctions()
// to let the user the possibility to also register some functions by hand.

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
