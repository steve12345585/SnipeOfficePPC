/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the Original Code is
 *       [ insert your name / company etc. here eg. Jim Bob <jim@bob.org> ]
 * Portions created by the Initial Developer are Copyright (C) 2010 the
 * Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Jim Bob <jim@bob.org>
 *                 Ted <ted@bear.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#include "sal/config.h"
#include "sal/precppunit.hxx"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/plugin/TestPlugIn.h>

#include <editeng/borderline.hxx>

using namespace ::com::sun::star::table::BorderLineStyle;

#define TEST_WIDTH long( 40 )

#define THINTHICKSG_IN_WIDTH long( 15 )
#define THINTHICKSG_OUT_WIDTH long( 40 )
#define THINTHICKSG_DIST_WIDTH long( 15 )

#define THINTHICKLG_IN_WIDTH long( 15 )
#define THINTHICKLG_OUT_WIDTH long( 30 )
#define THINTHICKLG_DIST_WIDTH long( 40 )

using namespace editeng;

CPPUNIT_NS_BEGIN

template<> struct assertion_traits<SvxBorderStyle>
{
    static bool equal( const SvxBorderStyle& x, const SvxBorderStyle& y )
    {
        return x == y;
    }

    static std::string toString( const SvxBorderStyle& x )
    {
        OStringStream ost;
        ost << static_cast<unsigned int>(x);
        return ost.str();
    }
};

CPPUNIT_NS_END

namespace {

class BorderLineTest : public CppUnit::TestFixture
{
    public:
        void testGuessWidthDouble();
        void testGuessWidthNoMatch();
        void testGuessWidthThinthickSmallgap();
        void testGuessWidthThinthickLargegap();
        void testGuessWidthNostyleDouble();
        void testGuessWidthNostyleSingle();

