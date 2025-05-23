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

#ifndef _FORMS_FORMCOMPONENT_HXX_
#define _FORMS_FORMCOMPONENT_HXX_

#include "cloneable.hxx"
#include "ids.hxx"
#include "property.hrc"
#include "property.hxx"
#include "propertybaghelper.hxx"
#include "resettable.hxx"
#include "services.hxx"
#include "windowstateguard.hxx"

#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/beans/XPropertyAccess.hpp>
#include <com/sun/star/beans/XPropertyContainer.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/form/binding/XBindableValue.hpp>
#include <com/sun/star/form/FormComponentType.hpp>
#include <com/sun/star/form/validation/XValidatableFormComponent.hpp>
#include <com/sun/star/form/validation/XValidityConstraintListener.hpp>
#include <com/sun/star/form/XBoundComponent.hpp>
#include <com/sun/star/form/XBoundControl.hpp>
#include <com/sun/star/form/XFormComponent.hpp>
#include <com/sun/star/form/XLoadListener.hpp>
#include <com/sun/star/form/XReset.hpp>
#include <com/sun/star/io/XMarkableStream.hpp>
#include <com/sun/star/io/XPersistObject.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/sdb/XColumn.hpp>
#include <com/sun/star/sdb/XColumnUpdate.hpp>
#include <com/sun/star/sdb/XRowSetChangeListener.hpp>
#include <com/sun/star/sdbc/XRowSet.hpp>
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#include <com/sun/star/uno/XAggregation.hpp>
#include <com/sun/star/util/XCloneable.hpp>
#include <com/sun/star/util/XModifyListener.hpp>
#include <com/sun/star/form/XLoadable.hpp>

#include <comphelper/componentcontext.hxx>
#include <comphelper/propagg.hxx>
#include <comphelper/propertybag.hxx>
#include <comphelper/propmultiplex.hxx>
#include <comphelper/sequence.hxx>
#include <comphelper/uno3.hxx>
#include <cppuhelper/component.hxx>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase7.hxx>
#include <osl/mutex.hxx>
#include <rtl/ustring.hxx>

#include <memory>

//.........................................................................
namespace frm
{
//.........................................................................

    // default tab index for components
    const sal_Int16 FRM_DEFAULT_TABINDEX = 0;

    // macros for quickly declaring/implementing XServiceInfo
    #define DECLARE_XPERSISTOBJECT() \
    virtual ::rtl::OUString SAL_CALL getServiceName() throw(::com::sun::star::uno::RuntimeException);    \
    virtual void SAL_CALL write(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream>& _rxOutStream) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);    \
    virtual void SAL_CALL read(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream>& _rxInStream) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);   \

    // old macro for quickly implementing XServiceInfo::getImplementationName
    #define IMPLEMENTATION_NAME(ImplName)                                       \
    virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException) \
        { return ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.forms.") ) + ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(#ImplName)); }

    class OControlModel;

    //=========================================================================
    //= ControlModelLock
    //=========================================================================
    /** class whose instances lock a OControlModel

        Locking here merely means locking the OControlModel's mutex.

        In addition to the locking facility, the class is also able to fire property
        change notifications. This happens when the last ControlModelLock instance on a stack
        dies.
    */
    class ControlModelLock
    {
    public:
        ControlModelLock( OControlModel& _rModel )
            :m_rModel( _rModel )
            ,m_bLocked( false )
        {
            acquire();
        }

        ~ControlModelLock()
        {
            if ( m_bLocked )
                release();
        }
        inline void acquire();
        inline void release();

        inline OControlModel& getModel() const { return m_rModel; };

        /** adds a property change notification, which is to be fired when the last lock on the model
            (in the current thread) is released.
        */
        void    addPropertyNotification(
                    const sal_Int32 _nHandle,
                    const ::com::sun::star::uno::Any& _rOldValue,
                    const ::com::sun::star::uno::Any& _rNewValue
                );

    private:
        void    impl_notifyAll_nothrow();

    private:
        OControlModel&                                                  m_rModel;
        bool                                                            m_bLocked;
        ::com::sun::star::uno::Sequence< sal_Int32 >                    m_aHandles;
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >   m_aOldValues;
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >   m_aNewValues;

    private:
        ControlModelLock();                                     // never implemented
        ControlModelLock( const ControlModelLock& );            // never implemented
        ControlModelLock& operator=( const ControlModelLock& ); // never implemented
    };

//=========================================================================
//= OControl
//= base class for form layer controls
//=========================================================================
typedef ::cppu::ImplHelper3 <   ::com::sun::star::awt::XControl
                            ,   ::com::sun::star::lang::XEventListener
                            ,   ::com::sun::star::lang::XServiceInfo
                            > OControl_BASE;

class OControl  :public ::cppu::OComponentHelper
                ,public OControl_BASE
{
protected:
    ::osl::Mutex                                m_aMutex;
    OImplementationIdsRef                       m_aHoldIdHelper;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >
                                                m_xControl;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation>
                                                m_xAggregate;

    ::comphelper::ComponentContext              m_aContext;
    WindowStateGuard                            m_aWindowStateGuard;

public:
    /** constructs a control

        @param _rFactory
            the service factory for this control
        @param _rAggregateService
            the service name of the component to aggregate
        @param _bSetDelegator
            set this to <FALSE/> if you don't want the constructor to set the delegator at
            the aggregate. In this case, you <em>have</em> to call doSetDelegator within your
            own constructor.

            This is helpfull, if your derived class wants to cache an interface of the aggregate.
            In this case, the aggregate needs to be queried for this interface <b>before</b> the
            <member scope="com::sun::star::uno">XAggregation::setDelegator</member> call.

            In such a case, pass <FALSE/> to this parameter. Then, cache the aggregate's interface(s)
            as needed. Afterwards, call <member>doSetDelegator</member>.

            In your destructor, you need to call <member>doResetDelegator</member> before
            resetting the cached interfaces. This will reset the aggregates delegator to <NULL/>,
            which will ensure that the <member scope="com::sun::star::uno">XInterface::release</member>
            calls on the cached interfaces are really applied to the aggregate, instead of
            the <type>OControl</type> itself.
    */
    OControl(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rFactory,
        const ::rtl::OUString& _rAggregateService,
        const sal_Bool _bSetDelegator = sal_True
    );

    /** initializes the given peer with various settings necessary for form controls
    */
    static  void    initFormControlPeer(
        const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >& _rxPeer );

protected:
    virtual ~OControl();

    /** sets the control as delegator at the aggregate

        This has to be called from within your derived class' constructor, if and only
        if you passed <FALSE/> to the <arg>_bSetDelegator</arg> parameter of the
        <type>OControl</type> constructor.
    */
    void    doSetDelegator();
    void    doResetDelegator();

// UNO
    DECLARE_UNO3_AGG_DEFAULTS(OControl, OComponentHelper);
    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& _rType ) throw(::com::sun::star::uno::RuntimeException);

// XTypeProvider
    virtual ::com::sun::star::uno::Sequence<sal_Int8>           SAL_CALL getImplementationId() throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type>   SAL_CALL getTypes() throw(::com::sun::star::uno::RuntimeException);

// OComponentHelper
    virtual void SAL_CALL disposing();

// XComponent (as base of XControl)
    virtual void SAL_CALL dispose(  ) throw(::com::sun::star::uno::RuntimeException)
        { OComponentHelper::dispose(); }
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener>& _rxListener) throw(::com::sun::star::uno::RuntimeException)
        { OComponentHelper::addEventListener(_rxListener); }
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener>& _rxListener) throw(::com::sun::star::uno::RuntimeException)
        { OComponentHelper::removeEventListener(_rxListener); }

