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

#include <com/sun/star/drawing/LineJoint.hpp>
#include <com/sun/star/drawing/LineCap.hpp>
#include <com/sun/star/uno/Any.hxx>

#include <svx/dialogs.hrc>
#include "svx/xattr.hxx"
#include <svx/xtable.hxx>
#include <svx/dialmgr.hxx>
#include <editeng/itemtype.hxx>
#include <svx/xdef.hxx>

/************************************************************************/

//------------------------------
// class XLineTransparenceItem
//------------------------------
TYPEINIT1_AUTOFACTORY(XLineTransparenceItem, SfxUInt16Item);

/*************************************************************************
|*
|*    XLineTransparenceItem::XLineTransparenceItem(sal_uInt16)
|*
*************************************************************************/

XLineTransparenceItem::XLineTransparenceItem(sal_uInt16 nLineTransparence) :
    SfxUInt16Item(XATTR_LINETRANSPARENCE, nLineTransparence)
{
}

/*************************************************************************
|*
|*    XLineTransparenceItem::XLineTransparenceItem(SvStream& rIn)
|*
*************************************************************************/

XLineTransparenceItem::XLineTransparenceItem(SvStream& rIn) :
    SfxUInt16Item(XATTR_LINETRANSPARENCE, rIn)
{
}

/*************************************************************************
|*
|*    XLineTransparenceItem::Clone(SfxItemPool* pPool) const
|*
*************************************************************************/

SfxPoolItem* XLineTransparenceItem::Clone(SfxItemPool* /*pPool*/) const
{
    return new XLineTransparenceItem(*this);
}

/*************************************************************************
|*
|*    SfxPoolItem* XLineTransparenceItem::Create(SvStream& rIn, sal_uInt16 nVer) const
|*
*************************************************************************/

SfxPoolItem* XLineTransparenceItem::Create(SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XLineTransparenceItem(rIn);
}

//------------------------------------------------------------------------

SfxItemPresentation XLineTransparenceItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
            rText = XubString( ResId( RID_SVXSTR_TRANSPARENCE, DIALOG_MGR() ) );
            rText.AppendAscii(": ");
        case SFX_ITEM_PRESENTATION_NAMELESS:
            rText += XubString( UniString::CreateFromInt32((sal_uInt16) GetValue()) );
            rText += sal_Unicode('%');
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}

//-----------------------
// class XLineJointItem -
//-----------------------

TYPEINIT1_AUTOFACTORY(XLineJointItem, SfxEnumItem);

// -----------------------------------------------------------------------------

XLineJointItem::XLineJointItem( XLineJoint eLineJoint ) :
    SfxEnumItem(XATTR_LINEJOINT, sal::static_int_cast< sal_uInt16 >(eLineJoint))
{
}

// -----------------------------------------------------------------------------

XLineJointItem::XLineJointItem( SvStream& rIn ) :
    SfxEnumItem( XATTR_LINEJOINT, rIn )
{
}

// -----------------------------------------------------------------------------

sal_uInt16 XLineJointItem::GetVersion( sal_uInt16 /*nFileFormatVersion*/) const
{
    return 1;
}

// -----------------------------------------------------------------------------

SfxPoolItem* XLineJointItem::Create( SvStream& rIn, sal_uInt16 nVer ) const
{
    XLineJointItem* pRet = new XLineJointItem( rIn );

    if(nVer < 1)
        pRet->SetValue(XLINEJOINT_ROUND);

    return pRet;
}

// -----------------------------------------------------------------------------

SfxPoolItem* XLineJointItem::Clone(SfxItemPool* /*pPool*/) const
{
    return new XLineJointItem( *this );
}

// -----------------------------------------------------------------------------

