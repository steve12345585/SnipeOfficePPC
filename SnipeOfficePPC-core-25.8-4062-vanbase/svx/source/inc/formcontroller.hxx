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
#ifndef _SVX_FMCTRLER_HXX
#define _SVX_FMCTRLER_HXX

#include "delayedevent.hxx"
#include "formdispatchinterceptor.hxx"
#include "sqlparserclient.hxx"

#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XFocusListener.hpp>
#include <com/sun/star/awt/XItemListener.hpp>
#include <com/sun/star/awt/XMouseListener.hpp>
#include <com/sun/star/awt/XTabController.hpp>
#include <com/sun/star/awt/XTextComponent.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/form/DatabaseDeleteEvent.hpp>
#include <com/sun/star/form/DatabaseParameterEvent.hpp>
#include <com/sun/star/form/ErrorEvent.hpp>
#include <com/sun/star/form/validation/XFormComponentValidityListener.hpp>
#include <com/sun/star/form/XConfirmDeleteBroadcaster.hpp>
#include <com/sun/star/form/XConfirmDeleteListener.hpp>
#include <com/sun/star/form/XDatabaseParameterBroadcaster2.hpp>
#include <com/sun/star/form/XDatabaseParameterListener.hpp>
#include <com/sun/star/form/runtime/XFormController.hpp>
#include <com/sun/star/form/runtime/XFilterController.hpp>
#include <com/sun/star/form/XFormControllerListener.hpp>
#include <com/sun/star/form/XGridControlListener.hpp>
#include <com/sun/star/form/XLoadListener.hpp>
#include <com/sun/star/form/XResetListener.hpp>
#include <com/sun/star/frame/DispatchDescriptor.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#include <com/sun/star/frame/XDispatchProviderInterceptor.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/script/XEventAttacherManager.hpp>
#include <com/sun/star/sdb/XRowSetApproveBroadcaster.hpp>
#include <com/sun/star/sdb/XRowSetApproveListener.hpp>
#include <com/sun/star/sdb/XSingleSelectQueryComposer.hpp>
#include <com/sun/star/sdb/XSQLErrorBroadcaster.hpp>
#include <com/sun/star/sdb/XSQLErrorListener.hpp>
#include <com/sun/star/sdbc/XRowSetListener.hpp>
#include <com/sun/star/task/XInteractionHandler.hpp>
#include <com/sun/star/util/XModeSelector.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <com/sun/star/util/XModifyListener.hpp>

#include <comphelper/broadcasthelper.hxx>
#include <comphelper/componentcontext.hxx>
#include <comphelper/proparrhlp.hxx>
#include <comphelper/stl_types.hxx>
#include <connectivity/sqlparse.hxx>
#include <cppuhelper/propshlp.hxx>
#include <vcl/timer.hxx>

#ifndef INCLUDED_COMPHELPER_IMPLBASE_VAR_HXX_22
#define INCLUDED_COMPHELPER_IMPLBASE_VAR_HXX_22
#define COMPHELPER_IMPLBASE_INTERFACE_NUMBER 22
#include <comphelper/implbase_var.hxx>
#endif

struct FmXTextComponentLess : public ::std::binary_function< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTextComponent >, ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTextComponent> , sal_Bool>
{
    sal_Bool operator() (const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTextComponent >& x, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTextComponent >& y) const
    {
        return reinterpret_cast<sal_Int64>(x.get()) < reinterpret_cast<sal_Int64>(y.get());
    }
};

typedef ::std::map< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTextComponent >, ::rtl::OUString, FmXTextComponentLess> FmFilterRow;
typedef ::std::vector< FmFilterRow > FmFilterRows;
typedef ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController > > FmFormControllers;

class Window;

namespace svxform
{
    typedef ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTextComponent > >    FilterComponents;
    class ControlBorderManager;
    struct FmFieldInfo;