// XEventListener
    virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source) throw (::com::sun::star::uno::RuntimeException);

// XServiceInfo
    virtual sal_Bool SAL_CALL           supportsService(const ::rtl::OUString& ServiceName) throw (::com::sun::star::uno::RuntimeException);
    virtual StringSequence SAL_CALL     getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL    getImplementationName() throw(::com::sun::star::uno::RuntimeException) = 0;

// XServiceInfo - static version
    static  StringSequence SAL_CALL     getSupportedServiceNames_Static() throw(::com::sun::star::uno::RuntimeException);

// XControl
    virtual void                                        SAL_CALL setContext(const InterfaceRef& Context) throw (::com::sun::star::uno::RuntimeException);
    virtual InterfaceRef                                SAL_CALL getContext() throw (::com::sun::star::uno::RuntimeException);
    virtual void                                        SAL_CALL createPeer(const ::com::sun::star::uno::Reference<starawt::XToolkit>& Toolkit, const ::com::sun::star::uno::Reference<starawt::XWindowPeer>& Parent) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<starawt::XWindowPeer>  SAL_CALL getPeer() throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool                                    SAL_CALL setModel(const ::com::sun::star::uno::Reference<starawt::XControlModel>& Model) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<starawt::XControlModel>    SAL_CALL getModel() throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<starawt::XView>            SAL_CALL getView() throw (::com::sun::star::uno::RuntimeException);
    virtual void                                        SAL_CALL setDesignMode(sal_Bool bOn) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool                                    SAL_CALL isDesignMode() throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool                                    SAL_CALL isTransparent() throw (::com::sun::star::uno::RuntimeException);

protected:
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type>   _getTypes();
        // overwrite this and call the base class if you have additional types

    ::com::sun::star::uno::Sequence< ::rtl::OUString > getAggregateServiceNames();

private:
    void    impl_resetStateGuard_nothrow();
};

//==================================================================
//= OBoundControl
//= a form control implementing the XBoundControl interface
//==================================================================
typedef ::cppu::ImplHelper1 <   ::com::sun::star::form::XBoundControl
                            >  OBoundControl_BASE;
class OBoundControl :public OControl
                    ,public OBoundControl_BASE
{
protected:
    sal_Bool    m_bLocked : 1;

    ::rtl::OUString m_sOriginalHelpText;                // as long as the text/value is invalid, we change the help text of our peer
    ::com::sun::star::awt::FontDescriptor
                    m_aOriginalFont;                    // as long as the text/value is invalid, we also change the font
    sal_Int32       m_nOriginalTextLineColor;           // (we add red underlining)

public:
    OBoundControl(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory,
        const ::rtl::OUString& _rAggregateService,
        const sal_Bool _bSetDelegator = sal_True
    );

    virtual ~OBoundControl();

    DECLARE_UNO3_AGG_DEFAULTS(OBoundControl, OControl);
    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& _rType ) throw(::com::sun::star::uno::RuntimeException);

    // XBoundControl
    virtual sal_Bool SAL_CALL   getLock() throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL       setLock(sal_Bool _bLock) throw(::com::sun::star::uno::RuntimeException);
        // default implementation just disables the controls, overwrite _setLock to change this behaviour

    // XControl
    virtual sal_Bool SAL_CALL setModel(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& Model) throw (::com::sun::star::uno::RuntimeException);

    // XEventListener
    virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source) throw (::com::sun::star::uno::RuntimeException);

    // OComponentHelper
    virtual void SAL_CALL disposing();

protected:
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type>   _getTypes();
    // implement the lock setting
    virtual void         _setLock(sal_Bool _bLock);
};

//==================================================================
//= OControlModel
//= model of a form layer control
//==================================================================

typedef ::cppu::ImplHelper7 <   ::com::sun::star::form::XFormComponent
                            ,   ::com::sun::star::io::XPersistObject
                            ,   ::com::sun::star::container::XNamed
                            ,   ::com::sun::star::lang::XServiceInfo
                            ,   ::com::sun::star::util::XCloneable
                            ,   ::com::sun::star::beans::XPropertyContainer
                            ,   ::com::sun::star::beans::XPropertyAccess
                            >   OControlModel_BASE;

