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

#ifndef _SV_WALL_HXX
#define _SV_WALL_HXX

#include <tools/solar.h>
#include <vcl/dllapi.h>
#include <tools/color.hxx>

class Rectangle;
class Gradient;
class BitmapEx;
class ImplWallpaper;

// -------------------
// - Wallpaper-Types -
// -------------------

#define WALLPAPER_NULL                  WallpaperStyle_NULL
#define WALLPAPER_TILE                  WallpaperStyle_TILE
#define WALLPAPER_CENTER                WallpaperStyle_CENTER
#define WALLPAPER_SCALE                 WallpaperStyle_SCALE
#define WALLPAPER_TOPLEFT               WallpaperStyle_TOPLEFT
#define WALLPAPER_TOP                   WallpaperStyle_TOP
#define WALLPAPER_TOPRIGHT              WallpaperStyle_TOPRIGHT
#define WALLPAPER_LEFT                  WallpaperStyle_LEFT
#define WALLPAPER_RIGHT                 WallpaperStyle_RIGHT
#define WALLPAPER_BOTTOMLEFT            WallpaperStyle_BOTTOMLEFT
#define WALLPAPER_BOTTOM                WallpaperStyle_BOTTOM
#define WALLPAPER_BOTTOMRIGHT           WallpaperStyle_BOTTOMRIGHT
#define WALLPAPER_APPLICATIONGRADIENT   WallpaperStyle_APPLICATIONGRADIENT
#define WALLPAPER_FORCE_EQUAL_SIZE      WallpaperStyle_FORCE_EQUAL_SIZE

#ifndef ENUM_WALLPAPERSTYLE_DECLARED
#define ENUM_WALLPAPERSTYLE_DECLARED

enum WallpaperStyle
{
    WALLPAPER_NULL,
    WALLPAPER_TILE,
    WALLPAPER_CENTER,
    WALLPAPER_SCALE,
    WALLPAPER_TOPLEFT,
    WALLPAPER_TOP,
    WALLPAPER_TOPRIGHT,
    WALLPAPER_LEFT,
    WALLPAPER_RIGHT,
    WALLPAPER_BOTTOMLEFT,
    WALLPAPER_BOTTOM,
    WALLPAPER_BOTTOMRIGHT,
    WALLPAPER_APPLICATIONGRADIENT,          // defines a gradient that internally covers the whole application
                                            // and uses a color derived from the face color
    WALLPAPER_FORCE_EQUAL_SIZE = 0x7fffffff
};

#endif

// -------------
// - Wallpaper -
// -------------

class VCL_DLLPUBLIC Wallpaper
{
private:
    ImplWallpaper*  mpImplWallpaper;

    SAL_DLLPRIVATE void           ImplMakeUnique( sal_Bool bReleaseCache = sal_True );
    SAL_DLLPRIVATE Gradient       ImplGetApplicationGradient() const;

public:
    SAL_DLLPRIVATE ImplWallpaper* ImplGetImpWallpaper() const { return mpImplWallpaper; }


public:
                    Wallpaper();
                    Wallpaper( const Wallpaper& rWallpaper );
                    Wallpaper( const Color& rColor );
                    Wallpaper( const BitmapEx& rBmpEx );
                    Wallpaper( const Gradient& rGradient );
                    ~Wallpaper();

    void            SetColor( const Color& rColor );
    const Color&    GetColor() const;

    void            SetStyle( WallpaperStyle eStyle );
    WallpaperStyle  GetStyle() const;

    void            SetBitmap( const BitmapEx& rBitmap );
    BitmapEx        GetBitmap() const;
    sal_Bool            IsBitmap() const;

    void            SetGradient( const Gradient& rGradient );
    Gradient        GetGradient() const;
    sal_Bool            IsGradient() const;

    void            SetRect( const Rectangle& rRect );
    Rectangle       GetRect() const;
    sal_Bool            IsRect() const;

    sal_Bool            IsFixed() const;
    sal_Bool            IsScrollable() const;

    Wallpaper&      operator=( const Wallpaper& rWallpaper );
    sal_Bool            operator==( const Wallpaper& rWallpaper ) const;
    sal_Bool            operator!=( const Wallpaper& rWallpaper ) const
                        { return !(Wallpaper::operator==( rWallpaper )); }
    sal_Bool            IsSameInstance( const Wallpaper& rWallpaper ) const
                        { return (mpImplWallpaper == rWallpaper.mpImplWallpaper); }

    friend VCL_DLLPUBLIC SvStream& operator>>( SvStream& rIStm, Wallpaper& rWallpaper );
    friend VCL_DLLPUBLIC SvStream& operator<<( SvStream& rOStm, const Wallpaper& rWallpaper );
};

#endif  // _SV_WALL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
