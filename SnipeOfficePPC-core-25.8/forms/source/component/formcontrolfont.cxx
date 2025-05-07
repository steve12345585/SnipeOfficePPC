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

#include "formcontrolfont.hxx"
#include "property.hrc"
#include "property.hxx"
#include <comphelper/property.hxx>
#include <comphelper/types.hxx>
#include <tools/color.hxx>
#include <toolkit/helper/emptyfontdescriptor.hxx>
#include <com/sun/star/awt/FontRelief.hpp>
#include <com/sun/star/awt/FontEmphasisMark.hpp>

//.........................................................................
namespace frm
{
//.........................................................................

    using namespace ::comphelper;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;

    //------------------------------------------------------------------------------
    namespace
    {
        Any lcl_extractFontDescriptorAggregate( sal_Int32 _nHandle, const FontDescriptor& _rFont )
        {
            Any aValue;
            switch ( _nHandle )
            {
            case PROPERTY_ID_FONT_NAME:
                aValue <<= _rFont.Name;
                break;

            case PROPERTY_ID_FONT_STYLENAME:
                aValue <<= _rFont.StyleName;
                break;

            case PROPERTY_ID_FONT_FAMILY:
                aValue <<= (sal_Int16)_rFont.Family;
                break;

            case PROPERTY_ID_FONT_CHARSET:
                aValue <<= (sal_Int16)_rFont.CharSet;
                break;

            case PROPERTY_ID_FONT_CHARWIDTH:
                aValue <<= _rFont.CharacterWidth;
                break;

            case PROPERTY_ID_FONT_KERNING:
                aValue <<= _rFont.Kerning;
                break;

            case PROPERTY_ID_FONT_ORIENTATION:
                aValue <<= _rFont.Orientation;
                break;

            case PROPERTY_ID_FONT_PITCH:
                aValue <<= _rFont.Pitch;
                break;

            case PROPERTY_ID_FONT_TYPE:
                aValue <<= _rFont.Type;
                break;

            case PROPERTY_ID_FONT_WIDTH:
                aValue <<= _rFont.Width;
                break;

            case PROPERTY_ID_FONT_HEIGHT:
                aValue <<= (float)( _rFont.Height );
                break;

            case PROPERTY_ID_FONT_WEIGHT:
                aValue <<= (float)_rFont.Weight;
                break;

            case PROPERTY_ID_FONT_SLANT:
                aValue = makeAny(_rFont.Slant);
                break;

            case PROPERTY_ID_FONT_UNDERLINE:
                aValue <<= (sal_Int16)_rFont.Underline;
                break;

            case PROPERTY_ID_FONT_STRIKEOUT:
                aValue <<= (sal_Int16)_rFont.Strikeout;
                break;

            case PROPERTY_ID_FONT_WORDLINEMODE:
                aValue = makeAny( (sal_Bool)_rFont.WordLineMode );
                break;

            default:
                OSL_FAIL( "lcl_extractFontDescriptorAggregate: invalid handle!" );
                break;
            }
            return aValue;
        }
    }

    //=====================================================================
    //= FontControlModel
    //=====================================================================
    //---------------------------------------------------------------------
    FontControlModel::FontControlModel( bool _bToolkitCompatibleDefaults )
        :m_nFontRelief( FontRelief::NONE )
        ,m_nFontEmphasis( FontEmphasisMark::NONE )
        ,m_bToolkitCompatibleDefaults( _bToolkitCompatibleDefaults )
    {
    }

    //---------------------------------------------------------------------
    FontControlModel::FontControlModel( const FontControlModel* _pOriginal )
    {
        m_aFont = _pOriginal->m_aFont;
        m_nFontRelief = _pOriginal->m_nFontRelief;
        m_nFontEmphasis = _pOriginal->m_nFontEmphasis;
        m_aTextLineColor = _pOriginal->m_aTextLineColor;
        m_aTextColor = _pOriginal->m_aTextColor;
        m_bToolkitCompatibleDefaults = _pOriginal->m_bToolkitCompatibleDefaults;
    }

    //---------------------------------------------------------------------
    bool FontControlModel::isFontRelatedProperty( sal_Int32 _nPropertyHandle ) const
    {
        return isFontAggregateProperty( _nPropertyHandle )
            || ( _nPropertyHandle == PROPERTY_ID_FONT )
            || ( _nPropertyHandle == PROPERTY_ID_FONTEMPHASISMARK )
            || ( _nPropertyHandle == PROPERTY_ID_FONTRELIEF )
            || ( _nPropertyHandle == PROPERTY_ID_TEXTLINECOLOR )
            || ( _nPropertyHandle == PROPERTY_ID_TEXTCOLOR );
    }

