/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the Original Code is
 *       Fridrich Strba  <fridrich.strba@bluewin.ch>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#pragma warning(disable:4273)       // inconsistent dll linkage
#endif

EXTERN_C DWORD WINAPI GetShortPathNameW(LPCWSTR lpszLongPath,LPWSTR lpszShortPath,DWORD cchBuffer)
{
    typedef DWORD (WINAPI * GetShortPathNameW_t) (LPCWSTR,LPWSTR,DWORD);

    GetShortPathNameW_t p_GetShortPathNameW =
        (GetShortPathNameW_t) GetProcAddress (
            GetModuleHandle ("kernel32.dll"), "GetShortPathNameW");

    if (p_GetShortPathNameW)
        return p_GetShortPathNameW(lpszLongPath,lpszShortPath,cchBuffer);
    else
        return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
