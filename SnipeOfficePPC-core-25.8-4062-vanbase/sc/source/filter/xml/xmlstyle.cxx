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

#include "xmlstyle.hxx"
#include "xmlexprt.hxx"
#include "xmlimprt.hxx"

#include "XMLConverter.hxx"
#include "rangeutl.hxx"
#include "unonames.hxx"

#include <xmloff/xmlnmspe.hxx>
#include <xmloff/xmltypes.hxx>
#include <xmloff/families.hxx>
#include <xmloff/xmlnumfe.hxx>
#include <xmloff/xmlnumfi.hxx>
#include <xmloff/nmspmap.hxx>
#include <xmloff/attrlist.hxx>
#include <xmloff/contextid.hxx>
#include <xmloff/txtprmap.hxx>
#include <sax/tools/converter.hxx>
#include <com/sun/star/util/CellProtection.hpp>
#include <com/sun/star/table/CellOrientation.hpp>
#include <com/sun/star/table/CellVertJustify2.hpp>
#include <com/sun/star/table/CellHoriJustify.hpp>
#include <com/sun/star/table/CellJustifyMethod.hpp>
#include <com/sun/star/table/TableBorder.hpp>
#include <com/sun/star/table/BorderLine2.hpp>
#include <com/sun/star/sheet/XSheetConditionalEntries.hpp>
#include <com/sun/star/sheet/XSheetConditionalEntry.hpp>
#include <com/sun/star/sheet/XSheetCondition.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <comphelper/extract.hxx>

#include <rtl/ustrbuf.hxx>

using namespace com::sun::star;
using namespace ::xmloff::token;
using namespace ::formula;
using ::rtl::OUString;

#define MAP(name,prefix,token,type,context)  { name, sizeof(name)-1, prefix, token, type, context, SvtSaveOptions::ODFVER_010 }
#define MAP_EXT(name,prefix,token,type,context)  { name, sizeof(name)-1, prefix, token, type, context, SvtSaveOptions::ODFVER_012_EXT_COMPAT }
#define MAP_END()   { NULL, 0, 0, XML_TOKEN_INVALID, 0, 0, SvtSaveOptions::ODFVER_010 }

const XMLPropertyMapEntry aXMLScCellStylesProperties[] =
{
    MAP( "AsianVerticalMode", XML_NAMESPACE_STYLE, XML_GLYPH_ORIENTATION_VERTICAL, XML_TYPE_PROP_TABLE_CELL|XML_SC_TYPE_VERTICAL, 0),
    MAP( "BottomBorder", XML_NAMESPACE_FO, XML_BORDER_BOTTOM, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER, CTF_SC_BOTTOMBORDER ),
    MAP( "BottomBorder", XML_NAMESPACE_STYLE, XML_BORDER_LINE_WIDTH_BOTTOM, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER_WIDTH, CTF_SC_BOTTOMBORDERWIDTH ),
    MAP( "CellBackColor", XML_NAMESPACE_FO, XML_BACKGROUND_COLOR, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_COLORTRANSPARENT|MID_FLAG_MULTI_PROPERTY|MID_FLAG_MERGE_ATTRIBUTE, 0 ),
    MAP( "CellProtection", XML_NAMESPACE_STYLE, XML_CELL_PROTECT, XML_TYPE_PROP_TABLE_CELL|XML_SC_TYPE_CELLPROTECTION|MID_FLAG_MERGE_PROPERTY, 0 ),
    MAP( "CellProtection", XML_NAMESPACE_STYLE, XML_PRINT_CONTENT, XML_TYPE_PROP_TABLE_CELL|XML_SC_TYPE_PRINTCONTENT|MID_FLAG_MERGE_PROPERTY, 0 ),
    MAP( "CellStyle", XML_NAMESPACE_STYLE, XML_STYLE, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_STRING, CTF_SC_CELLSTYLE ),
    MAP( "ConditionalFormatXML", XML_NAMESPACE_STYLE, XML_MAP, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_STRING|MID_FLAG_SPECIAL_ITEM, CTF_SC_IMPORT_MAP ),
    MAP( "ConditionalFormatXML", XML_NAMESPACE_STYLE, XML_MAP, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_STRING|MID_FLAG_SPECIAL_ITEM, CTF_SC_MAP ),
    MAP( "DiagonalBLTR", XML_NAMESPACE_STYLE, XML_DIAGONAL_BL_TR, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER, CTF_SC_DIAGONALBLTR ),
    MAP( "DiagonalBLTR", XML_NAMESPACE_STYLE, XML_DIAGONAL_BL_TR_WIDTH, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER_WIDTH, CTF_SC_DIAGONALBLTRWIDTH ), // #i102690# for old files
    MAP( "DiagonalBLTR", XML_NAMESPACE_STYLE, XML_DIAGONAL_BL_TR_WIDTHS, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER_WIDTH, CTF_SC_DIAGONALBLTRWIDTHS ),
    MAP( "DiagonalTLBR", XML_NAMESPACE_STYLE, XML_DIAGONAL_TL_BR, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER, CTF_SC_DIAGONALTLBR ),
    MAP( "DiagonalTLBR", XML_NAMESPACE_STYLE, XML_DIAGONAL_TL_BR_WIDTH, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER_WIDTH, CTF_SC_DIAGONALTLBRWIDTH ), // #i102690# for old files
    MAP( "DiagonalTLBR", XML_NAMESPACE_STYLE, XML_DIAGONAL_TL_BR_WIDTHS, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER_WIDTH, CTF_SC_DIAGONALTLBRWIDTHS ),
    MAP( "HoriJustify", XML_NAMESPACE_FO, XML_TEXT_ALIGN, XML_TYPE_PROP_PARAGRAPH|XML_SC_TYPE_HORIJUSTIFY|MID_FLAG_MERGE_PROPERTY, 0 ),
    MAP( "HoriJustify", XML_NAMESPACE_STYLE, XML_TEXT_ALIGN_SOURCE, XML_TYPE_PROP_TABLE_CELL|XML_SC_TYPE_HORIJUSTIFYSOURCE|MID_FLAG_MERGE_PROPERTY, 0 ),
    MAP( "HoriJustify", XML_NAMESPACE_STYLE, XML_REPEAT_CONTENT, XML_TYPE_PROP_TABLE_CELL|XML_SC_TYPE_HORIJUSTIFYREPEAT|MID_FLAG_MERGE_PROPERTY, 0 ),
    MAP_EXT( SC_UNONAME_CELLHJUS_METHOD, XML_NAMESPACE_CSS3TEXT, XML_TEXT_JUSTIFY, XML_TYPE_PROP_PARAGRAPH|XML_SC_TYPE_HORIJUSTIFY_METHOD, 0 ),
    MAP( "IsCellBackgroundTransparent", XML_NAMESPACE_FO, XML_BACKGROUND_COLOR, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_ISTRANSPARENT|MID_FLAG_MULTI_PROPERTY|MID_FLAG_MERGE_ATTRIBUTE, 0 ),
    MAP( "IsTextWrapped", XML_NAMESPACE_FO, XML_WRAP_OPTION, XML_TYPE_PROP_TABLE_CELL|XML_SC_ISTEXTWRAPPED, 0 ),
    MAP( "LeftBorder", XML_NAMESPACE_FO, XML_BORDER, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER, CTF_SC_ALLBORDER ),
    MAP( "LeftBorder", XML_NAMESPACE_FO, XML_BORDER_LEFT, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER, CTF_SC_LEFTBORDER ),
    MAP( "LeftBorder", XML_NAMESPACE_STYLE, XML_BORDER_LINE_WIDTH, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER_WIDTH, CTF_SC_ALLBORDERWIDTH ),
    MAP( "LeftBorder", XML_NAMESPACE_STYLE, XML_BORDER_LINE_WIDTH_LEFT, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER_WIDTH, CTF_SC_LEFTBORDERWIDTH ),
    MAP( "NumberFormat", XML_NAMESPACE_STYLE, XML_DATA_STYLE_NAME, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_NUMBER|MID_FLAG_SPECIAL_ITEM, CTF_SC_NUMBERFORMAT),
    MAP( "Orientation", XML_NAMESPACE_STYLE, XML_DIRECTION, XML_TYPE_PROP_TABLE_CELL|XML_SC_TYPE_ORIENTATION, 0 ),
    MAP( "ParaBottomMargin", XML_NAMESPACE_FO, XML_PADDING, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_MEASURE, CTF_SC_ALLPADDING ),
    MAP( "ParaBottomMargin", XML_NAMESPACE_FO, XML_PADDING_BOTTOM, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_MEASURE, CTF_SC_BOTTOMPADDING ),
    MAP( "ParaIndent", XML_NAMESPACE_FO, XML_MARGIN_LEFT, XML_TYPE_PROP_PARAGRAPH|XML_TYPE_MEASURE16, 0 ),
    MAP( "ParaLeftMargin", XML_NAMESPACE_FO, XML_PADDING_LEFT, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_MEASURE, CTF_SC_LEFTPADDING ),
    MAP( "ParaRightMargin", XML_NAMESPACE_FO, XML_PADDING_RIGHT, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_MEASURE, CTF_SC_RIGHTPADDING ),
    MAP( "ParaTopMargin", XML_NAMESPACE_FO, XML_PADDING_TOP, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_MEASURE, CTF_SC_TOPPADDING ),
    MAP( "RightBorder", XML_NAMESPACE_FO, XML_BORDER_RIGHT, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER, CTF_SC_RIGHTBORDER ),
    MAP( "RightBorder", XML_NAMESPACE_STYLE, XML_BORDER_LINE_WIDTH_RIGHT, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER_WIDTH, CTF_SC_RIGHTBORDERWIDTH ),
    MAP( "RotateAngle", XML_NAMESPACE_STYLE, XML_ROTATION_ANGLE, XML_TYPE_PROP_TABLE_CELL|XML_SC_TYPE_ROTATEANGLE, 0 ),
    MAP( "RotateReference", XML_NAMESPACE_STYLE, XML_ROTATION_ALIGN, XML_TYPE_PROP_TABLE_CELL|XML_SC_TYPE_ROTATEREFERENCE, 0),
    MAP( "ShadowFormat", XML_NAMESPACE_STYLE, XML_SHADOW, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_TEXT_SHADOW, 0 ),
    MAP( "ShrinkToFit", XML_NAMESPACE_STYLE, XML_SHRINK_TO_FIT, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BOOL, 0 ),
    MAP( "StandardDecimals", XML_NAMESPACE_STYLE, XML_DECIMAL_PLACES, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_NUMBER16, 0 ),
    MAP( "TopBorder", XML_NAMESPACE_FO, XML_BORDER_TOP, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER, CTF_SC_TOPBORDER ),
    MAP( "TopBorder", XML_NAMESPACE_STYLE, XML_BORDER_LINE_WIDTH_TOP, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BORDER_WIDTH, CTF_SC_TOPBORDERWIDTH ),
    MAP( "UserDefinedAttributes", XML_NAMESPACE_TEXT, XML_XMLNS, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_ATTRIBUTE_CONTAINER | MID_FLAG_SPECIAL_ITEM, 0 ),
    MAP( "ValidationXML", XML_NAMESPACE_TABLE, XML_CONTENT_VALIDATION, XML_TYPE_PROP_TABLE_CELL|XML_TYPE_BUILDIN_CMP_ONLY, CTF_SC_VALIDATION ),
    MAP( "VertJustify", XML_NAMESPACE_STYLE, XML_VERTICAL_ALIGN, XML_TYPE_PROP_TABLE_CELL|XML_SC_TYPE_VERTJUSTIFY, 0),
    MAP_EXT( SC_UNONAME_CELLVJUS_METHOD, XML_NAMESPACE_STYLE, XML_VERTICAL_JUSTIFY, XML_TYPE_PROP_TABLE_CELL|XML_SC_TYPE_VERTJUSTIFY_METHOD, 0 ),

    MAP_END()
};

