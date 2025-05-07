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


#include <basegfx/polygon/b2dpolypolygon.hxx>

#include "implcustomsprite.hxx"
#include "implcanvas.hxx"

using namespace ::com::sun::star;

namespace cppcanvas
{
    namespace internal
    {
        ImplCustomSprite::ImplCustomSprite( const uno::Reference< rendering::XSpriteCanvas >&       rParentCanvas,
                                            const uno::Reference< rendering::XCustomSprite >&       rSprite,
                                            const ImplSpriteCanvas::TransformationArbiterSharedPtr& rTransformArbiter ) :
            ImplSprite( rParentCanvas,
                        uno::Reference< rendering::XSprite >(rSprite,
                                                             uno::UNO_QUERY),
                        rTransformArbiter ),
            mpLastCanvas(),
            mxCustomSprite( rSprite )
        {
            OSL_ENSURE( rParentCanvas.is(), "ImplCustomSprite::ImplCustomSprite(): Invalid canvas" );
            OSL_ENSURE( mxCustomSprite.is(), "ImplCustomSprite::ImplCustomSprite(): Invalid sprite" );
        }

        ImplCustomSprite::~ImplCustomSprite()
        {
        }

        CanvasSharedPtr ImplCustomSprite::getContentCanvas() const
        {
            OSL_ENSURE( mxCustomSprite.is(), "ImplCustomSprite::getContentCanvas(): Invalid sprite" );

            if( !mxCustomSprite.is() )
                return CanvasSharedPtr();

            uno::Reference< rendering::XCanvas > xCanvas( mxCustomSprite->getContentCanvas() );

            if( !xCanvas.is() )
                return CanvasSharedPtr();

            // cache content canvas C++ wrapper
            if( mpLastCanvas.get() == NULL ||
                mpLastCanvas->getUNOCanvas() != xCanvas )
            {
                mpLastCanvas = CanvasSharedPtr( new ImplCanvas( xCanvas ) );
            }

            return mpLastCanvas;
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
