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


#include "dbustrings.hrc"
#include <toolkit/awt/vclxwindow.hxx>
#include "dbu_reghelper.hxx"
#ifndef _DBAUI_UNOADMIN_
#include "unoadmin.hxx"
#endif
#include "dbadmin.hxx"
#include <comphelper/extract.hxx>
#include <cppuhelper/typeprovider.hxx>
#include <comphelper/property.hxx>
#include <osl/diagnose.h>
#include <vcl/msgbox.hxx>

// --- needed because of the solar mutex
#include <osl/mutex.hxx>
#include <vcl/svapp.hxx>
// ---

#define THISREF()   static_cast< XServiceInfo* >(this)

//.........................................................................
namespace dbaui
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;

//=========================================================================
DBG_NAME(ODatabaseAdministrationDialog)
//-------------------------------------------------------------------------
ODatabaseAdministrationDialog::ODatabaseAdministrationDialog(const Reference< XMultiServiceFactory >& _rxORB)
    :ODatabaseAdministrationDialogBase(_rxORB)
    ,m_pDatasourceItems(NULL)
    ,m_pItemPool(NULL)
    ,m_pItemPoolDefaults(NULL)
    ,m_pCollection(NULL)
{
    DBG_CTOR(ODatabaseAdministrationDialog,NULL);

    m_pCollection = new ::dbaccess::ODsnTypeCollection(_rxORB);
    ODbAdminDialog::createItemSet(m_pDatasourceItems, m_pItemPool, m_pItemPoolDefaults, m_pCollection);
}

//-------------------------------------------------------------------------
ODatabaseAdministrationDialog::~ODatabaseAdministrationDialog()
{
    // we do this here cause the base class' call to destroyDialog won't reach us anymore : we're within an dtor,
    // so this virtual-method-call the base class does does not work, we're already dead then ...
    if (m_pDialog)
    {
        ::osl::MutexGuard aGuard(m_aMutex);
        if (m_pDialog)
            destroyDialog();
    }

    delete m_pCollection;
    m_pCollection = NULL;

    DBG_DTOR(ODatabaseAdministrationDialog,NULL);
}
//-------------------------------------------------------------------------
void ODatabaseAdministrationDialog::destroyDialog()
{
    ODatabaseAdministrationDialogBase::destroyDialog();
    ODbAdminDialog::destroyItemSet(m_pDatasourceItems, m_pItemPool, m_pItemPoolDefaults);
}
//------------------------------------------------------------------------------
void ODatabaseAdministrationDialog::implInitialize(const Any& _rValue)
{
    PropertyValue aProperty;
    if (_rValue >>= aProperty)
    {
        if (0 == aProperty.Name.compareToAscii("InitialSelection"))
        {
            m_aInitialSelection = aProperty.Value;
        }
        else if (0 == aProperty.Name.compareToAscii("ActiveConnection"))
        {
            m_xActiveConnection.set(aProperty.Value,UNO_QUERY);
        }
        else
            ODatabaseAdministrationDialogBase::implInitialize(_rValue);
    }
    else
        ODatabaseAdministrationDialogBase::implInitialize(_rValue);
}

//.........................................................................
}   // namespace dbaui
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
