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

#include <com/sun/star/style/TabStop.hpp>
#include <com/sun/star/style/LineSpacing.hpp>
#include <com/sun/star/style/LineSpacingMode.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <comphelper/processfactory.hxx>
#include <unotools/syslocale.hxx>
#include <comphelper/types.hxx>

using namespace ::rtl;
using namespace ::com::sun::star;

#include <tools/rtti.hxx>
#include <svl/itempool.hxx>
#include <svl/memberid.hrc>
#include <editeng/editrids.hrc>
#include <editeng/lspcitem.hxx>
#include <editeng/adjitem.hxx>
#include <editeng/orphitem.hxx>
#include <editeng/widwitem.hxx>
#include <editeng/tstpitem.hxx>
#include <editeng/pmdlitem.hxx>
#include <editeng/spltitem.hxx>
#include <editeng/hyznitem.hxx>
#include <editeng/scriptspaceitem.hxx>
#include <editeng/hngpnctitem.hxx>
#include <editeng/forbiddenruleitem.hxx>
#include <editeng/paravertalignitem.hxx>
#include <editeng/pgrditem.hxx>
#include <rtl/ustring.hxx>
#include <editeng/memberids.hrc>
#include <editeng/editids.hrc>
#include <editeng/itemtype.hxx>
#include <editeng/eerdll.hxx>
#include <editeng/paperinf.hxx>
#include <vcl/svapp.hxx>
#include <algorithm>

using namespace ::rtl;
using namespace ::com::sun::star;

// Konvertierung fuer UNO
#define TWIP_TO_MM100(TWIP)     ((TWIP) >= 0 ? (((TWIP)*127L+36L)/72L) : (((TWIP)*127L-36L)/72L))
#define TWIP_TO_MM100_UNSIGNED(TWIP)     ((((TWIP)*127L+36L)/72L))
#define MM100_TO_TWIP(MM100)    ((MM100) >= 0 ? (((MM100)*72L+63L)/127L) : (((MM100)*72L-63L)/127L))
#define MM100_TO_TWIP_UNSIGNED(MM100)    ((((MM100)*72L+63L)/127L))


// STATIC DATA -----------------------------------------------------------


// -----------------------------------------------------------------------


TYPEINIT1_FACTORY(SvxLineSpacingItem, SfxPoolItem , new SvxLineSpacingItem(LINE_SPACE_DEFAULT_HEIGHT, 0));
TYPEINIT1_FACTORY(SvxAdjustItem, SfxPoolItem, new SvxAdjustItem(SVX_ADJUST_LEFT, 0));
TYPEINIT1_FACTORY(SvxWidowsItem, SfxByteItem, new SvxWidowsItem(0, 0));
TYPEINIT1_FACTORY(SvxOrphansItem, SfxByteItem, new SvxOrphansItem(0, 0));
TYPEINIT1_FACTORY(SvxHyphenZoneItem, SfxPoolItem, new SvxHyphenZoneItem(sal_False, 0));
TYPEINIT1_FACTORY(SvxTabStopItem, SfxPoolItem, new SvxTabStopItem(0));
TYPEINIT1_FACTORY(SvxFmtSplitItem, SfxBoolItem, new SvxFmtSplitItem(sal_False, 0));
TYPEINIT1_FACTORY(SvxPageModelItem, SfxStringItem, new SvxPageModelItem(0));
TYPEINIT1_FACTORY(SvxScriptSpaceItem, SfxBoolItem, new SvxScriptSpaceItem(sal_False, 0));
TYPEINIT1_FACTORY(SvxHangingPunctuationItem, SfxBoolItem, new SvxHangingPunctuationItem(sal_False, 0));
TYPEINIT1_FACTORY(SvxForbiddenRuleItem, SfxBoolItem, new SvxForbiddenRuleItem(sal_False, 0));
TYPEINIT1_FACTORY(SvxParaVertAlignItem, SfxUInt16Item, new SvxParaVertAlignItem(0, 0));
TYPEINIT1_FACTORY(SvxParaGridItem, SfxBoolItem, new SvxParaGridItem(sal_True, 0));

// -----------------------------------------------------------------------

SvxLineSpacingItem::SvxLineSpacingItem( sal_uInt16 nHeight, const sal_uInt16 nId )
    : SfxEnumItemInterface( nId )
{
    nPropLineSpace = 100;
    nInterLineSpace = 0;
    nLineHeight = nHeight;
    eLineSpace = SVX_LINE_SPACE_AUTO;
    eInterLineSpace = SVX_INTER_LINE_SPACE_OFF;
}

// -----------------------------------------------------------------------

int SvxLineSpacingItem::operator==( const SfxPoolItem& rAttr ) const
{
    DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unequal types" );

    const SvxLineSpacingItem& rLineSpace = (const SvxLineSpacingItem&)rAttr;
    return (
        // Same Linespacing Rule?
        (eLineSpace == rLineSpace.eLineSpace)
        // For maximum and minimum Linespacing be the size must coincide.
        && (eLineSpace == SVX_LINE_SPACE_AUTO ||
            nLineHeight == rLineSpace.nLineHeight)
        // Same Linespacing Rule?
        && ( eInterLineSpace == rLineSpace.eInterLineSpace )
        // Either set proportional or additive.
        && (( eInterLineSpace == SVX_INTER_LINE_SPACE_OFF)
            || (eInterLineSpace == SVX_INTER_LINE_SPACE_PROP
                && nPropLineSpace == rLineSpace.nPropLineSpace)
            || (eInterLineSpace == SVX_INTER_LINE_SPACE_FIX
                && (nInterLineSpace == rLineSpace.nInterLineSpace)))) ?
                1 : 0;
}

/* Who does still know why the LineSpacingItem is so complicated?
   We can not use it for UNO since there are only two values:
      - ein sal_uInt16 for the mode
      - ein sal_uInt32 for all values (distance, height, rel. detail)
*/
bool SvxLineSpacingItem::QueryValue( uno::Any& rVal, sal_uInt8 nMemberId ) const
{
    sal_Bool bConvert = 0!=(nMemberId&CONVERT_TWIPS);
    nMemberId &= ~CONVERT_TWIPS;

    style::LineSpacing aLSp;
    switch( eLineSpace )
    {
        case SVX_LINE_SPACE_AUTO:
            if(eInterLineSpace == SVX_INTER_LINE_SPACE_FIX)
            {
                aLSp.Mode = style::LineSpacingMode::LEADING;
                aLSp.Height = ( bConvert ? (short)TWIP_TO_MM100(nInterLineSpace) : nInterLineSpace);
            }
            else if(eInterLineSpace == SVX_INTER_LINE_SPACE_OFF)
            {
                aLSp.Mode = style::LineSpacingMode::PROP;
                aLSp.Height = 100;
            }
            else
            {
                aLSp.Mode = style::LineSpacingMode::PROP;
                aLSp.Height = nPropLineSpace;
            }
        break;
        case SVX_LINE_SPACE_FIX :
        case SVX_LINE_SPACE_MIN :
            aLSp.Mode = eLineSpace == SVX_LINE_SPACE_FIX ? style::LineSpacingMode::FIX : style::LineSpacingMode::MINIMUM;
            aLSp.Height = ( bConvert ? (short)TWIP_TO_MM100_UNSIGNED(nLineHeight) : nLineHeight );
        break;
        default:
            ;//prevent warning about SVX_LINE_SPACE_END
    }

    switch ( nMemberId )
    {
        case 0 :                rVal <<= aLSp; break;
        case MID_LINESPACE :    rVal <<= aLSp.Mode; break;
        case MID_HEIGHT :       rVal <<= aLSp.Height; break;
        default: OSL_FAIL("Wrong MemberId!"); break;
    }

    return true;
}