    //---------------------------------------------------------------------
    bool FontControlModel::isFontAggregateProperty( sal_Int32 _nPropertyHandle ) const
    {
        return ( _nPropertyHandle == PROPERTY_ID_FONT_CHARWIDTH )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_ORIENTATION )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_WIDTH )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_NAME )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_STYLENAME )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_FAMILY )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_CHARSET )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_HEIGHT )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_WEIGHT )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_SLANT )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_UNDERLINE )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_STRIKEOUT )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_WORDLINEMODE )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_PITCH )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_KERNING )
            || ( _nPropertyHandle == PROPERTY_ID_FONT_TYPE );
    }

    //---------------------------------------------------------------------
    sal_Int32 FontControlModel::getTextColor( ) const
    {
        sal_Int32 nColor = COL_TRANSPARENT;
        m_aTextColor >>= nColor;
        return nColor;
    }

    //---------------------------------------------------------------------
    sal_Int32 FontControlModel::getTextLineColor( ) const
    {
        sal_Int32 nColor = COL_TRANSPARENT;
        m_aTextLineColor >>= nColor;
        return nColor;
    }

    //------------------------------------------------------------------------------
    void FontControlModel::describeFontRelatedProperties( Sequence< Property >& /* [out] */ _rProps ) const
    {
        sal_Int32 nPos = _rProps.getLength();
        _rProps.realloc( nPos + 21 );
        Property* pProperties = _rProps.getArray();

        DECL_PROP2      ( FONT,               FontDescriptor,   BOUND, MAYBEDEFAULT );
        DECL_PROP2      ( FONTEMPHASISMARK,   sal_Int16,        BOUND, MAYBEDEFAULT );
        DECL_PROP2      ( FONTRELIEF,         sal_Int16,        BOUND, MAYBEDEFAULT );
        DECL_PROP3      ( TEXTCOLOR,          sal_Int32,        BOUND, MAYBEDEFAULT, MAYBEVOID );
        DECL_PROP3      ( TEXTLINECOLOR,      sal_Int32,        BOUND, MAYBEDEFAULT, MAYBEVOID );

        DECL_PROP1      ( FONT_CHARWIDTH,     float,            MAYBEDEFAULT );
        DECL_BOOL_PROP1 ( FONT_KERNING,                         MAYBEDEFAULT );
        DECL_PROP1      ( FONT_ORIENTATION,   float,            MAYBEDEFAULT );
        DECL_PROP1      ( FONT_PITCH,         sal_Int16,        MAYBEDEFAULT );
        DECL_PROP1      ( FONT_TYPE,          sal_Int16,        MAYBEDEFAULT );
        DECL_PROP1      ( FONT_WIDTH,         sal_Int16,        MAYBEDEFAULT );
        DECL_PROP1      ( FONT_NAME,          ::rtl::OUString,  MAYBEDEFAULT );
        DECL_PROP1      ( FONT_STYLENAME,     ::rtl::OUString,  MAYBEDEFAULT );
        DECL_PROP1      ( FONT_FAMILY,        sal_Int16,        MAYBEDEFAULT );
        DECL_PROP1      ( FONT_CHARSET,       sal_Int16,        MAYBEDEFAULT );
        DECL_PROP1      ( FONT_HEIGHT,        float,            MAYBEDEFAULT );
        DECL_PROP1      ( FONT_WEIGHT,        float,            MAYBEDEFAULT );
        DECL_PROP1      ( FONT_SLANT,         sal_Int16,        MAYBEDEFAULT );
        DECL_PROP1      ( FONT_UNDERLINE,     sal_Int16,        MAYBEDEFAULT );
        DECL_PROP1      ( FONT_STRIKEOUT,     sal_Int16,        MAYBEDEFAULT );
        DECL_BOOL_PROP1 ( FONT_WORDLINEMODE,                    MAYBEDEFAULT );
    }

    //---------------------------------------------------------------------
    void FontControlModel::getFastPropertyValue( Any& _rValue, sal_Int32 _nHandle ) const
    {
        switch( _nHandle )
        {
        case PROPERTY_ID_TEXTCOLOR:
            _rValue = m_aTextColor;
            break;

        case PROPERTY_ID_FONTEMPHASISMARK:
            _rValue <<= m_nFontEmphasis;
            break;

        case PROPERTY_ID_FONTRELIEF:
            _rValue <<= m_nFontRelief;
            break;

        case PROPERTY_ID_TEXTLINECOLOR:
            _rValue = m_aTextLineColor;
            break;

        case PROPERTY_ID_FONT:
            _rValue = makeAny( m_aFont );
            break;

        default:
            _rValue = lcl_extractFontDescriptorAggregate( _nHandle, m_aFont );
            break;
        }
    }

    //---------------------------------------------------------------------
    sal_Bool FontControlModel::convertFastPropertyValue( Any& _rConvertedValue, Any& _rOldValue,
                sal_Int32 _nHandle, const Any& _rValue ) throw( IllegalArgumentException )
    {
        sal_Bool bModified = sal_False;
        switch( _nHandle )
        {
        case PROPERTY_ID_TEXTCOLOR:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, m_aTextColor, ::getCppuType( static_cast< const sal_Int32* >( NULL ) ) );
            break;

        case PROPERTY_ID_TEXTLINECOLOR:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, m_aTextLineColor, ::getCppuType( static_cast< sal_Int32* >( NULL ) ) );
            break;

        case PROPERTY_ID_FONTEMPHASISMARK:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, m_nFontEmphasis );
            break;

        case PROPERTY_ID_FONTRELIEF:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, m_nFontRelief );
            break;

        case PROPERTY_ID_FONT:
        {
            Any aWorkAroundGccLimitation = makeAny( m_aFont );
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, aWorkAroundGccLimitation, ::getCppuType( &m_aFont ) );
        }
        break;

        case PROPERTY_ID_FONT_NAME:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, m_aFont.Name );
            break;

        case PROPERTY_ID_FONT_STYLENAME:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, m_aFont.StyleName );
            break;

        case PROPERTY_ID_FONT_FAMILY:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, (sal_Int16)m_aFont.Family );
            break;

        case PROPERTY_ID_FONT_CHARSET:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, (sal_Int16)m_aFont.CharSet );
            break;

        case PROPERTY_ID_FONT_CHARWIDTH:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, float( m_aFont.CharacterWidth ) );
            break;

        case PROPERTY_ID_FONT_KERNING:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, (sal_Int16)m_aFont.Kerning );
            break;

        case PROPERTY_ID_FONT_ORIENTATION:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, float( m_aFont.Orientation ) );
            break;

        case PROPERTY_ID_FONT_PITCH:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, (sal_Int16)m_aFont.Pitch );
            break;

        case PROPERTY_ID_FONT_TYPE:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, (sal_Int16)m_aFont.Type );
            break;

        case PROPERTY_ID_FONT_WIDTH:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, (sal_Int16)m_aFont.Width );
            break;

        case PROPERTY_ID_FONT_HEIGHT:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, float( m_aFont.Height ) );
            break;

        case PROPERTY_ID_FONT_WEIGHT:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, m_aFont.Weight );
            break;

        case PROPERTY_ID_FONT_SLANT:
            bModified = tryPropertyValueEnum( _rConvertedValue, _rOldValue, _rValue, m_aFont.Slant );
            break;

        case PROPERTY_ID_FONT_UNDERLINE:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, (sal_Int16)m_aFont.Underline );
            break;

        case PROPERTY_ID_FONT_STRIKEOUT:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, (sal_Int16)m_aFont.Strikeout );
            break;

        case PROPERTY_ID_FONT_WORDLINEMODE:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, (sal_Bool)m_aFont.WordLineMode );
            break;

        default:
            OSL_FAIL( "FontControlModel::convertFastPropertyValue: no font aggregate!" );
        }
        return bModified;
    }

    //------------------------------------------------------------------------------
    void FontControlModel::setFastPropertyValue_NoBroadcast( sal_Int32 _nHandle, const Any& _rValue ) throw ( Exception )
    {
        switch( _nHandle )
        {
        case PROPERTY_ID_TEXTCOLOR:
            m_aTextColor = _rValue;
            break;

        case PROPERTY_ID_TEXTLINECOLOR:
            m_aTextLineColor = _rValue;
            break;

        case PROPERTY_ID_FONTEMPHASISMARK:
            _rValue >>= m_nFontEmphasis;
            break;

        case PROPERTY_ID_FONTRELIEF:
            _rValue >>= m_nFontRelief;
            break;

        case PROPERTY_ID_FONT:
            _rValue >>= m_aFont;
            break;

        case PROPERTY_ID_FONT_NAME:
            _rValue >>= m_aFont.Name;
            break;

        case PROPERTY_ID_FONT_STYLENAME:
            _rValue >>= m_aFont.StyleName;
            break;

        case PROPERTY_ID_FONT_FAMILY:
            _rValue >>= m_aFont.Family;
            break;

        case PROPERTY_ID_FONT_CHARSET:
            _rValue >>= m_aFont.CharSet;
            break;

        case PROPERTY_ID_FONT_CHARWIDTH:
            _rValue >>= m_aFont.CharacterWidth;
            break;

        case PROPERTY_ID_FONT_KERNING:
            _rValue >>= m_aFont.Kerning;
            break;

        case PROPERTY_ID_FONT_ORIENTATION:
            _rValue >>= m_aFont.Orientation;
            break;

        case PROPERTY_ID_FONT_PITCH:
            _rValue >>= m_aFont.Pitch;
            break;

        case PROPERTY_ID_FONT_TYPE:
            _rValue >>= m_aFont.Type;
            break;

        case PROPERTY_ID_FONT_WIDTH:
            _rValue >>= m_aFont.Width;
            break;

        case PROPERTY_ID_FONT_HEIGHT:
        {
            float nHeight = 0;
            _rValue >>= nHeight;
            m_aFont.Height = (sal_Int16)nHeight;
        }
        break;

        case PROPERTY_ID_FONT_WEIGHT:
            _rValue >>= m_aFont.Weight;
            break;

        case PROPERTY_ID_FONT_SLANT:
            _rValue >>= m_aFont.Slant;
            break;

        case PROPERTY_ID_FONT_UNDERLINE:
            _rValue >>= m_aFont.Underline;
            break;

        case PROPERTY_ID_FONT_STRIKEOUT:
            _rValue >>= m_aFont.Strikeout;
            break;

        case PROPERTY_ID_FONT_WORDLINEMODE:
        {
            sal_Bool bWordLineMode = sal_False;
            _rValue >>= bWordLineMode;
            m_aFont.WordLineMode = bWordLineMode;
        }
        break;

        default:
            OSL_FAIL( "FontControlModel::setFastPropertyValue_NoBroadcast: invalid property!" );
        }
    }

    //------------------------------------------------------------------------------
    Any FontControlModel::getPropertyDefaultByHandle( sal_Int32 _nHandle ) const
    {
        Any aReturn;
        // some defaults which are the same, not matter if we have toolkit-compatible
        // defaults or not
        bool bHandled = false;
        switch( _nHandle )
        {
        case PROPERTY_ID_TEXTCOLOR:
        case PROPERTY_ID_TEXTLINECOLOR:
            // void
            bHandled = true;
            break;

        case PROPERTY_ID_FONTEMPHASISMARK:
            aReturn <<= FontEmphasisMark::NONE;
            bHandled = true;
            break;

        case PROPERTY_ID_FONTRELIEF:
            aReturn <<= FontRelief::NONE;
            bHandled = true;
            break;
        }
        if ( bHandled )
            return aReturn;

        if ( m_bToolkitCompatibleDefaults )
        {
            EmptyFontDescriptor aEmpty;
            if ( PROPERTY_ID_FONT == _nHandle )
                return makeAny( (FontDescriptor)aEmpty );
            return lcl_extractFontDescriptorAggregate( _nHandle, aEmpty );
        }

        switch( _nHandle )
        {
        case PROPERTY_ID_FONT:
            aReturn <<= ::comphelper::getDefaultFont();
            break;

        case PROPERTY_ID_FONT_WORDLINEMODE:
            aReturn = makeBoolAny(sal_False);

        case PROPERTY_ID_FONT_NAME:
        case PROPERTY_ID_FONT_STYLENAME:
            aReturn <<= ::rtl::OUString();

        case PROPERTY_ID_FONT_FAMILY:
        case PROPERTY_ID_FONT_CHARSET:
        case PROPERTY_ID_FONT_SLANT:
        case PROPERTY_ID_FONT_UNDERLINE:
        case PROPERTY_ID_FONT_STRIKEOUT:
            aReturn <<= (sal_Int16)1;
            break;

        case PROPERTY_ID_FONT_KERNING:
            aReturn = makeBoolAny(sal_False);
            break;

        case PROPERTY_ID_FONT_PITCH:
        case PROPERTY_ID_FONT_TYPE:
        case PROPERTY_ID_FONT_WIDTH:
            aReturn <<= (sal_Int16)0;
            break;

        case PROPERTY_ID_FONT_HEIGHT:
        case PROPERTY_ID_FONT_WEIGHT:
        case PROPERTY_ID_FONT_CHARWIDTH:
        case PROPERTY_ID_FONT_ORIENTATION:
            aReturn <<= (float)0;
            break;

        default:
            OSL_FAIL( "FontControlModel::getPropertyDefaultByHandle: invalid property!" );
        }

        return aReturn;
    }

//.........................................................................
}   // namespace frm
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
