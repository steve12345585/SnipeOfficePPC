/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#include "GroupBox.hxx"
#include "property.hxx"
#include "property.hrc"
#include "services.hxx"
#include <tools/debug.hxx>

//.........................................................................
namespace frm
{
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

//==================================================================
// OGroupBoxModel
//==================================================================

//------------------------------------------------------------------
InterfaceRef SAL_CALL OGroupBoxModel_CreateInstance(const Reference<starlang::XMultiServiceFactory>& _rxFactory) throw (RuntimeException)
{
    return *(new OGroupBoxModel(_rxFactory));
}

//------------------------------------------------------------------
DBG_NAME( OGroupBoxModel )
//------------------------------------------------------------------
OGroupBoxModel::OGroupBoxModel(const Reference<starlang::XMultiServiceFactory>& _rxFactory)
    :OControlModel(_rxFactory, VCL_CONTROLMODEL_GROUPBOX, VCL_CONTROL_GROUPBOX)
{
    DBG_CTOR( OGroupBoxModel, NULL );
    m_nClassId = FormComponentType::GROUPBOX;
}

//------------------------------------------------------------------
OGroupBoxModel::OGroupBoxModel( const OGroupBoxModel* _pOriginal, const Reference<starlang::XMultiServiceFactory>& _rxFactory )
    :OControlModel( _pOriginal, _rxFactory )
{
    DBG_CTOR( OGroupBoxModel, NULL );
}

// XServiceInfo
//------------------------------------------------------------------------------
StringSequence SAL_CALL OGroupBoxModel::getSupportedServiceNames() throw(RuntimeException)
{
    StringSequence aSupported = OControlModel::getSupportedServiceNames();
    aSupported.realloc(aSupported.getLength() + 1);

    ::rtl::OUString* pArray = aSupported.getArray();
    pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_GROUPBOX;
    return aSupported;
}

//------------------------------------------------------------------
OGroupBoxModel::~OGroupBoxModel()
{
    DBG_DTOR( OGroupBoxModel, NULL );
}

//------------------------------------------------------------------------------
IMPLEMENT_DEFAULT_CLONING( OGroupBoxModel )

//------------------------------------------------------------------------------
void OGroupBoxModel::describeAggregateProperties( Sequence< Property >& _rAggregateProps ) const
{
    OControlModel::describeAggregateProperties( _rAggregateProps );
    // don't want to have the TabStop property
    RemoveProperty(_rAggregateProps, PROPERTY_TABSTOP);
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OGroupBoxModel::getServiceName() throw(RuntimeException)
{
    return FRM_COMPONENT_GROUPBOX;  // old (non-sun) name for compatibility !
}

//------------------------------------------------------------------------------
void SAL_CALL OGroupBoxModel::write(const Reference< XObjectOutputStream>& _rxOutStream)
    throw(IOException, RuntimeException)
{
    OControlModel::write(_rxOutStream);

    // Version
    _rxOutStream->writeShort(0x0002);
    writeHelpTextCompatibly(_rxOutStream);
}

//------------------------------------------------------------------------------
void SAL_CALL OGroupBoxModel::read(const Reference< XObjectInputStream>& _rxInStream) throw(IOException, RuntimeException)
{
    OControlModel::read( _rxInStream );

    // Version
    sal_uInt16 nVersion = _rxInStream->readShort();
    DBG_ASSERT(nVersion > 0, "OGroupBoxModel::read : version 0 ? this should never have been written !");
        // ups, ist das Englisch richtig ? ;)

    if (nVersion == 2)
        readHelpTextCompatibly(_rxInStream);

    if (nVersion > 0x0002)
    {
        OSL_FAIL("OGroupBoxModel::read : unknown version !");
    }
};

//==================================================================
// OGroupBoxControl
//==================================================================

//------------------------------------------------------------------
InterfaceRef SAL_CALL OGroupBoxControl_CreateInstance(const Reference<starlang::XMultiServiceFactory>& _rxFactory) throw (RuntimeException)
{
    return *(new OGroupBoxControl(_rxFactory));
}

//------------------------------------------------------------------------------
OGroupBoxControl::OGroupBoxControl(const Reference<starlang::XMultiServiceFactory>& _rxFactory)
                   :OControl(_rxFactory, VCL_CONTROL_GROUPBOX)
{
}

//------------------------------------------------------------------------------
StringSequence SAL_CALL OGroupBoxControl::getSupportedServiceNames() throw(RuntimeException)
{
    StringSequence aSupported = OControl::getSupportedServiceNames();
    aSupported.realloc(aSupported.getLength() + 1);

    ::rtl::OUString* pArray = aSupported.getArray();
    pArray[aSupported.getLength()-1] = FRM_SUN_CONTROL_GROUPBOX;
    return aSupported;
}

//.........................................................................
}
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
