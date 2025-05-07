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

#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/plugin/TestPlugIn.h>

#include <rtl/ustrbuf.hxx>

#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/util/Date.hpp>
#include <com/sun/star/util/Duration.hpp>
#include <com/sun/star/util/MeasureUnit.hpp>

#include "sax/tools/converter.hxx"


using namespace ::com::sun::star;
using namespace ::com::sun::star::util::MeasureUnit;
using sax::Converter;


namespace {

class ConverterTest
    : public ::CppUnit::TestFixture
{
public:
    virtual void setUp();
    virtual void tearDown();

    void testDuration();
    void testDateTime();
    void testDouble();

    CPPUNIT_TEST_SUITE(ConverterTest);
    CPPUNIT_TEST(testDuration);
    CPPUNIT_TEST(testDateTime);
    CPPUNIT_TEST(testDouble);
    CPPUNIT_TEST_SUITE_END();

private:
};

void ConverterTest::setUp()
{
}

void ConverterTest::tearDown()
{
}

static bool eqDuration(util::Duration a, util::Duration b) {
    return a.Years == b.Years && a.Months == b.Months && a.Days == b.Days
        && a.Hours == b.Hours && a.Minutes == b.Minutes
        && a.Seconds == b.Seconds
        && a.MilliSeconds == b.MilliSeconds
        && a.Negative == b.Negative;
}

static void doTest(util::Duration const & rid, char const*const pis,
        char const*const i_pos = 0)
{
    char const*const pos((i_pos) ? i_pos : pis);
    util::Duration od;
    ::rtl::OUString is(::rtl::OUString::createFromAscii(pis));
    bool bSuccess = Converter::convertDuration(od, is);
    OSL_TRACE("%d %dY %dM %dD %dH %dM %dS %dm",
        od.Negative, od.Years, od.Months, od.Days,
        od.Hours, od.Minutes, od.Seconds, od.MilliSeconds);
    CPPUNIT_ASSERT(bSuccess);
    CPPUNIT_ASSERT(eqDuration(rid, od));
    ::rtl::OUStringBuffer buf;
    Converter::convertDuration(buf, od);
    OSL_TRACE("%s",
        ::rtl::OUStringToOString(buf.getStr(), RTL_TEXTENCODING_UTF8).getStr());
    CPPUNIT_ASSERT(buf.makeStringAndClear().equalsAscii(pos));
}

static void doTestDurationF(char const*const pis)
{
    util::Duration od;
    bool bSuccess = Converter::convertDuration(od,
            ::rtl::OUString::createFromAscii(pis));
    OSL_TRACE("%d %dY %dM %dD %dH %dM %dS %dH",
        od.Negative, od.Years, od.Months, od.Days,
        od.Hours, od.Minutes, od.Seconds, od.MilliSeconds);
    CPPUNIT_ASSERT(!bSuccess);
}

void ConverterTest::testDuration()
{
    OSL_TRACE("\nSAX CONVERTER TEST BEGIN");
    doTest( util::Duration(false, 1, 0, 0, 0, 0, 0, 0), "P1Y" );
    doTest( util::Duration(false, 0, 42, 0, 0, 0, 0, 0), "P42M" );
    doTest( util::Duration(false, 0, 0, 111, 0, 0, 0, 0), "P111D" );
    doTest( util::Duration(false, 0, 0, 0, 52, 0, 0, 0), "PT52H" );
    doTest( util::Duration(false, 0, 0, 0, 0, 717, 0, 0), "PT717M" );
    doTest( util::Duration(false, 0, 0, 0, 0, 0, 121, 0), "PT121S" );
    doTest( util::Duration(false, 0, 0, 0, 0, 0, 0, 190), "PT0.19S" );
    doTest( util::Duration(false, 0, 0, 0, 0, 0, 0, 90), "PT0.09S" );
    doTest( util::Duration(false, 0, 0, 0, 0, 0, 0, 9), "PT0.009S" );
    doTest( util::Duration(false, 0, 0, 0, 0, 0, 9, 999),
            "PT9.999999999999999999999999999999S", "PT9.999S" );
    doTest( util::Duration(true , 0, 0, 9999, 0, 0, 0, 0), "-P9999D" );
    doTest( util::Duration(true , 7, 6, 5, 4, 3, 2, 10),
            "-P7Y6M5DT4H3M2.01S" );
    doTest( util::Duration(false, 0, 6, 0, 0, 3, 0, 0), "P6MT3M" );
    doTest( util::Duration(false, 0, 0, 0, 0, 0, 0, 0), "P0D" );
    doTestDurationF("1Y1M");        // invalid: no ^P
    doTestDurationF("P-1Y1M");      // invalid: - after P
    doTestDurationF("P1M1Y");       // invalid: Y after M
    doTestDurationF("PT1Y");        // invalid: Y after T
    doTestDurationF("P1Y1M1M");     // invalid: M twice, no T
    doTestDurationF("P1YT1MT1M");   // invalid: T twice
    doTestDurationF("P1YT");        // invalid: T but no H,M,S
    doTestDurationF("P99999999999Y");   // cannot parse so many Ys
    doTestDurationF("PT.1S");       // invalid: no 0 preceding .
    doTestDurationF("PT5M.134S");   // invalid: no 0 preceding .
    doTestDurationF("PT1.S");       // invalid: no digit following .
    OSL_TRACE("\nSAX CONVERTER TEST END");
}


static bool eqDateTime(util::DateTime a, util::DateTime b) {
    return a.Year == b.Year && a.Month == b.Month && a.Day == b.Day
        && a.Hours == b.Hours && a.Minutes == b.Minutes
        && a.Seconds == b.Seconds
        && a.HundredthSeconds == b.HundredthSeconds;
}

static void doTest(util::DateTime const & rdt, char const*const pis,
        char const*const i_pos = 0)
{
    char const*const pos((i_pos) ? i_pos : pis);
    ::rtl::OUString is(::rtl::OUString::createFromAscii(pis));
    util::DateTime odt;
    bool bSuccess( Converter::convertDateTime(odt, is) );
    OSL_TRACE("Y:%d M:%d D:%d  H:%d M:%d S:%d H:%d",
        odt.Year, odt.Month, odt.Day,
        odt.Hours, odt.Minutes, odt.Seconds, odt.HundredthSeconds);
    CPPUNIT_ASSERT(bSuccess);
    CPPUNIT_ASSERT(eqDateTime(rdt, odt));
    ::rtl::OUStringBuffer buf;
    Converter::convertDateTime(buf, odt, true);
    OSL_TRACE("%s",
        ::rtl::OUStringToOString(buf.getStr(), RTL_TEXTENCODING_UTF8).getStr());
    CPPUNIT_ASSERT(buf.makeStringAndClear().equalsAscii(pos));
}

static void doTestDateTimeF(char const*const pis)
{
    util::DateTime odt;
    bool bSuccess = Converter::convertDateTime(odt,
            ::rtl::OUString::createFromAscii(pis));
    OSL_TRACE("Y:%d M:%d D:%d  H:%dH M:%d S:%d H:%d",
        odt.Year, odt.Month, odt.Day,
        odt.Hours, odt.Minutes, odt.Seconds, odt.HundredthSeconds);
    CPPUNIT_ASSERT(!bSuccess);
}

void ConverterTest::testDateTime()
{
    OSL_TRACE("\nSAX CONVERTER TEST BEGIN");
    doTest( util::DateTime(0, 0, 0, 0, 1, 1, 1), "0001-01-01T00:00:00" );
    doTest( util::DateTime(0, 0, 0, 0, 1, 1, 1),
            "0001-01-01T00:00:00Z", "0001-01-01T00:00:00" );
//    doTest( util::DateTime(0, 0, 0, 0, 1, 1, -1), "-0001-01-01T00:00:00" );
//    doTest( util::DateTime(0, 0, 0, 0, 1, 1, -1), "-0001-01-01T00:00:00Z" );
    doTest( util::DateTime(0, 0, 0, 0, 1, 1, 1),
            "0001-01-01T00:00:00-00:00", "0001-01-01T00:00:00" );
    doTest( util::DateTime(0, 0, 0, 0, 1, 1, 1),
            "0001-01-01T00:00:00+00:00", "0001-01-01T00:00:00" );
    doTest( util::DateTime(0, 0, 0, 0, 2, 1, 1)/*(0, 0, 12, 0, 2, 1, 1)*/,
            "0001-01-02T00:00:00-12:00", "0001-01-02T00:00:00" );
//            "0001-02-01T12:00:00" );
    doTest( util::DateTime(0, 0, 0, 0, 2, 1, 1)/*(0, 0, 12, 0, 1, 1, 1)*/,
            "0001-01-02T00:00:00+12:00", "0001-01-02T00:00:00" );
//            "0001-01-01T12:00:00" );
    doTest( util::DateTime(99, 59, 59, 23, 31, 12, 9999),
            "9999-12-31T23:59:59.99" );
    doTest( util::DateTime(99, 59, 59, 23, 31, 12, 9999),
            "9999-12-31T23:59:59.99Z", "9999-12-31T23:59:59.99" );
    doTest( util::DateTime(99, 59, 59, 23, 31, 12, 9999),
            "9999-12-31T23:59:59.9999999999999999999999999999999999999",
            "9999-12-31T23:59:59.99" );
    doTest( util::DateTime(99, 59, 59, 23, 31, 12, 9999),
            "9999-12-31T23:59:59.9999999999999999999999999999999999999Z",
            "9999-12-31T23:59:59.99" );
    doTest( util::DateTime(0, 0, 0, 0, 29, 2, 2000), // leap year
            "2000-02-29T00:00:00-00:00", "2000-02-29T00:00:00" );
    doTest( util::DateTime(0, 0, 0, 0, 29, 2, 1600), // leap year
            "1600-02-29T00:00:00-00:00", "1600-02-29T00:00:00" );
    doTest( util::DateTime(0, 0, 0, 24, 1, 1, 333)
                /*(0, 0, 0, 0, 2, 1, 333)*/,
            "0333-01-01T24:00:00"/*, "0333-01-02T00:00:00"*/ );
    // While W3C XMLSchema specifies a minimum of 4 year digits we are lenient
    // in what we accept.
    doTest( util::DateTime(0, 0, 0, 0, 1, 1, 1),
            "1-01-01T00:00:00", "0001-01-01T00:00:00" );
    doTestDateTimeF( "+0001-01-01T00:00:00" ); // invalid: ^+
    doTestDateTimeF( "0001-1-01T00:00:00" ); // invalid: < 2 M
    doTestDateTimeF( "0001-01-1T00:00:00" ); // invalid: < 2 D
    doTestDateTimeF( "0001-01-01T0:00:00" ); // invalid: < 2 H
    doTestDateTimeF( "0001-01-01T00:0:00" ); // invalid: < 2 M
    doTestDateTimeF( "0001-01-01T00:00:0" ); // invalid: < 2 S
    doTestDateTimeF( "0001-01-01T00:00:00." ); // invalid: .$
    doTestDateTimeF( "0001-01-01T00:00:00+1:00" ); // invalid: < 2 TZ H
    doTestDateTimeF( "0001-01-01T00:00:00+00:1" ); // invalid: < 2 TZ M
    doTestDateTimeF( "0001-13-01T00:00:00" ); // invalid: M > 12
    doTestDateTimeF( "0001-01-32T00:00:00" ); // invalid: D > 31
    doTestDateTimeF( "0001-01-01T25:00:00" ); // invalid: H > 24
    doTestDateTimeF( "0001-01-01T00:60:00" ); // invalid: H > 59
    doTestDateTimeF( "0001-01-01T00:00:60" ); // invalid: S > 59
    doTestDateTimeF( "0001-01-01T24:01:00" ); // invalid: H=24, but M != 0
    doTestDateTimeF( "0001-01-01T24:00:01" ); // invalid: H=24, but S != 0
    doTestDateTimeF( "0001-01-01T24:00:00.1" ); // invalid: H=24, but H != 0
    doTestDateTimeF( "0001-01-02T00:00:00+15:00" ); // invalid: TZ > +14:00
    doTestDateTimeF( "0001-01-02T00:00:00+14:01" ); // invalid: TZ > +14:00
    doTestDateTimeF( "0001-01-02T00:00:00-15:00" ); // invalid: TZ < -14:00
    doTestDateTimeF( "0001-01-02T00:00:00-14:01" ); // invalid: TZ < -14:00
    doTestDateTimeF( "2100-02-29T00:00:00-00:00" ); // invalid: no leap year
    doTestDateTimeF( "1900-02-29T00:00:00-00:00" ); // invalid: no leap year
    OSL_TRACE("\nSAX CONVERTER TEST END");
}

void doTestDouble(char const*const pis, double const rd,
        sal_Int16 const nSourceUnit, sal_Int16 const nTargetUnit)
{
    ::rtl::OUString const is(::rtl::OUString::createFromAscii(pis));
    double od;
    bool bSuccess(Converter::convertDouble(od, is, nSourceUnit, nTargetUnit));
    OSL_TRACE("%f", od);
    CPPUNIT_ASSERT(bSuccess);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(rd, od, 0.00000001);
    ::rtl::OUStringBuffer buf;
    Converter::convertDouble(buf, od, true, nTargetUnit, nSourceUnit);
    OSL_TRACE("%s",
        ::rtl::OUStringToOString(buf.getStr(), RTL_TEXTENCODING_UTF8).getStr());
    CPPUNIT_ASSERT_EQUAL(is, buf.makeStringAndClear());
}

void ConverterTest::testDouble()
{
    doTestDouble("42", 42.0, TWIP, TWIP);
    doTestDouble("42", 42.0, POINT, POINT);
    doTestDouble("42", 42.0, MM_100TH, MM_100TH);
    doTestDouble("42", 42.0, MM_10TH, MM_10TH);
    doTestDouble("42", 42.0, MM, MM); // identity don't seem to add unit?
    doTestDouble("42", 42.0, CM, CM);
    doTestDouble("42", 42.0, INCH, INCH);
    doTestDouble("2pt", 40.0, POINT, TWIP);
    doTestDouble("20pc", 1, TWIP, POINT);
    doTestDouble("4", 2.26771653543307, MM_100TH, TWIP);
    doTestDouble("4", 22.6771653543307, MM_10TH, TWIP);
    doTestDouble("4mm", 226.771653543307, MM, TWIP);
    doTestDouble("4cm", 2267.71653543307, CM, TWIP);
    doTestDouble("4in", 5760.0, INCH, TWIP);
    doTestDouble("1440pc", 1.0, TWIP, INCH);
    doTestDouble("567pc", 1.000125, TWIP, CM);
    doTestDouble("56.7pc", 1.000125, TWIP, MM);
    doTestDouble("5.67pc", 1.000125, TWIP, MM_10TH);
    doTestDouble("0.567pc", 1.000125, TWIP, MM_100TH);
    doTestDouble("42pt", 1.4816666666666, POINT, CM);
    doTestDouble("42pt", 14.816666666666, POINT, MM);
    doTestDouble("42pt", 148.16666666666, POINT, MM_10TH);
    doTestDouble("42pt", 1481.6666666666, POINT, MM_100TH);
    doTestDouble("72pt", 1.0, POINT, INCH);
    doTestDouble("3.5in", 8.89, INCH, CM);
    doTestDouble("3.5in", 88.9, INCH, MM);
    doTestDouble("3.5in", 889.0, INCH, MM_10TH);
    doTestDouble("3.5in", 8890.0, INCH, MM_100TH);
    doTestDouble("2in", 144, INCH, POINT);
    doTestDouble("5.08cm", 2.0, CM, INCH);
    doTestDouble("3.5cm", 3500.0, CM, MM_100TH);
    doTestDouble("3.5cm", 350.0, CM, MM_10TH);
    doTestDouble("3.5cm", 35.0, CM, MM);
    doTestDouble("10cm", 283.464566929134, CM, POINT);
    doTestDouble("0.5cm", 283.464566929134, CM, TWIP);
    doTestDouble("10mm", 28.3464566929134, MM, POINT);
    doTestDouble("0.5mm", 28.3464566929134, MM, TWIP);
    doTestDouble("10", 2.83464566929134, MM_10TH, POINT);
    doTestDouble("0.5", 2.83464566929134, MM_10TH, TWIP);
    doTestDouble("10", 0.283464566929134, MM_100TH, POINT);
    doTestDouble("0.5", 0.283464566929134, MM_100TH, TWIP);
    doTestDouble("10mm", 1.0, MM, CM);
    doTestDouble("10mm", 100.0, MM, MM_10TH);
    doTestDouble("20mm", 2000.0, MM, MM_100TH);
    doTestDouble("300", 30.0, MM_10TH, MM);
    doTestDouble("400", 4.0, MM_100TH, MM);
    doTestDouble("600", 6000.0, MM_10TH, MM_100TH);
    doTestDouble("700", 70.0, MM_100TH, MM_10TH);
}

CPPUNIT_TEST_SUITE_REGISTRATION(ConverterTest);

}

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
