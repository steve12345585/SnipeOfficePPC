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

#include "CharacterProperties.hxx"
#include "ContainerHelper.hxx"
#include "macros.hxx"

#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/awt/FontSlant.hpp>
#include <com/sun/star/lang/Locale.hpp>

#include <com/sun/star/awt/FontFamily.hpp>
#include <com/sun/star/awt/CharSet.hpp>
#include <com/sun/star/awt/FontPitch.hpp>
#include <com/sun/star/awt/FontUnderline.hpp>
#include <com/sun/star/awt/FontWeight.hpp>
#include <com/sun/star/style/CaseMap.hpp>
#include <com/sun/star/text/FontRelief.hpp>
#include <com/sun/star/text/FontEmphasis.hpp>
#include <com/sun/star/text/RubyAdjust.hpp>
#include <com/sun/star/awt/FontStrikeout.hpp>
#include <com/sun/star/text/WritingMode2.hpp>
#include <com/sun/star/i18n/ScriptType.hpp>

#include <comphelper/InlineContainer.hxx>


// header for struct SvtLinguConfig
#include <unotools/lingucfg.hxx>
#include <i18npool/mslangid.hxx>
#include <i18npool/languagetag.hxx>
#include <vcl/outdev.hxx>

using namespace ::com::sun::star;

using ::com::sun::star::beans::Property;

using ::rtl::OUString;

