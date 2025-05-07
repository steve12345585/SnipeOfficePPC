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


#include "wizardservices.hxx"
#include "unoautopilot.hxx"
#include "groupboxwiz.hxx"
#include "listcombowizard.hxx"
#include "gridwizard.hxx"

// the registration methods
extern "C" void SAL_CALL createRegistryInfo_OGroupBoxWizard()
{
    static ::dbp::OMultiInstanceAutoRegistration<
        ::dbp::OUnoAutoPilot< ::dbp::OGroupBoxWizard, ::dbp::OGroupBoxSI >
    > aAutoRegistration;
}

extern "C" void SAL_CALL createRegistryInfo_OListComboWizard()
{
    static ::dbp::OMultiInstanceAutoRegistration<
        ::dbp::OUnoAutoPilot< ::dbp::OListComboWizard, ::dbp::OListComboSI >
    > aAutoRegistration;
}

extern "C" void SAL_CALL createRegistryInfo_OGridWizard()
{
    static ::dbp::OMultiInstanceAutoRegistration<
        ::dbp::OUnoAutoPilot< ::dbp::OGridWizard, ::dbp::OGridSI >
    > aAutoRegistration;
}

//.........................................................................
namespace dbp
{
//.........................................................................

    using namespace ::com::sun::star::uno;

    //=====================================================================
    //= OGroupBoxSI
    //=====================================================================
    //---------------------------------------------------------------------
    ::rtl::OUString OGroupBoxSI::getImplementationName() const
    {
        return ::rtl::OUString("org.openoffice.comp.dbp.OGroupBoxWizard");
    }

    //---------------------------------------------------------------------
    Sequence< ::rtl::OUString > OGroupBoxSI::getServiceNames() const
    {
        Sequence< ::rtl::OUString > aReturn(1);
        aReturn[0] = ::rtl::OUString("com.sun.star.sdb.GroupBoxAutoPilot");
        return aReturn;
    }

    //=====================================================================
    //= OListComboSI
    //=====================================================================
    //---------------------------------------------------------------------
    ::rtl::OUString OListComboSI::getImplementationName() const
    {
        return ::rtl::OUString("org.openoffice.comp.dbp.OListComboWizard");
    }

    //---------------------------------------------------------------------
    Sequence< ::rtl::OUString > OListComboSI::getServiceNames() const
    {
        Sequence< ::rtl::OUString > aReturn(1);
        aReturn[0] = ::rtl::OUString("com.sun.star.sdb.ListComboBoxAutoPilot");
        return aReturn;
    }

    //=====================================================================
    //= OGridSI
    //=====================================================================
    //---------------------------------------------------------------------
    ::rtl::OUString OGridSI::getImplementationName() const
    {
        return ::rtl::OUString("org.openoffice.comp.dbp.OGridWizard");
    }

    //---------------------------------------------------------------------
    Sequence< ::rtl::OUString > OGridSI::getServiceNames() const
    {
        Sequence< ::rtl::OUString > aReturn(1);
        aReturn[0] = ::rtl::OUString("com.sun.star.sdb.GridControlAutoPilot");
        return aReturn;
    }

//.........................................................................
}   // namespace dbp
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
