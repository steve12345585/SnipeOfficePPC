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

#ifndef EXTENSIONS_SOURCE_PROPCTRLR_XSDVALIDATIONPROPERTYHANDLER_HXX
#define EXTENSIONS_SOURCE_PROPCTRLR_XSDVALIDATIONPROPERTYHANDLER_HXX

#include "propertyhandler.hxx"

/** === begin UNO includes === **/
/** === end UNO includes === **/

#include <memory>

//........................................................................
namespace pcr
{
//........................................................................

    class XSDValidationHelper;
    //====================================================================
    //= XSDValidationPropertyHandler
    //====================================================================
    class XSDValidationPropertyHandler;
    typedef HandlerComponentBase< XSDValidationPropertyHandler > XSDValidationPropertyHandler_Base;
    class XSDValidationPropertyHandler : public XSDValidationPropertyHandler_Base
    {
    private:
        ::std::auto_ptr< XSDValidationHelper >  m_pHelper;

    public:
        XSDValidationPropertyHandler(
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& _rxContext
        );

        static ::rtl::OUString SAL_CALL getImplementationName_static(  ) throw (::com::sun::star::uno::RuntimeException);
        static ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_static(  ) throw (::com::sun::star::uno::RuntimeException);

    protected:
        ~XSDValidationPropertyHandler();

    protected:
        // XPropertyHandler overriables
        virtual ::com::sun::star::uno::Any  SAL_CALL getPropertyValue( const ::rtl::OUString& _rPropertyName ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
        virtual void                        SAL_CALL setPropertyValue( const ::rtl::OUString& _rPropertyName, const ::com::sun::star::uno::Any& _rValue ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                            SAL_CALL getSupersededProperties( ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                            SAL_CALL getActuatingProperties( ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::inspection::LineDescriptor
                                            SAL_CALL describePropertyLine( const ::rtl::OUString& _rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XPropertyControlFactory >& _rxControlFactory ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::NullPointerException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::inspection::InteractiveSelectionResult
                                            SAL_CALL onInteractivePropertySelection( const ::rtl::OUString& _rPropertyName, sal_Bool _bPrimary, ::com::sun::star::uno::Any& _rData, const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspectorUI >& _rxInspectorUI ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::NullPointerException, ::com::sun::star::uno::RuntimeException);
        virtual void                        SAL_CALL actuatingPropertyChanged( const ::rtl::OUString& _rActuatingPropertyName, const ::com::sun::star::uno::Any& _rNewValue, const ::com::sun::star::uno::Any& _rOldValue, const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspectorUI >& _rxInspectorUI, sal_Bool ) throw (::com::sun::star::lang::NullPointerException, ::com::sun::star::uno::RuntimeException);
        virtual void                        SAL_CALL addPropertyChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& _rxListener ) throw (::com::sun::star::uno::RuntimeException);
        virtual void                        SAL_CALL removePropertyChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& _rxListener ) throw (::com::sun::star::uno::RuntimeException);

        // PropertyHandler overridables
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >
                                            SAL_CALL doDescribeSupportedProperties() const;
        virtual void onNewComponent();

    private:
        bool    implPrepareRemoveCurrentDataType() SAL_THROW(());
        bool    implDoRemoveCurrentDataType() SAL_THROW(());

        bool    implPrepareCloneDataCurrentType( ::rtl::OUString& _rNewName ) SAL_THROW(());
        bool    implDoCloneCurrentDataType( const ::rtl::OUString& _rNewName ) SAL_THROW(());

        /** retrieves the names of the data types which our introspectee can be validated against
        */
        void    implGetAvailableDataTypeNames( ::std::vector< ::rtl::OUString >& /* [out] */ _rNames ) const SAL_THROW(());
    };

//........................................................................
} // namespace pcr
//........................................................................

#endif // EXTENSIONS_SOURCE_PROPCTRLR_XSDVALIDATIONPROPERTYHANDLER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
