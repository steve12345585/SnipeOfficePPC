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
#ifndef FORMS_SOURCE_RICHTEXT_RICHTEXTMODEL_HXX
#define FORMS_SOURCE_RICHTEXT_RICHTEXTMODEL_HXX

#include "FormComponent.hxx"
#include "frm_module.hxx"
#include "formcontrolfont.hxx"
#include "richtextunowrapper.hxx"
#include <comphelper/propertycontainerhelper.hxx>
#include <comphelper/listenernotification.hxx>

#include <com/sun/star/awt/XDevice.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <cppuhelper/implbase3.hxx>
#include <tools/link.hxx>
#include <memory>

class EditEngine;
//........................................................................
namespace frm
{
//........................................................................

    class RichTextEngine;
    //====================================================================
    //= ORichTextModel
    //====================================================================
    typedef ::cppu::ImplHelper3 <   ::com::sun::star::awt::XControlModel
                                ,   ::com::sun::star::lang::XUnoTunnel
                                ,   ::com::sun::star::util::XModifyBroadcaster
                                >   ORichTextModel_BASE;

    class ORichTextModel
            :public OControlModel
            ,public FontControlModel
            ,public IEngineTextChangeListener
            ,public ::comphelper::OPropertyContainerHelper
            ,public ORichTextModel_BASE
    {
    private:
        // <properties>
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XDevice >
                                    m_xReferenceDevice;
        ::com::sun::star::uno::Any  m_aTabStop;
        ::com::sun::star::uno::Any  m_aBackgroundColor;
        ::com::sun::star::uno::Any  m_aBorderColor;
        ::com::sun::star::uno::Any  m_aVerticalAlignment;
        ::rtl::OUString             m_sDefaultControl;
        ::rtl::OUString             m_sHelpText;
        ::rtl::OUString             m_sHelpURL;
        ::rtl::OUString             m_sLastKnownEngineText;
        sal_Int16                   m_nLineEndFormat;
        sal_Int16                   m_nTextWritingMode;
        sal_Int16                   m_nContextWritingMode;
        sal_Int16                   m_nBorder;
        sal_Bool                    m_bEnabled;
        sal_Bool                    m_bEnableVisible;
        sal_Bool                    m_bHardLineBreaks;
        sal_Bool                    m_bHScroll;
        sal_Bool                    m_bVScroll;
        sal_Bool                    m_bReadonly;
        sal_Bool                    m_bPrintable;
        sal_Bool                    m_bReallyActAsRichText; // despite the class name, the RichTextControl later on
                                                            // will create "ordinary" text peers depending on this property
        sal_Bool                    m_bHideInactiveSelection;
        // </properties>

        // <properties_for_awt_edit_compatibility>
        ::com::sun::star::uno::Any  m_aAlign;
        sal_Int16                   m_nEchoChar;
        sal_Int16                   m_nMaxTextLength;
        sal_Bool                    m_bMultiLine;
        // </properties_for_awt_edit_compatibility>

        ::std::auto_ptr<RichTextEngine>
                                    m_pEngine;
        bool                        m_bSettingEngineText;

        ::cppu::OInterfaceContainerHelper
                                    m_aModifyListeners;

    public:
        static  RichTextEngine* getEditEngine( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& _rxModel );

    protected:
        DECLARE_DEFAULT_LEAF_XTOR( ORichTextModel );

        // UNO
        DECLARE_UNO3_AGG_DEFAULTS( ORichTextModel, OControlModel );
        virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& _rType ) throw (::com::sun::star::uno::RuntimeException);

        // XServiceInfo
        DECLARE_SERVICE_REGISTRATION( ORichTextModel )

        // XPersistObject
        DECLARE_XPERSISTOBJECT()

        // XTypeProvider
        DECLARE_XTYPEPROVIDER()

        // XCloneable
        DECLARE_XCLONEABLE();

        // XUnoTunnel
        virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw (::com::sun::star::uno::RuntimeException);

        // XModifyBroadcaster
        virtual void SAL_CALL addModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL removeModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);

        // XPropertySet and friends
        virtual void SAL_CALL getFastPropertyValue(::com::sun::star::uno::Any& rValue, sal_Int32 nHandle ) const;
        virtual sal_Bool SAL_CALL convertFastPropertyValue(::com::sun::star::uno::Any& rConvertedValue, ::com::sun::star::uno::Any& rOldValue,
                                            sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue )
                                        throw(::com::sun::star::lang::IllegalArgumentException);
        virtual void SAL_CALL setFastPropertyValue_NoBroadcast(sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue) throw ( ::com::sun::star::uno::Exception);
        virtual ::com::sun::star::uno::Any getPropertyDefaultByHandle( sal_Int32 nHandle ) const;

        // OControlModel's property handling
        virtual void describeFixedProperties(
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps
        ) const;
        virtual void describeAggregateProperties(
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rAggregateProps
        ) const;

        // prevent method hiding
        using OControlModel::disposing;
        using OControlModel::getFastPropertyValue;

        // OComponentHelper
        virtual void SAL_CALL disposing();

        // IEngineTextChangeListener
        virtual void potentialTextChange( );

    private:
        void    implInit();
        void    implDoAggregation();
        void    implRegisterProperties();

        /** propagates a new text to the EditEngine

            This method needs to lock the global solar mutex, so our own mutex must not
            be locked when calling.

        @precond
            our mutex is not locked
        */
        void    impl_smlock_setEngineText( const ::rtl::OUString& _rText );

        DECL_LINK( OnEngineContentModified, void* );

        static  ::com::sun::star::uno::Sequence< sal_Int8 > getEditEngineTunnelId();

    private:
        ORichTextModel();                                   // never implemented
        ORichTextModel( const ORichTextModel& );            // never implemented
        ORichTextModel& operator=( const ORichTextModel& ); // never implemented
    };

//........................................................................
} // namespace frm
//........................................................................

#endif // FORMS_SOURCE_RICHTEXT_RICHTEXTMODEL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