const XMLPropertyMapEntry aXMLScColumnStylesProperties[] =
{
    MAP( "IsManualPageBreak", XML_NAMESPACE_FO, XML_BREAK_BEFORE, XML_TYPE_PROP_TABLE_COLUMN|XML_SC_TYPE_BREAKBEFORE, 0),
    MAP( "IsVisible", XML_NAMESPACE_TABLE, XML_DISPLAY, XML_TYPE_PROP_TABLE_COLUMN|XML_SC_TYPE_EQUAL|MID_FLAG_SPECIAL_ITEM, CTF_SC_ISVISIBLE ),
    MAP( "Width", XML_NAMESPACE_STYLE, XML_COLUMN_WIDTH, XML_TYPE_PROP_TABLE_COLUMN|XML_TYPE_MEASURE, 0 ),
    MAP_END()
};

const XMLPropertyMapEntry aXMLScRowStylesImportProperties[] =
{
    // #i57867# Include background color (CellBackColor/IsCellBackgroundTransparent) for import only.
    // Import and export should use the same map, with MID_FLAG_NO_PROPERTY_EXPORT for the background entries,
    // but this doesn't work at the moment because SvXMLImportPropertyMapper compares MID_FLAG_NO_PROPERTY to 0.
    // If this is changed (not for 2.0.x), a single map can be used again.

    MAP( "CellBackColor", XML_NAMESPACE_FO, XML_BACKGROUND_COLOR, XML_TYPE_PROP_TABLE_ROW|XML_TYPE_COLORTRANSPARENT|MID_FLAG_MULTI_PROPERTY|MID_FLAG_MERGE_ATTRIBUTE, 0 ),
    MAP( "Height", XML_NAMESPACE_STYLE, XML_ROW_HEIGHT, XML_TYPE_PROP_TABLE_ROW|XML_TYPE_MEASURE, CTF_SC_ROWHEIGHT),
    MAP( "IsCellBackgroundTransparent", XML_NAMESPACE_FO, XML_BACKGROUND_COLOR, XML_TYPE_PROP_TABLE_ROW|XML_TYPE_ISTRANSPARENT|MID_FLAG_MULTI_PROPERTY|MID_FLAG_MERGE_ATTRIBUTE, 0 ),
    MAP( "IsManualPageBreak", XML_NAMESPACE_FO, XML_BREAK_BEFORE, XML_TYPE_PROP_TABLE_ROW|XML_SC_TYPE_BREAKBEFORE, CTF_SC_ROWBREAKBEFORE),
    MAP( "OptimalHeight", XML_NAMESPACE_STYLE, XML_USE_OPTIMAL_ROW_HEIGHT, XML_TYPE_PROP_TABLE_ROW|XML_TYPE_BOOL, CTF_SC_ROWOPTIMALHEIGHT),
    MAP_END()
};

const XMLPropertyMapEntry aXMLScRowStylesProperties[] =
{
    MAP( "Height", XML_NAMESPACE_STYLE, XML_ROW_HEIGHT, XML_TYPE_PROP_TABLE_ROW|XML_TYPE_MEASURE, CTF_SC_ROWHEIGHT),
    MAP( "IsManualPageBreak", XML_NAMESPACE_FO, XML_BREAK_BEFORE, XML_TYPE_PROP_TABLE_ROW|XML_SC_TYPE_BREAKBEFORE, CTF_SC_ROWBREAKBEFORE),
    MAP( "OptimalHeight", XML_NAMESPACE_STYLE, XML_USE_OPTIMAL_ROW_HEIGHT, XML_TYPE_PROP_TABLE_ROW|XML_TYPE_BOOL, CTF_SC_ROWOPTIMALHEIGHT),
    MAP_END()
};

const XMLPropertyMapEntry aXMLScFromXLSRowStylesProperties[] =
{
    MAP( "Height", XML_NAMESPACE_STYLE, XML_ROW_HEIGHT, XML_TYPE_PROP_TABLE_ROW|XML_TYPE_MEASURE, CTF_SC_ROWHEIGHT),
    MAP( "IsManualPageBreak", XML_NAMESPACE_FO, XML_BREAK_BEFORE, XML_TYPE_PROP_TABLE_ROW|XML_SC_TYPE_BREAKBEFORE, CTF_SC_ROWBREAKBEFORE),
    MAP( "OptimalHeight", XML_NAMESPACE_STYLE, XML_USE_OPTIMAL_ROW_HEIGHT, XML_TYPE_PROP_TABLE_ROW|XML_TYPE_BOOL_FALSE, CTF_SC_ROWOPTIMALHEIGHT),
    MAP_END()
};

const XMLPropertyMapEntry aXMLScTableStylesImportProperties[] =
{
    // #i57869# Include background color (CellBackColor/IsCellBackgroundTransparent) for import only.
    // Import and export should use the same map, with MID_FLAG_NO_PROPERTY_EXPORT for the background entries,
    // but this doesn't work at the moment because SvXMLImportPropertyMapper compares MID_FLAG_NO_PROPERTY to 0.
    // If this is changed (not for 2.0.x), a single map can be used again.

    MAP( "CellBackColor", XML_NAMESPACE_FO, XML_BACKGROUND_COLOR, XML_TYPE_PROP_TABLE|XML_TYPE_COLORTRANSPARENT|MID_FLAG_MULTI_PROPERTY|MID_FLAG_MERGE_ATTRIBUTE, 0 ),
    MAP( "IsCellBackgroundTransparent", XML_NAMESPACE_FO, XML_BACKGROUND_COLOR, XML_TYPE_PROP_TABLE|XML_TYPE_ISTRANSPARENT|MID_FLAG_MULTI_PROPERTY|MID_FLAG_MERGE_ATTRIBUTE, 0 ),
    MAP( "IsVisible", XML_NAMESPACE_TABLE, XML_DISPLAY, XML_TYPE_PROP_TABLE|XML_TYPE_BOOL, 0 ),
    MAP( "PageStyle", XML_NAMESPACE_STYLE, XML_MASTER_PAGE_NAME, XML_TYPE_PROP_TABLE|XML_TYPE_STRING|MID_FLAG_SPECIAL_ITEM, CTF_SC_MASTERPAGENAME ),
    MAP( "TableLayout", XML_NAMESPACE_STYLE, XML_WRITING_MODE, XML_TYPE_PROP_TABLE|XML_TYPE_TEXT_WRITING_MODE, 0 ),
    MAP( "TabColor", XML_NAMESPACE_TABLE, XML_TAB_COLOR, XML_TYPE_PROP_TABLE|XML_TYPE_COLORAUTO, 0 ),
    MAP_EXT( "TabColor", XML_NAMESPACE_TABLE_EXT, XML_TAB_COLOR, XML_TYPE_PROP_TABLE|XML_TYPE_COLORAUTO, 0 ),
    MAP_END()
};

