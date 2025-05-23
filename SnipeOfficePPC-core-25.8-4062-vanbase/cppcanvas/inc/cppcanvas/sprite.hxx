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

#ifndef _CPPCANVAS_SPRITE_HXX
#define _CPPCANVAS_SPRITE_HXX

#include <com/sun/star/uno/Reference.hxx>

#include <boost/shared_ptr.hpp>

namespace basegfx
{
    class B2DHomMatrix;
    class B2DPolyPolygon;
    class B2DPoint;
}

namespace com { namespace sun { namespace star { namespace rendering
{
    class  XSprite;
} } } }


/* Definition of Sprite class */

namespace cppcanvas
{

    class Sprite
    {
    public:
        virtual ~Sprite() {}

        virtual void setAlpha( const double& rAlpha ) = 0;

        /** Set the sprite position on screen

            This method differs from the XSprite::move() insofar, as
            no viewstate/renderstate transformations are applied to
            the specified position. The given position is interpreted
            in device coordinates (i.e. screen pixel)
         */
        virtual void movePixel( const ::basegfx::B2DPoint& rNewPos ) = 0;

        /** Set the sprite position on screen

            This method sets the sprite position in the view
            coordinate system of the parent canvas
         */
        virtual void move( const ::basegfx::B2DPoint& rNewPos ) = 0;

        virtual void transform( const ::basegfx::B2DHomMatrix& rMatrix ) = 0;

        /** Set output clipping

            This method differs from the XSprite::clip() insofar, as
            no viewstate/renderstate transformations are applied to
            the specified clip polygon. The given polygon is
            interpreted in device coordinates (i.e. screen pixel)
         */
        virtual void setClipPixel( const ::basegfx::B2DPolyPolygon& rClipPoly ) = 0;

        /** Set output clipping

            This method applies the clip poly-polygon interpreted in
            the view coordinate system of the parent canvas.
         */
        virtual void setClip( const ::basegfx::B2DPolyPolygon& rClipPoly ) = 0;

        virtual void setClip() = 0;

        virtual void show() = 0;
        virtual void hide() = 0;

        /** Change the sprite priority

            @param fPriority
            New sprite priority. The higher the priority, the further
            towards the viewer the sprite appears. That is, sprites
            with higher priority appear before ones with lower
            priority.
         */
        virtual void setPriority( double fPriority ) = 0;

        virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::rendering::XSprite > getUNOSprite() const = 0;
    };

    typedef ::boost::shared_ptr< ::cppcanvas::Sprite > SpriteSharedPtr;
}

#endif /* _CPPCANVAS_SPRITE_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
