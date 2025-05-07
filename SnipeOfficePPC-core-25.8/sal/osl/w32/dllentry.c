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

#ifdef _MSC_VER
#pragma warning(push,1) /* disable warnings within system headers */
#endif
#include <windows.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <tlhelp32.h>
#include <systools/win32/uwinapi.h>
#include <winsock.h>
#include <osl/diagnose.h>
#include <sal/types.h>
#include <float.h>

#include <osl/diagnose.h>
#include <osl/mutex.h>
#include <sal/types.h>

#include "internal/rtllifecycle.h"

//------------------------------------------------------------------------------
// externals
//------------------------------------------------------------------------------

extern DWORD            g_dwTLSTextEncodingIndex;
extern void SAL_CALL    _osl_callThreadKeyCallbackOnThreadDetach(void);
extern CRITICAL_SECTION g_ThreadKeyListCS;
extern oslMutex         g_Mutex;
extern oslMutex         g_CurrentDirectoryMutex;

#ifdef __MINGW32__

typedef void (*func_ptr) (void);
extern func_ptr __CTOR_LIST__[];
extern func_ptr __DTOR_LIST__[];

static void do_startup(void);
static void do_cleanup(void);

#else

/*
This is needed because DllMain is called after static constructors. A DLL's
startup and shutdown sequence looks like this:

_pRawDllMain()
_CRT_INIT()
DllMain()
....
DllMain()
_CRT_INIT()
_pRawDllMain()

*/

static BOOL WINAPI _RawDllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved );
extern BOOL (WINAPI *_pRawDllMain)(HANDLE, DWORD, LPVOID) = _RawDllMain;

#endif

//------------------------------------------------------------------------------
// DllMain
//------------------------------------------------------------------------------
int osl_isSingleCPU = 0;

#ifdef __MINGW32__

void
__do_global_dtors (void)
{
  static func_ptr *p = __DTOR_LIST__ + 1;

  /*
   * Call each destructor in the destructor list until a null pointer
   * is encountered.
   */
  while (*p)
    {
      (*(p)) ();
      p++;
    }
}

void
__do_global_ctors (void)
{
  unsigned long nptrs = (unsigned long) __CTOR_LIST__[0];
  unsigned i;

  /*
   * If the first entry in the constructor list is -1 then the list
   * is terminated with a null entry. Otherwise the first entry was
   * the number of pointers in the list.
   */
  if (nptrs == (unsigned long)-1)
    {
      for (nptrs = 0; __CTOR_LIST__[nptrs + 1] != 0; nptrs++)
    ;
    }

  /*
   * Go through the list backwards calling constructors.
   */
  for (i = nptrs; i >= 1; i--)
    {
      __CTOR_LIST__[i] ();
    }

  /*
   * Register the destructors for processing on exit.
   */
  atexit (__do_global_dtors);
}

static int initialized = 0;

void
__main (void)
{
  if (!initialized)
    {
      initialized = 1;
      do_startup();
      __do_global_ctors ();
    }
}

