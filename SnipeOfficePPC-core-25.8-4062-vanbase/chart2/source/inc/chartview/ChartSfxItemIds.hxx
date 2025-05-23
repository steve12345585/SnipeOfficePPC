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
#ifndef CHARTSFXITEMIDS_HXX
#define CHARTSFXITEMIDS_HXX

// ============================================================
// SCHATTR
// ============================================================

// can't this be changed to 0?
#define SCHATTR_START                       1

#define SCHATTR_DATADESCR_START             SCHATTR_START
#define SCHATTR_DATADESCR_SHOW_NUMBER       SCHATTR_DATADESCR_START
#define SCHATTR_DATADESCR_SHOW_PERCENTAGE   (SCHATTR_DATADESCR_START + 1)
#define SCHATTR_DATADESCR_SHOW_CATEGORY     (SCHATTR_DATADESCR_START + 2)
#define SCHATTR_DATADESCR_SHOW_SYMBOL       (SCHATTR_DATADESCR_START + 3)
#define SCHATTR_DATADESCR_SEPARATOR         (SCHATTR_DATADESCR_START + 4)
#define SCHATTR_DATADESCR_PLACEMENT         (SCHATTR_DATADESCR_START + 5)
#define SCHATTR_DATADESCR_AVAILABLE_PLACEMENTS   (SCHATTR_DATADESCR_START + 6)
#define SCHATTR_DATADESCR_NO_PERCENTVALUE   (SCHATTR_DATADESCR_START + 7) //percentage values should not be offered
#define SCHATTR_PERCENT_NUMBERFORMAT_VALUE  (SCHATTR_DATADESCR_START + 8)
#define SCHATTR_PERCENT_NUMBERFORMAT_SOURCE (SCHATTR_DATADESCR_START + 9)
#define SCHATTR_DATADESCR_END               SCHATTR_PERCENT_NUMBERFORMAT_SOURCE

//legend
#define SCHATTR_LEGEND_START                (SCHATTR_DATADESCR_END + 1)
#define SCHATTR_LEGEND_POS                  SCHATTR_LEGEND_START
#define SCHATTR_LEGEND_SHOW                 (SCHATTR_LEGEND_START + 1)
#define SCHATTR_LEGEND_END                  SCHATTR_LEGEND_SHOW

//text
#define SCHATTR_TEXT_START                  (SCHATTR_LEGEND_END + 1)
#define SCHATTR_TEXT_DEGREES                SCHATTR_TEXT_START
#define SCHATTR_TEXT_STACKED                (SCHATTR_TEXT_START + 1)
#define SCHATTR_TEXT_END                    SCHATTR_TEXT_STACKED

// statistic
#define SCHATTR_STAT_START                  (SCHATTR_TEXT_END + 1)
#define SCHATTR_STAT_AVERAGE                SCHATTR_STAT_START
#define SCHATTR_STAT_KIND_ERROR             (SCHATTR_STAT_START + 1)
#define SCHATTR_STAT_PERCENT                (SCHATTR_STAT_START + 2)
#define SCHATTR_STAT_BIGERROR               (SCHATTR_STAT_START + 3)
#define SCHATTR_STAT_CONSTPLUS              (SCHATTR_STAT_START + 4)
#define SCHATTR_STAT_CONSTMINUS             (SCHATTR_STAT_START + 5)
#define SCHATTR_STAT_INDICATE               (SCHATTR_STAT_START + 6)
#define SCHATTR_STAT_RANGE_POS              (SCHATTR_STAT_START + 7)
#define SCHATTR_STAT_RANGE_NEG              (SCHATTR_STAT_START + 8)
#define SCHATTR_STAT_ERRORBAR_TYPE          (SCHATTR_STAT_START + 9)
#define SCHATTR_STAT_END                    SCHATTR_STAT_ERRORBAR_TYPE

// --------------------------------------------------------
// these attributes are for replacement of enum eChartStyle
// --------------------------------------------------------

#define SCHATTR_STYLE_START             ( SCHATTR_STAT_END +1 )

// for whole chart
#define SCHATTR_STYLE_DEEP              ( SCHATTR_STYLE_START     )
#define SCHATTR_STYLE_3D                ( SCHATTR_STYLE_START + 1 )
#define SCHATTR_STYLE_VERTICAL          ( SCHATTR_STYLE_START + 2 )

// also for series
#define SCHATTR_STYLE_BASETYPE          ( SCHATTR_STYLE_START + 3 )// Line,Area,...,Pie
#define SCHATTR_STYLE_LINES             ( SCHATTR_STYLE_START + 4 )// draw line
#define SCHATTR_STYLE_PERCENT           ( SCHATTR_STYLE_START + 5 )
#define SCHATTR_STYLE_STACKED           ( SCHATTR_STYLE_START + 6 )
#define SCHATTR_STYLE_SPLINES           ( SCHATTR_STYLE_START + 7 )

