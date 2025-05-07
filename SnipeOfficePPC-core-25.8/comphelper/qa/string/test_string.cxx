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

#include "sal/precppunit.hxx"

#include <comphelper/string.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/i18n/CharType.hpp>

#include "cppunit/TestAssert.h"
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/plugin/TestPlugIn.h"
#include "rtl/string.hxx"
#include "rtl/ustring.hxx"

namespace {

class TestString: public CppUnit::TestFixture
{
public:
    void testNatural();
    void testRemove();
    void testStripStart();
    void testStripEnd();
    void testStrip();
    void testToken();
    void testTokenCount();
    void testDecimalStringToNumber();
    void testIsdigitAsciiString();
    void testIndexOfL();

    CPPUNIT_TEST_SUITE(TestString);
    CPPUNIT_TEST(testNatural);
    CPPUNIT_TEST(testRemove);
    CPPUNIT_TEST(testStripStart);
    CPPUNIT_TEST(testStripEnd);
    CPPUNIT_TEST(testStrip);
    CPPUNIT_TEST(testToken);
    CPPUNIT_TEST(testTokenCount);
    CPPUNIT_TEST(testDecimalStringToNumber);
    CPPUNIT_TEST(testIsdigitAsciiString);
    CPPUNIT_TEST_SUITE_END();
};

void TestString::testDecimalStringToNumber()
{
    rtl::OUString s1(RTL_CONSTASCII_USTRINGPARAM("1234"));
    CPPUNIT_ASSERT_EQUAL((sal_uInt32)1234, comphelper::string::decimalStringToNumber(s1));
    s1 += rtl::OUString(static_cast<sal_Unicode>(0x07C6));
    CPPUNIT_ASSERT_EQUAL((sal_uInt32)12346, comphelper::string::decimalStringToNumber(s1));
    // Codepoints on 2 16bits words
    sal_uInt32 utf16String[] = { 0x1D7FE /* 8 */, 0x1D7F7 /* 1 */};
    s1 = rtl::OUString(utf16String, 2);
    CPPUNIT_ASSERT_EQUAL((sal_uInt32)81, comphelper::string::decimalStringToNumber(s1));
}

void TestString::testIsdigitAsciiString()
{
    rtl::OString s1(RTL_CONSTASCII_STRINGPARAM("1234"));
    CPPUNIT_ASSERT_EQUAL(comphelper::string::isdigitAsciiString(s1), true);

    rtl::OString s2(RTL_CONSTASCII_STRINGPARAM("1A34"));
    CPPUNIT_ASSERT_EQUAL(comphelper::string::isdigitAsciiString(s2), false);

    rtl::OString s3;
    CPPUNIT_ASSERT_EQUAL(comphelper::string::isdigitAsciiString(s3), true);
}

using namespace ::com::sun::star;

class testCollator : public cppu::WeakImplHelper1< i18n::XCollator >
{
public:
    virtual sal_Int32 SAL_CALL compareSubstring(
        const rtl::OUString& str1, sal_Int32 off1, sal_Int32 len1,
        const rtl::OUString& str2, sal_Int32 off2, sal_Int32 len2) throw(uno::RuntimeException)
    {
        return str1.copy(off1, len1).compareTo(str2.copy(off2, len2));
    }
    virtual sal_Int32 SAL_CALL compareString(
        const rtl::OUString& str1,
        const rtl::OUString& str2) throw(uno::RuntimeException)
    {
        return str1.compareTo(str2);
    }
    virtual sal_Int32 SAL_CALL loadDefaultCollator(const lang::Locale&, sal_Int32)
        throw(uno::RuntimeException) {return 0;}
    virtual sal_Int32 SAL_CALL loadCollatorAlgorithm(const rtl::OUString&,
        const lang::Locale&, sal_Int32) throw(uno::RuntimeException) {return 0;}
    virtual void SAL_CALL loadCollatorAlgorithmWithEndUserOption(const rtl::OUString&,
        const lang::Locale&, const uno::Sequence< sal_Int32 >&) throw(uno::RuntimeException) {}
    virtual uno::Sequence< rtl::OUString > SAL_CALL listCollatorAlgorithms(const lang::Locale&)
        throw(uno::RuntimeException)
    {
        return uno::Sequence< rtl::OUString >();
    }
    virtual uno::Sequence< sal_Int32 > SAL_CALL listCollatorOptions(const rtl::OUString&)
        throw(uno::RuntimeException)
    {
        return uno::Sequence< sal_Int32 >();
    }
};

#define IS_DIGIT(CHAR) (((CHAR) >= 48) && ((CHAR <= 57)))

class testBreakIterator : public cppu::WeakImplHelper1< i18n::XBreakIterator >
{
public:
    virtual sal_Int32 SAL_CALL nextCharacters( const rtl::OUString&, sal_Int32,
        const lang::Locale&, sal_Int16, sal_Int32, sal_Int32& )
            throw(uno::RuntimeException) {return -1;}
    virtual sal_Int32 SAL_CALL previousCharacters( const rtl::OUString&, sal_Int32,
        const lang::Locale&, sal_Int16, sal_Int32, sal_Int32& )
            throw(uno::RuntimeException) {return -1;}

