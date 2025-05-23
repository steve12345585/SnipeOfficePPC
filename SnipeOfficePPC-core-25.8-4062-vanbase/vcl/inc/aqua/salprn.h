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

#include "aqua/aquavcltypes.h"

#include "salprn.hxx"

#include <boost/shared_array.hpp>


// ---------------------
// - AquaSalInfoPrinter -
// ---------------------

class AquaSalGraphics;

class AquaSalInfoPrinter : public SalInfoPrinter
{
    /// Printer graphics
    AquaSalGraphics*        mpGraphics;
    /// is Graphics used
    bool                    mbGraphics;
    /// job active ?
    bool                    mbJob;

    /// cocoa printer object
    NSPrinter*              mpPrinter;
    /// cocoa print info object
    NSPrintInfo*            mpPrintInfo;

    /// FIXME: get real printer context for infoprinter if possible
    /// fake context for info printer
    /// graphics context for Quartz 2D
    CGContextRef                            mrContext;
    /// memory for graphics bitmap context for querying metrics
    boost::shared_array< sal_uInt8 >        maContextMemory;

    // since changes to NSPrintInfo during a job are ignored
    // we have to care for some settings ourselves
    // currently we do this for orientation;
    // really needed however is a solution for paper formats
    Orientation               mePageOrientation;

    int                       mnStartPageOffsetX;
    int                       mnStartPageOffsetY;
    sal_Int32                 mnCurPageRangeStart;
    sal_Int32                 mnCurPageRangeCount;

    public:
    AquaSalInfoPrinter( const SalPrinterQueueInfo& pInfo );
    virtual ~AquaSalInfoPrinter();

    void                        SetupPrinterGraphics( CGContextRef i_xContext ) const;

    virtual SalGraphics*        GetGraphics();
    virtual void                ReleaseGraphics( SalGraphics* i_pGraphics );
    virtual sal_Bool                Setup( SalFrame* i_pFrame, ImplJobSetup* i_pSetupData );
    virtual sal_Bool                SetPrinterData( ImplJobSetup* pSetupData );
    virtual sal_Bool                SetData( sal_uLong i_nFlags, ImplJobSetup* i_pSetupData );
    virtual void                GetPageInfo( const ImplJobSetup* i_pSetupData,
                                             long& o_rOutWidth, long& o_rOutHeight,
                                             long& o_rPageOffX, long& o_rPageOffY,
                                             long& o_rPageWidth, long& o_rPageHeight );
    virtual sal_uLong               GetCapabilities( const ImplJobSetup* i_pSetupData, sal_uInt16 i_nType );
    virtual sal_uLong               GetPaperBinCount( const ImplJobSetup* i_pSetupData );
    virtual rtl::OUString              GetPaperBinName( const ImplJobSetup* i_pSetupData, sal_uLong i_nPaperBin );
    virtual void                InitPaperFormats( const ImplJobSetup* i_pSetupData );
    virtual int                 GetLandscapeAngle( const ImplJobSetup* i_pSetupData );

    // the artificial separation between InfoPrinter and Printer
    // is not really useful for us
    // so let's make AquaSalPrinter just a forwarder to AquaSalInfoPrinter
    // and concentrate the real work in one class
    // implement pull model print system
    sal_Bool                        StartJob( const rtl::OUString* i_pFileName,
                                          const rtl::OUString& rJobName,
                                          const rtl::OUString& i_rAppName,
                                          ImplJobSetup* i_pSetupData,
                                          vcl::PrinterController& i_rController );
    sal_Bool                        EndJob();
    sal_Bool                        AbortJob();
    SalGraphics*                StartPage( ImplJobSetup* i_pSetupData, sal_Bool i_bNewJobData );
    sal_Bool                        EndPage();
    sal_uLong                       GetErrorCode() const;

    NSPrintInfo* getPrintInfo() const { return mpPrintInfo; }
    void setStartPageOffset( int nOffsetX, int nOffsetY ) { mnStartPageOffsetX = nOffsetX; mnStartPageOffsetY = nOffsetY; }
    sal_Int32 getCurPageRangeStart() const { return mnCurPageRangeStart; }
    sal_Int32 getCurPageRangeCount() const { return mnCurPageRangeCount; }

    // match width/height against known paper formats, possibly switching orientation
    const PaperInfo* matchPaper( long i_nWidth, long i_nHeight, Orientation& o_rOrientation ) const;
    void setPaperSize( long i_nWidth, long i_nHeight, Orientation i_eSetOrientation );

    private:
    AquaSalInfoPrinter( const AquaSalInfoPrinter& );
    AquaSalInfoPrinter& operator=(const AquaSalInfoPrinter&);
};

// -----------------
// - AquaSalPrinter -
// -----------------

class AquaSalPrinter : public SalPrinter
{
    AquaSalInfoPrinter*         mpInfoPrinter;          // pointer to the compatible InfoPrinter
    public:
    AquaSalPrinter( AquaSalInfoPrinter* i_pInfoPrinter );
    virtual ~AquaSalPrinter();

    virtual sal_Bool                    StartJob( const rtl::OUString* i_pFileName,
                                              const rtl::OUString& i_rJobName,
                                              const rtl::OUString& i_rAppName,
                                              sal_uLong i_nCopies,
                                              bool i_bCollate,
                                              bool i_bDirect,
                                              ImplJobSetup* i_pSetupData );
    // implement pull model print system
    virtual sal_Bool                    StartJob( const rtl::OUString* i_pFileName,
                                              const rtl::OUString& rJobName,
                                              const rtl::OUString& i_rAppName,
                                              ImplJobSetup* i_pSetupData,
                                              vcl::PrinterController& i_rListener );

    virtual sal_Bool                    EndJob();
    virtual sal_Bool                    AbortJob();
    virtual SalGraphics*            StartPage( ImplJobSetup* i_pSetupData, sal_Bool i_bNewJobData );
    virtual sal_Bool                    EndPage();
    virtual sal_uLong                   GetErrorCode();

    private:
    AquaSalPrinter( const AquaSalPrinter& );
    AquaSalPrinter& operator=(const AquaSalPrinter&);
};

const double fPtTo100thMM = 35.27777778;

inline int PtTo10Mu( double nPoints ) { return (int)(((nPoints)*fPtTo100thMM)+0.5); }

inline double TenMuToPt( double nUnits ) { return floor(((nUnits)/fPtTo100thMM)+0.5); }



#endif // _SV_SALPRN_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