// also for data point
#define SCHATTR_STYLE_SYMBOL            ( SCHATTR_STYLE_START + 8 )
#define SCHATTR_STYLE_SHAPE             ( SCHATTR_STYLE_START + 9 )
#define SCHATTR_STYLE_END               ( SCHATTR_STYLE_SHAPE )

// --------------------------------------------------------------------------

#define SCHATTR_AXIS                    (SCHATTR_STYLE_END + 1)// see chtmodel.hxx defines CHART_AXIS_PRIMARY_X, etc.
//Re-mapped:
#define SCHATTR_AXIS_START              (SCHATTR_AXIS + 1)
//axis scale
#define SCHATTR_AXISTYPE                 SCHATTR_AXIS_START
#define SCHATTR_AXIS_REVERSE            (SCHATTR_AXIS_START + 1)
#define SCHATTR_AXIS_AUTO_MIN           (SCHATTR_AXIS_START + 2)
#define SCHATTR_AXIS_MIN                (SCHATTR_AXIS_START + 3)
#define SCHATTR_AXIS_AUTO_MAX           (SCHATTR_AXIS_START + 4)
#define SCHATTR_AXIS_MAX                (SCHATTR_AXIS_START + 5)
#define SCHATTR_AXIS_AUTO_STEP_MAIN     (SCHATTR_AXIS_START + 6)
#define SCHATTR_AXIS_STEP_MAIN          (SCHATTR_AXIS_START + 7)
#define SCHATTR_AXIS_MAIN_TIME_UNIT     (SCHATTR_AXIS_START + 8)
#define SCHATTR_AXIS_AUTO_STEP_HELP     (SCHATTR_AXIS_START + 9)
#define SCHATTR_AXIS_STEP_HELP          (SCHATTR_AXIS_START + 10)
#define SCHATTR_AXIS_HELP_TIME_UNIT     (SCHATTR_AXIS_START + 11)
#define SCHATTR_AXIS_AUTO_TIME_RESOLUTION   (SCHATTR_AXIS_START + 12)
#define SCHATTR_AXIS_TIME_RESOLUTION    (SCHATTR_AXIS_START + 13)
#define SCHATTR_AXIS_LOGARITHM          (SCHATTR_AXIS_START + 14)
#define SCHATTR_AXIS_AUTO_DATEAXIS      (SCHATTR_AXIS_START + 15)
#define SCHATTR_AXIS_ALLOW_DATEAXIS     (SCHATTR_AXIS_START + 16)
#define SCHATTR_AXIS_AUTO_ORIGIN        (SCHATTR_AXIS_START + 17)
#define SCHATTR_AXIS_ORIGIN             (SCHATTR_AXIS_START + 18)
//axis position
#define SCHATTR_AXIS_POSITION_START     (SCHATTR_AXIS_ORIGIN +1)
#define SCHATTR_AXIS_TICKS              SCHATTR_AXIS_POSITION_START
#define SCHATTR_AXIS_HELPTICKS          (SCHATTR_AXIS_POSITION_START + 1)
#define SCHATTR_AXIS_POSITION           (SCHATTR_AXIS_POSITION_START + 2)
#define SCHATTR_AXIS_POSITION_VALUE     (SCHATTR_AXIS_POSITION_START + 3)
#define SCHATTR_AXIS_CROSSING_MAIN_AXIS_NUMBERFORMAT    (SCHATTR_AXIS_POSITION_START + 4)
#define SCHATTR_AXIS_LABEL_POSITION     (SCHATTR_AXIS_POSITION_START + 5)
#define SCHATTR_AXIS_MARK_POSITION      (SCHATTR_AXIS_POSITION_START + 6)
//axis label
#define SCHATTR_AXIS_LABEL_START        (SCHATTR_AXIS_MARK_POSITION +1)
#define SCHATTR_AXIS_SHOWDESCR          SCHATTR_AXIS_LABEL_START
#define SCHATTR_AXIS_LABEL_ORDER        (SCHATTR_AXIS_LABEL_START + 1)
#define SCHATTR_AXIS_LABEL_OVERLAP      (SCHATTR_AXIS_LABEL_START + 2)
#define SCHATTR_AXIS_LABEL_BREAK        (SCHATTR_AXIS_LABEL_START + 3)
#define SCHATTR_AXIS_LABEL_END          SCHATTR_AXIS_LABEL_BREAK

#define SCHATTR_AXIS_END                SCHATTR_AXIS_LABEL_END

