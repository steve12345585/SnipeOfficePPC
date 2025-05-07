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


#include <tools.hxx>


using namespace ::com::sun::star;

namespace cppcanvas
{
    namespace tools
    {
        uno::Sequence< double > intSRGBAToDoubleSequence( const uno::Reference< rendering::XGraphicDevice >&,
                                                          Color::IntSRGBA                                   aColor  )
        {
            uno::Sequence< double > aRes( 4 );

            aRes[0] = getRed(aColor) / 255.0;
            aRes[1] = getGreen(aColor) / 255.0;
            aRes[2] = getBlue(aColor) / 255.0;
            aRes[3] = getAlpha(aColor) / 255.0;

            return aRes;
        }

        Color::IntSRGBA doubleSequenceToIntSRGBA( const uno::Reference< rendering::XGraphicDevice >&,
                                                  const uno::Sequence< double >&                    rColor  )
        {
            return makeColor( static_cast<sal_uInt8>( 255*rColor[0] + .5 ),
                              static_cast<sal_uInt8>( 255*rColor[1] + .5 ),
                              static_cast<sal_uInt8>( 255*rColor[2] + .5 ),
                              static_cast<sal_uInt8>( 255*rColor[3] + .5 ) );
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
