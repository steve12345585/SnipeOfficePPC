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

#include "inspectormodelbase.hxx"
#include "pcrcommon.hxx"

#include <com/sun/star/beans/PropertyAttribute.hpp>

#include <comphelper/propertycontainerhelper.hxx>

//........................................................................
namespace pcr
{
//........................................................................

#define MODEL_PROPERTY_ID_HAS_HELP_SECTION      2000
#define MODEL_PROPERTY_ID_MIN_HELP_TEXT_LINES   2001
#define MODEL_PROPERTY_ID_MAX_HELP_TEXT_LINES   2002
#define MODEL_PROPERTY_ID_IS_READ_ONLY          2003

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::beans::XPropertySetInfo;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::lang::IllegalArgumentException;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::inspection::PropertyCategoryDescriptor;
    using ::com::sun::star::uno::makeAny;
    using ::com::sun::star::beans::Property;
    /** === end UNO using === **/
    namespace PropertyAttribute = ::com::sun::star::beans::PropertyAttribute;

    //====================================================================
    //= InspectorModelProperties
    //====================================================================
    /** helper class for implementing the property set related functionality
        of an ImplInspectorModel
    */
    class InspectorModelProperties : public ::comphelper::OPropertyContainerHelper
    {
    private:
        ::osl::Mutex&           m_rMutex;
        sal_Bool                m_bHasHelpSection;
        sal_Int32               m_nMinHelpTextLines;
        sal_Int32               m_nMaxHelpTextLines;
        sal_Bool                m_bIsReadOnly;
        ::std::auto_ptr< ::cppu::IPropertyArrayHelper >
                                m_pPropertyInfo;

    public:
        InspectorModelProperties( ::osl::Mutex& _rMutex );

        using ::comphelper::OPropertyContainerHelper::convertFastPropertyValue;
        using ::comphelper::OPropertyContainerHelper::setFastPropertyValue;
        using ::comphelper::OPropertyContainerHelper::getFastPropertyValue;

    public:
        inline  sal_Bool    hasHelpSection() const { return m_bHasHelpSection; }
        inline  sal_Bool    isReadOnly() const { return m_bIsReadOnly; }
        inline  sal_Int32   getMinHelpTextLines() const { return m_nMinHelpTextLines; }
        inline  sal_Int32   getMaxHelpTextLines() const { return m_nMaxHelpTextLines; }

        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
                            getPropertySetInfo();
        ::cppu::IPropertyArrayHelper&
                            getInfoHelper();

        void    constructWithHelpSection( sal_Int32 _nMinHelpTextLines, sal_Int32 _nMaxHelpTextLines );
    };

