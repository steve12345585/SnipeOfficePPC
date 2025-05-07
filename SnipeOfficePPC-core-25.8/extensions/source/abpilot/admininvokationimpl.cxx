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

#include "admininvokationimpl.hxx"
#include <tools/debug.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <vcl/stdtext.hxx>
#include <toolkit/unohlp.hxx>
#include "abpresid.hrc"
#include "componentmodule.hxx"
#include <vcl/waitobj.hxx>


//.........................................................................
namespace abp
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::ui::dialogs;

    //=====================================================================
    //= OAdminDialogInvokation
    //=====================================================================
    //---------------------------------------------------------------------
    OAdminDialogInvokation::OAdminDialogInvokation(const Reference< XMultiServiceFactory >& _rxORB
                    , const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > _xDataSource
                    , Window* _pMessageParent)
        :m_xORB(_rxORB)
        ,m_xDataSource(_xDataSource)
        ,m_pMessageParent(_pMessageParent)
    {
        DBG_ASSERT(m_xORB.is(), "OAdminDialogInvokation::OAdminDialogInvokation: invalid service factory!");
        DBG_ASSERT(m_xDataSource.is(), "OAdminDialogInvokation::OAdminDialogInvokation: invalid preferred name!");
        DBG_ASSERT(m_pMessageParent, "OAdminDialogInvokation::OAdminDialogInvokation: invalid message parent!");
    }

    //---------------------------------------------------------------------
    sal_Bool OAdminDialogInvokation::invokeAdministration( sal_Bool _bFixedType )
    {
        if (!m_xORB.is())
            return sal_False;

        try
        {
            // the service name of the administration dialog
            const static ::rtl::OUString s_sAdministrationServiceName = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sdb.DatasourceAdministrationDialog"));
            const static ::rtl::OUString s_sDataSourceTypeChangeDialog = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sdb.DataSourceTypeChangeDialog"));

            // the parameters for the call
            Sequence< Any > aArguments(3);
            Any* pArguments = aArguments.getArray();

            // the parent window
            Reference< XWindow > xDialogParent = VCLUnoHelper::GetInterface(m_pMessageParent);
            *pArguments++ <<= PropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ParentWindow")), -1, makeAny(xDialogParent), PropertyState_DIRECT_VALUE);

            // the title of the dialog
            String sAdminDialogTitle(ModuleRes(RID_STR_ADMINDIALOGTITLE));
            *pArguments++ <<= PropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Title")), -1, makeAny(::rtl::OUString(sAdminDialogTitle)), PropertyState_DIRECT_VALUE);

            // the name of the new data source
            *pArguments++ <<= PropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("InitialSelection")), -1, makeAny(m_xDataSource), PropertyState_DIRECT_VALUE);

            // create the dialog
            Reference< XExecutableDialog > xDialog;
            {
                // creating the dialog service is potentially expensive (if all the libraries invoked need to be loaded)
                // so we display a wait cursor
                WaitObject aWaitCursor(m_pMessageParent);
                xDialog = Reference< XExecutableDialog >( m_xORB->createInstanceWithArguments( _bFixedType ? s_sAdministrationServiceName : s_sDataSourceTypeChangeDialog, aArguments ), UNO_QUERY );

                // just for a smoother UI: What the dialog does upon execution, is (amongst other things) creating
                // the DriverManager service
                // If this context has never been accessed before, this may be expensive (it includes loading of
                // at least one library).
                // As this wizard is intended to run on the first office start, it is very likely that the
                // context needs to be freshly created
                // Thus, we access the context here (within the WaitCursor), which means the user sees a waitcursor
                // while his/her office blocks a few seconds ....
                m_xORB->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sdbc.DriverManager" )) );
            }

            if (xDialog.is())
            {   // execute it
                if (xDialog->execute())
                    return sal_True;
            }
            else
                ShowServiceNotAvailableError(m_pMessageParent, s_sAdministrationServiceName, sal_True);
        }
        catch(const Exception&)
        {
            OSL_FAIL("OAdminDialogInvokation::invokeAdministration: caught an exception while executing the dialog!");
        }
        return sal_False;
    }

//.........................................................................
}   // namespace abp
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
