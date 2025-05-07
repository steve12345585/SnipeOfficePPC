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

#include <basegfx/point/b3ipoint.hxx>
#include <basegfx/matrix/b3dhommatrix.hxx>
#include <basegfx/numeric/ftools.hxx>

namespace basegfx
{
    B3IPoint& B3IPoint::operator*=( const ::basegfx::B3DHomMatrix& rMat )
    {
        double fTempX(
            rMat.get(0, 0) * mnX +
            rMat.get(0, 1) * mnY +
            rMat.get(0, 2) * mnZ +
            rMat.get(0, 3));
        double fTempY(
            rMat.get(1, 0) * mnX +
            rMat.get(1, 1) * mnY +
            rMat.get(1, 2) * mnZ +
            rMat.get(1, 3));
        double fTempZ(
            rMat.get(2, 0) * mnX +
            rMat.get(2, 1) * mnY +
            rMat.get(2, 2) * mnZ +
            rMat.get(2, 3));

        if(!rMat.isLastLineDefault())
        {
            const double fOne(1.0);
            const double fTempM(
                rMat.get(3, 0) * mnX +
                rMat.get(3, 1) * mnY +
                rMat.get(3, 2) * mnZ +
                rMat.get(3, 3));

            if(!fTools::equalZero(fTempM) && !fTools::equal(fOne, fTempM))
            {
                fTempX /= fTempM;
                fTempY /= fTempM;
                fTempZ /= fTempM;
            }
        }

        mnX = fround(fTempX);
        mnY = fround(fTempY);
        mnZ = fround(fTempZ);

        return *this;
    }
} // end of namespace basegfx

// eof

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
