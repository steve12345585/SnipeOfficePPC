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

#include <svl/itemiter.hxx>
#include <vcl/svapp.hxx>
#include <editeng/itemtype.hxx>
#include <svtools/grfmgr.hxx>
#include <unotools/intlwrapper.hxx>
#include <comphelper/processfactory.hxx>
#include <rtl/ustrbuf.hxx>
#include <fmtanchr.hxx>
#include <fmtfsize.hxx>
#include <fmtinfmt.hxx>
#include <fchrfmt.hxx>
#include <fmtautofmt.hxx>
#include <fmtsrnd.hxx>
#include <fmtornt.hxx>
#include <fmtlsplt.hxx>
#include <fmtrowsplt.hxx>
#include <fmtpdsc.hxx>
#include <fmtclds.hxx>
#include <fmteiro.hxx>
#include <fmturl.hxx>
#include <fmthdft.hxx>
#include <fmtcnct.hxx>
#include <fmtline.hxx>
#include <tgrditem.hxx>
#include <hfspacingitem.hxx>
#include <fmtruby.hxx>
#include <paratr.hxx>
#include <grfatr.hxx>
#include <pagedesc.hxx>
#include <charfmt.hxx>
#include <fmtcol.hxx>
#include <tox.hxx>
#include <attrdesc.hrc>
#include <fmtftntx.hxx>
#include <fmtfollowtextflow.hxx>


using namespace com::sun::star;


TYPEINIT2(SwFmtCharFmt,SfxPoolItem,SwClient);

// query the attribute descriptions
void SwAttrSet::GetPresentation(
        SfxItemPresentation ePres,
        SfxMapUnit eCoreMetric,
        SfxMapUnit ePresMetric,
        String &rText ) const
{
    static sal_Char const sKomma[] = ", ";

    rText.Erase();
    String aStr;
    if( Count() )
    {
        SfxItemIter aIter( *this );
        const IntlWrapper rInt( ::comphelper::getProcessComponentContext(),
                                    GetAppLanguageTag() );
        while( sal_True )
        {
            aIter.GetCurItem()->GetPresentation( ePres, eCoreMetric,
                                                 ePresMetric, aStr,
                                                 &rInt );
            if( rText.Len() && aStr.Len() )
                rText += rtl::OUString(sKomma);
            rText += aStr;
            if( aIter.IsAtEnd() )
                break;
            aIter.NextItem();
        }
    }
}

// ATT_CHARFMT *********************************************


SfxItemPresentation SwFmtCharFmt::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          eCoreUnit,
    SfxMapUnit          ePresUnit,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            const SwCharFmt *pCharFmt = GetCharFmt();
            if ( pCharFmt )
            {
                String aStr;
                rText = SW_RESSTR(STR_CHARFMT );
                pCharFmt->GetPresentation( ePres, eCoreUnit, ePresUnit, aStr );
                rText += '(';
                rText += aStr;
                rText += ')';
            }
            else
                rText = SW_RESSTR( STR_NO_CHARFMT );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

// ATT_AUTOFMT *********************************************


SfxItemPresentation SwFmtAutoFmt::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            rText.Erase(); //TODO
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

// ATT_INETFMT *********************************************


SfxItemPresentation SwFmtINetFmt::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            rText = GetValue();
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

SfxItemPresentation SwFmtRuby::GetPresentation( SfxItemPresentation ePres,
                            SfxMapUnit /*eCoreMetric*/, SfxMapUnit /*ePresMetric*/,
                            String &rText, const IntlWrapper* /*pIntl*/ ) const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
            {
                rText = aEmptyStr;
                return ePres;
            }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

/*************************************************************************
|*    class     SwFmtDrop
*************************************************************************/


SfxItemPresentation SwFmtDrop::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    rText.Erase();
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            if ( GetLines() > 1 )
            {
                if ( GetChars() > 1 )
                {
                    rText = String::CreateFromInt32( GetChars() );
                    rText += ' ';
                }
                rText += SW_RESSTR( STR_DROP_OVER );
                rText += ' ';
                rText += String::CreateFromInt32( GetLines() );
                rText += ' ';
                rText += SW_RESSTR( STR_DROP_LINES );
            }
            else
                rText = SW_RESSTR( STR_NO_DROP_LINES );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

/*************************************************************************
|*    class     SwRegisterItem
*************************************************************************/


