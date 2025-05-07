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



#include <sfx2/bindings.hxx>
#include <svx/svxids.hrc>
#include <svx/grafctrl.hxx>
#include <svl/cjkoptions.hxx>
#include <sfx2/viewsh.hxx>
#include <sfx2/imagemgr.hxx>
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>

#include "sddll.hxx"
#include "GraphicDocShell.hxx"

#include <vcl/toolbox.hxx>

#include "app.hxx"
#include "app.hrc"
#include "res_bmp.hrc"
#include "sdresid.hxx"
#include "tbx_ww.hxx"

SFX_IMPL_TOOLBOX_CONTROL( SdTbxControl, TbxImageItem )

/*************************************************************************
|*
|* Klasse fuer Toolbox
|*
\************************************************************************/

SdTbxControl::SdTbxControl( sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx ) :
        SfxToolBoxControl( nSlotId, nId, rTbx )
{
    rTbx.SetItemBits( nId, TIB_DROPDOWN | rTbx.GetItemBits( nId ) );
    rTbx.Invalidate();
}

/*-------------------------------------------------------------------------*/

SfxPopupWindowType SdTbxControl::GetPopupWindowType() const
{
    return( SFX_POPUPWINDOW_ONTIMEOUT );
}

/*************************************************************************
|*
|* Hier wird das Fenster erzeugt
|* Lage der Toolbox mit GetToolBox() abfragbar
|* rItemRect sind die Screen-Koordinaten
|*
\************************************************************************/

