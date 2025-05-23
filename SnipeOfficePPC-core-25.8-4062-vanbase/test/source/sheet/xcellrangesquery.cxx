/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * [ Copyright (C) 2011 Markus Mohrhard <markus.mohrhard@googlemail.com> (initial developer) ]
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#include <test/sheet/xcellrangesquery.hxx>

#include <com/sun/star/sheet/XCellRangesQuery.hpp>
#include <com/sun/star/sheet/XSheetCellRanges.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/sheet/CellFlags.hpp>

#include "cppunit/extensions/HelperMacros.h"

using namespace com::sun::star;
using namespace com::sun::star::uno;


namespace apitest {

void XCellRangesQuery::testQueryColumnDifference()
{
    OUString aExpected( "Sheet1.B1:C1,Sheet1.B3:C5" );
    uno::Reference<sheet::XCellRangesQuery> xCellRangesQuery(init(),UNO_QUERY_THROW);
    uno::Reference<sheet::XSheetCellRanges> xRanges = xCellRangesQuery->queryColumnDifferences(table::CellAddress(0, 1, 1));
    OUString aResult = xRanges->getRangeAddressesAsString();
    std::cout << "testQueryColumnDifference: Result: " << OUStringToOString(aResult, RTL_TEXTENCODING_UTF8).getStr() << std::endl;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("testQueryColumnDifference", aResult, aExpected);
}

void XCellRangesQuery::testQueryContentDifference()
{
    OUString aExpected( "Sheet1.B2:B3" );
    uno::Reference<sheet::XCellRangesQuery> xCellRangesQuery(init(),UNO_QUERY_THROW);
    uno::Reference<sheet::XSheetCellRanges> xRanges = xCellRangesQuery->queryContentCells(sheet::CellFlags::VALUE);
    OUString aResult = xRanges->getRangeAddressesAsString();
    std::cout << "testQueryContentDifference: Result: " << OUStringToOString(aResult, RTL_TEXTENCODING_UTF8).getStr() << std::endl;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("testQueryContentDifference", aResult, aExpected);
}

void XCellRangesQuery::testQueryEmptyCells()
{
    OUString aExpected( "Sheet1.A1:A5,Sheet1.B1:C1,Sheet1.B5,Sheet1.C3:C5,Sheet1.D1:D5" );
    uno::Reference<sheet::XCellRangesQuery> xCellRangesQuery(init(),UNO_QUERY_THROW);
    uno::Reference<sheet::XSheetCellRanges> xRanges = xCellRangesQuery->queryEmptyCells();
    OUString aResult = xRanges->getRangeAddressesAsString();
    std::cout << "testQueryEmptyCells: Result: " << OUStringToOString(aResult, RTL_TEXTENCODING_UTF8).getStr() << std::endl;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("testQueryEmptyCells", aResult, aExpected);
}

void XCellRangesQuery::testQueryFormulaCells()
{
    OUString aExpected( "Sheet1.C2" );
    uno::Reference<sheet::XCellRangesQuery> xCellRangesQuery(init(),UNO_QUERY_THROW);
    uno::Reference<sheet::XSheetCellRanges> xRanges = xCellRangesQuery->queryFormulaCells(sheet::CellFlags::FORMULA);
    OUString aResult = xRanges->getRangeAddressesAsString();
    std::cout << "testQueryFormulaCells: Result: " << OUStringToOString(aResult, RTL_TEXTENCODING_UTF8).getStr() << std::endl;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("testQueryFormulaCells", aResult, aExpected);
}

void XCellRangesQuery::testQueryIntersection()
{
    OUString aExpected( "Sheet1.D4:D5" );
    uno::Reference<sheet::XCellRangesQuery> xCellRangesQuery(init(),UNO_QUERY_THROW);
    uno::Reference<sheet::XSheetCellRanges> xRanges = xCellRangesQuery->queryIntersection(table::CellRangeAddress(0,3,3,7,7));
    OUString aResult = xRanges->getRangeAddressesAsString();
    std::cout << "testQueryIntersection: Result: " << OUStringToOString(aResult, RTL_TEXTENCODING_UTF8).getStr() << std::endl;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("testQueryIntersection", aResult, aExpected);
}

void XCellRangesQuery::testQueryRowDifference()
{
    OUString aExpected( "Sheet1.A2:A4,Sheet1.C2:D4" );
    uno::Reference<sheet::XCellRangesQuery> xCellRangesQuery(init(),UNO_QUERY_THROW);
    uno::Reference<sheet::XSheetCellRanges> xRanges = xCellRangesQuery->queryRowDifferences(table::CellAddress(0,1,1));
    OUString aResult = xRanges->getRangeAddressesAsString();
    std::cout << "testQueryRowDifference: Result: " << OUStringToOString(aResult, RTL_TEXTENCODING_UTF8).getStr() << std::endl;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("testQueryRowDifference", aResult, aExpected);
}

void XCellRangesQuery::testQueryVisibleCells()
{
    OUString aExpected( "Sheet1.A1:D5" );
    uno::Reference<sheet::XCellRangesQuery> xCellRangesQuery(init(),UNO_QUERY_THROW);
    uno::Reference<sheet::XSheetCellRanges> xRanges = xCellRangesQuery->queryVisibleCells();
    OUString aResult = xRanges->getRangeAddressesAsString();
    std::cout << "testQueryVisibleCells: Result: " << OUStringToOString(aResult, RTL_TEXTENCODING_UTF8).getStr() << std::endl;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("testQueryVisibleCells", aResult, aExpected);
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