SfxItemPresentation SwRegisterItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText,
    const IntlWrapper*        /*pIntl*/
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
            sal_uInt16 nId = GetValue() ? STR_REGISTER_ON : STR_REGISTER_OFF;
            rText = SW_RESSTR( nId );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

/*************************************************************************
|*    class     SwNumRuleItem
*************************************************************************/


SfxItemPresentation SwNumRuleItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText,
    const IntlWrapper*        /*pIntl*/
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
            if( GetValue().Len() )
                (((rText = SW_RESSTR( STR_NUMRULE_ON )) +=
                    '(' ) += GetValue() ) += ')';
            else
                rText = SW_RESSTR( STR_NUMRULE_OFF );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}
/*************************************************************************
|*    class     SwParaConnectBorderItem
*************************************************************************/

SfxItemPresentation SwParaConnectBorderItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText,
    const IntlWrapper*        /*pIntl*/
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
            sal_uInt16 nId = GetValue() ? STR_CONNECT_BORDER_ON : STR_CONNECT_BORDER_OFF;
            rText = SW_RESSTR( nId );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}



/******************************************************************************
 *  Frame-Attribute:
 ******************************************************************************/


SfxItemPresentation SwFmtFrmSize::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          eCoreUnit,
    SfxMapUnit          ePresUnit,
    String&             rText,
    const IntlWrapper*        pIntl
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
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            rText = SW_RESSTR( STR_FRM_WIDTH );
            rText += ' ';
            if ( GetWidthPercent() )
            {
                rText += String::CreateFromInt32(GetWidthPercent());
                rText += '%';
            }
            else
            {
                rText += ::GetMetricText( GetWidth(), eCoreUnit, ePresUnit, pIntl );
                rText += ::GetSvxString( ::GetMetricId( ePresUnit ) );
            }
            if ( ATT_VAR_SIZE != GetHeightSizeType() )
            {
                rText += ',';
                rText += ' ';
                const sal_uInt16 nId = ATT_FIX_SIZE == eFrmHeightType ?
                                        STR_FRM_FIXEDHEIGHT : STR_FRM_MINHEIGHT;
                rText += SW_RESSTR( nId );
                rText += ' ';
                if ( GetHeightPercent() )
                {
                    rText += String::CreateFromInt32(GetHeightPercent());
                    rText += '%';
                }
                else
                {
                    rText += ::GetMetricText( GetHeight(), eCoreUnit, ePresUnit, pIntl );
                    rText += ::GetSvxString( ::GetMetricId( ePresUnit ) );
                }
            }
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

//Kopfzeile, fuer Seitenformate
//Client von FrmFmt das den Header beschreibt.


SfxItemPresentation SwFmtHeader::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            const sal_uInt16 nId = GetHeaderFmt() ? STR_HEADER : STR_NO_HEADER;
            rText = SW_RESSTR( nId );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

//Fusszeile, fuer Seitenformate
//Client von FrmFmt das den Footer beschreibt.


SfxItemPresentation SwFmtFooter::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            const sal_uInt16 nId = GetFooterFmt() ? STR_FOOTER : STR_NO_FOOTER;
            rText = SW_RESSTR( nId );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}


SfxItemPresentation SwFmtSurround::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            sal_uInt16 nId = 0;
            switch ( (SwSurround)GetValue() )
            {
                case SURROUND_NONE:
                    nId = STR_SURROUND_NONE;
                break;
                case SURROUND_THROUGHT:
                    nId = STR_SURROUND_THROUGHT;
                break;
                case SURROUND_PARALLEL:
                    nId = STR_SURROUND_PARALLEL;
                break;
                case SURROUND_IDEAL:
                    nId = STR_SURROUND_IDEAL;
                break;
                case SURROUND_LEFT:
                    nId = STR_SURROUND_LEFT;
                break;
                case SURROUND_RIGHT:
                    nId = STR_SURROUND_RIGHT;
                break;
                default:;//prevent warning
            }
            if ( nId )
                rText = SW_RESSTR( nId );

            if ( IsAnchorOnly() )
            {
                rText += ' ';
                rText += SW_RESSTR( STR_SURROUND_ANCHORONLY );
            }
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}


//VertOrientation, wie und woran orientiert --
//  sich der FlyFrm in der Vertikalen -----------