    CPPUNIT_TEST_SUITE(BorderLineTest);
    CPPUNIT_TEST(testGuessWidthDouble);
    CPPUNIT_TEST(testGuessWidthNoMatch);
    CPPUNIT_TEST(testGuessWidthThinthickSmallgap);
    CPPUNIT_TEST(testGuessWidthThinthickLargegap);
    CPPUNIT_TEST(testGuessWidthNostyleDouble);
    CPPUNIT_TEST(testGuessWidthNostyleSingle);
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(BorderLineTest);

void BorderLineTest::testGuessWidthDouble()
{
    // Normal double case
    SvxBorderLine line;
    line.GuessLinesWidths( DOUBLE, TEST_WIDTH, TEST_WIDTH, TEST_WIDTH );
    CPPUNIT_ASSERT_EQUAL( DOUBLE, line.GetBorderLineStyle() );
    CPPUNIT_ASSERT_EQUAL( TEST_WIDTH, static_cast<long>(line.GetOutWidth()) );
    CPPUNIT_ASSERT_EQUAL( TEST_WIDTH, static_cast<long>(line.GetInWidth()) );
    CPPUNIT_ASSERT_EQUAL( TEST_WIDTH, static_cast<long>(line.GetDistance()) );
    CPPUNIT_ASSERT_EQUAL( 3*TEST_WIDTH, line.GetWidth() );
}

void BorderLineTest::testGuessWidthNoMatch()
{
    SvxBorderLine line;
    line.GuessLinesWidths( DOUBLE,
            TEST_WIDTH + 1, TEST_WIDTH + 2, TEST_WIDTH + 3 );
    CPPUNIT_ASSERT_EQUAL( DOUBLE, line.GetBorderLineStyle() );
    CPPUNIT_ASSERT_EQUAL( TEST_WIDTH+1, static_cast<long>(line.GetOutWidth()) );
    CPPUNIT_ASSERT_EQUAL( TEST_WIDTH+2, static_cast<long>(line.GetInWidth()) );
    CPPUNIT_ASSERT_EQUAL( TEST_WIDTH+3, static_cast<long>(line.GetDistance()));
    CPPUNIT_ASSERT_EQUAL( long( (3 * TEST_WIDTH) + 6 ), line.GetWidth() );
}

void BorderLineTest::testGuessWidthThinthickSmallgap()
{
    SvxBorderLine line;
    line.GuessLinesWidths( DOUBLE,
            THINTHICKSG_OUT_WIDTH,
            THINTHICKSG_IN_WIDTH,
            THINTHICKSG_DIST_WIDTH );
    CPPUNIT_ASSERT_EQUAL( THINTHICK_SMALLGAP, line.GetBorderLineStyle() );
    CPPUNIT_ASSERT_EQUAL( THINTHICKSG_OUT_WIDTH,
            static_cast<long>(line.GetOutWidth()) );
    CPPUNIT_ASSERT_EQUAL( THINTHICKSG_IN_WIDTH,
            static_cast<long>(line.GetInWidth()) );
    CPPUNIT_ASSERT_EQUAL( THINTHICKSG_DIST_WIDTH,
            static_cast<long>(line.GetDistance()) );
    CPPUNIT_ASSERT_EQUAL( THINTHICKSG_OUT_WIDTH + THINTHICKSG_IN_WIDTH
            + THINTHICKSG_DIST_WIDTH, line.GetWidth() );
}

void BorderLineTest::testGuessWidthThinthickLargegap()
{
    SvxBorderLine line;
    line.GuessLinesWidths( DOUBLE,
            THINTHICKLG_OUT_WIDTH,
            THINTHICKLG_IN_WIDTH,
            THINTHICKLG_DIST_WIDTH );
    CPPUNIT_ASSERT_EQUAL( THINTHICK_LARGEGAP, line.GetBorderLineStyle() );
    CPPUNIT_ASSERT_EQUAL( THINTHICKLG_OUT_WIDTH,
            static_cast<long>(line.GetOutWidth()) );
    CPPUNIT_ASSERT_EQUAL( THINTHICKLG_IN_WIDTH,
            static_cast<long>(line.GetInWidth()) );
    CPPUNIT_ASSERT_EQUAL( THINTHICKLG_DIST_WIDTH,
            static_cast<long>(line.GetDistance()) );
    CPPUNIT_ASSERT_EQUAL( THINTHICKLG_OUT_WIDTH + THINTHICKLG_IN_WIDTH
            + THINTHICKLG_DIST_WIDTH, line.GetWidth() );
}

void BorderLineTest::testGuessWidthNostyleDouble()
{
    SvxBorderLine line;
    line.GuessLinesWidths( NONE,
            THINTHICKLG_OUT_WIDTH,
            THINTHICKLG_IN_WIDTH,
            THINTHICKLG_DIST_WIDTH );
    CPPUNIT_ASSERT_EQUAL( THINTHICK_LARGEGAP, line.GetBorderLineStyle() );
    CPPUNIT_ASSERT_EQUAL( THINTHICKLG_OUT_WIDTH,
            static_cast<long>(line.GetOutWidth()) );
    CPPUNIT_ASSERT_EQUAL( THINTHICKLG_IN_WIDTH,
            static_cast<long>(line.GetInWidth()) );
    CPPUNIT_ASSERT_EQUAL( THINTHICKLG_DIST_WIDTH,
            static_cast<long>(line.GetDistance()) );
    CPPUNIT_ASSERT_EQUAL( THINTHICKLG_OUT_WIDTH + THINTHICKLG_IN_WIDTH
            + THINTHICKLG_DIST_WIDTH, line.GetWidth() );
}

void BorderLineTest::testGuessWidthNostyleSingle()
{
    SvxBorderLine line;
    line.GuessLinesWidths( NONE, TEST_WIDTH );
    CPPUNIT_ASSERT_EQUAL( SOLID, line.GetBorderLineStyle() );
    CPPUNIT_ASSERT_EQUAL( TEST_WIDTH, line.GetWidth() );
}

}

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
