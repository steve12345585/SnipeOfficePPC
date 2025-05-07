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


#include <canvas/debug.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include "barwipepolypolygon.hxx"


namespace slideshow {
namespace internal {

::basegfx::B2DPolyPolygon BarWipePolyPolygon::operator () ( double t )
{
    ::basegfx::B2DPolyPolygon res;
    ::basegfx::B2DHomMatrix aTransform;
    aTransform.scale( ::basegfx::pruneScaleValue( t / m_nBars ), 1.0 );
    for ( sal_Int32 i = m_nBars; i--; )
    {
        ::basegfx::B2DHomMatrix transform( aTransform );
        transform.translate( static_cast<double>(i) / m_nBars, 0.0 );
        ::basegfx::B2DPolygon poly( m_unitRect );
        poly.transform( transform );
        res.append( poly );
    }
    return res;
}

}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
