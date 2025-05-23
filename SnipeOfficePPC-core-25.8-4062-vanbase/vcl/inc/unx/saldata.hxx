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

#ifndef _SV_SALDATA_HXX
#define _SV_SALDATA_HXX

#include <unx/salunx.h>
#include <vcl/salgtype.hxx>
#include <unx/salstd.hxx>
#include <salframe.hxx>
#include <unx/salinst.h>
#include <generic/gendata.hxx>
#include <osl/module.h>
#include <vclpluginapi.h>

// -=-= forwards -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class SalXLib;
class SalDisplay;
class SalPrinter;

// -=-= typedefs -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#if defined LINUX || defined NETBSD || defined AIX || \
    defined FREEBSD || defined OPENBSD || defined DRAGONFLY || \
    defined ANDROID
#include <pthread.h>
#else
typedef unsigned int pthread_t;
#endif

// -=-= SalData =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class VCLPLUG_GEN_PUBLIC X11SalData : public SalGenericData
{
    static int XErrorHdl( Display*, XErrorEvent* );
    static int XIOErrorHdl( Display* );

    struct XErrorStackEntry
    {
        bool            m_bIgnore;
        bool            m_bWas;
        unsigned int    m_nLastErrorRequest;
        XErrorHandler   m_aHandler;
    };
    std::vector< XErrorStackEntry > m_aXErrorHandlerStack;
    XIOErrorHandler m_aOrigXIOErrorHandler;

protected:
    SalXLib      *pXLib_;

public:
             X11SalData( SalGenericDataType t, SalInstance *pInstance );
    virtual ~X11SalData();

    virtual void            Init();
    virtual void            Dispose();

    virtual void            initNWF();
    virtual void            deInitNWF();

    SalDisplay*             GetX11Display() const { return GetSalDisplay(); }
    void                    DeleteDisplay(); // for shutdown

    inline  SalXLib*        GetLib() const { return pXLib_; }

    void                    StartTimer( sal_uLong nMS );
    inline  void            StopTimer();
    void                    Timeout() const;

    // X errors
    virtual void            ErrorTrapPush();
    virtual bool            ErrorTrapPop( bool bIgnoreError );
    void                    XError( Display *pDisp, XErrorEvent *pEvent );
    bool                    HasXErrorOccurred() const
                                { return m_aXErrorHandlerStack.back().m_bWas; }
    unsigned int            GetLastXErrorRequestCode() const
                                { return m_aXErrorHandlerStack.back().m_nLastErrorRequest; }
    void                    ResetXErrorOccurred()
                                { m_aXErrorHandlerStack.back().m_bWas = false; }
    void                    PushXErrorLevel( bool bIgnore );
    void                    PopXErrorLevel();
};

X11SalData* GetX11SalData();

#endif // _SV_SALDATA_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