SfxItemPresentation XLineJointItem::GetPresentation( SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/,
                                                     SfxMapUnit /*ePresUnit*/, XubString& rText, const IntlWrapper*) const
{
    rText.Erase();

    switch( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE: return ePres;

        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
        {
            sal_uInt16 nId = 0;

            switch( GetValue() )
            {
                case( XLINEJOINT_NONE ):
                    nId = RID_SVXSTR_LINEJOINT_NONE;
                break;

                case( XLINEJOINT_MIDDLE ):
                    nId = RID_SVXSTR_LINEJOINT_MIDDLE;
                break;


                case( XLINEJOINT_BEVEL ):
                    nId = RID_SVXSTR_LINEJOINT_BEVEL;
                break;


                case( XLINEJOINT_MITER ):
                    nId = RID_SVXSTR_LINEJOINT_MITER;
                break;


                case( XLINEJOINT_ROUND ):
                    nId = RID_SVXSTR_LINEJOINT_ROUND;
                break;
            }

            if( nId )
                rText = SVX_RESSTR( nId );

            return ePres;
        }
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}

// -----------------------------------------------------------------------------

bool XLineJointItem::QueryValue( ::com::sun::star::uno::Any& rVal, sal_uInt8 /*nMemberId*/) const
{
    ::com::sun::star::drawing::LineJoint eJoint = ::com::sun::star::drawing::LineJoint_NONE;

    switch( GetValue() )
    {
    case XLINEJOINT_NONE:
        break;
    case XLINEJOINT_MIDDLE:
        eJoint = ::com::sun::star::drawing::LineJoint_MIDDLE;
        break;
    case XLINEJOINT_BEVEL:
        eJoint = ::com::sun::star::drawing::LineJoint_BEVEL;
        break;
    case XLINEJOINT_MITER:
        eJoint = ::com::sun::star::drawing::LineJoint_MITER;
        break;
    case XLINEJOINT_ROUND:
        eJoint = ::com::sun::star::drawing::LineJoint_ROUND;
        break;
    default:
        OSL_FAIL( "Unknown LineJoint enum value!" );
    }

    rVal <<= eJoint;
    return true;
}

// -----------------------------------------------------------------------------

bool XLineJointItem::PutValue( const ::com::sun::star::uno::Any& rVal, sal_uInt8 /*nMemberId*/)
{
    XLineJoint eJoint = XLINEJOINT_NONE;
    ::com::sun::star::drawing::LineJoint eUnoJoint;

    if(!(rVal >>= eUnoJoint))
    {
        // also try an int (for Basic)
        sal_Int32 nLJ = 0;
        if(!(rVal >>= nLJ))
            return false;
        eUnoJoint = (::com::sun::star::drawing::LineJoint)nLJ;
    }

    switch( eUnoJoint )
    {
    case ::com::sun::star::drawing::LineJoint_MIDDLE:
        eJoint = XLINEJOINT_MIDDLE;
        break;
    case ::com::sun::star::drawing::LineJoint_BEVEL:
        eJoint = XLINEJOINT_BEVEL;
        break;
    case ::com::sun::star::drawing::LineJoint_MITER:
        eJoint = XLINEJOINT_MITER;
        break;
    case ::com::sun::star::drawing::LineJoint_ROUND:
        eJoint = XLINEJOINT_ROUND;
        break;
    default:
        break;
    }

    SetValue( sal::static_int_cast< sal_uInt16 >( eJoint ) );

    return true;
}

// -----------------------------------------------------------------------------

sal_uInt16 XLineJointItem::GetValueCount() const
{
    // don't forget to update the api interface also
    return 5;
}

//-----------------------
// class XLineCapItem -
//-----------------------

TYPEINIT1_AUTOFACTORY(XLineCapItem, SfxEnumItem);

// -----------------------------------------------------------------------------

XLineCapItem::XLineCapItem(com::sun::star::drawing::LineCap eLineCap)
:   SfxEnumItem(XATTR_LINECAP, sal::static_int_cast< sal_uInt16 >(eLineCap))
{
}

// -----------------------------------------------------------------------------

XLineCapItem::XLineCapItem( SvStream& rIn )
:   SfxEnumItem(XATTR_LINECAP, rIn)
{
}

// -----------------------------------------------------------------------------

sal_uInt16 XLineCapItem::GetVersion( sal_uInt16 /*nFileFormatVersion*/) const
{
    return 1;
}

// -----------------------------------------------------------------------------

SfxPoolItem* XLineCapItem::Create( SvStream& rIn, sal_uInt16 nVer ) const
{
    XLineCapItem* pRet = new XLineCapItem( rIn );

    if(nVer < 1)
        pRet->SetValue(com::sun::star::drawing::LineCap_BUTT);

    return pRet;
}

// -----------------------------------------------------------------------------

SfxPoolItem* XLineCapItem::Clone(SfxItemPool* /*pPool*/) const
{
    return new XLineCapItem( *this );
}

// -----------------------------------------------------------------------------

SfxItemPresentation XLineCapItem::GetPresentation( SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/,
                                                     SfxMapUnit /*ePresUnit*/, XubString& rText, const IntlWrapper*) const
{
    rText.Erase();

    switch( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE: return ePres;

        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
        {
            sal_uInt16 nId = 0;

            switch( GetValue() )
            {
                default: /*com::sun::star::drawing::LineCap_BUTT*/
                    nId = RID_SVXSTR_LINECAP_BUTT;
                break;

                case(com::sun::star::drawing::LineCap_ROUND):
                    nId = RID_SVXSTR_LINECAP_ROUND;
                break;

                case(com::sun::star::drawing::LineCap_SQUARE):
                    nId = RID_SVXSTR_LINECAP_SQUARE;
                break;
            }

            if( nId )
                rText = SVX_RESSTR( nId );

            return ePres;
        }
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}

// -----------------------------------------------------------------------------

bool XLineCapItem::QueryValue( ::com::sun::star::uno::Any& rVal, sal_uInt8 /*nMemberId*/) const
{
    const com::sun::star::drawing::LineCap eCap(GetValue());
    rVal <<= eCap;
    return true;
}

// -----------------------------------------------------------------------------

bool XLineCapItem::PutValue( const ::com::sun::star::uno::Any& rVal, sal_uInt8 /*nMemberId*/)
{
    com::sun::star::drawing::LineCap eUnoCap;

    if(!(rVal >>= eUnoCap))
    {
        // also try an int (for Basic)
        sal_Int32 nLJ(0);

        if(!(rVal >>= nLJ))
        {
            return false;
        }

        eUnoCap = (com::sun::star::drawing::LineCap)nLJ;
    }

    OSL_ENSURE(com::sun::star::drawing::LineCap_BUTT == eUnoCap
        || com::sun::star::drawing::LineCap_ROUND == eUnoCap
        || com::sun::star::drawing::LineCap_SQUARE == eUnoCap, "Unknown enum value in XATTR_LINECAP (!)");

    SetValue(sal::static_int_cast< sal_uInt16 >(eUnoCap));

    return true;
}

// -----------------------------------------------------------------------------

sal_uInt16 XLineCapItem::GetValueCount() const
{
    // don't forget to update the api interface also
    return 3;
}

// -----------------------------------------------------------------------------

com::sun::star::drawing::LineCap XLineCapItem::GetValue() const
{
    const com::sun::star::drawing::LineCap eRetval((com::sun::star::drawing::LineCap)SfxEnumItem::GetValue());
    OSL_ENSURE(com::sun::star::drawing::LineCap_BUTT == eRetval
        || com::sun::star::drawing::LineCap_ROUND == eRetval
        || com::sun::star::drawing::LineCap_SQUARE == eRetval, "Unknown enum value in XATTR_LINECAP (!)");

    return eRetval;
}

//------------------------------
// class XFillTransparenceItem
//------------------------------
TYPEINIT1_AUTOFACTORY(XFillTransparenceItem, SfxUInt16Item);

/*************************************************************************
|*
|*    XFillTransparenceItem::XFillTransparenceItem(sal_uInt16)
|*
*************************************************************************/

XFillTransparenceItem::XFillTransparenceItem(sal_uInt16 nFillTransparence) :
    SfxUInt16Item(XATTR_FILLTRANSPARENCE, nFillTransparence)
{
}

/*************************************************************************
|*
|*    XFillTransparenceItem::XFillTransparenceItem(SvStream& rIn)
|*
*************************************************************************/

XFillTransparenceItem::XFillTransparenceItem(SvStream& rIn) :
    SfxUInt16Item(XATTR_FILLTRANSPARENCE, rIn)
{
}

/*************************************************************************
|*
|*    XFillTransparenceItem::Clone(SfxItemPool* pPool) const
|*
*************************************************************************/

SfxPoolItem* XFillTransparenceItem::Clone(SfxItemPool* /*pPool*/) const
{
    return new XFillTransparenceItem(*this);
}

/*************************************************************************
|*
|*    SfxPoolItem* XFillTransparenceItem::Create(SvStream& rIn, sal_uInt16 nVer) const
|*
*************************************************************************/

SfxPoolItem* XFillTransparenceItem::Create(SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillTransparenceItem(rIn);
}

//------------------------------------------------------------------------

SfxItemPresentation XFillTransparenceItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
            rText = XubString( ResId( RID_SVXSTR_TRANSPARENCE, DIALOG_MGR() ) );
            rText.AppendAscii(": ");
        case SFX_ITEM_PRESENTATION_NAMELESS:
            rText += XubString( UniString::CreateFromInt32((sal_uInt16) GetValue() ));
            rText += sal_Unicode('%');
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}

//------------------------------
// class XFormTextShadowTranspItem
//------------------------------
TYPEINIT1_AUTOFACTORY(XFormTextShadowTranspItem, SfxUInt16Item);

/*************************************************************************
|*
|*    XFormTextShadowTranspItem::XFormTextShadowTranspItem(sal_uInt16)
|*
*************************************************************************/

XFormTextShadowTranspItem::XFormTextShadowTranspItem(sal_uInt16 nShdwTransparence) :
    SfxUInt16Item(XATTR_FORMTXTSHDWTRANSP, nShdwTransparence)
{
}

/*************************************************************************
|*
|*    XFormTextShadowTranspItem::XFormTextShadowTranspItem(SvStream& rIn)
|*
*************************************************************************/

XFormTextShadowTranspItem::XFormTextShadowTranspItem(SvStream& rIn) :
    SfxUInt16Item(XATTR_FORMTXTSHDWTRANSP, rIn)
{
}

/*************************************************************************
|*
|*    XFormTextShadowTranspItem::Clone(SfxItemPool* pPool) const
|*
*************************************************************************/

SfxPoolItem* XFormTextShadowTranspItem::Clone(SfxItemPool* /*pPool*/) const
{
    return new XFormTextShadowTranspItem(*this);
}

/*************************************************************************
|*
|*    SfxPoolItem* XFormTextShadowTranspItem::Create(SvStream& rIn, sal_uInt16 nVer) const
|*
*************************************************************************/

SfxPoolItem* XFormTextShadowTranspItem::Create(SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFormTextShadowTranspItem(rIn);
}


//------------------------------
// class XFillGradientStepCountItem
//------------------------------
TYPEINIT1_AUTOFACTORY(XGradientStepCountItem, SfxUInt16Item);

/*************************************************************************
|*
|*    XGradientStepCountItem::XGradientStepCountItem( sal_uInt16 )
|*
*************************************************************************/

XGradientStepCountItem::XGradientStepCountItem( sal_uInt16 nStepCount ) :
    SfxUInt16Item( XATTR_GRADIENTSTEPCOUNT, nStepCount )
{
}

/*************************************************************************
|*
|*    XGradientStepCountItem::XGradientStepCountItem( SvStream& rIn )
|*
*************************************************************************/

XGradientStepCountItem::XGradientStepCountItem( SvStream& rIn ) :
    SfxUInt16Item( XATTR_GRADIENTSTEPCOUNT, rIn )
{
}

/*************************************************************************
|*
|*    XGradientStepCountItem::Clone( SfxItemPool* pPool ) const
|*
*************************************************************************/

SfxPoolItem* XGradientStepCountItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XGradientStepCountItem( *this );
}

