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

#ifndef _SAL_I18N_INPUTMETHOD_HXX
#define _SAL_I18N_INPUTMETHOD_HXX

#include <vclpluginapi.h>

extern "C" char* GetMethodName( XIMStyle nStyle, char *pBuf, int nBufSize);

#define bUseInputMethodDefault True

class VCLPLUG_GEN_PUBLIC SalI18N_InputMethod
{
    Bool        mbUseable;  // system supports locale as well as status
                            // and preedit style ?
    XIM         maMethod;
    XIMCallback maDestroyCallback;
    XIMStyles  *mpStyles;

public:

    Bool        PosixLocale();
    Bool        UseMethod()             { return mbUseable; }
    XIM         GetMethod()             { return maMethod;  }
    void        HandleDestroyIM();
    Bool        CreateMethod( Display *pDisplay );
    XIMStyles  *GetSupportedStyles()    { return mpStyles;  }
    Bool        SetLocale( const char* pLocale = "" );
    Bool        FilterEvent( XEvent *pEvent, XLIB_Window window );

    SalI18N_InputMethod();
    ~SalI18N_InputMethod();
};

#endif // _SAL_I18N_INPUTMETHOD_HXX


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