SfxItemPresentation SwFmtVertOrient::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          eCoreUnit,
    SfxMapUnit          ePresUnit,
    String&             rText,
    const IntlWrapper*        pIntl
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            sal_uInt16 nId = 0;
            switch ( GetVertOrient() )
            {
                case text::VertOrientation::NONE:
                {
                    rText += SW_RESSTR( STR_POS_Y );
                    rText += ' ';
                    rText += ::GetMetricText( GetPos(), eCoreUnit, ePresUnit, pIntl );
                    rText += ::GetSvxString( ::GetMetricId( ePresUnit ) );
                }
                break;
                case text::VertOrientation::TOP:
                    nId = STR_VERT_TOP;
                    break;
                case text::VertOrientation::CENTER:
                    nId = STR_VERT_CENTER;
                    break;
                case text::VertOrientation::BOTTOM:
                    nId = STR_VERT_BOTTOM;
                    break;
                case text::VertOrientation::LINE_TOP:
                    nId = STR_LINE_TOP;
                    break;
                case text::VertOrientation::LINE_CENTER:
                    nId = STR_LINE_CENTER;
                    break;
                case text::VertOrientation::LINE_BOTTOM:
                    nId = STR_LINE_BOTTOM;
                    break;
                default:;//prevent warning
            }
            if ( nId )
                rText += SW_RESSTR( nId );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

//HoriOrientation, wie und woran orientiert --
//  sich der FlyFrm in der Hoizontalen ----------


SfxItemPresentation SwFmtHoriOrient::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          eCoreUnit,
    SfxMapUnit          ePresUnit,
    String&             rText,
    const IntlWrapper*        pIntl
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            sal_uInt16 nId = 0;
            switch ( GetHoriOrient() )
            {
                case text::HoriOrientation::NONE:
                {
                    rText += SW_RESSTR( STR_POS_X );
                    rText += ' ';
                    rText += ::GetMetricText( GetPos(), eCoreUnit, ePresUnit, pIntl );
                    rText += ::GetSvxString( ::GetMetricId( ePresUnit ) );
                }
                break;
                case text::HoriOrientation::RIGHT:
                    nId = STR_HORI_RIGHT;
                break;
                case text::HoriOrientation::CENTER:
                    nId = STR_HORI_CENTER;
                break;
                case text::HoriOrientation::LEFT:
                    nId = STR_HORI_LEFT;
                break;
                case text::HoriOrientation::INSIDE:
                    nId = STR_HORI_INSIDE;
                break;
                case text::HoriOrientation::OUTSIDE:
                    nId = STR_HORI_OUTSIDE;
                break;
                case text::HoriOrientation::FULL:
                    nId = STR_HORI_FULL;
                break;
                default:;//prevent warning
            }
            if ( nId )
                rText += SW_RESSTR( nId );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

//FlyAnchor, Anker des Freifliegenden Rahmen ----


SfxItemPresentation SwFmtAnchor::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            sal_uInt16 nId = 0;
            switch ( GetAnchorId() )
            {
                case FLY_AT_PARA:
                    nId = STR_FLY_AT_PARA;
                    break;
                case FLY_AS_CHAR:
                    nId = STR_FLY_AS_CHAR;
                    break;
                case FLY_AT_PAGE:
                    nId = STR_FLY_AT_PAGE;
                    break;
                default:;//prevent warning
            }
            if ( nId )
                rText += SW_RESSTR( nId );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}


SfxItemPresentation SwFmtPageDesc::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            const SwPageDesc *pPageDesc = GetPageDesc();
            if ( pPageDesc )
                rText = pPageDesc->GetName();
            else
                rText = SW_RESSTR( STR_NO_PAGEDESC );
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

//Der ColumnDescriptor --------------------------


SfxItemPresentation SwFmtCol::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          eCoreUnit,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        pIntl
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            sal_uInt16 nCnt = GetNumCols();
            if ( nCnt > 1 )
            {
                rText = String::CreateFromInt32(nCnt);
                rText += ' ';
                rText += SW_RESSTR( STR_COLUMNS );
                if ( COLADJ_NONE != GetLineAdj() )
                {
                    sal_uInt16 nWdth = sal_uInt16(GetLineWidth());
                    rText += ' ';
                    rText += SW_RESSTR( STR_LINE_WIDTH );
                    rText += ' ';
                    rText += ::GetMetricText( nWdth, eCoreUnit,
                                              SFX_MAPUNIT_POINT, pIntl );
                }
            }
            else
                rText.Erase();
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

//URL's und Maps


