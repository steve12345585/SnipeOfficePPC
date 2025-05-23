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


#include "componenttools.hxx"
#include "FormComponent.hxx"
#include "frm_resource.hrc"
#include "frm_resource.hxx"
#include "property.hrc"
#include "services.hxx"

#include <com/sun/star/awt/XTextComponent.hpp>
#include <com/sun/star/awt/XVclWindowPeer.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/form/XForm.hpp>
#include <com/sun/star/form/XLoadable.hpp>
#include <com/sun/star/io/XMarkableStream.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/sdb/XRowSetChangeBroadcaster.hpp>
#include <com/sun/star/sdb/XRowSetSupplier.hpp>
#include <com/sun/star/sdbc/ColumnValue.hpp>
#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>

#include <comphelper/basicio.hxx>
#include <comphelper/guarding.hxx>
#include <comphelper/listenernotification.hxx>
#include <comphelper/property.hxx>
#include <connectivity/dbtools.hxx>
#include <cppuhelper/queryinterface.hxx>
#include <rtl/logfile.hxx>
#include <toolkit/helper/emptyfontdescriptor.hxx>
#include <tools/debug.hxx>
#include <tools/diagnose_ex.h>

#include <functional>
#include <algorithm>


//... namespace frm .......................................................
namespace frm
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::sdb;
    using namespace ::com::sun::star::sdbc;
    using namespace ::com::sun::star::sdbcx;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::container;
    using namespace ::com::sun::star::form;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::io;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::util;
    using namespace ::com::sun::star::form::binding;
    using namespace ::com::sun::star::form::validation;
    using namespace ::dbtools;
    using namespace ::comphelper;

    //=========================================================================
    //= FieldChangeNotifier
    //=========================================================================
    //-------------------------------------------------------------------------
    void ControlModelLock::impl_notifyAll_nothrow()
    {
        m_rModel.firePropertyChanges( m_aHandles, m_aOldValues, m_aNewValues, OControlModel::LockAccess() );
    }

    //-------------------------------------------------------------------------
    void ControlModelLock::addPropertyNotification( const sal_Int32 _nHandle, const Any& _rOldValue, const Any& _rNewValue )
    {
        sal_Int32 nOldLength = m_aHandles.getLength();
        if  (   ( nOldLength != m_aOldValues.getLength() )
            ||  ( nOldLength != m_aNewValues.getLength() )
            )
            throw RuntimeException( ::rtl::OUString(), m_rModel );

        m_aHandles.realloc( nOldLength + 1 );
        m_aHandles[ nOldLength ] = _nHandle;
        m_aOldValues.realloc( nOldLength + 1 );
        m_aOldValues[ nOldLength ] = _rOldValue;
        m_aNewValues.realloc( nOldLength + 1 );
        m_aNewValues[ nOldLength ] = _rNewValue;
    }

    //=========================================================================
    //= FieldChangeNotifier
    //=========================================================================
    //-------------------------------------------------------------------------
    class FieldChangeNotifier
    {
    public:
        FieldChangeNotifier( ControlModelLock& _rLock )
            :m_rLock( _rLock )
            ,m_rModel( dynamic_cast< OBoundControlModel& >( _rLock.getModel() ) )
        {
            m_xOldField = m_rModel.getField();
        }

        ~FieldChangeNotifier()
        {
            Reference< XPropertySet > xNewField( m_rModel.getField() );
            if ( m_xOldField != xNewField )
                m_rLock.addPropertyNotification( PROPERTY_ID_BOUNDFIELD, makeAny( m_xOldField ), makeAny( xNewField ) );
        }

    private:
        ControlModelLock&           m_rLock;
        OBoundControlModel&         m_rModel;
        Reference< XPropertySet >   m_xOldField;
    };

//=============================================================================
//= base class for form layer controls
//=============================================================================
DBG_NAME(frm_OControl)
//------------------------------------------------------------------------------
OControl::OControl( const Reference< XMultiServiceFactory >& _rxFactory, const rtl::OUString& _rAggregateService, const sal_Bool _bSetDelegator )
            :OComponentHelper(m_aMutex)
            ,m_aContext( _rxFactory )
{
    DBG_CTOR(frm_OControl, NULL);
    // VCL-Control aggregieren
    // bei Aggregation den Refcount um eins erhoehen da im setDelegator
    // das Aggregat selbst den Refcount erhoeht
    increment( m_refCount );
    {
        m_xAggregate = m_xAggregate.query( _rxFactory->createInstance( _rAggregateService ) );
        m_xControl = m_xControl.query( m_xAggregate );
    }
    decrement( m_refCount );

    if ( _bSetDelegator )
        doSetDelegator();
}

//------------------------------------------------------------------------------
OControl::~OControl()
{
    DBG_DTOR(frm_OControl, NULL);
    doResetDelegator();
}

//------------------------------------------------------------------------------
void OControl::doResetDelegator()
{
    if ( m_xAggregate.is() )
        m_xAggregate->setDelegator( NULL );
}

//------------------------------------------------------------------------------
void OControl::doSetDelegator()
{
    increment( m_refCount );
    if ( m_xAggregate.is() )
    {   // those brackets are important for some compilers, don't remove!
        // (they ensure that the temporary object created in the line below
        // is destroyed *before* the refcount-decrement)
        m_xAggregate->setDelegator( static_cast< XWeak* >( this ) );
    }
    decrement( m_refCount );
}

// UNO Anbindung
//------------------------------------------------------------------------------
Any SAL_CALL OControl::queryAggregation( const Type& _rType ) throw(RuntimeException)
{
    // ask the base class
    Any aReturn( OComponentHelper::queryAggregation(_rType) );
    // ask our own interfaces
    if (!aReturn.hasValue())
    {
        aReturn = OControl_BASE::queryInterface(_rType);
        // ask our aggregate
        if (!aReturn.hasValue() && m_xAggregate.is())
            aReturn = m_xAggregate->queryAggregation(_rType);
    }

    return aReturn;
}

//------------------------------------------------------------------------------
Sequence<sal_Int8> SAL_CALL OControl::getImplementationId() throw(RuntimeException)
{
    return OImplementationIds::getImplementationId(getTypes());
}

//------------------------------------------------------------------------------
Sequence<Type> SAL_CALL OControl::getTypes() throw(RuntimeException)
{
    TypeBag aTypes( _getTypes() );

    Reference< XTypeProvider > xProv;
    if ( query_aggregation( m_xAggregate, xProv ) )
        aTypes.addTypes( xProv->getTypes() );

    return aTypes.getTypes();
}

//------------------------------------------------------------------------------
Sequence<Type> OControl::_getTypes()
{
    return TypeBag( OComponentHelper::getTypes(), OControl_BASE::getTypes() ).getTypes();
}

//------------------------------------------------------------------------------
void OControl::initFormControlPeer( const Reference< XWindowPeer >& /*_rxPeer*/ )
{
    // nothing to do here
}

// OComponentHelper
//------------------------------------------------------------------------------
void OControl::disposing()
{
    OComponentHelper::disposing();

    m_aWindowStateGuard.attach( NULL, NULL );

    Reference< XComponent > xComp;
    if (query_aggregation(m_xAggregate, xComp))
        xComp->dispose();
}

// XServiceInfo
//------------------------------------------------------------------------------
sal_Bool SAL_CALL OControl::supportsService(const rtl::OUString& _rsServiceName) throw ( RuntimeException)
{
    Sequence<rtl::OUString> aSupported = getSupportedServiceNames();
    const rtl::OUString* pSupported = aSupported.getConstArray();
    for (sal_Int32 i=0; i<aSupported.getLength(); ++i, ++pSupported)
        if (pSupported->equals(_rsServiceName))
            return sal_True;
    return sal_False;
}

//------------------------------------------------------------------------------
Sequence< ::rtl::OUString > OControl::getAggregateServiceNames()
{
    Sequence< ::rtl::OUString > aAggServices;
    Reference< XServiceInfo > xInfo;
    if ( query_aggregation( m_xAggregate, xInfo ) )
        aAggServices = xInfo->getSupportedServiceNames();
    return aAggServices;
}

//------------------------------------------------------------------------------
Sequence<rtl::OUString> SAL_CALL OControl::getSupportedServiceNames() throw(RuntimeException)
{
    return ::comphelper::concatSequences(
        getAggregateServiceNames(),
        getSupportedServiceNames_Static()
   );
}

//------------------------------------------------------------------------------
Sequence< ::rtl::OUString > SAL_CALL OControl::getSupportedServiceNames_Static() throw( RuntimeException )
{
    // no own supported service names
    return Sequence< ::rtl::OUString >();
}

// XEventListener
//------------------------------------------------------------------------------
void SAL_CALL OControl::disposing(const com::sun::star::lang::EventObject& _rEvent) throw (RuntimeException)
{
    Reference< XInterface > xAggAsIface;
    query_aggregation(m_xAggregate, xAggAsIface);

    // does the disposing come from the aggregate ?
    if (xAggAsIface != Reference< XInterface >(_rEvent.Source, UNO_QUERY))
    {   // no -> forward it
                Reference<com::sun::star::lang::XEventListener> xListener;
        if (query_aggregation(m_xAggregate, xListener))
            xListener->disposing(_rEvent);
    }
}

// XControl
//------------------------------------------------------------------------------
void SAL_CALL OControl::setContext(const Reference< XInterface >& Context) throw (RuntimeException)
{
    if (m_xControl.is())
        m_xControl->setContext(Context);
}

//------------------------------------------------------------------------------
Reference< XInterface > SAL_CALL OControl::getContext() throw (RuntimeException)
{
    return m_xControl.is() ? m_xControl->getContext() : Reference< XInterface >();
}