    virtual i18n::Boundary SAL_CALL previousWord( const rtl::OUString&, sal_Int32,
        const lang::Locale&, sal_Int16) throw(uno::RuntimeException)
        { return i18n::Boundary(); }
    virtual i18n::Boundary SAL_CALL nextWord( const rtl::OUString&, sal_Int32,
        const lang::Locale&, sal_Int16) throw(uno::RuntimeException)
        { return i18n::Boundary(); }
    virtual i18n::Boundary SAL_CALL getWordBoundary( const rtl::OUString&, sal_Int32,
        const lang::Locale&, sal_Int16, sal_Bool )
        throw(uno::RuntimeException)
        { return i18n::Boundary(); }

    virtual sal_Bool SAL_CALL isBeginWord( const rtl::OUString&, sal_Int32,
        const lang::Locale&, sal_Int16 ) throw(uno::RuntimeException)
        { return false; }
    virtual sal_Bool SAL_CALL isEndWord( const rtl::OUString&, sal_Int32,
        const lang::Locale& , sal_Int16 ) throw(uno::RuntimeException)
        { return false; }
    virtual sal_Int16 SAL_CALL getWordType( const rtl::OUString&, sal_Int32,
        const lang::Locale& ) throw(uno::RuntimeException)
        { return 0; }

    virtual sal_Int32 SAL_CALL beginOfSentence( const rtl::OUString&, sal_Int32,
        const lang::Locale& ) throw(uno::RuntimeException)
        { return 0; }
    virtual sal_Int32 SAL_CALL endOfSentence( const rtl::OUString& rText, sal_Int32,
        const lang::Locale& ) throw(uno::RuntimeException)
        { return rText.getLength(); }

    virtual i18n::LineBreakResults SAL_CALL getLineBreak( const rtl::OUString&, sal_Int32,
        const lang::Locale&, sal_Int32,
        const i18n::LineBreakHyphenationOptions&,
        const i18n::LineBreakUserOptions&)
        throw(uno::RuntimeException)
    {
        return i18n::LineBreakResults();
    }

    virtual sal_Int16 SAL_CALL getScriptType( const rtl::OUString&, sal_Int32 )
        throw(uno::RuntimeException) { return -1; }
    virtual sal_Int32 SAL_CALL beginOfScript( const rtl::OUString&, sal_Int32,
        sal_Int16 ) throw(uno::RuntimeException) { return -1; }
    virtual sal_Int32 SAL_CALL endOfScript( const rtl::OUString&, sal_Int32,
        sal_Int16 ) throw(uno::RuntimeException) { return -1; }
    virtual sal_Int32 SAL_CALL previousScript( const rtl::OUString&, sal_Int32,
        sal_Int16 ) throw(uno::RuntimeException) { return -1; }
    virtual sal_Int32 SAL_CALL nextScript( const rtl::OUString&, sal_Int32,
        sal_Int16 ) throw(uno::RuntimeException) { return -1; }

