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
#ifndef SW_NOTXTFRM_HXX
#define SW_NOTXTFRM_HXX

#include <cntfrm.hxx>

class SwNoTxtNode;
class OutputDevice;
class SwBorderAttrs;
class Bitmap;
struct SwCrsrMoveState;

class SwNoTxtFrm: public SwCntntFrm
{
    friend void _FrmFinit();

    short    nWeight;                   // importance of the graphic

    const Size& GetSize() const;

    void InitCtor();

    void Format ( const SwBorderAttrs *pAttrs = 0 );
    void PaintCntnt  ( OutputDevice*, const SwRect&, const SwRect& ) const;
    void PaintPicture( OutputDevice*, const SwRect& ) const;
protected:
    virtual void MakeAll();
    virtual void Modify( const SfxPoolItem*, const SfxPoolItem* );
public:
    SwNoTxtFrm( SwNoTxtNode * const, SwFrm* );
    ~SwNoTxtFrm();

    virtual void Paint( SwRect const&,
                        SwPrintData const*const pPrintData = NULL ) const;
    virtual sal_Bool GetCharRect( SwRect &, const SwPosition&,
                              SwCrsrMoveState* = 0) const;
    sal_Bool GetCrsrOfst(SwPosition* pPos, Point& aPoint,
                     SwCrsrMoveState* = 0, bool bTestBackground = false) const;

    const Size &GetGrfSize() const  { return GetSize(); }
    void GetGrfArea( SwRect &rRect, SwRect * = 0, sal_Bool bMirror = sal_True ) const;

    sal_Bool IsTransparent() const;

    void StopAnimation( OutputDevice* = 0 ) const;
    sal_Bool HasAnimation()  const;

    // Routine for the graphics cache
    sal_uInt16 GetWeight() { return nWeight; }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