//------------------------------------------------------------------------------
void OControl::impl_resetStateGuard_nothrow()
{
    Reference< XWindow2 > xWindow;
    Reference< XControlModel > xModel;
    try
    {
        xWindow.set( getPeer(), UNO_QUERY );
        xModel.set( getModel(), UNO_QUERY );
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
    m_aWindowStateGuard.attach( xWindow, xModel );
}

//------------------------------------------------------------------------------
void SAL_CALL OControl::createPeer(const Reference<XToolkit>& _rxToolkit, const Reference<XWindowPeer>& _rxParent) throw (RuntimeException)
{
    if ( m_xControl.is() )
    {
        m_xControl->createPeer( _rxToolkit, _rxParent );

        initFormControlPeer( getPeer() );

        impl_resetStateGuard_nothrow();
    }
}

//------------------------------------------------------------------------------
Reference<XWindowPeer> SAL_CALL OControl::getPeer() throw ( RuntimeException)
{
    return m_xControl.is() ? m_xControl->getPeer() : Reference<XWindowPeer>();
}

//------------------------------------------------------------------------------
sal_Bool SAL_CALL OControl::setModel(const Reference<XControlModel>& Model) throw ( RuntimeException)
{
    if ( !m_xControl.is() )
        return sal_False;

    sal_Bool bSuccess = m_xControl->setModel( Model );
    impl_resetStateGuard_nothrow();
    return bSuccess;
}

//------------------------------------------------------------------------------
Reference<XControlModel> SAL_CALL OControl::getModel() throw ( RuntimeException)
{
    return m_xControl.is() ? m_xControl->getModel() : Reference<XControlModel>();
}

//------------------------------------------------------------------------------
Reference<XView> SAL_CALL OControl::getView() throw ( RuntimeException)
{
    return m_xControl.is() ? m_xControl->getView() : Reference<XView>();
}

//------------------------------------------------------------------------------
void SAL_CALL OControl::setDesignMode(sal_Bool bOn) throw ( RuntimeException)
{
    if (m_xControl.is())
        m_xControl->setDesignMode(bOn);
}

//------------------------------------------------------------------------------
sal_Bool SAL_CALL OControl::isDesignMode() throw ( RuntimeException)
{
    return m_xControl.is() ? m_xControl->isDesignMode() : sal_True;
}

//------------------------------------------------------------------------------
sal_Bool SAL_CALL OControl::isTransparent() throw ( RuntimeException)
{
    return m_xControl.is() ? m_xControl->isTransparent() : sal_True;
}

//==================================================================
//= OBoundControl
//==================================================================
DBG_NAME(frm_OBoundControl);
//------------------------------------------------------------------
OBoundControl::OBoundControl( const Reference< XMultiServiceFactory >& _rxFactory,
            const ::rtl::OUString& _rAggregateService, const sal_Bool _bSetDelegator )
    :OControl( _rxFactory, _rAggregateService, _bSetDelegator )
    ,m_bLocked(sal_False)
    ,m_aOriginalFont( EmptyFontDescriptor() )
    ,m_nOriginalTextLineColor( 0 )
{
    DBG_CTOR(frm_OBoundControl, NULL);
}

//------------------------------------------------------------------
OBoundControl::~OBoundControl()
{
    DBG_DTOR(frm_OBoundControl, NULL);
}
// -----------------------------------------------------------------------------
Sequence< Type> OBoundControl::_getTypes()
{
    return TypeBag( OControl::_getTypes(), OBoundControl_BASE::getTypes() ).getTypes();
}
//------------------------------------------------------------------
Any SAL_CALL OBoundControl::queryAggregation(const Type& _rType) throw(RuntimeException)
{
    Any aReturn;

    // XTypeProvider first - don't ask the OBoundControl_BASE, it would deliver incomplete types
    if ( _rType.equals( ::getCppuType( static_cast< Reference< XTypeProvider >* >( NULL ) ) ) )
        aReturn = OControl::queryAggregation( _rType );

    // ask our own interfaces
    // (do this first (except XTypeProvider ) - we want to "overwrite" XPropertiesChangeListener)
    if ( !aReturn.hasValue() )
        aReturn = OBoundControl_BASE::queryInterface( _rType );

    // ask the base class
    if ( !aReturn.hasValue() )
        aReturn = OControl::queryAggregation( _rType );

    return aReturn;
}

//------------------------------------------------------------------
sal_Bool SAL_CALL OBoundControl::getLock() throw(RuntimeException)
{
    return m_bLocked;
}

//------------------------------------------------------------------
void SAL_CALL OBoundControl::setLock(sal_Bool _bLock) throw(RuntimeException)
{
    if (m_bLocked == _bLock)
        return;

    osl::MutexGuard aGuard(m_aMutex);
    _setLock(_bLock);
    m_bLocked = _bLock;
}

//------------------------------------------------------------------
void OBoundControl::_setLock(sal_Bool _bLock)
{
    // try to set the text component to readonly
    Reference< XWindowPeer > xPeer = getPeer();
    Reference< XTextComponent > xText( xPeer, UNO_QUERY );

    if ( xText.is() )
        xText->setEditable( !_bLock );
    else
    {
        // disable the window
        Reference< XWindow > xComp( xPeer, UNO_QUERY );
        if ( xComp.is() )
            xComp->setEnable( !_bLock );
    }
}

//--------------------------------------------------------------------
sal_Bool SAL_CALL OBoundControl::setModel( const Reference< XControlModel >& _rxModel ) throw (RuntimeException)
{
    return OControl::setModel( _rxModel );
}

//--------------------------------------------------------------------
void SAL_CALL OBoundControl::disposing(const EventObject& Source) throw (RuntimeException)
{
    // just disambiguate
    OControl::disposing(Source);
}

//--------------------------------------------------------------------
void OBoundControl::disposing()
{
    OControl::disposing();
}

//==================================================================
//= OControlModel
//==================================================================
DBG_NAME(OControlModel)
//------------------------------------------------------------------
Sequence<sal_Int8> SAL_CALL OControlModel::getImplementationId() throw(RuntimeException)
{
    return OImplementationIds::getImplementationId(getTypes());
}

//------------------------------------------------------------------
Sequence<Type> SAL_CALL OControlModel::getTypes() throw(RuntimeException)
{
    TypeBag aTypes( _getTypes() );

    Reference< XTypeProvider > xProv;
    if ( query_aggregation( m_xAggregate, xProv ) )
        aTypes.addTypes( xProv->getTypes() );

    return aTypes.getTypes();
}

//------------------------------------------------------------------------------
Sequence<Type> OControlModel::_getTypes()
{
    return TypeBag( OComponentHelper::getTypes(),
        OPropertySetAggregationHelper::getTypes(),
        OControlModel_BASE::getTypes()
    ).getTypes();
}

//------------------------------------------------------------------
Any SAL_CALL OControlModel::queryAggregation(const Type& _rType) throw (RuntimeException)
{
    // base class 1
    Any aReturn(OComponentHelper::queryAggregation(_rType));

    // base class 2
    if (!aReturn.hasValue())
    {
        aReturn = OControlModel_BASE::queryInterface(_rType);

        // our own interfaces
        if (!aReturn.hasValue())
        {
            aReturn = OPropertySetAggregationHelper::queryInterface(_rType);
            // our aggregate
            if (!aReturn.hasValue() && m_xAggregate.is() && !_rType.equals(::getCppuType(static_cast< Reference< XCloneable>* >(NULL))))
                aReturn = m_xAggregate->queryAggregation(_rType);
        }
    }
    return aReturn;
}

//------------------------------------------------------------------------------
void OControlModel::readHelpTextCompatibly(const staruno::Reference< stario::XObjectInputStream >& _rxInStream)
{
    ::rtl::OUString sHelpText;
    ::comphelper::operator>>( _rxInStream, sHelpText);
    try
    {
        if (m_xAggregateSet.is())
            m_xAggregateSet->setPropertyValue(PROPERTY_HELPTEXT, makeAny(sHelpText));
    }
    catch(const Exception&)
    {
        OSL_FAIL("OControlModel::readHelpTextCompatibly: could not forward the property value to the aggregate!");
    }
}

//------------------------------------------------------------------------------
void OControlModel::writeHelpTextCompatibly(const staruno::Reference< stario::XObjectOutputStream >& _rxOutStream)
{
    ::rtl::OUString sHelpText;
    try
    {
        if (m_xAggregateSet.is())
            m_xAggregateSet->getPropertyValue(PROPERTY_HELPTEXT) >>= sHelpText;
    }
    catch(const Exception&)
    {
        OSL_FAIL("OControlModel::writeHelpTextCompatibly: could not retrieve the property value from the aggregate!");
    }
    ::comphelper::operator<<( _rxOutStream, sHelpText);
}

//------------------------------------------------------------------
OControlModel::OControlModel(
                        const Reference<com::sun::star::lang::XMultiServiceFactory>& _rxFactory,
            const ::rtl::OUString& _rUnoControlModelTypeName,
            const ::rtl::OUString& rDefault, const sal_Bool _bSetDelegator)
    :OComponentHelper(m_aMutex)
    ,OPropertySetAggregationHelper(OComponentHelper::rBHelper)
    ,m_aContext( _rxFactory )
    ,m_lockCount( 0 )
    ,m_aPropertyBagHelper( *this )
    ,m_nTabIndex(FRM_DEFAULT_TABINDEX)
    ,m_nClassId(FormComponentType::CONTROL)
    ,m_bNativeLook( sal_False )
    ,m_bGenerateVbEvents( sal_False )
        // form controls are usually embedded into documents, not dialogs, and in documents
        // the native look is ugly ....
        // #i37342#
{
    DBG_CTOR(OControlModel, NULL);
    if (!_rUnoControlModelTypeName.isEmpty())  // the is a model we have to aggregate
    {
        increment(m_refCount);

        {
            m_xAggregate = Reference<XAggregation>(_rxFactory->createInstance(_rUnoControlModelTypeName), UNO_QUERY);
            setAggregation(m_xAggregate);

            if ( m_xAggregateSet.is() )
            {
                try
                {
                    if ( !rDefault.isEmpty() )
                        m_xAggregateSet->setPropertyValue( PROPERTY_DEFAULTCONTROL, makeAny( rDefault ) );
                }
                catch( const Exception& )
                {
                    OSL_FAIL( "OControlModel::OControlModel: caught an exception!" );
                }
            }
        }

        if (_bSetDelegator)
            doSetDelegator();

        // Refcount wieder bei NULL
        decrement(m_refCount);
    }

}

//------------------------------------------------------------------
OControlModel::OControlModel( const OControlModel* _pOriginal, const Reference< XMultiServiceFactory>& _rxFactory, const sal_Bool _bCloneAggregate, const sal_Bool _bSetDelegator )
    :OComponentHelper( m_aMutex )
    ,OPropertySetAggregationHelper( OComponentHelper::rBHelper )
    ,m_aContext( _rxFactory )
    ,m_lockCount( 0 )
    ,m_aPropertyBagHelper( *this )
    ,m_nTabIndex( FRM_DEFAULT_TABINDEX )
    ,m_nClassId( FormComponentType::CONTROL )
{
    DBG_CTOR( OControlModel, NULL );
    DBG_ASSERT( _pOriginal, "OControlModel::OControlModel: invalid original!" );

    // copy members
    m_aName = _pOriginal->m_aName;
    m_aTag = _pOriginal->m_aTag;
    m_nTabIndex = _pOriginal->m_nTabIndex;
    m_nClassId = _pOriginal->m_nClassId;
    m_bNativeLook = _pOriginal->m_bNativeLook;
    m_bGenerateVbEvents = _pOriginal->m_bGenerateVbEvents;

    if ( _bCloneAggregate )
    {
        // temporarily increment refcount because of temporary references to ourself in the following
        increment( m_refCount );

        {
            // transfer the (only, at the very moment!) ref count
            m_xAggregate = createAggregateClone( _pOriginal );

            // set aggregation (retrieve other direct interfaces of the aggregate)
            setAggregation( m_xAggregate );
        }

        // set the delegator, if allowed by our derived class
        if ( _bSetDelegator )
            doSetDelegator();

        // decrement ref count
        decrement( m_refCount );
    }

}

//------------------------------------------------------------------
OControlModel::~OControlModel()
{
    // release the aggregate
    doResetDelegator( );

    DBG_DTOR(OControlModel, NULL);
}

//------------------------------------------------------------------
void OControlModel::clonedFrom( const OControlModel* /*_pOriginal*/ )
{
    // nothing to do in this base class
}

//------------------------------------------------------------------------------
void OControlModel::doResetDelegator()
{
    if (m_xAggregate.is())
        m_xAggregate->setDelegator(NULL);
}

//------------------------------------------------------------------------------
void OControlModel::doSetDelegator()
{
    increment(m_refCount);
    if (m_xAggregate.is())
    {
        m_xAggregate->setDelegator(static_cast<XWeak*>(this));
    }
    decrement(m_refCount);
}

// XChild
//------------------------------------------------------------------------------
Reference< XInterface > SAL_CALL OControlModel::getParent() throw(RuntimeException)
{
    return m_xParent;
}

//------------------------------------------------------------------------------
void SAL_CALL OControlModel::setParent(const Reference< XInterface >& _rxParent) throw(com::sun::star::lang::NoSupportException, RuntimeException)
{
    osl::MutexGuard aGuard(m_aMutex);

    Reference<XComponent> xComp(m_xParent, UNO_QUERY);
    if (xComp.is())
        xComp->removeEventListener(static_cast<XPropertiesChangeListener*>(this));

    m_xParent = _rxParent;
    xComp = xComp.query( m_xParent );

    if ( xComp.is() )
        xComp->addEventListener(static_cast<XPropertiesChangeListener*>(this));
}

// XNamed
//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OControlModel::getName() throw(RuntimeException)
{
    ::rtl::OUString aReturn;
    OPropertySetHelper::getFastPropertyValue(PROPERTY_ID_NAME) >>= aReturn;
    return aReturn;
}

//------------------------------------------------------------------------------
void SAL_CALL OControlModel::setName(const ::rtl::OUString& _rName) throw(RuntimeException)
{
        setFastPropertyValue(PROPERTY_ID_NAME, makeAny(_rName));
}

// XServiceInfo
//------------------------------------------------------------------------------
sal_Bool SAL_CALL OControlModel::supportsService(const rtl::OUString& _rServiceName) throw ( RuntimeException)
{
    Sequence<rtl::OUString> aSupported = getSupportedServiceNames();
    const rtl::OUString* pSupported = aSupported.getConstArray();
    for (sal_Int32 i=0; i<aSupported.getLength(); ++i, ++pSupported)
        if (pSupported->equals(_rServiceName))
            return sal_True;
    return sal_False;
}

//------------------------------------------------------------------------------
Sequence< ::rtl::OUString > OControlModel::getAggregateServiceNames()
{
    Sequence< ::rtl::OUString > aAggServices;
    Reference< XServiceInfo > xInfo;
    if ( query_aggregation( m_xAggregate, xInfo ) )
        aAggServices = xInfo->getSupportedServiceNames();
    return aAggServices;
}

//------------------------------------------------------------------------------
Sequence<rtl::OUString> SAL_CALL OControlModel::getSupportedServiceNames() throw(RuntimeException)
{
    return ::comphelper::concatSequences(
        getAggregateServiceNames(),
        getSupportedServiceNames_Static()
    );
}

//------------------------------------------------------------------------------
Sequence< ::rtl::OUString > SAL_CALL OControlModel::getSupportedServiceNames_Static() throw( RuntimeException )
{
    Sequence< ::rtl::OUString > aServiceNames( 2 );
    aServiceNames[ 0 ] = FRM_SUN_FORMCOMPONENT;
    aServiceNames[ 1 ] = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.form.FormControlModel") );
    return aServiceNames;
}

// XEventListener
//------------------------------------------------------------------------------
void SAL_CALL OControlModel::disposing(const com::sun::star::lang::EventObject& _rSource) throw (RuntimeException)
{
    // release the parent
    if (_rSource.Source == m_xParent)
    {
        osl::MutexGuard aGuard(m_aMutex);
        m_xParent = NULL;
    }
    else
    {
        Reference<com::sun::star::lang::XEventListener> xEvtLst;
        if (query_aggregation(m_xAggregate, xEvtLst))
        {
            osl::MutexGuard aGuard(m_aMutex);
            xEvtLst->disposing(_rSource);
        }
    }
}

// OComponentHelper
//-----------------------------------------------------------------------------
void OControlModel::disposing()
{
    OPropertySetAggregationHelper::disposing();

    Reference<com::sun::star::lang::XComponent> xComp;
    if (query_aggregation(m_xAggregate, xComp))
        xComp->dispose();

    setParent(Reference<XFormComponent>());

    m_aPropertyBagHelper.dispose();
}

//------------------------------------------------------------------------------
void OControlModel::writeAggregate( const Reference< XObjectOutputStream >& _rxOutStream ) const
{
    Reference< XPersistObject > xPersist;
    if ( query_aggregation( m_xAggregate, xPersist ) )
        xPersist->write( _rxOutStream );
}

//------------------------------------------------------------------------------
void OControlModel::readAggregate( const Reference< XObjectInputStream >& _rxInStream )
{
    Reference< XPersistObject > xPersist;
    if ( query_aggregation( m_xAggregate, xPersist ) )
        xPersist->read( _rxInStream );
}

//------------------------------------------------------------------------------
void SAL_CALL OControlModel::write(const Reference<stario::XObjectOutputStream>& _rxOutStream)
                        throw(stario::IOException, RuntimeException)
{
    osl::MutexGuard aGuard(m_aMutex);

    // 1. Schreiben des UnoControls
    Reference<stario::XMarkableStream> xMark(_rxOutStream, UNO_QUERY);
    if ( !xMark.is() )
    {
        throw IOException(
            FRM_RES_STRING( RID_STR_INVALIDSTREAM ),
            static_cast< ::cppu::OWeakObject* >( this )
        );
    }

    sal_Int32 nMark = xMark->createMark();
    sal_Int32 nLen = 0;

    _rxOutStream->writeLong(nLen);

    writeAggregate( _rxOutStream );

    // feststellen der Laenge
    nLen = xMark->offsetToMark(nMark) - 4;
    xMark->jumpToMark(nMark);
    _rxOutStream->writeLong(nLen);
    xMark->jumpToFurthest();
    xMark->deleteMark(nMark);

    // 2. Schreiben einer VersionsNummer
    _rxOutStream->writeShort(0x0003);

    // 3. Schreiben der allgemeinen Properties
    ::comphelper::operator<<( _rxOutStream, m_aName);
    _rxOutStream->writeShort(m_nTabIndex);
    ::comphelper::operator<<( _rxOutStream, m_aTag); // 3. version

    // !!! IMPORTANT NOTE !!!
    // don't write any new members here : this wouldn't be compatible with older versions, as OControlModel
    // is a base class which is called in derived classes "read" method. So if you increment the version
    // and write new stuff, older office versions will read this in the _derived_ classes, which may result
    // in anything from data loss to crash.
    // !!! EOIN !!!
}

//------------------------------------------------------------------------------
void OControlModel::read(const Reference<stario::XObjectInputStream>& InStream) throw (::com::sun::star::io::IOException, RuntimeException)
{
    osl::MutexGuard aGuard(m_aMutex);

    Reference<stario::XMarkableStream> xMark(InStream, UNO_QUERY);
    if ( !xMark.is() )
    {
        throw IOException(
            FRM_RES_STRING( RID_STR_INVALIDSTREAM ),
            static_cast< ::cppu::OWeakObject* >( this )
        );
    }

    // 1. Lesen des UnoControls
    sal_Int32 nLen = InStream->readLong();
    if (nLen)
    {
        sal_Int32 nMark = xMark->createMark();

        try
        {
            readAggregate( InStream );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }

        xMark->jumpToMark(nMark);
        InStream->skipBytes(nLen);
        xMark->deleteMark(nMark);
    }

    // 2. Lesen des Versionsnummer
    sal_uInt16 nVersion = InStream->readShort();

    // 3. Lesen der allgemeinen Properties
    ::comphelper::operator>>( InStream, m_aName);
    m_nTabIndex  = InStream->readShort();

    if (nVersion > 0x0002)
        ::comphelper::operator>>( InStream, m_aTag);

    // we had a version where we wrote the help text
    if (nVersion == 0x0004)
        readHelpTextCompatibly(InStream);

    DBG_ASSERT(nVersion < 5, "OControlModel::read : suspicious version number !");
    // 4 was the version where we wrote the help text
    // later versions shouldn't exist (see write for a detailed comment)
}

//------------------------------------------------------------------------------
PropertyState OControlModel::getPropertyStateByHandle( sal_Int32 _nHandle )
{
    // simply compare the current and the default value
    Any aCurrentValue = getPropertyDefaultByHandle( _nHandle );
    Any aDefaultValue;  getFastPropertyValue( aDefaultValue, _nHandle );

    sal_Bool bEqual = uno_type_equalData(
            const_cast< void* >( aCurrentValue.getValue() ), aCurrentValue.getValueType().getTypeLibType(),
            const_cast< void* >( aDefaultValue.getValue() ), aDefaultValue.getValueType().getTypeLibType(),
            reinterpret_cast< uno_QueryInterfaceFunc >(cpp_queryInterface),
            reinterpret_cast< uno_ReleaseFunc >(cpp_release)
        );
    return bEqual ? PropertyState_DEFAULT_VALUE : PropertyState_DIRECT_VALUE;
}

//------------------------------------------------------------------------------
void OControlModel::setPropertyToDefaultByHandle( sal_Int32 _nHandle)
{
    Any aDefault = getPropertyDefaultByHandle( _nHandle );

    Any aConvertedValue, aOldValue;
    if ( convertFastPropertyValue( aConvertedValue, aOldValue, _nHandle, aDefault ) )
    {
        setFastPropertyValue_NoBroadcast( _nHandle, aConvertedValue );
        // TODO: fire the property change
    }
}

//------------------------------------------------------------------------------
Any OControlModel::getPropertyDefaultByHandle( sal_Int32 _nHandle ) const
{
    Any aReturn;
    switch ( _nHandle )
    {
        case PROPERTY_ID_NAME:
        case PROPERTY_ID_TAG:
            aReturn <<= ::rtl::OUString();
            break;

        case PROPERTY_ID_CLASSID:
            aReturn <<= (sal_Int16)FormComponentType::CONTROL;
            break;

        case PROPERTY_ID_TABINDEX:
            aReturn <<= (sal_Int16)FRM_DEFAULT_TABINDEX;
            break;

        case PROPERTY_ID_NATIVE_LOOK:
            aReturn <<= (sal_Bool)sal_True;
            break;

        case PROPERTY_ID_GENERATEVBAEVENTS:
            aReturn <<= (sal_Bool)sal_False;
            break;


        default:
            if ( m_aPropertyBagHelper.hasDynamicPropertyByHandle( _nHandle ) )
                m_aPropertyBagHelper.getDynamicPropertyDefaultByHandle( _nHandle, aReturn );
            else
                OSL_FAIL( "OControlModel::convertFastPropertyValue: unknown handle!" );
    }
    return aReturn;
}

//------------------------------------------------------------------------------
void OControlModel::getFastPropertyValue( Any& _rValue, sal_Int32 _nHandle ) const
{
    switch ( _nHandle )
    {
        case PROPERTY_ID_NAME:
            _rValue <<= m_aName;
            break;
        case PROPERTY_ID_TAG:
            _rValue <<= m_aTag;
            break;
        case PROPERTY_ID_CLASSID:
            _rValue <<= m_nClassId;
            break;
        case PROPERTY_ID_TABINDEX:
            _rValue <<= m_nTabIndex;
            break;
        case PROPERTY_ID_NATIVE_LOOK:
            _rValue <<= (sal_Bool)m_bNativeLook;
            break;
        case PROPERTY_ID_GENERATEVBAEVENTS:
            _rValue <<= (sal_Bool)m_bGenerateVbEvents;
        default:
            if ( m_aPropertyBagHelper.hasDynamicPropertyByHandle( _nHandle ) )
                m_aPropertyBagHelper.getDynamicFastPropertyValue( _nHandle, _rValue );
            else
                OPropertySetAggregationHelper::getFastPropertyValue( _rValue, _nHandle );
            break;
    }
}

//------------------------------------------------------------------------------
sal_Bool OControlModel::convertFastPropertyValue(
                        Any& _rConvertedValue, Any& _rOldValue, sal_Int32 _nHandle, const Any& _rValue)
                        throw (com::sun::star::lang::IllegalArgumentException)
{
    sal_Bool bModified(sal_False);
    switch (_nHandle)
    {
        case PROPERTY_ID_NAME:
            bModified = tryPropertyValue(_rConvertedValue, _rOldValue, _rValue, m_aName);
            break;
        case PROPERTY_ID_TAG:
            bModified = tryPropertyValue(_rConvertedValue, _rOldValue, _rValue, m_aTag);
            break;
        case PROPERTY_ID_TABINDEX:
            bModified = tryPropertyValue(_rConvertedValue, _rOldValue, _rValue, m_nTabIndex);
            break;
        case PROPERTY_ID_NATIVE_LOOK:
            bModified = tryPropertyValue(_rConvertedValue, _rOldValue, _rValue, m_bNativeLook);
            break;
        case PROPERTY_ID_GENERATEVBAEVENTS:
            bModified = tryPropertyValue(_rConvertedValue, _rOldValue, _rValue, m_bGenerateVbEvents);
            break;
        default:
            if ( m_aPropertyBagHelper.hasDynamicPropertyByHandle( _nHandle ) )
                bModified = m_aPropertyBagHelper.convertDynamicFastPropertyValue( _nHandle, _rValue, _rConvertedValue, _rOldValue );
            else
                OSL_FAIL( "OControlModel::convertFastPropertyValue: unknown handle!" );
            break;
    }
    return bModified;
}

//------------------------------------------------------------------------------
void OControlModel::setFastPropertyValue_NoBroadcast(sal_Int32 _nHandle, const Any& _rValue)
                        throw (Exception)
{
    switch (_nHandle)
    {
        case PROPERTY_ID_NAME:
            DBG_ASSERT(_rValue.getValueType() == getCppuType((const ::rtl::OUString*)NULL),
                "OControlModel::setFastPropertyValue_NoBroadcast : invalid type" );
            _rValue >>= m_aName;
            break;
        case PROPERTY_ID_TAG:
            DBG_ASSERT(_rValue.getValueType() == getCppuType((const ::rtl::OUString*)NULL),
                "OControlModel::setFastPropertyValue_NoBroadcast : invalid type" );
            _rValue >>= m_aTag;
            break;
        case PROPERTY_ID_TABINDEX:
            DBG_ASSERT(_rValue.getValueType() == getCppuType((const sal_Int16*)NULL),
                "OControlModel::setFastPropertyValue_NoBroadcast : invalid type" );
            _rValue >>= m_nTabIndex;
            break;
        case PROPERTY_ID_NATIVE_LOOK:
            OSL_VERIFY( _rValue >>= m_bNativeLook );
            break;
        case PROPERTY_ID_GENERATEVBAEVENTS:
            OSL_VERIFY( _rValue >>= m_bGenerateVbEvents );
            break;
        default:
            if ( m_aPropertyBagHelper.hasDynamicPropertyByHandle( _nHandle ) )
                m_aPropertyBagHelper.setDynamicFastPropertyValue( _nHandle, _rValue );
            else
                OSL_FAIL( "OControlModel::setFastPropertyValue_NoBroadcast: unknown handle!" );
            break;
    }
}

//------------------------------------------------------------------------------
void OControlModel::describeFixedProperties( Sequence< Property >& _rProps ) const
{
    BEGIN_DESCRIBE_BASE_PROPERTIES( 5 )
        DECL_PROP2      (CLASSID,     sal_Int16,        READONLY, TRANSIENT);
        DECL_PROP1      (NAME,        ::rtl::OUString,  BOUND);
        DECL_BOOL_PROP2 (NATIVE_LOOK,                   BOUND, TRANSIENT);
        DECL_PROP1      (TAG,         ::rtl::OUString,  BOUND);
        DECL_PROP1      (GENERATEVBAEVENTS,         sal_Bool,  TRANSIENT);
    END_DESCRIBE_PROPERTIES()
}

//------------------------------------------------------------------------------
void OControlModel::describeAggregateProperties( Sequence< Property >& /* [out] */ _rAggregateProps ) const
{
    if ( m_xAggregateSet.is() )
    {
        Reference< XPropertySetInfo > xPSI( m_xAggregateSet->getPropertySetInfo() );
        if ( xPSI.is() )
            _rAggregateProps = xPSI->getProperties();
    }
}

//------------------------------------------------------------------------------
::osl::Mutex& OControlModel::getMutex()
{
    return m_aMutex;
}

//------------------------------------------------------------------------------
void OControlModel::describeFixedAndAggregateProperties( Sequence< Property >& _out_rFixedProperties, Sequence< Property >& _out_rAggregateProperties ) const
{
    describeFixedProperties( _out_rFixedProperties );
    describeAggregateProperties( _out_rAggregateProperties );
}

//------------------------------------------------------------------------------
Reference< XMultiPropertySet > OControlModel::getPropertiesInterface()
{
    return Reference< XMultiPropertySet >( *this, UNO_QUERY );
}

//------------------------------------------------------------------------------
Reference< XPropertySetInfo> SAL_CALL OControlModel::getPropertySetInfo() throw( RuntimeException)
{
    return createPropertySetInfo( getInfoHelper() );
}

//------------------------------------------------------------------------------
::cppu::IPropertyArrayHelper& OControlModel::getInfoHelper()
{
    return m_aPropertyBagHelper.getInfoHelper();
}

//--------------------------------------------------------------------
void SAL_CALL OControlModel::addProperty( const ::rtl::OUString& _rName, ::sal_Int16 _nAttributes, const Any& _rInitialValue ) throw (PropertyExistException, IllegalTypeException, IllegalArgumentException, RuntimeException)
{
    m_aPropertyBagHelper.addProperty( _rName, _nAttributes, _rInitialValue );
}

//--------------------------------------------------------------------
void SAL_CALL OControlModel::removeProperty( const ::rtl::OUString& _rName ) throw (UnknownPropertyException, NotRemoveableException, RuntimeException)
{
    m_aPropertyBagHelper.removeProperty( _rName );
}

//--------------------------------------------------------------------
Sequence< PropertyValue > SAL_CALL OControlModel::getPropertyValues() throw (RuntimeException)
{
    return m_aPropertyBagHelper.getPropertyValues();
}

//--------------------------------------------------------------------
void SAL_CALL OControlModel::setPropertyValues( const Sequence< PropertyValue >& _rProps ) throw (UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    m_aPropertyBagHelper.setPropertyValues( _rProps );
}

//--------------------------------------------------------------------
void OControlModel::lockInstance( LockAccess )
{
    m_aMutex.acquire();
    osl_atomic_increment( &m_lockCount );
}

//--------------------------------------------------------------------
oslInterlockedCount OControlModel::unlockInstance( LockAccess )
{
    OSL_ENSURE( m_lockCount > 0, "OControlModel::unlockInstance: not locked!" );
    oslInterlockedCount lockCount = osl_atomic_decrement( &m_lockCount );
    m_aMutex.release();
    return lockCount;
}

//--------------------------------------------------------------------
void OControlModel::firePropertyChanges( const Sequence< sal_Int32 >& _rHandles, const Sequence< Any >& _rOldValues,
                                        const Sequence< Any >& _rNewValues, LockAccess )
{
    OPropertySetHelper::fire(
        const_cast< Sequence< sal_Int32 >& >( _rHandles ).getArray(),
        _rNewValues.getConstArray(),
        _rOldValues.getConstArray(),
        _rHandles.getLength(),
        sal_False
    );
}

//==================================================================
//= OBoundControlModel
//==================================================================
DBG_NAME(frm_OBoundControlModel);
//------------------------------------------------------------------
Any SAL_CALL OBoundControlModel::queryAggregation( const Type& _rType ) throw (RuntimeException)
{
    Any aReturn( OControlModel::queryAggregation(_rType) );
    if (!aReturn.hasValue())
    {
        aReturn = OBoundControlModel_BASE1::queryInterface(_rType);

        if ( !aReturn.hasValue() && m_bCommitable )
            aReturn = OBoundControlModel_COMMITTING::queryInterface( _rType );

        if ( !aReturn.hasValue() && m_bSupportsExternalBinding )
            aReturn = OBoundControlModel_BINDING::queryInterface( _rType );

        if ( !aReturn.hasValue() && m_bSupportsValidation )
            aReturn = OBoundControlModel_VALIDATION::queryInterface( _rType );
    }

    return aReturn;
}

//------------------------------------------------------------------
OBoundControlModel::OBoundControlModel(
        const Reference< XMultiServiceFactory>& _rxFactory,
        const ::rtl::OUString& _rUnoControlModelTypeName, const ::rtl::OUString& _rDefault,
        const sal_Bool _bCommitable, const sal_Bool _bSupportExternalBinding, const sal_Bool _bSupportsValidation )
    :OControlModel( _rxFactory, _rUnoControlModelTypeName, _rDefault, sal_False )
    ,OPropertyChangeListener( m_aMutex )
    ,m_xField()
    ,m_xAmbientForm()
    ,m_nValuePropertyAggregateHandle( -1 )
    ,m_nFieldType( DataType::OTHER )
    ,m_bValuePropertyMayBeVoid( false )
    ,m_aResetHelper( *this, m_aMutex )
    ,m_aUpdateListeners(m_aMutex)
    ,m_aFormComponentListeners( m_aMutex )
    ,m_bInputRequired( sal_True )
    ,m_pAggPropMultiplexer( NULL )
    ,m_bFormListening( false )
    ,m_bLoaded(sal_False)
    ,m_bRequired(sal_False)
    ,m_bCommitable(_bCommitable)
    ,m_bSupportsExternalBinding( _bSupportExternalBinding )
    ,m_bSupportsValidation( _bSupportsValidation )
    ,m_bForwardValueChanges(sal_True)
    ,m_bTransferingValue( sal_False )
    ,m_bIsCurrentValueValid( sal_True )
    ,m_bBindingControlsRO( sal_False )
    ,m_bBindingControlsEnable( sal_False )
    ,m_eControlValueChangeInstigator( eOther )
    ,m_aLabelServiceName(FRM_SUN_COMPONENT_FIXEDTEXT)
{
    DBG_CTOR(frm_OBoundControlModel, NULL);

    // start property listening at the aggregate
    implInitAggMultiplexer( );
}

//------------------------------------------------------------------
OBoundControlModel::OBoundControlModel(
        const OBoundControlModel* _pOriginal, const Reference< XMultiServiceFactory>& _rxFactory )
    :OControlModel( _pOriginal, _rxFactory, sal_True, sal_False )
    ,OPropertyChangeListener( m_aMutex )
    ,m_xField()
    ,m_xAmbientForm()
    ,m_nValuePropertyAggregateHandle( _pOriginal->m_nValuePropertyAggregateHandle )
    ,m_nFieldType( DataType::OTHER )
    ,m_bValuePropertyMayBeVoid( _pOriginal->m_bValuePropertyMayBeVoid )
    ,m_aResetHelper( *this, m_aMutex )
    ,m_aUpdateListeners( m_aMutex )
    ,m_aFormComponentListeners( m_aMutex )
    ,m_xValidator( _pOriginal->m_xValidator )
    ,m_bInputRequired( sal_True )
    ,m_pAggPropMultiplexer( NULL )
    ,m_bFormListening( false )
    ,m_bLoaded( sal_False )
    ,m_bRequired( sal_False )
    ,m_bCommitable( _pOriginal->m_bCommitable )
    ,m_bSupportsExternalBinding( _pOriginal->m_bSupportsExternalBinding )
    ,m_bSupportsValidation( _pOriginal->m_bSupportsValidation )
    ,m_bForwardValueChanges( sal_True )
    ,m_bTransferingValue( sal_False )
    ,m_bIsCurrentValueValid( _pOriginal->m_bIsCurrentValueValid )
    ,m_bBindingControlsRO( sal_False )
    ,m_bBindingControlsEnable( sal_False )
    ,m_eControlValueChangeInstigator( eOther )
{
    DBG_CTOR(frm_OBoundControlModel, NULL);

    // start property listening at the aggregate
    implInitAggMultiplexer( );

    m_aLabelServiceName = _pOriginal->m_aLabelServiceName;
    m_sValuePropertyName = _pOriginal->m_sValuePropertyName;
    m_nValuePropertyAggregateHandle = _pOriginal->m_nValuePropertyAggregateHandle;
    m_bValuePropertyMayBeVoid = _pOriginal->m_bValuePropertyMayBeVoid;
    m_aValuePropertyType = _pOriginal->m_aValuePropertyType;
    m_aControlSource = _pOriginal->m_aControlSource;
    m_bInputRequired = _pOriginal->m_bInputRequired;
    // m_xLabelControl, though being a property, is not to be cloned, not even the reference will be transfered.
    // (the former should be clear - a clone of the object we're only referencing does not make sense)
    // (the second would violate the restriction for label controls that they're part of the
    // same form component hierarchy - we ourself are no part, yet, so we can't have a label control)

    // start listening for changes at the value property
    implInitValuePropertyListening( );
}

//------------------------------------------------------------------
OBoundControlModel::~OBoundControlModel()
{
    if ( !OComponentHelper::rBHelper.bDisposed )
    {
        acquire();
        dispose();
    }

    doResetDelegator( );

    OSL_ENSURE( m_pAggPropMultiplexer, "OBoundControlModel::~OBoundControlModel: what about my property multiplexer?" );
    if ( m_pAggPropMultiplexer )
    {
        m_pAggPropMultiplexer->dispose();
        m_pAggPropMultiplexer->release();
        m_pAggPropMultiplexer = NULL;
    }

    DBG_DTOR(frm_OBoundControlModel, NULL);
}

//------------------------------------------------------------------
void OBoundControlModel::clonedFrom( const OControlModel* _pOriginal )
{
    const OBoundControlModel* pBoundOriginal = static_cast< const OBoundControlModel* >( _pOriginal );
    // the value binding can be handled as if somebody called setValueBinding here
    // By definition, bindings can be share between bindables
    if ( pBoundOriginal && pBoundOriginal->m_xExternalBinding.is() )
    {
        try
        {
            setValueBinding( pBoundOriginal->m_xExternalBinding );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }
}

//-----------------------------------------------------------------------------
void OBoundControlModel::implInitAggMultiplexer( )
{
    increment( m_refCount );
    if ( m_xAggregateSet.is() )
    {
        m_pAggPropMultiplexer = new OPropertyChangeMultiplexer( this, m_xAggregateSet, sal_False );
        m_pAggPropMultiplexer->acquire();
    }
    decrement( m_refCount );

       doSetDelegator();
}

//-----------------------------------------------------------------------------
void OBoundControlModel::implInitValuePropertyListening( ) const
{
    // start listening for changes at the value property
    // There are three pre-requisites for this to be done:
    // 1. We support external value bindings. In this case, the changes in the control value need to
    //    be propagated to the external binding immediately when they happen
    // 2. We support external validation. In this case, we need to listen for changes in the value
    //    property, since we need to revalidate then.
    // 3. We are not committable. In this case, changes in the control value need to be propagated
    //    to the database column immediately when they happen.
    if ( m_bSupportsExternalBinding || m_bSupportsValidation || !m_bCommitable )
    {
        OSL_ENSURE( m_pAggPropMultiplexer, "OBoundControlModel::implInitValuePropertyListening: no multiplexer!" );
        if ( m_pAggPropMultiplexer && !m_sValuePropertyName.isEmpty() )
            m_pAggPropMultiplexer->addProperty( m_sValuePropertyName );
    }
}

//-----------------------------------------------------------------------------
void OBoundControlModel::initOwnValueProperty( const ::rtl::OUString& i_rValuePropertyName )
{
    OSL_PRECOND( m_sValuePropertyName.isEmpty() && -1 == m_nValuePropertyAggregateHandle,
        "OBoundControlModel::initOwnValueProperty: value property is already initialized!" );
    OSL_ENSURE( !i_rValuePropertyName.isEmpty(), "OBoundControlModel::initOwnValueProperty: invalid property name!" );
    m_sValuePropertyName = i_rValuePropertyName;
}

//-----------------------------------------------------------------------------
void OBoundControlModel::initValueProperty( const ::rtl::OUString& _rValuePropertyName, sal_Int32 _nValuePropertyExternalHandle )
{
    OSL_PRECOND( m_sValuePropertyName.isEmpty() && -1 == m_nValuePropertyAggregateHandle,
        "OBoundControlModel::initValueProperty: value property is already initialized!" );
    OSL_ENSURE( !_rValuePropertyName.isEmpty(), "OBoundControlModel::initValueProperty: invalid property name!" );
    OSL_ENSURE( _nValuePropertyExternalHandle != -1, "OBoundControlModel::initValueProperty: invalid property handle!" );

    m_sValuePropertyName = _rValuePropertyName;
    m_nValuePropertyAggregateHandle = getOriginalHandle( _nValuePropertyExternalHandle );
    OSL_ENSURE( m_nValuePropertyAggregateHandle != -1, "OBoundControlModel::initValueProperty: unable to find the original handle!" );

    if ( m_nValuePropertyAggregateHandle != -1 )
    {
        Reference< XPropertySetInfo > xPropInfo( m_xAggregateSet->getPropertySetInfo(), UNO_SET_THROW );
        Property aValuePropDesc = xPropInfo->getPropertyByName( m_sValuePropertyName );
        m_aValuePropertyType = aValuePropDesc.Type;
        m_bValuePropertyMayBeVoid = ( aValuePropDesc.Attributes & PropertyAttribute::MAYBEVOID ) != 0;
    }

    // start listening for changes at the value property
    implInitValuePropertyListening( );
}

//-----------------------------------------------------------------------------
void OBoundControlModel::suspendValueListening( )
{
    OSL_PRECOND( !m_sValuePropertyName.isEmpty(), "OBoundControlModel::suspendValueListening: don't have a value property!" );
    OSL_PRECOND( m_pAggPropMultiplexer, "OBoundControlModel::suspendValueListening: I *am* not listening!" );

    if ( m_pAggPropMultiplexer )
        m_pAggPropMultiplexer->lock();
}

//-----------------------------------------------------------------------------
void OBoundControlModel::resumeValueListening( )
{
    OSL_PRECOND( !m_sValuePropertyName.isEmpty(), "OBoundControlModel::resumeValueListening: don't have a value property!" );
    OSL_PRECOND( m_pAggPropMultiplexer, "OBoundControlModel::resumeValueListening: I *am* not listening at all!" );
    OSL_PRECOND( !m_pAggPropMultiplexer || m_pAggPropMultiplexer->locked(), "OBoundControlModel::resumeValueListening: listening not suspended currently!" );

    if ( m_pAggPropMultiplexer )
        m_pAggPropMultiplexer->unlock();
}

//-----------------------------------------------------------------------------
Sequence< Type > OBoundControlModel::_getTypes()
{
    TypeBag aTypes(
        OControlModel::_getTypes(),
        OBoundControlModel_BASE1::getTypes()
    );

    if ( m_bCommitable )
        aTypes.addTypes( OBoundControlModel_COMMITTING::getTypes() );

    if ( m_bSupportsExternalBinding )
        aTypes.addTypes( OBoundControlModel_BINDING::getTypes() );

    if ( m_bSupportsValidation )
        aTypes.addTypes( OBoundControlModel_VALIDATION::getTypes() );

    return aTypes.getTypes();
}

// OComponentHelper
//-----------------------------------------------------------------------------
void OBoundControlModel::disposing()
{
    OControlModel::disposing();

    ::osl::ClearableMutexGuard aGuard(m_aMutex);

    if ( m_pAggPropMultiplexer )
        m_pAggPropMultiplexer->dispose();

    // notify all our listeners
    com::sun::star::lang::EventObject aEvt( static_cast< XWeak* >( this ) );
    m_aUpdateListeners.disposeAndClear( aEvt );
    m_aResetHelper.disposing();

    // disconnect from our database column
    // TODO: could we replace the following 5 lines with a call to impl_disconnectDatabaseColumn_noNotify?
    // The only more thing which it does is calling onDisconnectedDbColumn - could this
    // cause trouble? At least when we continue to call OControlModel::disposing before, it *may*.
    if ( hasField() )
    {
        getField()->removePropertyChangeListener( PROPERTY_VALUE, this );
        resetField();
    }
    m_xCursor = NULL;

    Reference< XComponent > xComp( m_xLabelControl, UNO_QUERY );
    if ( xComp.is() )
        xComp->removeEventListener(static_cast< XEventListener* >( static_cast< XPropertyChangeListener* >( this ) ) );

    // disconnect from our external value binding
    if ( hasExternalValueBinding() )
        disconnectExternalValueBinding();

    // dito for the validator
    if ( hasValidator() )
        disconnectValidator( );
}

//------------------------------------------------------------------------------
void OBoundControlModel::onValuePropertyChange( ControlModelLock& i_rControLock )
{
    if ( hasExternalValueBinding() )
    {   // the control value changed, while we have an external value binding
        // -> forward the value to it
        if ( m_eControlValueChangeInstigator != eExternalBinding )
            transferControlValueToExternal( i_rControLock );
    }
    else if ( !m_bCommitable && m_xColumnUpdate.is() )
    {   // the control value changed, while we are  bound to a database column,
        // but not committable (which means changes in the control have to be reflected to
        // the underlying database column immediately)
        // -> forward the value to the database column
        if ( m_eControlValueChangeInstigator != eDbColumnBinding )
            commitControlValueToDbColumn( false );
    }

    // validate the new value
    if ( m_bSupportsValidation )
        recheckValidity( true );
}

//------------------------------------------------------------------------------
void OBoundControlModel::_propertyChanged( const PropertyChangeEvent& _rEvt ) throw ( RuntimeException )
{
    ControlModelLock aLock( *this );

    OSL_ENSURE( _rEvt.PropertyName == m_sValuePropertyName,
        "OBoundControlModel::_propertyChanged: where did this come from (1)?" );
    OSL_ENSURE( m_pAggPropMultiplexer && !m_pAggPropMultiplexer->locked(),
        "OBoundControlModel::_propertyChanged: where did this come from (2)?" );

    if ( _rEvt.PropertyName == m_sValuePropertyName )
    {
        onValuePropertyChange( aLock );
    }
}

//------------------------------------------------------------------------------
void OBoundControlModel::startAggregatePropertyListening( const ::rtl::OUString& _rPropertyName )
{
    OSL_PRECOND( m_pAggPropMultiplexer, "OBoundControlModel::startAggregatePropertyListening: no multiplexer!" );
    OSL_ENSURE( !_rPropertyName.isEmpty(), "OBoundControlModel::startAggregatePropertyListening: invalid property name!" );

    if ( m_pAggPropMultiplexer && !_rPropertyName.isEmpty() )
    {
        m_pAggPropMultiplexer->addProperty( _rPropertyName );
    }
}

//------------------------------------------------------------------------------
void OBoundControlModel::doFormListening( const bool _bStart )
{
    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::doFormListening: external value binding should overrule the database binding!" );

    if ( isFormListening() == _bStart )
        return;

    if ( m_xAmbientForm.is() )
        _bStart ? m_xAmbientForm->addLoadListener( this ) : m_xAmbientForm->removeLoadListener( this );

    Reference< XLoadable > xParentLoadable( getParent(), UNO_QUERY );
    if ( getParent().is() && !xParentLoadable.is() )
    {
        // if our parent does not directly support the XLoadable interface, then it might support the
        // XRowSetSupplier/XRowSetChangeBroadcaster interfaces. In this case we have to listen for changes
        // broadcasted by the latter.
        Reference< XRowSetChangeBroadcaster > xRowSetBroadcaster( getParent(), UNO_QUERY );
        if ( xRowSetBroadcaster.is() )
            _bStart ? xRowSetBroadcaster->addRowSetChangeListener( this ) : xRowSetBroadcaster->removeRowSetChangeListener( this );
    }

    m_bFormListening = _bStart && m_xAmbientForm.is();
}

// XChild
//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::setParent(const Reference<XInterface>& _rxParent) throw(com::sun::star::lang::NoSupportException, RuntimeException)
{
    ControlModelLock aLock( *this );
    FieldChangeNotifier aBoundFieldNotifier( aLock );

    if ( getParent() == _rxParent )
        return;

    // disconnect from database column (which is controlled by parent, directly or indirectly)
    if ( hasField() )
        impl_disconnectDatabaseColumn_noNotify();

    // log off old listeners
    if ( isFormListening() )
        doFormListening( false );

    // actually set the new parent
    OControlModel::setParent( _rxParent );

    // a new parent means a new ambient form
    impl_determineAmbientForm_nothrow();

    if ( !hasExternalValueBinding() )
    {
        // log on new listeners
        doFormListening( true );

        // re-connect to database column of the new parent
        if ( m_xAmbientForm.is() && m_xAmbientForm->isLoaded() )
            impl_connectDatabaseColumn_noNotify( false );
    }
}

// XEventListener
//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::disposing(const com::sun::star::lang::EventObject& _rEvent) throw (RuntimeException)
{
    ControlModelLock aLock( *this );

    if ( _rEvent.Source == getField() )
    {
        resetField();
    }
    else if ( _rEvent.Source == m_xLabelControl )
    {
        Reference<XPropertySet> xOldValue = m_xLabelControl;
        m_xLabelControl = NULL;

        // fire a propertyChanged (when we leave aLock's scope)
        aLock.addPropertyNotification( PROPERTY_ID_CONTROLLABEL, makeAny( xOldValue ), makeAny( m_xLabelControl ) );
    }
    else if ( _rEvent.Source == m_xExternalBinding )
    {   // *first* check for the external binding
        disconnectExternalValueBinding( );
    }
    else if ( _rEvent.Source == m_xValidator )
    {   // *then* check for the validator. Reason is that bindings may also act as validator at the same
        // time, in this case, the validator is automatically revoked when the binding is revoked
        disconnectValidator( );
    }
    else
        OControlModel::disposing(_rEvent);
}

// XServiceInfo
//------------------------------------------------------------------------------
StringSequence SAL_CALL OBoundControlModel::getSupportedServiceNames() throw(RuntimeException)
{
    return ::comphelper::concatSequences(
        getAggregateServiceNames(),
        getSupportedServiceNames_Static()
    );
}

//------------------------------------------------------------------------------
Sequence< ::rtl::OUString > SAL_CALL OBoundControlModel::getSupportedServiceNames_Static() throw( RuntimeException )
{
    Sequence< ::rtl::OUString > aOwnServiceNames( 1 );
    aOwnServiceNames[ 0 ] = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.form.DataAwareControlModel") );

    return ::comphelper::concatSequences(
        OControlModel::getSupportedServiceNames_Static(),
        aOwnServiceNames
    );
}

// XPersist
//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::write( const Reference<stario::XObjectOutputStream>& _rxOutStream ) throw(stario::IOException, RuntimeException)
{
    OControlModel::write(_rxOutStream);

    osl::MutexGuard aGuard(m_aMutex);

    // Version
    _rxOutStream->writeShort(0x0002);

    // Controlsource
    ::comphelper::operator<<( _rxOutStream, m_aControlSource);

    // !!! IMPORTANT NOTE !!!
    // don't write any new members here : this wouldn't be compatible with older versions, as OBoundControlModel
    // is a base class which is called in derived classes "read" method. So if you increment the version
    // and write new stuff, older office versions will read this in the _derived_ classes, which may result
    // in anything from data loss to crash.
    // (use writeCommonProperties instead, this is called in derived classes write-method)
    // !!! EOIN !!!
}

//------------------------------------------------------------------------------
void OBoundControlModel::defaultCommonProperties()
{
    Reference<com::sun::star::lang::XComponent> xComp(m_xLabelControl, UNO_QUERY);
    if (xComp.is())
        xComp->removeEventListener(static_cast<com::sun::star::lang::XEventListener*>(static_cast<XPropertyChangeListener*>(this)));
    m_xLabelControl = NULL;
}

//------------------------------------------------------------------------------
void OBoundControlModel::readCommonProperties(const Reference<stario::XObjectInputStream>& _rxInStream)
{
    sal_Int32 nLen = _rxInStream->readLong();

    Reference<stario::XMarkableStream> xMark(_rxInStream, UNO_QUERY);
    DBG_ASSERT(xMark.is(), "OBoundControlModel::readCommonProperties : can only work with markable streams !");
    sal_Int32 nMark = xMark->createMark();

    // read the reference to the label control
    Reference<stario::XPersistObject> xPersist;
    sal_Int32 nUsedFlag;
    nUsedFlag = _rxInStream->readLong();
    if (nUsedFlag)
        xPersist = _rxInStream->readObject();
    m_xLabelControl = m_xLabelControl.query( xPersist );
    Reference< XComponent > xComp( m_xLabelControl, UNO_QUERY );
    if (xComp.is())
        xComp->addEventListener(static_cast<com::sun::star::lang::XEventListener*>(static_cast<XPropertyChangeListener*>(this)));

    // read any other new common properties here

    // skip the remaining bytes
    xMark->jumpToMark(nMark);
    _rxInStream->skipBytes(nLen);
    xMark->deleteMark(nMark);
}

//------------------------------------------------------------------------------
void OBoundControlModel::writeCommonProperties(const Reference<stario::XObjectOutputStream>& _rxOutStream)
{
    Reference<stario::XMarkableStream> xMark(_rxOutStream, UNO_QUERY);
    DBG_ASSERT(xMark.is(), "OBoundControlModel::writeCommonProperties : can only work with markable streams !");
    sal_Int32 nMark = xMark->createMark();

    // a placeholder where we will write the overall length (later in this method)
    sal_Int32 nLen = 0;
    _rxOutStream->writeLong(nLen);

    // write the reference to the label control
    Reference<stario::XPersistObject> xPersist(m_xLabelControl, UNO_QUERY);
    sal_Int32 nUsedFlag = 0;
    if (xPersist.is())
        nUsedFlag = 1;
    _rxOutStream->writeLong(nUsedFlag);
    if (xPersist.is())
        _rxOutStream->writeObject(xPersist);

    // write any other new common properties here

    // write the correct length at the beginning of the block
    nLen = xMark->offsetToMark(nMark) - sizeof(nLen);
    xMark->jumpToMark(nMark);
    _rxOutStream->writeLong(nLen);
    xMark->jumpToFurthest();
    xMark->deleteMark(nMark);
}

//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::read( const Reference< stario::XObjectInputStream >& _rxInStream ) throw(stario::IOException, RuntimeException)
{
    OControlModel::read(_rxInStream);

    osl::MutexGuard aGuard(m_aMutex);
    sal_uInt16 nVersion = _rxInStream->readShort(); (void)nVersion;
    ::comphelper::operator>>( _rxInStream, m_aControlSource);
}

//------------------------------------------------------------------------------
void OBoundControlModel::getFastPropertyValue(Any& rValue, sal_Int32 nHandle) const
{
    switch (nHandle)
    {
        case PROPERTY_ID_INPUT_REQUIRED:
            rValue <<= m_bInputRequired;
            break;
        case PROPERTY_ID_CONTROLSOURCEPROPERTY:
            rValue <<= m_sValuePropertyName;
            break;
        case PROPERTY_ID_CONTROLSOURCE:
            rValue <<= m_aControlSource;
            break;
        case PROPERTY_ID_BOUNDFIELD:
            rValue <<= getField();
            break;
        case PROPERTY_ID_CONTROLLABEL:
            if (!m_xLabelControl.is())
                rValue.clear();
            else
                rValue <<= m_xLabelControl;
            break;
        default:
            OControlModel::getFastPropertyValue(rValue, nHandle);
    }
}

//------------------------------------------------------------------------------
sal_Bool OBoundControlModel::convertFastPropertyValue(
                                Any& _rConvertedValue, Any& _rOldValue,
                sal_Int32 _nHandle,
                                const Any& _rValue)
                throw (com::sun::star::lang::IllegalArgumentException)
{
    sal_Bool bModified(sal_False);
    switch (_nHandle)
    {
        case PROPERTY_ID_INPUT_REQUIRED:
            bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, m_bInputRequired );
            break;
        case PROPERTY_ID_CONTROLSOURCE:
            bModified = tryPropertyValue(_rConvertedValue, _rOldValue, _rValue, m_aControlSource);
            break;
        case PROPERTY_ID_BOUNDFIELD:
            OSL_FAIL( "OBoundControlModel::convertFastPropertyValue: BoundField should be a read-only property !" );
            throw com::sun::star::lang::IllegalArgumentException();
        case PROPERTY_ID_CONTROLLABEL:
            if (!_rValue.hasValue())
            {   // property set to void
                _rConvertedValue = Any();
                getFastPropertyValue(_rOldValue, _nHandle);
                bModified = m_xLabelControl.is();
            }
            else
            {
                bModified = tryPropertyValue(_rConvertedValue, _rOldValue, _rValue, m_xLabelControl);
                if (!m_xLabelControl.is())
                    // an empty interface is interpreted as VOID
                    _rOldValue.clear();
            }
            break;
        default:
            bModified = OControlModel::convertFastPropertyValue(_rConvertedValue, _rOldValue, _nHandle, _rValue);
    }
    return bModified;
}

//------------------------------------------------------------------------------
Any OBoundControlModel::getPropertyDefaultByHandle( sal_Int32 _nHandle ) const
{
    Any aDefault;
    switch ( _nHandle )
    {
        case PROPERTY_ID_INPUT_REQUIRED:
            aDefault <<= sal_Bool( sal_True );
            break;

        case PROPERTY_ID_CONTROLSOURCE:
            aDefault <<= ::rtl::OUString();
            break;

        case PROPERTY_ID_CONTROLLABEL:
            aDefault <<= Reference< XPropertySet >();
            break;
    }
    return aDefault;
}

//------------------------------------------------------------------------------
void OBoundControlModel::setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const Any& rValue ) throw (Exception)
{
    switch (nHandle)
    {
        case PROPERTY_ID_INPUT_REQUIRED:
            OSL_VERIFY( rValue >>= m_bInputRequired );
            break;
        case PROPERTY_ID_CONTROLSOURCE:
            OSL_VERIFY( rValue >>= m_aControlSource );
            break;
        case PROPERTY_ID_BOUNDFIELD:
            OSL_FAIL("OBoundControlModel::setFastPropertyValue_NoBroadcast : BoundField should be a read-only property !");
            throw com::sun::star::lang::IllegalArgumentException();
        case PROPERTY_ID_CONTROLLABEL:
        {
            if ( rValue.hasValue() && ( rValue.getValueTypeClass() != TypeClass_INTERFACE ) )
                throw com::sun::star::lang::IllegalArgumentException();

            Reference< XInterface > xNewValue( rValue, UNO_QUERY );
            if ( !xNewValue.is() )
            {   // set property to "void"
                Reference< XComponent > xComp( m_xLabelControl, UNO_QUERY );
                if ( xComp.is() )
                    xComp->removeEventListener( static_cast< XPropertyChangeListener* >( this ) );
                m_xLabelControl = NULL;
                break;
            }

            Reference< XControlModel >  xAsModel        ( xNewValue,        UNO_QUERY );
            Reference< XServiceInfo >   xAsServiceInfo  ( xAsModel,         UNO_QUERY );
            Reference< XPropertySet >   xAsPropSet      ( xAsServiceInfo,   UNO_QUERY );
            Reference< XChild >         xAsChild        ( xAsPropSet,       UNO_QUERY );
            if ( !xAsChild.is() || !xAsServiceInfo->supportsService( m_aLabelServiceName ) )
            {
                throw com::sun::star::lang::IllegalArgumentException();
            }

            // check if weself and the given model have a common anchestor (up to the forms collection)
            Reference<XChild> xCont;
            query_interface(static_cast<XWeak*>(this), xCont);
            Reference< XInterface > xMyTopLevel = xCont->getParent();
            while (xMyTopLevel.is())
            {
                Reference<XForm> xAsForm(xMyTopLevel, UNO_QUERY);
                if (!xAsForm.is())
                    // found my root
                    break;

                Reference<XChild> xLoopAsChild(xMyTopLevel, UNO_QUERY);
                xMyTopLevel = xLoopAsChild.is() ? xLoopAsChild->getParent() : Reference< XInterface >();
            }
            Reference< XInterface > xNewTopLevel = xAsChild->getParent();
            while (xNewTopLevel.is())
            {
                Reference<XForm> xAsForm(xNewTopLevel, UNO_QUERY);
                if (!xAsForm.is())
                    break;

                Reference<XChild> xLoopAsChild(xNewTopLevel, UNO_QUERY);
                xNewTopLevel = xLoopAsChild.is() ? xLoopAsChild->getParent() : Reference< XInterface >();
            }
            if (xNewTopLevel != xMyTopLevel)
            {
                // the both objects don't belong to the same forms collection -> not acceptable
                throw com::sun::star::lang::IllegalArgumentException();
            }

            m_xLabelControl = xAsPropSet;
            Reference<com::sun::star::lang::XComponent> xComp(m_xLabelControl, UNO_QUERY);
            if (xComp.is())
                xComp->addEventListener(static_cast<com::sun::star::lang::XEventListener*>(static_cast<XPropertyChangeListener*>(this)));
        }
        break;
        default:
            OControlModel::setFastPropertyValue_NoBroadcast(nHandle, rValue );
    }
}