const XMLPropertyMapEntry aXMLScTableStylesProperties[] =
{
    MAP( "IsVisible", XML_NAMESPACE_TABLE, XML_DISPLAY, XML_TYPE_PROP_TABLE|XML_TYPE_BOOL, 0 ),
    MAP( "PageStyle", XML_NAMESPACE_STYLE, XML_MASTER_PAGE_NAME, XML_TYPE_PROP_TABLE|XML_TYPE_STRING|MID_FLAG_SPECIAL_ITEM, CTF_SC_MASTERPAGENAME ),
    MAP( "TableLayout", XML_NAMESPACE_STYLE, XML_WRITING_MODE, XML_TYPE_PROP_TABLE|XML_TYPE_TEXT_WRITING_MODE, 0 ),
    MAP_EXT( "TabColor", XML_NAMESPACE_TABLE_EXT, XML_TAB_COLOR, XML_TYPE_PROP_TABLE|XML_TYPE_COLORAUTO, 0 ),
    MAP_END()
};

ScXMLCellExportPropertyMapper::ScXMLCellExportPropertyMapper(
            const UniReference< XMLPropertySetMapper >& rMapper )
            : SvXMLExportPropertyMapper(rMapper)
{
}

ScXMLCellExportPropertyMapper::~ScXMLCellExportPropertyMapper()
{
}

void ScXMLCellExportPropertyMapper::ContextFilter(
    ::std::vector< XMLPropertyState >& rProperties,
    uno::Reference< beans::XPropertySet > rPropSet ) const
{
    XMLPropertyState* pPadding = NULL;
    XMLPropertyState* pPadding_Bottom = NULL;
    XMLPropertyState* pPadding_Left = NULL;
    XMLPropertyState* pPadding_Right = NULL;
    XMLPropertyState* pPadding_Top = NULL;

    XMLPropertyState* pBorder = NULL;
    XMLPropertyState* pBorder_Bottom = NULL;
    XMLPropertyState* pBorder_Left = NULL;
    XMLPropertyState* pBorder_Right = NULL;
    XMLPropertyState* pBorder_Top = NULL;
    XMLPropertyState* pSWBorder = NULL;
    XMLPropertyState* pSWBorder_Bottom = NULL;
    XMLPropertyState* pSWBorder_Left = NULL;
    XMLPropertyState* pSWBorder_Right = NULL;
    XMLPropertyState* pSWBorder_Top = NULL;

    XMLPropertyState* pAllBorderWidthState = NULL;
    XMLPropertyState* pLeftBorderWidthState = NULL;
    XMLPropertyState* pRightBorderWidthState = NULL;
    XMLPropertyState* pTopBorderWidthState = NULL;
    XMLPropertyState* pBottomBorderWidthState = NULL;
    XMLPropertyState* pSWAllBorderWidthState = NULL;
    XMLPropertyState* pSWLeftBorderWidthState = NULL;
    XMLPropertyState* pSWRightBorderWidthState = NULL;
    XMLPropertyState* pSWTopBorderWidthState = NULL;
    XMLPropertyState* pSWBottomBorderWidthState = NULL;
    XMLPropertyState* pDiagonalTLBRWidthState = NULL;
    XMLPropertyState* pDiagonalBLTRWidthState = NULL;

    XMLPropertyState* pParaMarginLeft = NULL;
    XMLPropertyState* pParaMarginLeftRel = NULL;
    XMLPropertyState* pParaMarginRight = NULL;
    XMLPropertyState* pParaMarginRightRel = NULL;
    XMLPropertyState* pParaMarginTop = NULL;
    XMLPropertyState* pParaMarginTopRel = NULL;
    XMLPropertyState* pParaMarginBottom = NULL;
    XMLPropertyState* pParaMarginBottomRel = NULL;

    XMLPropertyState* pParaAdjust = NULL;
    XMLPropertyState* pParaAdjustLast = NULL;

    ::std::vector< XMLPropertyState >::iterator aEndIter(rProperties.end());
    for( ::std::vector< XMLPropertyState >::iterator aIter = rProperties.begin();
         aIter != aEndIter; ++aIter )
    {
        XMLPropertyState* propertie = &(*aIter);
        if (propertie->mnIndex != -1)
        {
            switch( getPropertySetMapper()->GetEntryContextId( propertie->mnIndex ) )
            {
                case CTF_SC_ALLPADDING:         pPadding = propertie; break;
                case CTF_SC_BOTTOMPADDING:      pPadding_Bottom = propertie; break;
                case CTF_SC_LEFTPADDING:        pPadding_Left = propertie; break;
                case CTF_SC_RIGHTPADDING:       pPadding_Right = propertie; break;
                case CTF_SC_TOPPADDING:         pPadding_Top = propertie; break;
                case CTF_SC_ALLBORDER:          pBorder = propertie; break;
                case CTF_SC_LEFTBORDER:         pBorder_Left = propertie; break;
                case CTF_SC_RIGHTBORDER:        pBorder_Right = propertie; break;
                case CTF_SC_BOTTOMBORDER:       pBorder_Bottom = propertie; break;
                case CTF_SC_TOPBORDER:          pBorder_Top = propertie; break;
                case CTF_SC_ALLBORDERWIDTH:     pAllBorderWidthState = propertie; break;
                case CTF_SC_LEFTBORDERWIDTH:    pLeftBorderWidthState = propertie; break;
                case CTF_SC_RIGHTBORDERWIDTH:   pRightBorderWidthState = propertie; break;
                case CTF_SC_TOPBORDERWIDTH:     pTopBorderWidthState = propertie; break;
                case CTF_SC_BOTTOMBORDERWIDTH:  pBottomBorderWidthState = propertie; break;
                case CTF_ALLBORDER:             pSWBorder = propertie; break;
                case CTF_LEFTBORDER:            pSWBorder_Left = propertie; break;
                case CTF_RIGHTBORDER:           pSWBorder_Right = propertie; break;
                case CTF_BOTTOMBORDER:          pSWBorder_Bottom = propertie; break;
                case CTF_TOPBORDER:             pSWBorder_Top = propertie; break;
                case CTF_ALLBORDERWIDTH:        pSWAllBorderWidthState = propertie; break;
                case CTF_LEFTBORDERWIDTH:       pSWLeftBorderWidthState = propertie; break;
                case CTF_RIGHTBORDERWIDTH:      pSWRightBorderWidthState = propertie; break;
                case CTF_TOPBORDERWIDTH:        pSWTopBorderWidthState = propertie; break;
                case CTF_BOTTOMBORDERWIDTH:     pSWBottomBorderWidthState = propertie; break;
                case CTF_SC_DIAGONALTLBR:       break; //old diagonal line attribute names without "s" are only read, not written
                case CTF_SC_DIAGONALTLBRWIDTH:  pDiagonalTLBRWidthState = propertie; break;
                case CTF_SC_DIAGONALBLTR:       break; //old diagonal line attribute names without "s" are only read, not written
                case CTF_SC_DIAGONALBLTRWIDTH:  pDiagonalBLTRWidthState = propertie; break;
                case CTF_SD_SHAPE_PARA_ADJUST:  pParaAdjust = propertie; break;
                case CTF_PARA_ADJUSTLAST:       pParaAdjustLast = propertie; break;
                case CTF_PARALEFTMARGIN:        pParaMarginLeft = propertie; break;
                case CTF_PARALEFTMARGIN_REL:    pParaMarginLeftRel = propertie; break;
                case CTF_PARARIGHTMARGIN:       pParaMarginRight = propertie; break;
                case CTF_PARARIGHTMARGIN_REL:   pParaMarginRightRel = propertie; break;
                case CTF_PARATOPMARGIN:         pParaMarginTop = propertie; break;
                case CTF_PARATOPMARGIN_REL:     pParaMarginTopRel = propertie; break;
                case CTF_PARABOTTOMMARGIN:      pParaMarginBottom = propertie; break;
                case CTF_PARABOTTOMMARGIN_REL:  pParaMarginBottomRel = propertie; break;
            }
        }
    }

    if (pPadding && pPadding_Bottom && pPadding_Left && pPadding_Right && pPadding_Top)
    {
        sal_Int32 nBottom = 0, nTop = 0, nLeft = 0, nRight = 0;
        if ((pPadding_Bottom->maValue >>= nBottom) &&
            (pPadding_Left->maValue >>= nLeft) &&
            (pPadding_Right->maValue >>= nRight) &&
            (pPadding_Top->maValue >>= nTop))
        {
            if ((nBottom == nTop) && (nLeft == nRight) && (nTop == nLeft))
            {
                pPadding_Bottom->mnIndex = -1;
                pPadding_Bottom->maValue.clear();
                pPadding_Left->mnIndex = -1;
                pPadding_Left->maValue.clear();
                pPadding_Right->mnIndex = -1;
                pPadding_Right->maValue.clear();
                pPadding_Top->mnIndex = -1;
                pPadding_Top->maValue.clear();
            }
            else
            {
                pPadding->mnIndex = -1;
                pPadding->maValue.clear();
            }
        }
    }
    if( pBorder )
    {
        if( pBorder_Left && pBorder_Right && pBorder_Top && pBorder_Bottom )
        {
            table::BorderLine2 aLeft, aRight, aTop, aBottom;

            pBorder_Left->maValue >>= aLeft;
            pBorder_Right->maValue >>= aRight;
            pBorder_Top->maValue >>= aTop;
            pBorder_Bottom->maValue >>= aBottom;
            if( aLeft.Color == aRight.Color && aLeft.InnerLineWidth == aRight.InnerLineWidth &&
                aLeft.OuterLineWidth == aRight.OuterLineWidth && aLeft.LineDistance == aRight.LineDistance &&
                aLeft.Color == aTop.Color && aLeft.InnerLineWidth == aTop.InnerLineWidth &&
                aLeft.OuterLineWidth == aTop.OuterLineWidth && aLeft.LineDistance == aTop.LineDistance &&
                aLeft.Color == aBottom.Color && aLeft.InnerLineWidth == aBottom.InnerLineWidth &&
                aLeft.OuterLineWidth == aBottom.OuterLineWidth && aLeft.LineDistance == aBottom.LineDistance &&
                aLeft.LineStyle == aRight.LineStyle && aLeft.LineStyle == aTop.LineStyle &&
                aLeft.LineStyle == aBottom.LineStyle && aLeft.LineWidth == aRight.LineWidth &&
                aLeft.LineWidth == aTop.LineWidth && aLeft.LineWidth == aBottom.LineWidth )
            {
                pBorder_Left->mnIndex = -1;
                pBorder_Left->maValue.clear();
                pBorder_Right->mnIndex = -1;
                pBorder_Right->maValue.clear();
                pBorder_Top->mnIndex = -1;
                pBorder_Top->maValue.clear();
                pBorder_Bottom->mnIndex = -1;
                pBorder_Bottom->maValue.clear();
            }
            else
            {
                pBorder->mnIndex = -1;
                pBorder->maValue.clear();
            }
        }
        else
        {
            pBorder->mnIndex = -1;
            pBorder->maValue.clear();
        }
    }
    if( pAllBorderWidthState )
    {
        if( pLeftBorderWidthState && pRightBorderWidthState && pTopBorderWidthState && pBottomBorderWidthState )
        {
            table::BorderLine2 aLeft, aRight, aTop, aBottom;

            pLeftBorderWidthState->maValue >>= aLeft;
            pRightBorderWidthState->maValue >>= aRight;
            pTopBorderWidthState->maValue >>= aTop;
            pBottomBorderWidthState->maValue >>= aBottom;
            if( aLeft.InnerLineWidth == aRight.InnerLineWidth && aLeft.OuterLineWidth == aRight.OuterLineWidth &&
                aLeft.LineDistance == aRight.LineDistance && aLeft.InnerLineWidth == aTop.InnerLineWidth &&
                aLeft.OuterLineWidth == aTop.OuterLineWidth && aLeft.LineDistance == aTop.LineDistance &&
                aLeft.InnerLineWidth == aBottom.InnerLineWidth && aLeft.OuterLineWidth == aBottom.OuterLineWidth &&
                aLeft.LineDistance == aBottom.LineDistance && aLeft.LineWidth == aRight.LineWidth &&
                aLeft.LineWidth == aTop.LineWidth && aLeft.LineWidth == aBottom.LineWidth )
            {
                pLeftBorderWidthState->mnIndex = -1;
                pLeftBorderWidthState->maValue.clear();
                pRightBorderWidthState->mnIndex = -1;
                pRightBorderWidthState->maValue.clear();
                pTopBorderWidthState->mnIndex = -1;
                pTopBorderWidthState->maValue.clear();
                pBottomBorderWidthState->mnIndex = -1;
                pBottomBorderWidthState->maValue.clear();
            }
            else
            {
                pAllBorderWidthState->mnIndex = -1;
                pAllBorderWidthState->maValue.clear();
            }
        }
        else
        {
            pAllBorderWidthState->mnIndex = -1;
            pAllBorderWidthState->maValue.clear();
        }
    }

    if (pParaAdjust)
    {
        pParaAdjust->mnIndex = -1;
        pParaAdjust->maValue.clear();
    }
    if (pParaAdjustLast)
    {
        pParaAdjustLast->mnIndex = -1;
        pParaAdjustLast->maValue.clear();
    }
    if (pSWBorder)
    {
        pSWBorder->mnIndex = -1;
        pSWBorder->maValue.clear();
    }
    if (pSWBorder_Left)
    {
        pSWBorder_Left->mnIndex = -1;
        pSWBorder_Left->maValue.clear();
    }
    if (pSWBorder_Right)
    {
        pSWBorder_Right->mnIndex = -1;
        pSWBorder_Right->maValue.clear();
    }
    if (pSWBorder_Bottom)
    {
        pSWBorder_Bottom->mnIndex = -1;
        pSWBorder_Bottom->maValue.clear();
    }
    if (pSWBorder_Top)
    {
        pSWBorder_Top->mnIndex = -1;
        pSWBorder_Top->maValue.clear();
    }
    if (pSWAllBorderWidthState)
    {
        pSWAllBorderWidthState->mnIndex = -1;
        pSWAllBorderWidthState->maValue.clear();
    }
    if (pSWLeftBorderWidthState)
    {
        pSWLeftBorderWidthState->mnIndex = -1;
        pSWLeftBorderWidthState->maValue.clear();
    }
    if (pSWRightBorderWidthState)
    {
        pSWRightBorderWidthState->mnIndex = -1;
        pSWRightBorderWidthState->maValue.clear();
    }
    if (pSWTopBorderWidthState)
    {
        pSWTopBorderWidthState->mnIndex = -1;
        pSWTopBorderWidthState->maValue.clear();
    }
    if (pSWBottomBorderWidthState)
    {
        pSWBottomBorderWidthState->mnIndex = -1;
        pSWBottomBorderWidthState->maValue.clear();
    }

    if (pParaMarginLeft)
    {
        pParaMarginLeft->mnIndex = -1;
        pParaMarginLeft->maValue.clear();
    }
    if (pParaMarginLeftRel)
    {
        pParaMarginLeftRel->mnIndex = -1;
        pParaMarginLeftRel->maValue.clear();
    }
    if (pParaMarginRight)
    {
        pParaMarginRight->mnIndex = -1;
        pParaMarginRight->maValue.clear();
    }
    if (pParaMarginRightRel)
    {
        pParaMarginRightRel->mnIndex = -1;
        pParaMarginRightRel->maValue.clear();
    }
    if (pParaMarginTop)
    {
        pParaMarginTop->mnIndex = -1;
        pParaMarginTop->maValue.clear();
    }
    if (pParaMarginTopRel)
    {
        pParaMarginTopRel->mnIndex = -1;
        pParaMarginTopRel->maValue.clear();
    }
    if (pParaMarginBottom)
    {
        pParaMarginBottom->mnIndex = -1;
        pParaMarginBottom->maValue.clear();
    }
    if (pParaMarginBottomRel)
    {
        pParaMarginBottomRel->mnIndex = -1;
        pParaMarginBottomRel->maValue.clear();
    }

    // #i102690# old diagonal line attribute names without "s" are only read, not written
    if (pDiagonalTLBRWidthState)
    {
        pDiagonalTLBRWidthState->mnIndex = -1;
        pDiagonalTLBRWidthState->maValue.clear();
    }
    if (pDiagonalBLTRWidthState)
    {
        pDiagonalBLTRWidthState->mnIndex = -1;
        pDiagonalBLTRWidthState->maValue.clear();
    }

    SvXMLExportPropertyMapper::ContextFilter(rProperties, rPropSet);
}