    virtual sal_Int32 SAL_CALL beginOfCharBlock( const rtl::OUString&, sal_Int32,
        const lang::Locale&, sal_Int16 ) throw(uno::RuntimeException) { return -1; }
    virtual sal_Int32 SAL_CALL endOfCharBlock( const rtl::OUString& rText, sal_Int32 nStartPos,
        const lang::Locale&, sal_Int16 CharType ) throw(uno::RuntimeException)
    {
        const sal_Unicode *pStr = rText.getStr()+nStartPos;
        for (sal_Int32 nI = nStartPos; nI < rText.getLength(); ++nI)
        {
            if (CharType == i18n::CharType::DECIMAL_DIGIT_NUMBER && !IS_DIGIT(*pStr))
                return nI;
            else if (CharType != i18n::CharType::DECIMAL_DIGIT_NUMBER && IS_DIGIT(*pStr))
                return nI;
            ++pStr;
        }
        return -1;
    }
    virtual sal_Int32 SAL_CALL previousCharBlock( const rtl::OUString&, sal_Int32,
        const lang::Locale&, sal_Int16 ) throw(uno::RuntimeException) { return -1; }
    virtual sal_Int32 SAL_CALL nextCharBlock( const rtl::OUString& rText, sal_Int32 nStartPos,
        const lang::Locale&, sal_Int16 CharType ) throw(uno::RuntimeException)
    {
        const sal_Unicode *pStr = rText.getStr()+nStartPos;
        for (sal_Int32 nI = nStartPos; nI < rText.getLength(); ++nI)
        {
            if (CharType == i18n::CharType::DECIMAL_DIGIT_NUMBER && IS_DIGIT(*pStr))
                return nI;
            else if (CharType != i18n::CharType::DECIMAL_DIGIT_NUMBER && !IS_DIGIT(*pStr))
                return nI;
            ++pStr;
        }
        return -1;
    }
};

void TestString::testNatural()
{
    using namespace comphelper::string;

    uno::Reference< i18n::XCollator > xCollator(new testCollator);
    uno::Reference< i18n::XBreakIterator > xBI(new testBreakIterator);

// --- Some generic tests to ensure we do not alter original behavior
// outside what we want
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("ABC"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("ABC"))), xCollator, xBI, lang::Locale()) == 0
    );
    // Case sensitivity
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("ABC"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("abc"))), xCollator, xBI, lang::Locale()) < 0
    );
    // Reverse
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("abc"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("ABC"))), xCollator, xBI, lang::Locale()) > 0
    );
    // First shorter
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("alongstring"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("alongerstring"))), xCollator, xBI, lang::Locale()) > 0
    );
    // Second shorter
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("alongerstring"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("alongstring"))), xCollator, xBI, lang::Locale()) < 0
    );
// -- Here we go on natural order, each one is followed by classic compare and the reverse comparison
    // That's why we originally made the patch
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("Heading 9"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("Heading 10"))), xCollator, xBI, lang::Locale()) < 0
    );
    // Original behavior
    CPPUNIT_ASSERT(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("Heading 9"))).compareTo(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("Heading 10")))) > 0
    );
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("Heading 10"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("Heading 9"))), xCollator, xBI, lang::Locale()) > 0
    );
    // Harder
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("July, the 4th"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("July, the 10th"))), xCollator, xBI, lang::Locale()) < 0
    );
    CPPUNIT_ASSERT(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("July, the 4th"))).compareTo(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("July, the 10th")))) > 0
    );
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("July, the 10th"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("July, the 4th"))), xCollator, xBI, lang::Locale()) > 0
    );
    // Hardest
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("abc08"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("abc010"))), xCollator, xBI, lang::Locale()) < 0
    );
    CPPUNIT_ASSERT(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("abc08"))).compareTo(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("abc010")))) > 0
    );
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("abc010"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("abc08"))), xCollator, xBI, lang::Locale()) > 0
    );
    CPPUNIT_ASSERT(
        compareNatural(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("apple10apple"))), rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(("apple10apple"))), xCollator, xBI, lang::Locale()) == 0
    );
}

