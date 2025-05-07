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


// include ---------------------------------------------------------------
#include <sfx2/docfile.hxx>
#include <com/sun/star/plugin/PluginDescription.hpp>
#include <com/sun/star/plugin/PluginManager.hpp>
#include <com/sun/star/plugin/XPluginManager.hpp>
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>

#include <comphelper/processfactory.hxx>

#include "svx/pfiledlg.hxx"
#include <svx/dialogs.hrc>

#include <svx/dialmgr.hxx>
#include <list>

using namespace ::rtl;
using namespace ::com::sun::star;

sal_Char const sAudio[] = "audio";
sal_Char const sVideo[] = "video";

// Filedialog to insert Plugin-Fileformats

ErrCode SvxPluginFileDlg::Execute()
{
    return maFileDlg.Execute();
}

String SvxPluginFileDlg::GetPath() const
{
    return maFileDlg.GetPath();
}

SvxPluginFileDlg::SvxPluginFileDlg (Window *, sal_uInt16 nKind )
    : maFileDlg(ui::dialogs::TemplateDescription::FILEOPEN_SIMPLE, SFXWB_INSERT)
{
    // set title of the dialogwindow
    switch (nKind)
    {
        case SID_INSERT_SOUND :
        {
            maFileDlg.SetTitle(SVX_RESSTR(STR_INSERT_SOUND_TITLE));
        }
        break;
        case SID_INSERT_VIDEO :
        {
            maFileDlg.SetTitle(SVX_RESSTR(STR_INSERT_VIDEO_TITLE));
        }
        break;
    }

    // fill the filterlist of the filedialog with data of installed plugins
    uno::Reference< uno::XComponentContext >  xContext = comphelper::getProcessComponentContext();
    uno::Reference< plugin::XPluginManager >  rPluginManager( plugin::PluginManager::create(xContext) );

    const uno::Sequence<plugin::PluginDescription > aSeq( rPluginManager->getPluginDescriptions() );
    const plugin::PluginDescription* pDescription = aSeq.getConstArray();
    sal_Int32 nAnzahlPlugins = rPluginManager->getPluginDescriptions().getLength();

    std::list< String > aPlugNames;
    std::list< String > aPlugExtensions;
    std::list< String >::iterator j;
    std::list< String >::iterator k;
    std::list< String >::const_iterator end;

    for ( int i = 0; i < nAnzahlPlugins; i++ )
    {
        String aStrPlugMIMEType( pDescription[i].Mimetype );
        String aStrPlugName( pDescription[i].Description );
        String aStrPlugExtension( pDescription[i].Extension );

        aStrPlugMIMEType.ToLowerAscii();
        aStrPlugExtension.ToLowerAscii();

        if ( ( nKind == SID_INSERT_SOUND && aStrPlugMIMEType.SearchAscii ( sAudio ) == 0 ) ||
             ( nKind == SID_INSERT_VIDEO && aStrPlugMIMEType.SearchAscii ( sVideo ) == 0 ) )
        {
            // extension already in the filterlist of the filedlg ?
            sal_Bool bAlreadyExist = sal_False;
            for ( j = aPlugExtensions.begin(), end = aPlugExtensions.end(); j != end && !bAlreadyExist; ++j )
            {
                bAlreadyExist = (j->Search( aStrPlugExtension ) != STRING_NOTFOUND );
            }

            if ( !bAlreadyExist )
            {
                // filterdescription already there?
                // (then append the new extension to the existing filter)
                int nfound = -1;
                 for ( j = aPlugNames.begin(),
                          k = aPlugExtensions.begin(),
                          end = aPlugNames.end();
                      j != end && nfound != 0;  )
                {
                    if ( ( nfound = j->Search( aStrPlugName ) ) == 0 )
                    {
                        if ( aStrPlugExtension.Len() > 0 )
                            aStrPlugExtension.Insert( sal_Unicode( ';' ) );
                        aStrPlugExtension.Insert( *k );

                        // remove old entry, increment (iterators are invalid thereafter, thus the postincrement)
                        aPlugNames.erase(j++); aPlugExtensions.erase(k++);

                        // update end iterator (which may be invalid, too!)
                        end = aPlugNames.end();
                    }
                    else
                    {
                        // next element
                        ++j; ++k;
                    }
                }

                // build filterdescription
                aStrPlugName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( "  (" ) );
                aStrPlugName.Append( aStrPlugExtension );
                aStrPlugName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( ")" ) );

                // use a own description for the video-formate avi, mov and mpeg
                // the descriptions of these MIME-types are not very meaningful
                const sal_Char sAVI[] = "*.avi";
                const sal_Char sMOV[] = "*.mov";
                const sal_Char sMPG[] = "*.mpg";
                const sal_Char sMPE[] = "*.mpe";
                const sal_Char sMPEG[] = "*.mpeg";

                if ( aStrPlugExtension.EqualsIgnoreCaseAscii( sAVI ) )
                    aStrPlugName = SVX_RESSTR( STR_INSERT_VIDEO_EXTFILTER_AVI );
                else if ( aStrPlugExtension.EqualsIgnoreCaseAscii( sMOV ) )
                    aStrPlugName = SVX_RESSTR( STR_INSERT_VIDEO_EXTFILTER_MOV );
                else if ( aStrPlugExtension.SearchAscii( sMPG ) != STRING_NOTFOUND ||
                          aStrPlugExtension.SearchAscii( sMPE ) != STRING_NOTFOUND ||
                          aStrPlugExtension.SearchAscii( sMPEG ) != STRING_NOTFOUND )
                    aStrPlugName = SVX_RESSTR(STR_INSERT_VIDEO_EXTFILTER_MPEG);

                aPlugNames.push_back( aStrPlugName );
                aPlugExtensions.push_back( aStrPlugExtension );
            }
        }
    }

    // add filter to dialog
    for ( j = aPlugNames.begin(),
              k = aPlugExtensions.begin(),
              end = aPlugNames.end();
          j != end; ++j, ++k )
    {
        maFileDlg.AddFilter( *j, *k );
    }

    // add the All-Filter
    String aAllFilter( ResId( STR_EXTFILTER_ALL, DIALOG_MGR() ) );
    maFileDlg.AddFilter( aAllFilter, UniString::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "*.*" ) ) );

    // and activate him
    maFileDlg.SetCurrentFilter( aAllFilter );
}

SvxPluginFileDlg::~SvxPluginFileDlg()
{
}

void SvxPluginFileDlg::SetContext( sfx2::FileDialogHelper::Context _eNewContext )
{
    maFileDlg.SetContext( _eNewContext );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
