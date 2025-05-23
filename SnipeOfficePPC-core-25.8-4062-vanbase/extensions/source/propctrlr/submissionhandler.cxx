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

#include "submissionhandler.hxx"
#include "formmetadata.hxx"
#include "formstrings.hxx"
#include "handlerhelper.hxx"

#include <com/sun/star/form/FormButtonType.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/form/submission/XSubmissionSupplier.hpp>
#include <com/sun/star/inspection/XObjectInspectorUI.hpp>
#include <tools/debug.hxx>
#include <rtl/ustrbuf.hxx>

//------------------------------------------------------------------------
extern "C" void SAL_CALL createRegistryInfo_SubmissionPropertyHandler()
{
    ::pcr::SubmissionPropertyHandler::registerImplementation();
}

//........................................................................
namespace pcr
{
//........................................................................

    using namespace ::comphelper;
    using namespace ::com::sun::star;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::script;
    using namespace ::com::sun::star::form;
    using namespace ::com::sun::star::xforms;
    using namespace ::com::sun::star::container;
    using namespace ::com::sun::star::inspection;

    //====================================================================
    //= SubmissionHelper
    //====================================================================
    //--------------------------------------------------------------------
    SubmissionHelper::SubmissionHelper( ::osl::Mutex& _rMutex, const Reference< XPropertySet >& _rxIntrospectee, const Reference< frame::XModel >& _rxContextDocument )
        :EFormsHelper( _rMutex, _rxIntrospectee, _rxContextDocument )
    {
        OSL_ENSURE( canTriggerSubmissions( _rxIntrospectee, _rxContextDocument ),
            "SubmissionHelper::SubmissionHelper: you should not have instantiated me!" );
    }

    //--------------------------------------------------------------------
    bool SubmissionHelper::canTriggerSubmissions( const Reference< XPropertySet >& _rxControlModel,
        const Reference< frame::XModel >& _rxContextDocument ) SAL_THROW(())
    {
        if ( !EFormsHelper::isEForm( _rxContextDocument ) )
            return false;

        try
        {
            Reference< submission::XSubmissionSupplier > xSubmissionSupp( _rxControlModel, UNO_QUERY );
            if ( xSubmissionSupp.is() )
                return true;
        }
        catch( const Exception& )
        {
            OSL_FAIL( "SubmissionHelper::canTriggerSubmissions: caught an exception!" );
        }
        return false;
    }

    //====================================================================
    //= SubmissionPropertyHandler
    //====================================================================
    DBG_NAME( SubmissionPropertyHandler )
    //--------------------------------------------------------------------
    SubmissionPropertyHandler::SubmissionPropertyHandler( const Reference< XComponentContext >& _rxContext )
        :EditPropertyHandler_Base( _rxContext )
        ,OPropertyChangeListener( m_aMutex )
        ,m_pPropChangeMultiplexer( NULL )
    {
        DBG_CTOR( SubmissionPropertyHandler, NULL );
    }