/*************************************************************************
|*
|*    SfxPoolItem* XGradientStepCountItem::Create(SvStream& rIn, sal_uInt16 nVer) const
|*
*************************************************************************/

SfxPoolItem* XGradientStepCountItem::Create(SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XGradientStepCountItem( rIn );
}

//------------------------------------------------------------------------

SfxItemPresentation XGradientStepCountItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
//          rText = XubString( ResId( RID_SVXSTR_GRADIENTSTEPCOUNT, DIALOG_MGR() ) );
//          rText += ": ";
        case SFX_ITEM_PRESENTATION_NAMELESS:
            rText += XubString( UniString::CreateFromInt32((sal_uInt16) GetValue() ));
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}


//------------------------------
// class XFillBmpTileItem
//------------------------------
TYPEINIT1_AUTOFACTORY( XFillBmpTileItem, SfxBoolItem );

XFillBmpTileItem::XFillBmpTileItem( sal_Bool bTile ) :
            SfxBoolItem( XATTR_FILLBMP_TILE, bTile )
{
}

XFillBmpTileItem::XFillBmpTileItem( SvStream& rIn ) :
            SfxBoolItem( XATTR_FILLBMP_TILE, rIn )
{
}

SfxPoolItem* XFillBmpTileItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBmpTileItem( *this );
}

