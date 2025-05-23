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

#include "File.hxx"

#include <com/sun/star/form/FormComponentType.hpp>

#include "property.hrc"
#include "services.hxx"
#include <tools/debug.hxx>
#include <comphelper/container.hxx>
#include <comphelper/basicio.hxx>
#include <comphelper/guarding.hxx>

//.........................................................................
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

//------------------------------------------------------------------
InterfaceRef SAL_CALL OFileControlModel_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory)
{
    return *(new OFileControlModel(_rxFactory));
}

//------------------------------------------------------------------------------
Sequence<Type> OFileControlModel::_getTypes()
{
    static Sequence<Type> aTypes;
    if (!aTypes.getLength())
    {
        // my base class
        Sequence<Type> aBaseClassTypes = OControlModel::_getTypes();

        Sequence<Type> aOwnTypes(1);
        Type* pOwnTypes = aOwnTypes.getArray();
        pOwnTypes[0] = getCppuType((Reference<XReset>*)NULL);

        aTypes = concatSequences(aBaseClassTypes, aOwnTypes);
    }
    return aTypes;
}


// XServiceInfo
//------------------------------------------------------------------------------
StringSequence  OFileControlModel::getSupportedServiceNames() throw(RuntimeException)
{
    StringSequence aSupported = OControlModel::getSupportedServiceNames();
    aSupported.realloc(aSupported.getLength() + 1);

    ::rtl::OUString*pArray = aSupported.getArray();
    pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_FILECONTROL;
    return aSupported;
}

//------------------------------------------------------------------
DBG_NAME( OFileControlModel )
//------------------------------------------------------------------
OFileControlModel::OFileControlModel(const Reference<XMultiServiceFactory>& _rxFactory)
                    :OControlModel(_rxFactory, VCL_CONTROLMODEL_FILECONTROL)
                    ,m_aResetListeners(m_aMutex)
{
    DBG_CTOR( OFileControlModel, NULL );
    m_nClassId = FormComponentType::FILECONTROL;
}

//------------------------------------------------------------------
OFileControlModel::OFileControlModel( const OFileControlModel* _pOriginal, const Reference<XMultiServiceFactory>& _rxFactory )
    :OControlModel( _pOriginal, _rxFactory )
    ,m_aResetListeners( m_aMutex )
{
    DBG_CTOR( OFileControlModel, NULL );

    m_sDefaultValue = _pOriginal->m_sDefaultValue;
}

//------------------------------------------------------------------
OFileControlModel::~OFileControlModel()
{
    if (!OComponentHelper::rBHelper.bDisposed)
    {
        acquire();
        dispose();
    }
    DBG_DTOR( OFileControlModel, NULL );
}

//------------------------------------------------------------------------------
IMPLEMENT_DEFAULT_CLONING( OFileControlModel )

//------------------------------------------------------------------------------
Any SAL_CALL OFileControlModel::queryAggregation(const Type& _rType) throw (RuntimeException)
{
    Any aReturn = OControlModel::queryAggregation(_rType);
    if (!aReturn.hasValue())
        aReturn = ::cppu::queryInterface(_rType
            ,static_cast<XReset*>(this)
        );

    return aReturn;
}

// OComponentHelper
//-----------------------------------------------------------------------------
void OFileControlModel::disposing()
{
    OControlModel::disposing();

    EventObject aEvt(static_cast<XWeak*>(this));
    m_aResetListeners.disposeAndClear(aEvt);
}

//------------------------------------------------------------------------------
Any OFileControlModel::getPropertyDefaultByHandle( sal_Int32 _nHandle ) const
{
    Any aReturn;
    switch ( _nHandle )
    {
        case PROPERTY_ID_DEFAULT_TEXT:
            return makeAny( ::rtl::OUString() );
    }
    return OControlModel::getPropertyDefaultByHandle( _nHandle );
}

//------------------------------------------------------------------------------
void OFileControlModel::getFastPropertyValue(Any& rValue, sal_Int32 nHandle) const
{
    switch (nHandle)
    {
        case PROPERTY_ID_DEFAULT_TEXT : rValue <<= m_sDefaultValue; break;
        default:
            OControlModel::getFastPropertyValue(rValue, nHandle);
    }
}

