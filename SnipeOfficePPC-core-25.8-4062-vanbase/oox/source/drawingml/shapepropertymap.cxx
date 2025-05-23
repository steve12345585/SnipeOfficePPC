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

#include "oox/drawingml/shapepropertymap.hxx"

#include <com/sun/star/awt/Gradient.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/drawing/LineDash.hpp>
#include <com/sun/star/drawing/PolyPolygonBezierCoords.hpp>
#include "oox/helper/modelobjecthelper.hxx"

namespace oox {
namespace drawingml {

// ============================================================================

using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::uno;

using ::rtl::OUString;

// ============================================================================

namespace {

static const sal_Int32 spnDefaultShapeIds[ SHAPEPROP_END ] =
{
    PROP_LineStyle, PROP_LineWidth, PROP_LineColor, PROP_LineTransparence, PROP_LineDash, PROP_LineJoint,
    PROP_LineStartName, PROP_LineStartWidth, PROP_LineStartCenter, PROP_LineEndName, PROP_LineEndWidth, PROP_LineEndCenter,
    PROP_FillStyle, PROP_FillColor, PROP_FillTransparence, PROP_FillTransparenceGradientName, PROP_FillGradient,
    PROP_FillBitmapURL, PROP_FillBitmapMode, PROP_FillBitmapSizeX, PROP_FillBitmapSizeY,
    PROP_FillBitmapPositionOffsetX, PROP_FillBitmapPositionOffsetY, PROP_FillBitmapRectanglePoint
};

} // namespace

ShapePropertyInfo ShapePropertyInfo::DEFAULT( spnDefaultShapeIds, true, false, false, false );

ShapePropertyInfo::ShapePropertyInfo( const sal_Int32* pnPropertyIds,
        bool bNamedLineMarker, bool bNamedLineDash, bool bNamedFillGradient, bool bNamedFillBitmapUrl ) :
    mpnPropertyIds( pnPropertyIds ),
    mbNamedLineMarker( bNamedLineMarker ),
    mbNamedLineDash( bNamedLineDash ),
    mbNamedFillGradient( bNamedFillGradient ),
    mbNamedFillBitmapUrl( bNamedFillBitmapUrl )
{
    OSL_ENSURE( mpnPropertyIds != 0, "ShapePropertyInfo::ShapePropertyInfo - missing property identifiers" );
}

// ============================================================================

ShapePropertyMap::ShapePropertyMap( ModelObjectHelper& rModelObjHelper, const ShapePropertyInfo& rShapePropInfo ) :
    mrModelObjHelper( rModelObjHelper ),
    maShapePropInfo( rShapePropInfo )
{
}

bool ShapePropertyMap::supportsProperty( ShapePropertyId ePropId ) const
{
    return maShapePropInfo.has( ePropId );
}

bool ShapePropertyMap::hasNamedLineMarkerInTable( const OUString& rMarkerName ) const
{
    return maShapePropInfo.mbNamedLineMarker && mrModelObjHelper.hasLineMarker( rMarkerName );
}

bool ShapePropertyMap::setAnyProperty( ShapePropertyId ePropId, const Any& rValue )
{
    // get current property identifier for the specified property
    sal_Int32 nPropId = maShapePropInfo[ ePropId ];
    if( nPropId < 0 ) return false;

    // special handling for properties supporting named objects in tables
    switch( ePropId )
    {
        case SHAPEPROP_LineStart:
        case SHAPEPROP_LineEnd:
            return setLineMarker( nPropId, rValue );

        case SHAPEPROP_LineDash:
            return setLineDash( nPropId, rValue );

        case SHAPEPROP_FillGradient:
            return setFillGradient( nPropId, rValue );

        case SHAPEPROP_GradientTransparency:
            return setGradientTrans( nPropId, rValue );

        case SHAPEPROP_FillBitmapUrl:
            return setFillBitmapUrl( nPropId, rValue );

        default:;   // suppress compiler warnings
    }

    // set plain property value
    operator[]( nPropId ) = rValue;
    return true;
}

// private --------------------------------------------------------------------

bool ShapePropertyMap::setLineMarker( sal_Int32 nPropId, const Any& rValue )
{
    NamedValue aNamedMarker;
    if( (rValue >>= aNamedMarker) && !aNamedMarker.Name.isEmpty() )
    {
        // push line marker explicitly
        if( !maShapePropInfo.mbNamedLineMarker )
            return setAnyProperty( nPropId, aNamedMarker.Value );

        // create named line marker (if coordinates have been passed) and push its name
        bool bInserted = !aNamedMarker.Value.has< PolyPolygonBezierCoords >() ||
            mrModelObjHelper.insertLineMarker( aNamedMarker.Name, aNamedMarker.Value.get< PolyPolygonBezierCoords >() );
        return bInserted && setProperty( nPropId, aNamedMarker.Name );
    }
    return false;
}

bool ShapePropertyMap::setLineDash( sal_Int32 nPropId, const Any& rValue )
{
    // push line dash explicitly
    if( !maShapePropInfo.mbNamedLineDash )
        return setAnyProperty( nPropId, rValue );

    // create named line dash and push its name
    if( rValue.has< LineDash >() )
    {
        OUString aDashName = mrModelObjHelper.insertLineDash( rValue.get< LineDash >() );
        return !aDashName.isEmpty() && setProperty( nPropId, aDashName );
    }

    return false;
}

bool ShapePropertyMap::setFillGradient( sal_Int32 nPropId, const Any& rValue )
{
    // push gradient explicitly
    if( !maShapePropInfo.mbNamedFillGradient )
        return setAnyProperty( nPropId, rValue );

    // create named gradient and push its name
    if( rValue.has< Gradient >() )
    {
        OUString aGradientName = mrModelObjHelper.insertFillGradient( rValue.get< Gradient >() );
        return !aGradientName.isEmpty() && setProperty( nPropId, aGradientName );
    }

    return false;
}

bool ShapePropertyMap::setGradientTrans( sal_Int32 nPropId, const Any& rValue )
{
    // create named gradient and push its name
    if( rValue.has< Gradient >() )
    {
        OUString aGradientName = mrModelObjHelper.insertTransGrandient( rValue.get< Gradient >() );
        return !aGradientName.isEmpty()  && setProperty( nPropId, aGradientName );
    }

    return false;
}

bool ShapePropertyMap::setFillBitmapUrl( sal_Int32 nPropId, const Any& rValue )
{
    // push bitmap URL explicitly
    if( !maShapePropInfo.mbNamedFillBitmapUrl )
        return setAnyProperty( nPropId, rValue );

    // create named bitmap URL and push its name
    if( rValue.has< OUString >() )
    {
        OUString aBitmapUrlName = mrModelObjHelper.insertFillBitmapUrl( rValue.get< OUString >() );
        return !aBitmapUrlName.isEmpty() && setProperty( nPropId, aBitmapUrlName );
    }

    return false;
}

// ============================================================================

} // namespace drawingml
} // namespace oox

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
