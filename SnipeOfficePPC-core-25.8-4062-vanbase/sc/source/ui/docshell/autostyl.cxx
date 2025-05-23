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


#include <time.h>

#include "attrib.hxx"
#include "autostyl.hxx"
#include "docsh.hxx"
#include "sc.hrc"

struct ScAutoStyleInitData
{
    ScRange aRange;
    String  aStyle1;
    sal_uLong   nTimeout;
    String  aStyle2;

    ScAutoStyleInitData( const ScRange& rR, const String& rSt1, sal_uLong nT, const String& rSt2 ) :
        aRange(rR), aStyle1(rSt1), nTimeout(nT), aStyle2(rSt2) {}
};

struct ScAutoStyleData
{
    sal_uLong   nTimeout;
    ScRange aRange;
    String  aStyle;

    ScAutoStyleData( sal_uLong nT, const ScRange& rR, const String& rT ) :
        nTimeout(nT), aRange(rR), aStyle(rT) {}
};

inline sal_uLong TimeNow()          // Sekunden
{
    return (sal_uLong) time(0);
}

namespace {

class FindByRange : public ::std::unary_function<ScAutoStyleData, bool>
{
    ScRange maRange;
public:
    FindByRange(const ScRange& r) : maRange(r) {}
    bool operator() (const ScAutoStyleData& rData) const { return rData.aRange == maRange; }
};

class FindByTimeout : public ::std::unary_function<ScAutoStyleData, bool>
{
    sal_uLong mnTimeout;
public:
    FindByTimeout(sal_uLong n) : mnTimeout(n) {}
    bool operator() (const ScAutoStyleData& rData) const { return rData.nTimeout >= mnTimeout; }
};

struct FindNonZeroTimeout : public ::std::unary_function<ScAutoStyleData, bool>
{
    bool operator() (const ScAutoStyleData& rData) const
    {
        return rData.nTimeout != 0;
    }
};

}

ScAutoStyleList::ScAutoStyleList(ScDocShell* pShell) :
    pDocSh( pShell )
{
    aTimer.SetTimeoutHdl( LINK( this, ScAutoStyleList, TimerHdl ) );
    aInitTimer.SetTimeoutHdl( LINK( this, ScAutoStyleList, InitHdl ) );
    aInitTimer.SetTimeout( 0 );
}

ScAutoStyleList::~ScAutoStyleList()
{
}

//  initial short delay (asynchronous call)

void ScAutoStyleList::AddInitial( const ScRange& rRange, const String& rStyle1,
                                    sal_uLong nTimeout, const String& rStyle2 )
{
    aInitials.push_back(new ScAutoStyleInitData( rRange, rStyle1, nTimeout, rStyle2 ));
    aInitTimer.Start();
}

IMPL_LINK_NOARG(ScAutoStyleList, InitHdl)
{
    boost::ptr_vector<ScAutoStyleInitData>::iterator iter;
    for (iter = aInitials.begin(); iter != aInitials.end(); ++iter)
    {
        //  apply first style immediately
        pDocSh->DoAutoStyle(iter->aRange,iter->aStyle1);

        //  add second style to list
        if (iter->nTimeout)
            AddEntry(iter->nTimeout,iter->aRange,iter->aStyle2 );
    }

    aInitials.clear();

    return 0;
}

void ScAutoStyleList::AddEntry( sal_uLong nTimeout, const ScRange& rRange, const String& rStyle )
{
    aTimer.Stop();
    sal_uLong nNow = TimeNow();

    // Remove the first item with the same range.
    ::boost::ptr_vector<ScAutoStyleData>::iterator itr =
        ::std::find_if(aEntries.begin(), aEntries.end(), FindByRange(rRange));

    if (itr != aEntries.end())
        aEntries.erase(itr);

    //  Timeouts von allen Eintraegen anpassen

    if (!aEntries.empty() && nNow != nTimerStart)
    {
        OSL_ENSURE(nNow>nTimerStart, "Zeit laeuft rueckwaerts?");
        AdjustEntries((nNow-nTimerStart)*1000);
    }

    //  Einfuege-Position suchen
    boost::ptr_vector<ScAutoStyleData>::iterator iter =
        ::std::find_if(aEntries.begin(), aEntries.end(), FindByTimeout(nTimeout));

    aEntries.insert(iter,new ScAutoStyleData(nTimeout,rRange,rStyle));

    //  abgelaufene ausfuehren, Timer neu starten

    ExecuteEntries();
    StartTimer(nNow);
}

void ScAutoStyleList::AdjustEntries( sal_uLong nDiff )  // Millisekunden
{
    boost::ptr_vector<ScAutoStyleData>::iterator iter;
    for (iter = aEntries.begin(); iter != aEntries.end(); ++iter)
    {
        if (iter->nTimeout <= nDiff)
            iter->nTimeout = 0;                 // abgelaufen
        else
            iter->nTimeout -= nDiff;                // weiterzaehlen
    }
}

void ScAutoStyleList::ExecuteEntries()
{
    // Execute and remove all items with timeout == 0 from the begin position
    // until the first item with non-zero timeout value.
    ::boost::ptr_vector<ScAutoStyleData>::iterator itr = aEntries.begin(), itrEnd = aEntries.end();
    for (; itr != itrEnd; ++itr)
    {
        if (itr->nTimeout)
            break;

        pDocSh->DoAutoStyle(itr->aRange, itr->aStyle);
    }
    // At this point itr should be on the first item with non-zero timeout, or
    // the end position in case all items have timeout == 0.
    aEntries.erase(aEntries.begin(), itr);
}

void ScAutoStyleList::ExecuteAllNow()
{
    aTimer.Stop();

    boost::ptr_vector<ScAutoStyleData>::iterator iter;
    for (iter = aEntries.begin(); iter != aEntries.end(); ++iter)
        pDocSh->DoAutoStyle(iter->aRange,iter->aStyle);

    aEntries.clear();
}

void ScAutoStyleList::StartTimer( sal_uLong nNow )      // Sekunden
{
    // ersten Eintrag mit Timeout != 0 suchen
    boost::ptr_vector<ScAutoStyleData>::iterator iter =
        ::std::find_if(aEntries.begin(),aEntries.end(), FindNonZeroTimeout());

    if (iter != aEntries.end())
    {
        aTimer.SetTimeout(iter->nTimeout);
        aTimer.Start();
    }

    nTimerStart = nNow;
}

IMPL_LINK_NOARG(ScAutoStyleList, TimerHdl)
{
    sal_uLong nNow = TimeNow();
    AdjustEntries(aTimer.GetTimeout());             // eingestellte Wartezeit
    ExecuteEntries();
    StartTimer(nNow);

    return 0;
}




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
