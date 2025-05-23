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

#include <com/sun/star/drawing/BitmapMode.hpp>
#include <osl/mutex.hxx>
#include <vcl/svapp.hxx>
#include <svl/itemset.hxx>
#include <svx/svdpool.hxx>
#include <comphelper/extract.hxx>
#include <svx/xflbstit.hxx>
#include <svx/xflbmtit.hxx>
#include <svx/svdobj.hxx>
#include <svx/unoprov.hxx>
#include <svx/unoshape.hxx>
#include <comphelper/serviceinfohelper.hxx>

#include "unopback.hxx"
#include "unohelp.hxx"
#include "drawdoc.hxx"
#include "unokywds.hxx"

using namespace ::rtl;
using namespace ::com::sun::star;

const SvxItemPropertySet* ImplGetPageBackgroundPropertySet()
{
    static const SfxItemPropertyMapEntry aPageBackgroundPropertyMap_Impl[] =
    {
        FILL_PROPERTIES
        {0,0,0,0,0,0}
    };

    static SvxItemPropertySet aPageBackgroundPropertySet_Impl( aPageBackgroundPropertyMap_Impl, SdrObject::GetGlobalDrawObjectItemPool() );
    return &aPageBackgroundPropertySet_Impl;
}

UNO3_GETIMPLEMENTATION_IMPL( SdUnoPageBackground );

SdUnoPageBackground::SdUnoPageBackground(
    SdDrawDocument* pDoc /* = NULL */,
    const SfxItemSet* pSet /* = NULL */) throw()
:   mpPropSet(ImplGetPageBackgroundPropertySet()),
    mpSet(NULL),
    mpDoc(pDoc)
{
    if( pDoc )
    {
        StartListening( *pDoc );
        mpSet = new SfxItemSet( pDoc->GetPool(), XATTR_FILL_FIRST, XATTR_FILL_LAST );

        if( pSet )
            mpSet->Put(*pSet);
    }
}

SdUnoPageBackground::~SdUnoPageBackground() throw()
{
    if( mpDoc )
        EndListening( *mpDoc );

    if( mpSet )
        delete mpSet;
}

void SdUnoPageBackground::Notify( SfxBroadcaster&, const SfxHint& rHint )
{
    const SdrHint* pSdrHint = PTR_CAST( SdrHint, &rHint );

    if( pSdrHint )
    {
        // delete item set if document is dying because then the pool
        // will also die
        if( pSdrHint->GetKind() == HINT_MODELCLEARED )
        {
            delete mpSet;
            mpSet = NULL;
            mpDoc = NULL;
        }
    }

}

void SdUnoPageBackground::fillItemSet( SdDrawDocument* pDoc, SfxItemSet& rSet ) throw()
{
    rSet.ClearItem();

    if( mpSet == NULL )
    {
        StartListening( *pDoc );
        mpDoc = pDoc;

        mpSet = new SfxItemSet( *rSet.GetPool(), XATTR_FILL_FIRST, XATTR_FILL_LAST );

        if( mpPropSet->AreThereOwnUsrAnys() )
        {
            uno::Any* pAny;
            PropertyEntryVector_t aProperties = mpPropSet->getPropertyMap()->getPropertyEntries();
            PropertyEntryVector_t::const_iterator aIt = aProperties.begin();

            while( aIt != aProperties.end() )
            {
                pAny = mpPropSet->GetUsrAnyForID( aIt->nWID );
                if( pAny )
                {
                    OUString aPropertyName( aIt->sName );
                    switch( aIt->nWID )
                    {
                        case XATTR_FILLFLOATTRANSPARENCE :
                        case XATTR_FILLGRADIENT :
                        {
                            if ( ( pAny->getValueType() == ::getCppuType((const ::com::sun::star::awt::Gradient*)0) )
                                && ( aIt->nMemberId == MID_FILLGRADIENT ) )
                            {
                                setPropertyValue( aPropertyName, *pAny );
                            }
                            else if ( ( pAny->getValueType() == ::getCppuType((const ::rtl::OUString*)0) ) &&
                                        ( aIt->nMemberId == MID_NAME ) )
                            {
                                setPropertyValue( aPropertyName, *pAny );
                            }
                        }
                        break;
                        case XATTR_FILLHATCH :
                        {
                            if ( ( pAny->getValueType() == ::getCppuType((const ::com::sun::star::drawing::Hatch*)0) )
                                && ( aIt->nMemberId == MID_FILLHATCH ) )
                            {
                                setPropertyValue( aPropertyName, *pAny );
                            }
                            else if ( ( pAny->getValueType() == ::getCppuType((const ::rtl::OUString*)0) ) &&
                                        ( aIt->nMemberId == MID_NAME ) )
                            {
                                setPropertyValue( aPropertyName, *pAny );
                            }
                        }
                        break;
                        case XATTR_FILLBITMAP :
                        {
                            if ( ( ( pAny->getValueType() == ::getCppuType((const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XBitmap >*)0) ) ||
                                    ( pAny->getValueType() == ::getCppuType((const ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic >*)0) ) ) &&
                                    ( aIt->nMemberId == MID_BITMAP ) )
                            {
                                setPropertyValue( aPropertyName, *pAny );
                            }
                            else if ( ( pAny->getValueType() == ::getCppuType((const ::rtl::OUString*)0) ) &&
                                        ( ( aIt->nMemberId == MID_NAME ) || ( aIt->nMemberId == MID_GRAFURL ) ) )
                            {
                                setPropertyValue( aPropertyName, *pAny );
                            }
                        }
                        break;

                        default:
                            setPropertyValue( aPropertyName, *pAny );
                    }
                }
                ++aIt;
            }
        }
    }

    rSet.Put( *mpSet );
}

