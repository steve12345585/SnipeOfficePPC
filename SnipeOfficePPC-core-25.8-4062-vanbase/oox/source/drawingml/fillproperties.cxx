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

#include "oox/drawingml/fillproperties.hxx"

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/awt/Gradient.hpp>
#include <com/sun/star/text/GraphicCrop.hpp>
#include <com/sun/star/awt/Size.hpp>
#include <com/sun/star/drawing/BitmapMode.hpp>
#include <com/sun/star/drawing/ColorMode.hpp>
#include <com/sun/star/drawing/FillStyle.hpp>
#include <com/sun/star/drawing/RectanglePoint.hpp>
#include <com/sun/star/graphic/XGraphicTransformer.hpp>
#include "oox/helper/graphichelper.hxx"
#include "oox/drawingml/drawingmltypes.hxx"
#include "oox/drawingml/shapepropertymap.hxx"
#include "oox/token/tokens.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::graphic;

using ::rtl::OUString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Exception;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::uno::UNO_QUERY_THROW;
using ::com::sun::star::geometry::IntegerRectangle2D;

namespace oox {
namespace drawingml {

// ============================================================================

namespace {

BitmapMode lclGetBitmapMode( sal_Int32 nToken )
{
    OSL_ASSERT((nToken & sal_Int32(0xFFFF0000))==0);
    switch( nToken )
    {
        case XML_tile:      return BitmapMode_REPEAT;
        case XML_stretch:   return BitmapMode_STRETCH;
    }
    return BitmapMode_NO_REPEAT;
}

RectanglePoint lclGetRectanglePoint( sal_Int32 nToken )
{
    OSL_ASSERT((nToken & sal_Int32(0xFFFF0000))==0);
    switch( nToken )
    {
        case XML_tl:    return RectanglePoint_LEFT_TOP;
        case XML_t:     return RectanglePoint_MIDDLE_TOP;
        case XML_tr:    return RectanglePoint_RIGHT_TOP;
        case XML_l:     return RectanglePoint_LEFT_MIDDLE;
        case XML_ctr:   return RectanglePoint_MIDDLE_MIDDLE;
        case XML_r:     return RectanglePoint_RIGHT_MIDDLE;
        case XML_bl:    return RectanglePoint_LEFT_BOTTOM;
        case XML_b:     return RectanglePoint_MIDDLE_BOTTOM;
        case XML_br:    return RectanglePoint_RIGHT_BOTTOM;
    }
    return RectanglePoint_LEFT_TOP;
}

const awt::Size lclGetOriginalSize( const GraphicHelper& rGraphicHelper, const Reference< XGraphic >& rxGraphic )
{
    awt::Size aSizeHmm( 0, 0 );
    try
    {
        Reference< beans::XPropertySet > xGraphicPropertySet( rxGraphic, UNO_QUERY_THROW );
        if( xGraphicPropertySet->getPropertyValue( CREATE_OUSTRING( "Size100thMM" ) ) >>= aSizeHmm )
        {
            if( !aSizeHmm.Width && !aSizeHmm.Height )
            {   // MAPMODE_PIXEL USED :-(
                awt::Size aSourceSizePixel( 0, 0 );
                if( xGraphicPropertySet->getPropertyValue( CREATE_OUSTRING( "SizePixel" ) ) >>= aSourceSizePixel )
                    aSizeHmm = rGraphicHelper.convertScreenPixelToHmm( aSourceSizePixel );
            }
        }
    }
    catch( Exception& )
    {
    }
    return aSizeHmm;
}

} // namespace

// ============================================================================

void GradientFillProperties::assignUsed( const GradientFillProperties& rSourceProps )
{
    if( !rSourceProps.maGradientStops.empty() )
        maGradientStops = rSourceProps.maGradientStops;
    moFillToRect.assignIfUsed( rSourceProps.moFillToRect );
    moTileRect.assignIfUsed( rSourceProps.moTileRect );
    moGradientPath.assignIfUsed( rSourceProps.moGradientPath );
    moShadeAngle.assignIfUsed( rSourceProps.moShadeAngle );
    moShadeFlip.assignIfUsed( rSourceProps.moShadeFlip );
    moShadeScaled.assignIfUsed( rSourceProps.moShadeScaled );
    moRotateWithShape.assignIfUsed( rSourceProps.moRotateWithShape );
}

// ============================================================================

void PatternFillProperties::assignUsed( const PatternFillProperties& rSourceProps )
{
    maPattFgColor.assignIfUsed( rSourceProps.maPattFgColor );
    maPattBgColor.assignIfUsed( rSourceProps.maPattBgColor );
    moPattPreset.assignIfUsed( rSourceProps.moPattPreset );
}

// ============================================================================

void BlipFillProperties::assignUsed( const BlipFillProperties& rSourceProps )
{
    if( rSourceProps.mxGraphic.is() )
        mxGraphic = rSourceProps.mxGraphic;
    moBitmapMode.assignIfUsed( rSourceProps.moBitmapMode );
    moFillRect.assignIfUsed( rSourceProps.moFillRect );
    moTileOffsetX.assignIfUsed( rSourceProps.moTileOffsetX );
    moTileOffsetY.assignIfUsed( rSourceProps.moTileOffsetY );
    moTileScaleX.assignIfUsed( rSourceProps.moTileScaleX );
    moTileScaleY.assignIfUsed( rSourceProps.moTileScaleY );
    moTileAlign.assignIfUsed( rSourceProps.moTileAlign );
    moTileFlip.assignIfUsed( rSourceProps.moTileFlip );
    moRotateWithShape.assignIfUsed( rSourceProps.moRotateWithShape );
    moColorEffect.assignIfUsed( rSourceProps.moColorEffect );
    moBrightness.assignIfUsed( rSourceProps.moBrightness );
    moContrast.assignIfUsed( rSourceProps.moContrast );
    maColorChangeFrom.assignIfUsed( rSourceProps.maColorChangeFrom );
    maColorChangeTo.assignIfUsed( rSourceProps.maColorChangeTo );
}

// ============================================================================

void FillProperties::assignUsed( const FillProperties& rSourceProps )
{
    moFillType.assignIfUsed( rSourceProps.moFillType );
    maFillColor.assignIfUsed( rSourceProps.maFillColor );
    maGradientProps.assignUsed( rSourceProps.maGradientProps );
    maPatternProps.assignUsed( rSourceProps.maPatternProps );
    maBlipProps.assignUsed( rSourceProps.maBlipProps );
}

Color FillProperties::getBestSolidColor() const
{
    Color aSolidColor;
    if( moFillType.has() ) switch( moFillType.get() )
    {
        case XML_solidFill:
            aSolidColor = maFillColor;
        break;
        case XML_gradFill:
            if( !maGradientProps.maGradientStops.empty() )
                aSolidColor = maGradientProps.maGradientStops.begin()->second;
        break;
        case XML_pattFill:
            aSolidColor = maPatternProps.maPattBgColor.isUsed() ? maPatternProps.maPattBgColor : maPatternProps.maPattFgColor;
        break;
    }
    return aSolidColor;
}

void FillProperties::pushToPropMap( ShapePropertyMap& rPropMap,
        const GraphicHelper& rGraphicHelper, sal_Int32 nShapeRotation, sal_Int32 nPhClr,
        bool bFlipH, bool bFlipV ) const
{
    if( moFillType.has() )
    {
        FillStyle eFillStyle = FillStyle_NONE;
        OSL_ASSERT((moFillType.get() & sal_Int32(0xFFFF0000))==0);
        switch( moFillType.get() )
        {
            case XML_noFill:
                eFillStyle = FillStyle_NONE;
            break;

            case XML_solidFill:
                if( maFillColor.isUsed() )
                {
                    rPropMap.setProperty( SHAPEPROP_FillColor, maFillColor.getColor( rGraphicHelper, nPhClr ) );
                    if( maFillColor.hasTransparency() )
                        rPropMap.setProperty( SHAPEPROP_FillTransparency, maFillColor.getTransparency() );
                    eFillStyle = FillStyle_SOLID;
                }
            break;

            case XML_gradFill:
                // do not create gradient struct if property is not supported...
                if( rPropMap.supportsProperty( SHAPEPROP_FillGradient ) )
                {
                    sal_Int32 nEndTrans     = 0;
                    sal_Int32 nStartTrans   = 0;
                    awt::Gradient aGradient;
                    aGradient.Angle = 900;
                    aGradient.StartIntensity = 100;
                    aGradient.EndIntensity = 100;

                    size_t nColorCount = maGradientProps.maGradientStops.size();
                    if( nColorCount > 1 )
                    {
                        aGradient.StartColor = maGradientProps.maGradientStops.begin()->second.getColor( rGraphicHelper, nPhClr );
                        aGradient.EndColor = maGradientProps.maGradientStops.rbegin()->second.getColor( rGraphicHelper, nPhClr );
                        if( maGradientProps.maGradientStops.rbegin()->second.hasTransparency() )
                            nEndTrans = maGradientProps.maGradientStops.rbegin()->second.getTransparency()*255/100;
                        if( maGradientProps.maGradientStops.begin()->second.hasTransparency() )
                            nStartTrans = maGradientProps.maGradientStops.begin()->second.getTransparency()*255/100;
                    }

                    // Adjust for flips
                    if ( bFlipH )
                        nShapeRotation = 180*60000 - nShapeRotation;
                    if ( bFlipV )
                        nShapeRotation = -nShapeRotation;

                    // "rotate with shape" not set, or set to false -> do not rotate
                    if ( !maGradientProps.moRotateWithShape.get( false ) )
                        nShapeRotation = 0;

                    sal_Int32 nDmlAngle = 0;
                    if( maGradientProps.moGradientPath.has() )
                    {
                        aGradient.Style = (maGradientProps.moGradientPath.get() == XML_circle) ? awt::GradientStyle_ELLIPTICAL : awt::GradientStyle_RECT;
                        // position of gradient center (limited to [30%;70%], otherwise gradient is too hidden)
                        IntegerRectangle2D aFillToRect = maGradientProps.moFillToRect.get( IntegerRectangle2D( 0, 0, MAX_PERCENT, MAX_PERCENT ) );
                        sal_Int32 nCenterX = (MAX_PERCENT + aFillToRect.X1 - aFillToRect.X2) / 2;
                        aGradient.XOffset = getLimitedValue< sal_Int16, sal_Int32 >( nCenterX / PER_PERCENT, 30, 70 );
                        sal_Int32 nCenterY = (MAX_PERCENT + aFillToRect.Y1 - aFillToRect.Y2) / 2;
                        aGradient.YOffset = getLimitedValue< sal_Int16, sal_Int32 >( nCenterY / PER_PERCENT, 30, 70 );
                        ::std::swap( aGradient.StartColor, aGradient.EndColor );
                        ::std::swap( nStartTrans, nEndTrans );
                        nDmlAngle = nShapeRotation;
                    }
                    else
                    {
                        /*  Try to detect a VML axial gradient. This type of
                            gradient is simulated by a 3-point linear gradient.
                            Even if its a multi-color linear gradient, its probably better to assume
                            axial gradient, when there are 3 or more points.
                         */
                        bool bAxial = (nColorCount >= 3);
                        aGradient.Style = bAxial ? awt::GradientStyle_AXIAL : awt::GradientStyle_LINEAR;
                        nDmlAngle = maGradientProps.moShadeAngle.get( 0 ) - nShapeRotation;
                        // convert DrawingML angle (in 1/60000 degrees) to API angle (in 1/10 degrees)
                        aGradient.Angle = static_cast< sal_Int16 >( (4500 - (nDmlAngle / (PER_DEGREE / 10))) % 3600 );
                        if( bAxial )
                        {
                            GradientFillProperties::GradientStopMap::const_iterator aIt = maGradientProps.maGradientStops.begin();
                            // Try to find the axial median
                            for(size_t i=0;i<nColorCount;i+=3)
                                ++aIt;
                            // API StartColor is inner color in axial gradient
                            // aIt->second.hasColor() kind would be better than Color != API_RGB_WHITE
                            if( aGradient.StartColor == aGradient.EndColor &&
                                ( !aIt->second.hasTransparency() || aIt->second.getColor( rGraphicHelper, nPhClr ) != API_RGB_WHITE ) )
                            {
                                aGradient.StartColor = aIt->second.getColor( rGraphicHelper, nPhClr );
                            }

                            if( nStartTrans == nEndTrans && aIt->second.hasTransparency() )
                                nStartTrans = aIt->second.getTransparency()*255/100;
                        }
                    }

                    // push gradient or named gradient to property map
                    if( rPropMap.setProperty( SHAPEPROP_FillGradient, aGradient ) )
                        eFillStyle = FillStyle_GRADIENT;

                    // push gradient transparency to property map
                    if( nStartTrans != 0 || nEndTrans != 0 )
                    {
                        awt::Gradient aGrad(aGradient);
                        uno::Any aVal;
                        aGrad.EndColor = (sal_Int32)( nEndTrans | nEndTrans << 8 | nEndTrans << 16 );
                        aGrad.StartColor = (sal_Int32)( nStartTrans | nStartTrans << 8 | nStartTrans << 16 );
                        aVal <<= aGrad;
                        rPropMap.setProperty( SHAPEPROP_GradientTransparency, aGrad );
                    }

                }
            break;

            case XML_blipFill:
                // do not start complex graphic transformation if property is not supported...
                if( maBlipProps.mxGraphic.is() && rPropMap.supportsProperty( SHAPEPROP_FillBitmapUrl ) )
                {
                    // TODO: "rotate with shape" is not possible with our current core

                    OUString aGraphicUrl = rGraphicHelper.createGraphicObject( maBlipProps.mxGraphic );
                    // push bitmap or named bitmap to property map
                    if( !aGraphicUrl.isEmpty() && rPropMap.setProperty( SHAPEPROP_FillBitmapUrl, aGraphicUrl ) )
                        eFillStyle = FillStyle_BITMAP;

                    // set other bitmap properties, if bitmap has been inserted into the map
                    if( eFillStyle == FillStyle_BITMAP )
                    {
                        // bitmap mode (single, repeat, stretch)
                        BitmapMode eBitmapMode = lclGetBitmapMode( maBlipProps.moBitmapMode.get( XML_TOKEN_INVALID ) );
                        rPropMap.setProperty( SHAPEPROP_FillBitmapMode, eBitmapMode );

                        // additional settings for repeated bitmap
                        if( eBitmapMode == BitmapMode_REPEAT )
                        {
                            // anchor position inside bitmap
                            RectanglePoint eRectPoint = lclGetRectanglePoint( maBlipProps.moTileAlign.get( XML_tl ) );
                            rPropMap.setProperty( SHAPEPROP_FillBitmapRectanglePoint, eRectPoint );

                            awt::Size aOriginalSize = lclGetOriginalSize( rGraphicHelper, maBlipProps.mxGraphic );
                            if( (aOriginalSize.Width > 0) && (aOriginalSize.Height > 0) )
                            {
                                // size of one bitmap tile (given as 1/1000 percent of bitmap size), convert to 1/100 mm
                                double fScaleX = maBlipProps.moTileScaleX.get( MAX_PERCENT ) / static_cast< double >( MAX_PERCENT );
                                sal_Int32 nFillBmpSizeX = getLimitedValue< sal_Int32, double >( aOriginalSize.Width * fScaleX, 1, SAL_MAX_INT32 );
                                rPropMap.setProperty( SHAPEPROP_FillBitmapSizeX, nFillBmpSizeX );
                                double fScaleY = maBlipProps.moTileScaleY.get( MAX_PERCENT ) / static_cast< double >( MAX_PERCENT );
                                sal_Int32 nFillBmpSizeY = getLimitedValue< sal_Int32, double >( aOriginalSize.Height * fScaleY, 1, SAL_MAX_INT32 );
                                rPropMap.setProperty( SHAPEPROP_FillBitmapSizeY, nFillBmpSizeY );

                                // offset of the first bitmap tile (given as EMUs), convert to percent
                                sal_Int16 nTileOffsetX = getDoubleIntervalValue< sal_Int16 >( maBlipProps.moTileOffsetX.get( 0 ) / 3.6 / aOriginalSize.Width, 0, 100 );
                                rPropMap.setProperty( SHAPEPROP_FillBitmapOffsetX, nTileOffsetX );
                                sal_Int16 nTileOffsetY = getDoubleIntervalValue< sal_Int16 >( maBlipProps.moTileOffsetY.get( 0 ) / 3.6 / aOriginalSize.Height, 0, 100 );
                                rPropMap.setProperty( SHAPEPROP_FillBitmapOffsetY, nTileOffsetY );
                            }
                        }
                    }
                }
            break;

            case XML_pattFill:
            {
                // todo
                Color aColor = getBestSolidColor();
                if( aColor.isUsed() )
                {
                    rPropMap.setProperty( SHAPEPROP_FillColor, aColor.getColor( rGraphicHelper, nPhClr ) );
                    if( aColor.hasTransparency() )
                        rPropMap.setProperty( SHAPEPROP_FillTransparency, aColor.getTransparency() );
                    eFillStyle = FillStyle_SOLID;
                }
            }
            break;

            case XML_grpFill:
                // todo
                eFillStyle = FillStyle_NONE;
            break;
        }

        // set final fill style property
        rPropMap.setProperty( SHAPEPROP_FillStyle, eFillStyle );
    }
}

// ============================================================================

void GraphicProperties::pushToPropMap( PropertyMap& rPropMap, const GraphicHelper& rGraphicHelper, sal_Int32 nPhClr ) const
{
    if( maBlipProps.mxGraphic.is() )
    {
        // created transformed graphic
        Reference< XGraphic > xGraphic = maBlipProps.mxGraphic;
        if( maBlipProps.maColorChangeFrom.isUsed() && maBlipProps.maColorChangeTo.isUsed() )
        {
            sal_Int32 nFromColor = maBlipProps.maColorChangeFrom.getColor( rGraphicHelper, nPhClr );
            sal_Int32 nToColor = maBlipProps.maColorChangeTo.getColor( rGraphicHelper, nPhClr );
            if ( (nFromColor != nToColor) || maBlipProps.maColorChangeTo.hasTransparency() ) try
            {
                sal_Int16 nToTransparence = maBlipProps.maColorChangeTo.getTransparency();
                sal_Int8 nToAlpha = static_cast< sal_Int8 >( (100 - nToTransparence) / 39.062 );   // ?!? correct ?!?
                Reference< XGraphicTransformer > xTransformer( maBlipProps.mxGraphic, UNO_QUERY_THROW );
                xGraphic = xTransformer->colorChange( maBlipProps.mxGraphic, nFromColor, 9, nToColor, nToAlpha );
            }
            catch( Exception& )
            {
            }
        }

        OUString aGraphicUrl = rGraphicHelper.createGraphicObject( xGraphic );
        if( !aGraphicUrl.isEmpty() )
            rPropMap[ PROP_GraphicURL ] <<= aGraphicUrl;

        // cropping
        if ( maBlipProps.moClipRect.has() )
        {
            geometry::IntegerRectangle2D oClipRect( maBlipProps.moClipRect.get() );
            awt::Size aOriginalSize( rGraphicHelper.getOriginalSize( xGraphic ) );
            if ( aOriginalSize.Width && aOriginalSize.Height )
            {
                text::GraphicCrop aGraphCrop( 0, 0, 0, 0 );
                if ( oClipRect.X1 )
                    aGraphCrop.Left = static_cast< sal_Int32 >( ( static_cast< double >( aOriginalSize.Width ) * oClipRect.X1 ) / 100000 );
                if ( oClipRect.Y1 )
                    aGraphCrop.Top = static_cast< sal_Int32 >( ( static_cast< double >( aOriginalSize.Height ) * oClipRect.Y1 ) / 100000 );
                if ( oClipRect.X2 )
                    aGraphCrop.Right = static_cast< sal_Int32 >( ( static_cast< double >( aOriginalSize.Width ) * oClipRect.X2 ) / 100000 );
                if ( oClipRect.Y2 )
                    aGraphCrop.Bottom = static_cast< sal_Int32 >( ( static_cast< double >( aOriginalSize.Height ) * oClipRect.Y2 ) / 100000 );
                rPropMap[ PROP_GraphicCrop ] <<= aGraphCrop;
            }
        }
    }

    // color effect
    ColorMode eColorMode = ColorMode_STANDARD;
    switch( maBlipProps.moColorEffect.get( XML_TOKEN_INVALID ) )
    {
        case XML_biLevel:   eColorMode = ColorMode_MONO;    break;
        case XML_grayscl:   eColorMode = ColorMode_GREYS;   break;
    }
    rPropMap[ PROP_GraphicColorMode ] <<= eColorMode;

    // brightness and contrast
    sal_Int16 nBrightness = getLimitedValue< sal_Int16, sal_Int32 >( maBlipProps.moBrightness.get( 0 ) / PER_PERCENT, -100, 100 );
    if( nBrightness != 0 )
        rPropMap[ PROP_AdjustLuminance ] <<= nBrightness;
    sal_Int16 nContrast = getLimitedValue< sal_Int16, sal_Int32 >( maBlipProps.moContrast.get( 0 ) / PER_PERCENT, -100, 100 );
    if( nContrast != 0 )
        rPropMap[ PROP_AdjustContrast ] <<= nContrast;

    // Media content
    if( !maAudio.msEmbed.isEmpty() )
        rPropMap[ PROP_MediaURL ] <<= maAudio.msEmbed;
}

// ============================================================================

} // namespace drawingml
} // namespace oox

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