static void do_startup( void )
{
#else
static BOOL WINAPI _RawDllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
    (void)hinstDLL; /* avoid warnings */
    (void)lpvReserved; /* avoid warnings */

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            {
#endif

                SYSTEM_INFO SystemInfo;

                GetSystemInfo(&SystemInfo);

                /* Determine if we are on a multiprocessor/multicore/HT x86/x64 system
                 *
                 * The lock prefix for atomic operations in osl_[inc|de]crementInterlockedCount()
                 * comes with a cost and is especially expensive on pre HT x86 single processor
                 * systems, where it isn't needed at all.
                 */
                if ( SystemInfo.dwNumberOfProcessors == 1 ) {
                    osl_isSingleCPU = 1;
                }

#if OSL_DEBUG_LEVEL < 2
                /* Suppress file error messages from system like "Floppy A: not inserted" */
                SetErrorMode( SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS );
#endif

                /* initialize global mutex */
                g_Mutex = osl_createMutex();

                /* initialize "current directory" mutex */
                g_CurrentDirectoryMutex = osl_createMutex();

                g_dwTLSTextEncodingIndex = TlsAlloc();
                InitializeCriticalSection( &g_ThreadKeyListCS );

                //We disable floating point exceptions. This is the usual state at program startup
                //but on Windows 98 and ME this is not always the case.
                _control87(_MCW_EM, _MCW_EM);
#ifdef __MINGW32__
        atexit(do_cleanup);
}

void do_cleanup( void )
{
#else
                break;
            }

        case DLL_PROCESS_DETACH:
#endif

            WSACleanup( );

            TlsFree( g_dwTLSTextEncodingIndex );
            DeleteCriticalSection( &g_ThreadKeyListCS );

            osl_destroyMutex( g_Mutex );

            osl_destroyMutex( g_CurrentDirectoryMutex );

#ifndef __MINGW32__

            /*

            On a product build memory management finalization might
            cause a crash without assertion (assertions off) if heap is
            corrupted. But a crash report won't help here because at
            this point all other threads have been terminated and only
            ntdll is on the stack. No chance to find the reason for the
            corrupted heap if so.

            So annoying the user with a crash report is completly useless.

            */

#ifndef DBG_UTIL
            __try
#endif
            {
                /* cleanup locale hashtable */
                rtl_locale_fini();

                /* finalize memory management */
                rtl_memory_fini();
                rtl_cache_fini();
                rtl_arena_fini();
            }
#ifndef DBG_UTIL
            __except( EXCEPTION_EXECUTE_HANDLER )
            {
            }
#endif
            break;
    }

    return TRUE;
#endif
}

static DWORD GetParentProcessId()
{
    DWORD   dwParentProcessId = 0;
    HANDLE  hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

    if ( IsValidHandle( hSnapshot ) )
    {
        PROCESSENTRY32  pe;
        BOOL            fSuccess;

        ZeroMemory( &pe, sizeof(pe) );
        pe.dwSize = sizeof(pe);
        fSuccess = Process32First( hSnapshot, &pe );

        while( fSuccess )
        {
            if ( GetCurrentProcessId() == pe.th32ProcessID )
            {
                dwParentProcessId = pe.th32ParentProcessID;
                break;
            }

            fSuccess = Process32Next( hSnapshot, &pe );
        }

        CloseHandle( hSnapshot );
    }

    return dwParentProcessId;
}

static DWORD WINAPI ParentMonitorThreadProc( LPVOID lpParam )
{
    DWORD   dwParentProcessId = (DWORD)lpParam;

    HANDLE  hParentProcess = OpenProcess( SYNCHRONIZE, FALSE, dwParentProcessId );
    if ( IsValidHandle( hParentProcess ) )
    {
        if ( WAIT_OBJECT_0 == WaitForSingleObject( hParentProcess, INFINITE ) )
        {
            TerminateProcess( GetCurrentProcess(), 0 );
        }
        CloseHandle( hParentProcess );
    }
    return 0;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
    (void)hinstDLL; /* avoid warning */
    (void)lpvReserved; /* avoid warning */
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            TCHAR   szBuffer[64];

            // This code will attach the process to it's parent process
            // if the parent process had set the environment variable.
            // The corresponding code (setting the environment variable)
            // is is desktop/win32/source/officeloader.cxx


            DWORD   dwResult = GetEnvironmentVariable( "ATTACHED_PARENT_PROCESSID", szBuffer, sizeof(szBuffer) );

            if ( dwResult && dwResult < sizeof(szBuffer) )
            {
                DWORD   dwThreadId = 0;

                DWORD   dwParentProcessId = (DWORD)atol( szBuffer );

                if ( dwParentProcessId && GetParentProcessId() == dwParentProcessId )
                {
                    // No error check, it works or it does not
                    // Thread should only be started for headless mode, see desktop/win32/source/officeloader.cxx
                    CreateThread( NULL, 0, ParentMonitorThreadProc, (LPVOID)dwParentProcessId, 0, &dwThreadId ); //
                }
            }

            return TRUE;
        }

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            _osl_callThreadKeyCallbackOnThreadDetach( );
            break;
    }

    return TRUE;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