SfxPoolItem* XFillBmpTileItem::Create( SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBmpTileItem( rIn );
}

SfxItemPresentation XFillBmpTileItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}


//------------------------------
// class XFillBmpTilePosItem
//------------------------------
TYPEINIT1_AUTOFACTORY( XFillBmpPosItem, SfxEnumItem );

XFillBmpPosItem::XFillBmpPosItem( RECT_POINT eRP ) :
    SfxEnumItem( XATTR_FILLBMP_POS, sal::static_int_cast< sal_uInt16 >( eRP ) )
{
}

XFillBmpPosItem::XFillBmpPosItem( SvStream& rIn ) :
            SfxEnumItem( XATTR_FILLBMP_POS, rIn )
{
}

SfxPoolItem* XFillBmpPosItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBmpPosItem( *this );
}

SfxPoolItem* XFillBmpPosItem::Create( SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBmpPosItem( rIn );
}

SfxItemPresentation XFillBmpPosItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}

sal_uInt16 XFillBmpPosItem::GetValueCount() const
{
    return 9;
}


//------------------------------
// class XFillBmpTileSizeXItem
//------------------------------
TYPEINIT1_AUTOFACTORY( XFillBmpSizeXItem, SfxMetricItem );

XFillBmpSizeXItem::XFillBmpSizeXItem( long nSizeX ) :
            SfxMetricItem( XATTR_FILLBMP_SIZEX, nSizeX )
{
}

