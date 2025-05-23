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

#ifndef _SV_SALTIMER_HXX
#define _SV_SALTIMER_HXX

#include <tools/solar.h>
#include <vcl/dllapi.h>
#include <salwtype.hxx>

// ------------
// - SalTimer -
// ------------

/*
 * note: there will be only a single instance of SalTimer
 * SalTimer originally had only static methods, but
 * this needed to be virtualized for the sal plugin migration
 */

class VCL_PLUGIN_PUBLIC SalTimer
{
    SALTIMERPROC        m_pProc;
public:
    SalTimer() : m_pProc( NULL ) {}
    virtual ~SalTimer();

    // AutoRepeat and Restart
    virtual void            Start( sal_uLong nMS ) = 0;
    virtual void            Stop() = 0;

    // Callbacks (indepen in \sv\source\app\timer.cxx)
    void            SetCallback( SALTIMERPROC pProc )
    {
        m_pProc = pProc;
    }

    void            CallCallback()
    {
        if( m_pProc )
            m_pProc();
    }
};

#endif // _SV_SALTIMER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
