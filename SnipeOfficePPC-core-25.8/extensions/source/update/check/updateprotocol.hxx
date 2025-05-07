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

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/task/XInteractionHandler.hpp>
#include <com/sun/star/deployment/UpdateInformationProvider.hpp>

#include <vector>
#include "updateinfo.hxx"

// Returns 'true' if successfully connected to the update server
bool checkForUpdates(
    UpdateInfo& o_rUpdateInfo,
    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
    const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >& rxInteractionHandler,
    const ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XUpdateInformationProvider >& rxProvider
);

// The same as above, that does not read the info from bootstrap
SAL_DLLPUBLIC_EXPORT bool
checkForUpdates(
    UpdateInfo& o_rUpdateInfo,
    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > & rxContext,
    const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler > & rxInteractionHandler,
    const ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XUpdateInformationProvider >& rUpdateInfoProvider,
    const rtl::OUString &rOS,
    const rtl::OUString &rArch,
    const ::com::sun::star::uno::Sequence< rtl::OUString > &rRepositoryList,
    const rtl::OUString &rGitID,
    const rtl::OUString &rInstallID
);

// Returns 'true' if there are updates for any extension
bool checkForExtensionUpdates(
    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext
);

bool checkForPendingUpdates(
    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext
);

bool storeExtensionUpdateInfos(
    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
    const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< rtl::OUString > > &rUpdateInfos
);

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
