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

#ifndef _CPPCANVAS_IMPLSPRITECANVAS_HXX
#define _CPPCANVAS_IMPLSPRITECANVAS_HXX

#include <com/sun/star/rendering/XSpriteCanvas.hpp>
#include <basegfx/vector/b2dsize.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>

#include <boost/shared_ptr.hpp>


#include <cppcanvas/spritecanvas.hxx>

#include <implbitmapcanvas.hxx>


namespace cppcanvas
{
    namespace internal
    {
        class ImplSpriteCanvas : public virtual SpriteCanvas, protected virtual ImplBitmapCanvas
        {
        public:
            ImplSpriteCanvas( const ::com::sun::star::uno::Reference<
                                  ::com::sun::star::rendering::XSpriteCanvas >& rCanvas );
            ImplSpriteCanvas(const ImplSpriteCanvas&);

            virtual ~ImplSpriteCanvas();

            virtual void                    setTransformation( const ::basegfx::B2DHomMatrix& rMatrix );

            virtual bool                    updateScreen( bool bUpdateAll ) const;

            virtual CustomSpriteSharedPtr   createCustomSprite( const ::basegfx::B2DSize& ) const;
            virtual SpriteSharedPtr         createClonedSprite( const SpriteSharedPtr& ) const;

            virtual CanvasSharedPtr         clone() const;

            virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::rendering::XSpriteCanvas >    getUNOSpriteCanvas() const;

            /** This class passes the view transformation
                to child sprites

                This helper class is necessary, because the
                ImplSpriteCanvas object cannot hand out shared ptrs of
                itself, but has somehow pass an object to child
                sprites those can query for the canvas' view transform.
             */
            class TransformationArbiter
            {
            public:
                TransformationArbiter();

                void                        setTransformation( const ::basegfx::B2DHomMatrix& rViewTransform );
                ::basegfx::B2DHomMatrix     getTransformation() const;

            private:
                ::basegfx::B2DHomMatrix     maTransformation;
            };

            typedef ::boost::shared_ptr< TransformationArbiter > TransformationArbiterSharedPtr;

        private:
            // default: disabled assignment
            ImplSpriteCanvas& operator=( const ImplSpriteCanvas& );

            const ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XSpriteCanvas >    mxSpriteCanvas;
            TransformationArbiterSharedPtr                                                          mpTransformArbiter;
        };
    }
}

#endif /* _CPPCANVAS_IMPLSPRITECANVAS_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
