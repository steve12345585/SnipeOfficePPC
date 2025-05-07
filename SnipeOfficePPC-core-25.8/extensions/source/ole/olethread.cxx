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

#if defined _MSC_VER && ( _MSC_VER >= 1200 )
#define _WIN32_WINNT  0x0400
#endif

#include "ole2uno.hxx"

#include <tools/presys.h>
#include <tools/postsys.h>

#include <osl/thread.hxx>

using namespace std;

namespace ole_adapter
{
// CoInitializeEx *
typedef DECLSPEC_IMPORT HRESULT (STDAPICALLTYPE *ptrCoInitEx)( LPVOID, DWORD);
// CoInitialize *
typedef DECLSPEC_IMPORT HRESULT (STDAPICALLTYPE *ptrCoInit)( LPVOID);

void o2u_attachCurrentThread()
{
    static osl::ThreadData oleThreadData;

    if ((sal_Bool)(sal_IntPtr)oleThreadData.getData() != sal_True)
    {
        HINSTANCE inst= LoadLibrary( _T("ole32.dll"));
        if( inst )
        {
            HRESULT hr;
            // Try DCOM
            ptrCoInitEx initFuncEx= (ptrCoInitEx)GetProcAddress( inst, _T("CoInitializeEx"));
            if( initFuncEx)
                hr= initFuncEx( NULL, COINIT_MULTITHREADED);
            // No DCOM, try COM
            else
            {
                ptrCoInit initFunc= (ptrCoInit)GetProcAddress( inst,_T("CoInitialize"));
                if( initFunc)
                    hr= initFunc( NULL);
            }
        }
        oleThreadData.setData((void*)sal_True);
    }
}

} // end namespace

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
