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

#include <vcl/print.hxx>

#include <svtools/svtresid.hxx>
#include "prnsetup.hrc"
#include <svtools/prnsetup.hxx>

// =======================================================================

void ImplFillPrnDlgListBox( const Printer* pPrinter,
                            ListBox* pBox, PushButton* pPropBtn )
{
    ImplFreePrnDlgListBox( pBox );

    const std::vector<rtl::OUString>& rPrinters = Printer::GetPrinterQueues();
    unsigned int nCount = rPrinters.size();
    if ( nCount )
    {
        for( unsigned int i = 0; i < nCount; i++ )
            pBox->InsertEntry( rPrinters[i] );
        pBox->SelectEntry( pPrinter->GetName() );
    }

    pBox->Enable( nCount != 0 );
    pPropBtn->Show( pPrinter->HasSupport( SUPPORT_SETUPDIALOG ) );
}

// -----------------------------------------------------------------------

void ImplFreePrnDlgListBox( ListBox* pBox, sal_Bool bClear )
{
    if ( bClear )
        pBox->Clear();
}

// -----------------------------------------------------------------------

Printer* ImplPrnDlgListBoxSelect( ListBox* pBox, PushButton* pPropBtn,
                                  Printer* pPrinter, Printer* pTempPrinter )
{
    if ( pBox->GetSelectEntryPos() != LISTBOX_ENTRY_NOTFOUND )
    {
        const QueueInfo* pInfo = Printer::GetQueueInfo( pBox->GetSelectEntry(), true );
        if( pInfo)
        {
            if ( !pTempPrinter )
            {
                if ( (pPrinter->GetName() == pInfo->GetPrinterName()) &&
                     (pPrinter->GetDriverName() == pInfo->GetDriver()) )
                    pTempPrinter = new Printer( pPrinter->GetJobSetup() );
                else
                    pTempPrinter = new Printer( *pInfo );
            }
            else
            {
                if ( (pTempPrinter->GetName() != pInfo->GetPrinterName()) ||
                     (pTempPrinter->GetDriverName() != pInfo->GetDriver()) )
                {
                    delete pTempPrinter;
                    pTempPrinter = new Printer( *pInfo );
                }
            }

            pPropBtn->Enable( pTempPrinter->HasSupport( SUPPORT_SETUPDIALOG ) );
        }
        else
            pPropBtn->Disable();
    }
    else
        pPropBtn->Disable();

    return pTempPrinter;
}

// -----------------------------------------------------------------------

Printer* ImplPrnDlgUpdatePrinter( Printer* pPrinter, Printer* pTempPrinter )
{
    XubString aPrnName;
    if ( pTempPrinter )
        aPrnName = pTempPrinter->GetName();
    else
        aPrnName = pPrinter->GetName();

    if ( ! Printer::GetQueueInfo( aPrnName, false ) )
    {
        if ( pTempPrinter )
            delete pTempPrinter;
        pTempPrinter = new Printer;
    }

    return pTempPrinter;
}

// -----------------------------------------------------------------------

void ImplPrnDlgUpdateQueueInfo( ListBox* pBox, QueueInfo& rInfo )
{
    if ( pBox->GetSelectEntryPos() != LISTBOX_ENTRY_NOTFOUND )
    {
        const QueueInfo* pInfo = Printer::GetQueueInfo( pBox->GetSelectEntry(), true );
        if( pInfo )
            rInfo = *pInfo;
    }
}

// -----------------------------------------------------------------------

static void ImplPrnDlgAddString( XubString& rStr, const XubString& rAddStr )
{
    if ( rStr.Len() )
        rStr.AppendAscii( "; " );
    rStr += rAddStr;
}

// -----------------------------------------------------------------------

static void ImplPrnDlgAddResString( XubString& rStr, sal_uInt16 nResId )
{
    ImplPrnDlgAddString( rStr, SVT_RESSTR(nResId) );
}

// -----------------------------------------------------------------------

