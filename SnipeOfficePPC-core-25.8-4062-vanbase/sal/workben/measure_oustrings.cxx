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
 * Copyright (C) 2010 Red Hat, Inc., Caolán McNamara <caolanm@redhat.com>
 *  (initial developer)
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

#include <sal/main.h>
#include <rtl/ustring.hxx>
#include <osl/time.h>
#include <osl/process.h>

#include <iostream>
#include <sstream>
#include <fstream>

#if defined HAVE_VALGRIND_HEADERS
#    include <valgrind/callgrind.h>
int COUNT = 1;
#else
#    define CALLGRIND_DUMP_STATS_AT
#    define CALLGRIND_START_INSTRUMENTATION
#    define CALLGRIND_STOP_INSTRUMENTATION
#    define CALLGRIND_ZERO_STATS
int COUNT = 10000000;
#endif

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#   define HAVE_CXX_Ox 1
#endif

#ifdef HAVE_CXX_Ox
#  define SAL_DECLARE_UTF16(str) u ## str
#elif (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x550)
#  define SAL_DECLARE_UTF16(str) U ## str
#elif __SIZEOF_WCHAR_T__ == 2
#  define SAL_DECLARE_UTF16(str) L ## str
#endif

#ifdef SAL_DECLARE_UTF16
#  define RTL_CONSTASCII_USTRINGPARAM_WIDE(str) \
     reinterpret_cast<const sal_Unicode*>(SAL_DECLARE_UTF16(str)), (SAL_N_ELEMENTS(str)-1)
#endif

#define RTL_CONSTASCII_USTRINGPARAM_CLASSIC(str) \
    str, ((sal_Int32)(SAL_N_ELEMENTS(str)-1)), RTL_TEXTENCODING_ASCII_US

int currenttest = 1;
oslProcessInfo pidinfo;

class TimerMeasure
{
private:
    const char *m_pMessage;
    sal_uInt32 m_nStartTime, m_nEndTime;
public:
    TimerMeasure(const char *pMessage)
    : m_pMessage(pMessage)
    {
        m_nStartTime = osl_getGlobalTimer();
        CALLGRIND_START_INSTRUMENTATION
        CALLGRIND_ZERO_STATS
    }

    ~TimerMeasure()
    {
        CALLGRIND_STOP_INSTRUMENTATION
        CALLGRIND_DUMP_STATS_AT(m_pMessage);
        m_nEndTime = osl_getGlobalTimer();
        std::cout << m_pMessage << std::endl;
        std::cout << "    callgrind Instruction cost is: " << std::flush;

        std::stringstream aFileName;
        aFileName << "callgrind.out." << pidinfo.Ident << "." << currenttest;

        std::ifstream myfile(aFileName.str(), std::ios::in);
        if (myfile.is_open())
        {
            std::stringstream aGetGrindStats;
            aGetGrindStats << "callgrind_annotate " << aFileName.str() <<
                " | grep TOTALS | sed 's/ PROGRAM TOTALS//'";
            system(aGetGrindStats.str().c_str());
            myfile.close();
        }
        else
            std::cout << "Unavailable" << std::endl;
        currenttest++;
#ifndef HAVE_CALLGRIND
        std::cout << "    Elapsed Time is: " << m_nEndTime - m_nStartTime << "ms" << std::endl;
#endif
    }
};

#define TIME(msg, test) \
{\
    { test } /*Run the test one to shake out any firsttime lazy loading stuff*/ \
    TimerMeasure aMeasure(msg);\
    for (int i = 0; i < COUNT; ++i)\
        test\
}

