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


#include <xmloff/controlpropertyhdl.hxx>

#include <com/sun/star/util/MeasureUnit.hpp>
#include <com/sun/star/awt/TextAlign.hpp>
#include <com/sun/star/awt/FontWidth.hpp>
#include <com/sun/star/awt/FontEmphasisMark.hpp>

#include <sax/tools/converter.hxx>

#include <xmloff/xmltypes.hxx>
#include "xmloff/NamedBoolPropertyHdl.hxx"
#include "formenums.hxx"
#include <xmloff/xmluconv.hxx>
#include <xmloff/xmltoken.hxx>
#include <rtl/ustrbuf.hxx>
#include <osl/diagnose.h>
#include "callbacks.hxx"
#include <xmloff/XMLConstantsPropertyHandler.hxx>

//.........................................................................
namespace xmloff
{
//.........................................................................

    using namespace ::com::sun::star;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::beans;
    using namespace ::xmloff::token;

    //=====================================================================
    //= OControlPropertyHandlerFactory
    //=====================================================================
    //---------------------------------------------------------------------
    OControlPropertyHandlerFactory::OControlPropertyHandlerFactory()
        :m_pTextAlignHandler(NULL)
        ,m_pControlBorderStyleHandler(NULL)
        ,m_pControlBorderColorHandler(NULL)
        ,m_pRotationAngleHandler(NULL)
        ,m_pFontWidthHandler(NULL)
        ,m_pFontEmphasisHandler(NULL)
        ,m_pFontReliefHandler(NULL)
    {
    }

    //---------------------------------------------------------------------
    OControlPropertyHandlerFactory::~OControlPropertyHandlerFactory()
    {
        delete m_pTextAlignHandler;
        delete m_pControlBorderStyleHandler;
        delete m_pControlBorderColorHandler;
        delete m_pRotationAngleHandler;
        delete m_pFontWidthHandler;
        delete m_pFontEmphasisHandler;
        delete m_pFontReliefHandler;
    }

    //---------------------------------------------------------------------
    const XMLPropertyHandler* OControlPropertyHandlerFactory::GetPropertyHandler(sal_Int32 _nType) const
    {
        const XMLPropertyHandler* pHandler = NULL;

        switch (_nType)
        {
            case XML_TYPE_TEXT_ALIGN:
                if (!m_pTextAlignHandler)
                    m_pTextAlignHandler = new XMLConstantsPropertyHandler(OEnumMapper::getEnumMap(OEnumMapper::epTextAlign), XML_TOKEN_INVALID );
                pHandler = m_pTextAlignHandler;
                break;

            case XML_TYPE_CONTROL_BORDER:
                if (!m_pControlBorderStyleHandler)
                    m_pControlBorderStyleHandler = new OControlBorderHandler( OControlBorderHandler::STYLE );
                pHandler = m_pControlBorderStyleHandler;
                break;

            case XML_TYPE_CONTROL_BORDER_COLOR:
                if ( !m_pControlBorderColorHandler )
                    m_pControlBorderColorHandler = new OControlBorderHandler( OControlBorderHandler::COLOR );
                pHandler = m_pControlBorderColorHandler;
                break;

            case XML_TYPE_ROTATION_ANGLE:
                if (!m_pRotationAngleHandler)
                    m_pRotationAngleHandler = new ORotationAngleHandler;
                pHandler = m_pRotationAngleHandler;
                break;

            case XML_TYPE_FONT_WIDTH:
                if (!m_pFontWidthHandler)
                    m_pFontWidthHandler = new OFontWidthHandler;
                pHandler = m_pFontWidthHandler;
                break;

            case XML_TYPE_CONTROL_TEXT_EMPHASIZE:
                if (!m_pFontEmphasisHandler)
                    m_pFontEmphasisHandler = new XMLConstantsPropertyHandler( OEnumMapper::getEnumMap(OEnumMapper::epFontEmphasis), XML_NONE );
                pHandler = m_pFontEmphasisHandler;
                break;

            case XML_TYPE_TEXT_FONT_RELIEF:
                if (!m_pFontReliefHandler)
                    m_pFontReliefHandler = new XMLConstantsPropertyHandler( OEnumMapper::getEnumMap(OEnumMapper::epFontRelief), XML_NONE );
                pHandler = m_pFontReliefHandler;
                break;
            case XML_TYPE_TEXT_LINE_MODE:
                pHandler = new XMLNamedBoolPropertyHdl(
                                            ::xmloff::token::XML_SKIP_WHITE_SPACE,
                                            ::xmloff::token::XML_CONTINUOUS);
                break;
        }

        if (!pHandler)
            pHandler = XMLPropertyHandlerFactory::GetPropertyHandler(_nType);
        return pHandler;
    }

