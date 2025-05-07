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

#ifndef _SV_CVTMTF_HXX
#define _SV_CVTMTF_HXX

#include <stack>
#include <vcl/dllapi.h>
#include <vcl/metaact.hxx>
#include <vcl/gdimtf.hxx>

// -----------
// - Defines -
// -----------

#define CONVERT_TO_SVM1                 0x00000001UL
#define CONVERT_FROM_SVM1               0x00000002UL

#define GDI_PIXEL_ACTION                1
#define GDI_POINT_ACTION                2
#define GDI_LINE_ACTION                 3
#define GDI_RECT_ACTION                 4
#define GDI_ELLIPSE_ACTION              5
#define GDI_ARC_ACTION                  6
#define GDI_PIE_ACTION                  7
#define GDI_INVERTRECT_ACTION           8
#define GDI_HIGHLIGHTRECT_ACTION        9
#define GDI_POLYLINE_ACTION             10
#define GDI_POLYGON_ACTION              11
#define GDI_POLYPOLYGON_ACTION          12
#define GDI_TEXT_ACTION                 13
#define GDI_TEXTARRAY_ACTION            14
#define GDI_STRETCHTEXT_ACTION          15
#define GDI_ICON_ACTION                 16
#define GDI_BITMAP_ACTION               17
#define GDI_BITMAPSCALE_ACTION          18
#define GDI_PEN_ACTION                  19
#define GDI_FONT_ACTION                 20
#define GDI_BACKBRUSH_ACTION            21
#define GDI_FILLBRUSH_ACTION            22
#define GDI_MAPMODE_ACTION              23
#define GDI_CLIPREGION_ACTION           24
#define GDI_RASTEROP_ACTION             25
#define GDI_PUSH_ACTION                 26
#define GDI_POP_ACTION                  27
#define GDI_MOVECLIPREGION_ACTION       28
#define GDI_ISECTCLIPREGION_ACTION      29
#define GDI_MTF_ACTION                  30
#define GDI_BITMAPSCALEPART_ACTION      32
#define GDI_GRADIENT_ACTION             33

#define GDI_TRANSPARENT_COMMENT         1024
#define GDI_HATCH_COMMENT               1025
#define GDI_REFPOINT_COMMENT            1026
#define GDI_TEXTLINECOLOR_COMMENT       1027
#define GDI_TEXTLINE_COMMENT            1028
#define GDI_FLOATTRANSPARENT_COMMENT    1029
#define GDI_GRADIENTEX_COMMENT          1030
#define GDI_COMMENT_COMMENT             1031
#define GDI_UNICODE_COMMENT             1032

#define GDI_LINEJOIN_ACTION             1033
#define GDI_EXTENDEDPOLYGON_ACTION      1034
#define GDI_LINEDASHDOT_ACTION          1035

// ----------------
// - SVMConverter -
// ----------------

class VCL_PLUGIN_PUBLIC SVMConverter
{
private:
    SAL_DLLPRIVATE void             ImplConvertFromSVM1( SvStream& rIStm, GDIMetaFile& rMtf );
    SAL_DLLPRIVATE void             ImplConvertToSVM1( SvStream& rOStm, GDIMetaFile& rMtf );
    SAL_DLLPRIVATE sal_uLong            ImplWriteActions( SvStream& rOStm, GDIMetaFile& rMtf,
                                          VirtualDevice& rSaveVDev, sal_Bool& rRop_0_1,
                                          Color& rLineCol, ::std::stack<Color*>& rLineColStack,
                                          rtl_TextEncoding& rActualCharSet );

public:
                        SVMConverter( SvStream& rIStm, GDIMetaFile& rMtf, sal_uLong nConvertMode  );
                        ~SVMConverter() {}

private:
                        // Not implemented
                        SVMConverter( const SVMConverter& );
    SVMConverter&       operator=( const SVMConverter& );
};

#endif // _SV_CVTMTF_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