SAL_IMPLEMENT_MAIN()
{
    CALLGRIND_STOP_INSTRUMENTATION
    CALLGRIND_ZERO_STATS

    pidinfo.Size = sizeof(pidinfo);
    osl_getProcessInfo(0, osl_Process_IDENTIFIER, &pidinfo);

#ifdef HAVE_CALLGRIND
    std::cout << "Execute using: valgrind --tool=callgrind ./measure_oustrings" << std::endl;
#else
    //get my cpu fan up to speed :-)
    for (int i = 0; i < 10000000; ++i)
    {
        rtl::OUString sFoo(rtl::OUString::createFromAscii("X"));
        rtl::OUString sBar(RTL_CONSTASCII_USTRINGPARAM_CLASSIC("X"));
        rtl::OUString sBaz(static_cast<sal_Unicode>('X'));
        rtl::OUString sNone;
    }
#endif

    std::cout << "--Empty Strings Construction--" << std::endl;

    TIME
    (
        "rtl::OUString()",
        rtl::OUString sFoo;
    )

    TIME
    (
        "rtl::OUString::createFromAscii()",
        rtl::OUString sFoo(rtl::OUString::createFromAscii(""));
    )

    std::cout << "--Single Chars Construction--" << std::endl;

    TIME
    (
        "rtl::OUString(static_cast<sal_Unicode>('X')",
        rtl::OUString sBaz(static_cast<sal_Unicode>('X'));
    )

#ifdef SAL_DECLARE_UTF16
    TIME
    (
        "rtl::OUString(RTL_CONSTASCII_USTRINGPARAM_WIDE(\"X\"))",
        rtl::OUString sBar(RTL_CONSTASCII_USTRINGPARAM_WIDE("X"));
    )
#endif

    TIME
    (
        "rtl::OUString(RTL_CONSTASCII_USTRINGPARAM_CLASSIC(\"X\"))",
        rtl::OUString sBar(RTL_CONSTASCII_USTRINGPARAM_CLASSIC("X"));
    )

    TIME
    (
        "rtl::OUString::createFromAscii(\"X\")",
        rtl::OUString sFoo(rtl::OUString::createFromAscii("X"));
    )

    std::cout << "--MultiChar Strings Construction--" << std::endl;

#ifdef SAL_DECLARE_UTF16
    TIME
    (
        "rtl::OUString(RTL_CONSTASCII_USTRINGPARAM_WIDE(\"XXXXXXXX\"))",
        rtl::OUString sBar(RTL_CONSTASCII_USTRINGPARAM_WIDE("XXXXXXXXXXXXXXX"));
    )
#endif

    TIME
    (
        "rtl::OUString(RTL_CONSTASCII_USTRINGPARAM_CLASSIC(\"XXXXXXXX\"))",
        rtl::OUString sBar(RTL_CONSTASCII_USTRINGPARAM_CLASSIC("XXXXXXXXXXXXXXX"));
    )

    TIME
    (
        "rtl::OUString::createFromAscii(\"XXXXXXXX\")",
        rtl::OUString sFoo(rtl::OUString::createFromAscii("XXXXXXXXXXXXXXX"));
    )

    std::cout << "--Ascii Unequal Comparison--" << std::endl;

    rtl::OUString sCompare(RTL_CONSTASCII_USTRINGPARAM_CLASSIC("XXXXXXXXXXXXXXX"));

    TIME
    (
        "rtl::OUString::equalsAsciiL",
        sCompare.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("apple"));
    )

    rtl::OUString sUnequalCompareTo(RTL_CONSTASCII_USTRINGPARAM_CLASSIC("apple"));

    TIME
    (
        "operator==(precreated OUString(\"apple\"))",
        sCompare == sUnequalCompareTo;
    )

    TIME
    (
        "rtl::OUString::equalsAscii",
        sCompare.equalsAscii("apple");
    )

    //(const sal_Char*, sal_Int32) version has different semantics
    TIME
    (
        "rtl::OUString::compareToAscii (const sal_Char*)",
        sCompare.compareToAscii("apple");
    )

    TIME
    (
        "operator==(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM_CLASSIC(\"apple\")))",
        sCompare == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM_CLASSIC("apple"));
    )

    std::cout << "--Ascii Equal Comparison--" << std::endl;

    TIME
    (
        "rtl::OUString::equalsAsciiL",
        sCompare.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("XXXXXXXXXXXXXXX"));
    )

    rtl::OUString sEqualCompareTo(RTL_CONSTASCII_USTRINGPARAM_CLASSIC("XXXXXXXXXXXXXXX"));

    TIME
    (
        "operator==(precreated OUString(\"XXXXXXXXXXXXXXX\"))",
        sCompare == sEqualCompareTo;
    )

    TIME
    (
        "rtl::OUString::equalsAscii",
        sCompare.equalsAscii("XXXXXXXXXXXXXXX");
    )

    //(const sal_Char*, sal_Int32) version has different semantics
    TIME
    (
        "rtl::OUString::compareToAscii (const sal_Char*)",
        sCompare.compareToAscii("XXXXXXXXXXXXXXX");
    )

    TIME
    (
        "operator==(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM_CLASSIC(\"XXXXXXXXXXXXXXX\"))",
        sCompare == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM_CLASSIC("XXXXXXXXXXXXXXX"));
    )

    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