SfxItemPresentation SwFmtURL::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    rText.Erase();
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            if ( pMap )
                rText.AppendAscii( RTL_CONSTASCII_STRINGPARAM("Client-Map"));
            if ( sURL.Len() )
            {
                if ( pMap )
                    rText.AppendAscii( RTL_CONSTASCII_STRINGPARAM(" - "));
                rText.AppendAscii( RTL_CONSTASCII_STRINGPARAM("URL: "));
                rText += sURL;
                if ( bIsServerMap )
                    rText.AppendAscii( RTL_CONSTASCII_STRINGPARAM(" (Server-Map)"));
            }
            if ( sTargetFrameName.Len() )
            {
                rText.AppendAscii( RTL_CONSTASCII_STRINGPARAM(", Target: "));
                rText += sTargetFrameName;
            }
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}


//SwFmtEditInReadonly


SfxItemPresentation SwFmtEditInReadonly::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    rText.Erase();
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            if ( GetValue() )
                rText = SW_RESSTR(STR_EDIT_IN_READONLY);
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}


SfxItemPresentation SwFmtLayoutSplit::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
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
            if ( GetValue() )
                rText = SW_RESSTR(STR_LAYOUT_SPLIT);
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

SfxItemPresentation SwFmtRowSplit::GetPresentation
(
    SfxItemPresentation /*ePres*/,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             /*rText*/,
    const IntlWrapper*        /*pIntl*/
)   const
{
    return SFX_ITEM_PRESENTATION_NONE;
}


SfxItemPresentation SwFmtFtnEndAtTxtEnd::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
)   const
{
    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            rText.Erase();
            break;

        case SFX_ITEM_PRESENTATION_NAMELESS:
        case SFX_ITEM_PRESENTATION_COMPLETE:
            {
                switch( GetValue() )
                {
                case FTNEND_ATPGORDOCEND:
                    break;

                case FTNEND_ATTXTEND:
                    break;

                case FTNEND_ATTXTEND_OWNNUMSEQ:
                    break;
                }
            }
            break;

        default:
            ePres = SFX_ITEM_PRESENTATION_NONE;
            break;
    }
    return ePres;
}

SfxItemPresentation SwFmtChain::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*        /*pIntl*/
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
            if ( GetPrev() || GetNext() )
            {
                rText = SW_RESSTR(STR_CONNECT1);
                if ( GetPrev() )
                {
                    rText += GetPrev()->GetName();
                    if ( GetNext() )
                        rText += SW_RESSTR(STR_CONNECT2);
                }
                if ( GetNext() )
                    rText += GetNext()->GetName();
            }
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}


SfxItemPresentation SwFmtLineNumber::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*    /*pIntl*/
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
            if ( IsCount() )
                rText += SW_RESSTR(STR_LINECOUNT);
            else
                rText += SW_RESSTR(STR_DONTLINECOUNT);
            if ( GetStartValue() )
            {
                rText += ' ';
                rText += SW_RESSTR(STR_LINCOUNT_START);
                rText += String::CreateFromInt32(GetStartValue());
            }
            return ePres;
        }
        default:;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

SfxItemPresentation SwTextGridItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             rText,
    const IntlWrapper*  /*pIntl*/
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
            sal_uInt16 nId = 0;

            switch ( GetGridType() )
            {
            case GRID_NONE :
                nId = STR_GRID_NONE;
                break;
            case GRID_LINES_ONLY :
                nId = STR_GRID_LINES_ONLY;
                break;
            case GRID_LINES_CHARS :
                nId = STR_GRID_LINES_CHARS;
                break;
            }
            if ( nId )
                rText += SW_RESSTR( nId );
            return ePres;
        }
        default:;//prevent warning
    }

    return SFX_ITEM_PRESENTATION_NONE;
}


//SwHeaderAndFooterEatSpacingItem


SfxItemPresentation SwHeaderAndFooterEatSpacingItem::GetPresentation
(
    SfxItemPresentation /*ePres*/,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    String&             /*rText*/,
    const IntlWrapper*        /*pIntl*/
)   const
{
    return SFX_ITEM_PRESENTATION_NONE;
}


// ---------------------- Grafik-Attribute --------------------------

