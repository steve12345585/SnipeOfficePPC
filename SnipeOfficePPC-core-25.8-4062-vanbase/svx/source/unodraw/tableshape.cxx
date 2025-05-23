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


#include "shapeimpl.hxx"
#include "svx/unoshprp.hxx"
#include "svx/svdotable.hxx"
#include <svx/svdpool.hxx>

///////////////////////////////////////////////////////////////////////

using ::rtl::OUString;

using namespace ::osl;
using namespace ::cppu;
using namespace ::sdr::table;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::beans;

SvxTableShape::SvxTableShape( SdrObject* pObj ) throw()
:   SvxShape( pObj, getSvxMapProvider().GetMap(SVXMAP_TABLE), getSvxMapProvider().GetPropertySet(SVXMAP_TABLE, SdrObject::GetGlobalDrawObjectItemPool()) )
{
    SetShapeType( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.TableShape" ) ) );
}

//----------------------------------------------------------------------
SvxTableShape::~SvxTableShape() throw()
{
}

//----------------------------------------------------------------------

bool SvxTableShape::setPropertyValueImpl(
    const ::rtl::OUString& rName,
    const SfxItemPropertySimpleEntry* pProperty,
    const ::com::sun::star::uno::Any& rValue )
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    switch( pProperty->nWID )
    {
    case OWN_ATTR_TABLETEMPLATE:
    {
        Reference< XIndexAccess > xTemplate;

        if( !(rValue >>= xTemplate) )
            throw IllegalArgumentException();

        if( mpObj.is() )
            static_cast< sdr::table::SdrTableObj* >( mpObj.get() )->setTableStyle(xTemplate);

        return true;
    }
    case OWN_ATTR_TABLETEMPLATE_FIRSTROW:
    case OWN_ATTR_TABLETEMPLATE_LASTROW:
    case OWN_ATTR_TABLETEMPLATE_FIRSTCOLUMN:
    case OWN_ATTR_TABLETEMPLATE_LASTCOLUMN:
    case OWN_ATTR_TABLETEMPLATE_BANDINGROWS:
    case OWN_ATTR_TABLETEMPLATE_BANDINGCOULUMNS:
    {
        if( mpObj.is() )
        {
            TableStyleSettings aSettings( static_cast< sdr::table::SdrTableObj* >( mpObj.get() )->getTableStyleSettings() );

            switch( pProperty->nWID )
            {
            case OWN_ATTR_TABLETEMPLATE_FIRSTROW:           rValue >>= aSettings.mbUseFirstRow; break;
            case OWN_ATTR_TABLETEMPLATE_LASTROW:            rValue >>= aSettings.mbUseLastRow; break;
            case OWN_ATTR_TABLETEMPLATE_FIRSTCOLUMN:        rValue >>= aSettings.mbUseFirstColumn; break;
            case OWN_ATTR_TABLETEMPLATE_LASTCOLUMN:         rValue >>= aSettings.mbUseLastColumn; break;
            case OWN_ATTR_TABLETEMPLATE_BANDINGROWS:        rValue >>= aSettings.mbUseRowBanding; break;
            case OWN_ATTR_TABLETEMPLATE_BANDINGCOULUMNS:    rValue >>= aSettings.mbUseColumnBanding; break;
            }

            static_cast< sdr::table::SdrTableObj* >( mpObj.get() )->setTableStyleSettings(aSettings);
        }

        return true;
    }
    default:
    {
        return SvxShape::setPropertyValueImpl( rName, pProperty, rValue );
    }
    }
}

extern Graphic SvxGetGraphicForShape( SdrObject& rShape, bool bVector );

bool SvxTableShape::getPropertyValueImpl(
    const ::rtl::OUString& rName,
    const SfxItemPropertySimpleEntry* pProperty,
    ::com::sun::star::uno::Any& rValue )
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    switch( pProperty->nWID )
    {
    case OWN_ATTR_OLEMODEL:
    {
        if( mpObj.is() )
        {
            rValue <<= static_cast< sdr::table::SdrTableObj* >( mpObj.get() )->getTable();
        }
        return true;
    }
    case OWN_ATTR_TABLETEMPLATE:
    {
        if( mpObj.is() )
        {
            rValue <<= static_cast< sdr::table::SdrTableObj* >( mpObj.get() )->getTableStyle();
        }
        return true;
    }
    case OWN_ATTR_BITMAP:
    {
        if( mpObj.is() )
        {
            Graphic aGraphic( SvxGetGraphicForShape( *mpObj.get(), true ) );
            rValue <<= aGraphic.GetXGraphic();
        }
        return true;
    }
    case OWN_ATTR_TABLETEMPLATE_FIRSTROW:
    case OWN_ATTR_TABLETEMPLATE_LASTROW:
    case OWN_ATTR_TABLETEMPLATE_FIRSTCOLUMN:
    case OWN_ATTR_TABLETEMPLATE_LASTCOLUMN:
    case OWN_ATTR_TABLETEMPLATE_BANDINGROWS:
    case OWN_ATTR_TABLETEMPLATE_BANDINGCOULUMNS:
    {
        if( mpObj.is() )
        {
            TableStyleSettings aSettings( static_cast< sdr::table::SdrTableObj* >( mpObj.get() )->getTableStyleSettings() );

            switch( pProperty->nWID )
            {
            case OWN_ATTR_TABLETEMPLATE_FIRSTROW:           rValue <<= aSettings.mbUseFirstRow; break;
            case OWN_ATTR_TABLETEMPLATE_LASTROW:            rValue <<= aSettings.mbUseLastRow; break;
            case OWN_ATTR_TABLETEMPLATE_FIRSTCOLUMN:        rValue <<= aSettings.mbUseFirstColumn; break;
            case OWN_ATTR_TABLETEMPLATE_LASTCOLUMN:         rValue <<= aSettings.mbUseLastColumn; break;
            case OWN_ATTR_TABLETEMPLATE_BANDINGROWS:        rValue <<= aSettings.mbUseRowBanding; break;
            case OWN_ATTR_TABLETEMPLATE_BANDINGCOULUMNS:    rValue <<= aSettings.mbUseColumnBanding; break;
            }
        }

        return true;
    }
    default:
    {
        return SvxShape::getPropertyValueImpl( rName, pProperty, rValue );
    }
    }
}

void SvxTableShape::lock()
{
    SvxShape::lock();
    if( mpObj.is() )
        static_cast< sdr::table::SdrTableObj* >( mpObj.get() )->uno_lock();
}

void SvxTableShape::unlock()
{
    if( mpObj.is() )
        static_cast< sdr::table::SdrTableObj* >( mpObj.get() )->uno_unlock();
    SvxShape::unlock();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
