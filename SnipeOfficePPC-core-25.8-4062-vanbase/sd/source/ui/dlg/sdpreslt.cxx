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


#include <svl/itemset.hxx>
#include <sfx2/new.hxx>
#include <vcl/msgbox.hxx>

#include "strings.hrc"
#include "res_bmp.hrc"
#include "sdpreslt.hxx"
#include "sdpreslt.hrc"
#include "sdattr.hxx"
#include "sdresid.hxx"
#include "drawdoc.hxx"
#include "sdpage.hxx"
#include "DrawDocShell.hxx"

#define DOCUMENT_TOKEN (sal_Unicode('#'))

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

SdPresLayoutDlg::SdPresLayoutDlg(
    ::sd::DrawDocShell* pDocShell,
    ::Window* pWindow,
    const SfxItemSet& rInAttrs ):
                ModalDialog         (pWindow, SdResId(DLG_PRESLT)),
                mpDocSh              ( pDocShell ),
                maFtLayout           (this, SdResId(FT_LAYOUT)),
                maVS                 (this, SdResId(VS_LAYOUT)),
                maBtnOK              (this, SdResId(BTN_OK)),
                maBtnCancel          (this, SdResId(BTN_CANCEL)),
                maBtnHelp            (this, SdResId(BTN_HELP)),
                maCbxMasterPage     (this, SdResId(CBX_MASTER_PAGE)),
                maCbxCheckMasters   (this, SdResId(CBX_CHECK_MASTERS)),
                maBtnLoad            (this, SdResId(BTN_LOAD)),
                mrOutAttrs           (rInAttrs),
                maStrNone           ( SdResId( STR_NULL ) )
{
    FreeResource();

    maVS.SetDoubleClickHdl(LINK(this, SdPresLayoutDlg, ClickLayoutHdl));
    maBtnLoad.SetClickHdl(LINK(this, SdPresLayoutDlg, ClickLoadHdl));

    Reset();
}

/*************************************************************************
|*
|*  Dtor
|*
*************************************************************************/

SdPresLayoutDlg::~SdPresLayoutDlg()
{
}

/*************************************************************************
|*
|*    Initialisierung
|*
*************************************************************************/

void SdPresLayoutDlg::Reset()
{
    const SfxPoolItem *pPoolItem = NULL;
    long nName;

    // MasterPage austauschen
    if( mrOutAttrs.GetItemState( ATTR_PRESLAYOUT_MASTER_PAGE, sal_False, &pPoolItem ) == SFX_ITEM_SET )
    {
        sal_Bool bMasterPage = ( (const SfxBoolItem*) pPoolItem)->GetValue();
        maCbxMasterPage.Enable( !bMasterPage );
        maCbxMasterPage.Check( bMasterPage );
    }

    // Nicht verwendete MasterPages entfernen
    maCbxCheckMasters.Check(sal_False);

    if(mrOutAttrs.GetItemState(ATTR_PRESLAYOUT_NAME, sal_True, &pPoolItem) == SFX_ITEM_SET)
        maName = ((const SfxStringItem*)pPoolItem)->GetValue();
    else
        maName.Erase();

    FillValueSet();

    mnLayoutCount = maLayoutNames.size();
    for( nName = 0; nName < mnLayoutCount; nName++ )
    {
        if (maLayoutNames[nName] == maName)
            break;
    }
    DBG_ASSERT(nName < mnLayoutCount, "Layout nicht gefunden");

    maVS.SelectItem((sal_uInt16)nName + 1);  // Inizes des ValueSets beginnen bei 1

}

/*************************************************************************
|*
|*    Fuellt uebergebenen Item-Set mit Dialogbox-Attributen
|*
*************************************************************************/

void SdPresLayoutDlg::GetAttr(SfxItemSet& rOutAttrs)
{
    short nId = maVS.GetSelectItemId();
    sal_Bool bLoad = nId > mnLayoutCount;
    rOutAttrs.Put( SfxBoolItem( ATTR_PRESLAYOUT_LOAD, bLoad ) );

    String aLayoutName;

    if( bLoad )
    {
        aLayoutName = maName;
        aLayoutName.Append( DOCUMENT_TOKEN );
        aLayoutName.Append( maLayoutNames[ nId - 1 ] );
    }
    else
    {
        aLayoutName = maLayoutNames[ nId - 1 ];
        if( aLayoutName == maStrNone )
            aLayoutName.Erase(); //  so wird "- keine -" codiert (s.u.)
    }

    rOutAttrs.Put( SfxStringItem( ATTR_PRESLAYOUT_NAME, aLayoutName ) );
    rOutAttrs.Put( SfxBoolItem( ATTR_PRESLAYOUT_MASTER_PAGE, maCbxMasterPage.IsChecked() ) );
    rOutAttrs.Put( SfxBoolItem( ATTR_PRESLAYOUT_CHECK_MASTERS, maCbxCheckMasters.IsChecked() ) );
}


/*************************************************************************
|*
|* Fuellt das ValueSet mit Bitmaps
|*
\************************************************************************/