SfxItemPresentation SwMirrorGrf::GetPresentation(
    SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/, SfxMapUnit /*ePresUnit*/,
    String& rText, const IntlWrapper* /*pIntl*/ ) const
{
    switch ( ePres )
    {
    case SFX_ITEM_PRESENTATION_NAMELESS:
    case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            sal_uInt16 nId;
            switch( GetValue() )
            {
            case RES_MIRROR_GRAPH_DONT:     nId = STR_NO_MIRROR;    break;
            case RES_MIRROR_GRAPH_VERT: nId = STR_VERT_MIRROR;  break;
            case RES_MIRROR_GRAPH_HOR:  nId = STR_HORI_MIRROR;  break;
            case RES_MIRROR_GRAPH_BOTH: nId = STR_BOTH_MIRROR;  break;
            default:                    nId = 0;    break;
            }
            if ( nId )
            {
                rText = SW_RESSTR( nId );
                if (bGrfToggle)
                    rText += SW_RESSTR( STR_MIRROR_TOGGLE );
            }
        }
        break;
    default:
        ePres = SFX_ITEM_PRESENTATION_NONE;
        rText.Erase();
        break;
    }
    return ePres;
}


SfxItemPresentation SwRotationGrf::GetPresentation(
    SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/, SfxMapUnit /*ePresUnit*/,
    String &rText, const IntlWrapper* /*pIntl*/) const
{
    switch( ePres )
    {
    case SFX_ITEM_PRESENTATION_NAMELESS:
    case SFX_ITEM_PRESENTATION_COMPLETE:
        if( SFX_ITEM_PRESENTATION_COMPLETE == ePres )
            rText = SW_RESSTR( STR_ROTATION );
        else if( rText.Len() )
            rText.Erase();
        ( rText += UniString::CreateFromInt32( GetValue() )) += '\xB0';
        break;

    default:
        ePres = SFX_ITEM_PRESENTATION_NONE;
        rText.Erase();
        break;
    }
    return ePres;
}

SfxItemPresentation SwLuminanceGrf::GetPresentation(
    SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/, SfxMapUnit /*ePresUnit*/,
    String &rText, const IntlWrapper* /*pIntl*/) const
{
    switch( ePres )
    {
    case SFX_ITEM_PRESENTATION_NAMELESS:
    case SFX_ITEM_PRESENTATION_COMPLETE:
        if( SFX_ITEM_PRESENTATION_COMPLETE == ePres )
            rText = SW_RESSTR( STR_LUMINANCE );
        else if( rText.Len() )
            rText.Erase();
        ( rText += UniString::CreateFromInt32( GetValue() )) += '%';
        break;

    default:
        ePres = SFX_ITEM_PRESENTATION_NONE;
        rText.Erase();
        break;
    }
    return ePres;
}

SfxItemPresentation SwContrastGrf::GetPresentation(
    SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/, SfxMapUnit /*ePresUnit*/,
    String &rText, const IntlWrapper* /*pIntl*/) const
{
    switch( ePres )
    {
    case SFX_ITEM_PRESENTATION_NAMELESS:
    case SFX_ITEM_PRESENTATION_COMPLETE:
        if( SFX_ITEM_PRESENTATION_COMPLETE == ePres )
            rText = SW_RESSTR( STR_CONTRAST );
        else if( rText.Len() )
            rText.Erase();
        ( rText += UniString::CreateFromInt32( GetValue() )) += '%';
        break;

    default:
        ePres = SFX_ITEM_PRESENTATION_NONE;
        rText.Erase();
        break;
    }
    return ePres;
}

SfxItemPresentation SwChannelGrf::GetPresentation(
    SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/, SfxMapUnit /*ePresUnit*/,
    String &rText, const IntlWrapper* /*pIntl*/) const
{
    switch( ePres )
    {
    case SFX_ITEM_PRESENTATION_NAMELESS:
    case SFX_ITEM_PRESENTATION_COMPLETE:
        if( SFX_ITEM_PRESENTATION_COMPLETE == ePres )
        {
            sal_uInt16 nId;
            switch ( Which() )
            {
            case RES_GRFATR_CHANNELR:   nId = STR_CHANNELR; break;
            case RES_GRFATR_CHANNELG:   nId = STR_CHANNELG; break;
            case RES_GRFATR_CHANNELB:   nId = STR_CHANNELB; break;
            default:                    nId = 0; break;
            }
            if( nId )
                rText = SW_RESSTR( nId );
            else if( rText.Len() )
                rText.Erase();
        }
        else if( rText.Len() )
            rText.Erase();
        ( rText += UniString::CreateFromInt32( GetValue() )) += '%';
        break;

    default:
        ePres = SFX_ITEM_PRESENTATION_NONE;
        rText.Erase();
        break;
    }
    return ePres;
}

