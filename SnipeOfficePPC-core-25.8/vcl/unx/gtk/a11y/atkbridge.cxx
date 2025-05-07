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


#include <unx/gtk/atkbridge.hxx>
#include <unx/gtk/gtkframe.hxx>

#include "atkfactory.hxx"
#include "atkutil.hxx"
#include "atkwindow.hxx"
#include <stdio.h>

bool InitAtkBridge(void)
{
#if !GTK_CHECK_VERSION(3,0,0)
    const char* pVersion = atk_get_toolkit_version();
    if( ! pVersion )
        return false;

    unsigned int major, minor, micro;

    /* check gail minimum version requirements */
    if( sscanf( pVersion, "%u.%u.%u", &major, &minor, &micro) < 3 )
    {
        // g_warning( "unable to parse gail version number" );
        return false;
    }

    if( ( (major << 16) | (minor << 8) | micro ) < ( (1 << 16) | 8 << 8 | 6 ) )
    {
        g_warning( "libgail >= 1.8.6 required for accessibility support" );
        return false;
    }

    /* Initialize the AtkUtilityWrapper class */
    g_type_class_unref( g_type_class_ref( OOO_TYPE_ATK_UTIL ) );

    /* Initialize the GailWindow wrapper class */
    g_type_class_unref( g_type_class_ref( OOO_TYPE_WINDOW_WRAPPER ) );

    /* Register AtkObject wrapper factory */
    AtkRegistry * registry = atk_get_default_registry();
    if( registry )
        atk_registry_set_factory_type( registry, OOO_TYPE_FIXED, OOO_TYPE_WRAPPER_FACTORY );
#endif

    return true;
}

void DeInitAtkBridge()
{
#if !GTK_CHECK_VERSION(3,0,0)
    restore_gail_window_vtable();
#endif
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
