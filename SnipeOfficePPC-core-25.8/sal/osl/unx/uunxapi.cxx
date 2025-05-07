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


 #ifndef _OSL_UUNXAPI_H_
 #include "uunxapi.h"
 #endif

 #ifndef __OSL_SYSTEM_H__
 #include "system.h"
 #endif

 #ifndef _LIMITS_H
 #include <limits.h>
 #endif

 #ifndef _RTL_USTRING_HXX_
 #include <rtl/ustring.hxx>
 #endif

 #ifndef _OSL_THREAD_H_
 #include <osl/thread.h>
 #endif

 #ifdef ANDROID
 #include <osl/detail/android-bootstrap.h>
 #endif

 //###########################
 inline rtl::OString OUStringToOString(const rtl_uString* s)
 {
    return rtl::OUStringToOString(
        rtl::OUString(const_cast<rtl_uString*>(s)),
        osl_getThreadTextEncoding());
 }

 //###########################
#ifdef MACOSX
/*
 * Helper function for resolving Mac native alias files (not the same as unix alias files)
 * and to return the resolved alias as rtl::OString
 */
 inline rtl::OString macxp_resolveAliasAndConvert(const rtl_uString* s)
 {
  rtl::OString p = OUStringToOString(s);
  sal_Char path[PATH_MAX];
  if (p.getLength() < PATH_MAX)
    {
      strcpy(path, p.getStr());
      macxp_resolveAlias(path, PATH_MAX);
      p = rtl::OString(path);
    }
  return p;
 }
#endif /* MACOSX */

 //###########################
 //access_u
 int access_u(const rtl_uString* pustrPath, int mode)
 {
#ifndef MACOSX // not MACOSX
    rtl::OString fn = OUStringToOString(pustrPath);
#ifdef ANDROID
    if (strncmp(fn.getStr(), "/assets", sizeof("/assets")-1) == 0 &&
        (fn.getStr()[sizeof("/assets")-1] == '\0' ||
         fn.getStr()[sizeof("/assets")-1] == '/'))
    {
        struct stat stat;
        if (lo_apk_lstat(fn.getStr(), &stat) == -1)
            return -1;
        if (mode & W_OK)
        {
            errno = EACCES;
            return -1;
        }
        return 0;
    }
#endif
    return access(fn.getStr(), mode);
#else
    return access(macxp_resolveAliasAndConvert(pustrPath).getStr(), mode);
#endif
 }

 //#########################
 //realpath_u
 sal_Bool realpath_u(const rtl_uString* pustrFileName, rtl_uString** ppustrResolvedName)
 {
#ifndef MACOSX // not MACOSX
    rtl::OString fn = OUStringToOString(pustrFileName);
#ifdef ANDROID
    if (strncmp(fn.getStr(), "/assets", sizeof("/assets")-1) == 0 &&
        (fn.getStr()[sizeof("/assets")-1] == '\0' ||
         fn.getStr()[sizeof("/assets")-1] == '/'))
    {
        if (access_u(pustrFileName, F_OK) == -1)
            return sal_False;

        rtl_uString silly(*pustrFileName);
        rtl_uString_assign(ppustrResolvedName, &silly);

        return sal_True;
    }
#endif
#else
    rtl::OString fn = macxp_resolveAliasAndConvert(pustrFileName);
#endif
    char  rp[PATH_MAX];
    bool  bRet = realpath(fn.getStr(), rp);

    if (bRet)
    {
        rtl::OUString resolved = rtl::OStringToOUString(
            rtl::OString(static_cast<sal_Char*>(rp)),
            osl_getThreadTextEncoding());

        rtl_uString_assign(ppustrResolvedName, resolved.pData);
    }
    return bRet;
 }

 //#########################
 //stat_c
  int stat_c(const char* cpPath, struct stat* buf)
 {
#ifdef ANDROID
    if (strncmp(cpPath, "/assets", sizeof("/assets")-1) == 0 &&
        (cpPath[sizeof("/assets")-1] == '\0' ||
         cpPath[sizeof("/assets")-1] == '/'))
        return lo_apk_lstat(cpPath, buf);
#endif
    return stat(cpPath, buf);
 }

 //#########################
 //lstat_c
  int lstat_c(const char* cpPath, struct stat* buf)
 {
#ifdef ANDROID
    if (strncmp(cpPath, "/assets", sizeof("/assets")-1) == 0 &&
        (cpPath[sizeof("/assets")-1] == '\0' ||
         cpPath[sizeof("/assets")-1] == '/'))
        return lo_apk_lstat(cpPath, buf);
#endif
    return lstat(cpPath, buf);
 }

 //#########################
 //lstat_u
  int lstat_u(const rtl_uString* pustrPath, struct stat* buf)
 {
#ifndef MACOSX  // not MACOSX
    rtl::OString fn = OUStringToOString(pustrPath);
    return lstat_c(fn.getStr(), buf);
#else
    return lstat(macxp_resolveAliasAndConvert(pustrPath).getStr(), buf);
#endif
 }

 //#########################
 // @see mkdir
 int mkdir_u(const rtl_uString* path, mode_t mode)
 {
    return mkdir(OUStringToOString(path).getStr(), mode);
 }


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