XFillBmpSizeXItem::XFillBmpSizeXItem( SvStream& rIn ) :
            SfxMetricItem( XATTR_FILLBMP_SIZEX, rIn )
{
}

SfxPoolItem* XFillBmpSizeXItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBmpSizeXItem( *this );
}

SfxPoolItem* XFillBmpSizeXItem::Create( SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBmpSizeXItem( rIn );
}

SfxItemPresentation XFillBmpSizeXItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}

bool XFillBmpSizeXItem::HasMetrics() const
{
    return GetValue() > 0L;
}


//------------------------------
// class XFillBmpTileSizeYItem
//------------------------------
TYPEINIT1_AUTOFACTORY( XFillBmpSizeYItem, SfxMetricItem );

XFillBmpSizeYItem::XFillBmpSizeYItem( long nSizeY ) :
            SfxMetricItem( XATTR_FILLBMP_SIZEY, nSizeY )
{
}

XFillBmpSizeYItem::XFillBmpSizeYItem( SvStream& rIn ) :
            SfxMetricItem( XATTR_FILLBMP_SIZEY, rIn )
{
}

SfxPoolItem* XFillBmpSizeYItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBmpSizeYItem( *this );
}

SfxPoolItem* XFillBmpSizeYItem::Create( SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBmpSizeYItem( rIn );
}

