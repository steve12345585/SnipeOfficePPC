/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
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

#include "osl/module.h"
#include "rtl/string.h"
#include "rtl/ustrbuf.hxx"
#include "rtl/ustring.hxx"

#include "loadmodule.hxx"

namespace cppu { namespace detail {

#ifndef DISABLE_DYNLOADING

::oslModule loadModule(rtl::OUString const & name) {
    rtl::OUStringBuffer b;
#if defined SAL_DLLPREFIX
    b.appendAscii(RTL_CONSTASCII_STRINGPARAM(SAL_DLLPREFIX));
#endif
    b.append(name);
    b.appendAscii(RTL_CONSTASCII_STRINGPARAM(SAL_DLLEXTENSION));
    return ::osl_loadModuleRelative(
        reinterpret_cast< oslGenericFunction >(&loadModule),
        b.makeStringAndClear().pData,
        SAL_LOADMODULE_GLOBAL | SAL_LOADMODULE_LAZY);
}

#endif

} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
