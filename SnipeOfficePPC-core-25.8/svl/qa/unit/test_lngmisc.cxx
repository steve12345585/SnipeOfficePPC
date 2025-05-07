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
 *  Copyright (C) 2011 August Sodora <augsod@gmail.com> (initial developer)
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

#include "sal/config.h"
#include "sal/precppunit.hxx"

#include "cppunit/TestAssert.h"
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/plugin/TestPlugIn.h"

#include "svl/lngmisc.hxx"

#include <rtl/ustrbuf.hxx>

namespace
{
  class LngMiscTest : public CppUnit::TestFixture
  {
  private:
    void testRemoveHyphens();
    void testRemoveControlChars();
    void testReplaceControlChars();
    void testGetThesaurusReplaceText();

    CPPUNIT_TEST_SUITE(LngMiscTest);

    CPPUNIT_TEST(testRemoveHyphens);
    CPPUNIT_TEST(testRemoveControlChars);
    CPPUNIT_TEST(testReplaceControlChars);
    CPPUNIT_TEST(testGetThesaurusReplaceText);

    CPPUNIT_TEST_SUITE_END();
  };

  void LngMiscTest::testRemoveHyphens()
  {
    ::rtl::OUString str1(RTL_CONSTASCII_USTRINGPARAM(""));
    ::rtl::OUString str2(RTL_CONSTASCII_USTRINGPARAM("a-b--c---"));

    ::rtl::OUStringBuffer str3Buf;
    str3Buf.append(SVT_SOFT_HYPHEN);
    str3Buf.append(SVT_HARD_HYPHEN);
    str3Buf.append(SVT_HARD_HYPHEN);
    ::rtl::OUString str3(str3Buf.makeStringAndClear());

    ::rtl::OUString str4(RTL_CONSTASCII_USTRINGPARAM("asdf"));

    bool bModified = linguistic::RemoveHyphens(str1);
    CPPUNIT_ASSERT(!bModified);
    CPPUNIT_ASSERT(str1.isEmpty());

    // Note that '-' isn't a hyphen to RemoveHyphens.
    bModified = linguistic::RemoveHyphens(str2);
    CPPUNIT_ASSERT(!bModified);
    CPPUNIT_ASSERT( str2 == "a-b--c---" );

    bModified = linguistic::RemoveHyphens(str3);
    CPPUNIT_ASSERT(bModified);
    CPPUNIT_ASSERT(str3.isEmpty());

    bModified = linguistic::RemoveHyphens(str4);
    CPPUNIT_ASSERT(!bModified);
    CPPUNIT_ASSERT( str4 == "asdf" );
  }

  void LngMiscTest::testRemoveControlChars()
  {
    ::rtl::OUString str1(RTL_CONSTASCII_USTRINGPARAM(""));
    ::rtl::OUString str2(RTL_CONSTASCII_USTRINGPARAM("asdf"));
    ::rtl::OUString str3(RTL_CONSTASCII_USTRINGPARAM("asdf\nasdf"));

    ::rtl::OUStringBuffer str4Buf(33);
    str4Buf.setLength(33);
    for(int i = 0; i < 33; i++)
      str4Buf[i] = static_cast<sal_Unicode>(i);
    //    TODO: is this a bug? shouldn't RemoveControlChars remove this?
    //    str4Buf[33] = static_cast<sal_Unicode>(0x7F);
    ::rtl::OUString str4(str4Buf.makeStringAndClear());

    bool bModified = linguistic::RemoveControlChars(str1);
    CPPUNIT_ASSERT(!bModified);
    CPPUNIT_ASSERT(str1.isEmpty());

    bModified = linguistic::RemoveControlChars(str2);
    CPPUNIT_ASSERT(!bModified);
    CPPUNIT_ASSERT( str2 == "asdf" );

    bModified = linguistic::RemoveControlChars(str3);
    CPPUNIT_ASSERT(bModified);
    CPPUNIT_ASSERT( str3 == "asdfasdf" );

    bModified = linguistic::RemoveControlChars(str4);
    CPPUNIT_ASSERT(bModified);
    CPPUNIT_ASSERT( str4 == " " );
  }

