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

#include <osl/diagnose.h>
#include "smplmailsuppl.hxx"
#include "smplmailclient.hxx"

using com::sun::star::uno::Reference;
using com::sun::star::uno::RuntimeException;
using com::sun::star::uno::Sequence;
using com::sun::star::lang::XServiceInfo;
using com::sun::star::system::XSimpleMailClientSupplier;
using com::sun::star::system::XSimpleMailClient;
using rtl::OUString;
using osl::Mutex;

using namespace cppu;

#define COMP_IMPL_NAME  "com.sun.star.sys.shell.SimpleSystemMail"

namespace // private
{
    Sequence< OUString > SAL_CALL Component_getSupportedServiceNames()
    {
        Sequence< OUString > aRet(1);
        aRet[0] = OUString("com.sun.star.sys.shell.SimpleSystemMail");
        return aRet;
    }

} // end private namespace

CSmplMailSuppl::CSmplMailSuppl() :
    WeakComponentImplHelper2<XSimpleMailClientSupplier, XServiceInfo>(m_aMutex)
{
}

CSmplMailSuppl::~CSmplMailSuppl()
{
}

Reference<XSimpleMailClient> SAL_CALL CSmplMailSuppl::querySimpleMailClient()
    throw (RuntimeException)
{
    /* We just try to load the MAPI dll as a test
       if a mail client is available */
    Reference<XSimpleMailClient> xSmplMailClient;
    HMODULE handle = LoadLibrary("mapi32.dll");
    if ((handle != INVALID_HANDLE_VALUE) && (handle != NULL))
    {
        FreeLibrary(handle);
        xSmplMailClient = Reference<XSimpleMailClient>(new CSmplMailClient());
    }
    return xSmplMailClient;
}

// XServiceInfo

OUString SAL_CALL CSmplMailSuppl::getImplementationName()
    throw(RuntimeException)
{
    return OUString(COMP_IMPL_NAME);
}

sal_Bool SAL_CALL CSmplMailSuppl::supportsService(const OUString& ServiceName)
    throw(RuntimeException)
{
    Sequence <OUString> SupportedServicesNames = Component_getSupportedServiceNames();

    for (sal_Int32 n = SupportedServicesNames.getLength(); n--;)
        if (SupportedServicesNames[n].compareTo(ServiceName) == 0)
            return sal_True;

    return sal_False;
}

Sequence<OUString> SAL_CALL CSmplMailSuppl::getSupportedServiceNames()
    throw(RuntimeException)
{
    return Component_getSupportedServiceNames();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
