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

#ifndef INCLUDED_DP_UPDATE_HXX
#define INCLUDED_DP_UPDATE_HXX


#include "com/sun/star/deployment/XPackage.hpp"
#include "com/sun/star/deployment/XExtensionManager.hpp"
#include "com/sun/star/deployment/XUpdateInformationProvider.hpp"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/xml/dom/XNode.hpp"

#include "rtl/ustrbuf.hxx"
#include "dp_misc_api.hxx"

#include <map>
#include <vector>

namespace dp_misc {

/** returns the default update URL (for the update information) which
    is used when an extension does not provide its own URL.
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::rtl::OUString getExtensionDefaultUpdateURL();

enum UPDATE_SOURCE
{
    UPDATE_SOURCE_NONE,
    UPDATE_SOURCE_SHARED,
    UPDATE_SOURCE_BUNDLED,
    UPDATE_SOURCE_ONLINE
};

/* determine if an update is available which is installed in the
   user repository.

   If the return value is UPDATE_SOURCE_NONE, then no update is
   available, otherwise the return value determine from which the
   repository the update is used.
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
UPDATE_SOURCE isUpdateUserExtension(
    bool bReadOnlyShared,
    ::rtl::OUString const & userVersion,
    ::rtl::OUString const & sharedVersion,
    ::rtl::OUString const & bundledVersion,
    ::rtl::OUString const & onlineVersion);

/* determine if an update is available which is installed in the
   shared repository.

   If the return value is UPDATE_SOURCE_NONE, then no update is
   available, otherwise the return value determine from which the
   repository the update is used.
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
UPDATE_SOURCE isUpdateSharedExtension(
    bool bReadOnlyShared,
    ::rtl::OUString const & sharedVersion,
    ::rtl::OUString const & bundledVersion,
    ::rtl::OUString const & onlineVersion);

/* determines the extension with the highest identifier and returns it

 */
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage>
getExtensionWithHighestVersion(
    ::com::sun::star::uno::Sequence<
    ::com::sun::star::uno::Reference<
    ::com::sun::star::deployment::XPackage> > const & seqExtensionsWithSameId);


struct UpdateInfo
{
    UpdateInfo( ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage> const & ext);
    ::com::sun::star::uno::Reference<
        ::com::sun::star::deployment::XPackage> extension;
//version of the update
    ::rtl::OUString version;
    ::com::sun::star::uno::Reference< ::com::sun::star::xml::dom::XNode > info;
};

typedef std::map< ::rtl::OUString, UpdateInfo > UpdateInfoMap;

/*
  @param extensionList
  List of extension for which online update information are to be obtained. If NULL, then
  for update information are obtained for all installed extension. There may be only one extension
  with a particular identifier contained in the list. If one extension is installed
  in several repositories, then the one with the highest version must be used, because it contains
  the more recent URLs for getting the update information (if at all).
  @param out_errors
  the first member of the pair is the extension  and the second the exception that was produced
  when processing the extension.

  @return
  A map of UpdateInfo instances. If the parameter extensionList was given, then the map contains
  at only information for those extensions.
 */
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
UpdateInfoMap getOnlineUpdateInfos(
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext> const &xContext,
    ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XExtensionManager> const & xExtMgr,
    ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XUpdateInformationProvider > const & updateInformation,
    std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage > > const * extensionList,
    ::std::vector< ::std::pair< ::com::sun::star::uno::Reference<
    ::com::sun::star::deployment::XPackage>, ::com::sun::star::uno::Any> > & out_errors);

/* retunrs the highest version from the provided arguments.
*/
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::rtl::OUString getHighestVersion(
    ::rtl::OUString const & userVersion,
    ::rtl::OUString const & sharedVersion,
    ::rtl::OUString const & bundledVersion,
    ::rtl::OUString const & onlineVersion);

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
