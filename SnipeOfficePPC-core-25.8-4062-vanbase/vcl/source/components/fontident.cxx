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


#include "vcl/svapp.hxx"
#include "vcl/font.hxx"

#include "svdata.hxx"

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XMaterialHolder.hpp>
#include <com/sun/star/awt/FontDescriptor.hpp>
#include <com/sun/star/awt/FontFamily.hpp>
#include <com/sun/star/awt/FontPitch.hpp>
#include <com/sun/star/awt/FontWeight.hpp>
#include <com/sun/star/awt/FontSlant.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/DisposedException.hpp>

#include <cppuhelper/implbase3.hxx>

using ::rtl::OUString;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::awt;

// -----------------------------------------------------------------------

namespace vcl
{

class FontIdentificator : public ::cppu::WeakAggImplHelper3< XMaterialHolder, XInitialization, XServiceInfo >
{
    Font        m_aFont;
public:
FontIdentificator() {}
    virtual ~FontIdentificator();


    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName(  ) throw (RuntimeException);
    virtual ::sal_Bool SAL_CALL supportsService( const OUString& ) throw (RuntimeException);
    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames(  ) throw (RuntimeException);

    // XInitialization
    virtual void SAL_CALL initialize( const Sequence< Any >& ) throw (Exception, RuntimeException);

    // XMaterialHolder
    virtual Any SAL_CALL getMaterial() throw(RuntimeException);

};

// --------------------------------------------------------------------

FontIdentificator::~FontIdentificator()
{
}

void SAL_CALL FontIdentificator::initialize( const Sequence<Any>& i_rArgs ) throw(Exception,RuntimeException)
{
    if( !ImplGetSVData() )
        return; // VCL not initialized

    sal_uInt32 nArgs = i_rArgs.getLength();
    const Any* pArgs = i_rArgs.getConstArray();
    Sequence< sal_Int8 > aFontBuf;
    for( sal_uInt32 i = 0; i < nArgs; i++ )
    {
        if( pArgs[i] >>= aFontBuf )
        {
            m_aFont = Font::identifyFont( aFontBuf.getConstArray(), aFontBuf.getLength() );
            break;
        }
    }
}

Any SAL_CALL FontIdentificator::getMaterial() throw(RuntimeException)
{
    if( !ImplGetSVData() )
        return Any(); // VCL not initialized

    FontDescriptor aFD;
    aFD.Name                = m_aFont.GetName();
    aFD.Height              = 0;
    aFD.Width               = 0;
    aFD.StyleName           = m_aFont.GetStyleName();
    aFD.CharSet             = 0;
    aFD.CharacterWidth      = 0;
    aFD.Underline           = 0;
    aFD.Strikeout           = 0;
    aFD.Orientation         = 0;
    aFD.Kerning             = sal_False;
    aFD.WordLineMode        = sal_False;
    aFD.Type                = 0;
    switch( m_aFont.GetFamily() )
    {
    case FAMILY_DECORATIVE: aFD.Family = com::sun::star::awt::FontFamily::DECORATIVE;break;
    case FAMILY_MODERN: aFD.Family = com::sun::star::awt::FontFamily::MODERN;break;
    case FAMILY_ROMAN: aFD.Family = com::sun::star::awt::FontFamily::ROMAN;break;
    case FAMILY_SCRIPT: aFD.Family = com::sun::star::awt::FontFamily::SCRIPT;break;
    case FAMILY_SWISS: aFD.Family = com::sun::star::awt::FontFamily::SWISS;break;
    case FAMILY_SYSTEM: aFD.Family = com::sun::star::awt::FontFamily::SYSTEM;break;
    default:
        aFD.Family = com::sun::star::awt::FontFamily::DONTKNOW;
        break;
    }
    switch( m_aFont.GetPitch() )
    {
    case PITCH_VARIABLE: aFD.Pitch = com::sun::star::awt::FontPitch::VARIABLE;break;
    case PITCH_FIXED: aFD.Pitch = com::sun::star::awt::FontPitch::FIXED;break;
    default:
        aFD.Pitch = com::sun::star::awt::FontPitch::DONTKNOW;
        break;
    }
    switch( m_aFont.GetWeight() )
    {
    case WEIGHT_THIN: aFD.Weight = com::sun::star::awt::FontWeight::THIN;break;
    case WEIGHT_ULTRALIGHT: aFD.Weight = com::sun::star::awt::FontWeight::ULTRALIGHT;break;
    case WEIGHT_LIGHT: aFD.Weight = com::sun::star::awt::FontWeight::LIGHT;break;
    case WEIGHT_SEMILIGHT: aFD.Weight = com::sun::star::awt::FontWeight::SEMILIGHT;break;
    case WEIGHT_MEDIUM:
    case WEIGHT_NORMAL: aFD.Weight = com::sun::star::awt::FontWeight::NORMAL;break;
    case WEIGHT_SEMIBOLD: aFD.Weight = com::sun::star::awt::FontWeight::SEMIBOLD;break;
    case WEIGHT_BOLD: aFD.Weight = com::sun::star::awt::FontWeight::BOLD;break;
    case WEIGHT_ULTRABOLD: aFD.Weight = com::sun::star::awt::FontWeight::ULTRABOLD;break;
    case WEIGHT_BLACK: aFD.Weight = com::sun::star::awt::FontWeight::BLACK;break;
    default:
        aFD.Weight = com::sun::star::awt::FontWeight::DONTKNOW;
        break;
    }
    switch( m_aFont.GetItalic() )
    {
    case ITALIC_OBLIQUE: aFD.Slant = com::sun::star::awt::FontSlant_OBLIQUE;break;
    case ITALIC_NORMAL: aFD.Slant = com::sun::star::awt::FontSlant_ITALIC;break;
    default:
        aFD.Slant = com::sun::star::awt::FontSlant_DONTKNOW;
        break;
    }
    return makeAny( aFD );
}

Sequence< OUString > FontIdentificator_getSupportedServiceNames()
{
    static OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.FontIdentificator" ) );
    static Sequence< OUString > aServiceNames( &aServiceName, 1 );
    return aServiceNames;
}

OUString FontIdentificator_getImplementationName()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( "vcl::FontIdentificator" ) );
}

Reference< XInterface > SAL_CALL FontIdentificator_createInstance( const Reference< XMultiServiceFactory >&  )
{
    return static_cast< ::cppu::OWeakObject * >( new FontIdentificator );
}


// XServiceInfo
OUString SAL_CALL FontIdentificator::getImplementationName() throw (RuntimeException)
{
    return FontIdentificator_getImplementationName();
}

sal_Bool SAL_CALL FontIdentificator::supportsService( const OUString& i_rServiceName ) throw (RuntimeException)
{
    Sequence< OUString > aSN( FontIdentificator_getSupportedServiceNames() );
    for( sal_Int32 nService = 0; nService < aSN.getLength(); nService++ )
    {
        if( aSN[nService] == i_rServiceName )
            return sal_True;
    }
    return sal_False;
}

Sequence< OUString > SAL_CALL FontIdentificator::getSupportedServiceNames() throw (RuntimeException)
{
    return FontIdentificator_getSupportedServiceNames();
}

} // namespace vcl

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