// XPropertyChangeListener
//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::propertyChange( const PropertyChangeEvent& evt ) throw(RuntimeException)
{
    // if the DBColumn value changed, transfer it to the control
    if ( evt.PropertyName.equals( PROPERTY_VALUE ) )
    {
        OSL_ENSURE( evt.Source == getField(), "OBoundControlModel::propertyChange: value changes from components other than our database colum?" );
        osl::MutexGuard aGuard(m_aMutex);
        if ( m_bForwardValueChanges && m_xColumn.is() )
            transferDbValueToControl();
    }
    else
    {
        OSL_ENSURE( evt.Source == m_xExternalBinding, "OBoundControlModel::propertyChange: where did this come from?" );

        // our binding has properties which can control properties of ourself
        ::rtl::OUString sBindingControlledProperty;
        bool bForwardToLabelControl = false;
        if ( evt.PropertyName.equals( PROPERTY_READONLY ) )
        {
            sBindingControlledProperty = PROPERTY_READONLY;
        }
        else if ( evt.PropertyName.equals( PROPERTY_RELEVANT ) )
        {
            sBindingControlledProperty = PROPERTY_ENABLED;
            bForwardToLabelControl = true;
        }
        else
            return;

        try
        {
            setPropertyValue( sBindingControlledProperty, evt.NewValue );
            if ( bForwardToLabelControl && m_xLabelControl.is() )
                m_xLabelControl->setPropertyValue( sBindingControlledProperty, evt.NewValue );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "OBoundControlModel::propertyChange: could not adjust my binding-controlled property!" );
        }
    }
}

