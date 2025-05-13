/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#include "sal/config.h"

#include <cstddef>
#include <new>
#include <string.h> // <cstring> not supported by old MSC versions

#define WIN32_LEAN_AND_MEAN
#if defined _MSC_VER
#pragma warning(push, 1)
#endif
#include <windows.h>
#include <msiquery.h>
#include <shellapi.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif

#include "boost/scoped_array.hpp"

#define LCL_LENGTH0(s) (sizeof (s) / sizeof *(s))
#define LCL_STRING0(s) (s), LCL_LENGTH0(s)

namespace {

enum Status { STATUS_NO, STATUS_YES, STATUS_ERROR };

Status fileExists(wchar_t const * path) {
    return GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES
        ? GetLastError() == ERROR_FILE_NOT_FOUND ? STATUS_NO : STATUS_ERROR
        : STATUS_YES;
}

wchar_t * getProperty(
    MSIHANDLE install, wchar_t const * name, wchar_t const * suffix,
    std::size_t suffixLength, wchar_t ** end = NULL)
{
    DWORD n = 0;
    UINT err = MsiGetPropertyW(install, name, L"", &n);
    if (err != ERROR_SUCCESS && err != ERROR_MORE_DATA) {
        return NULL;
    }
    DWORD n2 = n + suffixLength; //TODO: overflow
    wchar_t * data = new(std::nothrow) wchar_t[n2];
    if (data == NULL) {
        return NULL;
    }
    if (MsiGetPropertyW(install, name, data, &n2) != ERROR_SUCCESS || n2 != n) {
        delete[] data;
        return NULL;
    }
    memcpy(data + n, suffix, suffixLength * sizeof (wchar_t)); //TODO: overflow
    if (end != NULL) {
        *end = data + n + suffixLength;
    }
    return data;
}

}

extern "C" UINT __stdcall copyEditionData(MSIHANDLE install) {
    boost::scoped_array<wchar_t> from(
        getProperty(install, L"SourceDir", LCL_STRING0(L"edition\0")));
    if (!from) {
        return ERROR_INSTALL_FAILURE;
    }
    Status stat = fileExists(from.get());
    if (stat == STATUS_ERROR) {
        return ERROR_INSTALL_FAILURE;
    }
    if (stat == STATUS_NO) {
        return ERROR_SUCCESS;
    }
    wchar_t * end;
    boost::scoped_array<wchar_t> to(
        getProperty(
            install, L"INSTALLLOCATION",
            LCL_STRING0(L"program\\edition\0"), &end));
    if (!to) {
        return ERROR_INSTALL_FAILURE;
    }
    stat = fileExists(to.get());
    if (stat == STATUS_ERROR) {
        return ERROR_INSTALL_FAILURE;
    }
    if (stat == STATUS_YES) {
        SHFILEOPSTRUCTW opDelete = {
            NULL, FO_DELETE, to.get(), NULL, FOF_NOCONFIRMATION | FOF_SILENT,
            FALSE, NULL, NULL }; //TODO: non-NULL hwnd
        if (SHFileOperationW(&opDelete) != 0) {
            return ERROR_INSTALL_FAILURE;
        }
    }
    *(end - LCL_LENGTH0(L"\\edition\0")) = L'\0';
    *(end - LCL_LENGTH0(L"\\edition\0") + 1) = L'\0';
    SHFILEOPSTRUCTW opCopy = {
        NULL, FO_COPY, from.get(), to.get(),
        FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT, FALSE, NULL,
        NULL }; //TODO: non-NULL hwnd
    if (SHFileOperationW(&opCopy) != 0) {
        return ERROR_INSTALL_FAILURE;
    }
    return ERROR_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
