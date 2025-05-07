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

#include "PropertyMapper.hxx"
#include "ContainerHelper.hxx"
#include "macros.hxx"

#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/drawing/LineStyle.hpp>
#include <com/sun/star/drawing/TextVerticalAdjust.hpp>
#include <com/sun/star/drawing/TextHorizontalAdjust.hpp>
#include <com/sun/star/drawing/LineJoint.hpp>

//.............................................................................
namespace chart
{
//.............................................................................
using namespace ::com::sun::star;

namespace
{

void lcl_overwriteOrAppendValues(
    tPropertyNameValueMap &rMap, const tPropertyNameValueMap& rOverwriteMap )
{
    tPropertyNameValueMap::const_iterator aIt( rOverwriteMap.begin() );
    tPropertyNameValueMap::const_iterator aEnd( rOverwriteMap.end() );

    for( ; aIt != aEnd; ++aIt )
        rMap[ aIt->first ] = aIt->second;
}

} // anonymous namespace

void PropertyMapper::setMappedProperties(
          const uno::Reference< beans::XPropertySet >& xTarget
        , const uno::Reference< beans::XPropertySet >& xSource
        , const tPropertyNameMap& rMap
        , tPropertyNameValueMap* pOverwriteMap )
{
    if( !xTarget.is() || !xSource.is() )
        return;

    tNameSequence aNames;
    tAnySequence  aValues;
    getMultiPropertyLists(aNames, aValues, xSource, rMap );
    if(pOverwriteMap && (aNames.getLength() == aValues.getLength()))
    {
        tPropertyNameValueMap aNewMap;
        for( sal_Int32 nI=0; nI<aNames.getLength(); ++nI )
            aNewMap[ aNames[nI] ] = aValues[nI];
        lcl_overwriteOrAppendValues( aNewMap, *pOverwriteMap );
        aNames = ContainerHelper::MapKeysToSequence( aNewMap );
        aValues = ContainerHelper::MapValuesToSequence( aNewMap );
    }

    PropertyMapper::setMultiProperties( aNames, aValues, xTarget );
}

void PropertyMapper::getValueMap(
                  tPropertyNameValueMap& rValueMap
                , const tPropertyNameMap& rNameMap
                , const uno::Reference< beans::XPropertySet >& xSourceProp
                )
{
    tPropertyNameMap::const_iterator aIt( rNameMap.begin() );
    tPropertyNameMap::const_iterator aEnd( rNameMap.end() );

    for( ; aIt != aEnd; ++aIt )
    {
        rtl::OUString aTarget = aIt->first;
        rtl::OUString aSource = aIt->second;
        try
        {
            uno::Any aAny( xSourceProp->getPropertyValue(aSource) );
            if( aAny.hasValue() )
                rValueMap.insert( tPropertyNameValueMap::value_type( aTarget, aAny ) );
        }
        catch( const uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
}

void PropertyMapper::getMultiPropertyLists(
                  tNameSequence& rNames
                , tAnySequence&  rValues
                , const uno::Reference< beans::XPropertySet >& xSourceProp
                , const tPropertyNameMap& rNameMap
                )
{
    tPropertyNameValueMap aValueMap;
    getValueMap( aValueMap, rNameMap, xSourceProp );
    getMultiPropertyListsFromValueMap( rNames, rValues, aValueMap );
}

void PropertyMapper::getMultiPropertyListsFromValueMap(
                  tNameSequence& rNames
                , tAnySequence&  rValues
                , const tPropertyNameValueMap& rValueMap
                )
{
    sal_Int32 nPropertyCount = rValueMap.size();
    rNames.realloc(nPropertyCount);
    rValues.realloc(nPropertyCount);

    //fill sequences
    tPropertyNameValueMap::const_iterator aValueIt(  rValueMap.begin() );
    tPropertyNameValueMap::const_iterator aValueEnd( rValueMap.end()   );
    sal_Int32 nN=0;
    for( ; aValueIt != aValueEnd; ++aValueIt )
    {
        const uno::Any& rAny = aValueIt->second;
        if( rAny.hasValue() )
        {
            //do not set empty anys because of performance (otherwise SdrAttrObj::ItemChange will take much longer)
            rNames[nN]  = aValueIt->first;
            rValues[nN] = rAny;
            ++nN;
        }
    }
    //reduce to real property count
    rNames.realloc(nN);
    rValues.realloc(nN);
}

uno::Any* PropertyMapper::getValuePointer( tAnySequence& rPropValues
                         , const tNameSequence& rPropNames
                         , const rtl::OUString& rPropName )
{
    sal_Int32 nCount = rPropNames.getLength();
    for( sal_Int32 nN = 0; nN < nCount; nN++ )
    {
        if(rPropNames[nN].equals(rPropName))
            return &rPropValues[nN];
    }
    return NULL;
}

uno::Any* PropertyMapper::getValuePointerForLimitedSpace( tAnySequence& rPropValues
                         , const tNameSequence& rPropNames
                         , bool bLimitedHeight)
{
    return PropertyMapper::getValuePointer( rPropValues, rPropNames
        , bLimitedHeight ? C2U("TextMaximumFrameHeight") : C2U("TextMaximumFrameWidth") );
}

const tMakePropertyNameMap& PropertyMapper::getPropertyNameMapForCharacterProperties()
{
    //shape property -- chart model object property
    static tMakePropertyNameMap m_aShapePropertyMapForCharacterProperties =
        tMakePropertyNameMap
        ( "CharColor",                "CharColor" )
        ( "CharContoured",            "CharContoured" )
        ( "CharEmphasis",             "CharEmphasis" )//the service style::CharacterProperties  describes a property called 'CharEmphasize' wich is nowhere implemented

        ( "CharFontFamily",           "CharFontFamily" )
        ( "CharFontFamilyAsian",      "CharFontFamilyAsian" )
        ( "CharFontFamilyComplex",    "CharFontFamilyComplex" )
        ( "CharFontCharSet",          "CharFontCharSet" )
        ( "CharFontCharSetAsian",     "CharFontCharSetAsian" )
        ( "CharFontCharSetComplex",   "CharFontCharSetComplex" )
        ( "CharFontName",             "CharFontName" )
        ( "CharFontNameAsian",        "CharFontNameAsian" )
        ( "CharFontNameComplex",      "CharFontNameComplex" )
        ( "CharFontPitch",            "CharFontPitch" )
        ( "CharFontPitchAsian",       "CharFontPitchAsian" )
        ( "CharFontPitchComplex",     "CharFontPitchComplex" )
        ( "CharFontStyleName",        "CharFontStyleName" )
        ( "CharFontStyleNameAsian",   "CharFontStyleNameAsian" )
        ( "CharFontStyleNameComplex", "CharFontStyleNameComplex" )

        ( "CharHeight",               "CharHeight" )
        ( "CharHeightAsian",          "CharHeightAsian" )
        ( "CharHeightComplex",        "CharHeightComplex" )
        ( "CharKerning",              "CharKerning" )
        ( "CharLocale",               "CharLocale" )
        ( "CharLocaleAsian",          "CharLocaleAsian" )
        ( "CharLocaleComplex",        "CharLocaleComplex" )
        ( "CharPosture",              "CharPosture" )
        ( "CharPostureAsian",         "CharPostureAsian" )
        ( "CharPostureComplex",       "CharPostureComplex" )
        ( "CharRelief",               "CharRelief" )
        ( "CharShadowed",             "CharShadowed" )
        ( "CharStrikeout",            "CharStrikeout" )
        ( "CharUnderline",            "CharUnderline" )
        ( "CharUnderlineColor",       "CharUnderlineColor" )
        ( "CharUnderlineHasColor",    "CharUnderlineHasColor" )
        ( "CharOverline",             "CharOverline" )
        ( "CharOverlineColor",        "CharOverlineColor" )
        ( "CharOverlineHasColor",     "CharOverlineHasColor" )
        ( "CharWeight",               "CharWeight" )
        ( "CharWeightAsian",          "CharWeightAsian" )
        ( "CharWeightComplex",        "CharWeightComplex" )
        ( "CharWordMode",             "CharWordMode" )

        ( "WritingMode",              "WritingMode" )

        ( "ParaIsCharacterDistance",  "ParaIsCharacterDistance" )
        ;
    return m_aShapePropertyMapForCharacterProperties;
}

const tMakePropertyNameMap& PropertyMapper::getPropertyNameMapForParagraphProperties()
{
    //shape property -- chart model object property
    static tMakePropertyNameMap m_aShapePropertyMapForParagraphProperties =
        tMakePropertyNameMap
        ( "ParaAdjust",          "ParaAdjust" )
        ( "ParaBottomMargin",    "ParaBottomMargin" )
        ( "ParaIsHyphenation",   "ParaIsHyphenation" )
        ( "ParaLastLineAdjust",  "ParaLastLineAdjust" )
        ( "ParaLeftMargin",      "ParaLeftMargin" )
        ( "ParaRightMargin",     "ParaRightMargin" )
        ( "ParaTopMargin",       "ParaTopMargin" )
        ;
    return m_aShapePropertyMapForParagraphProperties;
}

const tMakePropertyNameMap& PropertyMapper::getPropertyNameMapForFillProperties()
{
    //shape property -- chart model object property
    static tMakePropertyNameMap m_aShapePropertyMapForFillProperties =
        tMakePropertyNameMap
        ( "FillBackground",               "FillBackground" )
        ( "FillBitmapName",               "FillBitmapName" )
        ( "FillColor",                    "FillColor" )
        ( "FillGradientName",             "FillGradientName" )
        ( "FillGradientStepCount",        "FillGradientStepCount" )
        ( "FillHatchName",                "FillHatchName" )
        ( "FillStyle",                    "FillStyle" )
        ( "FillTransparence",             "FillTransparence" )
        ( "FillTransparenceGradientName", "FillTransparenceGradientName" )
        //bitmap properties
        ( "FillBitmapMode",               "FillBitmapMode" )
        ( "FillBitmapSizeX",              "FillBitmapSizeX" )
        ( "FillBitmapSizeY",              "FillBitmapSizeY" )
        ( "FillBitmapLogicalSize",        "FillBitmapLogicalSize" )
        ( "FillBitmapOffsetX",            "FillBitmapOffsetX" )
        ( "FillBitmapOffsetY",            "FillBitmapOffsetY" )
        ( "FillBitmapRectanglePoint",     "FillBitmapRectanglePoint" )
        ( "FillBitmapPositionOffsetX",    "FillBitmapPositionOffsetX" )
        ( "FillBitmapPositionOffsetY",    "FillBitmapPositionOffsetY" )
        ;
    return m_aShapePropertyMapForFillProperties;
}

const tMakePropertyNameMap& PropertyMapper::getPropertyNameMapForLineProperties()
{
    //shape property -- chart model object property
    static tMakePropertyNameMap m_aShapePropertyMapForLineProperties =
        tMakePropertyNameMap
        ( "LineColor",              "LineColor" )
        ( "LineDashName",           "LineDashName" )
        ( "LineJoint",              "LineJoint" )
        ( "LineStyle",              "LineStyle" )
        ( "LineTransparence",       "LineTransparence" )
        ( "LineWidth",              "LineWidth" )
        ;
    return m_aShapePropertyMapForLineProperties;
}

const tMakePropertyNameMap& PropertyMapper::getPropertyNameMapForFillAndLineProperties()
{
    static tMakePropertyNameMap m_aShapePropertyMapForFillAndLineProperties =
        tMakePropertyNameMap
        ( PropertyMapper::getPropertyNameMapForFillProperties() )
        ( PropertyMapper::getPropertyNameMapForLineProperties() )
        ;

    return m_aShapePropertyMapForFillAndLineProperties;
}

const tMakePropertyNameMap& PropertyMapper::getPropertyNameMapForTextShapeProperties()
{
    static tMakePropertyNameMap m_aShapePropertyMapForTextShapeProperties =
        tMakePropertyNameMap
        ( PropertyMapper::getPropertyNameMapForCharacterProperties() )
        ( PropertyMapper::getPropertyNameMapForFillProperties() )
        ( PropertyMapper::getPropertyNameMapForLineProperties() );

    return m_aShapePropertyMapForTextShapeProperties;
}

const tMakePropertyNameMap& PropertyMapper::getPropertyNameMapForLineSeriesProperties()
{
    //shape property -- chart model object property
    static tMakePropertyNameMap m_aShapePropertyMapForLineSeriesProperties =
        tMakePropertyNameMap
        ( "LineColor",           "Color" )
        ( "LineDashName",        "LineDashName" )
        ( "LineStyle",           "LineStyle" )
        ( "LineTransparence",    "Transparency" )
        ( "LineWidth",           "LineWidth" )

        ;
    return m_aShapePropertyMapForLineSeriesProperties;
}

const tMakePropertyNameMap& PropertyMapper::getPropertyNameMapForFilledSeriesProperties()
{
    //shape property -- chart model object property
    static tMakePropertyNameMap m_aShapePropertyMapForFilledSeriesProperties =
        tMakePropertyNameMap
        ( "FillBackground",               "FillBackground" )
        ( "FillBitmapName",               "FillBitmapName" )
        ( "FillColor",                    "Color" )
        ( "FillGradientName",             "GradientName" )
        ( "FillGradientStepCount",        "GradientStepCount" )
        ( "FillHatchName",                "HatchName" )
        ( "FillStyle",                    "FillStyle" )
        ( "FillTransparence",             "Transparency" )
        ( "FillTransparenceGradientName", "TransparencyGradientName" )
        //bitmap properties
        ( "FillBitmapMode",               "FillBitmapMode" )
        ( "FillBitmapSizeX",              "FillBitmapSizeX" )
        ( "FillBitmapSizeY",              "FillBitmapSizeY" )
        ( "FillBitmapLogicalSize",        "FillBitmapLogicalSize" )
        ( "FillBitmapOffsetX",            "FillBitmapOffsetX" )
        ( "FillBitmapOffsetY",            "FillBitmapOffsetY" )
        ( "FillBitmapRectanglePoint",     "FillBitmapRectanglePoint" )
        ( "FillBitmapPositionOffsetX",    "FillBitmapPositionOffsetX" )
        ( "FillBitmapPositionOffsetY",    "FillBitmapPositionOffsetY" )
        //line properties
        ( "LineColor",                    "BorderColor" )
        ( "LineDashName",                 "BorderDashName" )
        ( "LineStyle",                    "BorderStyle" )
        ( "LineTransparence",             "BorderTransparency" )
        ( "LineWidth",                    "BorderWidth" )
        ;
    return m_aShapePropertyMapForFilledSeriesProperties;
}

void PropertyMapper::setMultiProperties(
                  const tNameSequence& rNames
                , const tAnySequence&  rValues
                , const ::com::sun::star::uno::Reference<
                  ::com::sun::star::beans::XPropertySet >& xTarget )
{
    bool bSuccess = false;
    try
    {
        uno::Reference< beans::XMultiPropertySet > xShapeMultiProp( xTarget, uno::UNO_QUERY );
        if( xShapeMultiProp.is() )
        {
            xShapeMultiProp->setPropertyValues( rNames, rValues );
            bSuccess = true;
        }
    }
    catch( const uno::Exception& e )
    {
        ASSERT_EXCEPTION( e ); //if this occurs more often think of removing the XMultiPropertySet completly for better performance
    }

    if(!bSuccess)
    try
    {
        sal_Int32 nCount = std::max( rNames.getLength(), rValues.getLength() );
        rtl::OUString aPropName;
        uno::Any aValue;
        for( sal_Int32 nN = 0; nN < nCount; nN++ )
        {
            aPropName = rNames[nN];
            aValue = rValues[nN];

            try
            {
                xTarget->setPropertyValue( aPropName, aValue );
            }
            catch( const uno::Exception& e )
            {
                ASSERT_EXCEPTION( e );
            }
        }
    }
    catch( const uno::Exception& e )
    {
        ASSERT_EXCEPTION( e );
    }
}

void PropertyMapper::getTextLabelMultiPropertyLists(
    const uno::Reference< beans::XPropertySet >& xSourceProp
    , tNameSequence& rPropNames, tAnySequence& rPropValues
    , bool bName
    , sal_Int32 nLimitedSpace
    , bool bLimitedHeight )
{
    //fill character properties into the ValueMap
    tPropertyNameValueMap aValueMap;
    PropertyMapper::getValueMap( aValueMap
            , PropertyMapper::getPropertyNameMapForCharacterProperties()
            , xSourceProp );

    //some more shape properties apart from character properties, position-matrix and label string
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("LineStyle"), uno::makeAny(drawing::LineStyle_NONE) ) ); // drawing::LineStyle
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextHorizontalAdjust"), uno::makeAny(drawing::TextHorizontalAdjust_CENTER) ) ); // drawing::TextHorizontalAdjust - needs to be overwritten
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextVerticalAdjust"), uno::makeAny(drawing::TextVerticalAdjust_CENTER) ) ); //drawing::TextVerticalAdjust - needs to be overwritten
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextAutoGrowHeight"), uno::makeAny(sal_True) ) ); // sal_Bool
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextAutoGrowWidth"), uno::makeAny(sal_True) ) ); // sal_Bool
    if( bName )
        aValueMap.insert( tPropertyNameValueMap::value_type( C2U("Name"), uno::makeAny( rtl::OUString() ) ) ); //CID rtl::OUString - needs to be overwritten for each point

    if( nLimitedSpace > 0 )
    {
        if(bLimitedHeight)
            aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextMaximumFrameHeight"), uno::makeAny(nLimitedSpace) ) ); //sal_Int32
        else
            aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextMaximumFrameWidth"), uno::makeAny(nLimitedSpace) ) ); //sal_Int32
        aValueMap.insert( tPropertyNameValueMap::value_type( C2U("ParaIsHyphenation"), uno::makeAny(sal_True) ) );
    }

    PropertyMapper::getMultiPropertyListsFromValueMap( rPropNames, rPropValues, aValueMap );
}

void PropertyMapper::getPreparedTextShapePropertyLists(
    const uno::Reference< beans::XPropertySet >& xSourceProp
    , tNameSequence& rPropNames, tAnySequence& rPropValues )
{
    //fill character, line and fill properties into the ValueMap
    tPropertyNameValueMap aValueMap;
    PropertyMapper::getValueMap( aValueMap
            , PropertyMapper::getPropertyNameMapForTextShapeProperties()
            , xSourceProp );

    // auto-grow makes sure the shape has the correct size after setting text
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextHorizontalAdjust"), uno::makeAny( drawing::TextHorizontalAdjust_CENTER )));
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextVerticalAdjust"), uno::makeAny( drawing::TextVerticalAdjust_CENTER )));
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextAutoGrowHeight"), uno::makeAny( true )));
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextAutoGrowWidth"), uno::makeAny( true )));

    // set some distance to the border, in case it is shown
    const sal_Int32 nWidthDist  = 250;
    const sal_Int32 nHeightDist = 125;
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextLeftDistance"),  uno::makeAny( nWidthDist )));
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextRightDistance"), uno::makeAny( nWidthDist )));
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextUpperDistance"), uno::makeAny( nHeightDist )));
    aValueMap.insert( tPropertyNameValueMap::value_type( C2U("TextLowerDistance"), uno::makeAny( nHeightDist )));

    // use a line-joint showing the border of thick lines like two rectangles
    // filled in between.
    aValueMap[C2U("LineJoint")] <<= drawing::LineJoint_ROUND;

    PropertyMapper::getMultiPropertyListsFromValueMap( rPropNames, rPropValues, aValueMap );
}

//.............................................................................
} //namespace chart
//.............................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