SfxItemPresentation XFillBmpSizeYItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}

bool XFillBmpSizeYItem::HasMetrics() const
{
    return GetValue() > 0L;
}


//------------------------------
// class XFillBmpTileLogItem
//------------------------------
TYPEINIT1_AUTOFACTORY( XFillBmpSizeLogItem, SfxBoolItem );

XFillBmpSizeLogItem::XFillBmpSizeLogItem( sal_Bool bLog ) :
            SfxBoolItem( XATTR_FILLBMP_SIZELOG, bLog )
{
}

XFillBmpSizeLogItem::XFillBmpSizeLogItem( SvStream& rIn ) :
            SfxBoolItem( XATTR_FILLBMP_SIZELOG, rIn )
{
}

SfxPoolItem* XFillBmpSizeLogItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBmpSizeLogItem( *this );
}

SfxPoolItem* XFillBmpSizeLogItem::Create( SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBmpSizeLogItem( rIn );
}

SfxItemPresentation XFillBmpSizeLogItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}


//------------------------------
// class XFillBmpTileOffXItem
//------------------------------
TYPEINIT1_AUTOFACTORY( XFillBmpTileOffsetXItem, SfxUInt16Item );

XFillBmpTileOffsetXItem::XFillBmpTileOffsetXItem( sal_uInt16 nOffX ) :
            SfxUInt16Item( XATTR_FILLBMP_TILEOFFSETX, nOffX )
{
}

XFillBmpTileOffsetXItem::XFillBmpTileOffsetXItem( SvStream& rIn ) :
            SfxUInt16Item( XATTR_FILLBMP_TILEOFFSETX, rIn )
{
}

SfxPoolItem* XFillBmpTileOffsetXItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBmpTileOffsetXItem( *this );
}

SfxPoolItem* XFillBmpTileOffsetXItem::Create( SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBmpTileOffsetXItem( rIn );
}

SfxItemPresentation XFillBmpTileOffsetXItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}


//------------------------------
// class XFillBmpTileOffYItem
//------------------------------
TYPEINIT1_AUTOFACTORY( XFillBmpTileOffsetYItem, SfxUInt16Item );

XFillBmpTileOffsetYItem::XFillBmpTileOffsetYItem( sal_uInt16 nOffY ) :
            SfxUInt16Item( XATTR_FILLBMP_TILEOFFSETY, nOffY )
{
}

XFillBmpTileOffsetYItem::XFillBmpTileOffsetYItem( SvStream& rIn ) :
            SfxUInt16Item( XATTR_FILLBMP_TILEOFFSETY, rIn )
{
}

SfxPoolItem* XFillBmpTileOffsetYItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBmpTileOffsetYItem( *this );
}

SfxPoolItem* XFillBmpTileOffsetYItem::Create( SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBmpTileOffsetYItem( rIn );
}

SfxItemPresentation XFillBmpTileOffsetYItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}


//------------------------------
// class XFillBmpStretchItem
//------------------------------
TYPEINIT1_AUTOFACTORY( XFillBmpStretchItem, SfxBoolItem );

XFillBmpStretchItem::XFillBmpStretchItem( sal_Bool bStretch ) :
            SfxBoolItem( XATTR_FILLBMP_STRETCH, bStretch )
{
}

XFillBmpStretchItem::XFillBmpStretchItem( SvStream& rIn ) :
            SfxBoolItem( XATTR_FILLBMP_STRETCH, rIn )
{
}

SfxPoolItem* XFillBmpStretchItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBmpStretchItem( *this );
}

SfxPoolItem* XFillBmpStretchItem::Create( SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBmpStretchItem( rIn );
}

