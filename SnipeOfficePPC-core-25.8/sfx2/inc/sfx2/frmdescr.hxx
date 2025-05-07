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
#ifndef _SFX_FRMDESCRHXX
#define _SFX_FRMDESCRHXX

#include "sal/config.h"
#include "sfx2/dllapi.h"

#include <sfx2/sfxsids.hrc>
#include <svl/poolitem.hxx>
#include <tools/urlobj.hxx>
#include <tools/string.hxx>
#include <svl/svarray.hxx>
#include <tools/gen.hxx>

class SvStream;
struct SfxFrameDescriptor_Impl;
class SfxFrameDescriptor;
class Wallpaper;

//===========================================================================
// The SfxFrame descriptors build a recursive structure, that covers all the
// required data in-order to display the frame document.
// Through a SfxFrameSetDescriptor access is given to the whole underlying
// structure. Due to the nature of the SfxFrames document, the
// SfxFramesSetDescriptor is not only the content of this document, but also
// describes the view on it. The FrameSet is made up of lines, which in turn,
// contains the actual window . A line can be horizontally or vertically
// aligned, from which also the alignment of the FrameSet is given.
//===========================================================================

enum ScrollingMode
{
    ScrollingYes,
    ScrollingNo,
    ScrollingAuto
};

enum SizeSelector
{
    SIZE_ABS,
    SIZE_PERCENT,
    SIZE_REL
};

#define BORDER_SET          2
#define BORDER_YES          1
#define BORDER_NO           0
#define SPACING_NOT_SET     -1L
#define SIZE_NOT_SET        -1L

class SfxItemSet;
struct SfxFrameProperties;

class SFX2_DLLPUBLIC SfxFrameDescriptor
{
    INetURLObject           aURL;
    INetURLObject           aActualURL;
    String                  aName;
    Size                    aMargin;
    long                    nWidth;
    ScrollingMode           eScroll;
    SizeSelector            eSizeSelector;
    sal_uInt16                  nHasBorder;
    sal_uInt16                  nItemId;
    sal_Bool                    bResizeHorizontal;
    sal_Bool                    bResizeVertical;
    sal_Bool                    bHasUI;
    sal_Bool                    bReadOnly;
    SfxFrameDescriptor_Impl* pImp;

public:
                            SfxFrameDescriptor();
                            ~SfxFrameDescriptor();

                            // FileName/URL
    SfxItemSet*             GetArgs();
    const INetURLObject&    GetURL() const
                            { return aURL; }
    void                    SetURL( const String& rURL );
    const INetURLObject&    GetActualURL() const
                            { return aActualURL; }
    void                    SetActualURL( const INetURLObject& rURL );
    void                    SetActualURL( const String& rURL );
    void                    SetReadOnly( sal_Bool bSet ) { bReadOnly = bSet;}
    sal_Bool                    IsReadOnly(  ) const { return bReadOnly;}
    void                    SetEditable( sal_Bool bSet );
    sal_Bool                    IsEditable() const;

                            // Size
    void                    SetWidth( long n )
                            { nWidth = n; }
    void                    SetWidthPercent( long n )
                            { nWidth = n; eSizeSelector = SIZE_PERCENT; }
    void                    SetWidthRel( long n )
                            { nWidth = n; eSizeSelector = SIZE_REL; }
    void                    SetWidthAbs( long n )
                            { nWidth = n; eSizeSelector = SIZE_ABS; }
    long                    GetWidth() const
                            { return nWidth; }
    SizeSelector            GetSizeSelector() const
                            { return eSizeSelector; }
    sal_Bool                    IsResizable() const
                            { return bResizeHorizontal && bResizeVertical; }
    void                    SetResizable( sal_Bool bRes )
                            { bResizeHorizontal = bResizeVertical = bRes; }

                            // FrameName
    const String&           GetName() const
                            { return aName; }
    void                    SetName( const String& rName )
                            { aName = rName; }

                            // Margin, Scrolling
    const Size&             GetMargin() const
                            { return aMargin; }
    void                    SetMargin( const Size& rMargin )
                            { aMargin = rMargin; }
    ScrollingMode           GetScrollingMode() const
                            { return eScroll; }
    void                    SetScrollingMode( ScrollingMode eMode )
                            { eScroll = eMode; }

                            // FrameBorder
    void                    SetWallpaper( const Wallpaper& rWallpaper );
    sal_Bool                    HasFrameBorder() const;