    //--------------------------------------------------------------------
    SubmissionPropertyHandler::~SubmissionPropertyHandler( )
    {
        disposeAdapter();
        DBG_DTOR( SubmissionPropertyHandler, NULL );
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL SubmissionPropertyHandler::getImplementationName_static(  ) throw (RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.extensions.SubmissionPropertyHandler" ) );
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL SubmissionPropertyHandler::getSupportedServiceNames_static(  ) throw (RuntimeException)
    {
        Sequence< ::rtl::OUString > aSupported( 1 );
        aSupported[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.form.inspection.SubmissionPropertyHandler" ) );
        return aSupported;
    }

    //--------------------------------------------------------------------
    Any SAL_CALL SubmissionPropertyHandler::getPropertyValue( const ::rtl::OUString& _rPropertyName ) throw (UnknownPropertyException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        PropertyId nPropId( impl_getPropertyId_throw( _rPropertyName ) );

        OSL_ENSURE( m_pHelper.get(), "SubmissionPropertyHandler::getPropertyValue: inconsistency!" );
            // if we survived impl_getPropertyId_throw, we should have a helper, since no helper implies no properties

        Any aReturn;
        try
        {
            switch ( nPropId )
            {
            case PROPERTY_ID_SUBMISSION_ID:
            {
                Reference< submission::XSubmissionSupplier > xSubmissionSupp( m_xComponent, UNO_QUERY );
                OSL_ENSURE( xSubmissionSupp.is(), "SubmissionPropertyHandler::getPropertyValue: this should never happen ..." );
                    // this handler is not intended for components which are no XSubmissionSupplier
                Reference< submission::XSubmission > xSubmission;
                if ( xSubmissionSupp.is() )
                    xSubmission = xSubmissionSupp->getSubmission( );
                aReturn <<= xSubmission;
            }
            break;

            case PROPERTY_ID_XFORMS_BUTTONTYPE:
            {
                FormButtonType eType = FormButtonType_PUSH;
                OSL_VERIFY( m_xComponent->getPropertyValue( PROPERTY_BUTTONTYPE ) >>= eType );
                if ( ( eType != FormButtonType_PUSH ) && ( eType != FormButtonType_SUBMIT ) )
                    eType = FormButtonType_PUSH;
                aReturn <<= eType;
            }
            break;

            default:
                OSL_FAIL( "SubmissionPropertyHandler::getPropertyValue: cannot handle this property!" );
                break;
            }
        }
        catch( const Exception& )
        {
            OSL_FAIL( "SubmissionPropertyHandler::getPropertyValue: caught an exception!" );
        }

        return aReturn;
    }

    //--------------------------------------------------------------------
    void SAL_CALL SubmissionPropertyHandler::setPropertyValue( const ::rtl::OUString& _rPropertyName, const Any& _rValue ) throw (UnknownPropertyException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        PropertyId nPropId( impl_getPropertyId_throw( _rPropertyName ) );

        OSL_ENSURE( m_pHelper.get(), "SubmissionPropertyHandler::setPropertyValue: inconsistency!" );
            // if we survived impl_getPropertyId_throw, we should have a helper, since no helper implies no properties

        try
        {
            switch ( nPropId )
            {
            case PROPERTY_ID_SUBMISSION_ID:
            {
                Reference< submission::XSubmission > xSubmission;
                OSL_VERIFY( _rValue >>= xSubmission );

                Reference< submission::XSubmissionSupplier > xSubmissionSupp( m_xComponent, UNO_QUERY );
                OSL_ENSURE( xSubmissionSupp.is(), "SubmissionPropertyHandler::setPropertyValue: this should never happen ..." );
                    // this handler is not intended for components which are no XSubmissionSupplier
                if ( xSubmissionSupp.is() )
                {
                    xSubmissionSupp->setSubmission( xSubmission );
                    impl_setContextDocumentModified_nothrow();
                }
            }
            break;

            case PROPERTY_ID_XFORMS_BUTTONTYPE:
                m_xComponent->setPropertyValue( PROPERTY_BUTTONTYPE, _rValue );
                break;

            default:
                OSL_FAIL( "SubmissionPropertyHandler::setPropertyValue: cannot handle this id!" );
            }
        }
        catch( const Exception& )
        {
            OSL_FAIL( "SubmissionPropertyHandler::setPropertyValue: caught an exception!" );
        }
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL SubmissionPropertyHandler::getActuatingProperties( ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !m_pHelper.get() )
            return Sequence< ::rtl::OUString >();

        Sequence< ::rtl::OUString > aReturn( 1 );
        aReturn[ 0 ] = PROPERTY_XFORMS_BUTTONTYPE;
        return aReturn;
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL SubmissionPropertyHandler::getSupersededProperties( ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !m_pHelper.get() )
            return Sequence< ::rtl::OUString >();

        Sequence< ::rtl::OUString > aReturn( 3 );
        aReturn[ 0 ] = PROPERTY_TARGET_URL;
        aReturn[ 1 ] = PROPERTY_TARGET_FRAME;
        aReturn[ 2 ] = PROPERTY_BUTTONTYPE;
        return aReturn;
    }

    //--------------------------------------------------------------------
    void SubmissionPropertyHandler::onNewComponent()
    {
        if ( m_pPropChangeMultiplexer )
        {
            m_pPropChangeMultiplexer->dispose();
            m_pPropChangeMultiplexer->release();
            m_pPropChangeMultiplexer = NULL;
        }

        EditPropertyHandler_Base::onNewComponent();

        Reference< frame::XModel > xDocument( impl_getContextDocument_nothrow() );
        DBG_ASSERT( xDocument.is(), "SubmissionPropertyHandler::onNewComponent: no document!" );

        m_pHelper.reset( NULL );

        if ( SubmissionHelper::canTriggerSubmissions( m_xComponent, xDocument ) )
        {
            m_pHelper.reset( new SubmissionHelper( m_aMutex, m_xComponent, xDocument ) );

            m_pPropChangeMultiplexer = new OPropertyChangeMultiplexer( this, m_xComponent );
            m_pPropChangeMultiplexer->acquire();
            m_pPropChangeMultiplexer->addProperty( PROPERTY_BUTTONTYPE );
        }
    }

    //--------------------------------------------------------------------
    Sequence< Property > SAL_CALL SubmissionPropertyHandler::doDescribeSupportedProperties() const
    {
        ::std::vector< Property > aProperties;
        if ( m_pHelper.get() )
        {
            implAddPropertyDescription( aProperties, PROPERTY_SUBMISSION_ID, ::getCppuType( static_cast< Reference< submission::XSubmission > * >( NULL ) ) );
            implAddPropertyDescription( aProperties, PROPERTY_XFORMS_BUTTONTYPE, ::getCppuType( static_cast< FormButtonType* >( NULL ) ) );
        }
        if ( aProperties.empty() )
            return Sequence< Property >();
        return Sequence< Property >( &(*aProperties.begin()), aProperties.size() );
    }

    //--------------------------------------------------------------------
    LineDescriptor SAL_CALL SubmissionPropertyHandler::describePropertyLine( const ::rtl::OUString& _rPropertyName,
        const Reference< XPropertyControlFactory >& _rxControlFactory )
        throw (UnknownPropertyException, NullPointerException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !_rxControlFactory.is() )
            throw NullPointerException();
        if ( !m_pHelper.get() )
            RuntimeException();

        ::std::vector< ::rtl::OUString > aListEntries;
        PropertyId nPropId( impl_getPropertyId_throw( _rPropertyName ) );
        switch ( nPropId )
        {
        case PROPERTY_ID_SUBMISSION_ID:
            const_cast< SubmissionHelper* >( m_pHelper.get() )->getAllElementUINames( EFormsHelper::Submission, aListEntries, false );
            break;

        case PROPERTY_ID_XFORMS_BUTTONTYPE:
        {
            // available options are nearly the same as for the "normal" button type, but only the
            // first two options
            aListEntries = m_pInfoService->getPropertyEnumRepresentations( PROPERTY_ID_BUTTONTYPE );
            aListEntries.resize( 2 );
        }
        break;

        default:
            OSL_FAIL( "SubmissionPropertyHandler::describePropertyLine: cannot handle this id!" );
            return LineDescriptor();
        }

        LineDescriptor aDescriptor;
        aDescriptor.Control = PropertyHandlerHelper::createListBoxControl( _rxControlFactory, aListEntries, sal_False, sal_True );
        aDescriptor.DisplayName = m_pInfoService->getPropertyTranslation( nPropId );
        aDescriptor.Category = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "General" ) );
        aDescriptor.HelpURL = HelpIdUrl::getHelpURL( m_pInfoService->getPropertyHelpId( nPropId ) );
        return aDescriptor;
    }

