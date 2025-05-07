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
#ifndef _VCL_SM_HXX
#define _VCL_SM_HXX

#include <rtl/string.hxx>
#include <rtl/ustring.hxx>
#include "tools/link.hxx"
#include "unx/salunx.h"
#include <X11/SM/SMlib.h>
#include "salsession.hxx"
#include "vclpluginapi.h"

class SessionManagerClient
{
    static SmcConn          aSmcConnection;
    static rtl::OString m_aClientID;
    static bool         bDocSaveDone;

    static void SaveYourselfProc(       SmcConn connection,
                                        SmPointer client_data,
                                        int save_type,
                                        Bool shutdown,
                                        int interact_style,
                                        Bool fast );
    static void DieProc(                SmcConn connection,
                                        SmPointer client_data );
    static void SaveCompleteProc(       SmcConn connection,
                                        SmPointer client_data );
    static void ShutdownCanceledProc(   SmcConn connection,
                                        SmPointer client_data );
    static void InteractProc(           SmcConn connection,
                                        SmPointer clientData );

    static const rtl::OString& getPreviousSessionID();

    DECL_STATIC_LINK( SessionManagerClient, ShutDownHdl, void* );
    DECL_STATIC_LINK( SessionManagerClient, ShutDownCancelHdl, void* );
    DECL_STATIC_LINK( SessionManagerClient, SaveYourselfHdl, void* );
    DECL_STATIC_LINK( SessionManagerClient, InteractionHdl, void* );
public:
    static VCLPLUG_GEN_PUBLIC void open(); // needed by other plugins, so export
    static void close();

    static bool checkDocumentsSaved();
    static bool queryInteraction();
    static void saveDone();
    static void interactionDone( bool bCancelShutdown );

    static rtl::OUString getExecName();
    static VCLPLUG_GEN_PUBLIC const rtl::OString& getSessionID();
};

class SalFrame;

class IceSalSession : public SalSession
{
public:
    IceSalSession();
    virtual ~IceSalSession();

    virtual void queryInteraction();
    virtual void interactionDone();
    virtual void saveDone();
    virtual bool cancelShutdown();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
