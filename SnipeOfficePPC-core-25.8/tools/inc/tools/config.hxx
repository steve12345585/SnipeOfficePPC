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
#ifndef _CONFIG_HXX
#define _CONFIG_HXX

#include "tools/toolsdllapi.h"
#include <tools/string.hxx>

struct ImplConfigData;
struct ImplGroupData;

// ----------
// - Config -
// ----------

class TOOLS_DLLPUBLIC Config
{
private:
    rtl::OUString       maFileName;
    rtl::OString        maGroupName;
    ImplConfigData*     mpData;
    ImplGroupData*      mpActGroup;
    sal_uIntPtr         mnDataUpdateId;
    sal_uInt16          mnLockCount;
    sal_Bool            mbPersistence;
    sal_Bool            mbDummy1;

#ifdef _CONFIG_CXX
    TOOLS_DLLPRIVATE sal_Bool               ImplUpdateConfig() const;
    TOOLS_DLLPRIVATE ImplGroupData*     ImplGetGroup() const;
#endif

public:
                        Config( const rtl::OUString& rFileName );
                        ~Config();

    const rtl::OUString& GetPathName() const { return maFileName; }

    void SetGroup(const rtl::OString& rGroup);
    const rtl::OString& GetGroup() const { return maGroupName; }
    void DeleteGroup(const rtl::OString& rGroup);
    rtl::OString GetGroupName(sal_uInt16 nGroup) const;
    sal_uInt16              GetGroupCount() const;
    sal_Bool HasGroup(const rtl::OString& rGroup) const;

    rtl::OString ReadKey(const rtl::OString& rKey) const;
    rtl::OUString ReadKey(const rtl::OString& rKey, rtl_TextEncoding eEncoding) const;
    rtl::OString ReadKey(const rtl::OString& rKey, const rtl::OString& rDefault) const;
    void                WriteKey(const rtl::OString& rKey, const rtl::OString& rValue);
    void DeleteKey(const rtl::OString& rKey);
    rtl::OString GetKeyName(sal_uInt16 nKey) const;
    rtl::OString ReadKey(sal_uInt16 nKey) const;
    sal_uInt16              GetKeyCount() const;

    sal_Bool                IsLocked() const { return (mnLockCount != 0); }
    void                Flush();

    void                EnablePersistence( sal_Bool bPersistence = sal_True )
                            { mbPersistence = bPersistence; }
    sal_Bool                IsPersistenceEnabled() const { return mbPersistence; }

private:
    TOOLS_DLLPRIVATE                Config( const Config& rConfig );
    TOOLS_DLLPRIVATE Config&            operator = ( const Config& rConfig );
};

#endif // _SV_CONFIG_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