namespace chart
{

void CharacterProperties::AddPropertiesToVector(
    ::std::vector< Property > & rOutProperties )
{
    // CharacterProperties
    rOutProperties.push_back(
        Property( C2U( "CharFontName" ),
                  PROP_CHAR_FONT_NAME,
                  ::getCppuType( reinterpret_cast< const OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    rOutProperties.push_back(
        Property( C2U( "CharFontStyleName" ),
                  PROP_CHAR_FONT_STYLE_NAME,
                  ::getCppuType( reinterpret_cast< const  OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharFontFamily (see awt.FontFamily)
    rOutProperties.push_back(
        Property( C2U( "CharFontFamily" ),
                  PROP_CHAR_FONT_FAMILY,
                  ::getCppuType( reinterpret_cast< const  sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontCharSet (see awt.CharSet)
    rOutProperties.push_back(
        Property( C2U( "CharFontCharSet" ),
                  PROP_CHAR_FONT_CHAR_SET,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontPitch (see awt.FontPitch)
    rOutProperties.push_back(
        Property( C2U( "CharFontPitch" ),
                  PROP_CHAR_FONT_PITCH,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharColor
    rOutProperties.push_back(
        Property( C2U( "CharColor" ),
                  PROP_CHAR_COLOR,
                  ::getCppuType( reinterpret_cast< const sal_Int32 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharEscapement
    rOutProperties.push_back(
        Property( C2U( "CharEscapement" ),
                  PROP_CHAR_ESCAPEMENT,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharHeight
    rOutProperties.push_back(
        Property( C2U( "CharHeight" ),
                  PROP_CHAR_CHAR_HEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharUnderline (see awt.FontUnderline)
    rOutProperties.push_back(
        Property( C2U( "CharUnderline" ),
                  PROP_CHAR_UNDERLINE,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharUnderlineColor
    rOutProperties.push_back(
        Property( C2U( "CharUnderlineColor" ),
                  PROP_CHAR_UNDERLINE_COLOR,
                  ::getCppuType( reinterpret_cast< const sal_Int32 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharUnderlineHasColor
    rOutProperties.push_back(
        Property( C2U( "CharUnderlineHasColor" ),
                  PROP_CHAR_UNDERLINE_HAS_COLOR,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharOverline (see awt.FontUnderline)
    rOutProperties.push_back(
        Property( C2U( "CharOverline" ),
                  PROP_CHAR_OVERLINE,
                  ::getCppuType( reinterpret_cast< const sal_Int16* >( 0 ) ),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ) );
    // CharOverlineColor
    rOutProperties.push_back(
        Property( C2U( "CharOverlineColor" ),
                  PROP_CHAR_OVERLINE_COLOR,
                  ::getCppuType( reinterpret_cast< const sal_Int32* >( 0 ) ),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ) );
    // CharOverlineHasColor
    rOutProperties.push_back(
        Property( C2U( "CharOverlineHasColor" ),
                  PROP_CHAR_OVERLINE_HAS_COLOR,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ) );
    // CharWeight (see awt.FontWeight)
    rOutProperties.push_back(
        Property( C2U( "CharWeight" ),
                  PROP_CHAR_WEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharPosture
    rOutProperties.push_back(
        Property( C2U( "CharPosture" ),
                  PROP_CHAR_POSTURE,
                  ::getCppuType( reinterpret_cast< const awt::FontSlant * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    rOutProperties.push_back(
        Property( C2U( "CharAutoKerning" ),
                  PROP_CHAR_AUTO_KERNING,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    rOutProperties.push_back(
        Property( C2U( "CharKerning" ),
                  PROP_CHAR_KERNING,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharEscapementHeight
    rOutProperties.push_back(
        Property( C2U( "CharEscapementHeight" ),
                  PROP_CHAR_ESCAPEMENT_HEIGHT,
                  ::getCppuType( reinterpret_cast< const sal_Int8 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharStrikeout (see awt.FontStrikeout)
    rOutProperties.push_back(
        Property( C2U( "CharStrikeout" ),
                  PROP_CHAR_STRIKE_OUT,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharWordMode
    rOutProperties.push_back(
        Property( C2U( "CharWordMode" ),
                  PROP_CHAR_WORD_MODE,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharLocale
    rOutProperties.push_back(
        Property( C2U( "CharLocale" ),
                  PROP_CHAR_LOCALE,
                  ::getCppuType( reinterpret_cast< const lang::Locale * >(0)),
                  //#i111967# no PropertyChangeEvent is fired on change so far
                  beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharShadowed
    rOutProperties.push_back(
        Property( C2U( "CharShadowed" ),
                  PROP_CHAR_SHADOWED,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharContoured
    rOutProperties.push_back(
        Property( C2U( "CharContoured" ),
                  PROP_CHAR_CONTOURED,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharRelief (see text.FontRelief)
    rOutProperties.push_back(
        Property( C2U( "CharRelief" ),
                  PROP_CHAR_RELIEF,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    // CharEmphasize (see text.FontEmphasis)
    rOutProperties.push_back(
        Property( C2U( "CharEmphasis" ),
                  PROP_CHAR_EMPHASIS,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharacterPropertiesAsian
    // =====
    // CharFontNameAsian
    rOutProperties.push_back(
        Property( C2U( "CharFontNameAsian" ),
                  PROP_CHAR_ASIAN_FONT_NAME,
                  ::getCppuType( reinterpret_cast< const OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontStyleNameAsian
    rOutProperties.push_back(
        Property( C2U( "CharFontStyleNameAsian" ),
                  PROP_CHAR_ASIAN_FONT_STYLE_NAME,
                  ::getCppuType( reinterpret_cast< const OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharFontFamilyAsian (see awt.FontFamily)
    rOutProperties.push_back(
        Property( C2U( "CharFontFamilyAsian" ),
                  PROP_CHAR_ASIAN_FONT_FAMILY,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontCharSetAsian (see awt.CharSet)
    rOutProperties.push_back(
        Property( C2U( "CharFontCharSetAsian" ),
                  PROP_CHAR_ASIAN_CHAR_SET,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontPitchAsian (see awt.FontPitch)
    rOutProperties.push_back(
        Property( C2U( "CharFontPitchAsian" ),
                  PROP_CHAR_ASIAN_FONT_PITCH,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharHeightAsian
    rOutProperties.push_back(
        Property( C2U( "CharHeightAsian" ),
                  PROP_CHAR_ASIAN_CHAR_HEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharWeightAsian
    rOutProperties.push_back(
        Property( C2U( "CharWeightAsian" ),
                  PROP_CHAR_ASIAN_WEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharPostureAsian
    rOutProperties.push_back(
        Property( C2U( "CharPostureAsian" ),
                  PROP_CHAR_ASIAN_POSTURE,
                  ::getCppuType( reinterpret_cast< const awt::FontSlant * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharLocaleAsian
    rOutProperties.push_back(
        Property( C2U( "CharLocaleAsian" ),
                  PROP_CHAR_ASIAN_LOCALE,
                  ::getCppuType( reinterpret_cast< const lang::Locale * >(0)),
                  //#i111967# no PropertyChangeEvent is fired on change so far
                  beans::PropertyAttribute::MAYBEDEFAULT ));

    // CharacterPropertiesComplex
    // ===
    // CharFontNameComplex
    rOutProperties.push_back(
        Property( C2U( "CharFontNameComplex" ),
                  PROP_CHAR_COMPLEX_FONT_NAME,
                  ::getCppuType( reinterpret_cast< const OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontStyleNameComplex
    rOutProperties.push_back(
        Property( C2U( "CharFontStyleNameComplex" ),
                  PROP_CHAR_COMPLEX_FONT_STYLE_NAME,
                  ::getCppuType( reinterpret_cast< const OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharFontFamilyComplex (see awt.FontFamily)
    rOutProperties.push_back(
        Property( C2U( "CharFontFamilyComplex" ),
                  PROP_CHAR_COMPLEX_FONT_FAMILY,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontCharSetComplex (see awt.CharSet)
    rOutProperties.push_back(
        Property( C2U( "CharFontCharSetComplex" ),
                  PROP_CHAR_COMPLEX_CHAR_SET,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontPitchComplex (see awt.FontPitch)
    rOutProperties.push_back(
        Property( C2U( "CharFontPitchComplex" ),
                  PROP_CHAR_COMPLEX_FONT_PITCH,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharHeightComplex
    rOutProperties.push_back(
        Property( C2U( "CharHeightComplex" ),
                  PROP_CHAR_COMPLEX_CHAR_HEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharWeightComplex
    rOutProperties.push_back(
        Property( C2U( "CharWeightComplex" ),
                  PROP_CHAR_COMPLEX_WEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharPostureComplex
    rOutProperties.push_back(
        Property( C2U( "CharPostureComplex" ),
                  PROP_CHAR_COMPLEX_POSTURE,
                  ::getCppuType( reinterpret_cast< const awt::FontSlant * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharLocaleComplex
    rOutProperties.push_back(
        Property( C2U( "CharLocaleComplex" ),
                  PROP_CHAR_COMPLEX_LOCALE,
                  ::getCppuType( reinterpret_cast< const lang::Locale * >(0)),
                  //#i111967# no PropertyChangeEvent is fired on change so far
                  beans::PropertyAttribute::MAYBEDEFAULT ));

    // Writing Mode left to right vs right to left
    rOutProperties.push_back(
        Property( C2U( "WritingMode" ),
                  PROP_WRITING_MODE,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)), /*com::sun::star::text::WritingMode2*/
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    rOutProperties.push_back(
        Property( C2U( "ParaIsCharacterDistance" ),
                  PROP_PARA_IS_CHARACTER_DISTANCE,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
}

void CharacterProperties::AddDefaultsToMap(
    ::chart::tPropertyValueMap & rOutMap )
{
    const float fDefaultFontHeight = 13.0;

    SvtLinguConfig aLinguConfig;
    lang::Locale aDefaultLocale;
    aLinguConfig.GetProperty(C2U("DefaultLocale")) >>= aDefaultLocale;
    lang::Locale aDefaultLocale_CJK;
    aLinguConfig.GetProperty(C2U("DefaultLocale_CJK")) >>= aDefaultLocale_CJK;
    lang::Locale aDefaultLocale_CTL;
    aLinguConfig.GetProperty(C2U("DefaultLocale_CTL")) >>= aDefaultLocale_CTL;

    using namespace ::com::sun::star::i18n::ScriptType;
    LanguageType nLang;
    nLang = MsLangId::resolveSystemLanguageByScriptType(LanguageTag(aDefaultLocale).getLanguageType( false), LATIN);
    Font aFont = OutputDevice::GetDefaultFont( DEFAULTFONT_LATIN_SPREADSHEET, nLang, DEFAULTFONT_FLAGS_ONLYONE, 0 );
    nLang = MsLangId::resolveSystemLanguageByScriptType(LanguageTag( aDefaultLocale_CJK).getLanguageType( false), ASIAN);
    Font aFontCJK = OutputDevice::GetDefaultFont( DEFAULTFONT_CJK_SPREADSHEET, nLang, DEFAULTFONT_FLAGS_ONLYONE, 0 );
    nLang = MsLangId::resolveSystemLanguageByScriptType(LanguageTag( aDefaultLocale_CTL).getLanguageType( false), COMPLEX);
    Font aFontCTL = OutputDevice::GetDefaultFont( DEFAULTFONT_CTL_SPREADSHEET, nLang, DEFAULTFONT_FLAGS_ONLYONE, 0 );

    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_FONT_NAME, OUString( aFont.GetName() ) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_FONT_STYLE_NAME, OUString(aFont.GetStyleName()) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_FONT_FAMILY, sal_Int16(aFont.GetFamily()) );//awt::FontFamily::SWISS
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_FONT_CHAR_SET, sal_Int16(aFont.GetCharSet()) );//use awt::CharSet::DONTKNOW instead of SYSTEM to avoid assertion issue 50249
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_FONT_PITCH, sal_Int16(aFont.GetPitch()) );//awt::FontPitch::VARIABLE
    ::chart::PropertyHelper::setPropertyValueDefault< sal_Int32 >( rOutMap, PROP_CHAR_COLOR, -1 ); //automatic color (COL_AUTO)
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_CHAR_HEIGHT, fDefaultFontHeight );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_UNDERLINE, awt::FontUnderline::NONE );
    ::chart::PropertyHelper::setPropertyValueDefault< sal_Int32 >( rOutMap, PROP_CHAR_UNDERLINE_COLOR, -1 ); //automatic color (COL_AUTO)
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_UNDERLINE_HAS_COLOR, false );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_OVERLINE, awt::FontUnderline::NONE );
    ::chart::PropertyHelper::setPropertyValueDefault< sal_Int32 >( rOutMap, PROP_CHAR_OVERLINE_COLOR, -1 ); //automatic color (COL_AUTO)
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_OVERLINE_HAS_COLOR, false );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_WEIGHT, awt::FontWeight::NORMAL );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_POSTURE, awt::FontSlant_NONE );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_AUTO_KERNING, true );
    ::chart::PropertyHelper::setPropertyValueDefault< sal_Int16 >( rOutMap, PROP_CHAR_KERNING, 0 );
    ::chart::PropertyHelper::setPropertyValueDefault< sal_Int16 >( rOutMap, PROP_CHAR_STRIKE_OUT, awt::FontStrikeout::NONE );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_WORD_MODE, false );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_LOCALE, aDefaultLocale );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_SHADOWED, false );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_CONTOURED, false );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_RELIEF, text::FontRelief::NONE );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_EMPHASIS, text::FontEmphasis::NONE );

    // Asian (com.sun.star.style.CharacterPropertiesAsian)
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_ASIAN_CHAR_HEIGHT, fDefaultFontHeight );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_ASIAN_WEIGHT, awt::FontWeight::NORMAL );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_ASIAN_POSTURE, awt::FontSlant_NONE );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_ASIAN_LOCALE, aDefaultLocale_CJK );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_ASIAN_FONT_NAME, OUString( aFontCJK.GetName() ) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_ASIAN_FONT_STYLE_NAME, OUString(aFontCJK.GetStyleName()) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_ASIAN_FONT_FAMILY, sal_Int16(aFontCJK.GetFamily()) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_ASIAN_CHAR_SET, sal_Int16(aFontCJK.GetCharSet()) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_ASIAN_FONT_PITCH, sal_Int16(aFontCJK.GetPitch()) );

    // Complex Text Layout (com.sun.star.style.CharacterPropertiesComplex)
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_COMPLEX_CHAR_HEIGHT, fDefaultFontHeight );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_COMPLEX_WEIGHT, awt::FontWeight::NORMAL );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_COMPLEX_POSTURE, awt::FontSlant_NONE );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_COMPLEX_LOCALE, aDefaultLocale_CTL );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_COMPLEX_FONT_NAME, OUString( aFontCTL.GetName() ) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_COMPLEX_FONT_STYLE_NAME, OUString(aFontCTL.GetStyleName()) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_COMPLEX_FONT_FAMILY, sal_Int16(aFontCTL.GetFamily()) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_COMPLEX_CHAR_SET, sal_Int16(aFontCTL.GetCharSet()) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_CHAR_COMPLEX_FONT_PITCH, sal_Int16(aFontCTL.GetPitch()) );

    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_WRITING_MODE, sal_Int16( com::sun::star::text::WritingMode2::PAGE ) );
    ::chart::PropertyHelper::setPropertyValueDefault( rOutMap, PROP_PARA_IS_CHARACTER_DISTANCE, sal_True );
}

bool CharacterProperties::IsCharacterPropertyHandle( sal_Int32 nHandle )
{
    return ( FAST_PROPERTY_ID_START_CHAR_PROP <= nHandle &&
             nHandle < CharacterProperties::FAST_PROPERTY_ID_END_CHAR_PROP );
}

awt::FontDescriptor CharacterProperties::createFontDescriptorFromPropertySet(
    const uno::Reference< beans::XMultiPropertySet > & xMultiPropSet )
{
    awt::FontDescriptor aResult;
    // Note: keep this sorted!
    ::comphelper::MakeVector< OUString > aPropNames
        ( C2U("CharFontCharSet"));                // CharSet
    aPropNames
        ( C2U("CharFontFamily"))                  // Family
        ( C2U("CharFontName"))                    // Name
        ( C2U("CharFontPitch"))                   // Pitch
        ( C2U("CharFontStyleName"))               // StyleName
        ( C2U("CharHeight"))                      // Height
        ( C2U("CharPosture"))                     // Slant
        ( C2U("CharStrikeout"))                   // Strikeout
        ( C2U("CharUnderline"))                   // Underline
        ( C2U("CharWeight"))                      // Weight
        ( C2U("CharWordMode"))                    // WordLineMode
        ;

    uno::Sequence< OUString > aPropNameSeq( ContainerHelper::ContainerToSequence( aPropNames ));
    uno::Sequence< uno::Any > aValues( xMultiPropSet->getPropertyValues( aPropNameSeq ));

    sal_Int32 i=0;
    // Note keep this sorted according to the list above (comments are the fieldnames)
    aValues[ i++ ]  >>= aResult.CharSet;
    aValues[ i++ ]  >>= aResult.Family;
    aValues[ i++ ]  >>= aResult.Name;
    aValues[ i++ ]  >>= aResult.Pitch;
    aValues[ i++ ]  >>= aResult.StyleName;
    float fCharHeight = 0;
    aValues[ i++ ]  >>= fCharHeight;
    aResult.Height = static_cast< sal_Int16 >( fCharHeight );
    aValues[ i++ ]  >>= aResult.Slant;
    aValues[ i++ ]  >>= aResult.Strikeout;
    aValues[ i++ ]  >>= aResult.Underline;
    aValues[ i++ ]  >>= aResult.Weight;
    aValues[ i++ ]  >>= aResult.WordLineMode;
    OSL_ASSERT( i == aValues.getLength());

    return aResult;
}

} //  namespace chart

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