/** this method is called for every item that has the MID_FLAG_SPECIAL_ITEM_EXPORT flag set */
void ScXMLCellExportPropertyMapper::handleSpecialItem(
            SvXMLAttributeList& /* rAttrList */,
            const XMLPropertyState& /* rProperty */,
            const SvXMLUnitConverter& /* rUnitConverter */,
            const SvXMLNamespaceMap& /* rNamespaceMap */,
            const ::std::vector< XMLPropertyState > * /* pProperties */,
            sal_uInt32 /* nIdx */ ) const
{
    // the SpecialItem NumberFormat must not be handled by this method
    // the SpecialItem ConditionlaFormat must not be handled by this method
    // the SpecialItem CharBackColor must not be handled by this method
}

ScXMLRowExportPropertyMapper::ScXMLRowExportPropertyMapper(
            const UniReference< XMLPropertySetMapper >& rMapper )
            : SvXMLExportPropertyMapper(rMapper)
{
}

ScXMLRowExportPropertyMapper::~ScXMLRowExportPropertyMapper()
{
}

void ScXMLRowExportPropertyMapper::ContextFilter(
    ::std::vector< XMLPropertyState >& /* rProperties */,
    uno::Reference< beans::XPropertySet > /* rPropSet */ ) const
{
    //#108550#; don't filter the height, so other applications know the calculated height
}

ScXMLColumnExportPropertyMapper::ScXMLColumnExportPropertyMapper(
            const UniReference< XMLPropertySetMapper >& rMapper )
            : SvXMLExportPropertyMapper(rMapper)
{
}

ScXMLColumnExportPropertyMapper::~ScXMLColumnExportPropertyMapper()
{
}

/** this method is called for every item that has the MID_FLAG_SPECIAL_ITEM_EXPORT flag set */
void ScXMLColumnExportPropertyMapper::handleSpecialItem(
            SvXMLAttributeList& /* rAttrList */,
            const XMLPropertyState& /* rProperty */,
            const SvXMLUnitConverter& /* rUnitConverter */,
            const SvXMLNamespaceMap& /* rNamespaceMap */,
            const ::std::vector< XMLPropertyState > * /* pProperties */,
            sal_uInt32 /* nIdx */ ) const
{
    // the SpecialItem IsVisible must not be handled by this method
}

