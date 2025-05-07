/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


#include "unoDirectSql.hxx"
#include "dbu_reghelper.hxx"
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/connection/XConnection.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include "directsql.hxx"
#include "dbustrings.hrc"
#include "datasourceconnector.hxx"
#include <tools/diagnose_ex.h>


extern "C" void SAL_CALL createRegistryInfo_ODirectSQLDialog()
{
    static ::dbaui::OMultiInstanceAutoRegistration< ::dbaui::ODirectSQLDialog > aAutoRegistration;
}

//.........................................................................
namespace dbaui
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::container;
    using namespace ::com::sun::star::sdbcx;
    using namespace ::com::sun::star::sdbc;
    using namespace ::com::sun::star::sdb;

    //=====================================================================
    //= ODirectSQLDialog
    //=====================================================================
DBG_NAME(ODirectSQLDialog)
//---------------------------------------------------------------------
    ODirectSQLDialog::ODirectSQLDialog(const Reference< XMultiServiceFactory >& _rxORB)
        :ODirectSQLDialog_BASE( _rxORB )
    {
        DBG_CTOR(ODirectSQLDialog,NULL);

    }

    //---------------------------------------------------------------------
    ODirectSQLDialog::~ODirectSQLDialog()
    {

        DBG_DTOR(ODirectSQLDialog,NULL);
    }

    //---------------------------------------------------------------------
    IMPLEMENT_IMPLEMENTATION_ID( ODirectSQLDialog )

    //---------------------------------------------------------------------
    IMPLEMENT_SERVICE_INFO1_STATIC( ODirectSQLDialog, "com.sun.star.comp.sdb.DirectSQLDialog", SERVICE_SDB_DIRECTSQLDIALOG.ascii )

    //---------------------------------------------------------------------
    IMPLEMENT_PROPERTYCONTAINER_DEFAULTS( ODirectSQLDialog )

    //---------------------------------------------------------------------
    Dialog* ODirectSQLDialog::createDialog(Window* _pParent)
    {
        // obtain all the objects needed for the dialog
        Reference< XConnection > xConnection = m_xActiveConnection;
        if ( !xConnection.is() )
        {
            try
            {
                // the connection the row set is working with
                ODatasourceConnector aDSConnector(m_aContext.getLegacyServiceFactory(),_pParent);
                xConnection = aDSConnector.connect( m_sInitialSelection, NULL );
            }
            catch( const Exception& )
            {
                DBG_UNHANDLED_EXCEPTION();
            }
        }
        if ( !xConnection.is() )
            // can't create the dialog if I have improper settings
            return NULL;

        return new DirectSQLDialog( _pParent, xConnection);
    }
    //---------------------------------------------------------------------
    void ODirectSQLDialog::implInitialize(const Any& _rValue)
    {
        PropertyValue aProperty;
        if (_rValue >>= aProperty)
        {
            if (0 == aProperty.Name.compareToAscii("InitialSelection"))
            {
                OSL_VERIFY( aProperty.Value >>= m_sInitialSelection );
                return;
            }
            else if (0 == aProperty.Name.compareToAscii("ActiveConnection"))
            {
                m_xActiveConnection.set( aProperty.Value, UNO_QUERY );
                OSL_ENSURE( m_xActiveConnection.is(), "ODirectSQLDialog::implInitialize: invalid connection!" );
                return;
            }
        }
        ODirectSQLDialog_BASE::implInitialize(_rValue);
    }
//.........................................................................
}   // namespace dbaui
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