    //====================================================================
    //= InspectorModelProperties
    //====================================================================
    //--------------------------------------------------------------------
    InspectorModelProperties::InspectorModelProperties( ::osl::Mutex& _rMutex )
        :m_rMutex( _rMutex )
        ,m_bHasHelpSection( sal_False )
        ,m_nMinHelpTextLines( 3 )
        ,m_nMaxHelpTextLines( 8 )
        ,m_bIsReadOnly( sal_False )
    {
        registerProperty(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "HasHelpSection" ) ),
            MODEL_PROPERTY_ID_HAS_HELP_SECTION,
            PropertyAttribute::READONLY,
            &m_bHasHelpSection, ::getCppuType( &m_bHasHelpSection )
        );
        registerProperty(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MinHelpTextLines" ) ),
            MODEL_PROPERTY_ID_MIN_HELP_TEXT_LINES,
            PropertyAttribute::READONLY,
            &m_nMinHelpTextLines, ::getCppuType( &m_nMinHelpTextLines )
        );
        registerProperty(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MaxHelpTextLines" ) ),
            MODEL_PROPERTY_ID_MAX_HELP_TEXT_LINES,
            PropertyAttribute::READONLY,
            &m_nMaxHelpTextLines, ::getCppuType( &m_nMaxHelpTextLines )
        );
        registerProperty(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsReadOnly" ) ),
            MODEL_PROPERTY_ID_IS_READ_ONLY,
            PropertyAttribute::BOUND,
            &m_bIsReadOnly, ::getCppuType( &m_bIsReadOnly )
        );
    }

    //--------------------------------------------------------------------
    void InspectorModelProperties::constructWithHelpSection( sal_Int32 _nMinHelpTextLines, sal_Int32 _nMaxHelpTextLines )
    {
        m_bHasHelpSection = sal_True;
        m_nMinHelpTextLines = _nMinHelpTextLines;
        m_nMaxHelpTextLines = _nMaxHelpTextLines;
        // no need to notify this, those properties are not bound. Also, the method should
        // only be used during construction phase, where we don't expect to have any listeners.
    }

    //--------------------------------------------------------------------
    ::cppu::IPropertyArrayHelper& InspectorModelProperties::getInfoHelper()
    {
        ::osl::MutexGuard aGuard( m_rMutex );
        if ( m_pPropertyInfo.get() == NULL )
        {
            Sequence< Property > aProperties;
            describeProperties( aProperties );

            m_pPropertyInfo.reset( new ::cppu::OPropertyArrayHelper( aProperties ) );
        }
        return *m_pPropertyInfo;
    }

    //--------------------------------------------------------------------
    Reference< XPropertySetInfo > InspectorModelProperties::getPropertySetInfo()
    {
        return ::cppu::OPropertySetHelper::createPropertySetInfo( getInfoHelper() );
    }

    //====================================================================
    //= ImplInspectorModel
    //====================================================================
    ImplInspectorModel::ImplInspectorModel( const Reference< XComponentContext >& _rxContext )
        :ImplInspectorModel_PBase( GetBroadcastHelper() )
        ,m_aContext( _rxContext )
        ,m_pProperties( new InspectorModelProperties( m_aMutex ) )
    {
    }

    //--------------------------------------------------------------------
    ImplInspectorModel::~ImplInspectorModel()
    {
    }

    //--------------------------------------------------------------------
    IMPLEMENT_FORWARD_XINTERFACE2( ImplInspectorModel, ImplInspectorModel_Base, ImplInspectorModel_PBase )

    //--------------------------------------------------------------------
    IMPLEMENT_FORWARD_XTYPEPROVIDER2( ImplInspectorModel, ImplInspectorModel_Base, ImplInspectorModel_PBase )

    //--------------------------------------------------------------------
    Reference< XPropertySetInfo > SAL_CALL ImplInspectorModel::getPropertySetInfo(  ) throw (RuntimeException)
    {
        return m_pProperties->getPropertySetInfo();
    }

    //--------------------------------------------------------------------
    ::cppu::IPropertyArrayHelper& SAL_CALL ImplInspectorModel::getInfoHelper()
    {
        return m_pProperties->getInfoHelper();
    }

    //--------------------------------------------------------------------
    sal_Bool SAL_CALL ImplInspectorModel::convertFastPropertyValue( Any & rConvertedValue, Any & rOldValue, sal_Int32 nHandle, const Any& rValue ) throw (IllegalArgumentException)
    {
        return m_pProperties->convertFastPropertyValue( rConvertedValue, rOldValue, nHandle, rValue );
    }

    //--------------------------------------------------------------------
    void SAL_CALL ImplInspectorModel::setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const Any& rValue ) throw (Exception)
    {
        m_pProperties->setFastPropertyValue( nHandle, rValue );
    }

    //--------------------------------------------------------------------
    void SAL_CALL ImplInspectorModel::getFastPropertyValue( Any& rValue, sal_Int32 nHandle ) const
    {
        m_pProperties->getFastPropertyValue( rValue, nHandle );
    }

    //--------------------------------------------------------------------
    ::sal_Bool SAL_CALL ImplInspectorModel::getHasHelpSection() throw (RuntimeException)
    {
        return m_pProperties->hasHelpSection();
    }

    //--------------------------------------------------------------------
    ::sal_Int32 SAL_CALL ImplInspectorModel::getMinHelpTextLines() throw (RuntimeException)
    {
        return m_pProperties->getMinHelpTextLines();
    }

    //--------------------------------------------------------------------
    ::sal_Int32 SAL_CALL ImplInspectorModel::getMaxHelpTextLines() throw (RuntimeException)
    {
        return m_pProperties->getMaxHelpTextLines();
    }

    //--------------------------------------------------------------------
    ::sal_Bool SAL_CALL ImplInspectorModel::getIsReadOnly() throw (::com::sun::star::uno::RuntimeException)
    {
        return m_pProperties->isReadOnly();
    }

    //--------------------------------------------------------------------
    void SAL_CALL ImplInspectorModel::setIsReadOnly( ::sal_Bool _IsReadOnly ) throw (::com::sun::star::uno::RuntimeException)
    {
        setFastPropertyValue( MODEL_PROPERTY_ID_IS_READ_ONLY, makeAny( _IsReadOnly ) );
    }

    //--------------------------------------------------------------------
    ::sal_Bool SAL_CALL ImplInspectorModel::supportsService( const ::rtl::OUString& ServiceName ) throw (RuntimeException)
    {
        StlSyntaxSequence< ::rtl::OUString > aSupported( getSupportedServiceNames() );
        for (   StlSyntaxSequence< ::rtl::OUString >::const_iterator check = aSupported.begin();
                check != aSupported.end();
                ++check
            )
            if ( check->equals( ServiceName ) )
                return sal_True;

        return sal_False;
    }

    //--------------------------------------------------------------------
    void ImplInspectorModel::enableHelpSectionProperties( sal_Int32 _nMinHelpTextLines, sal_Int32 _nMaxHelpTextLines )
    {
        m_pProperties->constructWithHelpSection( _nMinHelpTextLines, _nMaxHelpTextLines );
    }

//........................................................................
} // namespace pcr
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
