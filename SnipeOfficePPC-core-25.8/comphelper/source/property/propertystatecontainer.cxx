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

#include "comphelper/propertystatecontainer.hxx"
#include <rtl/ustrbuf.hxx>

//.........................................................................
namespace comphelper
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::lang;

    namespace
    {
        static ::rtl::OUString lcl_getUnknownPropertyErrorMessage( const ::rtl::OUString& _rPropertyName )
        {
            // TODO: perhaps it's time to think about resources in the comphelper module?
            // Would be nice to have localized exception strings (a simply resource file containing
            // strings only would suffice, and could be realized with an UNO service, so we do not
            // need the dependency to the Tools project)
            ::rtl::OUStringBuffer sMessage;
            sMessage.appendAscii( "The property \"" );
            sMessage.append( _rPropertyName );
            sMessage.appendAscii( "\" is unknown." );
            return sMessage.makeStringAndClear();
        }
    }

    //=====================================================================
    //= OPropertyStateContainer
    //=====================================================================
    //---------------------------------------------------------------------
    OPropertyStateContainer::OPropertyStateContainer( ::cppu::OBroadcastHelper& _rBHelper )
        :OPropertyContainer( _rBHelper )
    {
    }

    //--------------------------------------------------------------------
    Any SAL_CALL OPropertyStateContainer::queryInterface( const Type& _rType ) throw (RuntimeException)
    {
        Any aReturn = OPropertyContainer::queryInterface( _rType );
        if ( !aReturn.hasValue() )
            aReturn = OPropertyStateContainer_TBase::queryInterface( _rType );
        return aReturn;
    }

    //--------------------------------------------------------------------
    IMPLEMENT_FORWARD_XTYPEPROVIDER2( OPropertyStateContainer, OPropertyContainer, OPropertyStateContainer_TBase )

    //--------------------------------------------------------------------
    sal_Int32 OPropertyStateContainer::getHandleForName( const ::rtl::OUString& _rPropertyName ) SAL_THROW( ( UnknownPropertyException ) )
    {
        // look up the handle for the name
        ::cppu::IPropertyArrayHelper& rPH = getInfoHelper();
        sal_Int32 nHandle = rPH.getHandleByName( _rPropertyName );

        if ( -1 == nHandle )
            throw  UnknownPropertyException( lcl_getUnknownPropertyErrorMessage( _rPropertyName ), static_cast< XPropertyState* >( this ) );

        return nHandle;
    }

    //--------------------------------------------------------------------
    PropertyState SAL_CALL OPropertyStateContainer::getPropertyState( const ::rtl::OUString& _rPropertyName ) throw (UnknownPropertyException, RuntimeException)
    {
        return getPropertyStateByHandle( getHandleForName( _rPropertyName ) );
    }

    //--------------------------------------------------------------------
    Sequence< PropertyState > SAL_CALL OPropertyStateContainer::getPropertyStates( const Sequence< ::rtl::OUString >& _rPropertyNames ) throw (UnknownPropertyException, RuntimeException)
    {
        sal_Int32 nProperties = _rPropertyNames.getLength();
        Sequence< PropertyState> aStates( nProperties );
        if ( !nProperties )
            return aStates;

#ifdef _DEBUG
        // precondition: property sequence is sorted (the algorythm below relies on this)
        {
            const ::rtl::OUString* pNames = _rPropertyNames.getConstArray();
            const ::rtl::OUString* pNamesCompare = pNames + 1;
            const ::rtl::OUString* pNamesEnd = _rPropertyNames.getConstArray() + _rPropertyNames.getLength();
            for ( ; pNamesCompare != pNamesEnd; ++pNames, ++pNamesCompare )
                OSL_PRECOND( pNames->compareTo( *pNamesCompare ) < 0,
                    "OPropertyStateContainer::getPropertyStates: property sequence not sorted!" );
        }
#endif

        const ::rtl::OUString* pLookup = _rPropertyNames.getConstArray();
        const ::rtl::OUString* pLookupEnd = pLookup + nProperties;
        PropertyState* pStates = aStates.getArray();

        cppu::IPropertyArrayHelper& rHelper = getInfoHelper();
        Sequence< Property> aAllProperties  = rHelper.getProperties();
        sal_Int32 nAllProperties            = aAllProperties.getLength();
        const  Property* pAllProperties     = aAllProperties.getConstArray();
        const  Property* pAllPropertiesEnd  = pAllProperties + nAllProperties;

        osl::MutexGuard aGuard( rBHelper.rMutex );
        for ( ; ( pAllProperties != pAllPropertiesEnd ) && ( pLookup != pLookupEnd ); ++pAllProperties )
        {
#ifdef _DEBUG
            if ( pAllProperties < pAllPropertiesEnd - 1 )
                OSL_ENSURE( pAllProperties->Name.compareTo( (pAllProperties + 1)->Name ) < 0,
                    "OPropertyStateContainer::getPropertyStates: all-properties not sorted!" );
#endif
            if ( pAllProperties->Name.equals( *pLookup ) )
            {
                *pStates++ = getPropertyState( *pLookup );
                ++pLookup;
            }
        }

        if ( pLookup != pLookupEnd )
            // we run out of properties from the IPropertyArrayHelper, but still have properties to lookup
            // -> we were asked for a nonexistent property
            throw UnknownPropertyException( lcl_getUnknownPropertyErrorMessage( *pLookup ), static_cast< XPropertyState* >( this ) );

        return aStates;
    }

    //--------------------------------------------------------------------
    void SAL_CALL OPropertyStateContainer::setPropertyToDefault( const ::rtl::OUString& _rPropertyName ) throw (UnknownPropertyException, RuntimeException)
    {
        setPropertyToDefaultByHandle( getHandleForName( _rPropertyName ) );
    }

    //--------------------------------------------------------------------
    Any SAL_CALL OPropertyStateContainer::getPropertyDefault( const ::rtl::OUString& _rPropertyName ) throw (UnknownPropertyException, WrappedTargetException, RuntimeException)
    {
        Any aDefault;
        getPropertyDefaultByHandle( getHandleForName( _rPropertyName ), aDefault );
        return aDefault;
    }

    //--------------------------------------------------------------------
    PropertyState OPropertyStateContainer::getPropertyStateByHandle( sal_Int32 _nHandle )
    {
        // simply compare the current and the default value
        Any aCurrentValue; getFastPropertyValue( aCurrentValue, _nHandle );
        Any aDefaultValue; getPropertyDefaultByHandle( _nHandle, aDefaultValue );

        sal_Bool bEqual = uno_type_equalData(
                const_cast< void* >( aCurrentValue.getValue() ), aCurrentValue.getValueType().getTypeLibType(),
                const_cast< void* >( aDefaultValue.getValue() ), aDefaultValue.getValueType().getTypeLibType(),
                reinterpret_cast< uno_QueryInterfaceFunc >(cpp_queryInterface),
                reinterpret_cast< uno_ReleaseFunc >(cpp_release)
            );
        if ( bEqual )
            return PropertyState_DEFAULT_VALUE;
        else
            return PropertyState_DIRECT_VALUE;
    }

    //--------------------------------------------------------------------
    void OPropertyStateContainer::setPropertyToDefaultByHandle( sal_Int32 _nHandle )
    {
        Any aDefault;
        getPropertyDefaultByHandle( _nHandle, aDefault );
        setFastPropertyValue( _nHandle, aDefault );
    }

//.........................................................................
}   // namespace comphelper
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
