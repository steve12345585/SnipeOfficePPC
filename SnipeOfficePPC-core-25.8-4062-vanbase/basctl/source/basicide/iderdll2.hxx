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

#ifndef BASCTL_IDERDLL2_HXX
#define BASCTL_IDERDLL2_HXX

class StarBASIC;
class SvxSearchItem;

#include <tools/gen.hxx>
#include <tools/link.hxx>

#include <bastypes.hxx>
#include <bastype2.hxx>

#include <boost/scoped_ptr.hpp>

namespace basctl
{

class ExtraData
{
private:
    boost::scoped_ptr<SvxSearchItem> pSearchItem;

    LibInfos        aLibInfos;

    EntryDescriptor m_aLastEntryDesc;

    String          aAddLibPath;
    String          aAddLibFilter;

    sal_uInt16      nBasicDialogCount;

    bool            bChoosingMacro;
    bool            bShellInCriticalSection;

protected:
    DECL_LINK( GlobalBasicBreakHdl, StarBASIC * );

public:
    ExtraData();
    ~ExtraData();

    LibInfos&         GetLibInfos () { return aLibInfos; }

    EntryDescriptor&  GetLastEntryDescriptor () { return m_aLastEntryDesc; }
    void              SetLastEntryDescriptor (EntryDescriptor& rDesc) { m_aLastEntryDesc = rDesc; }

    bool&             ChoosingMacro()          { return bChoosingMacro; }
    bool&             ShellInCriticalSection() { return bShellInCriticalSection; }

    sal_uInt16        GetBasicDialogCount() const { return nBasicDialogCount; }
    void              IncBasicDialogCount()       { nBasicDialogCount++; }
    void              DecBasicDialogCount()       { nBasicDialogCount--; }

    SvxSearchItem&    GetSearchItem() const { return *pSearchItem; }
    void              SetSearchItem( const SvxSearchItem& rItem );

    const String&     GetAddLibPath() const   { return aAddLibPath; }
    void              SetAddLibPath( const String& rPath ) { aAddLibPath = rPath; }

    const String&     GetAddLibFilter() const { return aAddLibFilter; }
    void              SetAddLibFilter( const String& rFilter )  { aAddLibFilter = rFilter; }
};

} // namespace basctl

#endif // BASCTL_IDERDLL2_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