    typedef ::comphelper::WeakComponentImplHelper22 <   ::com::sun::star::form::runtime::XFormController
                                                    ,   ::com::sun::star::form::runtime::XFilterController
                                                    ,   ::com::sun::star::awt::XFocusListener
                                                    ,   ::com::sun::star::form::XLoadListener
                                                    ,   ::com::sun::star::beans::XPropertyChangeListener
                                                    ,   ::com::sun::star::awt::XTextListener
                                                    ,   ::com::sun::star::awt::XItemListener
                                                    ,   ::com::sun::star::container::XContainerListener
                                                    ,   ::com::sun::star::util::XModifyListener
                                                    ,   ::com::sun::star::form::XConfirmDeleteListener
                                                    ,   ::com::sun::star::sdb::XSQLErrorListener
                                                    ,   ::com::sun::star::sdbc::XRowSetListener
                                                    ,   ::com::sun::star::sdb::XRowSetApproveListener
                                                    ,   ::com::sun::star::form::XDatabaseParameterListener
                                                    ,   ::com::sun::star::lang::XServiceInfo
                                                    ,   ::com::sun::star::form::XResetListener
                                                    ,   ::com::sun::star::frame::XDispatch
                                                    ,   ::com::sun::star::awt::XMouseListener
                                                    ,   ::com::sun::star::form::validation::XFormComponentValidityListener
                                                    ,   ::com::sun::star::task::XInteractionHandler
                                                    ,   ::com::sun::star::form::XGridControlListener
                                                    ,   ::com::sun::star::form::runtime::XFeatureInvalidation
                                                    >   FormController_BASE;