ScXMLTableExportPropertyMapper::ScXMLTableExportPropertyMapper(
            const UniReference< XMLPropertySetMapper >& rMapper )
            : SvXMLExportPropertyMapper(rMapper)
{
}

ScXMLTableExportPropertyMapper::~ScXMLTableExportPropertyMapper()
{
}

/** this method is called for every item that has the MID_FLAG_SPECIAL_ITEM_EXPORT flag set */
void ScXMLTableExportPropertyMapper::handleSpecialItem(
            SvXMLAttributeList& /* rAttrList */,
            const XMLPropertyState& /* rProperty */,
            const SvXMLUnitConverter& /* rUnitConverter */,
            const SvXMLNamespaceMap& /* rNamespaceMap */,
            const ::std::vector< XMLPropertyState > * /* pProperties */,
            sal_uInt32 /* nIdx */ ) const
{
    // the SpecialItem PageStyle must not be handled by this method
}

void ScXMLAutoStylePoolP::exportStyleAttributes(
            SvXMLAttributeList& rAttrList,
            sal_Int32 nFamily,
            const ::std::vector< XMLPropertyState >& rProperties,
            const SvXMLExportPropertyMapper& rPropExp
            , const SvXMLUnitConverter& rUnitConverter,
            const SvXMLNamespaceMap& rNamespaceMap
            ) const
{
    SvXMLAutoStylePoolP::exportStyleAttributes( rAttrList, nFamily, rProperties, rPropExp, rUnitConverter, rNamespaceMap );
    if (nFamily == XML_STYLE_FAMILY_TABLE_CELL)
    {
        ::std::vector< XMLPropertyState >::const_iterator i(rProperties.begin());
        ::std::vector< XMLPropertyState >::const_iterator endi(rProperties.end());
        while (i != endi)
        {
            UniReference< XMLPropertySetMapper > aPropMapper(rScXMLExport.GetCellStylesPropertySetMapper());
            sal_Int16 nContextID(aPropMapper->GetEntryContextId(i->mnIndex));
            switch (nContextID)
            {
                case CTF_SC_NUMBERFORMAT :
                {
                    sal_Int32 nNumberFormat = 0;
                    if (i->maValue >>= nNumberFormat)
                    {
                        rtl::OUString sAttrValue(rScXMLExport.getDataStyleName(nNumberFormat));
                        if (!sAttrValue.isEmpty())
                        {
                            GetExport().AddAttribute(
                                aPropMapper->GetEntryNameSpace(i->mnIndex),
                                aPropMapper->GetEntryXMLName(i->mnIndex),
                                sAttrValue );
                        }
                    }
                }
                break;
            }
            ++i;
        }
    }
    else if (nFamily == XML_STYLE_FAMILY_TABLE_TABLE)
    {
        ::std::vector< XMLPropertyState >::const_iterator i(rProperties.begin());
        ::std::vector< XMLPropertyState >::const_iterator endi(rProperties.end());
        while(i != endi)
        {
            UniReference< XMLPropertySetMapper > aPropMapper(rScXMLExport.GetTableStylesPropertySetMapper());
            sal_Int16 nContextID(aPropMapper->GetEntryContextId(i->mnIndex));
            switch (nContextID)
            {
                case CTF_SC_MASTERPAGENAME :
                {
                    rtl::OUString sName;
                    if (i->maValue >>= sName)
                    {
                        GetExport().AddAttribute(
                            aPropMapper->GetEntryNameSpace(i->mnIndex),
                            aPropMapper->GetEntryXMLName(i->mnIndex),
                            GetExport().EncodeStyleName( sName ));
                    }
                }
                break;
            }
            ++i;
        }
    }
}

void ScXMLAutoStylePoolP::exportStyleContent(
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
        sal_Int32 nFamily,
        const std::vector< XMLPropertyState >& rProperties,
        const SvXMLExportPropertyMapper& rPropExp
        , const SvXMLUnitConverter& rUnitConverter,
        const SvXMLNamespaceMap& rNamespaceMap
        ) const
{
    SvXMLAutoStylePoolP::exportStyleContent( rHandler, nFamily, rProperties, rPropExp, rUnitConverter, rNamespaceMap );
    if (nFamily == XML_STYLE_FAMILY_TABLE_CELL)
    {
        sal_Bool bNotFound = sal_True;
        ::std::vector< XMLPropertyState >::const_iterator i(rProperties.begin());
        ::std::vector< XMLPropertyState >::const_iterator endi(rProperties.end());
        while ((i != endi) && bNotFound)
        {
            if (i->mnIndex != -1)
            {
                sal_Int16 nContextID = rScXMLExport.GetCellStylesPropertySetMapper()->GetEntryContextId(i->mnIndex);
                switch (nContextID)
                {
                    case CTF_SC_MAP :
                    {
                        uno::Reference<container::XIndexAccess> xIndex( i->maValue, uno::UNO_QUERY );
                           if ( xIndex.is() )
                        {
                            sal_Int32 nConditionCount(xIndex->getCount());
                            for (sal_Int32 nCondition = 0; nCondition < nConditionCount; ++nCondition)
                            {
                                uno::Reference <sheet::XSheetConditionalEntry> xSheetConditionalEntry(xIndex->getByIndex(nCondition), uno::UNO_QUERY);
                                if (xSheetConditionalEntry.is())
                                {
                                    rtl::OUString sStyleName(xSheetConditionalEntry->getStyleName());
                                    uno::Reference <sheet::XSheetCondition> xSheetCondition(xSheetConditionalEntry, uno::UNO_QUERY);
                                    if (xSheetCondition.is())
                                    {
                                        sheet::ConditionOperator aOperator = xSheetCondition->getOperator();
                                        if (aOperator != sheet::ConditionOperator_NONE)
                                        {
                                            if (aOperator == sheet::ConditionOperator_FORMULA)
                                            {
                                                rtl::OUString sCondition(RTL_CONSTASCII_USTRINGPARAM("is-true-formula("));
                                                sCondition += xSheetCondition->getFormula1();
                                                sCondition += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(")"));
                                                rScXMLExport.AddAttribute(XML_NAMESPACE_STYLE, XML_CONDITION, sCondition);
                                                rScXMLExport.AddAttribute(XML_NAMESPACE_STYLE, XML_APPLY_STYLE_NAME, rScXMLExport.EncodeStyleName( sStyleName ));
                                                OUString sOUBaseAddress;
                                                ScDocument* pDoc = rScXMLExport.GetDocument();
                                                ScRangeStringConverter::GetStringFromAddress( sOUBaseAddress,
                                                    xSheetCondition->getSourcePosition(), pDoc, FormulaGrammar::CONV_OOO );
                                                rScXMLExport.AddAttribute(XML_NAMESPACE_STYLE, XML_BASE_CELL_ADDRESS, sOUBaseAddress);
                                                SvXMLElementExport aMElem(rScXMLExport, XML_NAMESPACE_STYLE, XML_MAP, sal_True, sal_True);
                                            }
                                            else
                                            {
                                                rtl::OUString sCondition;
                                                if (aOperator == sheet::ConditionOperator_BETWEEN ||
                                                    aOperator == sheet::ConditionOperator_NOT_BETWEEN)
                                                {
                                                    if (aOperator == sheet::ConditionOperator_BETWEEN)
                                                        sCondition = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("cell-content-is-between("));
                                                    else
                                                        sCondition = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("cell-content-is-not-between("));
                                                    sCondition += xSheetCondition->getFormula1();
                                                    sCondition += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(","));
                                                    sCondition += xSheetCondition->getFormula2();
                                                    sCondition += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(")"));
                                                }
                                                else
                                                {
                                                    sCondition = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("cell-content()"));
                                                    switch (aOperator)
                                                    {
                                                        case sheet::ConditionOperator_LESS:
                                                            sCondition += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("<"));
                                                        break;
                                                        case sheet::ConditionOperator_GREATER:
                                                            sCondition += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(">"));
                                                        break;
                                                        case sheet::ConditionOperator_LESS_EQUAL:
                                                            sCondition += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("<="));
                                                        break;
                                                        case sheet::ConditionOperator_GREATER_EQUAL:
                                                            sCondition += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(">="));
                                                        break;
                                                        case sheet::ConditionOperator_EQUAL:
                                                            sCondition += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("="));
                                                        break;
                                                        case sheet::ConditionOperator_NOT_EQUAL:
                                                            sCondition += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("!="));
                                                        break;
                                                        default:
                                                        {
                                                            // added to avoid warnings
                                                        }
                                                    }
                                                    sCondition += xSheetCondition->getFormula1();
                                                }
                                                rScXMLExport.AddAttribute(XML_NAMESPACE_STYLE, XML_CONDITION, sCondition);
                                                rScXMLExport.AddAttribute(XML_NAMESPACE_STYLE, XML_APPLY_STYLE_NAME, rScXMLExport.EncodeStyleName( sStyleName ));
                                                OUString sOUBaseAddress;
                                                ScRangeStringConverter::GetStringFromAddress( sOUBaseAddress,
                                                    xSheetCondition->getSourcePosition(), rScXMLExport.GetDocument(), FormulaGrammar::CONV_OOO );
                                                rScXMLExport.AddAttribute(XML_NAMESPACE_STYLE, XML_BASE_CELL_ADDRESS, sOUBaseAddress);
                                                SvXMLElementExport aMElem(rScXMLExport, XML_NAMESPACE_STYLE, XML_MAP, sal_True, sal_True);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            }
            ++i;
        }
    }
}

