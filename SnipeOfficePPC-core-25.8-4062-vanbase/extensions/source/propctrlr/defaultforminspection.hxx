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
#ifndef EXTENSIONS_SOURCE_PROPCTRLR_DEFAULTFORMINSPECTION_HXX
#define EXTENSIONS_SOURCE_PROPCTRLR_DEFAULTFORMINSPECTION_HXX

#include "inspectormodelbase.hxx"

#include <memory>

//........................................................................
namespace pcr
{
//........................................................................

    class OPropertyInfoService;
    //====================================================================
    //= DefaultFormComponentInspectorModel
    //====================================================================
    class DefaultFormComponentInspectorModel : public ImplInspectorModel
    {
    private:
        bool                                        m_bUseFormComponentHandlers;
        bool                                        m_bConstructed;

        /// access to property meta data
        SAL_WNODEPRECATED_DECLARATIONS_PUSH
        ::std::auto_ptr< OPropertyInfoService >     m_pInfoService;
        SAL_WNODEPRECATED_DECLARATIONS_POP

    protected:
        ~DefaultFormComponentInspectorModel();

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

        // XObjectInspectorModel
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getHandlerFactories() throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::inspection::PropertyCategoryDescriptor > SAL_CALL describeCategories(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::sal_Int32 SAL_CALL getPropertyOrderIndex( const ::rtl::OUString& PropertyName ) throw (::com::sun::star::uno::RuntimeException);

        // XInitialization
        virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    public:
        // XServiceInfo - static versions
        static ::rtl::OUString getImplementationName_static(  ) throw(::com::sun::star::uno::RuntimeException);
        static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_static(  ) throw(::com::sun::star::uno::RuntimeException);
        static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
                        Create(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >&);

    public:
        DefaultFormComponentInspectorModel( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& _rxContext, bool _bUseFormFormComponentHandlers = true );

    protected:
        // Service constructors
        void    createDefault();
        void    createWithHelpSection( sal_Int32 _nMinHelpTextLines, sal_Int32 _nMaxHelpTextLines );
    };

//........................................................................
} // namespace pcr
//........................................................................

#endif // EXTENSIONS_SOURCE_PROPCTRLR_DEFAULTFORMINSPECTION_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