    //==================================================================
    // FormController
    //==================================================================
    class ColumnInfoCache;
    class SAL_DLLPRIVATE FormController :public ::comphelper::OBaseMutex
                                        ,public FormController_BASE
                                        ,public ::cppu::OPropertySetHelper
                                        ,public DispatchInterceptor
                                        ,public ::comphelper::OAggregationArrayUsageHelper< FormController >
                                        ,public ::svxform::OSQLParserClient
    {
        typedef ::std::map  <   sal_Int16,
                                ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >
                            >   DispatcherContainer;

        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation>              m_xAggregate;
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabController>            m_xTabController;
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>                  m_xActiveControl, m_xCurrentControl;
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess>        m_xModelAsIndex;
        ::com::sun::star::uno::Reference< ::com::sun::star::script::XEventAttacherManager>  m_xModelAsManager;
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>                m_xParent;
        ::comphelper::ComponentContext                                                      m_aContext;
        // Composer used for checking filter conditions
        ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XSingleSelectQueryComposer >       m_xComposer;
        ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >             m_xInteractionHandler;
        ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormControllerContext > m_xContext;

        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl> >   m_aControls;
        ::cppu::OInterfaceContainerHelper
                                    m_aActivateListeners,
                                    m_aModifyListeners,
                                    m_aErrorListeners,
                                    m_aDeleteListeners,
                                    m_aRowSetApproveListeners,
                                    m_aParameterListeners,
                                    m_aFilterListeners;

        FmFormControllers           m_aChildren;
        FilterComponents            m_aFilterComponents;
        FmFilterRows                m_aFilterRows;

        Timer                       m_aTabActivationTimer;
        Timer                       m_aFeatureInvalidationTimer;

        ::svxform::ControlBorderManager*
                                    m_pControlBorderManager;

        ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormOperations >
                                    m_xFormOperations;
        DispatcherContainer         m_aFeatureDispatchers;
        ::std::set< sal_Int16 >     m_aInvalidFeatures;     // for asynchronous feature invalidation

        ::rtl::OUString             m_aMode;

        ::svxform::DelayedEvent     m_aLoadEvent;
        ::svxform::DelayedEvent     m_aToggleEvent;
        ::svxform::DelayedEvent     m_aActivationEvent;
        ::svxform::DelayedEvent     m_aDeactivationEvent;

        ::std::auto_ptr< ColumnInfoCache >
                                    m_pColumnInfoCache;

        sal_Int32                   m_nCurrentFilterPosition;   // current level for filtering (or-criteria)

        sal_Bool                    m_bCurrentRecordModified    : 1;
        sal_Bool                    m_bCurrentRecordNew         : 1;
        sal_Bool                    m_bLocked                   : 1;
        sal_Bool                    m_bDBConnection             : 1;    // Focuslistener nur fuer Datenbankformulare
        sal_Bool                    m_bCycle                    : 1;
        sal_Bool                    m_bCanInsert                : 1;
        sal_Bool                    m_bCanUpdate                : 1;
        sal_Bool                    m_bCommitLock               : 1;    // lock the committing of controls see focusGained
        sal_Bool                    m_bModified                 : 1;    // ist der Inhalt eines Controls modifiziert ?
        sal_Bool                    m_bControlsSorted           : 1;
        sal_Bool                    m_bFiltering                : 1;
        sal_Bool                    m_bAttachEvents             : 1;
        sal_Bool                    m_bDetachEvents             : 1;
        bool                        m_bAttemptedHandlerCreation : 1;
        bool                        m_bSuspendFilterTextListening;          // no bit field, passed around as reference

        // as we want to intercept dispatches of _all_ controls we're responsible for, and an object implementing
        // the ::com::sun::star::frame::XDispatchProviderInterceptor interface can intercept only _one_ objects dispatches, we need a helper class
        DECLARE_STL_VECTOR(DispatchInterceptionMultiplexer*, Interceptors);
        Interceptors    m_aControlDispatchInterceptors;

    public:
        FormController( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & _rxORB );

    protected:
        ~FormController();

    // XInterface
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& type) throw ( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL acquire() throw ();
        virtual void SAL_CALL release() throw ();

    // XTypeProvider
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() throw(::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);

    // XDispatch
        virtual void SAL_CALL dispatch( const ::com::sun::star::util::URL& _rURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& _rArgs ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL addStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& _rxListener, const ::com::sun::star::util::URL& _rURL ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL removeStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& _rxListener, const ::com::sun::star::util::URL& _rURL ) throw (::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::container::XChild
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> SAL_CALL getParent(void) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL setParent(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& Parent) throw( ::com::sun::star::lang::NoSupportException, ::com::sun::star::uno::RuntimeException );

    // ::com::sun::star::lang::XEventListener
        virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source) throw( ::com::sun::star::uno::RuntimeException );

    // OComponentHelper
        virtual void SAL_CALL disposing();

    // OPropertySetHelper
        virtual sal_Bool SAL_CALL convertFastPropertyValue( ::com::sun::star::uno::Any & rConvertedValue, ::com::sun::star::uno::Any & rOldValue,
                                                sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue )
                    throw( ::com::sun::star::lang::IllegalArgumentException );

        virtual void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue ) throw( ::com::sun::star::uno::Exception );
        virtual void SAL_CALL getFastPropertyValue( ::com::sun::star::uno::Any& rValue, sal_Int32 nHandle ) const;

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo() throw( ::com::sun::star::uno::RuntimeException );
        virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

        using OPropertySetHelper::getFastPropertyValue;

