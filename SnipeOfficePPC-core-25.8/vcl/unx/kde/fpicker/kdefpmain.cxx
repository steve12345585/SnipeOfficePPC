/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2010 Novell, Inc.
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

#include <cstddef>

#include <kdemodalityfilter.hxx>
#include <kdefilepicker.hxx>
#include <kdecommandthread.hxx>

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>

#include <iostream>
#include <stdlib.h>

#ifdef ENABLE_TDE
#define THIS_DESKENV_NAME_CAP "TDE"
#define THIS_DESKENV_NAME_LOW "tde"
#else // ENABLE_TDE
#define THIS_DESKENV_NAME_CAP "KDE"
#define THIS_DESKENV_NAME_LOW "kde"
#endif // ENABLE_TDE

//////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////

static KCmdLineOptions sOptions[] =
{
    { "winid <argument>", I18N_NOOP("Window ID to which is the fpicker modal"), "0" },
    KCmdLineLastOption
};

int main( int argc, char* argv[] )
{
    // we fake the name of the application to have "LibreOffice" in the
    // title
    KAboutData qAboutData( "kdefilepicker", I18N_NOOP( "LibreOffice" ),
            "0.1", I18N_NOOP( "kdefilepicker is an implementation of the " THIS_DESKENV_NAME_CAP " file dialog for LibreOffice." ),
            KAboutData::License_LGPL,
            "(c) 2004, Jan Holesovsky" );
    qAboutData.addAuthor( "Jan Holesovsky", I18N_NOOP("Original author and current maintainer"), "kendy@openoffice.org" );

    // Let the user see that this does something...
    ::std::cerr << "kdefilepicker, an implementation of a " THIS_DESKENV_NAME_CAP " file dialog for OOo." << ::std::endl
        << "Type 'exit' and press Enter to finish." << ::std::endl;

    KCmdLineArgs::addCmdLineOptions( sOptions );
    KCmdLineArgs::init( argc, argv, &qAboutData );

    KLocale::setMainCatalogue( "kdialog" );

    KApplication kApplication;

    // Setup the modality
    KCmdLineArgs *pArgs = KCmdLineArgs::parsedArgs();
    long nWinId = atol( pArgs->getOption( "winid" ) );
    pArgs->clear();

    KDEModalityFilter qModalityFilter( nWinId );

    KDEFileDialog aFileDialog( NULL, QString(), NULL, THIS_DESKENV_NAME_LOW "filedialog" );

    KDECommandThread qCommandThread( &aFileDialog );
    qCommandThread.start();

    kApplication.exec();

    qCommandThread.wait();

    ::std::cout << "exited" << ::std::endl;

    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