SfxPopupWindow* SdTbxControl::CreatePopupWindow()
{
    SfxPopupWindow *pWin = NULL;
    rtl::OUString aToolBarResStr;
    rtl::OUStringBuffer aTbxResName( "private:resource/toolbar/" );
    switch( GetSlotId() )
    {
        case SID_OBJECT_ALIGN:
            aTbxResName.appendAscii( "alignmentbar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_ZOOM_TOOLBOX:
            aTbxResName.appendAscii( "zoombar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_OBJECT_CHOOSE_MODE:
            aTbxResName.appendAscii( "choosemodebar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_POSITION:
            aTbxResName.appendAscii( "positionbar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_DRAWTBX_TEXT:
            aTbxResName.appendAscii( "textbar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_DRAWTBX_RECTANGLES:
            aTbxResName.appendAscii( "rectanglesbar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_DRAWTBX_ELLIPSES:
            aTbxResName.appendAscii( "ellipsesbar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_DRAWTBX_LINES:
            aTbxResName.appendAscii( "linesbar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_DRAWTBX_ARROWS:
            aTbxResName.appendAscii( "arrowsbar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_DRAWTBX_3D_OBJECTS:
            aTbxResName.appendAscii( "3dobjectsbar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_DRAWTBX_CONNECTORS:
            aTbxResName.appendAscii( "connectorsbar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
        case SID_DRAWTBX_INSERT:
            aTbxResName.appendAscii( "insertbar" );
            aToolBarResStr = aTbxResName.makeStringAndClear();
        break;
    }

    if ( !aToolBarResStr.isEmpty() )
        createAndPositionSubToolBar( aToolBarResStr );

    return( pWin );
}

/*-------------------------------------------------------------------------*/

void SdTbxControl::StateChanged( sal_uInt16 nSId,
                        SfxItemState eState, const SfxPoolItem* pState )
{
    SfxToolBoxControl::StateChanged( nSId, eState, pState );

    if( eState == SFX_ITEM_AVAILABLE )
    {
        TbxImageItem* pItem = PTR_CAST( TbxImageItem, pState );
        // Im StarDesktop kann jetzt auch ein anderes Item ankommen,
        // das nicht ausgewertet werden darf
        if( pItem )
        {
            ToolBox& rTbx = GetToolBox();
            sal_uInt16 nImage = pItem->GetValue();
            if( nImage == 0 )
            {
                if( rTbx.IsItemChecked( nSId ) )
                    rTbx.CheckItem( nSId, sal_False );
            }
            else
            {
                rtl::OUString aSlotURL( "slot:" );
                aSlotURL += rtl::OUString::valueOf( sal_Int32( nImage ));
                Image aImage = GetImage( m_xFrame,
                                         aSlotURL,
                                         hasBigImages()
                                       );

                // !-Operator prueft, ob Image nicht vorhanden ist
                if( !!aImage )
                {
                    rTbx.SetItemImage( GetId(), aImage );
                    rTbx.CheckItem( GetId(), IsCheckable( nImage ) );

                    if( nSId != SID_ZOOM_TOOLBOX &&
                        nSId != SID_DRAWTBX_INSERT &&
                        nSId != SID_POSITION &&
                        nSId != SID_OBJECT_ALIGN )
                    {
                        if( nSId != SID_OBJECT_CHOOSE_MODE &&
                            rTbx.IsItemChecked( SID_OBJECT_CHOOSE_MODE ) )
                            rTbx.CheckItem( SID_OBJECT_CHOOSE_MODE, sal_False );
                        if( nSId != SID_DRAWTBX_TEXT &&
                            rTbx.IsItemChecked( SID_DRAWTBX_TEXT ) )
                             rTbx.CheckItem( SID_DRAWTBX_TEXT, sal_False );
                        if( nSId != SID_DRAWTBX_RECTANGLES &&
                            rTbx.IsItemChecked( SID_DRAWTBX_RECTANGLES ) )
                               rTbx.CheckItem( SID_DRAWTBX_RECTANGLES, sal_False );
                        if( nSId != SID_DRAWTBX_ELLIPSES &&
                            rTbx.IsItemChecked( SID_DRAWTBX_ELLIPSES ) )
                               rTbx.CheckItem( SID_DRAWTBX_ELLIPSES, sal_False );
                        if( nSId != SID_DRAWTBX_LINES &&
                            rTbx.IsItemChecked( SID_DRAWTBX_LINES ) )
                            rTbx.CheckItem( SID_DRAWTBX_LINES, sal_False );
                        if( nSId != SID_DRAWTBX_ARROWS &&
                            rTbx.IsItemChecked( SID_DRAWTBX_ARROWS ) )
                            rTbx.CheckItem( SID_DRAWTBX_ARROWS, sal_False );
                        if( nSId != SID_DRAWTBX_3D_OBJECTS &&
                            rTbx.IsItemChecked( SID_DRAWTBX_3D_OBJECTS ) )
                            rTbx.CheckItem( SID_DRAWTBX_3D_OBJECTS, sal_False );
                        if( nSId != SID_DRAWTBX_CONNECTORS &&
                            rTbx.IsItemChecked( SID_DRAWTBX_CONNECTORS ) )
                            rTbx.CheckItem( SID_DRAWTBX_CONNECTORS, sal_False );
                    }
                }
            }
        }
    }
}

/*-------------------------------------------------------------------------*/

sal_Bool SdTbxControl::IsCheckable( sal_uInt16 nSId )
{
    switch( nSId )
    {
        case SID_OBJECT_ROTATE:
        case SID_OBJECT_MIRROR:
        case SID_OBJECT_CROP:
        case SID_OBJECT_TRANSPARENCE:
        case SID_OBJECT_GRADIENT:
        case SID_OBJECT_SHEAR:
        case SID_OBJECT_CROOK_ROTATE:
        case SID_OBJECT_CROOK_SLANT:
        case SID_OBJECT_CROOK_STRETCH:
        case SID_CONVERT_TO_3D_LATHE:

        case SID_ATTR_CHAR:
        case SID_ATTR_CHAR_VERTICAL:
        case SID_TEXT_FITTOSIZE:
        case SID_TEXT_FITTOSIZE_VERTICAL:
        case SID_DRAW_CAPTION:
        case SID_DRAW_CAPTION_VERTICAL:
        case SID_DRAW_FONTWORK:
        case SID_DRAW_FONTWORK_VERTICAL:

        case SID_DRAW_RECT:
        case SID_DRAW_SQUARE:
        case SID_DRAW_RECT_ROUND:
        case SID_DRAW_SQUARE_ROUND:
        case SID_DRAW_RECT_NOFILL:
        case SID_DRAW_SQUARE_NOFILL:
        case SID_DRAW_RECT_ROUND_NOFILL:
        case SID_DRAW_SQUARE_ROUND_NOFILL:

        case SID_DRAW_ELLIPSE:
        case SID_DRAW_CIRCLE:
        case SID_DRAW_PIE:
        case SID_DRAW_CIRCLEPIE:
        case SID_DRAW_ELLIPSECUT:
        case SID_DRAW_CIRCLECUT:
        case SID_DRAW_ARC:
        case SID_DRAW_CIRCLEARC:
        case SID_DRAW_ELLIPSE_NOFILL:
        case SID_DRAW_CIRCLE_NOFILL:
        case SID_DRAW_PIE_NOFILL:
        case SID_DRAW_CIRCLEPIE_NOFILL:
        case SID_DRAW_ELLIPSECUT_NOFILL:
        case SID_DRAW_CIRCLECUT_NOFILL:

        case SID_DRAW_BEZIER_NOFILL:
        case SID_DRAW_POLYGON_NOFILL:
        case SID_DRAW_XPOLYGON_NOFILL:
        case SID_DRAW_BEZIER_FILL:
        case SID_DRAW_POLYGON:
        case SID_DRAW_XPOLYGON:
        case SID_DRAW_FREELINE:
        case SID_DRAW_FREELINE_NOFILL:

        case SID_DRAW_LINE:
        case SID_DRAW_XLINE:
        case SID_DRAW_MEASURELINE:
        case SID_LINE_ARROW_START:
        case SID_LINE_ARROW_END:
        case SID_LINE_ARROWS:
        case SID_LINE_ARROW_CIRCLE:
        case SID_LINE_CIRCLE_ARROW:
        case SID_LINE_ARROW_SQUARE:
        case SID_LINE_SQUARE_ARROW:

        case SID_3D_CUBE:
        case SID_3D_SPHERE:
        case SID_3D_CYLINDER:
        case SID_3D_CONE:
        case SID_3D_PYRAMID:
        case SID_3D_TORUS:
        case SID_3D_SHELL:
        case SID_3D_HALF_SPHERE:

        case SID_TOOL_CONNECTOR:
        case SID_CONNECTOR_ARROW_START:
        case SID_CONNECTOR_ARROW_END:
        case SID_CONNECTOR_ARROWS:
        case SID_CONNECTOR_CIRCLE_START:
        case SID_CONNECTOR_CIRCLE_END:
        case SID_CONNECTOR_CIRCLES:
        case SID_CONNECTOR_LINE:
        case SID_CONNECTOR_LINE_ARROW_START:
        case SID_CONNECTOR_LINE_ARROW_END:
        case SID_CONNECTOR_LINE_ARROWS:
        case SID_CONNECTOR_LINE_CIRCLE_START:
        case SID_CONNECTOR_LINE_CIRCLE_END:
        case SID_CONNECTOR_LINE_CIRCLES:
        case SID_CONNECTOR_CURVE:
        case SID_CONNECTOR_CURVE_ARROW_START:
        case SID_CONNECTOR_CURVE_ARROW_END:
        case SID_CONNECTOR_CURVE_ARROWS:
        case SID_CONNECTOR_CURVE_CIRCLE_START:
        case SID_CONNECTOR_CURVE_CIRCLE_END:
        case SID_CONNECTOR_CURVE_CIRCLES:
        case SID_CONNECTOR_LINES:
        case SID_CONNECTOR_LINES_ARROW_START:
        case SID_CONNECTOR_LINES_ARROW_END:
        case SID_CONNECTOR_LINES_ARROWS:
        case SID_CONNECTOR_LINES_CIRCLE_START:
        case SID_CONNECTOR_LINES_CIRCLE_END:
        case SID_CONNECTOR_LINES_CIRCLES:

            return( sal_True );
    }
    return( sal_False );
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
