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

#include "xsddatatypes.hxx"
#include "formstrings.hxx"

#include <com/sun/star/xsd/DataTypeClass.hpp>
#include <com/sun/star/xsd/XDataType.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <tools/debug.hxx>

//........................................................................
namespace pcr
{
//........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::xsd;
    using namespace ::com::sun::star::beans;

    //====================================================================
    //= helper
    //====================================================================
    //--------------------------------------------------------------------
    template< typename INTERFACE, typename ARGUMENT >
    void setSave( INTERFACE* pObject, void ( SAL_CALL INTERFACE::*pSetter )( ARGUMENT ), ARGUMENT _rArg )
    {
        try
        {
            (pObject->*pSetter)( _rArg );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "XSDDataType: setSave: caught an exception!" );
        }
    }

    //--------------------------------------------------------------------
    template< typename INTERFACE, typename ARGUMENT >
    ARGUMENT getSave( INTERFACE* pObject, ARGUMENT ( SAL_CALL INTERFACE::*pGetter )( ) )
    {
        ARGUMENT aReturn = ARGUMENT();
        try
        {
            aReturn = (pObject->*pGetter)( );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "XSDDataType: getSave: caught an exception!" );
        }
        return aReturn;
    }

    template< typename FACETTYPE >
    FACETTYPE getFacet( const Reference< XPropertySet >& _rxFacets, const ::rtl::OUString& _rFacetName ) SAL_THROW(())
    {
        FACETTYPE aReturn;
        try
        {
            OSL_VERIFY( _rxFacets->getPropertyValue( _rFacetName ) >>= aReturn );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "XSDDataType: getFacet: caught an exception!" );
        }
        return aReturn;
    }

    //====================================================================
    //= XSDDataType
    //====================================================================
    //--------------------------------------------------------------------
    XSDDataType::XSDDataType( const Reference< XDataType >& _rxDataType )
        :m_xDataType( _rxDataType )
        ,m_refCount( 0 )
    {
        DBG_ASSERT( m_xDataType.is(), "XSDDataType::XSDDataType: invalid UNO object!" );
        if ( m_xDataType.is() )
            m_xFacetInfo = m_xDataType->getPropertySetInfo();
    }

    //--------------------------------------------------------------------
    oslInterlockedCount SAL_CALL XSDDataType::acquire()
    {
        return osl_atomic_increment( &m_refCount );
    }

    //--------------------------------------------------------------------
    oslInterlockedCount SAL_CALL XSDDataType::release()
    {
        if ( 0 == osl_atomic_decrement( &m_refCount ) )
        {
           delete this;
           return 0;
        }
        return m_refCount;
    }

    //--------------------------------------------------------------------
    XSDDataType::~XSDDataType()
    {
    }

    //--------------------------------------------------------------------
    sal_Int16 XSDDataType::classify() const SAL_THROW(())
    {
        sal_Int16 nTypeClass = DataTypeClass::STRING;
        try
        {
            if ( m_xDataType.is() )
                nTypeClass = m_xDataType->getTypeClass();
        }
        catch( const Exception& )
        {
            OSL_FAIL( "XSDDataType::classify: caught an exception!" );
        }
        return nTypeClass;
    }

    //--------------------------------------------------------------------
    bool XSDDataType::isBasicType() const SAL_THROW(())
    {
        return getSave( m_xDataType.get(), &XDataType::getIsBasic );
    }

    //--------------------------------------------------------------------
    ::rtl::OUString XSDDataType::getName() const SAL_THROW(())
    {
        return getSave( m_xDataType.get(), &XDataType::getName );
    }

     //--------------------------------------------------------------------
    void XSDDataType::setFacet( const ::rtl::OUString& _rFacetName, const Any& _rValue ) SAL_THROW(())
    {
        try
        {
            m_xDataType->setPropertyValue( _rFacetName, _rValue );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "XSDDataType::setFacet: caught an exception - sure this is the right data type class for this property?" );
        }
    }

    //--------------------------------------------------------------------
    bool XSDDataType::hasFacet( const ::rtl::OUString& _rFacetName ) const SAL_THROW(())
    {
        bool bReturn = false;
        try
        {
            bReturn = m_xFacetInfo.is() && m_xFacetInfo->hasPropertyByName( _rFacetName );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "XSDDataType::hasFacet: caught an exception!" );
        }
        return bReturn;
    }
    //--------------------------------------------------------------------
    Any XSDDataType::getFacet( const ::rtl::OUString& _rFacetName ) SAL_THROW(())
    {
        Any aReturn;
        try
        {
            aReturn = m_xDataType->getPropertyValue( _rFacetName );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "XSDDataType::getFacet: caught an exception - sure this is the right data type class for this property?" );
        }
        return aReturn;
    }

    //--------------------------------------------------------------------
    namespace
    {
        void lcl_copyProperties( const Reference< XPropertySet >& _rxSource, const Reference< XPropertySet >& _rxDest )
        {
            Reference< XPropertySetInfo > xSourceInfo;
            if ( _rxSource.is() )
                xSourceInfo = _rxSource->getPropertySetInfo();
            Reference< XPropertySetInfo > xDestInfo;
            if ( _rxDest.is() )
                xDestInfo = _rxDest->getPropertySetInfo();
            OSL_ENSURE( xSourceInfo.is() && xDestInfo.is(), "lcl_copyProperties: invalid property set( info)s!" );
            if ( !xSourceInfo.is() || !xDestInfo.is() )
                return;

            Sequence< Property > aProperties( xSourceInfo->getProperties() );
            const Property* pProperties = aProperties.getConstArray();
            const Property* pPropertiesEnd = pProperties + aProperties.getLength();
            for ( ; pProperties != pPropertiesEnd; ++pProperties )
            {
                if ( xDestInfo->hasPropertyByName( pProperties->Name ) )
                    _rxDest->setPropertyValue( pProperties->Name, _rxSource->getPropertyValue( pProperties->Name ) );
            }
        }
    }

    //--------------------------------------------------------------------
    void XSDDataType::copyFacetsFrom( const ::rtl::Reference< XSDDataType >& _pSourceType )
    {
        OSL_ENSURE( _pSourceType.is(), "XSDDataType::copyFacetsFrom: invalid source type!" );
        if ( !_pSourceType.is() )
            return;

        try
        {
            Reference< XPropertySet > xSource( _pSourceType->getUnoDataType(), UNO_QUERY );
            Reference< XPropertySet > xDest( getUnoDataType(), UNO_QUERY );
            lcl_copyProperties( xSource, xDest );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "XSDDataType::copyFacetsFrom: caught an exception!" );
        }
    }

//........................................................................
} // namespace pcr
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
