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

#include "sal/types.h"
#include "rtl/ustring.hxx"

#include "cppuhelper/implementationentry.hxx"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"

#include "svtools/miscopt.hxx"
#include "svl/pickerhistoryaccess.hxx"

#include "vcl/svapp.hxx"

#ifdef WNT
#define GradientStyle_RECT BLA_GradientStyle_RECT
#include <windows.h>
#undef GradientStyle_RECT
#include <odma_lib.hxx>
#endif

namespace css = com::sun::star;

using css::uno::Reference;
using css::uno::Sequence;
using rtl::OUString;

/*
 * FilePicker implementation.
 */
static OUString FilePicker_getSystemPickerServiceName()
{
#ifdef UNX
    OUString aDesktopEnvironment (Application::GetDesktopEnvironment());
    if (aDesktopEnvironment.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("tde")))
        return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.TDEFilePicker"));
    else if (aDesktopEnvironment.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("kde")))
        return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.KDEFilePicker"));
    else if (aDesktopEnvironment.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("kde4")))
        return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.KDE4FilePicker"));
    else if (aDesktopEnvironment.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("macosx")))
        return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.AquaFilePicker"));
    else
        return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.SystemFilePicker"));
#endif
#ifdef WNT
    if (SvtMiscOptions().TryODMADialog() && ::odma::DMSsAvailable()) {
        return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.ODMAFilePicker"));
    }
    return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.Win32FilePicker"));
#endif
}

static Reference< css::uno::XInterface > FilePicker_createInstance (
    Reference< css::uno::XComponentContext > const & rxContext)
{
    Reference< css::uno::XInterface > xResult;

    if (!rxContext.is())
        return xResult;

    Reference< css::lang::XMultiComponentFactory > xFactory (rxContext->getServiceManager());
    if (xFactory.is() && SvtMiscOptions().UseSystemFileDialog())
    {
        xResult = Reference< css::uno::XInterface >( Application::createFilePicker( rxContext ) );

        if (!xResult.is())
        {
            try
            {
                xResult = xFactory->createInstanceWithContext (
                        FilePicker_getSystemPickerServiceName(),
                        rxContext);
            }
            catch (css::uno::Exception const &)
            {
                // Handled below (see @ fallback).
            }
        }
    }


    if (!xResult.is() && xFactory.is())
    {
        // Always fall back to OfficeFilePicker.
        xResult = xFactory->createInstanceWithContext (
                OUString (RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.ui.dialogs.OfficeFilePicker")),
                rxContext);
    }
    if (xResult.is())
    {
        // Add to FilePicker history.
        svt::addFilePicker (xResult);
    }
    return xResult;
}

static OUString FilePicker_getImplementationName()
{
    return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.comp.fpicker.FilePicker"));
}

static Sequence< OUString > FilePicker_getSupportedServiceNames()
{
    Sequence< OUString > aServiceNames(1);
    aServiceNames.getArray()[0] =
        OUString (RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.ui.dialogs.FilePicker"));
    return aServiceNames;
}

/*
 * FolderPicker implementation.
 */
static OUString FolderPicker_getSystemPickerServiceName()
{
    OUString aDesktopEnvironment (Application::GetDesktopEnvironment());
#ifdef UNX
    if (aDesktopEnvironment.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("tde")))
        return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.TDEFolderPicker"));
    else if (aDesktopEnvironment.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("kde")))
        return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.KDEFolderPicker"));
    else if (aDesktopEnvironment.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("macosx")))
        return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.AquaFolderPicker"));
#endif
#ifdef WNT
    if (SvtMiscOptions().TryODMADialog() && ::odma::DMSsAvailable()) {
        return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.ODMAFolderPicker"));
    }
#endif
    return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.ui.dialogs.SystemFolderPicker"));
}

static Reference< css::uno::XInterface > FolderPicker_createInstance (
    Reference< css::uno::XComponentContext > const & rxContext)
{
    Reference< css::uno::XInterface > xResult;

    if (!rxContext.is())
        return xResult;

    Reference< css::lang::XMultiComponentFactory > xFactory (rxContext->getServiceManager());
    if (xFactory.is() && SvtMiscOptions().UseSystemFileDialog())
    {
        xResult = Reference< css::uno::XInterface >( Application::createFolderPicker( rxContext ) );
        if (!xResult.is())
        {
            try
            {
                xResult = xFactory->createInstanceWithContext (
                                FolderPicker_getSystemPickerServiceName(),
                                rxContext);
            }
            catch (css::uno::Exception const &)
            {
                // Handled below (see @ fallback).
            }
        }
    }
    if (!xResult.is() && xFactory.is() )
    {
        // Always fall back to OfficeFolderPicker.
        xResult = xFactory->createInstanceWithContext (
                OUString (RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.ui.dialogs.OfficeFolderPicker")),
                rxContext);
    }
    if (xResult.is())
    {
        // Add to FolderPicker history.
        svt::addFolderPicker (xResult);
    }
    return xResult;
}

static OUString FolderPicker_getImplementationName()
{
    return OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.comp.fpicker.FolderPicker"));
}

static Sequence< OUString > FolderPicker_getSupportedServiceNames()
{
    Sequence< OUString > aServiceNames(1);
    aServiceNames.getArray()[0] =
        OUString (RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.ui.dialogs.FolderPicker"));
    return aServiceNames;
}

/*
 * Implementation entries.
 */
static cppu::ImplementationEntry g_entries[] =
{
    {
        FilePicker_createInstance,
        FilePicker_getImplementationName,
        FilePicker_getSupportedServiceNames,
        cppu::createSingleComponentFactory, 0, 0
    },
    {
        FolderPicker_createInstance,
        FolderPicker_getImplementationName,
        FolderPicker_getSupportedServiceNames,
        cppu::createSingleComponentFactory, 0, 0
    },
    { 0, 0, 0, 0, 0, 0 }
};

/*
 * Public (exported) interface.
 */
extern "C"
{
SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory (
    const sal_Char * pImplementationName, void * pServiceManager, void * pRegistryKey)
{
    return cppu::component_getFactoryHelper (
        pImplementationName, pServiceManager, pRegistryKey, g_entries);
}

} // extern "C"

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