//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::onRowSetChanged( const EventObject& /*i_Event*/ ) throw (RuntimeException)
{
    ControlModelLock aLock( *this );
    FieldChangeNotifier aBoundFieldNotifier( aLock );

    // disconnect from database column (which is controlled by parent, directly or indirectly)
    if ( hasField() )
        impl_disconnectDatabaseColumn_noNotify();

    // log off old listeners
    if ( isFormListening() )
        doFormListening( false );

    // determine the new ambient form
    impl_determineAmbientForm_nothrow();

    // log on new listeners
    doFormListening( true );

    // re-connect to database column if needed and possible
    if ( m_xAmbientForm.is() && m_xAmbientForm->isLoaded() )
        impl_connectDatabaseColumn_noNotify( false );
}

// XBoundComponent
//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::addUpdateListener(const Reference<XUpdateListener>& _rxListener) throw(RuntimeException)
{
    m_aUpdateListeners.addInterface(_rxListener);
}

//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::removeUpdateListener(const Reference< XUpdateListener>& _rxListener) throw(RuntimeException)
{
    m_aUpdateListeners.removeInterface(_rxListener);
}

//------------------------------------------------------------------------------
sal_Bool SAL_CALL OBoundControlModel::commit() throw(RuntimeException)
{
    ControlModelLock aLock( *this );

    OSL_PRECOND( m_bCommitable, "OBoundControlModel::commit: invalid call (I'm not commitable !) " );
    if ( hasExternalValueBinding() )
    {
        // in most cases, no action is required: For most derivees, we know the value property of
        // our control (see initValueProperty), and when an external binding is active, we
        // instantly forward all changes in this property to the external binding.
        if ( m_sValuePropertyName.isEmpty() )
            // but for those derivees which did not use this feature, we need an
            // explicit transfer
            transferControlValueToExternal( aLock );
        return sal_True;
    }

    OSL_ENSURE( !hasExternalValueBinding(), "OBoundControlModel::commit: control flow broken!" );
        // we reach this only if we're not working with an external binding

    if ( !hasField() )
        return sal_True;

    ::cppu::OInterfaceIteratorHelper aIter( m_aUpdateListeners );
    EventObject aEvent;
    aEvent.Source = static_cast< XWeak* >( this );
    sal_Bool bSuccess = sal_True;

    aLock.release();
    // >>>>>>>> ----- UNSAFE ----- >>>>>>>>
    while (aIter.hasMoreElements() && bSuccess)
        bSuccess = static_cast< XUpdateListener* >( aIter.next() )->approveUpdate( aEvent );
    // <<<<<<<< ----- UNSAFE ----- <<<<<<<<
    aLock.acquire();

    if ( bSuccess )
    {
        try
        {
            if ( m_xColumnUpdate.is() )
                bSuccess = commitControlValueToDbColumn( sal_False );
        }
        catch(const Exception&)
        {
            bSuccess = sal_False;
        }
    }

    if ( bSuccess )
    {
        aLock.release();
        m_aUpdateListeners.notifyEach( &XUpdateListener::updated, aEvent );
    }

    return bSuccess;
}

