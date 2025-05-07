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

#include <basegfx/range/b2drange.hxx>
#include <basegfx/range/b2irange.hxx>
#include <basegfx/numeric/ftools.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>

namespace basegfx
{
    B2DRange::B2DRange( const B2IRange& rRange ) :
        maRangeX(),
        maRangeY()
    {
        if( !rRange.isEmpty() )
        {
            maRangeX = rRange.getMinX();
            maRangeY = rRange.getMinY();

            maRangeX.expand(rRange.getMaxX());
            maRangeY.expand(rRange.getMaxY());
        }
    }

    void B2DRange::transform(const B2DHomMatrix& rMatrix)
    {
        if(!isEmpty() && !rMatrix.isIdentity())
        {
            const B2DRange aSource(*this);
            reset();
            expand(rMatrix * B2DPoint(aSource.getMinX(), aSource.getMinY()));
            expand(rMatrix * B2DPoint(aSource.getMaxX(), aSource.getMinY()));
            expand(rMatrix * B2DPoint(aSource.getMinX(), aSource.getMaxY()));
            expand(rMatrix * B2DPoint(aSource.getMaxX(), aSource.getMaxY()));
        }
    }

    B2IRange fround(const B2DRange& rRange)
    {
        return rRange.isEmpty() ?
            B2IRange() :
            B2IRange(fround(rRange.getMinimum()),
                     fround(rRange.getMaximum()));
    }
} // end of namespace basegfx

// eof

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
