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

#ifndef \
    INCLUDED_stoc_source_uriproc_UriSchemeParser_vndDOTsunDOTstarDOTscript_hxx
#define \
    INCLUDED_stoc_source_uriproc_UriSchemeParser_vndDOTsunDOTstarDOTscript_hxx

#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Sequence.hxx"
#include "sal/types.h"

namespace com { namespace sun { namespace star { namespace uno {
    class XComponentContext;
    class XInterface;
} } } }
namespace rtl { class OUString; }

namespace stoc { namespace uriproc {

namespace UriSchemeParser_vndDOTsunDOTstarDOTscript {
    com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SAL_CALL
    create(
        com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const &)
        SAL_THROW((com::sun::star::uno::Exception));

    rtl::OUString SAL_CALL getImplementationName();

    com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL
    getSupportedServiceNames();
}

} }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