    //--------------------------------------------------------------------
    void SAL_CALL SubmissionPropertyHandler::actuatingPropertyChanged( const ::rtl::OUString& _rActuatingPropertyName, const Any& _rNewValue, const Any& /*_rOldValue*/, const Reference< XObjectInspectorUI >& _rxInspectorUI, sal_Bool ) throw (NullPointerException, RuntimeException)
    {
        if ( !_rxInspectorUI.is() )
            throw NullPointerException();

        ::osl::MutexGuard aGuard( m_aMutex );
        PropertyId nActuatingPropId( impl_getPropertyId_throw( _rActuatingPropertyName ) );
        OSL_PRECOND( m_pHelper.get(), "SubmissionPropertyHandler::actuatingPropertyChanged: inconsistentcy!" );
            // if we survived impl_getPropertyId_throw, we should have a helper, since no helper implies no properties

        switch ( nActuatingPropId )
        {
        case PROPERTY_ID_XFORMS_BUTTONTYPE:
        {
            FormButtonType eType = FormButtonType_PUSH;
            OSL_VERIFY( _rNewValue >>= eType );
            _rxInspectorUI->enablePropertyUI( PROPERTY_SUBMISSION_ID, eType == FormButtonType_SUBMIT );
        }
        break;

        default:
            OSL_FAIL( "SubmissionPropertyHandler::actuatingPropertyChanged: cannot handle this id!" );
        }
    }

