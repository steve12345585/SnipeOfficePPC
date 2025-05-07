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

#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#define _BGFX_TOOLS_CANVASTOOLS_HXX

#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <basegfx/basegfxdllapi.h>


namespace com { namespace sun { namespace star { namespace geometry
{
    struct AffineMatrix2D;
    struct AffineMatrix3D;
    struct Matrix2D;
    struct RealPoint2D;
    struct RealSize2D;
    struct RealRectangle2D;
    struct RealRectangle3D;
    struct IntegerPoint2D;
    struct IntegerSize2D;
    struct IntegerRectangle2D;
    struct RealBezierSegment2D;
} } } }

namespace com { namespace sun { namespace star { namespace rendering
{
    class  XGraphicDevice;
    class  XPolyPolygon2D;
} } } }

namespace com { namespace sun { namespace star { namespace awt
{
    struct Point;
    struct Size;
    struct Rectangle;
} } } }

namespace basegfx
{
    class B2DHomMatrix;
    class B3DHomMatrix;
    class B2DVector;
    class B2DPoint;
    class B2DRange;
    class B3DRange;
    class B2IBox;
    class B2IVector;
    class B2IPoint;
    class B2IRange;
    class B2DPolygon;
    class B2DPolyPolygon;

    namespace unotools
    {
        // Polygon conversions
        // ===================================================================

        BASEGFX_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XPolyPolygon2D >
            xPolyPolygonFromB2DPolygon( const ::com::sun::star::uno::Reference<
                                            ::com::sun::star::rendering::XGraphicDevice >&  xGraphicDevice,
                                        const ::basegfx::B2DPolygon&                        rPoly    );

        BASEGFX_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XPolyPolygon2D >
            xPolyPolygonFromB2DPolyPolygon( const ::com::sun::star::uno::Reference<
                                                 ::com::sun::star::rendering::XGraphicDevice >&     xGraphicDevice,
                                            const ::basegfx::B2DPolyPolygon&                    rPolyPoly    );


        BASEGFX_DLLPUBLIC ::com::sun::star::uno::Sequence<
              ::com::sun::star::uno::Sequence<
                  ::com::sun::star::geometry::RealBezierSegment2D > >
                    bezierSequenceSequenceFromB2DPolyPolygon( const ::basegfx::B2DPolyPolygon& rPolyPoly );

        BASEGFX_DLLPUBLIC ::com::sun::star::uno::Sequence<
              ::com::sun::star::uno::Sequence<
                  ::com::sun::star::geometry::RealPoint2D > >
                    pointSequenceSequenceFromB2DPolyPolygon( const ::basegfx::B2DPolyPolygon& rPolyPoly );

        BASEGFX_DLLPUBLIC ::basegfx::B2DPolygon polygonFromPoint2DSequence(
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::geometry::RealPoint2D >& rPoints );

        BASEGFX_DLLPUBLIC ::basegfx::B2DPolyPolygon polyPolygonFromPoint2DSequenceSequence(
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::uno::Sequence< ::com::sun::star::geometry::RealPoint2D > >& rPoints );

        BASEGFX_DLLPUBLIC ::basegfx::B2DPolygon polygonFromBezier2DSequence(
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::geometry::RealBezierSegment2D >& rPoints );

        BASEGFX_DLLPUBLIC ::basegfx::B2DPolyPolygon polyPolygonFromBezier2DSequenceSequence(
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::uno::Sequence< ::com::sun::star::geometry::RealBezierSegment2D > >& rPoints );

        BASEGFX_DLLPUBLIC ::basegfx::B2DPolyPolygon b2DPolyPolygonFromXPolyPolygon2D(
            const ::com::sun::star::uno::Reference<
                     ::com::sun::star::rendering::XPolyPolygon2D >& rPoly );

        // Matrix conversions
        // ===================================================================

        BASEGFX_DLLPUBLIC ::com::sun::star::geometry::AffineMatrix2D&
            affineMatrixFromHomMatrix( ::com::sun::star::geometry::AffineMatrix2D&  matrix,
                                       const ::basegfx::B2DHomMatrix&               transform);

        BASEGFX_DLLPUBLIC ::com::sun::star::geometry::AffineMatrix3D& affineMatrixFromHomMatrix3D(
            ::com::sun::star::geometry::AffineMatrix3D& matrix,
            const ::basegfx::B3DHomMatrix& transform);

