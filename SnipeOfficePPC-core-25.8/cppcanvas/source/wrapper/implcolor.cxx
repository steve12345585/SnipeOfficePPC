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


#include <implcolor.hxx>
#include <tools.hxx>

using namespace ::com::sun::star;

namespace cppcanvas
{
    namespace internal
    {

        ImplColor::ImplColor( const uno::Reference< rendering::XGraphicDevice >& rDevice ) :
            mxDevice( rDevice )
        {
            OSL_ENSURE( mxDevice.is(), "ImplColor::ImplColor(): Invalid graphic device" );
        }

        ImplColor::~ImplColor()
        {
        }

        Color::IntSRGBA ImplColor::getIntSRGBA( uno::Sequence< double >& rDeviceColor ) const
        {
            OSL_ENSURE( mxDevice.is(), "ImplColor::getIntSRGBA(): Invalid graphic device" );

            // TODO(F1): Color space handling
            return tools::doubleSequenceToIntSRGBA( mxDevice, rDeviceColor );
        }

        uno::Sequence< double > ImplColor::getDeviceColor( Color::IntSRGBA aSRGBA ) const
        {
            OSL_ENSURE( mxDevice.is(), "ImplColor::getDeviceColor(): Invalid graphic device" );

            // TODO(F1): Color space handling
            return tools::intSRGBAToDoubleSequence( mxDevice, aSRGBA );
        }

    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