void SdPresLayoutDlg::FillValueSet()
{
    maVS.SetStyle(maVS.GetStyle() | WB_ITEMBORDER | WB_DOUBLEBORDER
                                      | WB_VSCROLL | WB_NAMEFIELD);

    maVS.SetColCount(2);
    maVS.SetLineCount(2);
    maVS.SetExtraSpacing(2);

    SdDrawDocument* pDoc = mpDocSh->GetDoc();

    sal_uInt16 nCount = pDoc->GetMasterPageCount();

    for (sal_uInt16 nLayout = 0; nLayout < nCount; nLayout++)
    {
        SdPage* pMaster = (SdPage*)pDoc->GetMasterPage(nLayout);
        if (pMaster->GetPageKind() == PK_STANDARD)
        {
            String aLayoutName(pMaster->GetLayoutName());
            aLayoutName.Erase( aLayoutName.SearchAscii( SD_LT_SEPARATOR ) );
            maLayoutNames.push_back(new String(aLayoutName));

            Bitmap aBitmap(mpDocSh->GetPagePreviewBitmap(pMaster, 90));
            maVS.InsertItem((sal_uInt16)maLayoutNames.size(), aBitmap, aLayoutName);
        }
    }

    maVS.Show();
}


/*************************************************************************
|*
|* Doppelklick-Handler
|*
\************************************************************************/

IMPL_LINK_NOARG(SdPresLayoutDlg, ClickLayoutHdl)
{
    EndDialog(RET_OK);
    return 0;
}

/*************************************************************************
|*
|* Klick-Handler fuer Laden-Button
|*
\************************************************************************/

IMPL_LINK_NOARG(SdPresLayoutDlg, ClickLoadHdl)
{
    SfxNewFileDialog* pDlg = new SfxNewFileDialog(this, SFXWB_PREVIEW);
    pDlg->SetText(String(SdResId(STR_LOAD_PRESENTATION_LAYOUT)));

    if(!IsReallyVisible())
    {
        delete pDlg;
        return 0;
    }

    sal_uInt16 nResult = pDlg->Execute();
    // Inserted update to force repaint
    Update();

    String aFile;
    sal_Bool   bCancel = sal_False;

    switch (nResult)
    {
        case RET_OK:
        {
            if (pDlg->IsTemplate())
            {
                maName = pDlg->GetTemplateFileName();
            }
            else
            {
                // so wird "- keine -" codiert
                maName.Erase();
            }
        }
        break;

        default:
            bCancel = sal_True;
    }
    delete pDlg;

    if( !bCancel )
    {
        // Pruefen, ob Vorlage schon vorhanden
        sal_Bool bExists = sal_False;
        String aCompareStr( maName );
        if( maName.Len() == 0 )
            aCompareStr = maStrNone;

        sal_uInt16 aPos = 0;
        for (boost::ptr_vector<String>::iterator it = maLayoutNames.begin();
	          it != maLayoutNames.end() && !bExists; ++it, ++aPos)
        {
            if( aCompareStr == *it )
            {
                bExists = sal_True;
                // Vorlage selektieren
                maVS.SelectItem( aPos + 1 );
            }
        }

        if( !bExists )
        {
            // Dokument laden um Preview-Bitmap zu ermitteln (wenn Vorlage ausgewaehlt)
            if( maName.Len() )
            {
                // Dokument ermitteln, um OpenBookmarkDoc rufen zu koennen
                SdDrawDocument* pDoc = mpDocSh->GetDoc();
                SdDrawDocument* pTemplDoc  = pDoc->OpenBookmarkDoc( maName );

                if (pTemplDoc)
                {
                    ::sd::DrawDocShell*  pTemplDocSh= pTemplDoc->GetDocSh();

                    sal_uInt16 nCount = pTemplDoc->GetMasterPageCount();

                    for (sal_uInt16 nLayout = 0; nLayout < nCount; nLayout++)
                    {
                        SdPage* pMaster = (SdPage*) pTemplDoc->GetMasterPage(nLayout);
                        if (pMaster->GetPageKind() == PK_STANDARD)
                        {
                            String aLayoutName(pMaster->GetLayoutName());
                            aLayoutName.Erase( aLayoutName.SearchAscii( SD_LT_SEPARATOR ) );
                            maLayoutNames.push_back(new String(aLayoutName));

                            Bitmap aBitmap(pTemplDocSh->GetPagePreviewBitmap(pMaster, 90));
                            maVS.InsertItem((sal_uInt16)maLayoutNames.size(), aBitmap, aLayoutName);
                        }
                    }
                }
                else
                {
                    bCancel = sal_True;
                }

                pDoc->CloseBookmarkDoc();
            }
            else
            {
                // leeres Layout
                maLayoutNames.push_back( new String( maStrNone ) );
                maVS.InsertItem( (sal_uInt16) maLayoutNames.size(),
                        Bitmap( SdResId( BMP_FOIL_NONE ) ), maStrNone );
            }

            if (!bCancel)
            {
                // Vorlage selektieren
                maVS.SelectItem( (sal_uInt16) maLayoutNames.size() );
            }
        }
    }

    return( 0 );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
