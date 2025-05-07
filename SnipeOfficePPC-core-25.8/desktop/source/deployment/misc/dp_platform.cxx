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


#include "dp_misc.h"
#include "dp_platform.hxx"
#include "rtl/ustring.hxx"
#include "rtl/ustrbuf.hxx"
#include "rtl/instance.hxx"
#include "rtl/bootstrap.hxx"

#define PLATFORM_ALL                "all"
#define PLATFORM_WIN_X86            "windows_x86"
#define PLATFORM_WIN_X86_64         "windows_x86_64"
#define PLATFORM_LINUX_X86          "linux_x86"
#define PLATFORM_LINUX_X86_64       "linux_x86_64"
#define PLATFORM_KFREEBSD_X86       "kfreebsd_x86"
#define PLATFORM_KFREEBSD_X86_64    "kfreebsd_x86_64"
#define PLATFORM_LINUX_SPARC        "linux_sparc"
#define PLATFORM_LINUX_POWERPC      "linux_powerpc"
#define PLATFORM_LINUX_POWERPC64    "linux_powerpc64"
#define PLATFORM_LINUX_ARM_EABI     "linux_arm_eabi"
#define PLATFORM_LINUX_ARM_OABI     "linux_arm_oabi"
#define PLATFORM_LINUX_MIPS_EL      "linux_mips_el"
#define PLATFORM_LINUX_MIPS_EB      "linux_mips_eb"
#define PLATFORM_LINUX_IA64         "linux_ia64"
#define PLATFORM_LINUX_M68K         "linux_m68k"
#define PLATFORM_LINUX_S390         "linux_s390"
#define PLATFORM_LINUX_S390x        "linux_s390x"
#define PLATFORM_LINUX_HPPA         "linux_hppa"
#define PLATFORM_LINUX_ALPHA        "linux_alpha"



#define PLATFORM_SOLARIS_SPARC      "solaris_sparc"
#define PLATFORM_SOLARIS_SPARC64    "solaris_sparc64"
#define PLATFORM_SOLARIS_X86        "solaris_x86"
#define PLATFORM_FREEBSD_X86        "freebsd_x86"
#define PLATFORM_FREEBSD_X86_64     "freebsd_x86_64"
#define PLATFORM_NETBSD_X86         "netbsd_x86"
#define PLATFORM_NETBSD_X86_64      "netbsd_x86_64"
#define PLATFORM_MACOSX_X86         "macosx_x86"
#define PLATFORM_MACOSX_PPC         "macosx_powerpc"
#define PLATFORM_OPENBSD_X86        "openbsd_x86"
#define PLATFORM_OPENBSD_X86_64     "openbsd_x86_64"
#define PLATFORM_DRAGONFLY_X86      "dragonfly_x86"
#define PLATFORM_DRAGONFLY_X86_64   "dragonfly_x86_64"


#define PLATFORM_AIX_POWERPC        "aix_powerpc"







#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )
using ::rtl::OUString;
namespace css = ::com::sun::star;

namespace dp_misc
{
namespace
{
    struct StrOperatingSystem :
        public rtl::StaticWithInit<OUString, StrOperatingSystem> {
            const OUString operator () () {
                OUString os( RTL_CONSTASCII_USTRINGPARAM("$_OS") );
                ::rtl::Bootstrap::expandMacros( os );
                return os;
            }
    };

    struct StrCPU :
        public rtl::StaticWithInit<OUString, StrCPU> {
            const OUString operator () () {
                OUString arch( RTL_CONSTASCII_USTRINGPARAM("$_ARCH") );
                ::rtl::Bootstrap::expandMacros( arch );
                return arch;
            }
    };


    struct StrPlatform : public rtl::StaticWithInit<
        OUString, StrPlatform> {
            const OUString operator () () {
                ::rtl::OUStringBuffer buf;
                buf.append( StrOperatingSystem::get() );
                buf.append( static_cast<sal_Unicode>('_') );
                buf.append( StrCPU::get() );
                return buf.makeStringAndClear();
            }
    };

    bool checkOSandCPU(OUString const & os, OUString const & cpu)
    {
        return os.equals(StrOperatingSystem::get())
            && cpu.equals(StrCPU::get());
    }

