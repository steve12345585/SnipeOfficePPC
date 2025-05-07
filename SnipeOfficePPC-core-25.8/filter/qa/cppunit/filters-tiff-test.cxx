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
 *       Caolán McNamara <caolanm@redhat.com>
 * Portions created by the Initial Developer are Copyright (C) 2012 the
 * Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Caolán McNamara <caolanm@redhat.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#include <unotest/filters-test.hxx>
#include <test/bootstrapfixture.hxx>
#include <svtools/FilterConfigItem.hxx>
#include <tools/stream.hxx>
#include <vcl/graph.hxx>

#include <osl/file.hxx>
#include <osl/process.h>

extern "C"
{
    SAL_DLLPUBLIC_EXPORT sal_Bool __LOADONCALLAPI
        GraphicImport(SvStream & rStream, Graphic & rGraphic,
        FilterConfigItem*, sal_Bool);
}

using namespace ::com::sun::star;

/* Implementation of Filters test */

class TiffFilterTest
    : public test::FiltersTest
    , public test::BootstrapFixture
{
public:
    TiffFilterTest() : BootstrapFixture(true, false) {}

    virtual bool load(const rtl::OUString &, const rtl::OUString &rURL, const rtl::OUString &);

    /**
     * Ensure CVEs remain unbroken
     */
    void testCVEs();

    CPPUNIT_TEST_SUITE(TiffFilterTest);
    CPPUNIT_TEST(testCVEs);
    CPPUNIT_TEST_SUITE_END();
};

bool TiffFilterTest::load(const rtl::OUString &,
    const rtl::OUString &rURL, const rtl::OUString &)
{
    SvFileStream aFileStream(rURL, STREAM_READ);
    Graphic aGraphic;
    return GraphicImport(aFileStream, aGraphic, NULL, 0);
}

void TiffFilterTest::testCVEs()
{
    testDir(rtl::OUString(),
        getURLFromSrc("/filter/qa/cppunit/data/tiff/"),
        rtl::OUString());
}

CPPUNIT_TEST_SUITE_REGISTRATION(TiffFilterTest);

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
