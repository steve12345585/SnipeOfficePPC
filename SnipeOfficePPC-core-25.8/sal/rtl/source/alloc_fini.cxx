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

#include <rtl/instance.hxx>

#include "internal/rtllifecycle.h"

namespace
{
    struct rtlMemorySingleton
    {
        rtlMemorySingleton()
        {
            rtl_memory_init();
        }
        ~rtlMemorySingleton()
        {
            rtl_memory_fini();
        }
    };
    class theMemorySingleton
        : public rtl::Static<rtlMemorySingleton, theMemorySingleton>{};
}

void ensureMemorySingleton()
{
    theMemorySingleton::get();
}

namespace
{
    struct rtlCacheSingleton
    {
        rtlCacheSingleton()
        {
            rtl_cache_init();
        }
        ~rtlCacheSingleton()
        {
            rtl_cache_fini();
        }
    };
    class theCacheSingleton
        : public rtl::Static<rtlCacheSingleton, theCacheSingleton>{};
}

void ensureCacheSingleton()
{
    theCacheSingleton::get();
}

namespace
{
    struct rtlArenaSingleton
    {
        rtlArenaSingleton()
        {
            rtl_arena_init();
        }
        ~rtlArenaSingleton()
        {
            rtl_arena_fini();
        }
    };
    class theArenaSingleton
        : public rtl::Static<rtlArenaSingleton, theArenaSingleton>{};
}

void ensureArenaSingleton()
{
    theArenaSingleton::get();
}

namespace
{
    struct rtlLocaleSingleton
    {
        rtlLocaleSingleton()
        {
            rtl_locale_init();
        }
        ~rtlLocaleSingleton()
        {
            rtl_locale_fini();
        }
    };
    class theLocaleSingleton
        : public rtl::Static<rtlLocaleSingleton, theLocaleSingleton>{};
}

void ensureLocaleSingleton()
{
    theLocaleSingleton::get();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
