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

#ifndef BASIC_VBAHELPR_HXX
#define BASIC_VBAHELPR_HXX

#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <rtl/ustring.hxx>
#include "basicdllapi.h"

namespace basic {
namespace vba {

/*  This header contains public helper functions for VBA used from this module
    and from other VBA implementation modules such as vbahelper.
 */

// ============================================================================

/** Locks or unlocks the controllers of all documents that have the same type
    as the specified document.

    First, the global module manager (com.sun.star.frame.ModuleManager) is
    asked for the type of the passed model, and all open documents with the
    same type will be locked or unlocked.

    @param rxModel
        A document model determining the type of the documents to be locked or
        unlocked.

    @param bLockControllers
        Passing true will lock all controllers, passing false will unlock them.
 */
BASIC_DLLPUBLIC void lockControllersOfAllDocuments(
    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& rxModel,
    sal_Bool bLockControllers );

// ============================================================================

/** Enables or disables the container windows of all controllers of all
    documents that have the same type as the specified document.

    First, the global module manager (com.sun.star.frame.ModuleManager) is
    asked for the type of the passed model, and the container windows of all
    open documents with the same type will be enabled or disabled.

    @param rxModel
        A document model determining the type of the documents to be enabled or
        disabled.

    @param bEnableWindows
        Passing true will enable all container windows of all controllers,
        passing false will disable them.
 */
BASIC_DLLPUBLIC void enableContainerWindowsOfAllDocuments(
    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& rxModel,
    sal_Bool bEnableWindows );

// ============================================================================

/** Registers the passed path as working directory for the application the
    passed document belongs to.

    @param rxModel
        A document model determining the type of the application whose working
        directory has been changed.

    @param rPath
        The new working directory.
 */
BASIC_DLLPUBLIC void registerCurrentDirectory(
    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& rxModel,
    const ::rtl::OUString& rPath );

// ============================================================================

} // namespace vba
} // namespace basic

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