    //=====================================================================
    //= OControlTextEmphasisHandler
    //=====================================================================
    OControlTextEmphasisHandler::OControlTextEmphasisHandler()
    {
    }

    //---------------------------------------------------------------------
    sal_Bool OControlTextEmphasisHandler::exportXML( ::rtl::OUString& _rStrExpValue, const Any& _rValue, const SvXMLUnitConverter& ) const
    {
        ::rtl::OUStringBuffer aReturn;
        sal_Bool bSuccess = sal_False;
        sal_Int16 nFontEmphasis = sal_Int16();
        if (_rValue >>= nFontEmphasis)
        {
            // the type
            sal_Int16 nType = nFontEmphasis & ~(FontEmphasisMark::ABOVE | FontEmphasisMark::BELOW);
            // the position of the mark
            sal_Bool bBelow = 0 != (nFontEmphasis & FontEmphasisMark::BELOW);

            // convert
            bSuccess = SvXMLUnitConverter::convertEnum(aReturn, nType, OEnumMapper::getEnumMap(OEnumMapper::epFontEmphasis), XML_NONE);
            if (bSuccess)
            {
                aReturn.append( (sal_Unicode)' ' );
                aReturn.append( GetXMLToken(bBelow ? XML_BELOW : XML_ABOVE) );

                _rStrExpValue = aReturn.makeStringAndClear();
            }
        }

        return bSuccess;
    }

    //---------------------------------------------------------------------
    sal_Bool OControlTextEmphasisHandler::importXML( const ::rtl::OUString& _rStrImpValue, Any& _rValue, const SvXMLUnitConverter& ) const
    {
        sal_Bool bSuccess = sal_True;
        sal_uInt16 nEmphasis = FontEmphasisMark::NONE;

        sal_Bool bBelow = sal_False;
        sal_Bool bHasPos = sal_False, bHasType = sal_False;

        ::rtl::OUString sToken;
        SvXMLTokenEnumerator aTokenEnum(_rStrImpValue);
        while (aTokenEnum.getNextToken(sToken))
        {
            if (!bHasPos)
            {
                if (IsXMLToken(sToken, XML_ABOVE))
                {
                    bBelow = sal_False;
                    bHasPos = sal_True;
                }
                else if (IsXMLToken(sToken, XML_BELOW))
                {
                    bBelow = sal_True;
                    bHasPos = sal_True;
                }
            }
            if (!bHasType)
            {
                if (SvXMLUnitConverter::convertEnum(nEmphasis, sToken, OEnumMapper::getEnumMap(OEnumMapper::epFontEmphasis)))
                {
                    bHasType = sal_True;
                }
                else
                {
                    bSuccess = sal_False;
                    break;
                }
            }
        }

        if (bSuccess)
        {
            nEmphasis |= bBelow ? FontEmphasisMark::BELOW : FontEmphasisMark::ABOVE;
            _rValue <<= (sal_Int16)nEmphasis;
        }

        return bSuccess;
    }

    //=====================================================================
    //= OControlBorderHandlerBase
    //=====================================================================
    //---------------------------------------------------------------------
    OControlBorderHandler::OControlBorderHandler( const OControlBorderHandler::BorderFacet _eFacet )
        :m_eFacet( _eFacet )
    {
    }

    //---------------------------------------------------------------------
    sal_Bool OControlBorderHandler::importXML( const ::rtl::OUString& _rStrImpValue, Any& _rValue, const SvXMLUnitConverter& ) const
    {
        ::rtl::OUString sToken;
        SvXMLTokenEnumerator aTokens(_rStrImpValue);

        sal_uInt16 nStyle = 1;

        while   (   aTokens.getNextToken(sToken)    // have a new token
                &&  (!sToken.isEmpty())       // really have a new token
                )
        {
            // try interpreting the token as border style
            if ( m_eFacet == STYLE )
            {
                // is it a valid enum value?
                if ( SvXMLUnitConverter::convertEnum( nStyle, sToken, OEnumMapper::getEnumMap( OEnumMapper::epBorderWidth ) ) )
                {
                    _rValue <<= nStyle;
                    return sal_True;
                }
            }

            // try interpreting it as color value
            if ( m_eFacet == COLOR )
            {
                sal_Int32 nColor(0);
                if (::sax::Converter::convertColor( nColor, sToken ))
                {
                    _rValue <<= nColor;
                    return sal_True;
                }
            }
        }

        return sal_False;
    }

