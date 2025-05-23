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

#ifndef SCRIPTING_DLGEVTATT_HXX
#define SCRIPTING_DLGEVTATT_HXX

#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/script/XAllListener.hpp>
#include <com/sun/star/script/XEventAttacher.hpp>
#include <com/sun/star/script/XScriptEventsAttacher.hpp>
#include <com/sun/star/script/XScriptListener.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/beans/XIntrospectionAccess.hpp>
#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/script/XScriptEventsSupplier.hpp>

#include <boost/unordered_map.hpp>

//.........................................................................
namespace dlgprov
{
//.........................................................................

    // =============================================================================
    // class DialogEventsAttacherImpl
    // =============================================================================
    typedef boost::unordered_map< rtl::OUString,
        ::com::sun::star::uno::Reference< ::com::sun::star::script::XScriptListener >,
        ::rtl::OUStringHash,
        ::std::equal_to< ::rtl::OUString > > ListenerHash;

    typedef ::cppu::WeakImplHelper1<
        ::com::sun::star::script::XScriptEventsAttacher > DialogEventsAttacherImpl_BASE;


    class DialogEventsAttacherImpl : public DialogEventsAttacherImpl_BASE
    {
    private:
        bool mbUseFakeVBAEvents;
        ListenerHash listernersForTypes;
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
        ::com::sun::star::uno::Reference< ::com::sun::star::script::XEventAttacher > m_xEventAttacher;
        ::com::sun::star::uno::Reference< ::com::sun::star::script::XScriptListener > getScriptListenerForKey( const rtl::OUString& sScriptName ) throw ( ::com::sun::star::uno::RuntimeException );
        ::com::sun::star::uno::Reference< ::com::sun::star::script::XScriptEventsSupplier > getFakeVbaEventsSupplier( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& xControl, rtl::OUString& sCodeName );
        void nestedAttachEvents( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > >& Objects, const ::com::sun::star::uno::Any& Helper, rtl::OUString& sDialogCodeName );
        void  SAL_CALL attachEventsToControl( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& xControl, const ::com::sun::star::uno::Reference< ::com::sun::star::script::XScriptEventsSupplier >& events, const ::com::sun::star::uno::Any& Helper  );
    public:
        DialogEventsAttacherImpl( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
             const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel,
             const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >& xControl,
             const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xHandler,
             const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XIntrospectionAccess >& xIntrospect,
             bool bProviderMode,
             const ::com::sun::star::uno::Reference< ::com::sun::star::script::XScriptListener >& xRTLListener ,const rtl::OUString& sDialogLibName );
        virtual ~DialogEventsAttacherImpl();

        // XScriptEventsAttacher
        virtual void SAL_CALL attachEvents( const ::com::sun::star::uno::Sequence<
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > >& Objects,
            const com::sun::star::uno::Reference<com::sun::star::script::XScriptListener>&,
            const ::com::sun::star::uno::Any& Helper )
            throw (::com::sun::star::lang::IllegalArgumentException,
                   ::com::sun::star::beans::IntrospectionException,
                   ::com::sun::star::script::CannotCreateAdapterException,
                   ::com::sun::star::lang::ServiceNotRegisteredException,
                   ::com::sun::star::uno::RuntimeException);
    };


    // =============================================================================
    // class DialogAllListenerImpl
    // =============================================================================

    typedef ::cppu::WeakImplHelper1<
        ::com::sun::star::script::XAllListener > DialogAllListenerImpl_BASE;


    class DialogAllListenerImpl : public DialogAllListenerImpl_BASE
    {
    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::script::XScriptListener > m_xScriptListener;
        ::rtl::OUString m_sScriptType;
        ::rtl::OUString m_sScriptCode;

        virtual void firing_impl( const ::com::sun::star::script::AllEventObject& Event, ::com::sun::star::uno::Any* pRet );

    public:
        DialogAllListenerImpl( const ::com::sun::star::uno::Reference< ::com::sun::star::script::XScriptListener >& rxListener,
            const ::rtl::OUString& rScriptType, const ::rtl::OUString& rScriptCode );
        virtual ~DialogAllListenerImpl();

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source )
            throw (::com::sun::star::uno::RuntimeException);

        // XAllListener
        virtual void SAL_CALL firing( const ::com::sun::star::script::AllEventObject& Event )
            throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Any SAL_CALL approveFiring( const ::com::sun::star::script::AllEventObject& Event )
            throw (::com::sun::star::reflection::InvocationTargetException, ::com::sun::star::uno::RuntimeException);
    };


    // =============================================================================
    // class DialogScriptListenerImpl
    // =============================================================================

    typedef ::cppu::WeakImplHelper1<
        ::com::sun::star::script::XScriptListener > DialogScriptListenerImpl_BASE;


    class DialogScriptListenerImpl : public DialogScriptListenerImpl_BASE
    {
    protected:
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;
        virtual void firing_impl( const ::com::sun::star::script::ScriptEvent& aScriptEvent, ::com::sun::star::uno::Any* pRet ) = 0;
    public:
        DialogScriptListenerImpl( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext ) : m_xContext( rxContext ) {}
        virtual ~DialogScriptListenerImpl();

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source )
            throw (::com::sun::star::uno::RuntimeException);

        // XScriptListener
        virtual void SAL_CALL firing( const ::com::sun::star::script::ScriptEvent& aScriptEvent )
            throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Any SAL_CALL approveFiring( const ::com::sun::star::script::ScriptEvent& aScriptEvent )
            throw (::com::sun::star::reflection::InvocationTargetException, ::com::sun::star::uno::RuntimeException);
    };


//.........................................................................
}   // namespace dlgprov
//.........................................................................

#endif // SCRIPTING_DLGEVT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
