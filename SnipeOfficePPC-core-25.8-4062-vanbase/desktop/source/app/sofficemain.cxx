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

#include "sal/config.h"

#include "desktopdllapi.h"

#include "app.hxx"
#include "cmdlineargs.hxx"
#include "cmdlinehelp.hxx"

#include <rtl/logfile.hxx>
#include <rtl/bootstrap.hxx>
#include <tools/extendapplicationenvironment.hxx>

int SVMain();

// -=-= main() -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

extern "C" int DESKTOP_DLLPUBLIC soffice_main()
{
#if defined ANDROID
    try {
        rtl::Bootstrap::setIniFilename(
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("file:///assets/program/lofficerc")));
#endif
    tools::extendApplicationEnvironment();

    RTL_LOGFILE_PRODUCT_TRACE( "PERFORMANCE - enter Main()" );

    desktop::Desktop aDesktop;
    // This string is used during initialization of the Gtk+ VCL module
    aDesktop.SetAppName( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("soffice")) );
#ifdef UNX
    // handle --version and --help already here, otherwise they would be handled
    // after VCL initialization that might fail if $DISPLAY is not set
    const desktop::CommandLineArgs& rCmdLineArgs = aDesktop.GetCommandLineArgs();
    OUString aUnknown( rCmdLineArgs.GetUnknown() );
    if ( !aUnknown.isEmpty() )
    {
        desktop::Desktop::InitApplicationServiceManager();
        desktop::displayCmdlineHelp( aUnknown );
        return EXIT_FAILURE;
    }
    if ( rCmdLineArgs.IsHelp() )
    {
        desktop::Desktop::InitApplicationServiceManager();
        desktop::displayCmdlineHelp( OUString() );
        return EXIT_SUCCESS;
    }
    if ( rCmdLineArgs.IsVersion() )
    {
        desktop::Desktop::InitApplicationServiceManager();
        desktop::displayVersion();
        return EXIT_SUCCESS;
    }
#endif
    return SVMain();
#if defined ANDROID
    } catch (const ::com::sun::star::uno::Exception &e) {
        fprintf (stderr, "Not handled UNO exception at main: '%s'\n",
                 rtl::OUStringToOString(e.Message, RTL_TEXTENCODING_UTF8).getStr());
        throw; // to get exception type printed
    }
#endif
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