XubString ImplPrnDlgGetStatusText( const QueueInfo& rInfo )
{
    XubString   aStr;
    sal_uLong       nStatus = rInfo.GetStatus();

    // Default-Printer
    if ( !rInfo.GetPrinterName().isEmpty() &&
         (rInfo.GetPrinterName() == Printer::GetDefaultPrinterName()) )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_DEFPRINTER );

    // Status
    if ( nStatus & QUEUE_STATUS_READY )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_READY );
    if ( nStatus & QUEUE_STATUS_PAUSED )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_PAUSED );
    if ( nStatus & QUEUE_STATUS_PENDING_DELETION )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_PENDING );
    if ( nStatus & QUEUE_STATUS_BUSY )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_BUSY );
    if ( nStatus & QUEUE_STATUS_INITIALIZING )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_INITIALIZING );
    if ( nStatus & QUEUE_STATUS_WAITING )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_WAITING );
    if ( nStatus & QUEUE_STATUS_WARMING_UP )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_WARMING_UP );
    if ( nStatus & QUEUE_STATUS_PROCESSING )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_PROCESSING );
    if ( nStatus & QUEUE_STATUS_PRINTING )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_PRINTING );
    if ( nStatus & QUEUE_STATUS_OFFLINE )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_OFFLINE );
    if ( nStatus & QUEUE_STATUS_ERROR )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_ERROR );
    if ( nStatus & QUEUE_STATUS_SERVER_UNKNOWN )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_SERVER_UNKNOWN );
    if ( nStatus & QUEUE_STATUS_PAPER_JAM )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_PAPER_JAM );
    if ( nStatus & QUEUE_STATUS_PAPER_OUT )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_PAPER_OUT );
    if ( nStatus & QUEUE_STATUS_MANUAL_FEED )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_MANUAL_FEED );
    if ( nStatus & QUEUE_STATUS_PAPER_PROBLEM )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_PAPER_PROBLEM );
    if ( nStatus & QUEUE_STATUS_IO_ACTIVE )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_IO_ACTIVE );
    if ( nStatus & QUEUE_STATUS_OUTPUT_BIN_FULL )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_OUTPUT_BIN_FULL );
    if ( nStatus & QUEUE_STATUS_TONER_LOW )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_TONER_LOW );
    if ( nStatus & QUEUE_STATUS_NO_TONER )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_NO_TONER );
    if ( nStatus & QUEUE_STATUS_PAGE_PUNT )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_PAGE_PUNT );
    if ( nStatus & QUEUE_STATUS_USER_INTERVENTION )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_USER_INTERVENTION );
    if ( nStatus & QUEUE_STATUS_OUT_OF_MEMORY )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_OUT_OF_MEMORY );
    if ( nStatus & QUEUE_STATUS_DOOR_OPEN )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_DOOR_OPEN );
    if ( nStatus & QUEUE_STATUS_POWER_SAVE )
        ImplPrnDlgAddResString( aStr, STR_SVT_PRNDLG_POWER_SAVE );

    // Anzahl Jobs
    sal_uLong nJobs = rInfo.GetJobs();
    if ( nJobs && (nJobs != QUEUE_JOBS_DONTKNOW) )
    {
        XubString aJobStr( SVT_RESSTR( STR_SVT_PRNDLG_JOBCOUNT ) );
        XubString aJobs( XubString::CreateFromInt32( nJobs ) );
        aJobStr.SearchAndReplaceAscii( "%d", aJobs );
        ImplPrnDlgAddString( aStr, aJobStr );
    }

    return aStr;
}

// =======================================================================

PrinterSetupDialog::PrinterSetupDialog( Window* pWindow ) :
    ModalDialog     ( pWindow, SvtResId( DLG_SVT_PRNDLG_PRNSETUPDLG ) ),
    maFlPrinter     ( this, SvtResId( FL_PRINTER ) ),
    maFtName        ( this, SvtResId( FT_NAME ) ),
    maLbName        ( this, SvtResId( LB_NAMES ) ),
    maBtnProperties ( this, SvtResId( BTN_PROPERTIES ) ),
    maBtnOptions    ( this, SvtResId( BTN_OPTIONS ) ),
    maFtStatus      ( this, SvtResId( FT_STATUS ) ),
    maFiStatus      ( this, SvtResId( FI_STATUS ) ),
    maFtType        ( this, SvtResId( FT_TYPE ) ),
    maFiType        ( this, SvtResId( FI_TYPE ) ),
    maFtLocation    ( this, SvtResId( FT_LOCATION ) ),
    maFiLocation    ( this, SvtResId( FI_LOCATION ) ),
    maFtComment     ( this, SvtResId( FT_COMMENT ) ),
    maFiComment     ( this, SvtResId( FI_COMMENT ) ),
    maFlSepButton   ( this, SvtResId( FL_SEPBUTTON ) ),
    maBtnOK         ( this, SvtResId( BTN_OK ) ),
    maBtnCancel     ( this, SvtResId( BTN_CANCEL ) ),
    maBtnHelp       ( this, SvtResId( BTN_HELP ) )
{
    FreeResource();

    // show options button only if link is set
    maBtnOptions.Hide();

    mpPrinter       = NULL;
    mpTempPrinter   = NULL;

    maStatusTimer.SetTimeout( IMPL_PRINTDLG_STATUS_UPDATE );
    maStatusTimer.SetTimeoutHdl( LINK( this, PrinterSetupDialog, ImplStatusHdl ) );
    maBtnProperties.SetClickHdl( LINK( this, PrinterSetupDialog, ImplPropertiesHdl ) );
    maLbName.SetSelectHdl( LINK( this, PrinterSetupDialog, ImplChangePrinterHdl ) );
}

