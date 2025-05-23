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

#ifndef INCLUDED_SVTOOLS_PRINTOPTIONS_HXX
#define INCLUDED_SVTOOLS_PRINTOPTIONS_HXX

#include "svtools/svtdllapi.h"
#include <sal/types.h>
#include <osl/mutex.hxx>
#include <rtl/ustring.hxx>
#include <unotools/options.hxx>

class SvtPrintOptions_Impl;

// -----------------------
// - SvtBasePrintOptions -
// -----------------------

class PrinterOptions;

class SVT_DLLPUBLIC SvtBasePrintOptions: public utl::detail::Options
{
protected:

    SvtPrintOptions_Impl* m_pDataContainer;

    void        SetDataContainer( SvtPrintOptions_Impl* pDataContainer ) { m_pDataContainer = pDataContainer; }

public:

    static ::osl::Mutex& GetOwnStaticMutex();

public:

                SvtBasePrintOptions();
                virtual ~SvtBasePrintOptions();

    sal_Bool    IsReduceTransparency() const;
    sal_Int16   GetReducedTransparencyMode() const;
    sal_Bool    IsReduceGradients() const;
    sal_Int16   GetReducedGradientMode() const;
    sal_Int16   GetReducedGradientStepCount() const;
    sal_Bool    IsReduceBitmaps() const;
    sal_Int16   GetReducedBitmapMode() const;
    sal_Int16   GetReducedBitmapResolution() const;
    sal_Bool    IsReducedBitmapIncludesTransparency() const;
    sal_Bool    IsConvertToGreyscales() const;
    sal_Bool    IsPDFAsStandardPrintJobFormat() const;

    void        SetReduceTransparency( sal_Bool bState );
    void        SetReducedTransparencyMode( sal_Int16 nMode );
    void        SetReduceGradients( sal_Bool bState );
    void        SetReducedGradientMode( sal_Int16 nMode );
    void        SetReducedGradientStepCount( sal_Int16 nStepCount );
    void        SetReduceBitmaps( sal_Bool bState );
    void        SetReducedBitmapMode( sal_Int16   bState );
    void        SetReducedBitmapResolution( sal_Int16 nResolution );
    void        SetReducedBitmapIncludesTransparency( sal_Bool bState );
    void        SetConvertToGreyscales( sal_Bool bState );
    void        SetPDFAsStandardPrintJobFormat( sal_Bool bState );

public:

    void        GetPrinterOptions( PrinterOptions& rOptions ) const;
    void        SetPrinterOptions( const PrinterOptions& rOptions );
};

// ---------------------
// - SvtPrinterOptions -
// ---------------------

class SVT_DLLPUBLIC SvtPrinterOptions : public SvtBasePrintOptions
{
private:

    static SvtPrintOptions_Impl*    m_pStaticDataContainer; /// impl. data container as dynamic pointer for smaller memory requirements!
    static sal_Int32                m_nRefCount;            /// internal ref count mechanism

public:

    SvtPrinterOptions();
    virtual ~SvtPrinterOptions();
};

// -----------------------
// - SvtPrintFileOptions -
// -----------------------

class SVT_DLLPUBLIC SvtPrintFileOptions : public SvtBasePrintOptions
{
private:

    static SvtPrintOptions_Impl*    m_pStaticDataContainer; /// impl. data container as dynamic pointer for smaller memory requirements!
    static sal_Int32                m_nRefCount;            /// internal ref count mechanism

public:

    SvtPrintFileOptions();
    virtual ~SvtPrintFileOptions();
};

#endif // INCLUDED_SVTOOLS_PRINTOPTIONS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
