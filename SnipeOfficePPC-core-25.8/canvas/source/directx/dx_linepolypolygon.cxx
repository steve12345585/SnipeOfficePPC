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


#include <basegfx/tools/canvastools.hxx>
#include "dx_linepolypolygon.hxx"


using namespace ::com::sun::star;

namespace dxcanvas
{
    LinePolyPolygon::LinePolyPolygon( const ::basegfx::B2DPolyPolygon& rPolyPoly ) :
        ::basegfx::unotools::UnoPolyPolygon( rPolyPoly ),
        mpGdiPlusUser( GDIPlusUser::createInstance() ),
        mpPath()
    {
    }

    GraphicsPathSharedPtr LinePolyPolygon::getGraphicsPath( bool bNoLineJoin ) const
    {
        // generate GraphicsPath only on demand (gets deleted as soon
        // as any of the modifying methods above touches the
        // B2DPolyPolygon).
        if( !mpPath )
        {
            mpPath = tools::graphicsPathFromB2DPolyPolygon( getPolyPolygonUnsafe(), bNoLineJoin );
            mpPath->SetFillMode( const_cast<LinePolyPolygon*>(this)->getFillRule() == rendering::FillRule_EVEN_ODD ?
                                 Gdiplus::FillModeAlternate : Gdiplus::FillModeWinding );
        }

        return mpPath;
    }

    void LinePolyPolygon::modifying() const
    {
        mpPath.reset();
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