ScXMLAutoStylePoolP::ScXMLAutoStylePoolP(ScXMLExport& rTempScXMLExport):
    SvXMLAutoStylePoolP(rTempScXMLExport),
    rScXMLExport(rTempScXMLExport)
{
}

ScXMLAutoStylePoolP::~ScXMLAutoStylePoolP()
{
}


void ScXMLStyleExport::exportStyleAttributes(
        const ::com::sun::star::uno::Reference<
                ::com::sun::star::style::XStyle > & rStyle )
{
    uno::Reference< beans::XPropertySet > xPropSet( rStyle, uno::UNO_QUERY );
    if (xPropSet.is())
    {
        uno::Reference< beans::XPropertySetInfo > xPropSetInfo(xPropSet->getPropertySetInfo());
        rtl::OUString sNumberFormat(RTL_CONSTASCII_USTRINGPARAM("NumberFormat"));
        if( xPropSetInfo->hasPropertyByName( sNumberFormat ) )
        {
            uno::Reference< beans::XPropertyState > xPropState( xPropSet, uno::UNO_QUERY );
            if( xPropState.is() && (beans::PropertyState_DIRECT_VALUE ==
                    xPropState->getPropertyState( sNumberFormat )) )
            {
                sal_Int32 nNumberFormat = 0;
                if (xPropSet->getPropertyValue( sNumberFormat ) >>= nNumberFormat)
                    GetExport().AddAttribute( XML_NAMESPACE_STYLE, XML_DATA_STYLE_NAME,
                                                        GetExport().getDataStyleName(nNumberFormat) );
            }
        }
    }
}

void ScXMLStyleExport::exportStyleContent(
        const ::com::sun::star::uno::Reference<
                ::com::sun::star::style::XStyle > & /* rStyle */ )
{
}

ScXMLStyleExport::ScXMLStyleExport(
        SvXMLExport& rExp,
        const ::rtl::OUString& rPoolStyleName,
        SvXMLAutoStylePoolP *pAutoStyleP )
    : XMLStyleExport(rExp, rPoolStyleName, pAutoStyleP)
{
}

ScXMLStyleExport::~ScXMLStyleExport()
{
}

XMLScPropHdlFactory::XMLScPropHdlFactory()
    : XMLPropertyHandlerFactory()
{
}

XMLScPropHdlFactory::~XMLScPropHdlFactory()
{
}

const XMLPropertyHandler* XMLScPropHdlFactory::GetPropertyHandler( sal_Int32 nType ) const
{
    nType &= MID_FLAG_MASK;

    XMLPropertyHandler* pHdl((XMLPropertyHandler*)XMLPropertyHandlerFactory::GetPropertyHandler( nType ));
    if(!pHdl)
    {
        switch(nType)
        {
            case XML_SC_TYPE_CELLPROTECTION :
            {
                pHdl = new XmlScPropHdl_CellProtection;
            }
            break;
            case XML_SC_TYPE_PRINTCONTENT :
            {
                pHdl = new XmlScPropHdl_PrintContent;
            }
            break;
            case XML_SC_TYPE_HORIJUSTIFY_METHOD:
            case XML_SC_TYPE_VERTJUSTIFY_METHOD:
            {
                pHdl = new XmlScPropHdl_JustifyMethod;
            }
            break;
            case XML_SC_TYPE_HORIJUSTIFY :
            {
                pHdl = new XmlScPropHdl_HoriJustify;
            }
            break;
            case XML_SC_TYPE_HORIJUSTIFYSOURCE :
            {
                pHdl = new XmlScPropHdl_HoriJustifySource;
            }
            break;
            case XML_SC_TYPE_HORIJUSTIFYREPEAT :
            {
                pHdl = new XmlScPropHdl_HoriJustifyRepeat;
            }
            break;
            case XML_SC_TYPE_ORIENTATION :
            {
                pHdl = new XmlScPropHdl_Orientation;
            }
            break;
            case XML_SC_TYPE_ROTATEANGLE :
            {
                pHdl = new XmlScPropHdl_RotateAngle;
            }
            break;
            case XML_SC_TYPE_ROTATEREFERENCE :
            {
                pHdl = new XmlScPropHdl_RotateReference;
            }
            break;
            case XML_SC_TYPE_VERTJUSTIFY :
            {
                pHdl = new XmlScPropHdl_VertJustify;
            }
            break;
            case XML_SC_TYPE_BREAKBEFORE :
            {
                pHdl = new XmlScPropHdl_BreakBefore;
            }
            break;
            case XML_SC_ISTEXTWRAPPED :
            {
                pHdl = new XmlScPropHdl_IsTextWrapped;
            }
            break;
            case XML_SC_TYPE_EQUAL :
            {
                pHdl = new XmlScPropHdl_IsEqual;
            }
            break;
            case XML_SC_TYPE_VERTICAL :
            {
                pHdl = new XmlScPropHdl_Vertical;
            }
            break;
        }

        if(pHdl)
            PutHdlCache(nType, pHdl);
    }

    return pHdl;
}

XmlScPropHdl_CellProtection::~XmlScPropHdl_CellProtection()
{
}

bool XmlScPropHdl_CellProtection::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    util::CellProtection aCellProtection1, aCellProtection2;

    if((r1 >>= aCellProtection1) && (r2 >>= aCellProtection2))
    {
        return ((aCellProtection1.IsHidden == aCellProtection2.IsHidden) &&
                (aCellProtection1.IsLocked == aCellProtection2.IsLocked) &&
                (aCellProtection1.IsFormulaHidden == aCellProtection2.IsFormulaHidden));
    }
    return false;
}

sal_Bool XmlScPropHdl_CellProtection::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    util::CellProtection aCellProtection;
    sal_Bool bDefault(false);
    if (!rValue.hasValue())
    {
        aCellProtection.IsHidden = false;
        aCellProtection.IsLocked = sal_True;
        aCellProtection.IsFormulaHidden = false;
        aCellProtection.IsPrintHidden = false;
        bDefault = sal_True;
    }
    if ((rValue >>= aCellProtection) || bDefault)
    {
        if (!IsXMLToken(rStrImpValue, XML_NONE))
        {
            if (!IsXMLToken(rStrImpValue, XML_HIDDEN_AND_PROTECTED))
            {
                if (!IsXMLToken(rStrImpValue, XML_PROTECTED))
                {
                    if (!IsXMLToken(rStrImpValue, XML_FORMULA_HIDDEN))
                    {
                        sal_Int16 i(0);
                        while (i < rStrImpValue.getLength() && rStrImpValue[i] != ' ')
                            ++i;
                        rtl::OUString sFirst(rStrImpValue.copy(0, i));
                        rtl::OUString sSecond(rStrImpValue.copy(i + 1));
                        aCellProtection.IsFormulaHidden = false;
                        aCellProtection.IsHidden = false;
                        aCellProtection.IsLocked = false;
                        if ((IsXMLToken(sFirst, XML_PROTECTED)) || (IsXMLToken(sSecond, XML_PROTECTED)))
                            aCellProtection.IsLocked = sal_True;
                        if ((IsXMLToken(sFirst, XML_FORMULA_HIDDEN)) || (IsXMLToken(sSecond, XML_FORMULA_HIDDEN)))
                            aCellProtection.IsFormulaHidden = sal_True;
                        rValue <<= aCellProtection;
                        bRetval = sal_True;
                    }
                    else
                    {
                        aCellProtection.IsFormulaHidden = sal_True;
                        aCellProtection.IsHidden = false;
                        aCellProtection.IsLocked = false;
                        rValue <<= aCellProtection;
                        bRetval = sal_True;
                    }
                }
                else
                {
                    aCellProtection.IsFormulaHidden = false;
                    aCellProtection.IsHidden = false;
                    aCellProtection.IsLocked = sal_True;
                    rValue <<= aCellProtection;
                    bRetval = sal_True;
                }
            }
            else
            {
                aCellProtection.IsFormulaHidden = sal_True;
                aCellProtection.IsHidden = sal_True;
                aCellProtection.IsLocked = sal_True;
                rValue <<= aCellProtection;
                bRetval = sal_True;
            }
        }
        else
        {
            aCellProtection.IsFormulaHidden = false;
            aCellProtection.IsHidden = false;
            aCellProtection.IsLocked = false;
            rValue <<= aCellProtection;
            bRetval = sal_True;
        }
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_CellProtection::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);
    util::CellProtection aCellProtection;

    if(rValue >>= aCellProtection)
    {
        if (!(aCellProtection.IsFormulaHidden || aCellProtection.IsHidden || aCellProtection.IsLocked))
        {
            rStrExpValue = GetXMLToken(XML_NONE);
            bRetval = sal_True;
        }
        else if (aCellProtection.IsHidden)
        {
            // #i105964# "Hide all" implies "Protected" in the UI, so it must be saved as "hidden-and-protected"
            // even if "IsLocked" is not set in the CellProtection struct.
            rStrExpValue = GetXMLToken(XML_HIDDEN_AND_PROTECTED);
            bRetval = sal_True;
        }
        else if (aCellProtection.IsLocked && !(aCellProtection.IsFormulaHidden || aCellProtection.IsHidden))
        {
            rStrExpValue = GetXMLToken(XML_PROTECTED);
            bRetval = sal_True;
        }
        else if (aCellProtection.IsFormulaHidden && !(aCellProtection.IsLocked || aCellProtection.IsHidden))
        {
            rStrExpValue = GetXMLToken(XML_FORMULA_HIDDEN);
            bRetval = sal_True;
        }
        else if (aCellProtection.IsFormulaHidden && aCellProtection.IsLocked)
        {
            rStrExpValue = GetXMLToken(XML_PROTECTED);
            rStrExpValue += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" "));
            rStrExpValue += GetXMLToken(XML_FORMULA_HIDDEN);
            bRetval = sal_True;
        }
    }

    return bRetval;
}