        // XFilterController
        virtual ::sal_Int32 SAL_CALL getFilterComponents() throw (::com::sun::star::uno::RuntimeException);
        virtual ::sal_Int32 SAL_CALL getDisjunctiveTerms() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL addFilterControllerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFilterControllerListener >& _Listener ) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeFilterControllerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFilterControllerListener >& _Listener ) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL setPredicateExpression( ::sal_Int32 _Component, ::sal_Int32 _Term, const ::rtl::OUString& _PredicateExpression ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl > SAL_CALL getFilterComponent( ::sal_Int32 _Component ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::rtl::OUString > > SAL_CALL getPredicateExpressions() throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeDisjunctiveTerm( ::sal_Int32 _Term ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL appendEmptyDisjunctiveTerm() throw (::com::sun::star::uno::RuntimeException);
        virtual ::sal_Int32 SAL_CALL getActiveTerm() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setActiveTerm( ::sal_Int32 _ActiveTerm ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);

    // XElementAccess
        virtual ::com::sun::star::uno::Type SAL_CALL getElementType(void) throw( ::com::sun::star::uno::RuntimeException );
        virtual sal_Bool SAL_CALL hasElements(void) throw( ::com::sun::star::uno::RuntimeException );

    // ::com::sun::star::container::XEnumerationAccess
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration> SAL_CALL createEnumeration(void) throw( ::com::sun::star::uno::RuntimeException );

    // ::com::sun::star::container::XContainerListener
        virtual void SAL_CALL elementInserted(const ::com::sun::star::container::ContainerEvent& rEvent) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL elementReplaced(const ::com::sun::star::container::ContainerEvent& rEvent) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL elementRemoved(const ::com::sun::star::container::ContainerEvent& rEvent) throw( ::com::sun::star::uno::RuntimeException );

    // XLoadListener
        virtual void SAL_CALL loaded(const ::com::sun::star::lang::EventObject& rEvent) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL unloaded(const ::com::sun::star::lang::EventObject& rEvent) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL unloading(const ::com::sun::star::lang::EventObject& aEvent) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL reloading(const ::com::sun::star::lang::EventObject& aEvent) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL reloaded(const ::com::sun::star::lang::EventObject& aEvent) throw( ::com::sun::star::uno::RuntimeException );

    // XModeSelector
        virtual void SAL_CALL setMode(const ::rtl::OUString& Mode) throw( ::com::sun::star::lang::NoSupportException, ::com::sun::star::uno::RuntimeException );
        virtual ::rtl::OUString SAL_CALL getMode(void) throw( ::com::sun::star::uno::RuntimeException );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedModes(void) throw( ::com::sun::star::uno::RuntimeException );
        virtual sal_Bool SAL_CALL supportsMode(const ::rtl::OUString& Mode) throw( ::com::sun::star::uno::RuntimeException );

    // ::com::sun::star::container::XIndexAccess
        virtual sal_Int32 SAL_CALL getCount(void) throw( ::com::sun::star::uno::RuntimeException );
        virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 Index) throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException );

    // XModifyBroadcaster
        virtual void SAL_CALL addModifyListener(const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener>& l) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeModifyListener(const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener>& l) throw( ::com::sun::star::uno::RuntimeException );

    // XFocusListener
        virtual void SAL_CALL focusGained(const  ::com::sun::star::awt::FocusEvent& e) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL focusLost(const  ::com::sun::star::awt::FocusEvent& e) throw( ::com::sun::star::uno::RuntimeException );

    // XMouseListener
        virtual void SAL_CALL mousePressed( const ::com::sun::star::awt::MouseEvent& _rEvent ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL mouseReleased( const ::com::sun::star::awt::MouseEvent& _rEvent ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL mouseEntered( const ::com::sun::star::awt::MouseEvent& _rEvent ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL mouseExited( const ::com::sun::star::awt::MouseEvent& _rEvent ) throw (::com::sun::star::uno::RuntimeException);

    // XFormComponentValidityListener
        virtual void SAL_CALL componentValidityChanged( const ::com::sun::star::lang::EventObject& _rSource ) throw (::com::sun::star::uno::RuntimeException);

    // XInteractionHandler
        virtual void SAL_CALL handle( const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionRequest >& Request ) throw (::com::sun::star::uno::RuntimeException);

    // XGridControlListener
        virtual void SAL_CALL columnChanged( const ::com::sun::star::lang::EventObject& _event ) throw (::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::beans::XPropertyChangeListener -> aenderung der stati
        virtual void SAL_CALL propertyChange(const  ::com::sun::star::beans::PropertyChangeEvent& evt) throw( ::com::sun::star::uno::RuntimeException );

    // XTextListener           -> modify setzen
        virtual void SAL_CALL textChanged(const  ::com::sun::star::awt::TextEvent& rEvent) throw( ::com::sun::star::uno::RuntimeException );

    // XItemListener            -> modify setzen
        virtual void SAL_CALL itemStateChanged(const  ::com::sun::star::awt::ItemEvent& rEvent) throw( ::com::sun::star::uno::RuntimeException );

    // XModifyListener   -> modify setzen
        virtual void SAL_CALL modified(const ::com::sun::star::lang::EventObject& rEvent) throw( ::com::sun::star::uno::RuntimeException );

    // XFormController
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormOperations > SAL_CALL getFormOperations() throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl> SAL_CALL getCurrentControl(void) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL addActivateListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormControllerListener>& l) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeActivateListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormControllerListener>& l) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL addChildController( const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController >& _ChildController ) throw( ::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IllegalArgumentException );

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormControllerContext > SAL_CALL getContext() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setContext( const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormControllerContext >& _context ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler > SAL_CALL getInteractionHandler() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setInteractionHandler( const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >& _interactionHandler ) throw (::com::sun::star::uno::RuntimeException);

    // XTabController
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl> > SAL_CALL getControls(void) throw( ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL setModel(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabControllerModel>& Model) throw( ::com::sun::star::uno::RuntimeException );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabControllerModel> SAL_CALL getModel() throw( ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL setContainer(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer>& Container) throw( ::com::sun::star::uno::RuntimeException );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer> SAL_CALL getContainer() throw( ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL autoTabOrder() throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL activateTabOrder() throw( ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL activateFirst() throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL activateLast() throw( ::com::sun::star::uno::RuntimeException );

    // com::sun::star::sdbc::XRowSetListener
        virtual void SAL_CALL cursorMoved(const ::com::sun::star::lang::EventObject& event) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL rowChanged(const ::com::sun::star::lang::EventObject& event) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL rowSetChanged(const ::com::sun::star::lang::EventObject& event) throw( ::com::sun::star::uno::RuntimeException );

    // XRowSetApproveListener
        virtual sal_Bool SAL_CALL approveCursorMove(const ::com::sun::star::lang::EventObject& event) throw( ::com::sun::star::uno::RuntimeException );
        virtual sal_Bool SAL_CALL approveRowChange(const  ::com::sun::star::sdb::RowChangeEvent& event) throw( ::com::sun::star::uno::RuntimeException );
        virtual sal_Bool SAL_CALL approveRowSetChange(const ::com::sun::star::lang::EventObject& event) throw( ::com::sun::star::uno::RuntimeException );

    // XRowSetApproveBroadcaster
        virtual void SAL_CALL addRowSetApproveListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XRowSetApproveListener>& listener) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeRowSetApproveListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XRowSetApproveListener>& listener) throw( ::com::sun::star::uno::RuntimeException );

    // XSQLErrorBroadcaster
        virtual void SAL_CALL errorOccured(const ::com::sun::star::sdb::SQLErrorEvent& aEvent) throw( ::com::sun::star::uno::RuntimeException );

    // XSQLErrorListener
        virtual void SAL_CALL addSQLErrorListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XSQLErrorListener>& _rListener) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeSQLErrorListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XSQLErrorListener>& _rListener) throw( ::com::sun::star::uno::RuntimeException );

    // XDatabaseParameterBroadcaster2
        virtual void SAL_CALL addDatabaseParameterListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XDatabaseParameterListener>& aListener) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeDatabaseParameterListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XDatabaseParameterListener>& aListener) throw( ::com::sun::star::uno::RuntimeException );

    // XDatabaseParameterBroadcaster
        virtual void SAL_CALL addParameterListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XDatabaseParameterListener>& aListener) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeParameterListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XDatabaseParameterListener>& aListener) throw( ::com::sun::star::uno::RuntimeException );

    // XDatabaseParameterListener
        virtual sal_Bool SAL_CALL approveParameter(const ::com::sun::star::form::DatabaseParameterEvent& aEvent) throw( ::com::sun::star::uno::RuntimeException );

    // XConfirmDeleteBroadcaster
        virtual void SAL_CALL addConfirmDeleteListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XConfirmDeleteListener>& aListener) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeConfirmDeleteListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XConfirmDeleteListener>& aListener) throw( ::com::sun::star::uno::RuntimeException );

    // XConfirmDeleteListener
        virtual sal_Bool SAL_CALL confirmDelete(const  ::com::sun::star::sdb::RowChangeEvent& aEvent) throw( ::com::sun::star::uno::RuntimeException );

    // XServiceInfo
        virtual sal_Bool SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw(::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getImplementationName() throw(::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >  SAL_CALL getSupportedServiceNames(void) throw(::com::sun::star::uno::RuntimeException);

    // XResetListener
        virtual sal_Bool SAL_CALL approveReset(const ::com::sun::star::lang::EventObject& rEvent) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL resetted(const ::com::sun::star::lang::EventObject& rEvent) throw( ::com::sun::star::uno::RuntimeException );

        // XFeatureInvalidation
        virtual void SAL_CALL invalidateFeatures( const ::com::sun::star::uno::Sequence< ::sal_Int16 >& Features ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL invalidateAllFeatures(  ) throw (::com::sun::star::uno::RuntimeException);

// method for registration
        static  ::com::sun::star::uno::Sequence< ::rtl::OUString >  getSupportedServiceNames_Static(void);

        // comphelper::OPropertyArrayUsageHelper
        virtual void fillProperties(
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps,
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rAggregateProps
            ) const;

    protected:
        // DispatchInterceptor
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch>
            interceptedQueryDispatch(
                    const ::com::sun::star::util::URL& aURL,
                    const ::rtl::OUString& aTargetFrameName,
                    sal_Int32 nSearchFlags
                )   throw( ::com::sun::star::uno::RuntimeException );

        virtual ::osl::Mutex* getInterceptorMutex() { return &m_aMutex; }

        /// update all our dispatchers
        void    updateAllDispatchers() const;

        /** disposes all dispatchers in m_aFeatureDispatchers, and empties m_aFeatureDispatchers
        */
        void    disposeAllFeaturesAndDispatchers() SAL_THROW(());

        void startFiltering();
        void stopFiltering();
        void setFilter(::std::vector<FmFieldInfo>&);
        void startListening();
        void stopListening();

        /** ensures that we have an interaction handler, if possible

            If an interaction handler was provided at creation time (<member>initialize</member>), this
            one will be used. Else, an attempt is made to create an <type scope="com::sun::star::sdb">InteractionHandler</type>
            is made.

            @return <TRUE/>
                if and only if <member>m_xInteractionHandler</member> is valid when the method returns
        */
        bool ensureInteractionHandler();

        /** replaces one of our controls with another one

            Upon successful replacing, the old control will be disposed. Also, internal members pointing
            to the current or active control will be adjusted. Yet more, if the replaced control was
            the active control, the new control will be made active.

            @param _rxExistentControl
                The control to replace. Must be one of the controls in our ControlContainer.
            @param _rxNewControl
                The control which should replace the existent control.
            @return
                <TRUE/> if and only if the control was successfully replaced
        */
        bool    replaceControl(
            const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >& _rxExistentControl,
            const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >& _rxNewControl
        );

        // we're listening at all bound controls for modifications
        void startControlModifyListening(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& xControl);
        void stopControlModifyListening(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& xControl);

        void setLocks();
        void setControlLock(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& xControl);
        void addToEventAttacher(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& xControl);
        void removeFromEventAttacher(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& xControl);
        void toggleAutoFields(sal_Bool bAutoFields);
        void unload() throw( ::com::sun::star::uno::RuntimeException );
        void removeBoundFieldListener();

        void startFormListening( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxForm, sal_Bool _bPropertiesOnly  );
        void stopFormListening( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxForm, sal_Bool _bPropertiesOnly );

        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl> findControl( ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl> >& rCtrls, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel>& rxCtrlModel, sal_Bool _bRemove, sal_Bool _bOverWrite ) const;

        void insertControl(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& xControl);
        void removeControl(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& xControl);

        /// called when a new control is to be handled by the controller
        void implControlInserted( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& _rxControl, bool _bAddToEventAttacher );
        /// called when a control is not to be handled by the controller anymore
        void implControlRemoved( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& _rxControl, bool _bRemoveFromEventAttacher );

        /** sets m_xCurrentControl, plus does administrative tasks depending on it
        */
        void    implSetCurrentControl( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >& _rxControl );

        /** invalidates the FormFeatures which depend on the current control
        */
        void    implInvalidateCurrentControlDependentFeatures();

        bool    impl_isDisposed_nofail() const { return FormController_BASE::rBHelper.bDisposed; }
        void    impl_checkDisposed_throw() const;

        void    impl_onModify();

        /** adds the given filter row to m_aFilterRows, setting m_nCurrentFilterPosition to 0 if the newly added
            row is the first one.

            @precond
                our mutex is locked
        */
        void    impl_addFilterRow( const FmFilterRow& _row );

        /** adds an empty filter row to m_aFilterRows, and notifies our listeners
        */
        void    impl_appendEmptyFilterRow( ::osl::ClearableMutexGuard& _rClearBeforeNotify );

        sal_Bool isLocked() const {return m_bLocked;}
        sal_Bool determineLockState() const;

        Window* getDialogParentWindow();
            // returns the window which should be used as parent window for dialogs

        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterceptor>    createInterceptor(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterception>& _xInterception);
            // create a new interceptor, register it on the given object
        void                            deleteInterceptor(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterception>& _xInterception);
            // if createInterceptor was called for the given object the according interceptor will be removed
            // from the objects interceptor chain and released

        /** checks all form controls belonging to our form for validity

            If a form control supports the XValidatableFormComponent interface, this is used to determine
            the validity of the control. If the interface is not supported, the control is supposed to be
            valid.

            @param _rFirstInvalidityExplanation
                if the method returns <FALSE/> (i.e. if there is an invalid control), this string contains
                the explanation for the invalidity, as obtained from the validator.

            @param _rxFirstInvalidModel
                if the method returns <FALSE/> (i.e. if there is an invalid control), this contains
                the control model

            @return
                <TRUE/> if and only if all controls belonging to our form are valid
        */
        bool    checkFormComponentValidity(
                    ::rtl::OUString& /* [out] */ _rFirstInvalidityExplanation,
                    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& /* [out] */ _rxFirstInvalidModel
                ) SAL_THROW(());

        /** locates the control which belongs to a given model
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >
                locateControl( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& _rxModel ) SAL_THROW(());

        // set the text for all filters
        void impl_setTextOnAllFilter_throw();

        // in filter mode we do not listen for changes
        sal_Bool isListeningForChanges() const {return m_bDBConnection && !m_bFiltering && !isLocked();}
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl> isInList(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer>& xPeer) const;

        DECL_LINK( OnActivateTabOrder, void* );
        DECL_LINK( OnInvalidateFeatures, void* );
        DECL_LINK( OnLoad, void* );
        DECL_LINK( OnToggleAutoFields, void* );
        DECL_LINK( OnActivated, void* );
        DECL_LINK( OnDeactivated, void* );
    };

}   // namespace svxform

#endif  // _SVX_FMCTRLER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
