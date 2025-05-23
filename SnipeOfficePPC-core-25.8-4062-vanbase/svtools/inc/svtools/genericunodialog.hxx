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

#ifndef _SVT_GENERICUNODIALOG_HXX_
#define _SVT_GENERICUNODIALOG_HXX_

#include "svtools/svtdllapi.h"

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/lang/NotInitializedException.hpp>

#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/propshlp.hxx>
#include <comphelper/proparrhlp.hxx>
#include <comphelper/uno3.hxx>
#include <comphelper/propertycontainer.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <comphelper/componentcontext.hxx>
#include <tools/link.hxx>

class Dialog;
class Window;
class VclWindowEvent;

//.........................................................................
namespace svt
{
//.........................................................................

    //=========================================================================
#define     UNODIALOG_PROPERTY_ID_TITLE     1
#define     UNODIALOG_PROPERTY_ID_PARENT    2

#define     UNODIALOG_PROPERTY_TITLE        "Title"
#define     UNODIALOG_PROPERTY_PARENT       "ParentWindow"


    //=========================================================================
    typedef ::cppu::WeakImplHelper3 <   com::sun::star::ui::dialogs::XExecutableDialog
                                    ,   com::sun::star::lang::XServiceInfo
                                    ,   com::sun::star::lang::XInitialization
                                    >   OGenericUnoDialogBase;

    /** abstract base class for implementing UNO objects representing dialogs (<type scope="com.sun.star.awt">XDialog</type>)
    */
    class SVT_DLLPUBLIC OGenericUnoDialog
            :public OGenericUnoDialogBase
            ,public ::comphelper::OMutexAndBroadcastHelper
            ,public ::comphelper::OPropertyContainer
    {
    private:
        ::osl::Mutex                    m_aExecutionMutex;  /// acess safety for execute/cancel

    protected:
        Dialog*                     m_pDialog;                  /// the dialog to execute
        sal_Bool                    m_bExecuting : 1;           /// we're currently executing the dialog
        sal_Bool                    m_bCanceled : 1;            /// endDialog was called while we were executing
        sal_Bool                    m_bTitleAmbiguous : 1;      /// m_sTitle has not been set yet
        bool                        m_bInitialized : 1;         /// has "initialize" been called?
        bool                        m_bNeedInitialization : 1;  /// do we need to be initialized before any other API call is allowed?

        // <properties>
        ::rtl::OUString                                                 m_sTitle;   /// title of the dialog
        com::sun::star::uno::Reference<com::sun::star::awt::XWindow>    m_xParent;  /// parent window
        // </properties>

        ::comphelper::ComponentContext m_aContext;

    public:
        inline bool needInitialization() const { return m_bNeedInitialization && !m_bInitialized; }

    protected:
        OGenericUnoDialog(const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& _rxORB);
        OGenericUnoDialog(const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& _rxContext);
        virtual ~OGenericUnoDialog();

    public:
        // UNO
        DECLARE_UNO3_DEFAULTS(OGenericUnoDialog, OGenericUnoDialogBase);
        virtual com::sun::star::uno::Any SAL_CALL queryInterface(const com::sun::star::uno::Type& _rType) throw (com::sun::star::uno::RuntimeException);

        // XTypeProvider
        virtual com::sun::star::uno::Sequence<com::sun::star::uno::Type> SAL_CALL getTypes(  ) throw(com::sun::star::uno::RuntimeException);
        virtual com::sun::star::uno::Sequence<sal_Int8> SAL_CALL getImplementationId(  ) throw(com::sun::star::uno::RuntimeException) = 0;

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName() throw(com::sun::star::uno::RuntimeException) = 0;
        virtual sal_Bool SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw(com::sun::star::uno::RuntimeException);
        virtual ::comphelper::StringSequence SAL_CALL getSupportedServiceNames() throw(com::sun::star::uno::RuntimeException) = 0;

        // OPropertySetHelper
        virtual void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const com::sun::star::uno::Any& rValue ) throw(com::sun::star::uno::Exception);
        virtual sal_Bool SAL_CALL convertFastPropertyValue( com::sun::star::uno::Any& rConvertedValue, com::sun::star::uno::Any& rOldValue, sal_Int32 nHandle, const com::sun::star::uno::Any& rValue) throw(com::sun::star::lang::IllegalArgumentException);

        // XExecutableDialog
        virtual void SAL_CALL setTitle( const ::rtl::OUString& aTitle ) throw(::com::sun::star::uno::RuntimeException);
        virtual sal_Int16 SAL_CALL execute(  ) throw(::com::sun::star::uno::RuntimeException);

        // XInitialization
        virtual void SAL_CALL initialize( const com::sun::star::uno::Sequence< com::sun::star::uno::Any >& aArguments ) throw(com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);

    protected:
        /** create the concret dialog instance. note that m_aMutex is not locked when this method get's called,
            but the application-wide solar mutex is (to guard the not thread-safe ctor of the dialog).
            @param      pParent     the parent window for the new dialog
        */
        virtual Dialog* createDialog(Window* _pParent) = 0;

        /// called to destroy the dialog used. the default implementation just deletes m_pDialog and resets it to NULL
        virtual void destroyDialog();

        /** called after the dialog has been executed
            @param      _nExecutionResult       the execution result as returned by Dialog::Execute
        */
        virtual void executedDialog(sal_Int16 /*_nExecutionResult*/) { }

        /** smaller form of <method>initialize</method>.<p/>
            The <method>initialize</method> method is called with a sequence of <type scope="com.sun.star.uno">Any</type>'s,
            which is split up into the single elements, which are passed to implInitialize. The default implementation
            tries to exract an <type scope="com.sun.star.beans">PropertyValue</type> from the value an pass it to the
            <type scope="com.sun.star.beans">XPropertySet</type> interface of the object.
        */
        virtual void implInitialize(const com::sun::star::uno::Any& _rValue);

    private:
        DECL_LINK( OnDialogDying, VclWindowEvent* );

        /** ensures that m_pDialog is not <NULL/>

            This method does nothing if m_pDialog is already non-<NULL/>. Else, it calls createDialog and does
            all necessary initializations of the new dialog instance.

            @precond
                m_aMutex is locked

            @return
                <TRUE/> if and only if m_pDialog is non-<NULL/> upon returning from the method. Note that the only
                case where m_pDialog is <NULL/> is when createDialog returned <NULL/>, which is will fire an assertion
                in non-product builds.
        */
        bool    impl_ensureDialog_lck();
    };

    /// helper class for guarding access to methods of a OGenericUnoDialog
    class UnoDialogEntryGuard
    {
    public:
        UnoDialogEntryGuard( OGenericUnoDialog& _rDialog )
            :m_aGuard( _rDialog.GetMutex() )
        {
            if ( _rDialog.needInitialization() )
                throw ::com::sun::star::lang::NotInitializedException();
        }

    private:
        ::osl::MutexGuard   m_aGuard;
    };

//.........................................................................
}   // namespace svt
//.........................................................................

#endif // _SVT_GENERICUNODIALOG_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