  void LngMiscTest::testReplaceControlChars()
  {
    ::rtl::OUString str1(RTL_CONSTASCII_USTRINGPARAM(""));
    ::rtl::OUString str2(RTL_CONSTASCII_USTRINGPARAM("asdf"));
    ::rtl::OUString str3(RTL_CONSTASCII_USTRINGPARAM("asdf\nasdf"));

    ::rtl::OUStringBuffer str4Buf(33);
    str4Buf.setLength(33);
    for(int i = 0; i < 33; i++)
      str4Buf[i] = static_cast<sal_Unicode>(i);
    //    TODO: is this a bug? shouldn't RemoveControlChars remove this?
    //    str4Buf[33] = static_cast<sal_Unicode>(0x7F);
    ::rtl::OUString str4(str4Buf.makeStringAndClear());

    bool bModified = linguistic::ReplaceControlChars(str1);
    CPPUNIT_ASSERT(!bModified);
    CPPUNIT_ASSERT(str1.isEmpty());

    bModified = linguistic::ReplaceControlChars(str2);
    CPPUNIT_ASSERT(!bModified);
    CPPUNIT_ASSERT( str2 == "asdf" );

    bModified = linguistic::ReplaceControlChars(str3);
    CPPUNIT_ASSERT(bModified);
    CPPUNIT_ASSERT( str3 == "asdf asdf" );

    bModified = linguistic::ReplaceControlChars(str4);
    CPPUNIT_ASSERT(bModified);
    CPPUNIT_ASSERT(str4.getLength() == 32);
    for(int i = 0; i < 32; i++)
      CPPUNIT_ASSERT(str4[i] == ' ');
  }

  void LngMiscTest::testGetThesaurusReplaceText()
  {
    const ::rtl::OUString str1(RTL_CONSTASCII_USTRINGPARAM(""));
    const ::rtl::OUString str2(RTL_CONSTASCII_USTRINGPARAM("asdf"));
    const ::rtl::OUString str3(RTL_CONSTASCII_USTRINGPARAM("asdf (abc)"));
    const ::rtl::OUString str4(RTL_CONSTASCII_USTRINGPARAM("asdf*"));
    const ::rtl::OUString str5(RTL_CONSTASCII_USTRINGPARAM("asdf * "));
    const ::rtl::OUString str6(RTL_CONSTASCII_USTRINGPARAM("asdf (abc) *"));
    const ::rtl::OUString str7(RTL_CONSTASCII_USTRINGPARAM("asdf asdf * (abc)"));
    const ::rtl::OUString str8(RTL_CONSTASCII_USTRINGPARAM(" * (abc) asdf *"));

    ::rtl::OUString r = linguistic::GetThesaurusReplaceText(str1);
    CPPUNIT_ASSERT(r.isEmpty());

    r = linguistic::GetThesaurusReplaceText(str2);
    CPPUNIT_ASSERT(r == str2);

    r = linguistic::GetThesaurusReplaceText(str3);
    CPPUNIT_ASSERT(r == str2);

    r = linguistic::GetThesaurusReplaceText(str4);
    CPPUNIT_ASSERT(r == str2);

    r = linguistic::GetThesaurusReplaceText(str5);
    CPPUNIT_ASSERT(r == str2);

    r = linguistic::GetThesaurusReplaceText(str6);
    CPPUNIT_ASSERT(r == str2);

    r = linguistic::GetThesaurusReplaceText(str7);
    CPPUNIT_ASSERT(r == ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("asdf asdf")));

    r = linguistic::GetThesaurusReplaceText(str8);
    CPPUNIT_ASSERT(r.isEmpty());
  }

  CPPUNIT_TEST_SUITE_REGISTRATION(LngMiscTest);
}
CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