class OControlModel :public ::cppu::OComponentHelper
                    ,public OPropertySetAggregationHelper
                    ,public OControlModel_BASE
                    ,public OCloneableAggregation
                    ,public IPropertyBagHelperContext
{

protected:
    ::comphelper::ComponentContext  m_aContext;

    ::osl::Mutex                    m_aMutex;
    oslInterlockedCount             m_lockCount;

    InterfaceRef                    m_xParent;                  // ParentComponent
    OImplementationIdsRef           m_aHoldIdHelper;
    PropertyBagHelper               m_aPropertyBagHelper;

    const ::comphelper::ComponentContext&
        getContext() const { return m_aContext; }

// <properties>
    ::rtl::OUString                 m_aName;                    // name of the control
    ::rtl::OUString                 m_aTag;                     // tag for additional data
    sal_Int16                       m_nTabIndex;                // index within the taborder
    sal_Int16                       m_nClassId;                 // type of the control
    sal_Bool                        m_bNativeLook;              // should the control use the native platform look?
    sal_Bool                        m_bGenerateVbEvents;        // should the control generate fake vba events
// </properties>


protected:
    OControlModel(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rFactory,   // factory to create the aggregate with
        const ::rtl::OUString& _rUnoControlModelTypeName,                       // service name of te model to aggregate
        const ::rtl::OUString& rDefault = ::rtl::OUString(),                    // service name of the default control
        const sal_Bool _bSetDelegator = sal_True                                // set to sal_False if you want to call setDelegator later (after returning from this ctor)
    );
    OControlModel(
        const OControlModel* _pOriginal,                                        // the original object to clone
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rFactory,   // factory to create the aggregate with
        const sal_Bool _bCloneAggregate = sal_True,                             // should the aggregate of the original be cloned, too?
        const sal_Bool _bSetDelegator = sal_True                                // set to sal_False if you want to call setDelegator later (after returning from this ctor)
    );
    virtual ~OControlModel();

    /** to be called after a OBoundControlModel (a derivee, respectively) has been cloned

        <p>This method contains late initializations which cannot be done in the
        constructor of this base class, since the virtual method of derived classes do
        not yet work there.</p>
    */
    virtual void clonedFrom( const OControlModel* _pOriginal );

    using OComponentHelper::rBHelper;

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type>   _getTypes();

    void    readHelpTextCompatibly(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream >& _rxInStream);
    void    writeHelpTextCompatibly(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream >& _rxOutStream);

    void    doSetDelegator();
    void    doResetDelegator();

    ::com::sun::star::uno::Sequence< ::rtl::OUString > getAggregateServiceNames();

public:
    DECLARE_UNO3_AGG_DEFAULTS(OControl, OComponentHelper);
    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& _rType ) throw (::com::sun::star::uno::RuntimeException);

// XTypeProvider
    virtual ::com::sun::star::uno::Sequence<sal_Int8>           SAL_CALL getImplementationId() throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type>   SAL_CALL getTypes() throw(::com::sun::star::uno::RuntimeException);

// OComponentHelper
    virtual void SAL_CALL disposing();

// XNamed
    virtual ::rtl::OUString SAL_CALL    getName() throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL               setName(const ::rtl::OUString& aName) throw(::com::sun::star::uno::RuntimeException);

// XServiceInfo
    virtual sal_Bool SAL_CALL           supportsService(const ::rtl::OUString& ServiceName) throw (::com::sun::star::uno::RuntimeException);
    virtual StringSequence SAL_CALL     getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL    getImplementationName() throw(::com::sun::star::uno::RuntimeException) = 0;

// XSericeInfo - static version(s)
    static  StringSequence SAL_CALL     getSupportedServiceNames_Static() throw(::com::sun::star::uno::RuntimeException);

// XPersistObject
    virtual ::rtl::OUString SAL_CALL    getServiceName() throw(::com::sun::star::uno::RuntimeException) = 0;
    virtual void SAL_CALL
        write(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream>& _rxOutStream) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL
        read(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream>& _rxInStream) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

// XChild (base of XFormComponent)
    virtual InterfaceRef SAL_CALL   getParent() throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL           setParent(const InterfaceRef& Parent) throw(::com::sun::star::lang::NoSupportException, ::com::sun::star::uno::RuntimeException);

// XEventListener
    virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source) throw (::com::sun::star::uno::RuntimeException);

// XPropertySet
    virtual void SAL_CALL getFastPropertyValue(::com::sun::star::uno::Any& rValue, sal_Int32 nHandle) const;
    virtual sal_Bool SAL_CALL convertFastPropertyValue(
                ::com::sun::star::uno::Any& _rConvertedValue, ::com::sun::star::uno::Any& _rOldValue, sal_Int32 _nHandle, const ::com::sun::star::uno::Any& _rValue )
                throw (::com::sun::star::lang::IllegalArgumentException);
    virtual void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue )
                throw (::com::sun::star::uno::Exception);
    using ::cppu::OPropertySetHelper::getFastPropertyValue;

// ::com::sun::star::beans::XPropertyState
    virtual ::com::sun::star::beans::PropertyState getPropertyStateByHandle(sal_Int32 nHandle);
    virtual void setPropertyToDefaultByHandle(sal_Int32 nHandle);
    virtual ::com::sun::star::uno::Any getPropertyDefaultByHandle( sal_Int32 nHandle ) const;

// XCloneable
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone(  ) throw (::com::sun::star::uno::RuntimeException) = 0;

