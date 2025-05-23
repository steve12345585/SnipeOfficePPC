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
#ifndef INCLUDED_DP_GUI_UPDATEDATA_HXX
#define INCLUDED_DP_GUI_UPDATEDATA_HXX

#include "sal/config.h"
#include "tools/solar.h"
#include "rtl/ustring.hxx"
#include "com/sun/star/uno/Reference.hxx"

#include <boost/shared_ptr.hpp>


namespace com { namespace sun { namespace star { namespace deployment {
        class XPackage;
}}}}
namespace com { namespace sun { namespace star { namespace xml { namespace dom {
        class XNode;
}}}}}


namespace dp_gui {

struct UpdateData
{
    UpdateData( ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage > const & aExt):
        bIsShared(false), aInstalledPackage(aExt){};

    //When entries added to the listbox then there can be one for the user update and one
    //for the shared update. However, both list entries will contain the same UpdateData.
    //isShared is used to indicate which one is used for the shared entry.
    bool bIsShared;

    //The currently installed extension which is going to be updated. If the extension exist in
    //multiple repositories then it is the one with the highest version.
    ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage > aInstalledPackage;

    //The version of the update
    ::rtl::OUString updateVersion;

    //For online update
    // ======================
    // The content of the update information.
    //Only if aUpdateInfo is set then there is an online update available with a better version
    //than any of the currently installed extensions with the same identifier.
    ::com::sun::star::uno::Reference< ::com::sun::star::xml::dom::XNode > aUpdateInfo;
    //The URL of the locally downloaded extension. It will only be set if there were no errors
    //during the download
    ::rtl::OUString sLocalURL;
    //The URL of the website wher the download can be obtained.
    ::rtl::OUString sWebsiteURL;

    //For local update
    //=====================
    //The locale extension which is used as update for the user or shared repository.
    //If set then the data for the online update (aUpdateInfo, sLocalURL, sWebsiteURL)
    //are to be ignored.
    ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage >
    aUpdateSource;

    // ID to find this entry in the update listbox
    sal_uInt16  m_nID;
    bool    m_bIgnored;
};
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
