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


#ifdef SD_DLLIMPLEMENTATION
#undef SD_DLLIMPLEMENTATION
#endif

#include "BreakDlg.hxx"
#include <sfx2/progress.hxx>

#include <svx/svdedtv.hxx>
#include <svx/svdetc.hxx>
#include <sfx2/app.hxx>
#include <vcl/msgbox.hxx>

#include "sdattr.hxx"
#include "brkdlg.hrc"
#include "sdresid.hxx"
#include "View.hxx"
#include "drawview.hxx"
#include "strings.hrc"
#include "DrawDocShell.hxx"

namespace sd {

/*************************************************************************
|*
|* Dialog zum aufbrechen von Metafiles
|*
\************************************************************************/

BreakDlg::BreakDlg(
    ::Window* pWindow,
    DrawView* _pDrView,
    DrawDocShell* pShell,
    sal_uLong nSumActionCount,
    sal_uLong nObjCount )
    : SfxModalDialog     ( pWindow, SdResId( DLG_BREAK ) ),
      aFtObjInfo            ( this, SdResId( FT_OBJ_INFO ) ),
      aFtActInfo            ( this, SdResId( FT_ACT_INFO ) ),
      aFtInsInfo            ( this, SdResId( FT_INS_INFO ) ),
      aFiObjInfo            ( this, SdResId( FI_OBJ_INFO ) ),
      aFiActInfo            ( this, SdResId( FI_ACT_INFO ) ),
      aFiInsInfo            ( this, SdResId( FI_INS_INFO ) ),
      aBtnCancel            ( this, SdResId( BTN_CANCEL ) ),
      aLink             ( LINK( this, BreakDlg, UpDate)),
      mpProgress            ( NULL )
{
    aBtnCancel.SetClickHdl( LINK( this, BreakDlg, CancelButtonHdl));

    mpProgress = new SfxProgress( pShell, String(SdResId(STR_BREAK_METAFILE)), nSumActionCount*3 );

    pProgrInfo = new SvdProgressInfo( &aLink );
    // jede Action wird in DoImport() 3mal bearbeitet
    pProgrInfo->Init( nSumActionCount*3, nObjCount );

    pDrView = _pDrView;
    bCancel = sal_False;

    FreeResource();
}

BreakDlg::~BreakDlg()
{
    if( mpProgress )
        delete mpProgress;

    if( pProgrInfo )
        delete pProgrInfo;
}

// Control-Handler fuer den Abbruch Button
IMPL_LINK_NOARG(BreakDlg, CancelButtonHdl)
{
  bCancel = sal_True;
  aBtnCancel.Disable();
  return( 0L );
}

// Die UpDate Methode muss regelmaessig von der Arbeitsfunktion
// ausgeuehrt werden.
// Beim ersten aufruf wird die gesamtanzahl der actions uebergeben.
// Jeder weitere sollte die bearbeiteten actions seit dem letzten aufruf von
// UpDate erhalten.

IMPL_LINK( BreakDlg, UpDate, void*, nInit )
{
    String aEmptyStr;

    if(pProgrInfo == NULL)
      return 1L;

    // Statuszeile updaten oder Fehlermeldung?
    if(nInit == (void*)1L)
    {
        ErrorBox aErrBox( this, WB_OK, String( SdResId( STR_BREAK_FAIL ) ) );
        aErrBox.Execute();
    }
    else
    {
        if(mpProgress)
            mpProgress->SetState( pProgrInfo->GetSumCurAction() );
    }

    // Welches Oject wird gerade angezeigt?
    String info = UniString::CreateFromInt32( pProgrInfo->GetCurObj() );
    info.Append( sal_Unicode('/') );
    info.Append( UniString::CreateFromInt32( pProgrInfo->GetObjCount() ) );
    aFiObjInfo.SetText(info);

    // Wieviele Actions sind schon aufgebrochen?
    if(pProgrInfo->GetActionCount() == 0)
    {
        aFiActInfo.SetText( aEmptyStr );
    }
    else
    {
        info = UniString::CreateFromInt32( pProgrInfo->GetCurAction() );
        info.Append( sal_Unicode('/') );
        info.Append( UniString::CreateFromInt32( pProgrInfo->GetActionCount() ) );
        aFiActInfo.SetText(info);
    }

    // Und erst eingefuegt????
    if(pProgrInfo->GetInsertCount() == 0)
    {
        aFiInsInfo.SetText( aEmptyStr );
    }
    else
    {
        info = UniString::CreateFromInt32( pProgrInfo->GetCurInsert() );
        info.Append( sal_Unicode('/') );
        info.Append( UniString::CreateFromInt32( pProgrInfo->GetInsertCount() ) );
        aFiInsInfo.SetText(info);
    }

    Application::Reschedule();
    return( bCancel?0L:1L );
}

// Oeffnet den Modalen Dialog und startet einen Timer der die Arbeitsfunktion
// nach oeffnen des Dialogs ausfuehrt
short BreakDlg::Execute()
{
  aTimer.SetTimeout( 10 );
  aTimer.SetTimeoutHdl( LINK( this, BreakDlg, InitialUpdate ) );
  aTimer.Start();

  return SfxModalDialog::Execute();
}

// Linkmethode welche die Arbeitsfunktion startet
IMPL_LINK_NOARG(BreakDlg, InitialUpdate)
{
    pDrView->DoImportMarkedMtf(pProgrInfo);
    EndDialog(sal_True);
    return 0L;
}

} // end of namespace sd

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
