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


#include "atkwrapper.hxx"

#include <com/sun/star/accessibility/XAccessibleValue.hpp>

#include <stdio.h>
#include <string.h>

using namespace ::com::sun::star;

static accessibility::XAccessibleValue*
    getValue( AtkValue *pValue ) throw (uno::RuntimeException)
{
    AtkObjectWrapper *pWrap = ATK_OBJECT_WRAPPER( pValue );
    if( pWrap )
    {
        if( !pWrap->mpValue && pWrap->mpContext )
        {
            uno::Any any = pWrap->mpContext->queryInterface( accessibility::XAccessibleValue::static_type(NULL) );
            pWrap->mpValue = reinterpret_cast< accessibility::XAccessibleValue * > (any.pReserved);
            pWrap->mpValue->acquire();
        }

        return pWrap->mpValue;
    }

    return NULL;
}

static void anyToGValue( uno::Any aAny, GValue *pValue )
{
    // FIXME: expand to lots of types etc.
    double aDouble=0;
    aAny >>= aDouble;

    memset( pValue,  0, sizeof( GValue ) );
    g_value_init( pValue, G_TYPE_DOUBLE );
    g_value_set_double( pValue, aDouble );
}

extern "C" {

static void
value_wrapper_get_current_value( AtkValue *value,
                                 GValue   *gval )
{
    try {
        accessibility::XAccessibleValue* pValue = getValue( value );
        if( pValue )
            anyToGValue( pValue->getCurrentValue(), gval );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getCurrentValue()" );
    }
}

static void
value_wrapper_get_maximum_value( AtkValue *value,
                                 GValue   *gval )
{
    try {
        accessibility::XAccessibleValue* pValue = getValue( value );
        if( pValue )
            anyToGValue( pValue->getMaximumValue(), gval );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getCurrentValue()" );
    }
}

static void
value_wrapper_get_minimum_value( AtkValue *value,
                                 GValue   *gval )
{
    try {
        accessibility::XAccessibleValue* pValue = getValue( value );
        if( pValue )
            anyToGValue( pValue->getMinimumValue(), gval );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getCurrentValue()" );
    }
}

static gboolean
value_wrapper_set_current_value( AtkValue     *value,
                                 const GValue *gval )
{
    try {
        accessibility::XAccessibleValue* pValue = getValue( value );
        if( pValue )
        {
            // FIXME - this needs expanding
            double aDouble = g_value_get_double( gval );
            uno::Any aAny;
            aAny <<= aDouble;
            return pValue->setCurrentValue( aAny );
        }
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getCurrentValue()" );
    }

    return FALSE;
}

} // extern "C"

void
valueIfaceInit (AtkValueIface *iface)
{
  g_return_if_fail (iface != NULL);

  iface->get_current_value = value_wrapper_get_current_value;
  iface->get_maximum_value = value_wrapper_get_maximum_value;
  iface->get_minimum_value = value_wrapper_get_minimum_value;
  iface->set_current_value = value_wrapper_set_current_value;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