//------------------------------------------------------------------------------
void OBoundControlModel::resetField()
{
    m_xColumnUpdate.clear();
    m_xColumn.clear();
    m_xField.clear();
    m_nFieldType = DataType::OTHER;
}

//------------------------------------------------------------------------------
sal_Bool OBoundControlModel::connectToField(const Reference<XRowSet>& rForm)
{
    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::connectToField: invalid call (have an external binding)!" );

    // wenn eine Verbindung zur Datenbank existiert
    if (rForm.is() && getConnection(rForm).is())
    {
        // Feld bestimmen und PropertyChangeListener
        m_xCursor = rForm;
        Reference<XPropertySet> xFieldCandidate;

        if (m_xCursor.is())
        {
            Reference<XColumnsSupplier> xColumnsSupplier(m_xCursor, UNO_QUERY);
            DBG_ASSERT(xColumnsSupplier.is(), "OBoundControlModel::connectToField : the row set should support the com::sun::star::sdb::ResultSet service !");
            if (xColumnsSupplier.is())
            {
                Reference<XNameAccess> xColumns(xColumnsSupplier->getColumns(), UNO_QUERY);
                if (xColumns.is() && xColumns->hasByName(m_aControlSource))
                {
                    OSL_VERIFY( xColumns->getByName(m_aControlSource) >>= xFieldCandidate );
                }
            }
        }

        try
        {
            sal_Int32 nFieldType = DataType::OTHER;
            if ( xFieldCandidate.is() )
            {
                xFieldCandidate->getPropertyValue( PROPERTY_FIELDTYPE ) >>= nFieldType;
                if ( approveDbColumnType( nFieldType ) )
                    impl_setField_noNotify( xFieldCandidate );
            }
            else
                impl_setField_noNotify( NULL );

            if ( m_xField.is() )
            {
                if( m_xField->getPropertySetInfo()->hasPropertyByName( PROPERTY_VALUE ) )
                {
                    m_nFieldType = nFieldType;

                    // an wertaenderungen horchen
                    m_xField->addPropertyChangeListener( PROPERTY_VALUE, this );
                    m_xColumnUpdate = Reference< XColumnUpdate >( m_xField, UNO_QUERY );
                    m_xColumn = Reference< XColumn >( m_xField, UNO_QUERY );

                    sal_Int32 nNullableFlag = ColumnValue::NO_NULLS;
                    m_xField->getPropertyValue(PROPERTY_ISNULLABLE) >>= nNullableFlag;
                    m_bRequired = (ColumnValue::NO_NULLS == nNullableFlag);
                        // we're optimistic : in case of ColumnValue_NULLABLE_UNKNOWN we assume nullability ....
                }
                else
                {
                    OSL_FAIL("OBoundControlModel::connectToField: property NAME not supported!");
                    impl_setField_noNotify( NULL );
                }
            }
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
            resetField();
        }
    }
    return hasField();
}

