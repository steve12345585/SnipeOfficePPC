/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */
#ifndef INCLUDED_JVMFWK_FWKUTIL_HXX
#define INCLUDED_JVMFWK_FWKUTIL_HXX

#include "sal/config.h"
#include "osl/mutex.hxx"
#include "rtl/bootstrap.hxx"
#include "rtl/instance.hxx"
#include "rtl/ustrbuf.hxx"
#include "rtl/byteseq.hxx"
#include "osl/thread.hxx"
#if OSL_DEBUG_LEVEL >=2
#include <stdio.h>
#endif


namespace jfw
{

/** Returns the file URL of the directory where the framework library
    (this library) resides.
*/
rtl::OUString getLibraryLocation();

/** provides a bootstrap class which already knows the values from the
    jvmfkwrc file.
*/
struct Bootstrap :
    public ::rtl::StaticWithInit< const rtl::Bootstrap *, Bootstrap > {
        const rtl::Bootstrap * operator () () {
            ::rtl::OUStringBuffer buf(256);
            buf.append(getLibraryLocation());
            buf.appendAscii(SAL_CONFIGFILE("/jvmfwk3"));
            ::rtl::OUString sIni = buf.makeStringAndClear();
            ::rtl::Bootstrap *  bootstrap = new ::rtl::Bootstrap(sIni);
#if OSL_DEBUG_LEVEL >=2
        rtl::OString o = rtl::OUStringToOString( sIni , osl_getThreadTextEncoding() );
        fprintf(stderr, "[Java framework] Using configuration file %s\n" , o.getStr() );
#endif
        return bootstrap;
    }
};

struct FwkMutex: public ::rtl::Static<osl::Mutex, FwkMutex> {};

rtl::ByteSequence encodeBase16(const rtl::ByteSequence& rawData);
rtl::ByteSequence decodeBase16(const rtl::ByteSequence& data);

rtl::OUString getPlatform();


rtl::OUString getDirFromFile(const rtl::OUString& usFilePath);

/** Returns the file URL of the folder where the executable resides.
 */
rtl::OUString getExecutableDirectory();
/** Locates the plugin library and returns the file URL.

    First tries to locate plugin relative to baseUrl (if relative);
    vnd.sun.star.expand URLs are supported.  If that fails, tries to
    locate plugin relative to the executable.  If that fails, and plugin
    contains no slashes, tries to locate plugin in a platform-specific way
    (e.g., LD_LIBRARY_PATH).

    @param baseUrl
    The base file URL relative to which the plugin argument is interpreted.

    @param plugin
    The argument is an absolute or relative URL or just the name of the plugin.
 */
rtl::OUString findPlugin(
    const rtl::OUString & baseUrl, const rtl::OUString & plugin);


enum FileStatus
{
    FILE_OK,
    FILE_DOES_NOT_EXIST,
    FILE_INVALID
};

/** checks if the URL is a file.

    If it is a link to a file than
    it is resolved. Assuming that the argument
    represents a relative URL then FILE_INVALID
    is returned.


    @return
    one of the values of FileStatus.

    @exception
    Errors occurred during determining if the file exists
 */
FileStatus checkFileURL(const rtl::OUString & path);

bool isAccessibilitySupportDesired();

rtl::OUString buildClassPathFromDirectory(const rtl::OUString & relPath);

rtl::OUString retrieveClassPath( ::rtl::OUString const & macro );
}
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
