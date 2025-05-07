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


#undef SC_DLLIMPLEMENTATION



#include <comphelper/string.hxx>
#include <sfx2/app.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/docfilt.hxx>
#include <sfx2/docinsert.hxx>
#include <sfx2/fcontnr.hxx>
#include <sfx2/filedlghelper.hxx>
#include <svtools/ehdl.hxx>
#include <svtools/sfxecode.hxx>
#include <vcl/waitobj.hxx>

#include "linkarea.hxx"
#include "linkarea.hrc"
#include "scresid.hxx"
#include "sc.hrc"
#include "rangeutl.hxx"
#include "docsh.hxx"
#include "tablink.hxx"

//==================================================================

ScLinkedAreaDlg::ScLinkedAreaDlg( Window* pParent ) :
    ModalDialog ( pParent, ScResId( RID_SCDLG_LINKAREA ) ),
    //
    aFlLocation ( this, ScResId( FL_LOCATION ) ),
    aCbUrl      ( this, ScResId( CB_URL ) ),
    aBtnBrowse  ( this, ScResId( BTN_BROWSE ) ),
    aTxtHint    ( this, ScResId( FT_HINT ) ),
    aFtRanges   ( this, ScResId( FT_RANGES ) ),
    aLbRanges   ( this, ScResId( LB_RANGES ) ),
    aBtnReload  ( this, ScResId( BTN_RELOAD ) ),
    aNfDelay    ( this, ScResId( NF_DELAY ) ),
    aFtSeconds  ( this, ScResId( FT_SECONDS ) ),
    aBtnOk      ( this, ScResId( BTN_OK ) ),
    aBtnCancel  ( this, ScResId( BTN_CANCEL ) ),
    aBtnHelp    ( this, ScResId( BTN_HELP ) ),
    //
    pSourceShell( NULL ),
    pDocInserter( NULL )

{
    FreeResource();

    aCbUrl.SetHelpId( HID_SCDLG_LINKAREAURL );  // SvtURLBox ctor always sets SID_OPENURL
    aCbUrl.SetSelectHdl( LINK( this, ScLinkedAreaDlg, FileHdl ) );
    aBtnBrowse.SetClickHdl( LINK( this, ScLinkedAreaDlg, BrowseHdl ) );
    aLbRanges.SetSelectHdl( LINK( this, ScLinkedAreaDlg, RangeHdl ) );
    aBtnReload.SetClickHdl( LINK( this, ScLinkedAreaDlg, ReloadHdl ) );
    UpdateEnable();

    aNfDelay.SetAccessibleName(aBtnReload.GetText());
    aNfDelay.SetAccessibleRelationLabeledBy(&aBtnReload);
}

ScLinkedAreaDlg::~ScLinkedAreaDlg()
{
    // pSourceShell is deleted by aSourceRef
}

short ScLinkedAreaDlg::Execute()
{
    // set parent for file dialog or filter options

    Window* pOldDefParent = Application::GetDefDialogParent();
    Application::SetDefDialogParent( this );

    short nRet = ModalDialog::Execute();

    Application::SetDefDialogParent( pOldDefParent );

    return nRet;
}

#define FILTERNAME_HTML  "HTML (StarCalc)"
#define FILTERNAME_QUERY "calc_HTML_WebQuery"

IMPL_LINK_NOARG(ScLinkedAreaDlg, BrowseHdl)
{
    if ( !pDocInserter )
        pDocInserter = new sfx2::DocumentInserter(
            String::CreateFromAscii( ScDocShell::Factory().GetShortName() ) );
    pDocInserter->StartExecuteModal( LINK( this, ScLinkedAreaDlg, DialogClosedHdl ) );
    return 0;
}