// XServiceInfo
OUString SAL_CALL SdUnoPageBackground::getImplementationName()
    throw(uno::RuntimeException)
{
    return OUString::createFromAscii( sUNO_SdUnoPageBackground );
}

sal_Bool SAL_CALL SdUnoPageBackground::supportsService( const OUString& ServiceName )
    throw(uno::RuntimeException)
{
    return comphelper::ServiceInfoHelper::supportsService( ServiceName, getSupportedServiceNames() );
}

uno::Sequence< OUString > SAL_CALL SdUnoPageBackground::getSupportedServiceNames()
    throw(uno::RuntimeException)
{
    uno::Sequence< OUString > aNameSequence( 2 );
    OUString* pStrings = aNameSequence.getArray();

    *pStrings++ = OUString( sUNO_Service_PageBackground );
    *pStrings   = OUString( sUNO_Service_FillProperties );

    return aNameSequence;
}

// XPropertySet
uno::Reference< beans::XPropertySetInfo > SAL_CALL SdUnoPageBackground::getPropertySetInfo()
    throw(uno::RuntimeException)
{
    return mpPropSet->getPropertySetInfo();
}

void SAL_CALL SdUnoPageBackground::setPropertyValue( const OUString& aPropertyName, const uno::Any& aValue )
    throw(beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertySimpleEntry* pEntry = getPropertyMapEntry( aPropertyName );

    if( pEntry == NULL )
    {
        throw beans::UnknownPropertyException();
    }
    else
    {
        if( mpSet )
        {
            if( pEntry->nWID == OWN_ATTR_FILLBMP_MODE )
            {
                drawing::BitmapMode eMode;
                if( aValue >>= eMode )
                {
                    mpSet->Put( XFillBmpStretchItem( eMode == drawing::BitmapMode_STRETCH ) );
                    mpSet->Put( XFillBmpTileItem( eMode == drawing::BitmapMode_REPEAT ) );
                    return;
                }
                throw lang::IllegalArgumentException();
            }

            SfxItemPool& rPool = *mpSet->GetPool();
            SfxItemSet aSet( rPool, pEntry->nWID, pEntry->nWID);
            aSet.Put( *mpSet );

            if( !aSet.Count() )
                aSet.Put( rPool.GetDefaultItem( pEntry->nWID ) );

            if( pEntry->nMemberId == MID_NAME && ( pEntry->nWID == XATTR_FILLBITMAP || pEntry->nWID == XATTR_FILLGRADIENT || pEntry->nWID == XATTR_FILLHATCH || pEntry->nWID == XATTR_FILLFLOATTRANSPARENCE ) )
            {
                OUString aName;
                if(!(aValue >>= aName ))
                    throw lang::IllegalArgumentException();

                SvxShape::SetFillAttribute( pEntry->nWID, aName, aSet );
            }
            else
            {
                SvxItemPropertySet_setPropertyValue( *mpPropSet, pEntry, aValue, aSet );
            }

            mpSet->Put( aSet );
        }
        else
        {
            if(pEntry && pEntry->nWID)
                mpPropSet->setPropertyValue( pEntry, aValue );
        }
    }
}

uno::Any SAL_CALL SdUnoPageBackground::getPropertyValue( const OUString& PropertyName )
    throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    uno::Any aAny;
    const SfxItemPropertySimpleEntry* pEntry = getPropertyMapEntry(PropertyName);

    if( pEntry == NULL )
    {
        throw beans::UnknownPropertyException();
    }
    else
    {
        if( mpSet )
        {
            if( pEntry->nWID == OWN_ATTR_FILLBMP_MODE )
            {
                XFillBmpStretchItem* pStretchItem = (XFillBmpStretchItem*)mpSet->GetItem(XATTR_FILLBMP_STRETCH);
                XFillBmpTileItem* pTileItem = (XFillBmpTileItem*)mpSet->GetItem(XATTR_FILLBMP_TILE);

                if( pStretchItem && pTileItem )
                {
                    if( pTileItem->GetValue() )
                        aAny <<= drawing::BitmapMode_REPEAT;
                    else if( pStretchItem->GetValue() )
                        aAny <<= drawing::BitmapMode_STRETCH;
                    else
                        aAny <<= drawing::BitmapMode_NO_REPEAT;
                }
            }
            else
            {
                SfxItemPool& rPool = *mpSet->GetPool();
                SfxItemSet aSet( rPool, pEntry->nWID, pEntry->nWID);
                aSet.Put( *mpSet );

                if( !aSet.Count() )
                    aSet.Put( rPool.GetDefaultItem( pEntry->nWID ) );

                // Hole Wert aus ItemSet
                aAny = SvxItemPropertySet_getPropertyValue( *mpPropSet, pEntry, aSet );
            }
        }
        else
        {
            if(pEntry && pEntry->nWID)
                aAny = mpPropSet->getPropertyValue( pEntry );
        }
    }
    return aAny;
}