    //---------------------------------------------------------------------
    sal_Bool OControlBorderHandler::exportXML( ::rtl::OUString& _rStrExpValue, const Any& _rValue, const SvXMLUnitConverter& ) const
    {
        sal_Bool bSuccess = sal_False;

        ::rtl::OUStringBuffer aOut;
        switch ( m_eFacet )
        {
        case STYLE:
        {
            sal_Int16 nBorder = 0;
            bSuccess =  (_rValue >>= nBorder)
                    &&  SvXMLUnitConverter::convertEnum( aOut, nBorder, OEnumMapper::getEnumMap( OEnumMapper::epBorderWidth ) );
        }
        break;
        case COLOR:
        {
            sal_Int32 nBorderColor = 0;
            if ( _rValue >>= nBorderColor )
            {
                ::sax::Converter::convertColor(aOut, nBorderColor);
                bSuccess = sal_True;
            }
        }
        break;
        }   // switch ( m_eFacet )

        if ( !bSuccess )
            return sal_False;

        if ( !_rStrExpValue.isEmpty() )
            _rStrExpValue += ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( " " ) );
        _rStrExpValue += aOut.makeStringAndClear();

        return sal_True;
    }

    //=====================================================================
    //= OFontWidthHandler
    //=====================================================================
    //---------------------------------------------------------------------
    OFontWidthHandler::OFontWidthHandler()
    {
    }

    //---------------------------------------------------------------------
    sal_Bool OFontWidthHandler::importXML( const ::rtl::OUString& _rStrImpValue, Any& _rValue, const SvXMLUnitConverter& ) const
    {
        sal_Int32 nWidth = 0;
        bool const bSuccess = ::sax::Converter::convertMeasure(
                nWidth, _rStrImpValue, util::MeasureUnit::POINT);
        if (bSuccess)
            _rValue <<= (sal_Int16)nWidth;

        return bSuccess;
    }

    //---------------------------------------------------------------------
    sal_Bool OFontWidthHandler::exportXML( ::rtl::OUString& _rStrExpValue, const Any& _rValue, const SvXMLUnitConverter& ) const
    {
        sal_Int16 nWidth = 0;
        ::rtl::OUStringBuffer aResult;
        if (_rValue >>= nWidth)
        {
            ::sax::Converter::convertMeasure(aResult, nWidth,
                    util::MeasureUnit::POINT, util::MeasureUnit::POINT);
        }
        _rStrExpValue = aResult.makeStringAndClear();

        return !_rStrExpValue.isEmpty();
    }

    //=====================================================================
    //= ORotationAngleHandler
    //=====================================================================
    //---------------------------------------------------------------------
    ORotationAngleHandler::ORotationAngleHandler()
    {
    }

    //---------------------------------------------------------------------
    sal_Bool ORotationAngleHandler::importXML( const ::rtl::OUString& _rStrImpValue, Any& _rValue, const SvXMLUnitConverter& ) const
    {
        double fValue;
        bool const bSucces =
            ::sax::Converter::convertDouble(fValue, _rStrImpValue);
        if (bSucces)
        {
            fValue *= 10;
            _rValue <<= (float)fValue;
        }

        return bSucces;
    }

    //---------------------------------------------------------------------
    sal_Bool ORotationAngleHandler::exportXML( ::rtl::OUString& _rStrExpValue, const Any& _rValue, const SvXMLUnitConverter& ) const
    {
        float fAngle = 0;
        sal_Bool bSuccess = (_rValue >>= fAngle);

        if (bSuccess)
        {
            rtl::OUStringBuffer sValue;
            ::sax::Converter::convertDouble(sValue, ((double)fAngle) / 10);
            _rStrExpValue = sValue.makeStringAndClear();
        }

        return bSuccess;
    }

    //=====================================================================
    //= ImageScaleModeHandler
    //=====================================================================
    //---------------------------------------------------------------------
    ImageScaleModeHandler::ImageScaleModeHandler()
        :XMLConstantsPropertyHandler( OEnumMapper::getEnumMap( OEnumMapper::epImageScaleMode ), XML_STRETCH )
    {
    }

//.........................................................................
}   // namespace xmloff
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