IMPL_LINK_NOARG(ScLinkedAreaDlg, FileHdl)
{
    rtl::OUString aEntered = aCbUrl.GetURL();
    if (pSourceShell)
    {
        SfxMedium* pMed = pSourceShell->GetMedium();
        if ( aEntered.equals(pMed->GetName()) )
        {
            //  already loaded - nothing to do
            return 0;
        }
    }

    rtl::OUString aFilter;
    rtl::OUString aOptions;
    //  get filter name by looking at the file content (bWithContent = TRUE)
    // Break operation if any error occurred inside.
    if (!ScDocumentLoader::GetFilterName( aEntered, aFilter, aOptions, true, true ))
        return 0;

    // #i53241# replace HTML filter with DataQuery filter
    if (aFilter.equalsAscii(FILTERNAME_HTML))
        aFilter = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(FILTERNAME_QUERY));

    LoadDocument( aEntered, aFilter, aOptions );

    UpdateSourceRanges();
    UpdateEnable();
    return 0;
}

void ScLinkedAreaDlg::LoadDocument( const rtl::OUString& rFile, const String& rFilter, const String& rOptions )
{
    if ( pSourceShell )
    {
        //  unload old document
        pSourceShell->DoClose();
        pSourceShell = NULL;
        aSourceRef.Clear();
    }

    if ( !rFile.isEmpty() )
    {
        WaitObject aWait( this );

        rtl::OUString aNewFilter = rFilter;
        rtl::OUString aNewOptions = rOptions;

        SfxErrorContext aEc( ERRCTX_SFX_OPENDOC, rFile );

        ScDocumentLoader aLoader( rFile, aNewFilter, aNewOptions, 0, true );    // with interaction
        pSourceShell = aLoader.GetDocShell();
        if ( pSourceShell )
        {
            sal_uLong nErr = pSourceShell->GetErrorCode();
            if (nErr)
                ErrorHandler::HandleError( nErr );      // including warnings

            aSourceRef = pSourceShell;
            aLoader.ReleaseDocRef();    // don't call DoClose in DocLoader dtor
        }
    }
}

void ScLinkedAreaDlg::InitFromOldLink( const String& rFile, const String& rFilter,
                                        const String& rOptions, const String& rSource,
                                        sal_uLong nRefresh )
{
    LoadDocument( rFile, rFilter, rOptions );
    if (pSourceShell)
    {
        SfxMedium* pMed = pSourceShell->GetMedium();
        aCbUrl.SetText( pMed->GetName() );
    }
    else
        aCbUrl.SetText( EMPTY_STRING );

    UpdateSourceRanges();

    xub_StrLen nRangeCount = comphelper::string::getTokenCount(rSource, ';');
    for ( xub_StrLen i=0; i<nRangeCount; i++ )
    {
        String aRange = rSource.GetToken(i);
        aLbRanges.SelectEntry( aRange );
    }

    bool bDoRefresh = (nRefresh != 0);
    aBtnReload.Check( bDoRefresh );
    if (bDoRefresh)
        aNfDelay.SetValue( nRefresh );

    UpdateEnable();
}

IMPL_LINK_NOARG(ScLinkedAreaDlg, RangeHdl)
{
    UpdateEnable();
    return 0;
}

IMPL_LINK_NOARG(ScLinkedAreaDlg, ReloadHdl)
{
    UpdateEnable();
    return 0;
}

