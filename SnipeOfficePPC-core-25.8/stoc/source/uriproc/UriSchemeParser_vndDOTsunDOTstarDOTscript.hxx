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