    //--------------------------------------------------------------------
    Any SAL_CALL SubmissionPropertyHandler::convertToPropertyValue( const ::rtl::OUString& _rPropertyName, const Any& _rControlValue ) throw (UnknownPropertyException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        Any aPropertyValue;

        OSL_ENSURE( m_pHelper.get(), "SubmissionPropertyHandler::convertToPropertyValue: we have no SupportedProperties!" );
        if ( !m_pHelper.get() )
            return aPropertyValue;

        ::rtl::OUString sControlValue;
        OSL_VERIFY( _rControlValue >>= sControlValue );

        PropertyId nPropId( m_pInfoService->getPropertyId( _rPropertyName ) );
        switch ( nPropId )
        {
        case PROPERTY_ID_SUBMISSION_ID:
        {
            Reference< XSubmission > xSubmission( m_pHelper->getModelElementFromUIName( EFormsHelper::Submission, sControlValue ), UNO_QUERY );
            aPropertyValue <<= xSubmission;
        }
        break;

        case PROPERTY_ID_XFORMS_BUTTONTYPE:
        {
            ::rtl::Reference< IPropertyEnumRepresentation > aEnumConversion(
                new DefaultEnumRepresentation( *m_pInfoService, ::getCppuType( static_cast< FormButtonType* >( NULL ) ), PROPERTY_ID_BUTTONTYPE ) );
            // TODO/UNOize: make aEnumConversion a member?
            aEnumConversion->getValueFromDescription( sControlValue, aPropertyValue );
        }
        break;

        default:
            OSL_FAIL( "SubmissionPropertyHandler::convertToPropertyValue: cannot handle this id!" );
        }

        return aPropertyValue;
    }

    //--------------------------------------------------------------------
    Any SAL_CALL SubmissionPropertyHandler::convertToControlValue( const ::rtl::OUString& _rPropertyName, const Any& _rPropertyValue, const Type& _rControlValueType ) throw (UnknownPropertyException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        Any aControlValue;

        OSL_ENSURE( m_pHelper.get(), "SubmissionPropertyHandler::convertToControlValue: we have no SupportedProperties!" );
        if ( !m_pHelper.get() )
            return aControlValue;

        OSL_ENSURE( _rControlValueType.getTypeClass() == TypeClass_STRING,
            "SubmissionPropertyHandler::convertToControlValue: all our controls should use strings for value exchange!" );
        (void)_rControlValueType;

        PropertyId nPropId( m_pInfoService->getPropertyId( _rPropertyName ) );
        switch ( nPropId )
        {
        case PROPERTY_ID_SUBMISSION_ID:
        {
            Reference< XPropertySet > xSubmission( _rPropertyValue, UNO_QUERY );
            if ( xSubmission.is() )
                aControlValue <<= m_pHelper->getModelElementUIName( EFormsHelper::Submission, xSubmission );
        }
        break;

        case PROPERTY_ID_XFORMS_BUTTONTYPE:
        {
            ::rtl::Reference< IPropertyEnumRepresentation > aEnumConversion(
                new DefaultEnumRepresentation( *m_pInfoService, _rPropertyValue.getValueType(), PROPERTY_ID_BUTTONTYPE ) );
            // TODO/UNOize: make aEnumConversion a member?
            aControlValue <<= aEnumConversion->getDescriptionForValue( _rPropertyValue );
        }
        break;

        default:
            OSL_FAIL( "SubmissionPropertyHandler::convertToControlValue: cannot handle this id!" );
        }

        return aControlValue;
    }

    //--------------------------------------------------------------------
    void SubmissionPropertyHandler::_propertyChanged( const PropertyChangeEvent& _rEvent ) throw(RuntimeException)
    {
        if ( _rEvent.PropertyName == PROPERTY_BUTTONTYPE )
            firePropertyChange( PROPERTY_XFORMS_BUTTONTYPE, PROPERTY_ID_XFORMS_BUTTONTYPE, _rEvent.OldValue, _rEvent.NewValue );
    }

//........................................................................
} // namespace pcr
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