bool SvxLineSpacingItem::PutValue( const uno::Any& rVal, sal_uInt8 nMemberId )
{
    sal_Bool bConvert = 0!=(nMemberId&CONVERT_TWIPS);
    nMemberId &= ~CONVERT_TWIPS;

    // fill with current data
    style::LineSpacing aLSp;
    uno::Any aAny;
    sal_Bool bRet = QueryValue( aAny, bConvert ? CONVERT_TWIPS : 0 ) && ( aAny >>= aLSp );

    // get new data
    switch ( nMemberId )
    {
        case 0 :                bRet = (rVal >>= aLSp); break;
        case MID_LINESPACE :    bRet = (rVal >>= aLSp.Mode); break;
        case MID_HEIGHT :       bRet = (rVal >>= aLSp.Height); break;
        default: OSL_FAIL("Wrong MemberId!"); break;
    }

    if( bRet )
    {
        nLineHeight = aLSp.Height;
        switch( aLSp.Mode )
        {
            case style::LineSpacingMode::LEADING:
            {
                eInterLineSpace = SVX_INTER_LINE_SPACE_FIX;
                eLineSpace = SVX_LINE_SPACE_AUTO;
                nInterLineSpace = aLSp.Height;
                if(bConvert)
                    nInterLineSpace = (short)MM100_TO_TWIP(nInterLineSpace);

            }
            break;
            case style::LineSpacingMode::PROP:
            {
                eLineSpace = SVX_LINE_SPACE_AUTO;
                nPropLineSpace = (sal_Int8)std::min(aLSp.Height, (short)0xFF);
                if(100 == aLSp.Height)
                    eInterLineSpace = SVX_INTER_LINE_SPACE_OFF;
                else
                    eInterLineSpace = SVX_INTER_LINE_SPACE_PROP;
            }
            break;
            case style::LineSpacingMode::FIX:
            case style::LineSpacingMode::MINIMUM:
            {
                eInterLineSpace =  SVX_INTER_LINE_SPACE_OFF;
                eLineSpace = aLSp.Mode == style::LineSpacingMode::FIX ? SVX_LINE_SPACE_FIX : SVX_LINE_SPACE_MIN;
                nLineHeight = aLSp.Height;
                if(bConvert)
                    nLineHeight = (sal_uInt16)MM100_TO_TWIP_UNSIGNED(nLineHeight);
            }
            break;
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxLineSpacingItem::Clone( SfxItemPool * ) const
{
    return new SvxLineSpacingItem( *this );
}

//------------------------------------------------------------------------

SfxItemPresentation SvxLineSpacingItem::GetPresentation
(
    SfxItemPresentation /*ePres*/,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
#ifdef DBG_UTIL
    rText.AssignAscii( RTL_CONSTASCII_STRINGPARAM( "SvxLineSpacingItem" ));
#else
    rText.Erase();
#endif
    return SFX_ITEM_PRESENTATION_NONE;
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxLineSpacingItem::Create(SvStream& rStrm, sal_uInt16) const
{
    sal_Int8    nPropSpace;
    short   nInterSpace;
    sal_uInt16  nHeight;
    sal_Int8    nRule, nInterRule;

    rStrm >> nPropSpace
          >> nInterSpace
          >> nHeight
          >> nRule
          >> nInterRule;

    SvxLineSpacingItem* pAttr = new SvxLineSpacingItem( nHeight, Which() );
    pAttr->SetInterLineSpace( nInterSpace );
    pAttr->SetPropLineSpace( nPropSpace );
    pAttr->GetLineSpaceRule() = (SvxLineSpace)nRule;
    pAttr->GetInterLineSpaceRule() = (SvxInterLineSpace)nInterRule;
    return pAttr;
}

// -----------------------------------------------------------------------

SvStream& SvxLineSpacingItem::Store( SvStream& rStrm, sal_uInt16 /*nItemVersion*/ ) const
{
    rStrm << (sal_Int8)  GetPropLineSpace()
          << (short)  GetInterLineSpace()
          << (sal_uInt16) GetLineHeight()
          << (sal_Int8)   GetLineSpaceRule()
          << (sal_Int8)   GetInterLineSpaceRule();
    return rStrm;
}

// -----------------------------------------------------------------------

sal_uInt16 SvxLineSpacingItem::GetValueCount() const
{
    return SVX_LINESPACE_END;   // SVX_LINESPACE_TWO_LINES + 1
}

// -----------------------------------------------------------------------

rtl::OUString SvxLineSpacingItem::GetValueTextByPos( sal_uInt16 nPos ) const
{
    //! load strings from resource
    rtl::OUString aText;
    switch ( nPos )
    {
        case SVX_LINESPACE_USER:
            aText = "User";
            break;
        case SVX_LINESPACE_ONE_LINE:
            aText = "One line";
            break;
        case SVX_LINESPACE_ONE_POINT_FIVE_LINES:
            aText = "1.5 line";
            break;
        case SVX_LINESPACE_TWO_LINES:
            aText = "Two lines";
            break;
    }
    return aText;
}

// -----------------------------------------------------------------------

sal_uInt16 SvxLineSpacingItem::GetEnumValue() const
{
    sal_uInt16 nVal;
    switch ( nPropLineSpace )
    {
        case 100:   nVal = SVX_LINESPACE_ONE_LINE;              break;
        case 150:   nVal = SVX_LINESPACE_ONE_POINT_FIVE_LINES;  break;
        case 200:   nVal = SVX_LINESPACE_TWO_LINES;             break;
        default:    nVal = SVX_LINESPACE_USER;                  break;
    }
    return nVal;
}

// -----------------------------------------------------------------------

void SvxLineSpacingItem::SetEnumValue( sal_uInt16 nVal )
{
    switch ( nVal )
    {
        case SVX_LINESPACE_ONE_LINE:             nPropLineSpace = 100; break;
        case SVX_LINESPACE_ONE_POINT_FIVE_LINES: nPropLineSpace = 150; break;
        case SVX_LINESPACE_TWO_LINES:            nPropLineSpace = 200; break;
    }
}

// class SvxAdjustItem ---------------------------------------------------

SvxAdjustItem::SvxAdjustItem(const SvxAdjust eAdjst, const sal_uInt16 nId )
    : SfxEnumItemInterface( nId ),
    bOneBlock( sal_False ), bLastCenter( sal_False ), bLastBlock( sal_False )
{
    SetAdjust( eAdjst );
}

// -----------------------------------------------------------------------

int SvxAdjustItem::operator==( const SfxPoolItem& rAttr ) const
{
    DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unequal types" );

    return( ( GetAdjust() == ((SvxAdjustItem&)rAttr).GetAdjust() &&
        bOneBlock == ((SvxAdjustItem&)rAttr).bOneBlock &&
        bLastCenter == ((SvxAdjustItem&)rAttr).bLastCenter &&
        bLastBlock == ((SvxAdjustItem&)rAttr).bLastBlock )
        ? 1 : 0 );
}

bool SvxAdjustItem::QueryValue( uno::Any& rVal, sal_uInt8 nMemberId ) const
{
    nMemberId &= ~CONVERT_TWIPS;
    switch( nMemberId )
    {
        case MID_PARA_ADJUST      : rVal <<= (sal_Int16)GetAdjust(); break;
        case MID_LAST_LINE_ADJUST : rVal <<= (sal_Int16)GetLastBlock(); break;
        case MID_EXPAND_SINGLE    :
        {
            sal_Bool bValue = bOneBlock;
            rVal.setValue( &bValue, ::getCppuBooleanType() );
            break;
        }
        default: ;//prevent warning
    }
    return true;
}

bool SvxAdjustItem::PutValue( const uno::Any& rVal, sal_uInt8 nMemberId  )
{
    nMemberId &= ~CONVERT_TWIPS;
    switch( nMemberId )
    {
        case MID_PARA_ADJUST              :
        case MID_LAST_LINE_ADJUST :
        {
            sal_Int32 eVal = - 1;
            try
            {
                eVal = ::comphelper::getEnumAsINT32(rVal);
            }
            catch(...) {}
            if(eVal >= 0 && eVal <= 4)
            {
                if(MID_LAST_LINE_ADJUST == nMemberId &&
                    eVal != SVX_ADJUST_LEFT &&
                    eVal != SVX_ADJUST_BLOCK &&
                    eVal != SVX_ADJUST_CENTER)
                        return sal_False;
                if(eVal < (sal_uInt16)SVX_ADJUST_END)
                    nMemberId == MID_PARA_ADJUST ?
                        SetAdjust((SvxAdjust)eVal) :
                            SetLastBlock((SvxAdjust)eVal);
            }
        }
        break;
        case MID_EXPAND_SINGLE :
            bOneBlock = Any2Bool(rVal);
            break;
    }
    return true;
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxAdjustItem::Clone( SfxItemPool * ) const
{
    return new SvxAdjustItem( *this );
}

//------------------------------------------------------------------------

SfxItemPresentation SvxAdjustItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            return SFX_ITEM_PRESENTATION_NONE;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
            rText = GetValueTextByPos( (sal_uInt16)GetAdjust() );
            return ePres;
        default: ;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

// -----------------------------------------------------------------------

sal_uInt16 SvxAdjustItem::GetValueCount() const
{
    return SVX_ADJUST_END;  // SVX_ADJUST_BLOCKLINE + 1
}

// -----------------------------------------------------------------------

rtl::OUString SvxAdjustItem::GetValueTextByPos( sal_uInt16 nPos ) const
{
    DBG_ASSERT( nPos <= (sal_uInt16)SVX_ADJUST_BLOCKLINE, "enum overflow!" );
    return EE_RESSTR(RID_SVXITEMS_ADJUST_BEGIN + nPos);
}

// -----------------------------------------------------------------------

sal_uInt16 SvxAdjustItem::GetEnumValue() const
{
    return (sal_uInt16)GetAdjust();
}

// -----------------------------------------------------------------------

void SvxAdjustItem::SetEnumValue( sal_uInt16 nVal )
{
    SetAdjust( (const SvxAdjust)nVal );
}

// -----------------------------------------------------------------------

sal_uInt16 SvxAdjustItem::GetVersion( sal_uInt16 nFileVersion ) const
{
    return (nFileVersion == SOFFICE_FILEFORMAT_31)
               ? 0 : ADJUST_LASTBLOCK_VERSION;
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxAdjustItem::Create(SvStream& rStrm, sal_uInt16 nVersion) const
{
    char eAdjustment;
    rStrm >> eAdjustment;
    SvxAdjustItem *pRet = new SvxAdjustItem( (SvxAdjust)eAdjustment, Which() );
    if( nVersion >= ADJUST_LASTBLOCK_VERSION )
    {
        sal_Int8 nFlags;
        rStrm >> nFlags;
        pRet->bOneBlock = 0 != (nFlags & 0x0001);
        pRet->bLastCenter = 0 != (nFlags & 0x0002);
        pRet->bLastBlock = 0 != (nFlags & 0x0004);
    }
    return pRet;
}

// -----------------------------------------------------------------------

SvStream& SvxAdjustItem::Store( SvStream& rStrm, sal_uInt16 nItemVersion ) const
{
    rStrm << (char)GetAdjust();
    if ( nItemVersion >= ADJUST_LASTBLOCK_VERSION )
    {
        sal_Int8 nFlags = 0;
        if ( bOneBlock )
            nFlags |= 0x0001;
        if ( bLastCenter )
            nFlags |= 0x0002;
        if ( bLastBlock )
            nFlags |= 0x0004;
        rStrm << (sal_Int8) nFlags;
    }
    return rStrm;
}

// class SvxWidowsItem ---------------------------------------------------

SvxWidowsItem::SvxWidowsItem(const sal_uInt8 nL, const sal_uInt16 nId ) :
    SfxByteItem( nId, nL )
{
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxWidowsItem::Clone( SfxItemPool * ) const
{
    return new SvxWidowsItem( *this );
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxWidowsItem::Create(SvStream& rStrm, sal_uInt16) const
{
    sal_Int8 nLines;
    rStrm >> nLines;
    return new SvxWidowsItem( nLines, Which() );
}

// -----------------------------------------------------------------------

SvStream& SvxWidowsItem::Store( SvStream& rStrm, sal_uInt16 /*nItemVersion*/ ) const
{
    rStrm << (sal_Int8)GetValue();
    return rStrm;
}

//------------------------------------------------------------------------

SfxItemPresentation SvxWidowsItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
        {
            rText.Erase();
            break;
        }

        case SFX_ITEM_PRESENTATION_NAMELESS:
        {
            rText = EE_RESSTR(RID_SVXITEMS_LINES);
            break;
        }

        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            rText = EE_RESSTR(RID_SVXITEMS_WIDOWS_COMPLETE);
            rText += ' ';
            rText += EE_RESSTR(RID_SVXITEMS_LINES);
        }

        default:
        {
            SAL_WARN( "editeng.items", "SvxWidowsItem::GetPresentation(): unknown SfxItemPresentation" );
        }
    }

    rText.SearchAndReplace( rtl::OUString("%1"), String::CreateFromInt32( GetValue() ) );
    return ePres;
}

// class SvxOrphansItem --------------------------------------------------

SvxOrphansItem::SvxOrphansItem(const sal_uInt8 nL, const sal_uInt16 nId ) :
    SfxByteItem( nId, nL )
{
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxOrphansItem::Clone( SfxItemPool * ) const
{
    return new SvxOrphansItem( *this );
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxOrphansItem::Create(SvStream& rStrm, sal_uInt16) const
{
    sal_Int8 nLines;
    rStrm >> nLines;
    return new SvxOrphansItem( nLines, Which() );
}

// -----------------------------------------------------------------------

SvStream& SvxOrphansItem::Store( SvStream& rStrm, sal_uInt16 /*nItemVersion*/ ) const
{
    rStrm << (sal_Int8) GetValue();
    return rStrm;
}

//------------------------------------------------------------------------

SfxItemPresentation SvxOrphansItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
        {
            rText.Erase();
            break;
        }

        case SFX_ITEM_PRESENTATION_NAMELESS:
        {
            rText = EE_RESSTR(RID_SVXITEMS_LINES);
            break;
        }

        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            rText = EE_RESSTR(RID_SVXITEMS_ORPHANS_COMPLETE);
            rText += ' ';
            rText += EE_RESSTR(RID_SVXITEMS_LINES);
        }

        default:
        {
            SAL_WARN( "editeng.items", "SvxOrphansItem::GetPresentation(): unknown SfxItemPresentation" );
        }
    }

    rText.SearchAndReplace( rtl::OUString("%1"), String::CreateFromInt32( GetValue() ) );
    return ePres;
}

// class SvxHyphenZoneItem -----------------------------------------------

SvxHyphenZoneItem::SvxHyphenZoneItem( const sal_Bool bHyph, const sal_uInt16 nId ) :
    SfxPoolItem( nId )
{
    bHyphen = bHyph;
    bPageEnd = sal_True;
    nMinLead = nMinTrail = 0;
    nMaxHyphens = 255;
}

// -----------------------------------------------------------------------
bool    SvxHyphenZoneItem::QueryValue( uno::Any& rVal, sal_uInt8 nMemberId ) const
{
    nMemberId &= ~CONVERT_TWIPS;
    switch(nMemberId)
    {
        case  MID_IS_HYPHEN:
            rVal = Bool2Any(bHyphen);
        break;
        case MID_HYPHEN_MIN_LEAD:
            rVal <<= (sal_Int16)nMinLead;
        break;
        case MID_HYPHEN_MIN_TRAIL:
            rVal <<= (sal_Int16)nMinTrail;
        break;
        case MID_HYPHEN_MAX_HYPHENS:
            rVal <<= (sal_Int16)nMaxHyphens;
        break;
    }
    return true;
}
// -----------------------------------------------------------------------
bool SvxHyphenZoneItem::PutValue( const uno::Any& rVal, sal_uInt8 nMemberId )
{
    nMemberId &= ~CONVERT_TWIPS;
    sal_Int16 nNewVal = 0;

    if( nMemberId != MID_IS_HYPHEN )
        if(!(rVal >>= nNewVal))
            return false;

    switch(nMemberId)
    {
        case  MID_IS_HYPHEN:
            bHyphen = Any2Bool(rVal);
        break;
        case MID_HYPHEN_MIN_LEAD:
            nMinLead = (sal_uInt8)nNewVal;
        break;
        case MID_HYPHEN_MIN_TRAIL:
            nMinTrail = (sal_uInt8)nNewVal;
        break;
        case MID_HYPHEN_MAX_HYPHENS:
            nMaxHyphens = (sal_uInt8)nNewVal;
        break;
    }
    return true;
}

// -----------------------------------------------------------------------

int SvxHyphenZoneItem::operator==( const SfxPoolItem& rAttr ) const
{
    DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unequal types" );

    return ( (((SvxHyphenZoneItem&)rAttr).bHyphen == bHyphen)
            && (((SvxHyphenZoneItem&)rAttr).bPageEnd == bPageEnd)
            && (((SvxHyphenZoneItem&)rAttr).nMinLead == nMinLead)
            && (((SvxHyphenZoneItem&)rAttr).nMinTrail == nMinTrail)
            && (((SvxHyphenZoneItem&)rAttr).nMaxHyphens == nMaxHyphens) );
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxHyphenZoneItem::Clone( SfxItemPool * ) const
{
    return new SvxHyphenZoneItem( *this );
}

//------------------------------------------------------------------------

SfxItemPresentation SvxHyphenZoneItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            return SFX_ITEM_PRESENTATION_NONE;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        {
            sal_uInt16 nId = RID_SVXITEMS_HYPHEN_FALSE;

            if ( bHyphen )
                nId = RID_SVXITEMS_HYPHEN_TRUE;
            rText = EE_RESSTR(nId);
            rText += cpDelim;
            nId = RID_SVXITEMS_PAGE_END_FALSE;

            if ( bPageEnd )
                nId = RID_SVXITEMS_PAGE_END_TRUE;
            rText += EE_RESSTR(nId);
            rText += cpDelim;
            rText += String::CreateFromInt32( nMinLead );
            rText += cpDelim;
            rText += String::CreateFromInt32( nMinTrail );
            rText += cpDelim;
            rText += String::CreateFromInt32( nMaxHyphens );
            return SFX_ITEM_PRESENTATION_COMPLETE;
        }
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            sal_uInt16 nId = RID_SVXITEMS_HYPHEN_FALSE;

            if ( bHyphen )
                nId = RID_SVXITEMS_HYPHEN_TRUE;
            rText = EE_RESSTR(nId);
            rText += cpDelim;
            nId = RID_SVXITEMS_PAGE_END_FALSE;

            if ( bPageEnd )
                nId = RID_SVXITEMS_PAGE_END_TRUE;
            rText += EE_RESSTR(nId);
            rText += cpDelim;
            rText += String::CreateFromInt32(nMinLead);
            rText += EE_RESSTR(RID_SVXITEMS_HYPHEN_MINLEAD);
            rText += cpDelim;
            rText += String::CreateFromInt32(nMinTrail);
            rText += EE_RESSTR(RID_SVXITEMS_HYPHEN_MINTRAIL);
            rText += cpDelim;
            rText += String::CreateFromInt32(nMaxHyphens);
            rText += EE_RESSTR(RID_SVXITEMS_HYPHEN_MAX);
            return SFX_ITEM_PRESENTATION_COMPLETE;
        }
        default: ;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxHyphenZoneItem::Create(SvStream& rStrm, sal_uInt16) const
{
    sal_Int8 _bHyphen, _bHyphenPageEnd;
    sal_Int8 _nMinLead, _nMinTrail, _nMaxHyphens;
    rStrm >> _bHyphen >> _bHyphenPageEnd >> _nMinLead >> _nMinTrail >> _nMaxHyphens;
    SvxHyphenZoneItem* pAttr = new SvxHyphenZoneItem( sal_False, Which() );
    pAttr->SetHyphen( sal_Bool( _bHyphen != 0 ) );
    pAttr->SetPageEnd( sal_Bool( _bHyphenPageEnd != 0 ) );
    pAttr->GetMinLead() = _nMinLead;
    pAttr->GetMinTrail() = _nMinTrail;
    pAttr->GetMaxHyphens() = _nMaxHyphens;
    return pAttr;
}

// -----------------------------------------------------------------------

SvStream& SvxHyphenZoneItem::Store( SvStream& rStrm, sal_uInt16 /*nItemVersion*/ ) const
{
    rStrm << (sal_Int8) IsHyphen()
          << (sal_Int8) IsPageEnd()
          << (sal_Int8) GetMinLead()
          << (sal_Int8) GetMinTrail()
          << (sal_Int8) GetMaxHyphens();
    return rStrm;
}

// class SvxTabStop ------------------------------------------------------

SvxTabStop::SvxTabStop()
{
    nTabPos = 0;
    eAdjustment = SVX_TAB_ADJUST_LEFT;
    m_cDecimal = cDfltDecimalChar;
    cFill = cDfltFillChar;
}

// -----------------------------------------------------------------------

SvxTabStop::SvxTabStop( const sal_Int32 nPos, const SvxTabAdjust eAdjst,
                        const sal_Unicode cDec, const sal_Unicode cFil )
{
    nTabPos = nPos;
    eAdjustment = eAdjst;
    m_cDecimal = cDec;
    cFill = cFil;
}
// -----------------------------------------------------------------------------
void SvxTabStop::fillDecimal() const
{
    if ( cDfltDecimalChar == m_cDecimal )
        m_cDecimal = SvtSysLocale().GetLocaleData().getNumDecimalSep()[0];
}
// -----------------------------------------------------------------------

XubString SvxTabStop::GetValueString() const
{
    XubString aStr;

    aStr += sal_Unicode( '(' );
    aStr += rtl::OUString::valueOf(static_cast<sal_Int32>(nTabPos));
    aStr += cpDelim;
    aStr += EE_RESSTR(RID_SVXITEMS_TAB_ADJUST_BEGIN + (sal_uInt16)eAdjustment);

    aStr += cpDelim;
    aStr += sal_Unicode('[');
    aStr += EE_RESSTR(RID_SVXITEMS_TAB_DECIMAL_CHAR);
    aStr += GetDecimal();
    aStr += sal_Unicode(']');
    aStr += cpDelim;
    aStr += cpDelim;
    aStr += sal_Unicode('[');
    aStr += EE_RESSTR(RID_SVXITEMS_TAB_FILL_CHAR);
    aStr += cFill;
    aStr += sal_Unicode(']');
    aStr += sal_Unicode(')');

    return aStr;
}

// class SvxTabStopItem --------------------------------------------------

SvxTabStopItem::SvxTabStopItem( sal_uInt16 _nWhich ) :
    SfxPoolItem( _nWhich ),
    maTabStops()
{
    const sal_uInt16 nTabs = SVX_TAB_DEFCOUNT, nDist = SVX_TAB_DEFDIST;
    const SvxTabAdjust eAdjst= SVX_TAB_ADJUST_DEFAULT;

    for (sal_uInt16 i = 0; i < nTabs; ++i)
    {
        SvxTabStop aTab( (i + 1) * nDist, eAdjst );
        maTabStops.insert( aTab );
    }
}

// -----------------------------------------------------------------------

SvxTabStopItem::SvxTabStopItem( const sal_uInt16 nTabs,
                                const sal_uInt16 nDist,
                                const SvxTabAdjust eAdjst,
                                sal_uInt16 _nWhich ) :
    SfxPoolItem( _nWhich ),
    maTabStops()
{
    for ( sal_uInt16 i = 0; i < nTabs; ++i )
    {
        SvxTabStop aTab( (i + 1) * nDist, eAdjst );
        maTabStops.insert( aTab );
    }
}

// -----------------------------------------------------------------------

SvxTabStopItem::SvxTabStopItem( const SvxTabStopItem& rTSI ) :
    SfxPoolItem( rTSI.Which() ),
    maTabStops( rTSI.maTabStops )
{
}

// -----------------------------------------------------------------------

sal_uInt16 SvxTabStopItem::GetPos( const SvxTabStop& rTab ) const
{
    SvxTabStopArr::const_iterator it = maTabStops.find( rTab );
    return it != maTabStops.end() ? it - maTabStops.begin() : SVX_TAB_NOTFOUND;
}

// -----------------------------------------------------------------------

sal_uInt16 SvxTabStopItem::GetPos( const sal_Int32 nPos ) const
{
    SvxTabStopArr::const_iterator it = maTabStops.find( SvxTabStop( nPos ) );
    return it != maTabStops.end() ? it - maTabStops.begin() : SVX_TAB_NOTFOUND;
}

// -----------------------------------------------------------------------

SvxTabStopItem& SvxTabStopItem::operator=( const SvxTabStopItem& rTSI )
{
    maTabStops = rTSI.maTabStops;
    return *this;
}

bool SvxTabStopItem::QueryValue( uno::Any& rVal, sal_uInt8 nMemberId ) const
{
    sal_Bool bConvert = 0!=(nMemberId&CONVERT_TWIPS);
    nMemberId &= ~CONVERT_TWIPS;
    switch ( nMemberId )
    {
        case MID_TABSTOPS:
        {
            sal_uInt16 nCount = Count();
            uno::Sequence< style::TabStop> aSeq(nCount);
            style::TabStop* pArr = aSeq.getArray();
            for(sal_uInt16 i = 0; i < nCount; i++)
            {
                const SvxTabStop& rTab = (*this)[i];
                pArr[i].Position        = bConvert ? TWIP_TO_MM100(rTab.GetTabPos()) : rTab.GetTabPos();
                switch(rTab.GetAdjustment())
                {
                case  SVX_TAB_ADJUST_LEFT   : pArr[i].Alignment = style::TabAlign_LEFT; break;
                case  SVX_TAB_ADJUST_RIGHT  : pArr[i].Alignment = style::TabAlign_RIGHT; break;
                case  SVX_TAB_ADJUST_DECIMAL: pArr[i].Alignment = style::TabAlign_DECIMAL; break;
                case  SVX_TAB_ADJUST_CENTER : pArr[i].Alignment = style::TabAlign_CENTER; break;
                    default: //SVX_TAB_ADJUST_DEFAULT
                        pArr[i].Alignment = style::TabAlign_DEFAULT;

                }
                pArr[i].DecimalChar     = rTab.GetDecimal();
                pArr[i].FillChar        = rTab.GetFill();
            }
            rVal <<= aSeq;
            break;
        }
        case MID_STD_TAB:
        {
            const SvxTabStop &rTab = maTabStops.front();
            rVal <<= (static_cast<sal_Int32>(bConvert ? TWIP_TO_MM100(rTab.GetTabPos()) : rTab.GetTabPos()));
            break;
        }
    }
    return sal_True;
}

bool SvxTabStopItem::PutValue( const uno::Any& rVal, sal_uInt8 nMemberId )
{
    sal_Bool bConvert = 0!=(nMemberId&CONVERT_TWIPS);
    nMemberId &= ~CONVERT_TWIPS;
    switch ( nMemberId )
    {
        case MID_TABSTOPS:
        {
            uno::Sequence< style::TabStop> aSeq;
            if(!(rVal >>= aSeq))
            {
                uno::Sequence < uno::Sequence < uno::Any >  > aAnySeq;
                if (!(rVal >>= aAnySeq))
                    return sal_False;
                sal_Int32 nLength = aAnySeq.getLength();
                aSeq.realloc( nLength );
                for ( sal_Int32 n=0; n<nLength; n++ )
                {
                    uno::Sequence < uno::Any >& rAnySeq = aAnySeq[n];
                    if ( rAnySeq.getLength() == 4 )
                    {
                        if (!(rAnySeq[0] >>= aSeq[n].Position)) return sal_False;
                        if (!(rAnySeq[1] >>= aSeq[n].Alignment))
                        {
                            sal_Int32 nVal = 0;
                            if (rAnySeq[1] >>= nVal)
                                aSeq[n].Alignment = (com::sun::star::style::TabAlign) nVal;
                            else
                                return sal_False;
                        }
                        if (!(rAnySeq[2] >>= aSeq[n].DecimalChar))
                        {
                            ::rtl::OUString aVal;
                            if ( (rAnySeq[2] >>= aVal) && aVal.getLength() == 1 )
                                aSeq[n].DecimalChar = aVal.toChar();
                            else
                                return sal_False;
                        }
                        if (!(rAnySeq[3] >>= aSeq[n].FillChar))
                        {
                            ::rtl::OUString aVal;
                            if ( (rAnySeq[3] >>= aVal) && aVal.getLength() == 1 )
                                aSeq[n].FillChar = aVal.toChar();
                            else
                                return sal_False;
                        }
                    }
                    else
                        return sal_False;
                }
            }

            maTabStops.clear();
            const style::TabStop* pArr = aSeq.getConstArray();
            const sal_uInt16 nCount = (sal_uInt16)aSeq.getLength();
            for(sal_uInt16 i = 0; i < nCount ; i++)
            {
                SvxTabAdjust eAdjust = SVX_TAB_ADJUST_DEFAULT;
                switch(pArr[i].Alignment)
                {
                case style::TabAlign_LEFT   : eAdjust = SVX_TAB_ADJUST_LEFT; break;
                case style::TabAlign_CENTER : eAdjust = SVX_TAB_ADJUST_CENTER; break;
                case style::TabAlign_RIGHT  : eAdjust = SVX_TAB_ADJUST_RIGHT; break;
                case style::TabAlign_DECIMAL: eAdjust = SVX_TAB_ADJUST_DECIMAL; break;
                default: ;//prevent warning
                }
                sal_Unicode cFill = pArr[i].FillChar;
                sal_Unicode cDecimal = pArr[i].DecimalChar;
                SvxTabStop aTab( bConvert ? MM100_TO_TWIP(pArr[i].Position) : pArr[i].Position,
                                    eAdjust,
                                    cDecimal,
                                    cFill );
                Insert(aTab);
            }
            break;
        }
        case MID_STD_TAB:
        {
            sal_Int32 nNewPos = 0;
            if (!(rVal >>= nNewPos) )
                return sal_False;
            if (bConvert)
                nNewPos = MM100_TO_TWIP ( nNewPos );
            if (nNewPos <= 0)
                return sal_False;
            const SvxTabStop& rTab = maTabStops.front();
            SvxTabStop aNewTab ( nNewPos, rTab.GetAdjustment(), rTab.GetDecimal(), rTab.GetFill() );
            Remove( 0 );
            Insert( aNewTab );
            break;
        }
    }
    return sal_True;
}
// -----------------------------------------------------------------------

int SvxTabStopItem::operator==( const SfxPoolItem& rAttr ) const
{
    DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unequal types" );

    const SvxTabStopItem& rTSI = (SvxTabStopItem&)rAttr;

    if ( Count() != rTSI.Count() )
        return 0;

    for ( sal_uInt16 i = 0; i < Count(); ++i )
        if( !(*this)[i].IsEqual( rTSI[i] ) )
            return 0;
    return 1;
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxTabStopItem::Clone( SfxItemPool * ) const
{
    return new SvxTabStopItem( *this );
}

//------------------------------------------------------------------------

SfxItemPresentation SvxTabStopItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          eCoreUnit,
    SfxMapUnit          ePresUnit,
    XubString&          rText, const IntlWrapper *pIntl
)   const
{
    rText.Erase();

    if ( ePres > SFX_ITEM_PRESENTATION_NONE )
    {
        sal_Bool bComma = sal_False;

        for ( sal_uInt16 i = 0; i < Count(); ++i )
        {
            if ( SVX_TAB_ADJUST_DEFAULT != ((*this)[i]).GetAdjustment() )
            {
                if ( bComma )
                    rText += sal_Unicode(',');
                rText += GetMetricText(
                    ((*this)[i]).GetTabPos(), eCoreUnit, ePresUnit, pIntl );
                if ( SFX_ITEM_PRESENTATION_COMPLETE == ePres )
                    rText += EE_RESSTR(GetMetricId(ePresUnit));
                bComma = sal_True;
            }
        }
    }
    return ePres;
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxTabStopItem::Create( SvStream& rStrm, sal_uInt16 ) const
{
    sal_Int8 nTabs;
    rStrm >> nTabs;
    SvxTabStopItem* pAttr =
        new SvxTabStopItem( 0, 0, SVX_TAB_ADJUST_DEFAULT, Which() );

    for ( sal_Int8 i = 0; i < nTabs; i++ )
    {
        sal_Int32 nPos(0);
        sal_Int8 eAdjust;
        unsigned char cDecimal, cFill;
        rStrm >> nPos >> eAdjust >> cDecimal >> cFill;
        if( !i || SVX_TAB_ADJUST_DEFAULT != eAdjust )
            pAttr->Insert( SvxTabStop
                ( nPos, (SvxTabAdjust)eAdjust, sal_Unicode(cDecimal), sal_Unicode(cFill) ) );
    }
    return pAttr;
}

// -----------------------------------------------------------------------

SvStream& SvxTabStopItem::Store( SvStream& rStrm, sal_uInt16 /*nItemVersion*/ ) const
{
    // Default-Tabs are only expanded for the default Attribute. For complete
    // backward compabillity (<=304) all tabs have to be expanded, this makes
    // the files grow large in size. All only SWG!

    const SfxItemPool *pPool = SfxItemPool::GetStoringPool();
    const bool bStoreDefTabs = pPool
        && pPool->GetName().equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("SWG"))
        && ::IsDefaultItem( this );

    const short nTabs = Count();
    sal_uInt16  nCount = 0, nDefDist = 0;
    sal_Int32 nNew = 0;

    if( bStoreDefTabs )
    {
        const SvxTabStopItem& rDefTab = (const SvxTabStopItem &)
            pPool->GetDefaultItem( pPool->GetWhich( SID_ATTR_TABSTOP, sal_False ) );
        nDefDist = sal_uInt16( rDefTab.maTabStops.front().GetTabPos() );
        const sal_Int32 nPos = nTabs > 0 ? (*this)[nTabs-1].GetTabPos() : 0;
        nCount  = (sal_uInt16)(nPos / nDefDist);
        nNew    = (nCount + 1) * nDefDist;

        if( nNew <= nPos + 50 )
            nNew += nDefDist;

        sal_Int32 lA3Width = SvxPaperInfo::GetPaperSize(PAPER_A3).Width();
        nCount = (sal_uInt16)(nNew < lA3Width ? ( lA3Width - nNew ) / nDefDist + 1 : 0);
    }

    rStrm << (sal_Int8) ( nTabs + nCount );
    for ( short i = 0; i < nTabs; i++ )
    {
        const SvxTabStop& rTab = (*this)[ i ];
        rStrm << rTab.GetTabPos()
              << (sal_Int8) rTab.GetAdjustment()
              << (unsigned char) rTab.GetDecimal()
              << (unsigned char) rTab.GetFill();
    }

    if ( bStoreDefTabs )
        for( ; nCount; --nCount )
        {
            SvxTabStop aSwTabStop(nNew, SVX_TAB_ADJUST_DEFAULT);
            rStrm << aSwTabStop.GetTabPos()
                  << (sal_Int8) aSwTabStop.GetAdjustment()
                  << (unsigned char) aSwTabStop.GetDecimal()
                  << (unsigned char) aSwTabStop.GetFill();
            nNew += nDefDist;
        }

    return rStrm;
}

// -----------------------------------------------------------------------
bool SvxTabStopItem::Insert( const SvxTabStop& rTab )
{
    sal_uInt16 nTabPos = GetPos(rTab);
    if(SVX_TAB_NOTFOUND != nTabPos )
        Remove(nTabPos);
    return maTabStops.insert( rTab ).second;
}
// -----------------------------------------------------------------------
void SvxTabStopItem::Insert( const SvxTabStopItem* pTabs, sal_uInt16 nStart,
                            sal_uInt16 nEnd )
{
    for( sal_uInt16 i = nStart; i < nEnd && i < pTabs->Count(); i++ )
    {
        const SvxTabStop& rTab = (*pTabs)[i];
        sal_uInt16 nTabPos = GetPos(rTab);
        if(SVX_TAB_NOTFOUND != nTabPos)
            Remove(nTabPos);
    }
    for( sal_uInt16 i = nStart; i < nEnd && i < pTabs->Count(); i++ )
    {
        maTabStops.insert( (*pTabs)[i] );
    }
}



// class SvxFmtSplitItem -------------------------------------------------
SvxFmtSplitItem::~SvxFmtSplitItem()
{
}
// -----------------------------------------------------------------------
SfxPoolItem* SvxFmtSplitItem::Clone( SfxItemPool * ) const
{
    return new SvxFmtSplitItem( *this );
}

// -----------------------------------------------------------------------

SvStream& SvxFmtSplitItem::Store( SvStream& rStrm, sal_uInt16 /*nItemVersion*/ ) const
{
    rStrm << (sal_Int8)GetValue();
    return rStrm;
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxFmtSplitItem::Create( SvStream& rStrm, sal_uInt16 ) const
{
    sal_Int8 bIsSplit;
    rStrm >> bIsSplit;
    return new SvxFmtSplitItem( sal_Bool( bIsSplit != 0 ), Which() );
}

//------------------------------------------------------------------------

SfxItemPresentation SvxFmtSplitItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            return SFX_ITEM_PRESENTATION_NONE;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            sal_uInt16 nId = RID_SVXITEMS_FMTSPLIT_FALSE;

            if ( GetValue() )
                nId = RID_SVXITEMS_FMTSPLIT_TRUE;
            rText = EE_RESSTR(nId);
            return ePres;
        }
        default: ;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

// --------------------------------------------------------------------

SfxPoolItem* SvxPageModelItem::Clone( SfxItemPool* ) const
{
    return new SvxPageModelItem( *this );
}

//------------------------------------------------------------------------

bool SvxPageModelItem::QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId ) const
{
    nMemberId &= ~CONVERT_TWIPS;

    switch ( nMemberId )
    {
        case MID_AUTO: rVal <<= (sal_Bool) bAuto; break;
        case MID_NAME: rVal <<= ::rtl::OUString( GetValue() ); break;
        default: OSL_FAIL("Wrong MemberId!"); return sal_False;
    }

    return sal_True;
}

bool SvxPageModelItem::PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId )
{
    nMemberId &= ~CONVERT_TWIPS;
    sal_Bool bRet;
    ::rtl::OUString aStr;
    switch ( nMemberId )
    {
        case MID_AUTO: bRet = ( rVal >>= bAuto ); break;
        case MID_NAME: bRet = ( rVal >>= aStr ); if ( bRet ) SetValue(aStr); break;
        default: OSL_FAIL("Wrong MemberId!"); return sal_False;
    }

    return bRet;
}

SfxItemPresentation SvxPageModelItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();
    bool bSet = ( GetValue().Len() > 0 );

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return SFX_ITEM_PRESENTATION_NONE;

        case SFX_ITEM_PRESENTATION_NAMELESS:
            if ( bSet )
                rText = GetValue();
            return SFX_ITEM_PRESENTATION_NAMELESS;

        case SFX_ITEM_PRESENTATION_COMPLETE:
            if ( bSet )
            {
                rText = EE_RESSTR(RID_SVXITEMS_PAGEMODEL_COMPLETE);
                rText += GetValue();
            }
            return SFX_ITEM_PRESENTATION_COMPLETE;
        default: ;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

//------------------------------------------------------------------------

SvxScriptSpaceItem::SvxScriptSpaceItem( sal_Bool bOn, const sal_uInt16 nId )
    : SfxBoolItem( nId, bOn )
{
}

SfxPoolItem* SvxScriptSpaceItem::Clone( SfxItemPool * ) const
{
    return new SvxScriptSpaceItem( GetValue(), Which() );
}

SfxPoolItem* SvxScriptSpaceItem::Create(SvStream & rStrm, sal_uInt16) const
{
    sal_Bool bFlag;
    rStrm >> bFlag;
    return new SvxScriptSpaceItem( bFlag, Which() );
}

sal_uInt16  SvxScriptSpaceItem::GetVersion( sal_uInt16 nFFVer ) const
{
    DBG_ASSERT( SOFFICE_FILEFORMAT_31==nFFVer ||
            SOFFICE_FILEFORMAT_40==nFFVer ||
            SOFFICE_FILEFORMAT_50==nFFVer,
            "SvxTwoLinesItem: Is there a new file format? ");

    return SOFFICE_FILEFORMAT_50 > nFFVer ? USHRT_MAX : 0;
}

SfxItemPresentation SvxScriptSpaceItem::GetPresentation(
        SfxItemPresentation ePres,
        SfxMapUnit /*eCoreMetric*/, SfxMapUnit /*ePresMetric*/,
        String &rText, const IntlWrapper* /*pIntl*/ ) const
{
    switch( ePres )
    {
    case SFX_ITEM_PRESENTATION_NONE:
        rText.Erase();
        break;
    case SFX_ITEM_PRESENTATION_NAMELESS:
    case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            rText = EE_RESSTR( !GetValue()
                                    ? RID_SVXITEMS_SCRPTSPC_OFF
                                    : RID_SVXITEMS_SCRPTSPC_ON );
            return ePres;
        }
    default: ;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

//------------------------------------------------------------------------

SvxHangingPunctuationItem::SvxHangingPunctuationItem(
                                    sal_Bool bOn, const sal_uInt16 nId )
    : SfxBoolItem( nId, bOn )
{
}

SfxPoolItem* SvxHangingPunctuationItem::Clone( SfxItemPool * ) const
{
    return new SvxHangingPunctuationItem( GetValue(), Which() );
}

SfxPoolItem* SvxHangingPunctuationItem::Create(SvStream & rStrm, sal_uInt16) const
{
    sal_Bool nValue;
    rStrm >> nValue;
    return new SvxHangingPunctuationItem( nValue, Which() );
}

sal_uInt16 SvxHangingPunctuationItem::GetVersion( sal_uInt16 nFFVer ) const
{
    DBG_ASSERT( SOFFICE_FILEFORMAT_31==nFFVer ||
            SOFFICE_FILEFORMAT_40==nFFVer ||
            SOFFICE_FILEFORMAT_50==nFFVer,
            "SvxHangingPunctuationItem: Is there a new file format? ");

    return SOFFICE_FILEFORMAT_50 > nFFVer ? USHRT_MAX : 0;
}

SfxItemPresentation SvxHangingPunctuationItem::GetPresentation(
        SfxItemPresentation ePres,
        SfxMapUnit /*eCoreMetric*/, SfxMapUnit /*ePresMetric*/,
        String &rText, const IntlWrapper* /*pIntl*/ ) const
{
    switch( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
            {
                rText = EE_RESSTR( !GetValue()
                                        ? RID_SVXITEMS_HNGPNCT_OFF
                                        : RID_SVXITEMS_HNGPNCT_ON );
                return ePres;
            }
        default: ;//prevent warning
            break;
    }
    return SFX_ITEM_PRESENTATION_NONE;
}
//------------------------------------------------------------------------

SvxForbiddenRuleItem::SvxForbiddenRuleItem(
                                    sal_Bool bOn, const sal_uInt16 nId )
    : SfxBoolItem( nId, bOn )
{
}

SfxPoolItem* SvxForbiddenRuleItem::Clone( SfxItemPool * ) const
{
    return new SvxForbiddenRuleItem( GetValue(), Which() );
}

SfxPoolItem* SvxForbiddenRuleItem::Create(SvStream & rStrm, sal_uInt16) const
{
    sal_Bool nValue;
    rStrm >> nValue;
    return new SvxForbiddenRuleItem( nValue, Which() );
}

sal_uInt16 SvxForbiddenRuleItem::GetVersion( sal_uInt16 nFFVer ) const
{
    DBG_ASSERT( SOFFICE_FILEFORMAT_31==nFFVer ||
            SOFFICE_FILEFORMAT_40==nFFVer ||
            SOFFICE_FILEFORMAT_50==nFFVer,
            "SvxForbiddenRuleItem: Is there a new file format? ");

    return SOFFICE_FILEFORMAT_50 > nFFVer ? USHRT_MAX : 0;
}

SfxItemPresentation SvxForbiddenRuleItem::GetPresentation(
        SfxItemPresentation ePres,
        SfxMapUnit /*eCoreMetric*/, SfxMapUnit /*ePresMetric*/,
        String &rText, const IntlWrapper* /*pIntl*/ ) const
{
    switch( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
            {
                rText = EE_RESSTR( !GetValue()
                                        ? RID_SVXITEMS_FORBIDDEN_RULE_OFF
                                        : RID_SVXITEMS_FORBIDDEN_RULE_ON );
                return ePres;
            }
        default: ;//prevent warning
            break;
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

/*************************************************************************
|*    class SvxParaVertAlignItem
*************************************************************************/

SvxParaVertAlignItem::SvxParaVertAlignItem( sal_uInt16 nValue,
    const sal_uInt16 nW )
    : SfxUInt16Item( nW, nValue )
{
}

SfxPoolItem* SvxParaVertAlignItem::Clone( SfxItemPool* ) const
{
    return new SvxParaVertAlignItem( GetValue(), Which() );
}

SfxPoolItem* SvxParaVertAlignItem::Create( SvStream& rStrm, sal_uInt16 ) const
{
    sal_uInt16 nVal;
    rStrm >> nVal;
    return new SvxParaVertAlignItem( nVal, Which() );
}

SvStream& SvxParaVertAlignItem::Store( SvStream & rStrm, sal_uInt16 ) const
{
    rStrm << GetValue();
    return rStrm;
}

sal_uInt16 SvxParaVertAlignItem::GetVersion( sal_uInt16 nFFVer ) const
{
    return SOFFICE_FILEFORMAT_50 > nFFVer ? USHRT_MAX : 0;
}

SfxItemPresentation SvxParaVertAlignItem::GetPresentation(
        SfxItemPresentation ePres,
        SfxMapUnit /*eCoreMetric*/, SfxMapUnit /*ePresMetric*/,
        String &rText, const IntlWrapper*  ) const
{
    switch( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
            {
                sal_uInt16 nTmp;
                switch( GetValue() )
                {
                    case AUTOMATIC: nTmp = RID_SVXITEMS_PARAVERTALIGN_AUTO; break;
                    case TOP:       nTmp = RID_SVXITEMS_PARAVERTALIGN_TOP; break;
                    case CENTER:    nTmp = RID_SVXITEMS_PARAVERTALIGN_CENTER; break;
                    case BOTTOM:    nTmp = RID_SVXITEMS_PARAVERTALIGN_BOTTOM; break;
                    default:    nTmp = RID_SVXITEMS_PARAVERTALIGN_BASELINE; break;
                }
                rText = EE_RESSTR( nTmp );
                return ePres;
            }
        default: ;//prevent warning
            break;
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

bool SvxParaVertAlignItem::QueryValue( com::sun::star::uno::Any& rVal,
                                           sal_uInt8 /*nMemberId*/ ) const
{
    rVal <<= (sal_Int16)GetValue();
    return sal_True;
}

bool SvxParaVertAlignItem::PutValue( const com::sun::star::uno::Any& rVal,
                                         sal_uInt8 /*nMemberId*/ )
{
    sal_Int16 nVal = sal_Int16();
    if((rVal >>= nVal) && nVal >=0 && nVal <= BOTTOM )
    {
        SetValue( (sal_uInt16)nVal );
        return sal_True;
    }
    else
        return sal_False;
}

int SvxParaVertAlignItem::operator==( const SfxPoolItem& rItem ) const
{
    DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );
    return SfxUInt16Item::operator==( rItem );
}


SvxParaGridItem::SvxParaGridItem( sal_Bool bOn, const sal_uInt16 nId )
    : SfxBoolItem( nId, bOn )
{
}

SfxPoolItem* SvxParaGridItem::Clone( SfxItemPool * ) const
{
    return new SvxParaGridItem( GetValue(), Which() );
}

SfxPoolItem* SvxParaGridItem::Create(SvStream & rStrm, sal_uInt16) const
{
    sal_Bool bFlag;
    rStrm >> bFlag;
    return new SvxParaGridItem( bFlag, Which() );
}

sal_uInt16  SvxParaGridItem::GetVersion( sal_uInt16 nFFVer ) const
{
    DBG_ASSERT( SOFFICE_FILEFORMAT_31==nFFVer ||
            SOFFICE_FILEFORMAT_40==nFFVer ||
            SOFFICE_FILEFORMAT_50==nFFVer,
            "SvxParaGridItem: Is there a new file format? ");

    return SOFFICE_FILEFORMAT_50 > nFFVer ? USHRT_MAX : 0;
}

SfxItemPresentation SvxParaGridItem::GetPresentation(
        SfxItemPresentation ePres,
        SfxMapUnit /*eCoreMetric*/, SfxMapUnit /*ePresMetric*/,
        String &rText, const IntlWrapper* /*pIntl*/ ) const
{
    switch( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
            {
                rText = GetValue() ?
                        EE_RESSTR( RID_SVXITEMS_PARASNAPTOGRID_ON ) :
                        EE_RESSTR( RID_SVXITEMS_PARASNAPTOGRID_OFF );

                return ePres;
            }
        default: ;//prevent warning
            break;
    }
    return SFX_ITEM_PRESENTATION_NONE;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