//------------------------------------------------------------------------------
void OBoundControlModel::initFromField( const Reference< XRowSet >& _rxRowSet )
{
    // but only if the rowset if posisitioned on a valid record
    if ( hasField() && _rxRowSet.is() )
    {
        if ( !_rxRowSet->isBeforeFirst() && !_rxRowSet->isAfterLast() )
            transferDbValueToControl();
        else
            // reset the field if the row set is empty
            // #i30661#
            resetNoBroadcast();
    }
}

//------------------------------------------------------------------------------
sal_Bool OBoundControlModel::approveDbColumnType(sal_Int32 _nColumnType)
{
    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::approveDbColumnType: invalid call (have an external binding)!" );

    if ((_nColumnType == DataType::BINARY) || (_nColumnType == DataType::VARBINARY)
        || (_nColumnType == DataType::LONGVARBINARY) || (_nColumnType == DataType::OTHER)
        || (_nColumnType == DataType::OBJECT) || (_nColumnType == DataType::DISTINCT)
        || (_nColumnType == DataType::STRUCT) || (_nColumnType == DataType::ARRAY)
        || (_nColumnType == DataType::BLOB) /*|| (_nColumnType == DataType::CLOB)*/
        || (_nColumnType == DataType::REF) || (_nColumnType == DataType::SQLNULL))
        return sal_False;

    return sal_True;
}

//------------------------------------------------------------------------------
void OBoundControlModel::impl_determineAmbientForm_nothrow()
{
    Reference< XInterface > xParent( const_cast< OBoundControlModel* >( this )->getParent() );

    m_xAmbientForm.set( xParent, UNO_QUERY );
    if ( !m_xAmbientForm.is() )
    {
        Reference< XRowSetSupplier > xSupRowSet( xParent, UNO_QUERY );
        if ( xSupRowSet.is() )
            m_xAmbientForm.set( xSupRowSet->getRowSet(), UNO_QUERY );
    }
}

//------------------------------------------------------------------------------
void OBoundControlModel::impl_connectDatabaseColumn_noNotify( bool _bFromReload )
{
    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::impl_connectDatabaseColumn_noNotify: not to be called with an external value binding!" );

    // consistency checks
    DBG_ASSERT( !( hasField() && !_bFromReload ),
        "OBoundControlModel::impl_connectDatabaseColumn_noNotify: the form is just *loaded*, but we already have a field!" );
    (void)_bFromReload;

    Reference< XRowSet > xRowSet( m_xAmbientForm, UNO_QUERY );
    OSL_ENSURE( xRowSet.is(), "OBoundControlModel::impl_connectDatabaseColumn_noNotify: no row set!" );
    if ( !xRowSet.is() )
        return;

    if ( !hasField() )
    {
        // connect to the column
        connectToField( xRowSet );
    }

    // now that we're connected (more or less, even if we did not find a column),
    // we definately want to forward any potentially occuring value changes
    m_bForwardValueChanges = sal_True;

    // let derived classes react on this new connection
    m_bLoaded = sal_True;
    onConnectedDbColumn( xRowSet );

    // initially transfer the db column value to the control, if we successfully connected to a database column
    if ( hasField() )
        initFromField( xRowSet );
}

//------------------------------------------------------------------------------
void OBoundControlModel::impl_disconnectDatabaseColumn_noNotify()
{
    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::impl_disconnectDatabaseColumn_noNotify: not to be called with an external value binding!" );

    // let derived classes react on this
    onDisconnectedDbColumn();

    if ( hasField() )
    {
        getField()->removePropertyChangeListener( PROPERTY_VALUE, this );
        resetField();
    }

    m_xCursor = NULL;
    m_bLoaded = sal_False;
}

//==============================================================================
// XLoadListener
//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::loaded( const EventObject& _rEvent ) throw(RuntimeException)
{
    ControlModelLock aLock( *this );
    FieldChangeNotifier aBoundFieldNotifier( aLock );

    OSL_ENSURE( _rEvent.Source == m_xAmbientForm, "OBoundControlModel::loaded: where does this come from?" );
    (void)_rEvent;

    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::loaded: we should never reach this with an external value binding!" );
    if ( hasExternalValueBinding() )
        return;

    impl_connectDatabaseColumn_noNotify( false );
}