// -----------------------------------------------------------------------

PrinterSetupDialog::~PrinterSetupDialog()
{
    ImplFreePrnDlgListBox( &maLbName, sal_False );
    delete mpTempPrinter;
}

// -----------------------------------------------------------------------

void PrinterSetupDialog::SetOptionsHdl( const Link& rLink )
{
    maBtnOptions.SetClickHdl( rLink );
    maBtnOptions.Show( rLink.IsSet() );
}

void PrinterSetupDialog::ImplSetInfo()
{
    const QueueInfo* pInfo = Printer::GetQueueInfo(maLbName.GetSelectEntry(), true);
    if ( pInfo )
    {
        maFiType.SetText( pInfo->GetDriver() );
        maFiLocation.SetText( pInfo->GetLocation() );
        maFiComment.SetText( pInfo->GetComment() );
        maFiStatus.SetText( ImplPrnDlgGetStatusText( *pInfo ) );
    }
    else
    {
        XubString aTempStr;
        maFiType.SetText( aTempStr );
        maFiLocation.SetText( aTempStr );
        maFiComment.SetText( aTempStr );
        maFiStatus.SetText( aTempStr );
    }
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(PrinterSetupDialog, ImplStatusHdl)
{
    QueueInfo aInfo;
    ImplPrnDlgUpdateQueueInfo( &maLbName, aInfo );
    maFiStatus.SetText( ImplPrnDlgGetStatusText( aInfo ) );

    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(PrinterSetupDialog, ImplPropertiesHdl)
{
    if ( !mpTempPrinter )
        mpTempPrinter = new Printer( mpPrinter->GetJobSetup() );
    mpTempPrinter->Setup( this );

    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(PrinterSetupDialog, ImplChangePrinterHdl)
{
    mpTempPrinter = ImplPrnDlgListBoxSelect( &maLbName, &maBtnProperties,
                                             mpPrinter, mpTempPrinter );
    ImplSetInfo();
    return 0;
}

// -----------------------------------------------------------------------

long PrinterSetupDialog::Notify( NotifyEvent& rNEvt )
{
    if ( (rNEvt.GetType() == EVENT_GETFOCUS) && IsReallyVisible() )
        ImplStatusHdl( &maStatusTimer );

    return ModalDialog::Notify( rNEvt );
}

// -----------------------------------------------------------------------

void PrinterSetupDialog::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( rDCEvt.GetType() == DATACHANGED_PRINTER )
    {
        mpTempPrinter = ImplPrnDlgUpdatePrinter( mpPrinter, mpTempPrinter );
        Printer* pPrn;
        if ( mpTempPrinter )
            pPrn = mpTempPrinter;
        else
            pPrn = mpPrinter;
        ImplFillPrnDlgListBox( pPrn, &maLbName, &maBtnProperties );
        ImplSetInfo();
    }

    ModalDialog::DataChanged( rDCEvt );
}

// -----------------------------------------------------------------------

short PrinterSetupDialog::Execute()
{
    if ( !mpPrinter || mpPrinter->IsPrinting() || mpPrinter->IsJobActive() )
    {
        SAL_WARN( "svtools.dialogs", "PrinterSetupDialog::Execute() - No Printer or printer is printing" );
        return sal_False;
    }

    Printer::updatePrinters();

    ImplFillPrnDlgListBox( mpPrinter, &maLbName, &maBtnProperties );
    ImplSetInfo();
    maStatusTimer.Start();

    // Dialog starten
    short nRet = ModalDialog::Execute();

    // Wenn Dialog mit OK beendet wurde, dann die Daten updaten
    if ( nRet == sal_True )
    {
        if ( mpTempPrinter )
            mpPrinter->SetPrinterProps( mpTempPrinter );
    }

    maStatusTimer.Stop();

    return nRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
