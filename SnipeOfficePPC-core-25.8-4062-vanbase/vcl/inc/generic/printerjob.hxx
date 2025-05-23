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

#ifndef _PSPRINT_PRINTERJOB_HXX_
#define _PSPRINT_PRINTERJOB_HXX_

#include "vcl/jobdata.hxx"
#include "osl/file.hxx"
#include "rtl/string.hxx"

#include <list>

namespace psp {

// forward declarations
class PrinterGfx;


class PrinterJob
{
private:            // private data

    rtl::OUString           maSpoolDirName;
    rtl::OUString           maFileName; // empty: spool to command, else spool to named file
    rtl::OUString           maJobTitle;
    int                     mnFileMode;

    osl::File*              mpJobHeader;
    osl::File*              mpJobTrailer;

    std::list< osl::File* > maPageList;
    std::list< osl::File* > maHeaderList;

    JobData                 m_aDocumentJobData;
    JobData                 m_aLastJobData;
    PrinterGfx*             m_pGraphics;

    sal_uInt32      mnResolution;

    sal_uInt32      mnWidthPt;
    sal_uInt32      mnHeightPt;
    sal_uInt32      mnMaxWidthPt;
    sal_uInt32      mnMaxHeightPt;

    int mnLandscapes;
    int mnPortraits;

    sal_uInt32      mnLMarginPt;
    sal_uInt32      mnRMarginPt;
    sal_uInt32      mnTMarginPt;
    sal_uInt32      mnBMarginPt;

    double          mfXScale;
    double          mfYScale;

    bool            m_bQuickJob;

private:            // private methods

    osl::File*      CreateSpoolFile (const rtl::OUString& rName,
                                     const rtl::OUString& rExtension);
    void            InitPaperSize (const JobData& rJobSetup);

    bool            writeFeatureList( osl::File* pFile, const JobData&, bool bDocumentSetup );
    bool            writeSetup( osl::File* pFile, const JobData& );
    bool            writePageSetup( osl::File* pFile, const JobData&, bool bWriteFeatures = true );
    void            writeJobPatch( osl::File* File, const JobData& );
    bool            writeProlog (osl::File* pFile, const JobData& );

public:             // for usage in PrinterGfx

    sal_uInt32      GetResolution () const { return mnResolution; }
    void            GetScale (double &rXScale, double &rYScale) const;
    sal_uInt16      GetDepth () const;
    sal_uInt16      GetPostscriptLevel (const JobData *pJobData = NULL) const;
    sal_Bool        IsColorPrinter () const;

    osl::File*      GetCurrentPageHeader ();
    osl::File*      GetCurrentPageBody ();

    const ::rtl::OUString& GetPrinterName() const { return m_aLastJobData.m_aPrinterName; }

public:
    PrinterJob ();
    ~PrinterJob ();

    /*  rFileName: if length is greater than 0 save resulting PostScript
     *  to named file.
     *  nMode: only meaningful when saving to file: if nonzero, try
     *  to impose the mode on the resulting file's inode; for nonexistant
     *  files use open, for existant files try a chmod
     *  rJobName: text to appear in the %%Title comment
     *  rAppName: text to appear in the %%Creator comment
     *  rSetupData: JobData that apply to this job
     *  pGraphics: the graphics used to print this job;
     *             this graphics must live until End/AbortJob has returned
     *  bIsQuickJob: the job was started as "direct print" meaning
     *               the quick command for spooling should be used instead
     *               of the normal command
     */
    sal_Bool        StartJob (const rtl::OUString& rFileName,
                              int nMode,
                              const rtl::OUString& rJobName,
                              const rtl::OUString& rAppName,
                              const JobData& rSetupData,
                              PrinterGfx* pGraphics,
                              bool bIsQuickJob
                              );
    sal_Bool        EndJob ();

    sal_Bool        AbortJob ();

    sal_Bool        StartPage (const JobData& rJobSetup);
    sal_Bool        EndPage ();
};

}  /* namespace psp */

#endif /* _PSPRINT_PRINTERJOB_HXX_ */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