//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::unloaded( const com::sun::star::lang::EventObject& /*aEvent*/ ) throw(RuntimeException)
{
    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::unloaded: we should never reach this with an external value binding!" );
}

//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::reloading( const com::sun::star::lang::EventObject& /*aEvent*/ ) throw(RuntimeException)
{
    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::reloading: we should never reach this with an external value binding!" );
    if ( hasExternalValueBinding() )
        return;

    osl::MutexGuard aGuard(m_aMutex);
    m_bForwardValueChanges = sal_False;
}

//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::unloading(const com::sun::star::lang::EventObject& /*aEvent*/) throw(RuntimeException)
{
    ControlModelLock aLock( *this );
    FieldChangeNotifier aBoundFieldNotifier( aLock );

    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::unloading: we should never reach this with an external value binding!" );
    if ( hasExternalValueBinding() )
        return;

    impl_disconnectDatabaseColumn_noNotify();
}

//------------------------------------------------------------------------------
void SAL_CALL OBoundControlModel::reloaded( const EventObject& _rEvent ) throw(RuntimeException)
{
    ControlModelLock aLock( *this );
    FieldChangeNotifier aBoundFieldNotifier( aLock );

    OSL_ENSURE( _rEvent.Source == m_xAmbientForm, "OBoundControlModel::reloaded: where does this come from?" );
    (void)_rEvent;

    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::reloaded: we should never reach this with an external value binding!" );
    if ( hasExternalValueBinding() )
        return;

    impl_connectDatabaseColumn_noNotify( true );
}

//------------------------------------------------------------------------------
void OBoundControlModel::setControlValue( const Any& _rValue, ValueChangeInstigator _eInstigator )
{
    m_eControlValueChangeInstigator = _eInstigator;
    doSetControlValue( _rValue );
    m_eControlValueChangeInstigator = eOther;
}

//------------------------------------------------------------------------------
void OBoundControlModel::doSetControlValue( const Any& _rValue )
{
    OSL_PRECOND( m_xAggregateFastSet.is() && m_xAggregateSet.is(),
        "OBoundControlModel::doSetControlValue: invalid aggregate !" );
    OSL_PRECOND( !m_sValuePropertyName.isEmpty() || ( m_nValuePropertyAggregateHandle != -1 ),
        "OBoundControlModel::doSetControlValue: please override if you have own value property handling!" );

    try
    {
        // release our mutex once (it's acquired in one of the the calling methods), as setting aggregate properties
        // may cause any uno controls belonging to us to lock the solar mutex, which is potentially dangerous with
        // our own mutex locked
        MutexRelease aRelease( m_aMutex );
        if ( ( m_nValuePropertyAggregateHandle != -1 ) && m_xAggregateFastSet.is() )
        {
            m_xAggregateFastSet->setFastPropertyValue( m_nValuePropertyAggregateHandle, _rValue );
        }
        else if ( !m_sValuePropertyName.isEmpty() && m_xAggregateSet.is() )
        {
            m_xAggregateSet->setPropertyValue( m_sValuePropertyName, _rValue );
        }
    }
    catch( const Exception& )
    {
        OSL_FAIL( "OBoundControlModel::doSetControlValue: caught an exception!" );
    }
}

//------------------------------------------------------------------------------
void OBoundControlModel::onConnectedValidator( )
{
    try
    {
        // if we have an external validator, we do not want the control to force invalid
        // inputs to the default value. Instead, invalid inputs should be translated
        // to NaN (not a number)
        Reference< XPropertySetInfo > xAggregatePropertyInfo;
        if ( m_xAggregateSet.is() )
            xAggregatePropertyInfo = m_xAggregateSet->getPropertySetInfo();
        if ( xAggregatePropertyInfo.is() && xAggregatePropertyInfo->hasPropertyByName( PROPERTY_ENFORCE_FORMAT ) )
            m_xAggregateSet->setPropertyValue( PROPERTY_ENFORCE_FORMAT, makeAny( (sal_Bool)sal_False ) );
    }
    catch( const Exception& )
    {
        OSL_FAIL( "OBoundControlModel::onConnectedValidator: caught an exception!" );
    }
    recheckValidity( false );
}

//------------------------------------------------------------------------------
void OBoundControlModel::onDisconnectedValidator( )
{
    try
    {
        Reference< XPropertySetInfo > xAggregatePropertyInfo;
        if ( m_xAggregateSet.is() )
            xAggregatePropertyInfo = m_xAggregateSet->getPropertySetInfo();
        if ( xAggregatePropertyInfo.is() && xAggregatePropertyInfo->hasPropertyByName( PROPERTY_ENFORCE_FORMAT ) )
            m_xAggregateSet->setPropertyValue( PROPERTY_ENFORCE_FORMAT, makeAny( (sal_Bool)sal_True ) );
    }
    catch( const Exception& )
    {
        OSL_FAIL( "OBoundControlModel::onDisconnectedValidator: caught an exception!" );
    }
    recheckValidity( false );
}

//------------------------------------------------------------------------------
void OBoundControlModel::onConnectedExternalValue( )
{
    calculateExternalValueType();
}

//------------------------------------------------------------------------------
void OBoundControlModel::onDisconnectedExternalValue( )
{
}

//------------------------------------------------------------------------------
void OBoundControlModel::onConnectedDbColumn( const Reference< XInterface >& /*_rxForm*/ )
{
    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::onConnectedDbColumn: how this? There's an external value binding!" );
}

//------------------------------------------------------------------------------
void OBoundControlModel::onDisconnectedDbColumn()
{
    OSL_PRECOND( !hasExternalValueBinding(), "OBoundControlModel::onDisconnectedDbColumn: how this? There's an external value binding!" );
}

// XReset
//-----------------------------------------------------------------------------
Any OBoundControlModel::getDefaultForReset() const
{
    return Any();
}

//-----------------------------------------------------------------------------
void OBoundControlModel::resetNoBroadcast()
{
    setControlValue( getDefaultForReset(), eOther );
}

//-----------------------------------------------------------------------------
void OBoundControlModel::addResetListener(const Reference<XResetListener>& l) throw (RuntimeException)
{
    m_aResetHelper.addResetListener( l );
}

//-----------------------------------------------------------------------------
void OBoundControlModel::removeResetListener(const Reference<XResetListener>& l) throw (RuntimeException)
{
    m_aResetHelper.removeResetListener( l );
}

