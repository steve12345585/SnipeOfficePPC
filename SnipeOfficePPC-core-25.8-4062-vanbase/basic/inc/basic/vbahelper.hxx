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