//------------------------------------------------------------------------------
void OFileControlModel::setFastPropertyValue_NoBroadcast(sal_Int32 nHandle, const Any& rValue) throw ( ::com::sun::star::uno::Exception)
{
    switch (nHandle)
    {
        case PROPERTY_ID_DEFAULT_TEXT :
            DBG_ASSERT(rValue.getValueType().getTypeClass() == TypeClass_STRING, "OFileControlModel::setFastPropertyValue_NoBroadcast : invalid type !" );
            rValue >>= m_sDefaultValue;
            break;
        default:
            OControlModel::setFastPropertyValue_NoBroadcast(nHandle, rValue);
    }
}

//------------------------------------------------------------------------------
sal_Bool OFileControlModel::convertFastPropertyValue(Any& rConvertedValue, Any& rOldValue, sal_Int32 nHandle, const Any& rValue)
                            throw( IllegalArgumentException )
{
    switch (nHandle)
    {
        case PROPERTY_ID_DEFAULT_TEXT :
            return tryPropertyValue(rConvertedValue, rOldValue, rValue, m_sDefaultValue);
        default:
            return OControlModel::convertFastPropertyValue(rConvertedValue, rOldValue, nHandle, rValue);
    }
}

//------------------------------------------------------------------------------
void OFileControlModel::describeFixedProperties( Sequence< Property >& _rProps ) const
{
    BEGIN_DESCRIBE_PROPERTIES( 2, OControlModel )
        DECL_PROP1(DEFAULT_TEXT,    ::rtl::OUString,    BOUND);
        DECL_PROP1(TABINDEX,        sal_Int16,          BOUND);
    END_DESCRIBE_PROPERTIES();
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OFileControlModel::getServiceName() throw ( ::com::sun::star::uno::RuntimeException)
{
    return FRM_COMPONENT_FILECONTROL;   // old (non-sun) name for compatibility !
}

//------------------------------------------------------------------------------
void OFileControlModel::write(const Reference<stario::XObjectOutputStream>& _rxOutStream) throw ( ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    OControlModel::write(_rxOutStream);

    ::osl::MutexGuard aGuard(m_aMutex);

    // Version
    _rxOutStream->writeShort(0x0002);
    // Default-Wert
    _rxOutStream << m_sDefaultValue;
    writeHelpTextCompatibly(_rxOutStream);
}

//------------------------------------------------------------------------------
void OFileControlModel::read(const Reference<stario::XObjectInputStream>& _rxInStream) throw ( ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    OControlModel::read(_rxInStream);
    ::osl::MutexGuard aGuard(m_aMutex);

    // Version
    sal_uInt16 nVersion = _rxInStream->readShort();
    // Default-Wert
    switch (nVersion)
    {
        case 1:
            _rxInStream >> m_sDefaultValue; break;
        case 2:
            _rxInStream >> m_sDefaultValue;
            readHelpTextCompatibly(_rxInStream);
            break;
        default:
            OSL_FAIL("OFileControlModel::read : unknown version !");
            m_sDefaultValue = ::rtl::OUString();
    }

    // Display default values after read
//  _reset();
}

//-----------------------------------------------------------------------------
void SAL_CALL OFileControlModel::reset() throw ( ::com::sun::star::uno::RuntimeException)
{
    ::cppu::OInterfaceIteratorHelper aIter(m_aResetListeners);
    EventObject aEvt(static_cast<XWeak*>(this));
    sal_Bool bContinue = sal_True;
    while (aIter.hasMoreElements() && bContinue)
        bContinue =((XResetListener*)aIter.next())->approveReset(aEvt);

    if (bContinue)
    {
        {
            // If Models are threadSave
            ::osl::MutexGuard aGuard(m_aMutex);
            _reset();
        }
        m_aResetListeners.notifyEach( &XResetListener::resetted, aEvt );
    }
}

//-----------------------------------------------------------------------------
void OFileControlModel::addResetListener(const Reference<XResetListener>& _rxListener) throw ( ::com::sun::star::uno::RuntimeException)
{
    m_aResetListeners.addInterface(_rxListener);
}

//-----------------------------------------------------------------------------
void OFileControlModel::removeResetListener(const Reference<XResetListener>& _rxListener) throw ( ::com::sun::star::uno::RuntimeException)
{
    m_aResetListeners.removeInterface(_rxListener);
}

//------------------------------------------------------------------------------
void OFileControlModel::_reset()
{
    {   // release our mutex once (it's acquired in the calling method !), as setting aggregate properties
        // may cause any uno controls belonging to us to lock the solar mutex, which is potentially dangerous with
        // our own mutex locked
        MutexRelease aRelease(m_aMutex);
        m_xAggregateSet->setPropertyValue(PROPERTY_TEXT, makeAny(m_sDefaultValue));
    }
}

//.........................................................................
}   // namespace frm
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