// XPropertyContainer
    virtual void SAL_CALL addProperty( const ::rtl::OUString& Name, ::sal_Int16 Attributes, const ::com::sun::star::uno::Any& DefaultValue ) throw (::com::sun::star::beans::PropertyExistException, ::com::sun::star::beans::IllegalTypeException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeProperty( const ::rtl::OUString& Name ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::NotRemoveableException, ::com::sun::star::uno::RuntimeException);

// XPropertyAccess
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getPropertyValues(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aProps ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

protected:
    using OPropertySetAggregationHelper::setPropertyValues;
    using OPropertySetAggregationHelper::getPropertyValues;

protected:
    virtual void writeAggregate( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream >& _rxOutStream ) const;
    virtual void readAggregate( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream >& _rxInStream );

protected:
    // XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo() throw( ::com::sun::star::uno::RuntimeException);
    // OPropertySetHelper
    virtual cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

    /** describes the properties provided by this class, or its respective
        derived class

        Derived classes usually call the base class first, and then append own properties.
    */
    virtual void describeFixedProperties(
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps
    ) const;

    // IPropertyBagHelperContext
    virtual ::osl::Mutex&   getMutex();
    virtual void            describeFixedAndAggregateProperties(
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& _out_rFixedProperties,
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& _out_rAggregateProperties
    ) const;
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XMultiPropertySet >
                            getPropertiesInterface();

    /** describes the properties of our aggregate

        The default implementation simply asks m_xAggregateSet for its properties.

        You usually only need to overload this method if you want to filter the aggregate
        properties.
    */
    virtual void describeAggregateProperties(
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rAggregateProps
    ) const;

public:
    struct LockAccess { friend class ControlModelLock; private: LockAccess() { } };

    void                lockInstance( LockAccess );
    oslInterlockedCount unlockInstance( LockAccess );

    void                firePropertyChanges(
                            const ::com::sun::star::uno::Sequence< sal_Int32 >& _rHandles,
                            const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& _rOldValues,
                            const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& _rNewValues,
                            LockAccess
                        );

    inline ::osl::Mutex&
                        getInstanceMutex() { return m_aMutex; }
};

//==================================================================
// simple destructor
#define DECLARE_DEFAULT_DTOR( classname )   \
    ~classname() \

// constructor for cloning a class
#define DECLARE_DEFAULT_CLONE_CTOR( classname )  \
    classname( \
        const classname* _pOriginal, \
        const   ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory \
    ); \

// all xtors for an inner class of the object hierarchy
#define DECLARE_DEFAULT_XTOR( classname )   \
    classname( \
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory, \
        const ::rtl::OUString& _rUnoControlModelTypeName, \
        const ::rtl::OUString& _rDefault \
    ); \
    DECLARE_DEFAULT_CLONE_CTOR( classname )  \
    DECLARE_DEFAULT_DTOR( classname )   \

// all xtors for an inner class of the object hierarchy which is *bound*
#define DECLARE_DEFAULT_BOUND_XTOR( classname ) \
    classname( \
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory, \
        const ::rtl::OUString& _rUnoControlModelTypeName, \
        const ::rtl::OUString& _rDefault, \
        const sal_Bool _bSupportExternalBinding, \
        const sal_Bool _bSupportsValidation \
    ); \
    DECLARE_DEFAULT_CLONE_CTOR( classname )  \
    DECLARE_DEFAULT_DTOR( classname )   \

// all xtors for a leas class of the object hierarchy
#define DECLARE_DEFAULT_LEAF_XTOR( classname )  \
    classname( \
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory \
    ); \
    classname( \
        const classname* _pOriginal, \
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory \
    ); \
    DECLARE_DEFAULT_DTOR( classname )   \

//==================================================================
// XCloneable
#define DECLARE_XCLONEABLE( ) \
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone(  ) throw (::com::sun::star::uno::RuntimeException)

#define IMPLEMENT_DEFAULT_CLONING( classname ) \
    ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL classname::createClone( ) throw (::com::sun::star::uno::RuntimeException) \
    { \
        classname* pClone = new classname( this, getContext().getLegacyServiceFactory() ); \
        pClone->clonedFrom( this ); \
        return pClone; \
    }

//==================================================================
//= OBoundControlModel
//= model of a form layer control which is bound to a data source field
//==================================================================
typedef ::cppu::ImplHelper4 <   ::com::sun::star::form::XLoadListener
                            ,   ::com::sun::star::form::XReset
                            ,   ::com::sun::star::beans::XPropertyChangeListener
                            ,   ::com::sun::star::sdb::XRowSetChangeListener
                            >   OBoundControlModel_BASE1;

// separated into an own base class since derivees can disable the support for this
// interface, thus we want to easily exclude it in the queryInterface and getTypes
typedef ::cppu::ImplHelper1 <   ::com::sun::star::form::XBoundComponent
                            >   OBoundControlModel_COMMITTING;

// dito
typedef ::cppu::ImplHelper2 <   ::com::sun::star::form::binding::XBindableValue
                            ,   ::com::sun::star::util::XModifyListener
                            >   OBoundControlModel_BINDING;

// dito
typedef ::cppu::ImplHelper2 <   ::com::sun::star::form::validation::XValidityConstraintListener
                            ,   ::com::sun::star::form::validation::XValidatableFormComponent
                            >   OBoundControlModel_VALIDATION;

class OBoundControlModel    :public OControlModel
                            ,public OBoundControlModel_BASE1
                            ,public OBoundControlModel_COMMITTING
                            ,public OBoundControlModel_BINDING
                            ,public OBoundControlModel_VALIDATION
                            ,public ::comphelper::OPropertyChangeListener
{
protected:
    enum ValueChangeInstigator
    {
        eDbColumnBinding,
        eExternalBinding,
        eOther
    };

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                                        m_xField;
    // the form which controls supplies the field we bind to.
    ::com::sun::star::uno::Reference< ::com::sun::star::form::XLoadable >
                                        m_xAmbientForm;

    ::rtl::OUString                     m_sValuePropertyName;
    sal_Int32                           m_nValuePropertyAggregateHandle;
    sal_Int32                           m_nFieldType;
    ::com::sun::star::uno::Type         m_aValuePropertyType;
    bool                                m_bValuePropertyMayBeVoid;

    ResetHelper                         m_aResetHelper;
    ::cppu::OInterfaceContainerHelper   m_aUpdateListeners;
    ::cppu::OInterfaceContainerHelper   m_aFormComponentListeners;

    ::com::sun::star::uno::Reference< ::com::sun::star::form::binding::XValueBinding >
                                        m_xExternalBinding;
    ::com::sun::star::uno::Reference< ::com::sun::star::form::validation::XValidator >
                                        m_xValidator;
    ::com::sun::star::uno::Type         m_aExternalValueType;

// <properties>
    ::rtl::OUString                     m_aControlSource;           // Datenquelle, Name des Feldes
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                                        m_xLabelControl;            // reference to a sibling control (model) which is our label
    sal_Bool                            m_bInputRequired;
// </properties>

    ::comphelper::OPropertyChangeMultiplexer*
                                m_pAggPropMultiplexer;

    bool                        m_bFormListening            : 1;    // are we currently a XLoadListener at our ambient form?
    sal_Bool                    m_bLoaded                   : 1;
    sal_Bool                    m_bRequired                 : 1;
    const sal_Bool              m_bCommitable               : 1;    // do we support XBoundComponent?
    const sal_Bool              m_bSupportsExternalBinding  : 1;    // do we support XBindableValue?
    const sal_Bool              m_bSupportsValidation       : 1;    // do we support XValidatable?
    sal_Bool                    m_bForwardValueChanges      : 1;    // do we currently handle changes in the bound database field?
    sal_Bool                    m_bTransferingValue         : 1;    // true if we're currently transfering our value to an external binding
    sal_Bool                    m_bIsCurrentValueValid      : 1;    // flag specifying whether our current value is valid, relative to our external validator
    sal_Bool                    m_bBindingControlsRO        : 1;    // is our ReadOnly property currently controlled by our external binding?
    sal_Bool                    m_bBindingControlsEnable    : 1;    // is our Enabled property currently controlled by our external binding?

    ValueChangeInstigator       m_eControlValueChangeInstigator;

protected:
    ::rtl::OUString                     m_aLabelServiceName;
        // when setting the label for our control (property FM_PROP_CONTROLLABEL, member m_xLabelControl),
        // we accept only objects supporting an XControlModel interface, an XServiceInfo interface and
        // support for a service (XServiceInfo::supportsService) determined by this string.
        // Any other arguments will throw an IllegalArgumentException.
        // The default value is FM_COMPONENT_FIXEDTEXT.

    ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet >
                                        m_xCursor;
    ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XColumnUpdate >
                                        m_xColumnUpdate;
    ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XColumn >
                                        m_xColumn;

protected:
    inline const ::rtl::OUString&   getValuePropertyName( ) const       { return m_sValuePropertyName; }
    inline sal_Int32                getValuePropertyAggHandle( ) const  { return m_nValuePropertyAggregateHandle; }
    inline const ::rtl::OUString&   getControlSource( ) const           { return m_aControlSource; }
    inline sal_Bool                 isRequired() const                  { return m_bRequired; }
    inline sal_Bool                 isLoaded() const                    { return m_bLoaded; }

protected:

    OBoundControlModel(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rFactory,
                                                            // factory to create the aggregate with
        const ::rtl::OUString& _rUnoControlModelTypeName,   // service name of te model to aggregate
        const ::rtl::OUString& _rDefault,                   // service name of the default control
        const sal_Bool _bCommitable,                        // is the control (model) commitable ?
        const sal_Bool _bSupportExternalBinding,            // set to sal_True if you want to support XBindableValue
        const sal_Bool _bSupportsValidation                 // set to sal_True if you want to support XValidatable
    );
    OBoundControlModel(
        const OBoundControlModel* _pOriginal,               // the original object to clone
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rFactory
                                                            // factory to create the aggregate with
    );
    virtual ~OBoundControlModel();

    /// late ctor after cloning
    virtual void clonedFrom( const OControlModel* _pOriginal );

    /** initializes the part of the class which is related to the control value.

        <p>Kind of late ctor, to be called for derivees which have a dedicated value property.<br/>
        The value property is the property which's value is synced with either the database
        column the object is bound to, or with the external value binding, if present.<br/>
        E.g. for a text control model, this property will most probably be "Text".</p>

        <p>Derived classes are stronly recommend to call this method - at least the
        "DataFieldProperty" (exposed in getFastPropertyValue) relies on the information
        given herein, and needs to be supplied otherwise else.</p>

        <p>If this method has been called properly, then <member>setControlValue</member>
        does not need to be overridden - it will simply set the property value at the
        aggregate then.</p>

        @precond
            The method has not be called before during the life time of the object.

        @param _rValuePropertyName
            the name of the value property
        @param _nValuePropertyExternalHandle
            the handle of the property, as exposed to external components.<br/>
            Normally, this information can be obtained dynamically (e.g. from describeFixedProperties),
            but since this method is to be called from within the constructor of derived classes,
            we prefer to be on the *really* safe side here ....

        @see setControlValue
        @see suspendValueListening
        @see resumeValueListening
        @see describeFixedProperties
    */
    void                    initValueProperty(
                                const ::rtl::OUString& _rValuePropertyName,
                                sal_Int32 _nValuePropertyExternalHandle
                            );

    /** initializes the part of the class which is related to the control value.

        <p>In opposite to ->initValueProperty, this method is to be used for value properties which are <em>not</em>
        implemented by our aggregate, but by ourselves.</p>

        <p>Certain functionality is not available when using own value properties. This includes binding to an external
        value and external validation. (This is not a conceptual limit, but simply missing implementation.)</p>
    */
    void                    initOwnValueProperty(
                                const ::rtl::OUString& i_rValuePropertyName
                            );

    /** suspends listening at the value property

        <p>As long as this listening is suspended, changes in the value property will not be
        recognized and not be handled.</p>

        @see initValueProperty
        @see resumeValueListening
    */
    void                    suspendValueListening( );

    /** resumes listening at the value property

        <p>As long as this listening is suspended, changes in the value property will not be
        recognized and not be handled.</p>

        @precond
            listening at the value property is currently suspended

        @see initValueProperty
        @see resumeValueListening
    */
    void                    resumeValueListening( );

    /** (to be) called when the value property changed

        Normally, this is done automatically, since the value property is a property of our aggregate, and we're
        a listener at this property.
        However, in some cases the value property might not be an aggregate property, but a property of the
        delegator instance. In this case, you'll need to call <code>onValuePropertyChange</code> whenever this
        property changes.
    */
    void                    onValuePropertyChange( ControlModelLock& i_rControLock );

    /** starts listening at the aggregate, for changes in the given property

        <p>The OBoundControlModel automatically registers a multiplexer which listens for
        changes in the aggregate property values. By default, only the control value property
        is observed. You may add additional properties to be observed with this method.</p>

        @see initValueProperty
        @see _propertyChanged
    */
    void                    startAggregatePropertyListening( const ::rtl::OUString& _rPropertyName );

    /** returns the default which should be used when resetting the control

        <p>The default implementation returns an empty Any.</p>

        @see resetNoBroadcast
    */
    virtual ::com::sun::star::uno::Any
                            getDefaultForReset() const;

    /** translates a db column value into a control value.

        <p>Must transform the very current value of the database column we're bound to
        (<member>m_xColumn</member>) into a value which can be used as current value
        for the control.</p>

        @see setControlValue
        @pure
    */
    virtual ::com::sun::star::uno::Any
                            translateDbColumnToControlValue( ) = 0;

    /** returns the data types which the control could use to exchange data with
        an external value binding

        The types returned here are completely independent from the concrete value binding,
        they're just candidates which depend on the control type, and possible the concrete state
        of the control (i.e. some property value).

        If a control implementation supports multiple types, the ordering in the returned
        sequence indicates preference: Preferred types are mentioned first.

        The default implementation returns the type of our value property.
    */
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >
                            getSupportedBindingTypes();

    /** translates the given value, which was obtained from the current external value binding,
        to a value which can be used in setControlValue

        <p>The default implementation returns the value itself, exception when it is VOID, and
        our value property is not allowed to be void - in this case, the returned value is a
        default-constructed value of the type required by our value property.

        @see hasExternalValueBinding
        @see getExternalValueType
    */
    virtual ::com::sun::star::uno::Any
                            translateExternalValueToControlValue( const ::com::sun::star::uno::Any& _rExternalValue ) const;

    /** commits the current control value to our external value binding

        <p>The default implementation simply calls getControlValue.</p>

        @see hasExternalValueBinding
        @see initValueProperty
    */
    virtual ::com::sun::star::uno::Any
                            translateControlValueToExternalValue( ) const;

    /** commits the current control value to the database column we're bound to
        @precond
            we're properly bound to a database column, especially <member>m_xColumnUpdate</member>
            is not <NULL/>
        @param _bPostReset
            <TRUE/> if and only if the current control value results from a reset (<member>getDefaultForReset</member>)
        @pure
    */
    virtual sal_Bool        commitControlValueToDbColumn(
                                bool _bPostReset
                            ) = 0;

    /** sets the given value as new current value for the control

        Besides some administrative work (such as caring for <member>m_eControlValueChangeInstigator</member>),
        this method simply calls <member>doSetControlValue</member>.

        @precond
            Our own mutex is locked.
        @param _rValue
            The value to set. This value is guaranteed to be created by
            <member>translateDbColumnToControlValue</member> or
            <member>translateExternalValueToControlValue</member>
        @param _eInstigator
            the instigator of the value change
    */
            void            setControlValue(
                                const ::com::sun::star::uno::Any& _rValue,
                                ValueChangeInstigator _eInstigator
                            );
    /**
        <p>The default implementation will forward the given value to the aggregate, using
        m_nValuePropertyAggregateHandle and/or m_sValuePropertyName.</p>

        @precond
            Our own mutex is locked.
        @param _rValue
            The value to set. This value is guaranteed to be created by
            <member>translateDbColumnToControlValue</member> or
            <member>translateExternalValueToControlValue</member>
    */
    virtual void            doSetControlValue(
                                const ::com::sun::star::uno::Any& _rValue
                            );

    /** retrieves the current value of the control

        <p>The default implementation will ask the aggregate for the property value
        determined by either m_nValuePropertyAggregateHandle and/or m_sValuePropertyName.</p>

        @precond
            Our own mutex is locked.
    */
    virtual ::com::sun::star::uno::Any
                            getControlValue( ) const;

    /** called whenever a connection to a database column has been established
    */
    virtual void            onConnectedDbColumn( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxForm );
    /** called whenever a connection to a database column has been suspended
    */
    virtual void            onDisconnectedDbColumn();

    /** called whenever a connection to an external supplier of values (XValueBinding) has been established
        @see m_xExternalBinding
    */
    virtual void            onConnectedExternalValue( );
    /** called whenever a connection to an external supplier of values (XValueBinding) has been suspended
    */
    virtual void            onDisconnectedExternalValue();

    /** called whenever an external validator has been registered
    */
    virtual void            onConnectedValidator( );
    /** called whenever an external validator has been revoked
    */
    virtual void            onDisconnectedValidator( );

    /** nFieldType ist der Typ des Feldes, an das das Model gebunden werden soll.
        Das Binden erfolgt genau dann, wenn Rueckgabewert sal_True.
        Die Standard-Implementation erlaubt alles ausser den drei binary-Typen und
        FieldType_OTHER.
    */
    virtual sal_Bool        approveDbColumnType(sal_Int32 _nColumnType);

    /** retrieves the current value of the control, in a shape which can be used with our
        external validator.

        The default implementation simply calls <member>>translateControlValueToExternalValue</member>.

        @precond
            Our own mutex is locked.
    */
    virtual ::com::sun::star::uno::Any
                            translateControlValueToValidatableValue( ) const;

    /** retrieves the current value of the form component

        This is the implementation method for XValidatableFormComponent::getCurrentValue. The default implementation
        calls translateControlValueToValidatableValue if a validator is present, otherwise getControlValue.

        @precond
            our mutex is locked when this method is called
    */
    virtual ::com::sun::star::uno::Any
                            getCurrentFormComponentValue() const;

    /** We can't write (new) common properties in this base class, as the file format doesn't allow this
        (unfortunally). So derived classes may use the following to methods. They secure the written
        data with marks, so any new common properties in newer versions will be skipped by older ones.
    */
    void    writeCommonProperties(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream>& _rxOutStream);
    void    readCommonProperties(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream>& _rxInStream);
    // the next method may be used in derived classes's read when an unknown version is encountered
    void    defaultCommonProperties();

    /** called to reset the control to some kind of default.

        <p>The semantics of "default" is finally defined by the derived class (in particular,
        by <member>getDefaultForReset</member>).</p>

        <p>No listener notification needs to be done in the derived class.</p>

        <p>Normally, you won't override this method, but <member>getDefaultForReset</member> instead.</p>

        @see getDefaultForReset
    */
    virtual void            resetNoBroadcast();

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type>   _getTypes();

    /// sets m_xField to the given new value, without notifying our listeners
    void    impl_setField_noNotify(
                const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxField
            );
    inline bool hasField() const
    {
        return m_xField.is();
    }
    inline sal_Int32 getFieldType() const
    {
        return m_nFieldType;
    }

    // OControlModel's property handling
    virtual void describeFixedProperties(
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps
    ) const;

public:
    inline const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& getField() const
    {
        return m_xField;
    }

public:
    // UNO Anbindung
    DECLARE_UNO3_AGG_DEFAULTS(OBoundControlModel, OControlModel);
    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& _rType ) throw (::com::sun::star::uno::RuntimeException);

    // OComponentHelper
    virtual void SAL_CALL disposing();

    // XReset
    virtual void SAL_CALL reset(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addResetListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XResetListener >& aListener ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeResetListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XResetListener >& aListener ) throw(::com::sun::star::uno::RuntimeException);

    // XServiceInfo
    virtual StringSequence SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

    // XServiceInfo - static version
    static  StringSequence SAL_CALL getSupportedServiceNames_Static() throw(::com::sun::star::uno::RuntimeException);

    // XChild
    virtual void SAL_CALL setParent( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& Parent ) throw(::com::sun::star::lang::NoSupportException, ::com::sun::star::uno::RuntimeException);

    // XPersistObject
    virtual void SAL_CALL write( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream >& OutStream ) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL read( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream >& InStream ) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    // XBoundComponent
    virtual sal_Bool SAL_CALL commit() throw(::com::sun::star::uno::RuntimeException);

    // XUpdateBroadcaster (base of XBoundComponent)
    virtual void SAL_CALL addUpdateListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XUpdateListener >& aListener ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeUpdateListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XUpdateListener >& aListener ) throw(::com::sun::star::uno::RuntimeException);

    // XPropertySet
    virtual void SAL_CALL getFastPropertyValue(::com::sun::star::uno::Any& rValue, sal_Int32 nHandle) const;
    virtual sal_Bool SAL_CALL convertFastPropertyValue(
                ::com::sun::star::uno::Any& _rConvertedValue, ::com::sun::star::uno::Any& _rOldValue, sal_Int32 _nHandle, const ::com::sun::star::uno::Any& _rValue )
                throw (::com::sun::star::lang::IllegalArgumentException);
    virtual void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue )
                throw (::com::sun::star::uno::Exception);
    using ::cppu::OPropertySetHelper::getFastPropertyValue;

