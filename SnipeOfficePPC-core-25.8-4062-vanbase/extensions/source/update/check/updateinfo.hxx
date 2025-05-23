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

#ifndef _UPDATE_INFO_INCLUDED_
#define _UPDATE_INFO_INCLUDED_

#include <rtl/ustring.hxx>
#include <vector>

struct DownloadSource
{
    bool IsDirect;
    rtl::OUString URL;

    DownloadSource(bool bIsDirect, const rtl::OUString& aURL) : IsDirect(bIsDirect), URL(aURL) {};
    DownloadSource(const DownloadSource& ds) : IsDirect(ds.IsDirect), URL(ds.URL) {};

    DownloadSource & operator=( const DownloadSource & ds ) { IsDirect = ds.IsDirect; URL = ds.URL; return *this; };
};

struct ReleaseNote
{
    sal_uInt8 Pos;
    rtl::OUString URL;
    sal_uInt8 Pos2;
    rtl::OUString URL2;

    ReleaseNote(sal_uInt8 pos, const rtl::OUString aURL) : Pos(pos), URL(aURL), Pos2(0), URL2() {};
    ReleaseNote(sal_uInt8 pos, const rtl::OUString aURL, sal_uInt8 pos2, const rtl::OUString aURL2) : Pos(pos), URL(aURL), Pos2(pos2), URL2(aURL2) {};

    ReleaseNote(const ReleaseNote& rn) :Pos(rn.Pos), URL(rn.URL), Pos2(rn.Pos2), URL2(rn.URL2) {};
    ReleaseNote & operator=( const ReleaseNote& rn) { Pos=rn.Pos; URL=rn.URL; Pos2=rn.Pos2; URL2=rn.URL2; return *this; };
};

struct UpdateInfo
{
    rtl::OUString BuildId;
    rtl::OUString Version;
    rtl::OUString Description;
    std::vector< DownloadSource > Sources;
    std::vector< ReleaseNote > ReleaseNotes;

    UpdateInfo() : BuildId(), Version(), Description(), Sources(), ReleaseNotes() {};
    UpdateInfo(const UpdateInfo& ui) : BuildId(ui.BuildId), Version(ui.Version), Description(ui.Description), Sources(ui.Sources), ReleaseNotes(ui.ReleaseNotes) {};
    inline UpdateInfo & operator=( const UpdateInfo& ui );
};

UpdateInfo & UpdateInfo::operator=( const UpdateInfo& ui )
{
    BuildId = ui.BuildId;
    Version = ui.Version;
    Description = ui.Description;
    Sources = ui.Sources;
    ReleaseNotes = ui.ReleaseNotes;
    return *this;
}


// Returns the URL of the release note for the given position
rtl::OUString getReleaseNote(const UpdateInfo& rInfo, sal_uInt8 pos, bool autoDownloadEnabled=false);

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