IMPL_LINK( ScLinkedAreaDlg, DialogClosedHdl, sfx2::FileDialogHelper*, _pFileDlg )
{
    if ( _pFileDlg->GetError() != ERRCODE_NONE )
        return 0;

    SfxMedium* pMed = pDocInserter->CreateMedium();
    if ( pMed )
    {
        WaitObject aWait( this );

        // replace HTML filter with DataQuery filter
        const rtl::OUString aHTMLFilterName( RTL_CONSTASCII_USTRINGPARAM( FILTERNAME_HTML ) );
        const rtl::OUString aWebQFilterName( RTL_CONSTASCII_USTRINGPARAM( FILTERNAME_QUERY ) );

        const SfxFilter* pFilter = pMed->GetFilter();
        if (pFilter && aHTMLFilterName.equals(pFilter->GetFilterName()))
        {
            const SfxFilter* pNewFilter =
                ScDocShell::Factory().GetFilterContainer()->GetFilter4FilterName( aWebQFilterName );
            if( pNewFilter )
                pMed->SetFilter( pNewFilter );
        }

        //  ERRCTX_SFX_OPENDOC -> "Fehler beim Laden des Dokumentes"
        SfxErrorContext aEc( ERRCTX_SFX_OPENDOC, pMed->GetName() );

        if (pSourceShell)
            pSourceShell->DoClose();        // deleted when assigning aSourceRef

        pMed->UseInteractionHandler( sal_True );    // to enable the filter options dialog

        pSourceShell = new ScDocShell;
        aSourceRef = pSourceShell;
        pSourceShell->DoLoad( pMed );

        sal_uLong nErr = pSourceShell->GetErrorCode();
        if (nErr)
            ErrorHandler::HandleError( nErr );              // including warnings

        if ( !pSourceShell->GetError() )                    // only errors
        {
            aCbUrl.SetText( pMed->GetName() );
        }
        else
        {
            pSourceShell->DoClose();
            pSourceShell = NULL;
            aSourceRef.Clear();

            aCbUrl.SetText( EMPTY_STRING );
        }
    }

    UpdateSourceRanges();
    UpdateEnable();
    return 0;
}

#undef FILTERNAME_HTML
#undef FILTERNAME_QUERY

void ScLinkedAreaDlg::UpdateSourceRanges()
{
    aLbRanges.SetUpdateMode(false);

    aLbRanges.Clear();
    if ( pSourceShell )
    {
        ScAreaNameIterator aIter( pSourceShell->GetDocument() );
        ScRange aDummy;
        rtl::OUString aName;
        while ( aIter.Next( aName, aDummy ) )
            aLbRanges.InsertEntry( aName );
    }

    aLbRanges.SetUpdateMode(true);

    if ( aLbRanges.GetEntryCount() == 1 )
        aLbRanges.SelectEntryPos(0);
}

void ScLinkedAreaDlg::UpdateEnable()
{
    bool bEnable = ( pSourceShell && aLbRanges.GetSelectEntryCount() );
    aBtnOk.Enable( bEnable );

    bool bReload = aBtnReload.IsChecked();
    aNfDelay.Enable( bReload );
    aFtSeconds.Enable( bReload );
}

rtl::OUString ScLinkedAreaDlg::GetURL()
{
    if (pSourceShell)
    {
        SfxMedium* pMed = pSourceShell->GetMedium();
        return pMed->GetName();
    }
    return EMPTY_STRING;
}

rtl::OUString ScLinkedAreaDlg::GetFilter()
{
    if (pSourceShell)
    {
        SfxMedium* pMed = pSourceShell->GetMedium();
        return pMed->GetFilter()->GetFilterName();
    }
    return rtl::OUString();
}

rtl::OUString ScLinkedAreaDlg::GetOptions()
{
    if (pSourceShell)
    {
        SfxMedium* pMed = pSourceShell->GetMedium();
        return ScDocumentLoader::GetOptions( *pMed );
    }
    return rtl::OUString();
}

rtl::OUString ScLinkedAreaDlg::GetSource()
{
    rtl::OUStringBuffer aBuf;
    sal_uInt16 nCount = aLbRanges.GetSelectEntryCount();
    for (sal_uInt16 i=0; i<nCount; i++)
    {
        if (i > 0)
            aBuf.append(sal_Unicode(';'));
        aBuf.append(aLbRanges.GetSelectEntry(i));
    }
    return aBuf.makeStringAndClear();
}

sal_uLong ScLinkedAreaDlg::GetRefresh()
{
    if ( aBtnReload.IsChecked() )
        return sal::static_int_cast<sal_uLong>( aNfDelay.GetValue() );
    else
        return 0;   // disabled
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