    bool isValidPlatform(OUString const &  token )
    {
        bool ret = false;
        if (token.equals(OUSTR(PLATFORM_ALL)))
            ret = true;
        else if (token.equals(OUSTR(PLATFORM_WIN_X86)))
            ret = checkOSandCPU(OUSTR("Windows"), OUSTR("x86"));
        else if (token.equals(OUSTR(PLATFORM_WIN_X86_64)))
            ret = checkOSandCPU(OUSTR("Windows"), OUSTR("x86_64"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_X86)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("x86"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_X86_64)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("X86_64"));
        else if (token.equals(OUSTR(PLATFORM_KFREEBSD_X86)))
            ret = checkOSandCPU(OUSTR("kFreeBSD"), OUSTR("x86"));
        else if (token.equals(OUSTR(PLATFORM_KFREEBSD_X86_64)))
            ret = checkOSandCPU(OUSTR("kFreeBSD"), OUSTR("X86_64"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_SPARC)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("SPARC"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_POWERPC)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("PowerPC"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_POWERPC64)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("PowerPC_64"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_ARM_EABI)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("ARM_EABI"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_ARM_OABI)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("ARM_OABI"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_MIPS_EL)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("MIPS_EL"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_MIPS_EB)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("MIPS_EB"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_IA64)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("IA64"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_M68K)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("M68K"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_S390)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("S390"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_S390x)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("S390x"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_HPPA)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("HPPA"));
        else if (token.equals(OUSTR(PLATFORM_LINUX_ALPHA)))
            ret = checkOSandCPU(OUSTR("Linux"), OUSTR("ALPHA"));
        else if (token.equals(OUSTR(PLATFORM_SOLARIS_SPARC)))
            ret = checkOSandCPU(OUSTR("Solaris"), OUSTR("SPARC"));
        else if (token.equals(OUSTR(PLATFORM_SOLARIS_SPARC64)))
            ret = checkOSandCPU(OUSTR("Solaris"), OUSTR("SPARC64"));
        else if (token.equals(OUSTR(PLATFORM_SOLARIS_X86)))
            ret = checkOSandCPU(OUSTR("Solaris"), OUSTR("x86"));
        else if (token.equals(OUSTR(PLATFORM_FREEBSD_X86)))
            ret = checkOSandCPU(OUSTR("FreeBSD"), OUSTR("x86"));
        else if (token.equals(OUSTR(PLATFORM_FREEBSD_X86_64)))
            ret = checkOSandCPU(OUSTR("FreeBSD"), OUSTR("X86_64"));
        else if (token.equals(OUSTR(PLATFORM_NETBSD_X86)))
            ret = checkOSandCPU(OUSTR("NetBSD"), OUSTR("x86"));
        else if (token.equals(OUSTR(PLATFORM_NETBSD_X86_64)))
            ret = checkOSandCPU(OUSTR("NetBSD"), OUSTR("X86_64"));
        else if (token.equals(OUSTR(PLATFORM_MACOSX_X86)))
            ret = checkOSandCPU(OUSTR("MacOSX"), OUSTR("x86"));
        else if (token.equals(OUSTR(PLATFORM_MACOSX_PPC)))
            ret = checkOSandCPU(OUSTR("MacOSX"), OUSTR("PowerPC"));
        else if (token.equals(OUSTR(PLATFORM_AIX_POWERPC)))
            ret = checkOSandCPU(OUSTR("AIX"), OUSTR("PowerPC"));
        else if (token.equals(OUSTR(PLATFORM_OPENBSD_X86)))
            ret = checkOSandCPU(OUSTR("OpenBSD"), OUSTR("x86"));
        else if (token.equals(OUSTR(PLATFORM_OPENBSD_X86_64)))
            ret = checkOSandCPU(OUSTR("OpenBSD"), OUSTR("X86_64"));
        else if (token.equals(OUSTR(PLATFORM_DRAGONFLY_X86)))
            ret = checkOSandCPU(OUSTR("DragonFly"), OUSTR("x86"));
        else if (token.equals(OUSTR(PLATFORM_DRAGONFLY_X86_64)))
            ret = checkOSandCPU(OUSTR("DragonFly"), OUSTR("X86_64"));
        else
        {
            OSL_FAIL("Extension Manager: The extension supports an unknown platform. "
            "Check the platform element in the description.xml");
            ret = false;
        }
        return ret;
    }

} // anon namespace
//=============================================================================

OUString const & getPlatformString()
{
    return StrPlatform::get();
}

bool platform_fits( OUString const & platform_string )
{
    sal_Int32 index = 0;
    for (;;)
    {
        const OUString token(
            platform_string.getToken( 0, ',', index ).trim() );
        // check if this platform:
        if (token.equalsIgnoreAsciiCase( StrPlatform::get() ) ||
            (token.indexOf( '_' ) < 0 && /* check OS part only */
             token.equalsIgnoreAsciiCase( StrOperatingSystem::get() )))
        {
            return true;
        }
        if (index < 0)
            break;
    }
    return false;
}

bool hasValidPlatform( css::uno::Sequence<OUString> const & platformStrings)
{
    bool ret = false;
    for (sal_Int32 i  = 0; i < platformStrings.getLength(); i++)
    {
        if (isValidPlatform(platformStrings[i]))
        {
            ret = true;
            break;
        }
    }
    return ret;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