XmlScPropHdl_PrintContent::~XmlScPropHdl_PrintContent()
{
}

bool XmlScPropHdl_PrintContent::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    util::CellProtection aCellProtection1, aCellProtection2;

    if((r1 >>= aCellProtection1) && (r2 >>= aCellProtection2))
    {
        return (aCellProtection1.IsPrintHidden == aCellProtection2.IsPrintHidden);
    }
    return false;
}

sal_Bool XmlScPropHdl_PrintContent::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);
    util::CellProtection aCellProtection;
    sal_Bool bDefault(false);
    if (!rValue.hasValue())
    {
        aCellProtection.IsHidden = false;
        aCellProtection.IsLocked = sal_True;
        aCellProtection.IsFormulaHidden = false;
        aCellProtection.IsPrintHidden = false;
        bDefault = sal_True;
    }
    if ((rValue >>= aCellProtection) || bDefault)
    {
        bool bValue(false);
        if (::sax::Converter::convertBool(bValue, rStrImpValue))
        {
            aCellProtection.IsPrintHidden = !bValue;
            rValue <<= aCellProtection;
            bRetval = sal_True;
        }
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_PrintContent::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    util::CellProtection aCellProtection;
    if(rValue >>= aCellProtection)
    {
        rtl::OUStringBuffer sValue;
        ::sax::Converter::convertBool(sValue, !aCellProtection.IsPrintHidden);
        rStrExpValue = sValue.makeStringAndClear();
        bRetval = sal_True;
    }

    return bRetval;
}


XmlScPropHdl_JustifyMethod::~XmlScPropHdl_JustifyMethod()
{
}

bool XmlScPropHdl_JustifyMethod::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    sal_Int32 nVal1(0), nVal2(0);

    if((r1 >>= nVal1) && (r2 >>= nVal2))
        return (nVal1 == nVal2);
    return false;
}

sal_Bool XmlScPropHdl_JustifyMethod::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    bool bRetval = false;

    sal_Int32 nValue = table::CellJustifyMethod::AUTO;
    if (IsXMLToken(rStrImpValue, XML_AUTO))
    {
        nValue = table::CellJustifyMethod::AUTO;
        rValue <<= nValue;
        bRetval = true;
    }
    else if (IsXMLToken(rStrImpValue, XML_DISTRIBUTE))
    {
        nValue = table::CellJustifyMethod::DISTRIBUTE;
        rValue <<= nValue;
        bRetval = true;
    }
    else
        bRetval = true;

    return bRetval;
}

sal_Bool XmlScPropHdl_JustifyMethod::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Int32 nVal(0);
    bool bRetval = false;

    if (rValue >>= nVal)
    {
        switch (nVal)
        {
            case table::CellJustifyMethod::AUTO:
            {
                rStrExpValue = GetXMLToken(XML_AUTO);
                bRetval = true;
            }
            break;
            case table::CellJustifyMethod::DISTRIBUTE:
            {
                rStrExpValue = GetXMLToken(XML_DISTRIBUTE);
                bRetval = true;
            }
            break;
            default:
            {
                // added to avoid warnings
            }
        }
    }
    return bRetval;
}

XmlScPropHdl_HoriJustify::~XmlScPropHdl_HoriJustify()
{
}

bool XmlScPropHdl_HoriJustify::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    table::CellHoriJustify aHoriJustify1, aHoriJustify2;

    if((r1 >>= aHoriJustify1) && (r2 >>= aHoriJustify2))
        return (aHoriJustify1 == aHoriJustify2);
    return false;
}

sal_Bool XmlScPropHdl_HoriJustify::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    table::CellHoriJustify nValue = table::CellHoriJustify_LEFT;
    rValue >>= nValue;
    if (nValue != table::CellHoriJustify_REPEAT)
    {
        if (IsXMLToken(rStrImpValue, XML_START))
        {
             nValue = table::CellHoriJustify_LEFT;
            rValue <<= nValue;
            bRetval = sal_True;
        }
        else if (IsXMLToken(rStrImpValue, XML_END))
        {
             nValue = table::CellHoriJustify_RIGHT;
            rValue <<= nValue;
            bRetval = sal_True;
        }
        else if (IsXMLToken(rStrImpValue, XML_CENTER))
        {
             nValue = table::CellHoriJustify_CENTER;
            rValue <<= nValue;
            bRetval = sal_True;
        }
        else if (IsXMLToken(rStrImpValue, XML_JUSTIFY))
        {
             nValue = table::CellHoriJustify_BLOCK;
            rValue <<= nValue;
            bRetval = sal_True;
        }
    }
    else
        bRetval = sal_True;

    return bRetval;
}

sal_Bool XmlScPropHdl_HoriJustify::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    table::CellHoriJustify nVal;
    sal_Bool bRetval(false);

    if(rValue >>= nVal)
    {
        switch (nVal)
        {
            case table::CellHoriJustify_REPEAT:
            case table::CellHoriJustify_LEFT:
            {
                rStrExpValue = GetXMLToken(XML_START);
                bRetval = sal_True;
            }
            break;
            case table::CellHoriJustify_RIGHT:
            {
                rStrExpValue = GetXMLToken(XML_END);
                bRetval = sal_True;
            }
            break;
            case table::CellHoriJustify_CENTER:
            {
                rStrExpValue = GetXMLToken(XML_CENTER);
                bRetval = sal_True;
            }
            break;
            case table::CellHoriJustify_BLOCK:
            {
                rStrExpValue = GetXMLToken(XML_JUSTIFY);
                bRetval = sal_True;
            }
            break;
            default:
            {
                // added to avoid warnings
            }
        }
    }

    return bRetval;
}

XmlScPropHdl_HoriJustifySource::~XmlScPropHdl_HoriJustifySource()
{
}

bool XmlScPropHdl_HoriJustifySource::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    table::CellHoriJustify aHoriJustify1, aHoriJustify2;

    if((r1 >>= aHoriJustify1) && (r2 >>= aHoriJustify2))
        return (aHoriJustify1 == aHoriJustify2);
    return false;
}

sal_Bool XmlScPropHdl_HoriJustifySource::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    if (IsXMLToken(rStrImpValue, XML_FIX))
    {
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_VALUE_TYPE))
    {
        table::CellHoriJustify nValue(table::CellHoriJustify_STANDARD);
        rValue <<= nValue;
        bRetval = sal_True;
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_HoriJustifySource::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    table::CellHoriJustify nVal;
    sal_Bool bRetval(false);

    if(rValue >>= nVal)
    {
        if (nVal == table::CellHoriJustify_STANDARD)
        {
            rStrExpValue = GetXMLToken(XML_VALUE_TYPE);
            bRetval = sal_True;
        }
        else
        {
            rStrExpValue = GetXMLToken(XML_FIX);
            bRetval = sal_True;
        }
    }

    return bRetval;
}

XmlScPropHdl_HoriJustifyRepeat::~XmlScPropHdl_HoriJustifyRepeat()
{
}

bool XmlScPropHdl_HoriJustifyRepeat::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    table::CellHoriJustify aHoriJustify1, aHoriJustify2;

    if((r1 >>= aHoriJustify1) && (r2 >>= aHoriJustify2))
        return (aHoriJustify1 == aHoriJustify2);
    return false;
}

sal_Bool XmlScPropHdl_HoriJustifyRepeat::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    if (IsXMLToken(rStrImpValue, XML_FALSE))
    {
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_TRUE))
    {
        table::CellHoriJustify nValue = table::CellHoriJustify_REPEAT;
        rValue <<= nValue;
        bRetval = sal_True;
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_HoriJustifyRepeat::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    table::CellHoriJustify nVal;
    sal_Bool bRetval(false);

    if(rValue >>= nVal)
    {
        if (nVal == table::CellHoriJustify_REPEAT)
        {
            rStrExpValue = GetXMLToken(XML_TRUE);
            bRetval = sal_True;
        }
        else
        {
            rStrExpValue = GetXMLToken(XML_FALSE);
            bRetval = sal_True;
        }
    }

    return bRetval;
}

XmlScPropHdl_Orientation::~XmlScPropHdl_Orientation()
{
}

bool XmlScPropHdl_Orientation::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    table::CellOrientation aOrientation1, aOrientation2;

    if((r1 >>= aOrientation1) && (r2 >>= aOrientation2))
        return (aOrientation1 == aOrientation2);
    return false;
}