// ::com::sun::star::beans::XPropertyState
    virtual ::com::sun::star::uno::Any getPropertyDefaultByHandle( sal_Int32 nHandle ) const;

// XEventListener
    virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source) throw (::com::sun::star::uno::RuntimeException);

// XPropertyChangeListener
    virtual void SAL_CALL propertyChange( const ::com::sun::star::beans::PropertyChangeEvent& evt ) throw(::com::sun::star::uno::RuntimeException);

    // XRowSetChangeListener
    virtual void SAL_CALL onRowSetChanged( const ::com::sun::star::lang::EventObject& i_Event ) throw (::com::sun::star::uno::RuntimeException);

// XLoadListener
    virtual void SAL_CALL loaded( const ::com::sun::star::lang::EventObject& aEvent ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL unloading( const ::com::sun::star::lang::EventObject& aEvent ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL unloaded( const ::com::sun::star::lang::EventObject& aEvent ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL reloading( const ::com::sun::star::lang::EventObject& aEvent ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL reloaded( const ::com::sun::star::lang::EventObject& aEvent ) throw(::com::sun::star::uno::RuntimeException);

private:
    // XBindableValue
    virtual void SAL_CALL setValueBinding( const ::com::sun::star::uno::Reference< ::com::sun::star::form::binding::XValueBinding >& _rxBinding ) throw (::com::sun::star::form::binding::IncompatibleTypesException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::form::binding::XValueBinding > SAL_CALL getValueBinding(  ) throw (::com::sun::star::uno::RuntimeException);

    // XModifyListener
    virtual void SAL_CALL modified( const ::com::sun::star::lang::EventObject& _rEvent ) throw (::com::sun::star::uno::RuntimeException);

    // XValidatable
    virtual void SAL_CALL setValidator( const ::com::sun::star::uno::Reference< ::com::sun::star::form::validation::XValidator >& Validator ) throw (::com::sun::star::util::VetoException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::form::validation::XValidator > SAL_CALL getValidator(  ) throw (::com::sun::star::uno::RuntimeException);

    // XValidityConstraintListener
    virtual void SAL_CALL validityConstraintChanged( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);

    // XValidatableFormComponent
    virtual sal_Bool SAL_CALL isValid(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getCurrentValue(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addFormComponentValidityListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::validation::XFormComponentValidityListener >& Listener ) throw (::com::sun::star::lang::NullPointerException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeFormComponentValidityListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::validation::XFormComponentValidityListener >& Listener ) throw (::com::sun::star::lang::NullPointerException, ::com::sun::star::uno::RuntimeException);

protected:
    // OPropertyChangeListener
    virtual void
                _propertyChanged( const ::com::sun::star::beans::PropertyChangeEvent& _rEvt ) throw ( ::com::sun::star::uno::RuntimeException );

    /// checks whether we currently have an external value binding in place
    inline  bool    hasExternalValueBinding() const { return m_xExternalBinding.is(); }

    // checks whether we currently have an external validator
    inline  bool    hasValidator() const { return m_xValidator.is(); }

    /** transfers the very current value of the db column we're bound to the control
        @precond
            our own mutex is locked
        @precond
            we don't have an external binding in place
    */
    void        transferDbValueToControl( );

    /** transfers the current value of the active external binding to the control
        @precond
            we do have an active external binding in place
    */
    void        transferExternalValueToControl( ControlModelLock& _rInstanceLock );

    /** transfers the control value to the external binding
        @precond
            our own mutex is locked, and _rInstanceLock is the guard locking it
        @precond
            we do have an active external binding in place
    */
    void        transferControlValueToExternal( ControlModelLock& _rInstanceLock );

    /** calculates the type which is to be used to communicate with the current external binding,
        and stores it in m_aExternalValueType

        The method checks the possible type candidates as returned by getSupportedBindingTypes,
        and the types supported by the current external binding, if any.
    */
    void        calculateExternalValueType();

    /** returns the type which should be used to exchange data with our external value binding

        @see initValueProperty
    */
    const ::com::sun::star::uno::Type&
                getExternalValueType() const { return m_aExternalValueType; }

    /** initializes the control from m_xField

        Basically, this method calls transferDbValueToControl - but only if our cursor is positioned
        on a valid row. Otherwise, the control is reset.

        @precond
            m_xField is not <NULL/>
    */
    void        initFromField( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet>& _rxForm );

private:
    sal_Bool    connectToField( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet>& _rxForm );
    void        resetField();

    /** does a new validation of the control value

        If necessary, our <member>m_bIsCurrentValueValid</member> member will be adjusted,
        and changes will be notified.

        Note that it's not necessary that we're connected to a validator. If we are not,
        it's assumed that our value is valid, and this is handled appropriately.

        Use this method if there is a potential that <b>only</b> the validity flag changed. If
        any of the other aspects (our current value, or our current text) changed, then
        pass <TRUE/> for <member>_bForceNotification</member>.

        @param _bForceNotification
            if <TRUE/>, then the validity listeners will be notified, not matter whether the validity
            changed.
    */
    void        recheckValidity( bool _bForceNotification );

    /// initializes m_pAggPropMultiplexer
    void        implInitAggMultiplexer( );

    /// initializes listening at the value property
    void        implInitValuePropertyListening( ) const;

    /** adds or removes the component as load listener to/from our form, and (if necessary) as RowSetChange listener at
        our parent.

        @precond there must no external value binding be in place
    */
    void        doFormListening( const bool _bStart );

    inline bool isFormListening() const { return m_bFormListening; }

    /** determines the new value of m_xAmbientForm
    */
    void        impl_determineAmbientForm_nothrow();

    /** connects to a value supplier which is an database column.

        The column is take from our parent, which must be a database form respectively row set.

        @precond The control does not have an external value supplier

        @param _bFromReload
            Determines whether the connection is made after the row set has been loaded (<FALSE/>)
            or reloaded (<TRUE/>)

        @see impl_disconnectDatabaseColumn_noNotify
    */
    void        impl_connectDatabaseColumn_noNotify(
                    bool  _bFromReload
                );

    /** disconnects from a value supplier which is an database column

        @precond The control does not have an external value supplier
        @see impl_connectDatabaseColumn_noNotify
    */
    void        impl_disconnectDatabaseColumn_noNotify();

    /** connects to an external value binding

        <p>Note that by definition, external data bindings superseede the SQL data binding which
        is defined by our RowSet-column-related properties. This means that in case we're currently
        connected to a database column when this is called, this connection is suspended.</p>

        @precond
                the new external binding has already been approved (see <member>impl_approveValueBinding_nolock</member>)
        @precond
                there currently is no external binding in place
    */
    void        connectExternalValueBinding(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::form::binding::XValueBinding >& _rxBinding,
                    ControlModelLock& _rInstanceLock
                );

    /** disconnects from an external value binding

        @precond
                there currently is an external binding in place
    */
    void        disconnectExternalValueBinding( );

    /** connects the component to an external validator

        @precond
            there currently is no active validator
        @precond
            our mutex is currently locked exactly once
    */
    void        connectValidator(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::form::validation::XValidator >& _rxValidator
                );

    /** disconnects the component from it's current an external validator

        @precond
            there currently is an active validator
        @precond
            our mutex is currently locked exactly once
    */
    void        disconnectValidator( );

    /** called from within <member scope="com::sun::star:::form::binding">XBindableValue::setValueBinding</member>
        to approve the new binding

        The default implementation approves the binding if and only if it is not <NULL/>, and supports
        the type returned by getExternalValueType.

        @param _rxBinding
            the binding which applies for being responsible for our value, Must not be
            <NULL/>
        @return
            <TRUE/> if and only if the given binding can supply values in the proper type

        @seealso getExternalValueType
    */
    sal_Bool    impl_approveValueBinding_nolock(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::form::binding::XValueBinding >& _rxBinding
                );
};

    //=========================================================================
    //= inlines
    //=========================================================================
    inline void ControlModelLock::acquire()
    {
        m_rModel.lockInstance( OControlModel::LockAccess() );
        m_bLocked = true;
    }
    inline void ControlModelLock::release()
    {
        OSL_ENSURE( m_bLocked, "ControlModelLock::release: not locked!" );
        m_bLocked = false;

        if ( 0 == m_rModel.unlockInstance( OControlModel::LockAccess() ) )
            impl_notifyAll_nothrow();
    }

//.........................................................................
}
//.........................................................................

#endif // _FORMS_FORMCOMPONENT_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
