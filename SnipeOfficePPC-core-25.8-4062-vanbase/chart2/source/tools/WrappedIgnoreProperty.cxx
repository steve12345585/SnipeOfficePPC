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


#include "WrappedIgnoreProperty.hxx"
#include "macros.hxx"
#include <com/sun/star/awt/Gradient.hpp>
#include <com/sun/star/drawing/Hatch.hpp>
#include <com/sun/star/drawing/BitmapMode.hpp>
#include <com/sun/star/drawing/FillStyle.hpp>
#include <com/sun/star/drawing/LineDash.hpp>
#include <com/sun/star/drawing/LineJoint.hpp>
#include <com/sun/star/drawing/LineStyle.hpp>
#include <com/sun/star/drawing/RectanglePoint.hpp>

using namespace ::com::sun::star;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Reference;
using ::rtl::OUString;

//.............................................................................
namespace chart
{
//.............................................................................


WrappedIgnoreProperty::WrappedIgnoreProperty( const OUString& rOuterName, const Any& rDefaultValue )
                         : WrappedProperty( rOuterName, OUString() )
                         , m_aDefaultValue( rDefaultValue )
                         , m_aCurrentValue( rDefaultValue )
{
}
WrappedIgnoreProperty::~WrappedIgnoreProperty()
{
}

void WrappedIgnoreProperty::setPropertyValue( const Any& rOuterValue, const Reference< beans::XPropertySet >& /* xInnerPropertySet */ ) const
                throw (beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    m_aCurrentValue = rOuterValue;
}

Any WrappedIgnoreProperty::getPropertyValue( const Reference< beans::XPropertySet >& /* xInnerPropertySet */ ) const
                        throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    return m_aCurrentValue;
}

void WrappedIgnoreProperty::setPropertyToDefault( const Reference< beans::XPropertyState >& /* xInnerPropertyState */ ) const
                        throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException)
{
    m_aCurrentValue = m_aDefaultValue;
}

Any WrappedIgnoreProperty::getPropertyDefault( const Reference< beans::XPropertyState >& /* xInnerPropertyState */ ) const
                        throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    return m_aDefaultValue;
}

beans::PropertyState WrappedIgnoreProperty::getPropertyState( const Reference< beans::XPropertyState >& /* xInnerPropertyState */ ) const
                        throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    return ( m_aCurrentValue == m_aDefaultValue
             ? beans::PropertyState_DEFAULT_VALUE
             : beans::PropertyState_DIRECT_VALUE );
}

void WrappedIgnoreProperties::addIgnoreLineProperties( std::vector< WrappedProperty* >& rList )
{
    rList.push_back( new WrappedIgnoreProperty( C2U( "LineStyle" ), uno::makeAny( drawing::LineStyle_SOLID ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "LineDashName" ), uno::makeAny( rtl::OUString() ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "LineColor" ), uno::makeAny( sal_Int32(0) ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "LineTransparence" ), uno::makeAny( sal_Int16(0) ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "LineWidth" ), uno::makeAny( sal_Int32(0) ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "LineJoint" ), uno::makeAny( drawing::LineJoint_ROUND ) ) );
}

void WrappedIgnoreProperties::addIgnoreFillProperties( std::vector< WrappedProperty* >& rList )
{
    addIgnoreFillProperties_without_BitmapProperties( rList );
    addIgnoreFillProperties_only_BitmapProperties( rList );
}

void WrappedIgnoreProperties::addIgnoreFillProperties_without_BitmapProperties( ::std::vector< WrappedProperty* >& rList )
{
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillStyle" ), uno::makeAny( drawing::FillStyle_SOLID ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillColor" ), uno::makeAny( sal_Int32(-1) ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillTransparence" ), uno::makeAny( sal_Int16(0) ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillTransparenceGradientName" ), uno::makeAny( ::rtl::OUString() ) ) );
//    rList.push_back( new WrappedIgnoreProperty( C2U( "FillTransparenceGradient" ), uno::makeAny( awt::Gradient() ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillGradientName" ), uno::makeAny( ::rtl::OUString() ) ) );
//    rList.push_back( new WrappedIgnoreProperty( C2U( "FillGradient" ), uno::makeAny( awt::Gradient() ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillHatchName" ), uno::makeAny( ::rtl::OUString() ) ) );
//    rList.push_back( new WrappedIgnoreProperty( C2U( "FillHatch" ), uno::makeAny( drawing::Hatch() ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBackground" ), uno::makeAny( sal_Bool(sal_False) ) ) );
}

void WrappedIgnoreProperties::addIgnoreFillProperties_only_BitmapProperties( ::std::vector< WrappedProperty* >& rList )
{
//     rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapName" ), uno::makeAny( ::rtl::OUString() ) ) );
//    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmap" ), uno::makeAny( uno::Reference< awt::XBitmap > (0) ) ) );
//    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapURL" ), uno::makeAny( ::rtl::OUString() ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapOffsetX" ), uno::makeAny( sal_Int16(0) ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapOffsetY" ), uno::makeAny( sal_Int16(0) ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapPositionOffsetX" ), uno::makeAny( sal_Int16(0) ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapPositionOffsetY" ), uno::makeAny( sal_Int16(0) ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapRectanglePoint" ), uno::makeAny( drawing::RectanglePoint_LEFT_TOP ) ) );
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapLogicalSize" ), uno::makeAny( sal_Bool(sal_False) ) ) );//todo correct default?
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapSizeX" ), uno::makeAny( sal_Int32(10) ) ) );//todo which default?
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapSizeY" ), uno::makeAny( sal_Int32(10) ) ) );//todo which default?
    rList.push_back( new WrappedIgnoreProperty( C2U( "FillBitmapMode" ), uno::makeAny( drawing::BitmapMode_REPEAT ) ) );
}

//.............................................................................
} //namespace chart
//.............................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