#define SCHATTR_SYMBOL_BRUSH            (SCHATTR_AXIS_END + 1)
#define SCHATTR_STOCK_VOLUME            (SCHATTR_AXIS_END + 2)
#define SCHATTR_STOCK_UPDOWN            (SCHATTR_AXIS_END + 3)
#define SCHATTR_SYMBOL_SIZE             (SCHATTR_AXIS_END + 4)

// non persistent items (binary format)
#define SCHATTR_CHARTTYPE_START         (SCHATTR_SYMBOL_SIZE + 1)

// new from New Chart
#define SCHATTR_BAR_OVERLAP             (SCHATTR_CHARTTYPE_START )
#define SCHATTR_BAR_GAPWIDTH            (SCHATTR_CHARTTYPE_START + 1)
#define SCHATTR_BAR_CONNECT             (SCHATTR_CHARTTYPE_START + 2)
#define SCHATTR_NUM_OF_LINES_FOR_BAR    (SCHATTR_CHARTTYPE_START + 3)
#define SCHATTR_SPLINE_ORDER            (SCHATTR_CHARTTYPE_START + 4)
#define SCHATTR_SPLINE_RESOLUTION       (SCHATTR_CHARTTYPE_START + 5)
#define SCHATTR_DIAGRAM_STYLE           (SCHATTR_CHARTTYPE_START + 6)
#define SCHATTR_GROUP_BARS_PER_AXIS     (SCHATTR_CHARTTYPE_START + 7)
#define SCHATTR_STARTING_ANGLE          (SCHATTR_CHARTTYPE_START + 8)
#define SCHATTR_CLOCKWISE               (SCHATTR_CHARTTYPE_START + 9)
#define SCHATTR_MISSING_VALUE_TREATMENT     (SCHATTR_CHARTTYPE_START + 10)
#define SCHATTR_AVAILABLE_MISSING_VALUE_TREATMENTS (SCHATTR_CHARTTYPE_START + 11)
#define SCHATTR_INCLUDE_HIDDEN_CELLS    (SCHATTR_CHARTTYPE_START + 12)

#define SCHATTR_CHARTTYPE_END           SCHATTR_INCLUDE_HIDDEN_CELLS

// items for transporting information to dialogs
#define SCHATTR_MISC_START              (SCHATTR_CHARTTYPE_END + 1)
#define SCHATTR_AXIS_FOR_ALL_SERIES     (SCHATTR_MISC_START)
#define SCHATTR_MISC_END                 SCHATTR_AXIS_FOR_ALL_SERIES

// regression curve equation
#define SCHATTR_REGRESSION_START         (SCHATTR_MISC_END + 1)
#define SCHATTR_REGRESSION_TYPE           SCHATTR_REGRESSION_START
#define SCHATTR_REGRESSION_SHOW_EQUATION (SCHATTR_REGRESSION_START + 1)
#define SCHATTR_REGRESSION_SHOW_COEFF    (SCHATTR_REGRESSION_START + 2)
#define SCHATTR_REGRESSION_END            SCHATTR_REGRESSION_SHOW_COEFF

#define SCHATTR_END                     SCHATTR_REGRESSION_END

// ============================================================
// values for Items
// ============================================================

// values for SCHATTR_AXIS_TICKS and SCHATTR_AXIS_HELPTICKS items
#define CHAXIS_MARK_BOTH   3
#define CHAXIS_MARK_OUTER  2
#define CHAXIS_MARK_INNER  1
#define CHAXIS_MARK_NONE   0

// values for SCHATTR_AXISTYPE items
#define CHART_AXIS_REALNUMBER   0
#define CHART_AXIS_PERCENT      1
#define CHART_AXIS_CATEGORY     2
#define CHART_AXIS_SERIES       3
#define CHART_AXIS_DATE         4

// values for SCHATTR_STYLE_SHAPE items
#define CHART_SHAPE3D_IGNORE  -2 //internal! (GetChartShapeStyle()!)
#define CHART_SHAPE3D_ANY     -1 //undefined type (GetChartShapeStyle()!)
#define CHART_SHAPE3D_SQUARE   0
#define CHART_SHAPE3D_CYLINDER 1
#define CHART_SHAPE3D_CONE     2
#define CHART_SHAPE3D_PYRAMID  3 //reserved
#define CHART_SHAPE3D_HANOI    4

// values for SCHATTR_AXIS items
#define CHART_AXIS_PRIMARY_X    1
#define CHART_AXIS_PRIMARY_Y    2
#define CHART_AXIS_PRIMARY_Z    3
#define CHART_AXIS_SECONDARY_Y  4
#define CHART_AXIS_SECONDARY_X  5

// ============================================================
// ============================================================

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