SfxItemPresentation SwGammaGrf::GetPresentation(
    SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/, SfxMapUnit /*ePresUnit*/,
    String &rText, const IntlWrapper* /*pIntl*/) const
{
    rtl::OUStringBuffer aText;
    switch( ePres )
    {
    case SFX_ITEM_PRESENTATION_NAMELESS:
    case SFX_ITEM_PRESENTATION_COMPLETE:
        if( SFX_ITEM_PRESENTATION_COMPLETE == ePres )
            aText.append(SW_RESSTR(STR_GAMMA));
        aText.append(GetValue()).append(static_cast<sal_Unicode>('%'));
        break;

    default:
        ePres = SFX_ITEM_PRESENTATION_NONE;
        break;
    }
    rText = aText.makeStringAndClear();
    return ePres;
}

SfxItemPresentation SwInvertGrf::GetPresentation(
    SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/, SfxMapUnit /*ePresUnit*/,
    String &rText, const IntlWrapper* /*pIntl*/) const
{
    rText.Erase();
    switch( ePres )
    {
    case SFX_ITEM_PRESENTATION_NAMELESS:
    case SFX_ITEM_PRESENTATION_COMPLETE:
        if( SFX_ITEM_PRESENTATION_COMPLETE == ePres )
        {
            sal_uInt16 nId = 0 != GetValue() ? STR_INVERT : STR_INVERT_NOT;
            rText = SW_RESSTR( nId );
        }
        break;

    default:
        ePres = SFX_ITEM_PRESENTATION_NONE;
        break;
    }
    return ePres;
}

SfxItemPresentation SwTransparencyGrf::GetPresentation(
    SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/, SfxMapUnit /*ePresUnit*/,
    String &rText, const IntlWrapper* /*pIntl*/) const
{
    switch( ePres )
    {
    case SFX_ITEM_PRESENTATION_NAMELESS:
    case SFX_ITEM_PRESENTATION_COMPLETE:
        if( SFX_ITEM_PRESENTATION_COMPLETE == ePres )
            rText = SW_RESSTR( STR_TRANSPARENCY );
        else if( rText.Len() )
            rText.Erase();
        ( rText += UniString::CreateFromInt32( GetValue() )) += '%';
        break;

    default:
        ePres = SFX_ITEM_PRESENTATION_NONE;
        rText.Erase();
        break;
    }
    return ePres;
}

SfxItemPresentation SwDrawModeGrf::GetPresentation(
    SfxItemPresentation ePres, SfxMapUnit /*eCoreUnit*/, SfxMapUnit /*ePresUnit*/,
    String &rText, const IntlWrapper* /*pIntl*/) const
{
    rText.Erase();
    switch( ePres )
    {
    case SFX_ITEM_PRESENTATION_NAMELESS:
    case SFX_ITEM_PRESENTATION_COMPLETE:
        if( SFX_ITEM_PRESENTATION_COMPLETE == ePres )
        {
            sal_uInt16 nId;
            switch ( GetValue() )
            {

            case GRAPHICDRAWMODE_GREYS:     nId = STR_DRAWMODE_GREY; break;
            case GRAPHICDRAWMODE_MONO:      nId = STR_DRAWMODE_BLACKWHITE; break;
            case GRAPHICDRAWMODE_WATERMARK: nId = STR_DRAWMODE_WATERMARK; break;
            default:                        nId = STR_DRAWMODE_STD; break;
            }
            (rText = SW_RESSTR( STR_DRAWMODE ) ) += SW_RESSTR( nId );
        }
        break;

    default:
        ePres = SFX_ITEM_PRESENTATION_NONE;
        break;
    }
    return ePres;
}

SfxItemPresentation SwFmtFollowTextFlow::GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit /*eCoreMetric*/,
                                    SfxMapUnit /*ePresMetric*/,
                                    String &rText,
                                    const IntlWrapper*    /*pIntl*/ ) const
{
    rText.Erase();
    switch( ePres )
    {
        case SFX_ITEM_PRESENTATION_NAMELESS:
        break;
        case SFX_ITEM_PRESENTATION_COMPLETE:
            {
                sal_uInt16 nId = GetValue() ? STR_FOLLOW_TEXT_FLOW : STR_DONT_FOLLOW_TEXT_FLOW;
                rText = SW_RESSTR( nId );
            }
        break;

        default:
            ePres = SFX_ITEM_PRESENTATION_NONE;
        break;
    }
    return ePres;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
