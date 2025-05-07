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
#ifndef _TOOLS_TIME_HXX
#define _TOOLS_TIME_HXX

#include "tools/toolsdllapi.h"
#include <tools/solar.h>

class ResId;

// --------
// - Time -
// --------

/**
 WARNING: This class can serve both as call clock time and time duration, and the mixing
          of these concepts leads to problems such as there being 25 hours
          or 10 minus 20 seconds being (non-negative) 10 seconds.
*/

class TOOLS_DLLPUBLIC Time
{
private:
    sal_Int32           nTime;

public:

    enum TimeInitSystem
    {
        SYSTEM
    };
    // temporary until all uses are inspected and resolved
    enum TimeInitEmpty
    {
        EMPTY
    };

                    Time( TimeInitEmpty )
                        { nTime = 0; }
                    Time( TimeInitSystem );
                    Time( const ResId & rResId );
                    Time( sal_Int32 _nTime ) { Time::nTime = _nTime; }
                    Time( const Time& rTime );
                    Time( sal_uIntPtr nHour, sal_uIntPtr nMin,
                          sal_uIntPtr nSec = 0, sal_uIntPtr n100Sec = 0 );

    void            SetTime( sal_Int32 nNewTime ) { nTime = nNewTime; }
    sal_Int32       GetTime() const { return nTime; }

    void            SetHour( sal_uInt16 nNewHour );
    void            SetMin( sal_uInt16 nNewMin );
    void            SetSec( sal_uInt16 nNewSec );
    void            Set100Sec( sal_uInt16 nNew100Sec );
    sal_uInt16          GetHour() const
                        { sal_uIntPtr nTempTime = (nTime >= 0) ? nTime : nTime*-1;
                          return (sal_uInt16)(nTempTime / 1000000); }
    sal_uInt16          GetMin() const
                        { sal_uIntPtr nTempTime = (nTime >= 0) ? nTime : nTime*-1;
                          return (sal_uInt16)((nTempTime / 10000) % 100); }
    sal_uInt16          GetSec() const
                        { sal_uIntPtr nTempTime = (nTime >= 0) ? nTime : nTime*-1;
                          return (sal_uInt16)((nTempTime / 100) % 100); }
    sal_uInt16          Get100Sec() const
                        { sal_uIntPtr nTempTime = (nTime >= 0) ? nTime : nTime*-1;
                          return (sal_uInt16)(nTempTime % 100); }

    sal_Int32       GetMSFromTime() const;
    void            MakeTimeFromMS( sal_Int32 nMS );

                    /// 12 hours == 0.5 days
    double          GetTimeInDays() const;

    sal_Bool            IsBetween( const Time& rFrom, const Time& rTo ) const
                        { return ((nTime >= rFrom.nTime) && (nTime <= rTo.nTime)); }

    sal_Bool            IsEqualIgnore100Sec( const Time& rTime ) const;

    sal_Bool            operator ==( const Time& rTime ) const
                        { return (nTime == rTime.nTime); }
    sal_Bool            operator !=( const Time& rTime ) const
                        { return (nTime != rTime.nTime); }
    sal_Bool            operator  >( const Time& rTime ) const
                        { return (nTime > rTime.nTime); }
    sal_Bool            operator  <( const Time& rTime ) const
                        { return (nTime < rTime.nTime); }
    sal_Bool            operator >=( const Time& rTime ) const
                        { return (nTime >= rTime.nTime); }
    sal_Bool            operator <=( const Time& rTime ) const
                        { return (nTime <= rTime.nTime); }

    static Time     GetUTCOffset();
    static sal_uIntPtr  GetSystemTicks();       // Elapsed time

    void            ConvertToUTC()       { *this -= Time::GetUTCOffset(); }
    void            ConvertToLocalTime() { *this += Time::GetUTCOffset(); }

    Time&           operator =( const Time& rTime );
    Time            operator -() const
                        { return Time( nTime * -1 ); }
    Time&           operator +=( const Time& rTime );
    Time&           operator -=( const Time& rTime );
    TOOLS_DLLPUBLIC friend Time     operator +( const Time& rTime1, const Time& rTime2 );
    TOOLS_DLLPUBLIC friend Time     operator -( const Time& rTime1, const Time& rTime2 );
};

#endif // _TOOLS_TIME_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
