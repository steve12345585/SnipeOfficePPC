/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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


#ifdef SD_DLLIMPLEMENTATION
#undef SD_DLLIMPLEMENTATION
#endif


#include "inspagob.hxx"

#include "strings.hrc"
#include "res_bmp.hrc"
#include "sdresid.hxx"
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#include "ViewShell.hxx"
#include "inspagob.hrc"


/*************************************************************************
|*
|*  Ctor
|*
\************************************************************************/

SdInsertPagesObjsDlg::SdInsertPagesObjsDlg(
    ::Window* pWindow,
    const SdDrawDocument* pInDoc,
    SfxMedium* pSfxMedium,
    const String& rFileName )
    : ModalDialog     ( pWindow, SdResId( DLG_INSERT_PAGES_OBJS ) ),
      aLbTree         ( this, SdResId( LB_TREE ) ),
      aCbxLink        ( this, SdResId( CBX_LINK ) ),
      aCbxMasters     ( this, SdResId( CBX_CHECK_MASTERS ) ),
      aBtnOk          ( this, SdResId( BTN_OK ) ),
      aBtnCancel      ( this, SdResId( BTN_CANCEL ) ),
      aBtnHelp        ( this, SdResId( BTN_HELP ) ),
      pMedium       ( pSfxMedium ),
      mpDoc         ( pInDoc ),
      rName         ( rFileName )
{
    FreeResource();

    aLbTree.SetViewFrame( ( (SdDrawDocument*) pInDoc )->GetDocSh()->GetViewShell()->GetViewFrame() );

    aLbTree.SetSelectHdl( LINK( this, SdInsertPagesObjsDlg, SelectObjectHdl ) );

    // Text wird eingefuegt
    if( !pMedium )
        SetText( String( SdResId( STR_INSERT_TEXT ) ) );

    Reset();
}

/*************************************************************************
|*
|*  Dtor
|*
\************************************************************************/

SdInsertPagesObjsDlg::~SdInsertPagesObjsDlg()
{
}

/*************************************************************************
|*
|*  Fuellt die TreeLB in Abhaengigkeit des Mediums. Ist kein Medium
|*  vorhanden, handelt es sich um ein Text- und kein Drawdokument
|*
\************************************************************************/

void SdInsertPagesObjsDlg::Reset()
{
    if( pMedium )
    {
        aLbTree.SetSelectionMode( MULTIPLE_SELECTION );

        // transfer ownership of Medium
        aLbTree.Fill( mpDoc, pMedium, rName );
    }
    else
    {
        Color aColor( COL_WHITE );
        Bitmap aBmpText( SdResId( BMP_DOC_TEXT ) );
        Image aImgText( aBmpText, aColor );
        aLbTree.InsertEntry( rName, aImgText, aImgText );
    }

    aCbxMasters.Check( sal_True );
}

std::vector<rtl::OUString> SdInsertPagesObjsDlg::GetList( const sal_uInt16 nType )
{
    // Bei Draw-Dokumenten muss bei der Selektion des Dokumentes NULL
    // zurueckgegeben werden
    if( pMedium )
    {
        // Um zu gewaehrleisten, dass die Bookmarks geoeffnet sind
        // (Wenn gesamtes Dokument ausgewaehlt wurde)
        aLbTree.GetBookmarkDoc();

        // Wenn das Dokument (mit-)selektiert oder nichst selektiert ist,
        // wird das gesamte Dokument (und nicht mehr!) eingefuegt.
        if( aLbTree.GetSelectionCount() == 0 ||
            ( aLbTree.IsSelected( aLbTree.First() ) ) )
            return std::vector<rtl::OUString>();
    }

    return aLbTree.GetSelectEntryList( nType );
}

/*************************************************************************
|*
|*  Ist Verknuepfung gechecked
|*
\************************************************************************/

sal_Bool SdInsertPagesObjsDlg::IsLink()
{
    return( aCbxLink.IsChecked() );
}

/*************************************************************************
|*
|*  Ist Verknuepfung gechecked
|*
\************************************************************************/

sal_Bool SdInsertPagesObjsDlg::IsRemoveUnnessesaryMasterPages() const
{
    return( aCbxMasters.IsChecked() );
}

/*************************************************************************
|*
|* Enabled und selektiert Endfarben-LB
|*
\************************************************************************/

IMPL_LINK_NOARG(SdInsertPagesObjsDlg, SelectObjectHdl)
{
    if( aLbTree.IsLinkableSelected() )
        aCbxLink.Enable();
    else
        aCbxLink.Disable();

    return( 0 );
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
