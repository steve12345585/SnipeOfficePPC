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

#ifndef INCLUDED_DP_MISC_H
#define INCLUDED_DP_MISC_H

#include "rtl/ustrbuf.hxx"
#include "rtl/instance.hxx"
#include "osl/mutex.hxx"
#include "osl/process.h"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/lang/XComponent.hpp"
#include "com/sun/star/lang/DisposedException.hpp"
#include "com/sun/star/deployment/XPackageRegistry.hpp"
#include "com/sun/star/ucb/XCommandEnvironment.hpp"
#include "com/sun/star/awt/XWindow.hpp"
#include "dp_misc_api.hxx"

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )
#define ARLEN(x) (sizeof (x) / sizeof *(x))

namespace dp_misc {

const sal_Char CR = 0x0d;
const sal_Char LF = 0x0a;

//==============================================================================
class MutexHolder
{
    mutable ::osl::Mutex m_mutex;
protected:
    inline ::osl::Mutex & getMutex() const { return m_mutex; }
};

//==============================================================================
inline void try_dispose( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> const & x )
{
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent> xComp( x, ::com::sun::star::uno::UNO_QUERY );
    if (xComp.is())
        xComp->dispose();
}

//##############################################################################

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::rtl::OUString expandUnoRcTerm( ::rtl::OUString const & term );

DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::rtl::OUString makeRcTerm( ::rtl::OUString const & url );

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::rtl::OUString expandUnoRcUrl( ::rtl::OUString const & url );

//==============================================================================

/** appends a relative path to a url.

    The relative path must already be correctly encoded for use in an URL.
    If the URL starts with vnd.sun.star.expand then the relative path will
    be again encoded for use in an "expand" URL.
 */
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC ::rtl::OUString makeURL(
    ::rtl::OUString const & baseURL, ::rtl::OUString const & relPath );


/** appends a relative path to a url.

    This is the same as makeURL, but the relative Path must me a segment
    of an system path.
 */
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC ::rtl::OUString makeURLAppendSysPathSegment(
    ::rtl::OUString const & baseURL, ::rtl::OUString const & relPath );

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC ::rtl::OUString generateRandomPipeId();

class AbortChannel;
//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> resolveUnoURL(
    ::rtl::OUString const & connectString,
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext> const & xLocalContext,
    AbortChannel * abortChannel = 0 );

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC bool office_is_running();

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
oslProcess raiseProcess( ::rtl::OUString const & appURL,
                         ::com::sun::star::uno::Sequence< ::rtl::OUString > const & args );

//==============================================================================

/** writes the argument string to the console.
    On Linux/Unix/etc. it converts the UTF16 string to an ANSI string using
    osl_getThreadTextEncoding() as target encoding. On Windows it uses WriteFile
    with the standard out stream. unopkg.com reads the data and prints them out using
    WriteConsoleW.
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
void writeConsole(::rtl::OUString const & sText);

/** writes the argument to the console using the error stream.
    Otherwise the same as writeConsole.
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
void writeConsoleError(::rtl::OUString const & sText);


/** reads from the console.
    On Linux/Unix/etc. it uses fgets to read char values and converts them to OUString
    using osl_getThreadTextEncoding as target encoding. The returned string has a maximum
    size of 1024 and does NOT include leading and trailing white space(applied OUString::trim())
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::rtl::OUString readConsole();

/** print the text to the console in a debug build.
    The argument is forwarded to writeConsole. The function does not add new line.
    The code is only executed if  OSL_DEBUG_LEVEL > 1
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
void TRACE(::rtl::OUString const & sText);

/** registers or revokes shared or bundled extensions which have been
    recently added or removed.
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
void syncRepositories(
    bool force,
    ::com::sun::star::uno::Reference<
        ::com::sun::star::ucb::XCommandEnvironment> const & xCmdEnv);

/** workaround: for some reason the bridge threads which communicate with the
    uno.exe process are not released on time
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
void disposeBridges(
    com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
        const & ctx);

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