SfxItemPresentation XFillBmpStretchItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}


//------------------------------
// class XFillBmpTileOffPosXItem
//------------------------------
TYPEINIT1_AUTOFACTORY( XFillBmpPosOffsetXItem, SfxUInt16Item );

XFillBmpPosOffsetXItem::XFillBmpPosOffsetXItem( sal_uInt16 nOffPosX ) :
            SfxUInt16Item( XATTR_FILLBMP_POSOFFSETX, nOffPosX )
{
}

XFillBmpPosOffsetXItem::XFillBmpPosOffsetXItem( SvStream& rIn ) :
            SfxUInt16Item( XATTR_FILLBMP_POSOFFSETX, rIn )
{
}

SfxPoolItem* XFillBmpPosOffsetXItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBmpPosOffsetXItem( *this );
}

SfxPoolItem* XFillBmpPosOffsetXItem::Create( SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBmpPosOffsetXItem( rIn );
}

SfxItemPresentation XFillBmpPosOffsetXItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}


//------------------------------
// class XFillBmpTileOffPosYItem
//------------------------------
TYPEINIT1_AUTOFACTORY( XFillBmpPosOffsetYItem, SfxUInt16Item );

XFillBmpPosOffsetYItem::XFillBmpPosOffsetYItem( sal_uInt16 nOffPosY ) :
            SfxUInt16Item( XATTR_FILLBMP_POSOFFSETY, nOffPosY )
{
}

XFillBmpPosOffsetYItem::XFillBmpPosOffsetYItem( SvStream& rIn ) :
            SfxUInt16Item( XATTR_FILLBMP_POSOFFSETY, rIn )
{
}

SfxPoolItem* XFillBmpPosOffsetYItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBmpPosOffsetYItem( *this );
}

SfxPoolItem* XFillBmpPosOffsetYItem::Create( SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBmpPosOffsetYItem( rIn );
}

SfxItemPresentation XFillBmpPosOffsetYItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;
        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}

//--------------------------
// class XFillBackgroundItem
//--------------------------
TYPEINIT1_AUTOFACTORY(XFillBackgroundItem, SfxBoolItem);

/*************************************************************************
|*
|*    XFillBackgroundItem::XFillBackgroundItem( sal_Bool )
|*
*************************************************************************/

XFillBackgroundItem::XFillBackgroundItem( sal_Bool bFill ) :
    SfxBoolItem( XATTR_FILLBACKGROUND, bFill )
{
}

/*************************************************************************
|*
|*    XFillBackgroundItem::XFillBackgroundItem( SvStream& rIn )
|*
*************************************************************************/

XFillBackgroundItem::XFillBackgroundItem( SvStream& rIn ) :
    SfxBoolItem( XATTR_FILLBACKGROUND, rIn )
{
}

/*************************************************************************
|*
|*    XFillBackgroundItem::Clone( SfxItemPool* pPool ) const
|*
*************************************************************************/

SfxPoolItem* XFillBackgroundItem::Clone( SfxItemPool* /*pPool*/) const
{
    return new XFillBackgroundItem( *this );
}

/*************************************************************************
|*
|*    SfxPoolItem* XFillBackgroundItem::Create(SvStream& rIn, sal_uInt16 nVer) const
|*
*************************************************************************/

SfxPoolItem* XFillBackgroundItem::Create(SvStream& rIn, sal_uInt16 /*nVer*/) const
{
    return new XFillBackgroundItem( rIn );
}

//------------------------------------------------------------------------

SfxItemPresentation XFillBackgroundItem::GetPresentation( SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/,
                                                          SfxMapUnit /*ePresUnit*/, XubString& rText, const IntlWrapper*) const
{
    rText.Erase();

    switch( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return ePres;

        case SFX_ITEM_PRESENTATION_COMPLETE:
        case SFX_ITEM_PRESENTATION_NAMELESS:
            return ePres;
        default:
            return SFX_ITEM_PRESENTATION_NONE;
    }
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
