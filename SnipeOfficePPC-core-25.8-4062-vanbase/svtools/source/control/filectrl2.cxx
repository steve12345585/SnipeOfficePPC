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


// this file contains code from filectrl.cxx which needs to be compiled with enabled exception hanling
#include <svtools/filectrl.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#include <comphelper/processfactory.hxx>
#include <tools/urlobj.hxx>
#include <osl/file.h>
#include <vcl/stdtext.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::ui;

void FileControl::ImplBrowseFile( )
{
    try
    {
        XubString aNewText;

        const ::rtl::OUString sServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.ui.dialogs.FilePicker" ));

        Reference< XMultiServiceFactory > xMSF = comphelper::getProcessServiceFactory();
        Reference < dialogs::XFilePicker > xFilePicker( xMSF->createInstance( sServiceName ), UNO_QUERY );
        if ( xFilePicker.is() )
        {
            // transform the system notation text into a file URL
            ::rtl::OUString sSystemNotation = GetText(), sFileURL;
            oslFileError nError = osl_getFileURLFromSystemPath( sSystemNotation.pData, &sFileURL.pData );
            if ( nError == osl_File_E_INVAL )
                sFileURL = GetText();   // #97709# Maybe URL is already a file URL...

            //#90430# Check if URL is really a file URL
            ::rtl::OUString aTmp;
            if ( osl_getSystemPathFromFileURL( sFileURL.pData, &aTmp.pData ) == osl_File_E_None )
            {
                // initially set this directory
                xFilePicker->setDisplayDirectory( sFileURL );
            }

            if ( xFilePicker.is() && xFilePicker->execute() )
            {
                Sequence < rtl::OUString > aPathSeq = xFilePicker->getFiles();

                if ( aPathSeq.getLength() )
                {
                    aNewText = aPathSeq[0];
                    INetURLObject aObj( aNewText );
                    if ( aObj.GetProtocol() == INET_PROT_FILE )
                        aNewText = aObj.PathToFileName();
                    SetText( aNewText );
                    maEdit.GetModifyHdl().Call( &maEdit );
                }
            }
        }
        else
            ShowServiceNotAvailableError( this, sServiceName, sal_True );
    }
    catch( const Exception& )
    {
        OSL_FAIL( "FileControl::ImplBrowseFile: caught an exception while executing the file picker!" );
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
