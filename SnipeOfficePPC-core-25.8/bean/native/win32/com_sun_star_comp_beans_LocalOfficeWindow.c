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

#if defined _MSC_VER
#pragma warning(push, 1)
#endif
#include <windows.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif

#include <windows.h>
#define JAWT_GetAWT hidden_JAWT_GetAWT
#include "jawt.h"
#undef JAWT_GetAWT

#if defined _MSC_VER
#pragma warning(push, 1)
#endif
/* When cross-compiling to Windows we don't have any Windows JDK
 * available. Copying this short snippet from win32/jawt_md.h can
 * surely not be against its license. The intent is to enable
 * interoperation with real Oracle Java after all. We leave out the
 * informative comments that might have "artistic merit" and be more
 * copyrightable. Use this also for native Windows compilation for
 * simplicity.
 */
typedef struct jawt_Win32DrawingSurfaceInfo {
    union {
        HWND hwnd;
        HBITMAP hbitmap;
        void* pbits;
    };
    HDC hdc;
    HPALETTE hpalette;
} JAWT_Win32DrawingSurfaceInfo;

extern __declspec(dllimport) unsigned char __stdcall JAWT_GetAWT(JNIEnv *, JAWT *);
#if defined _MSC_VER
#pragma warning(pop)
#endif

#define SYSTEM_WIN32   1

#define OLD_PROC_KEY "oldwindowproc"

static LRESULT APIENTRY OpenOfficeWndProc( HWND , UINT , WPARAM , LPARAM );

/* type must be something like java/lang/RuntimeException
 */
static void ThrowException(JNIEnv * env, char const * type, char const * msg) {
    jclass c;
    (*env)->ExceptionClear(env);
    c = (*env)->FindClass(env, type);
    if (c == NULL) {
        (*env)->ExceptionClear(env);
        (*env)->FatalError(
            env, "JNI FindClass failed");
    }
    if ((*env)->ThrowNew(env, c, msg) != 0) {
        (*env)->ExceptionClear(env);
        (*env)->FatalError(env, "JNI ThrowNew failed");
    }
}


/*****************************************************************************/
/*
 * Class:     com_sun_star_comp_beans_LocalOfficeWindow
 * Method:    getNativeWindowSystemType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_sun_star_comp_beans_LocalOfficeWindow_getNativeWindowSystemType
  (JNIEnv * env, jobject obj_this)
{
    (void) env; // unused
    (void) obj_this; // unused
    return (SYSTEM_WIN32);
}


/*****************************************************************************/
/*
 * Class:     com_sun_star_comp_beans_LocalOfficeWindow
 * Method:    getNativeWindow
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_sun_star_comp_beans_LocalOfficeWindow_getNativeWindow
  (JNIEnv * env, jobject obj_this)
{
    jboolean result;
    jint lock;

    JAWT awt;
    JAWT_DrawingSurface* ds;
    JAWT_DrawingSurfaceInfo* dsi;
    JAWT_Win32DrawingSurfaceInfo* dsi_win;
    HWND hWnd;
    LONG hFuncPtr;

    /* Get the AWT */
    awt.version = JAWT_VERSION_1_3;
    result = JAWT_GetAWT(env, &awt);
    if (result == JNI_FALSE)
        ThrowException(env, "java/lang/RuntimeException", "JAWT_GetAWT failed");

                                /* Get the drawing surface */
    if ((ds = awt.GetDrawingSurface(env, obj_this)) == NULL)
        return 0L;

    /* Lock the drawing surface */
    lock = ds->Lock(ds);
    if ( (lock & JAWT_LOCK_ERROR) != 0)
        ThrowException(env, "java/lang/RuntimeException",
                       "Could not get AWT drawing surface.");

    /* Get the drawing surface info */
    dsi = ds->GetDrawingSurfaceInfo(ds);

    /* Get the platform-specific drawing info */
    dsi_win = (JAWT_Win32DrawingSurfaceInfo*)dsi->platformInfo;

    hWnd = dsi_win->hwnd;

    /* Free the drawing surface info */
    ds->FreeDrawingSurfaceInfo(dsi);
    /* Unlock the drawing surface */
    ds->Unlock(ds);
    /* Free the drawing surface */
    awt.FreeDrawingSurface(ds);

    /* Register own window procedure
       Do it one times only! Otherwhise
       multiple instances will be registered
       and calls on such construct produce
       a stack overflow.
     */
    if (GetProp( hWnd, OLD_PROC_KEY )==0)
    {
        hFuncPtr = SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)OpenOfficeWndProc );
        SetProp( hWnd, OLD_PROC_KEY, (HANDLE)hFuncPtr );
    }

    return ((jlong)(LONG)hWnd);
}


static LRESULT APIENTRY OpenOfficeWndProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_PARENTNOTIFY: {
            if (wParam == WM_CREATE) {
                RECT rect;
                HWND hChild = (HWND) lParam;

                GetClientRect(hWnd, &rect);

                SetWindowPos(hChild,
                             NULL,
                             rect.left,
                             rect.top,
                             rect.right - rect.left,
                             rect.bottom - rect.top,
                             SWP_NOZORDER);
            }
            break;
        }
        case WM_SIZE: {
            WORD newHeight = HIWORD(lParam);
            WORD newWidth = LOWORD(lParam);
            HWND hChild = GetWindow(hWnd, GW_CHILD);

            if (hChild != NULL) {
                SetWindowPos(hChild, NULL, 0, 0, newWidth, newHeight, SWP_NOZORDER);
            }
            break;
        }
    }

#if defined _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4152) /* function/data pointer conversion: */
#endif
    return CallWindowProc(GetProp(hWnd, OLD_PROC_KEY),
                          hWnd, uMsg, wParam, lParam);
#if defined _MSC_VER
#pragma warning(pop)
#endif
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