void SAL_CALL SdUnoPageBackground::addPropertyChangeListener( const OUString& , const uno::Reference< beans::XPropertyChangeListener >&  ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}
void SAL_CALL SdUnoPageBackground::removePropertyChangeListener( const OUString& , const uno::Reference< beans::XPropertyChangeListener >&  ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}
void SAL_CALL SdUnoPageBackground::addVetoableChangeListener( const OUString& , const uno::Reference< beans::XVetoableChangeListener >&  ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}
void SAL_CALL SdUnoPageBackground::removeVetoableChangeListener( const OUString& , const uno::Reference< beans::XVetoableChangeListener >&  ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}

// XPropertyState
beans::PropertyState SAL_CALL SdUnoPageBackground::getPropertyState( const OUString& PropertyName )
    throw(beans::UnknownPropertyException, uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertySimpleEntry* pEntry = getPropertyMapEntry(PropertyName);

    if( pEntry == NULL )
        throw beans::UnknownPropertyException();

    if( mpSet )
    {
        if( pEntry->nWID == OWN_ATTR_FILLBMP_MODE )
        {
            if( mpSet->GetItemState( XATTR_FILLBMP_STRETCH, false ) == SFX_ITEM_SET ||
                mpSet->GetItemState( XATTR_FILLBMP_TILE, false ) == SFX_ITEM_SET )
            {
                return beans::PropertyState_DIRECT_VALUE;
            }
            else
            {
                return beans::PropertyState_AMBIGUOUS_VALUE;
            }
        }

        switch( mpSet->GetItemState( pEntry->nWID, sal_False ) )
        {
        case SFX_ITEM_READONLY:
        case SFX_ITEM_SET:
            return beans::PropertyState_DIRECT_VALUE;
        case SFX_ITEM_DEFAULT:
            return beans::PropertyState_DEFAULT_VALUE;
        default:
//      case SFX_ITEM_DONTCARE:
//      case SFX_ITEM_DISABLED:
            return beans::PropertyState_AMBIGUOUS_VALUE;
        }
    }
    else
    {
        if( NULL == mpPropSet->GetUsrAnyForID(pEntry->nWID) )
            return beans::PropertyState_DEFAULT_VALUE;
        else
            return beans::PropertyState_DIRECT_VALUE;
    }
}

uno::Sequence< beans::PropertyState > SAL_CALL SdUnoPageBackground::getPropertyStates( const uno::Sequence< OUString >& aPropertyName )
    throw(beans::UnknownPropertyException, uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    sal_Int32 nCount = aPropertyName.getLength();
    const OUString* pNames = aPropertyName.getConstArray();

    uno::Sequence< beans::PropertyState > aPropertyStateSequence( nCount );
    beans::PropertyState* pState = aPropertyStateSequence.getArray();

    while( nCount-- )
        *pState++ = getPropertyState( *pNames++ );

    return aPropertyStateSequence;
}

void SAL_CALL SdUnoPageBackground::setPropertyToDefault( const OUString& PropertyName )
    throw(beans::UnknownPropertyException, uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertySimpleEntry* pEntry = getPropertyMapEntry(PropertyName);

    if( pEntry == NULL )
        throw beans::UnknownPropertyException();

    if( mpSet )
    {
        if( pEntry->nWID == OWN_ATTR_FILLBMP_MODE )
        {
            mpSet->ClearItem( XATTR_FILLBMP_STRETCH );
            mpSet->ClearItem( XATTR_FILLBMP_TILE );
        }
        else
        {
            mpSet->ClearItem( pEntry->nWID );
        }
    }
}

uno::Any SAL_CALL SdUnoPageBackground::getPropertyDefault( const OUString& aPropertyName )
    throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertySimpleEntry* pEntry = getPropertyMapEntry(aPropertyName);
    if( pEntry == NULL || mpSet == NULL )
        throw beans::UnknownPropertyException();

    uno::Any aAny;
    if( mpSet )
    {
        if( pEntry->nWID == OWN_ATTR_FILLBMP_MODE )
        {
            aAny <<= drawing::BitmapMode_REPEAT;
        }
        else
        {
            SfxItemPool& rPool = *mpSet->GetPool();
            SfxItemSet aSet( rPool, pEntry->nWID, pEntry->nWID);
            aSet.Put( rPool.GetDefaultItem( pEntry->nWID ) );

            aAny = SvxItemPropertySet_getPropertyValue( *mpPropSet, pEntry, aSet );
        }
    }
    return aAny;
}

/** this is used because our property map is not sorted yet */
const SfxItemPropertySimpleEntry* SdUnoPageBackground::getPropertyMapEntry( const OUString& rPropertyName ) const throw()
{
    return mpPropSet->getPropertyMap()->getByName(rPropertyName);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
