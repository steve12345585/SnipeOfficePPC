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

#ifndef _SV_SALBMP_H
#define _SV_SALBMP_H

#include <unx/salstd.hxx>
#ifndef _SV_SALGTYPE
#include <vcl/salgtype.hxx>
#endif
#include <unx/saldisp.hxx>
#include <salbmp.hxx>
#include <vclpluginapi.h>
#include <list>

struct  BitmapBuffer;
class   BitmapPalette;
class   SalGraphics;
class   ImplSalDDB;
class   ImplSalBitmapCache;

// -------------
// - SalBitmap -
// -------------

class VCLPLUG_GEN_PUBLIC X11SalBitmap : public SalBitmap
{
private:

    static BitmapBuffer*        ImplCreateDIB(
                                    const Size& rSize,
                                    sal_uInt16 nBitCount,
                                    const BitmapPalette& rPal
                                );

    static BitmapBuffer*        ImplCreateDIB(
                                    Drawable aDrawable,
                                    SalX11Screen nXScreen,
                                    long nDrawableDepth,
                                    long nX,
                                    long nY,
                                    long nWidth,
                                    long nHeight,
                                    bool bGrey
                                );

public:

    static ImplSalBitmapCache*  mpCache;
    static sal_uIntPtr          mnCacheInstCount;

    static void                 ImplCreateCache();
    static void                 ImplDestroyCache();
    void                        ImplRemovedFromCache();

private:

    BitmapBuffer*   mpDIB;
    ImplSalDDB*     mpDDB;
    bool            mbGrey;

public:

    SAL_DLLPRIVATE bool         ImplCreateFromDrawable(
                                    Drawable aDrawable,
                                    SalX11Screen nXScreen,
                                    long nDrawableDepth,
                                    long nX,
                                    long nY,
                                    long nWidth,
                                    long nHeight
                                );

    SAL_DLLPRIVATE XImage*      ImplCreateXImage(
                                    SalDisplay* pSalDisp,
                                    SalX11Screen nXScreen,
                                    long nDepth,
                                    const SalTwoRect& rTwoRect
                                ) const;

    SAL_DLLPRIVATE ImplSalDDB*  ImplGetDDB(
                                    Drawable,
                                    SalX11Screen nXScreen,
                                    long nDrawableDepth,
                                    const SalTwoRect&
                                ) const;

    void                        ImplDraw(
                                    Drawable aDrawable,
                                    SalX11Screen nXScreen,
                                    long nDrawableDepth,
                                    const SalTwoRect& rTwoRect,
                                    const GC& rGC
                                ) const;

public:

                                X11SalBitmap();
    virtual                     ~X11SalBitmap();

    // overload pure virtual methods
    virtual bool                Create(
                                    const Size& rSize,
                                    sal_uInt16 nBitCount,
                                    const BitmapPalette& rPal
                                );

    virtual bool                Create( const SalBitmap& rSalBmp );
    virtual bool                Create(
                                    const SalBitmap& rSalBmp,
                                    SalGraphics* pGraphics
                                );

    virtual bool                Create(
                                    const SalBitmap& rSalBmp,
                                    sal_uInt16 nNewBitCount
                                );

    virtual bool                Create(
                                    const ::com::sun::star::uno::Reference<
                                        ::com::sun::star::rendering::XBitmapCanvas
                                    > xBitmapCanvas,
                                    Size& rSize,
                                    bool bMask = false
                                );

    virtual void                Destroy();

    virtual Size                GetSize() const;
    virtual sal_uInt16          GetBitCount() const;

    virtual BitmapBuffer*       AcquireBuffer( bool bReadOnly );
    virtual void                ReleaseBuffer( BitmapBuffer* pBuffer, bool bReadOnly );
    virtual bool                GetSystemData( BitmapSystemData& rData );
};

// --------------
// - ImplSalDDB -
// --------------

class ImplSalDDB
{
private:

    Pixmap          maPixmap;
    SalTwoRect      maTwoRect;
    long            mnDepth;
    SalX11Screen    mnXScreen;

                    ImplSalDDB() : mnXScreen(0) {}

    static void     ImplDraw(
                        Drawable aSrcDrawable,
                        long nSrcDrawableDepth,
                        Drawable aDstDrawable,
                        long nDstDrawableDepth,
                        long nSrcX,
                        long nSrcY,
                        long nDestWidth,
                        long nDestHeight,
                        long nDestX,
                        long nDestY,
                        const GC& rGC
                    );

public:

                    ImplSalDDB(
                        XImage* pImage,
                        Drawable aDrawable,
                        SalX11Screen nXScreen,
                        const SalTwoRect& rTwoRect
                    );

                    ImplSalDDB(
                        Drawable aDrawable,
                        SalX11Screen nXScreen,
                        long nDrawableDepth,
                        long nX,
                        long nY,
                        long nWidth,
                        long nHeight
                    );

                    ~ImplSalDDB();

    Pixmap          ImplGetPixmap() const { return maPixmap; }
    long            ImplGetWidth() const { return maTwoRect.mnDestWidth; }
    long            ImplGetHeight() const { return maTwoRect.mnDestHeight; }
    long            ImplGetDepth() const { return mnDepth; }
    sal_uIntPtr     ImplGetMemSize() const
                    {
                        return( ( maTwoRect.mnDestWidth * maTwoRect.mnDestHeight * mnDepth ) >> 3 );
                    }
    SalX11Screen    ImplGetScreen() const { return mnXScreen; }

    bool            ImplMatches( SalX11Screen nXScreen, long nDepth, const SalTwoRect& rTwoRect ) const;

    void            ImplDraw(
                        Drawable aDrawable,
                        long nDrawableDepth,
                        const SalTwoRect& rTwoRect,
                        const GC& rGC
                    ) const;
};

// ----------------------
// - ImplSalBitmapCache -
// ----------------------

struct ImplBmpObj;

class ImplSalBitmapCache
{
private:
    typedef ::std::list< ImplBmpObj* > BmpList_impl;

    BmpList_impl    maBmpList;
    sal_uIntPtr     mnTotalSize;

public:

                    ImplSalBitmapCache();
                    ~ImplSalBitmapCache();

    void            ImplAdd( X11SalBitmap* pBmp, sal_uIntPtr nMemSize = 0UL, sal_uIntPtr nFlags = 0UL );
    void            ImplRemove( X11SalBitmap* pBmp );
    void            ImplClear();
};

#endif // _SV_SALBMP_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
