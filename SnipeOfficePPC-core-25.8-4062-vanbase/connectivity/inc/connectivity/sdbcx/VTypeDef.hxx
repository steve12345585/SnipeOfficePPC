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

#ifndef _CONNECTIVITY_SDBCX_TYPEDEF_HXX_
#define _CONNECTIVITY_SDBCX_TYPEDEF_HXX_

#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#include <cppuhelper/compbase3.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/container/XNamed.hpp>

namespace connectivity
{
    namespace sdbcx
    {
        typedef ::cppu::WeakComponentImplHelper3< ::com::sun::star::sdbcx::XColumnsSupplier,
                                                  ::com::sun::star::container::XNamed,
                                                  ::com::sun::star::lang::XServiceInfo> ODescriptor_BASE;
    }
}

#endif // _CONNECTIVITY_SDBCX_TYPEDEF_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