//-----------------------------------------------------------------------------
void OBoundControlModel::reset() throw (RuntimeException)
{
    if ( !m_aResetHelper.approveReset() )
       return;

    ControlModelLock aLock( *this );

    // on a new record?
    sal_Bool bIsNewRecord = sal_False;
    Reference<XPropertySet> xSet( m_xCursor, UNO_QUERY );
    if ( xSet.is() )
    {
        try
        {
            xSet->getPropertyValue( PROPERTY_ISNEW ) >>= bIsNewRecord;
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    // cursor on an invalid row?
    sal_Bool bInvalidCursorPosition = sal_True;
    try
    {
        bInvalidCursorPosition =    m_xCursor.is()
                                &&  (  m_xCursor->isAfterLast()
                                    || m_xCursor->isBeforeFirst()
                                    )
                                &&  !bIsNewRecord;
    }
    catch( const SQLException& )
    {
        OSL_FAIL( "OBoundControlModel::reset: caught an SQL exception!" );
    }
    // #i24495# - don't count the insert row as "invalid"

    sal_Bool bSimpleReset =
                        (   !m_xColumn.is()                     // no connection to a database column
                        ||  (   m_xCursor.is()                  // OR   we have an improperly positioned cursor
                            &&  bInvalidCursorPosition
                            )
                        ||  hasExternalValueBinding()           // OR we have an external value binding
                        );

    if ( !bSimpleReset )
    {
        // The default values will be set if and only if the current value of the field which we're bound
        // to is NULL.
        // Else, the current field value should be refreshed
        // This behaviour is not completely ... "matured": What should happen if the field as well as the
        // control have a default value?

        sal_Bool bIsNull = sal_True;
        // we have to access the field content at least once to get a reliable result by XColumn::wasNull
        try
        {
            // normally, we'd do a getString here. However, this is extremely expensive in the case
            // of binary fields. Unfortunately, getString is the only method which is guaranteed
            // to *always* succeed, all other getXXX methods may fail if the column is asked for a
            // non-convertible type
            sal_Int32 nFieldType = DataType::OBJECT;
            getField()->getPropertyValue( PROPERTY_FIELDTYPE ) >>= nFieldType;
            if  (  ( nFieldType == DataType::BINARY        )
                || ( nFieldType == DataType::VARBINARY     )
                || ( nFieldType == DataType::LONGVARBINARY )
                || ( nFieldType == DataType::OBJECT        )
                /*|| ( nFieldType == DataType::CLOB          )*/
                )
                m_xColumn->getBinaryStream();
            else if ( nFieldType == DataType::BLOB          )
                m_xColumn->getBlob();
            else
                m_xColumn->getString();

            bIsNull = m_xColumn->wasNull();
        }
        catch(const Exception&)
        {
            OSL_FAIL("OBoundControlModel::reset: this should have succeeded in all cases!");
        }

        sal_Bool bNeedValueTransfer = sal_True;

        if ( bIsNull )
        {
            if ( bIsNewRecord )
            {
                // reset the control to it's default
                resetNoBroadcast();
                // and immediately commit the changes to the DB column, to keep consistency
                commitControlValueToDbColumn( sal_True );

                bNeedValueTransfer = sal_False;
            }
        }

        if ( bNeedValueTransfer )
            transferDbValueToControl();
    }
    else
    {
        resetNoBroadcast();

        // transfer to the external binding, if necessary
        if ( hasExternalValueBinding() )
            transferControlValueToExternal( aLock );
    }

    // revalidate, if necessary
    if ( hasValidator() )
        recheckValidity( true );

    aLock.release();

    m_aResetHelper.notifyResetted();
}

// -----------------------------------------------------------------------------
void OBoundControlModel::impl_setField_noNotify( const Reference< XPropertySet>& _rxField )
{
    DBG_ASSERT( !hasExternalValueBinding(), "OBoundControlModel::impl_setField_noNotify: We have an external value binding!" );
    m_xField = _rxField;
}

//--------------------------------------------------------------------
sal_Bool OBoundControlModel::impl_approveValueBinding_nolock( const Reference< XValueBinding >& _rxBinding )
{
    if ( !_rxBinding.is() )
        return sal_False;

    Sequence< Type > aTypeCandidates;
    {
        // SYNCHRONIZED -->
        ::osl::MutexGuard aGuard( m_aMutex );
        aTypeCandidates = getSupportedBindingTypes();
        // <-- SYNCHRONIZED
    }

    for (   const Type* pType = aTypeCandidates.getConstArray();
            pType != aTypeCandidates.getConstArray() + aTypeCandidates.getLength();
            ++pType
        )
    {
        if ( _rxBinding->supportsType( *pType ) )
            return sal_True;
    }

    return sal_False;
}

//--------------------------------------------------------------------
void OBoundControlModel::connectExternalValueBinding(
        const Reference< XValueBinding >& _rxBinding, ControlModelLock& _rInstanceLock )
{
    OSL_PRECOND( _rxBinding.is(), "OBoundControlModel::connectExternalValueBinding: invalid binding instance!" );
    OSL_PRECOND( !hasExternalValueBinding( ), "OBoundControlModel::connectExternalValueBinding: precond not met (currently have a binding)!" );

    // if we're connected to a database column, suspend this
    if ( hasField() )
        impl_disconnectDatabaseColumn_noNotify();

    // suspend listening for load-related events at out ambient form.
    // This is because an external value binding overrules a possible database binding.
    if ( isFormListening() )
        doFormListening( false );

    // remember this new binding
    m_xExternalBinding = _rxBinding;

    // tell the derivee
    onConnectedExternalValue();

    try
    {
        // add as value listener so we get notified when the value changes
        Reference< XModifyBroadcaster > xModifiable( m_xExternalBinding, UNO_QUERY );
        if ( xModifiable.is() )
            xModifiable->addModifyListener( this );

        // add as property change listener for some (possibly present) properties we're
        // interested in
        Reference< XPropertySet > xBindingProps( m_xExternalBinding, UNO_QUERY );
        Reference< XPropertySetInfo > xBindingPropsInfo( xBindingProps.is() ? xBindingProps->getPropertySetInfo() : Reference< XPropertySetInfo >() );
        if ( xBindingPropsInfo.is() )
        {
            if ( xBindingPropsInfo->hasPropertyByName( PROPERTY_READONLY ) )
            {
                xBindingProps->addPropertyChangeListener( PROPERTY_READONLY, this );
                m_bBindingControlsRO = sal_True;
            }
            if ( xBindingPropsInfo->hasPropertyByName( PROPERTY_RELEVANT ) )
            {
                xBindingProps->addPropertyChangeListener( PROPERTY_RELEVANT, this );
                m_bBindingControlsEnable = sal_True;
            }
        }
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }

    // propagate our new value
    transferExternalValueToControl( _rInstanceLock );

    // if the binding is also a validator, use it, too. This is a constraint of the
    // com.sun.star.form.binding.ValidatableBindableFormComponent service
    if ( m_bSupportsValidation )
    {
        try
        {
            Reference< XValidator > xAsValidator( _rxBinding, UNO_QUERY );
            if ( xAsValidator.is() )
                setValidator( xAsValidator );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }
}

//--------------------------------------------------------------------
void OBoundControlModel::disconnectExternalValueBinding( )
{
    try
    {
        // not listening at the binding anymore
        Reference< XModifyBroadcaster > xModifiable( m_xExternalBinding, UNO_QUERY );
        if ( xModifiable.is() )
            xModifiable->removeModifyListener( this );

        // remove as property change listener
        Reference< XPropertySet > xBindingProps( m_xExternalBinding, UNO_QUERY );
        if ( m_bBindingControlsRO )
            xBindingProps->removePropertyChangeListener( PROPERTY_READONLY, this );
        if ( m_bBindingControlsEnable )
            xBindingProps->removePropertyChangeListener( PROPERTY_RELEVANT, this );
    }
    catch( const Exception& )
    {
        OSL_FAIL( "OBoundControlModel::disconnectExternalValueBinding: caught an exception!" );
    }

    // if the binding also acts as our validator, disconnect the validator, too
    if ( ( m_xExternalBinding == m_xValidator ) && m_xValidator.is() )
        disconnectValidator( );

    // no binding anymore
    m_xExternalBinding.clear();

    // be a load listener at our form, again. This was suspended while we had
    // an external value binding in place.
    doFormListening( true );

    // re-connect to database column of the new parent
    if ( m_xAmbientForm.is() && m_xAmbientForm->isLoaded() )
        impl_connectDatabaseColumn_noNotify( false );

    // tell the derivee
    onDisconnectedExternalValue();
}

//--------------------------------------------------------------------
void SAL_CALL OBoundControlModel::setValueBinding( const Reference< XValueBinding >& _rxBinding ) throw (IncompatibleTypesException, RuntimeException)
{
    OSL_PRECOND( m_bSupportsExternalBinding, "OBoundControlModel::setValueBinding: How did you reach this method?" );
        // the interface for this method should not have been exposed if we do not
        // support binding to external data
    // allow reset
    if ( _rxBinding.is() && !impl_approveValueBinding_nolock( _rxBinding ) )
    {
        throw IncompatibleTypesException(
            FRM_RES_STRING( RID_STR_INCOMPATIBLE_TYPES ),
            *this
        );
    }

    ControlModelLock aLock( *this );

    // since a ValueBinding overrules any potentially active database binding, the change in a ValueBinding
    // might trigger a change in our BoundField.
    FieldChangeNotifier aBoundFieldNotifier( aLock );

    // disconnect from the old binding
    if ( hasExternalValueBinding() )
        disconnectExternalValueBinding( );

    // connect to the new binding
    if ( _rxBinding.is() )
        connectExternalValueBinding( _rxBinding, aLock );
}

//--------------------------------------------------------------------
Reference< XValueBinding > SAL_CALL OBoundControlModel::getValueBinding(  ) throw (RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    OSL_PRECOND( m_bSupportsExternalBinding, "OBoundControlModel::getValueBinding: How did you reach this method?" );
        // the interface for this method should not have been exposed if we do not
        // support binding to external data

    return m_xExternalBinding;
}

//--------------------------------------------------------------------
void SAL_CALL OBoundControlModel::modified( const EventObject& _rEvent ) throw ( RuntimeException )
{
    ControlModelLock aLock( *this );

    OSL_PRECOND( hasExternalValueBinding(), "OBoundControlModel::modified: Where did this come from?" );
    if ( !m_bTransferingValue && ( m_xExternalBinding == _rEvent.Source ) && m_xExternalBinding.is() )
    {
        transferExternalValueToControl( aLock );
    }
}

//--------------------------------------------------------------------
void OBoundControlModel::transferDbValueToControl( )
{
    try
    {
        setControlValue( translateDbColumnToControlValue(), eDbColumnBinding );
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
}

//------------------------------------------------------------------------------
void OBoundControlModel::transferExternalValueToControl( ControlModelLock& _rInstanceLock )
{
        Reference< XValueBinding > xExternalBinding( m_xExternalBinding );
        Type aValueExchangeType( getExternalValueType() );

        _rInstanceLock.release();
        // >>>>>>>> ----- UNSAFE ----- >>>>>>>>
        Any aExternalValue;
        try
        {
            aExternalValue = xExternalBinding->getValue( aValueExchangeType );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        // <<<<<<<< ----- UNSAFE ----- <<<<<<<<
        _rInstanceLock.acquire();

        setControlValue( translateExternalValueToControlValue( aExternalValue ), eExternalBinding );
}

//------------------------------------------------------------------------------
void OBoundControlModel::transferControlValueToExternal( ControlModelLock& _rInstanceLock )
{
    OSL_PRECOND( m_bSupportsExternalBinding && hasExternalValueBinding(),
        "OBoundControlModel::transferControlValueToExternal: precondition not met!" );

    if ( m_xExternalBinding.is() )
    {
        Any aExternalValue( translateControlValueToExternalValue() );
        m_bTransferingValue = sal_True;

        _rInstanceLock.release();
         // >>>>>>>> ----- UNSAFE ----- >>>>>>>>
        try
        {
            m_xExternalBinding->setValue( aExternalValue );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        // <<<<<<<< ----- UNSAFE ----- <<<<<<<<
        _rInstanceLock.acquire();

        m_bTransferingValue = sal_False;
    }
}

// -----------------------------------------------------------------------------
Sequence< Type > OBoundControlModel::getSupportedBindingTypes()
{
    return Sequence< Type >( &m_aValuePropertyType, 1 );
}

//-----------------------------------------------------------------------------
void OBoundControlModel::calculateExternalValueType()
{
    m_aExternalValueType = Type();
    if ( !m_xExternalBinding.is() )
        return;

    Sequence< Type > aTypeCandidates( getSupportedBindingTypes() );
    for (   const Type* pTypeCandidate = aTypeCandidates.getConstArray();
            pTypeCandidate != aTypeCandidates.getConstArray() + aTypeCandidates.getLength();
            ++pTypeCandidate
        )
    {
        if ( m_xExternalBinding->supportsType( *pTypeCandidate ) )
        {
            m_aExternalValueType = *pTypeCandidate;
            break;
        }
    }
}

//-----------------------------------------------------------------------------
Any OBoundControlModel::translateExternalValueToControlValue( const Any& _rExternalValue ) const
{
    OSL_PRECOND( m_bSupportsExternalBinding && hasExternalValueBinding(),
        "OBoundControlModel::translateExternalValueToControlValue: precondition not met!" );

    Any aControlValue( _rExternalValue );

    // if the external value is VOID, and our value property is not allowed to be VOID,
    // then default-construct a value
    if ( !aControlValue.hasValue() && !m_bValuePropertyMayBeVoid )
        aControlValue.setValue( NULL, m_aValuePropertyType );

    // outta here
    return aControlValue;
}

//------------------------------------------------------------------------------
Any OBoundControlModel::translateControlValueToExternalValue( ) const
{
    return getControlValue( );
}

//------------------------------------------------------------------------------
Any OBoundControlModel::translateControlValueToValidatableValue( ) const
{
    OSL_PRECOND( m_xValidator.is(), "OBoundControlModel::translateControlValueToValidatableValue: no validator, so why should I?" );
    if ( ( m_xValidator == m_xExternalBinding ) && m_xValidator.is() )
        return translateControlValueToExternalValue();
    return getControlValue();
}

//------------------------------------------------------------------------------
Any OBoundControlModel::getControlValue( ) const
{
    OSL_PRECOND( m_xAggregateFastSet.is() && m_xAggregateSet.is(),
        "OBoundControlModel::getControlValue: invalid aggregate !" );
    OSL_PRECOND( !m_sValuePropertyName.isEmpty() || ( m_nValuePropertyAggregateHandle != -1 ),
        "OBoundControlModel::getControlValue: please override if you have own value property handling!" );

    // determine the current control value
    Any aControlValue;
    if ( ( m_nValuePropertyAggregateHandle != -1 ) && m_xAggregateFastSet.is() )
    {
        aControlValue = m_xAggregateFastSet->getFastPropertyValue( m_nValuePropertyAggregateHandle );
    }
    else if ( !m_sValuePropertyName.isEmpty() && m_xAggregateSet.is() )
    {
        aControlValue = m_xAggregateSet->getPropertyValue( m_sValuePropertyName );
    }

    return aControlValue;
}

//--------------------------------------------------------------------
void OBoundControlModel::connectValidator( const Reference< XValidator >& _rxValidator )
{
    OSL_PRECOND( _rxValidator.is(), "OBoundControlModel::connectValidator: invalid validator instance!" );
    OSL_PRECOND( !hasValidator( ), "OBoundControlModel::connectValidator: precond not met (have a validator currently)!" );

    m_xValidator = _rxValidator;

    // add as value listener so we get notified when the value changes
    if ( m_xValidator.is() )
    {
        try
        {
            m_xValidator->addValidityConstraintListener( this );
        }
        catch( const RuntimeException& )
        {
        }
    }

    onConnectedValidator( );
}

//--------------------------------------------------------------------
void OBoundControlModel::disconnectValidator( )
{
    OSL_PRECOND( hasValidator( ), "OBoundControlModel::connectValidator: precond not met (don't have a validator currently)!" );

    // add as value listener so we get notified when the value changes
    if ( m_xValidator.is() )
    {
        try
        {
            m_xValidator->removeValidityConstraintListener( this );
        }
        catch( const RuntimeException& )
        {
        }
    }

    m_xValidator.clear();

    onDisconnectedValidator( );
}

//--------------------------------------------------------------------
void SAL_CALL OBoundControlModel::setValidator( const Reference< XValidator >& _rxValidator ) throw (VetoException,RuntimeException)
{
    ::osl::ClearableMutexGuard aGuard( m_aMutex );
    OSL_PRECOND( m_bSupportsValidation, "OBoundControlModel::setValidator: How did you reach this method?" );
        // the interface for this method should not have been exposed if we do not
        // support validation

    // early out if the validator does not change
    if( _rxValidator == m_xValidator )
        return;

    if ( m_xValidator.is() && ( m_xValidator == m_xExternalBinding ) )
        throw VetoException(
            FRM_RES_STRING( RID_STR_INVALID_VALIDATOR ),
            *this
        );

    // disconnect from the old validator
    if ( hasValidator() )
        disconnectValidator( );

    // connect to the new validator
    if ( _rxValidator.is() )
        connectValidator( _rxValidator );
}

//--------------------------------------------------------------------
Reference< XValidator > SAL_CALL OBoundControlModel::getValidator(  ) throw (RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    OSL_PRECOND( m_bSupportsValidation, "OBoundControlModel::getValidator: How did you reach this method?" );
        // the interface for this method should not have been exposed if we do not
        // support validation

    return m_xValidator;
}

//--------------------------------------------------------------------
void SAL_CALL OBoundControlModel::validityConstraintChanged( const EventObject& /*Source*/ ) throw (RuntimeException)
{
    ::osl::ClearableMutexGuard aGuard( m_aMutex );
    OSL_PRECOND( m_bSupportsValidation, "OBoundControlModel::validityConstraintChanged: How did you reach this method?" );
        // the interface for this method should not have been exposed if we do not
        // support validation

    recheckValidity( false );
}

//--------------------------------------------------------------------
sal_Bool SAL_CALL OBoundControlModel::isValid(  ) throw (RuntimeException)
{
    return m_bIsCurrentValueValid;
}

//--------------------------------------------------------------------
::com::sun::star::uno::Any OBoundControlModel::getCurrentFormComponentValue() const
{
    if ( hasValidator() )
        return translateControlValueToValidatableValue();
    return getControlValue();
}

//--------------------------------------------------------------------
Any SAL_CALL OBoundControlModel::getCurrentValue(  ) throw (RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    return getCurrentFormComponentValue();
}

//--------------------------------------------------------------------
void SAL_CALL OBoundControlModel::addFormComponentValidityListener( const Reference< validation::XFormComponentValidityListener >& Listener ) throw (NullPointerException, RuntimeException)
{
    if ( Listener.is() )
        m_aFormComponentListeners.addInterface( Listener );
}

//--------------------------------------------------------------------
void SAL_CALL OBoundControlModel::removeFormComponentValidityListener( const Reference< validation::XFormComponentValidityListener >& Listener ) throw (NullPointerException, RuntimeException)
{
    if ( Listener.is() )
        m_aFormComponentListeners.removeInterface( Listener );
}

//--------------------------------------------------------------------
void OBoundControlModel::recheckValidity( bool _bForceNotification )
{
    try
    {
        sal_Bool bIsCurrentlyValid = sal_True;
        if ( hasValidator() )
            bIsCurrentlyValid = m_xValidator->isValid( translateControlValueToValidatableValue() );

        if ( ( bIsCurrentlyValid != m_bIsCurrentValueValid ) || _bForceNotification )
        {
            m_bIsCurrentValueValid = bIsCurrentlyValid;

            // release our mutex for the notifications
            MutexRelease aRelease( m_aMutex );
            m_aFormComponentListeners.notifyEach( &validation::XFormComponentValidityListener::componentValidityChanged, EventObject( *this ) );
        }
    }
    catch( const Exception& )
    {
        OSL_FAIL( "OBoundControlModel::recheckValidity: caught an exception!" );
    }
}

//------------------------------------------------------------------------------
void OBoundControlModel::describeFixedProperties( Sequence< Property >& _rProps ) const
{
    BEGIN_DESCRIBE_PROPERTIES( 5, OControlModel )
        DECL_PROP1      ( CONTROLSOURCE,           ::rtl::OUString,     BOUND );
        DECL_IFACE_PROP3( BOUNDFIELD,               XPropertySet,       BOUND, READONLY, TRANSIENT );
        DECL_IFACE_PROP2( CONTROLLABEL,             XPropertySet,       BOUND, MAYBEVOID );
        DECL_PROP2      ( CONTROLSOURCEPROPERTY,    ::rtl::OUString,    READONLY, TRANSIENT );
        DECL_BOOL_PROP1 ( INPUT_REQUIRED,                               BOUND );
    END_DESCRIBE_PROPERTIES()
}

// -----------------------------------------------------------------------------

//.........................................................................
}
//... namespace frm .......................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
