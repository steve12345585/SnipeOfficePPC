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