sal_Bool XmlScPropHdl_Orientation::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    table::CellOrientation nValue;
    if (IsXMLToken(rStrImpValue, XML_LTR))
    {
        nValue = table::CellOrientation_STANDARD;
        rValue <<= nValue;
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_TTB))
    {
        nValue = table::CellOrientation_STACKED;
        rValue <<= nValue;
        bRetval = sal_True;
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_Orientation::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    table::CellOrientation nVal;
    sal_Bool bRetval(false);

    if(rValue >>= nVal)
    {
        switch (nVal)
        {
            case table::CellOrientation_STACKED :
            {
                rStrExpValue = GetXMLToken(XML_TTB);
                bRetval = sal_True;
            }
            break;
            default:
            {
                rStrExpValue = GetXMLToken(XML_LTR);
                bRetval = sal_True;
            }
            break;
        }
    }

    return bRetval;
}

XmlScPropHdl_RotateAngle::~XmlScPropHdl_RotateAngle()
{
}

bool XmlScPropHdl_RotateAngle::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    sal_Int32 aAngle1 = 0, aAngle2 = 0;

    if((r1 >>= aAngle1) && (r2 >>= aAngle2))
        return (aAngle1 == aAngle2);
    return false;
}

sal_Bool XmlScPropHdl_RotateAngle::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    sal_Int32 nValue;
    if (::sax::Converter::convertNumber(nValue, rStrImpValue))
    {
        nValue *= 100;
        rValue <<= nValue;
        bRetval = sal_True;
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_RotateAngle::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Int32 nVal = 0;
    sal_Bool bRetval(false);

    if(rValue >>= nVal)
    {
        rtl::OUStringBuffer sValue;
        ::sax::Converter::convertNumber(sValue, sal_Int32(nVal / 100));
        rStrExpValue = sValue.makeStringAndClear();
        bRetval = sal_True;
    }

    return bRetval;
}

XmlScPropHdl_RotateReference::~XmlScPropHdl_RotateReference()
{
}

bool XmlScPropHdl_RotateReference::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    sal_Int32 aReference1(0), aReference2(0);

    if((r1 >>= aReference1) && (r2 >>= aReference2))
        return (aReference1 == aReference2);
    return false;
}

sal_Bool XmlScPropHdl_RotateReference::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    sal_Int32 nValue;
    if (IsXMLToken(rStrImpValue, XML_NONE))
    {
        nValue = table::CellVertJustify2::STANDARD;
        rValue <<= nValue;
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_BOTTOM))
    {
        nValue = table::CellVertJustify2::BOTTOM;
        rValue <<= nValue;
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_TOP))
    {
        nValue = table::CellVertJustify2::TOP;
        rValue <<= nValue;
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_CENTER))
    {
        nValue = table::CellVertJustify2::CENTER;
        rValue <<= nValue;
        bRetval = sal_True;
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_RotateReference::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Int32 nVal(0);
    sal_Bool bRetval(false);

    if(rValue >>= nVal)
    {
        switch (nVal)
        {
            case table::CellVertJustify2::BOTTOM :
            {
                rStrExpValue = GetXMLToken(XML_BOTTOM);
                bRetval = sal_True;
            }
            break;
            case table::CellVertJustify2::CENTER :
            {
                rStrExpValue = GetXMLToken(XML_CENTER);
                bRetval = sal_True;
            }
            break;
            case table::CellVertJustify2::STANDARD :
            {
                rStrExpValue = GetXMLToken(XML_NONE);
                bRetval = sal_True;
            }
            break;
            case table::CellVertJustify2::TOP :
            {
                rStrExpValue = GetXMLToken(XML_TOP);
                bRetval = sal_True;
            }
            break;
            default:
            {
                // added to avoid warnings
            }
        }
    }

    return bRetval;
}

XmlScPropHdl_VertJustify::~XmlScPropHdl_VertJustify()
{
}

bool XmlScPropHdl_VertJustify::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    sal_Int32 aReference1(0), aReference2(0);

    if((r1 >>= aReference1) && (r2 >>= aReference2))
        return (aReference1 == aReference2);
    return false;
}

sal_Bool XmlScPropHdl_VertJustify::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    sal_Int32 nValue;
    if (IsXMLToken(rStrImpValue, XML_AUTOMATIC))
    {
        nValue = table::CellVertJustify2::STANDARD;
        rValue <<= nValue;
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_BOTTOM))
    {
        nValue = table::CellVertJustify2::BOTTOM;
        rValue <<= nValue;
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_TOP))
    {
        nValue = table::CellVertJustify2::TOP;
        rValue <<= nValue;
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_MIDDLE))
    {
        nValue = table::CellVertJustify2::CENTER;
        rValue <<= nValue;
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_JUSTIFY))
    {
        nValue = table::CellVertJustify2::BLOCK;
        rValue <<= nValue;
        bRetval = sal_True;
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_VertJustify::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Int32 nVal(0);
    sal_Bool bRetval(false);

    if(rValue >>= nVal)
    {
        switch (nVal)
        {
            case table::CellVertJustify2::BOTTOM :
            {
                rStrExpValue = GetXMLToken(XML_BOTTOM);
                bRetval = sal_True;
            }
            break;
            case table::CellVertJustify2::CENTER :
            {
                rStrExpValue = GetXMLToken(XML_MIDDLE);
                bRetval = sal_True;
            }
            break;
            case table::CellVertJustify2::STANDARD :
            {
                rStrExpValue = GetXMLToken(XML_AUTOMATIC);
                bRetval = sal_True;
            }
            break;
            case table::CellVertJustify2::TOP :
            {
                rStrExpValue = GetXMLToken(XML_TOP);
                bRetval = sal_True;
            }
            break;
            case table::CellVertJustify2::BLOCK :
            {
                rStrExpValue = GetXMLToken(XML_JUSTIFY);
                bRetval = sal_True;
            }
            break;
            default:
            {
                // added to avoid warnings
            }
        }
    }

    return bRetval;
}

XmlScPropHdl_BreakBefore::~XmlScPropHdl_BreakBefore()
{
}

bool XmlScPropHdl_BreakBefore::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    sal_Bool aBreak1 = 0, aBreak2 = 0;

    if((r1 >>= aBreak1) && (r2 >>= aBreak2))
        return (aBreak1 == aBreak2);
    return false;
}

sal_Bool XmlScPropHdl_BreakBefore::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    sal_Bool bValue;
    if (IsXMLToken(rStrImpValue, XML_AUTO))
    {
        bValue = false;
        rValue = ::cppu::bool2any(bValue);
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_PAGE))
    {
        bValue = sal_True;
        rValue = ::cppu::bool2any(bValue);
        bRetval = sal_True;
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_BreakBefore::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    if(::cppu::any2bool(rValue))
    {
        rStrExpValue = GetXMLToken(XML_PAGE);
        bRetval = sal_True;
    }
    else
    {
        rStrExpValue = GetXMLToken(XML_AUTO);
        bRetval = sal_True;
    }

    return bRetval;
}

XmlScPropHdl_IsTextWrapped::~XmlScPropHdl_IsTextWrapped()
{
}

bool XmlScPropHdl_IsTextWrapped::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    return (::cppu::any2bool(r1) == ::cppu::any2bool(r2));
}

sal_Bool XmlScPropHdl_IsTextWrapped::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    if (IsXMLToken(rStrImpValue, XML_WRAP))
    {
        rValue = ::cppu::bool2any(sal_True);
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_NO_WRAP))
    {
        rValue = ::cppu::bool2any(false);
        bRetval = sal_True;
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_IsTextWrapped::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    if (::cppu::any2bool(rValue))
    {
        rStrExpValue = GetXMLToken(XML_WRAP);
        bRetval = sal_True;
    }
    else
    {
        rStrExpValue = GetXMLToken(XML_NO_WRAP);
        bRetval = sal_True;
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_IsEqual::importXML( const ::rtl::OUString& /* rStrImpValue */,
    ::com::sun::star::uno::Any& /* rValue */,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    OSL_FAIL("should never be called");
    return false;
}

sal_Bool XmlScPropHdl_IsEqual::exportXML( ::rtl::OUString& /* rStrExpValue */,
    const ::com::sun::star::uno::Any& /* rValue */,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    OSL_FAIL("should never be called");
    return false;
}

XmlScPropHdl_Vertical::~XmlScPropHdl_Vertical()
{
}

bool XmlScPropHdl_Vertical::equals(
    const ::com::sun::star::uno::Any& r1,
    const ::com::sun::star::uno::Any& r2 ) const
{
    return (::cppu::any2bool(r1) == ::cppu::any2bool(r2));
}

sal_Bool XmlScPropHdl_Vertical::importXML(
    const ::rtl::OUString& rStrImpValue,
    ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    if (IsXMLToken(rStrImpValue, XML_AUTO))
    {
        rValue = ::cppu::bool2any(sal_True);
        bRetval = sal_True;
    }
    else if (IsXMLToken(rStrImpValue, XML_0))
    {
        rValue = ::cppu::bool2any(false);
        bRetval = sal_True;
    }

    return bRetval;
}

sal_Bool XmlScPropHdl_Vertical::exportXML(
    ::rtl::OUString& rStrExpValue,
    const ::com::sun::star::uno::Any& rValue,
    const SvXMLUnitConverter& /* rUnitConverter */ ) const
{
    sal_Bool bRetval(false);

    if (::cppu::any2bool(rValue))
    {
        rStrExpValue = GetXMLToken(XML_AUTO);
        bRetval = sal_True;
    }
    else
    {
        rStrExpValue = GetXMLToken(XML_0);
        bRetval = sal_True;
    }

    return bRetval;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