void TestString::testRemove()
{
    ::rtl::OString aIn(RTL_CONSTASCII_STRINGPARAM("abc"));
    ::rtl::OString aOut;

    aOut = ::comphelper::string::remove(aIn, 'b');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("ac")));

    aIn = rtl::OString(RTL_CONSTASCII_STRINGPARAM("aaa"));

    aOut = ::comphelper::string::remove(aIn, 'a');
    CPPUNIT_ASSERT(aOut.isEmpty());
}

void TestString::testStripStart()
{
    ::rtl::OString aIn(RTL_CONSTASCII_STRINGPARAM("abc"));
    ::rtl::OString aOut;

    aOut = ::comphelper::string::stripStart(aIn, 'b');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("abc")));

    aOut = ::comphelper::string::stripStart(aIn, 'a');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("bc")));

    aIn = rtl::OString(RTL_CONSTASCII_STRINGPARAM("aaa"));
    aOut = ::comphelper::string::stripStart(aIn, 'a');
    CPPUNIT_ASSERT(aOut.isEmpty());

    aIn = rtl::OString(RTL_CONSTASCII_STRINGPARAM("aba"));
    aOut = ::comphelper::string::stripStart(aIn, 'a');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("ba")));
}

void TestString::testStripEnd()
{
    ::rtl::OString aIn(RTL_CONSTASCII_STRINGPARAM("abc"));
    ::rtl::OString aOut;

    aOut = ::comphelper::string::stripEnd(aIn, 'b');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("abc")));

    aOut = ::comphelper::string::stripEnd(aIn, 'c');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("ab")));

    aIn = rtl::OString(RTL_CONSTASCII_STRINGPARAM("aaa"));
    aOut = ::comphelper::string::stripEnd(aIn, 'a');
    CPPUNIT_ASSERT(aOut.isEmpty());

    aIn = rtl::OString(RTL_CONSTASCII_STRINGPARAM("aba"));
    aOut = ::comphelper::string::stripEnd(aIn, 'a');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("ab")));
}

void TestString::testStrip()
{
    ::rtl::OString aIn(RTL_CONSTASCII_STRINGPARAM("abc"));
    ::rtl::OString aOut;

    aOut = ::comphelper::string::strip(aIn, 'b');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("abc")));

    aOut = ::comphelper::string::strip(aIn, 'c');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("ab")));

    aIn = rtl::OString(RTL_CONSTASCII_STRINGPARAM("aaa"));
    aOut = ::comphelper::string::strip(aIn, 'a');
    CPPUNIT_ASSERT(aOut.isEmpty());

    aIn = rtl::OString(RTL_CONSTASCII_STRINGPARAM("aba"));
    aOut = ::comphelper::string::strip(aIn, 'a');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("b")));
}

void TestString::testToken()
{
    ::rtl::OString aIn(RTL_CONSTASCII_STRINGPARAM("10.11.12"));
    ::rtl::OString aOut;

    aOut = ::comphelper::string::getToken(aIn, -1, '.');
    CPPUNIT_ASSERT(aOut.isEmpty());

    aOut = ::comphelper::string::getToken(aIn, 0, '.');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("10")));

    aOut = ::comphelper::string::getToken(aIn, 1, '.');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("11")));

    aOut = ::comphelper::string::getToken(aIn, 2, '.');
    CPPUNIT_ASSERT(aOut.equalsL(RTL_CONSTASCII_STRINGPARAM("12")));

    aOut = ::comphelper::string::getToken(aIn, 3, '.');
    CPPUNIT_ASSERT(aOut.isEmpty());
}

void TestString::testTokenCount()
{
    ::rtl::OString aIn(RTL_CONSTASCII_STRINGPARAM("10.11.12"));
    sal_Int32 nOut;

    nOut = ::comphelper::string::getTokenCount(aIn, '.');
    CPPUNIT_ASSERT(nOut == 3);

    nOut = ::comphelper::string::getTokenCount(aIn, 'X');
    CPPUNIT_ASSERT(nOut == 1);

    nOut = ::comphelper::string::getTokenCount(rtl::OString(), 'X');
    CPPUNIT_ASSERT(nOut == 0);
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestString);

}

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
