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

// ============================================================================

#ifndef SC_XLTRACER_HXX
#define SC_XLTRACER_HXX

#include "global.hxx"        // ScAddress
#include "xltools.hxx"

// As Trace features become implemented, we can safely delete the enum entry as
// we use the member mnID to keep track of the actual trace tag ID value.
enum XclTracerId
{
    eUnKnown  ,          /// unused but allows us to set the correct index
    eRowLimitExceeded ,
    eTabLimitExceeded ,
    ePassword ,
    ePrintRange ,
    eShortDate ,
    eBorderLineStyle ,
    eFillPattern ,
    eInvisibleGrid ,
    eFormattedNote ,
    eFormulaExtName ,
    eFormulaMissingArg ,
    ePivotDataSource ,
    ePivotChartExists ,
    eChartUnKnownType ,
    eChartTrendLines ,
    eChartErrorBars ,
    eChartOnlySheet ,
    eChartRange ,
    eChartDSName,
    eChartDataTable,
    eChartLegendPosition,
    eChartTextFormatting,
    eChartEmbeddedObj,
    eChartAxisAuto,
    eChartAxisManual,
    eChartInvalidXY,
    eUnsupportedObject ,
    eObjectNotPrintable ,
    eDVType,
    eTraceLength         /// this *should* always be the final entry
};

struct XclTracerDetails
{
    XclTracerId                 meProblemId;    /// Excel Import Trace index.
    sal_uInt32                  mnID;           /// actual ID Index trace tag Value
    const sal_Char*             mpContext;      /// Context for problem e.g. Limits
    const sal_Char*             mpDetail;       /// Context Detail e.g. SheetX
    const sal_Char*             mpProblem;      /// Description of problem
};


// ============================================================================

/** This class wraps an MSFilterTracer to create trace logs for import/export filters. */
class XclTracer
{
public:
    explicit                    XclTracer( const String& rDocUrl );
    virtual                     ~XclTracer();

    /** Returns true, if tracing is enabled. */
    inline bool                 IsEnabled() const { return mbEnabled; }

    /** Ensure that particular traces are logged once per document. */
    void                        ProcessTraceOnce(XclTracerId eProblem);

    void                        TraceInvalidAddress(const ScAddress& rPos, const ScAddress& rMaxPos);
    void                        TraceInvalidRow( sal_uInt32 nRow, sal_uInt32 nMaxrow );
    void                        TraceInvalidTab( SCTAB nTab, SCTAB nMaxTab);
    void                        TracePrintRange();
    void                        TraceDates(sal_uInt16 nNumFmt);
    void                        TraceBorderLineStyle(bool bBorderLineStyle);
    void                        TraceFillPattern(bool bFillPattern);
    void                        TraceFormulaMissingArg();
    void                        TracePivotDataSource(bool bExternal);
    void                        TracePivotChartExists();
    void                        TraceChartUnKnownType();
    void                        TraceChartOnlySheet();
    void                        TraceChartDataTable();
    void                        TraceChartLegendPosition();
    void                        TraceUnsupportedObjects();
    void                        TraceObjectNotPrintable();
    void                        TraceDVType(bool bType);

private:
    bool                        mbEnabled;
    typedef ::std::vector< bool >     BoolVec;
    /** array of flags corresponding to each entry in the XclTracerDetails table. */
    BoolVec                     maFirstTimes;
};


// ============================================================================

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
