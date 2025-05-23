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

#ifndef SC_AUTOSTYL_HXX
#define SC_AUTOSTYL_HXX

#include <boost/ptr_container/ptr_vector.hpp>

#include <tools/string.hxx>

#include <vcl/timer.hxx>

class ScDocShell;
class ScRange;
struct ScAutoStyleData;
struct ScAutoStyleInitData;

class ScAutoStyleList
{
private:

    ScDocShell*     pDocSh;
    Timer           aTimer;
    Timer           aInitTimer;
    sal_uLong           nTimerStart;
    boost::ptr_vector<ScAutoStyleData> aEntries;
    boost::ptr_vector<ScAutoStyleInitData> aInitials;

    void    ExecuteEntries();
    void    AdjustEntries(sal_uLong nDiff);
    void    StartTimer(sal_uLong nNow);
    DECL_LINK( TimerHdl, void* );
    DECL_LINK( InitHdl, void* );

public:
            ScAutoStyleList(ScDocShell* pShell);
            ~ScAutoStyleList();

    void    AddInitial( const ScRange& rRange, const String& rStyle1,
                        sal_uLong nTimeout, const String& rStyle2 );
    void    AddEntry( sal_uLong nTimeout, const ScRange& rRange, const String& rStyle );

    void    ExecuteAllNow();
};



#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
