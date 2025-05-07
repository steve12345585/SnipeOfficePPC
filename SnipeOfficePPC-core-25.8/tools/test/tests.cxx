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
#include <cppunit/plugin/TestPlugIn.h>

#include <rtl/math.hxx>
#include <tools/fract.hxx>

namespace tools
{

class FractionTest : public CppUnit::TestFixture
{
public:
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void testFraction()
    {
        const Fraction aFract(1082130431,1073741824);
        CPPUNIT_ASSERT_MESSAGE( "Fraction #1 not approximately equal to 1.007812499068677",
                                rtl::math::approxEqual((double)aFract,1.007812499068677) );

        Fraction aFract2( aFract );
        aFract2.ReduceInaccurate(8);
        CPPUNIT_ASSERT_MESSAGE( "Fraction #2 not 1",
                                aFract2.GetNumerator() == 1 &&
                                aFract2.GetDenominator() == 1 );

        Fraction aFract3( 0x7AAAAAAA, 0x35555555 );
        CPPUNIT_ASSERT_MESSAGE( "Fraction #3 cancellation wrong",
                                aFract3.GetNumerator() == 0x7AAAAAAA &&
                                aFract3.GetDenominator() == 0x35555555 );
        aFract3.ReduceInaccurate(30);
        CPPUNIT_ASSERT_MESSAGE( "Fraction #3 ReduceInaccurate errorneously cut precision",
                                aFract3.GetNumerator() == 0x7AAAAAAA &&
                                aFract3.GetDenominator() == 0x35555555 );

        aFract3.ReduceInaccurate(29);
        CPPUNIT_ASSERT_MESSAGE( "Fraction #3 reduce to 29 bits failed",
                                aFract3.GetNumerator() == 0x3D555555 &&
                                aFract3.GetDenominator() == 0x1AAAAAAA );

        aFract3.ReduceInaccurate(9);
        CPPUNIT_ASSERT_MESSAGE( "Fraction #3 reduce to 9 bits failed",
                                aFract3.GetNumerator() == 0x0147 &&
                                aFract3.GetDenominator() == 0x008E );

        aFract3.ReduceInaccurate(1);
        CPPUNIT_ASSERT_MESSAGE( "Fraction #3 reduce to 1 bit failed",
                                aFract3.GetNumerator() == 2 &&
                                aFract3.GetDenominator() == 1 );

        aFract3.ReduceInaccurate(0);
        CPPUNIT_ASSERT_MESSAGE( "Fraction #3 reduce to 0 bits failed",
                                aFract3.GetNumerator() == 2 &&
                                aFract3.GetDenominator() == 1 );

#if SAL_TYPES_SIZEOFLONG == 8
        Fraction aFract4(0x7AAAAAAAAAAAAAAA, 0x3555555555555555);
        CPPUNIT_ASSERT_MESSAGE( "Fraction #4 cancellation wrong",
                                aFract4.GetNumerator() == 0x7AAAAAAAAAAAAAAA &&
                                aFract4.GetDenominator() == 0x3555555555555555 );
        aFract4.ReduceInaccurate(62);
        CPPUNIT_ASSERT_MESSAGE( "Fraction #4 ReduceInaccurate errorneously cut precision",
                                aFract4.GetNumerator() == 0x7AAAAAAAAAAAAAAA &&
                                aFract4.GetDenominator() == 0x3555555555555555 );

        aFract4.ReduceInaccurate(61);
        CPPUNIT_ASSERT_MESSAGE( "Fraction #4 ReduceInaccurate reduce to 61 bit failed",
                                aFract4.GetNumerator() == 0x3D55555555555555 &&
                                aFract4.GetDenominator() == 0x1AAAAAAAAAAAAAAA );
#endif
    }

    CPPUNIT_TEST_SUITE(FractionTest);
    CPPUNIT_TEST(testFraction);
    CPPUNIT_TEST_SUITE_END();
};

// -----------------------------------------------------------------------------
CPPUNIT_TEST_SUITE_REGISTRATION(FractionTest);
} // namespace tools

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
