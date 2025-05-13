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

#ifndef INCLUDED_DP_RESOURCE_H
#define INCLUDED_DP_RESOURCE_H

#include "tools/string.hxx"
#include "tools/resid.hxx"
#include "com/sun/star/lang/Locale.hpp"
#include "dp_misc.h"
#include <memory>
#include "dp_misc_api.hxx"

namespace dp_misc {

//==============================================================================
ResId getResId( sal_uInt16 id );

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC String getResourceString( sal_uInt16 id );

template <typename Unique, sal_uInt16 id>
struct StaticResourceString :
        public ::rtl::StaticWithInit< ::rtl::OUString, Unique > {
    const ::rtl::OUString operator () () { return getResourceString(id); }
};

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::com::sun::star::lang::Locale toLocale( ::rtl::OUString const & slang );

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::com::sun::star::lang::Locale getOfficeLocale();

//==============================================================================
DESKTOP_DEPLOYMENTMISC_DLLPUBLIC
::rtl::OUString getOfficeLocaleString();

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
