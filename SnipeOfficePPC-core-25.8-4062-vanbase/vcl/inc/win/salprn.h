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

#ifndef _SV_SALPRN_H
#define _SV_SALPRN_H

#include <salprn.hxx>

// -----------------
// - SalDriverData -
// -----------------

// WNT3
#define SAL_DRIVERDATA_SYSSIGN              ((sal_uIntPtr)0x574E5433)

#pragma pack( 1 )

struct SalDriverData
{
    sal_uIntPtr                 mnSysSignature;
    sal_uInt16                  mnDriverOffset;
    BYTE                    maDriverData[1];
};

#pragma pack()

// ---------------------
// - WinSalInfoPrinter -
// ---------------------

class WinSalGraphics;

class WinSalInfoPrinter : public SalInfoPrinter
{
public:
    WinSalGraphics*         mpGraphics;             // current Printer graphics
    rtl::OUString               maDriverName;           // printer driver name
    rtl::OUString               maDeviceName;           // printer device name
    rtl::OUString               maPortName;             // printer port name
    HDC                     mhDC;                   // printer hdc
    sal_Bool                    mbGraphics;             // is Graphics used
public:
    WinSalInfoPrinter();
    virtual ~WinSalInfoPrinter();

    virtual SalGraphics*            GetGraphics();
    virtual void                    ReleaseGraphics( SalGraphics* pGraphics );
    virtual sal_Bool                    Setup( SalFrame* pFrame, ImplJobSetup* pSetupData );
    virtual sal_Bool                    SetPrinterData( ImplJobSetup* pSetupData );
    virtual sal_Bool                    SetData( sal_uIntPtr nFlags, ImplJobSetup* pSetupData );
    virtual void                    GetPageInfo( const ImplJobSetup* pSetupData,
                                                 long& rOutWidth, long& rOutHeight,
                                                 long& rPageOffX, long& rPageOffY,
                                                 long& rPageWidth, long& rPageHeight );
    virtual sal_uIntPtr                 GetCapabilities( const ImplJobSetup* pSetupData, sal_uInt16 nType );
    virtual sal_uIntPtr                 GetPaperBinCount( const ImplJobSetup* pSetupData );
    virtual rtl::OUString                  GetPaperBinName( const ImplJobSetup* pSetupData, sal_uIntPtr nPaperBin );
    virtual void                    InitPaperFormats( const ImplJobSetup* pSetupData );
    virtual int                 GetLandscapeAngle( const ImplJobSetup* pSetupData );
};

// -----------------
// - WinSalPrinter -
// -----------------

class WinSalPrinter : public SalPrinter
{
public:
    WinSalGraphics*         mpGraphics;             // current Printer graphics
    WinSalInfoPrinter*      mpInfoPrinter;          // pointer to the compatible InfoPrinter
    WinSalPrinter*          mpNextPrinter;          // next printing printer
    HDC                     mhDC;                   // printer hdc
    sal_uIntPtr                 mnError;                // Error Code
    sal_uIntPtr                 mnCopies;               // Kopien
    sal_Bool                    mbCollate;              // Sortierte Kopien
    sal_Bool                    mbAbort;                // Job Aborted

    bool                    mbValid;

public:
    WinSalPrinter();
    virtual ~WinSalPrinter();

    using SalPrinter::StartJob;
    virtual sal_Bool                    StartJob( const rtl::OUString* pFileName,
                                              const rtl::OUString& rJobName,
                                              const rtl::OUString& rAppName,
                                              sal_uIntPtr nCopies,
                                              bool bCollate,
                                              bool bDirect,
                                              ImplJobSetup* pSetupData );
    virtual sal_Bool                    EndJob();
    virtual sal_Bool                    AbortJob();
    virtual SalGraphics*            StartPage( ImplJobSetup* pSetupData, sal_Bool bNewJobData );
    virtual sal_Bool                    EndPage();
    virtual sal_uIntPtr                 GetErrorCode();

    void markInvalid();
    bool isValid() const { return mbValid; }
};

#endif // _SV_SALPRN_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
