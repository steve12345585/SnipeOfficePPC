/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of SnipeOffice.
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
#ifndef __PLUGIN_INC_UNXPLUG_HXX
#define __PLUGIN_INC_UNXPLUG_HXX

#include <unistd.h>

#include <plugin/unx/plugcon.hxx>
#include <plugin/plcom.hxx>
#include <vcl/sysdata.hxx>

class UnxPluginComm : public PluginComm, public PluginConnector
{
private:
    pid_t       m_nCommPID;
public:
    UnxPluginComm( const String& mimetype,
                   const String& library,
                   XLIB_Window aParent,
                   int nDescriptor1,
                   int nDescriptor2
                   );
    virtual ~UnxPluginComm();

    using PluginComm::NPP_Destroy;
    virtual NPError NPP_Destroy( NPP instance, NPSavedData** save );
    virtual NPError NPP_DestroyStream( NPP instance, NPStream* stream,
                                       NPError reason );
    virtual void* NPP_GetJavaClass();
    virtual NPError NPP_Initialize();
    virtual NPError NPP_New( NPMIMEType pluginType, NPP instance,
                             uint16_t mode, int16_t argc,
                             char* argn[], char* argv[], NPSavedData *saved );
    virtual NPError NPP_NewStream( NPP instance, NPMIMEType type,
                                   NPStream* stream,
                                   NPBool seekable, uint16_t* stype );
    virtual void NPP_Print( NPP instance, NPPrint* platformPrint );

    using PluginComm::NPP_SetWindow;
    virtual NPError NPP_SetWindow( NPP instance, NPWindow* window );
    virtual void NPP_Shutdown();
    virtual void NPP_StreamAsFile( NPP instance, NPStream* stream,
                                   const char* fname );
    virtual void NPP_URLNotify( NPP instance, const char* url, NPReason reason,
                                void* notifyData );
    virtual int32_t NPP_Write( NPP instance, NPStream* stream, int32_t offset,
                               int32_t len, void* buffer );
    virtual int32_t NPP_WriteReady( NPP instance, NPStream* stream );
    virtual char* NPP_GetMIMEDescription();
    virtual NPError NPP_GetValue( NPP instance, NPPVariable variable, void* value );
    virtual NPError NPP_SetValue( NPP instance, NPNVariable variable,
                                 void *value);

    static bool getPluginappPath(rtl::OString * path);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