        BASEGFX_DLLPUBLIC ::basegfx::B2DHomMatrix&
            homMatrixFromAffineMatrix( ::basegfx::B2DHomMatrix&                             transform,
                                       const ::com::sun::star::geometry::AffineMatrix2D&    matrix );

        BASEGFX_DLLPUBLIC ::basegfx::B3DHomMatrix homMatrixFromAffineMatrix3D( const ::com::sun::star::geometry::AffineMatrix3D& matrix );

        BASEGFX_DLLPUBLIC ::com::sun::star::geometry::Matrix2D&
            matrixFromHomMatrix( ::com::sun::star::geometry::Matrix2D& matrix,
                                 const ::basegfx::B2DHomMatrix&        transform);

        BASEGFX_DLLPUBLIC ::basegfx::B2DHomMatrix&
            homMatrixFromMatrix( ::basegfx::B2DHomMatrix&                    transform,
                                 const ::com::sun::star::geometry::Matrix2D& matrix );

        // Geometry conversions
        // ===================================================================

        BASEGFX_DLLPUBLIC ::com::sun::star::geometry::RealSize2D        size2DFromB2DSize( const ::basegfx::B2DVector& );
        BASEGFX_DLLPUBLIC ::com::sun::star::geometry::RealPoint2D       point2DFromB2DPoint( const ::basegfx::B2DPoint& );
        BASEGFX_DLLPUBLIC ::com::sun::star::geometry::RealRectangle2D   rectangle2DFromB2DRectangle( const ::basegfx::B2DRange& );
        BASEGFX_DLLPUBLIC ::com::sun::star::geometry::RealRectangle3D   rectangle3DFromB3DRectangle( const ::basegfx::B3DRange& );

        BASEGFX_DLLPUBLIC ::basegfx::B2DPoint       b2DPointFromRealPoint2D( const ::com::sun::star::geometry::RealPoint2D& );
        BASEGFX_DLLPUBLIC ::basegfx::B2DRange       b2DRectangleFromRealRectangle2D( const ::com::sun::star::geometry::RealRectangle2D& );
        BASEGFX_DLLPUBLIC ::basegfx::B3DRange       b3DRectangleFromRealRectangle3D( const ::com::sun::star::geometry::RealRectangle3D& );

        BASEGFX_DLLPUBLIC ::com::sun::star::geometry::IntegerSize2D         integerSize2DFromB2ISize( const ::basegfx::B2IVector& );

        BASEGFX_DLLPUBLIC ::basegfx::B2IVector      b2ISizeFromIntegerSize2D( const ::com::sun::star::geometry::IntegerSize2D& );
        BASEGFX_DLLPUBLIC ::basegfx::B2IRange       b2IRectangleFromIntegerRectangle2D( const ::com::sun::star::geometry::IntegerRectangle2D& );

        BASEGFX_DLLPUBLIC ::basegfx::B2IRange       b2IRectangleFromAwtRectangle( const ::com::sun::star::awt::Rectangle& );

        // Geometry comparisons
        // ===================================================================

        /** Return smalltest integer range, which completely contains
            given floating point range.

            @param rRange
            Input range. Values must be within the representable
            bounds of sal_Int32

            @return the closest integer range, which completely
            contains rRange.
         */
        BASEGFX_DLLPUBLIC ::basegfx::B2IRange   b2ISurroundingRangeFromB2DRange( const ::basegfx::B2DRange& rRange );

        /** Return smalltest integer box, which completely contains
            given floating point range.

            @param rRange
            Input range. Values must be within the representable
            bounds of sal_Int32

            @return the closest integer box, which completely contains
            rRange. Note that this box will contain all pixel affected
            by a polygon fill operation over the input range.
         */
        BASEGFX_DLLPUBLIC ::basegfx::B2IBox     b2ISurroundingBoxFromB2DRange( const ::basegfx::B2DRange& rRange );

        /** Return smalltest B2DRange with integer values, which
            completely contains given floating point range.

            @param rRange
            Input range.

            @return the closest B2DRange with integer coordinates,
            which completely contains rRange.
         */
        BASEGFX_DLLPUBLIC ::basegfx::B2DRange   b2DSurroundingIntegerRangeFromB2DRange( const ::basegfx::B2DRange& rRange );

    }
}

#endif /* _BGFX_TOOLS_CANVASTOOLS_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