    sal_Bool                    IsFrameBorderOn() const
                            { return ( nHasBorder & BORDER_YES ) != 0; }

    void                    SetFrameBorder( sal_Bool bBorder )
                            {
                                nHasBorder = bBorder ?
                                            BORDER_YES | BORDER_SET :
                                            BORDER_NO | BORDER_SET;
                            }
    sal_Bool                    IsFrameBorderSet() const
                            { return (nHasBorder & BORDER_SET) != 0; }
    void                    ResetBorder()
                            { nHasBorder = 0; }

    sal_Bool                    HasUI() const
                            { return bHasUI; }
    void                    SetHasUI( sal_Bool bOn )
                            { bHasUI = bOn; }

                            // Attribute for Splitwindow
    sal_uInt16                  GetItemId() const
                            { return nItemId; }
    void                    SetItemId( sal_uInt16 nId )
                            { nItemId = nId; }

                            // Copy for example for Views
    SfxFrameDescriptor*     Clone( sal_Bool bWithIds = sal_True ) const;
};

// No block to implement a =operator
struct SfxFrameProperties
{
    String                              aURL;
    String                              aName;
    long                                lMarginWidth;
    long                                lMarginHeight;
    long                                lSize;
    long                                lSetSize;
    long                                lFrameSpacing;
    long                                lInheritedFrameSpacing;
    ScrollingMode                       eScroll;
    SizeSelector                        eSizeSelector;
    SizeSelector                        eSetSizeSelector;
    sal_Bool                                bHasBorder;
    sal_Bool                                bBorderSet;
    sal_Bool                                bResizable;
    sal_Bool                                bSetResizable;
    sal_Bool                                bIsRootSet;
    sal_Bool                                bIsInColSet;
    sal_Bool                                bHasBorderInherited;
    SfxFrameDescriptor*                 pFrame;

private:
    SfxFrameProperties( SfxFrameProperties& ) {}
public:
                                        SfxFrameProperties()
                                            : lMarginWidth( SIZE_NOT_SET ),
                                              lMarginHeight( SIZE_NOT_SET ),
                                              lSize( 1L ),
                                              lSetSize( 1L ),
                                              lFrameSpacing( SPACING_NOT_SET ),
                                              lInheritedFrameSpacing( SPACING_NOT_SET ),
                                              eScroll( ScrollingAuto ),
                                              eSizeSelector( SIZE_REL ),
                                              eSetSizeSelector( SIZE_REL ),
                                              bHasBorder( sal_True ),
                                              bBorderSet( sal_True ),
                                              bResizable( sal_True ),
                                              bSetResizable( sal_True ),
                                              bIsRootSet( sal_False ),
                                              bIsInColSet( sal_False ),
                                              bHasBorderInherited( sal_True ),
                                              pFrame( 0 ) {}

                                        ~SfxFrameProperties() { delete pFrame; }

    int                                 operator ==( const SfxFrameProperties& ) const;
    SfxFrameProperties&                 operator =( const SfxFrameProperties &rProp );
};

class SfxFrameDescriptorItem : public SfxPoolItem
{
    SfxFrameProperties                  aProperties;
public:
                                        TYPEINFO();

                                        SfxFrameDescriptorItem ( const sal_uInt16 nId = SID_FRAMEDESCRIPTOR )
                                            : SfxPoolItem( nId )
                                        {}

                                        SfxFrameDescriptorItem( const SfxFrameDescriptorItem& rCpy )
                                            : SfxPoolItem( rCpy )
                                        {
                                            aProperties = rCpy.aProperties;
                                        }

    virtual                             ~SfxFrameDescriptorItem();

    virtual int                         operator ==( const SfxPoolItem& ) const;
    SfxFrameDescriptorItem&             operator =( const SfxFrameDescriptorItem & );

    virtual SfxItemPresentation         GetPresentation( SfxItemPresentation ePres,
                                            SfxMapUnit eCoreMetric,
                                            SfxMapUnit ePresMetric,
                                            UniString &rText, const IntlWrapper * = 0 ) const;

    virtual SfxPoolItem*                Clone( SfxItemPool *pPool = 0 ) const;

    const SfxFrameProperties&           GetProperties() const
                                        { return aProperties; }
    void                                SetProperties( const SfxFrameProperties& rProp )
                                        { aProperties = rProp; }
};

#endif // #ifndef _SFX_FRMDESCRHXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
