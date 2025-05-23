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

#ifndef INCLUDED_PDFI_WRAPPER_HXX
#define INCLUDED_PDFI_WRAPPER_HXX

#include "contentsink.hxx"
#include <com/sun/star/uno/Reference.hxx>

namespace rtl { class OUString; }
namespace com { namespace sun { namespace star {
    namespace uno {
        class XComponentContext;
    }
    namespace io {
        class XInputStream;
    }
} } }

namespace pdfi
{
    bool xpdf_ImportFromFile( const rtl::OUString&                               rURL,
                              const ContentSinkSharedPtr&                        rSink,
                              const com::sun::star::uno::Reference<
                                    com::sun::star::task::XInteractionHandler >& xIHdl,
                              const rtl::OUString&                               rPwd,
                              const com::sun::star::uno::Reference<
                                    com::sun::star::uno::XComponentContext >&    xContext );
    bool xpdf_ImportFromStream( const com::sun::star::uno::Reference<
                                      com::sun::star::io::XInputStream >&          xInput,
                                const ContentSinkSharedPtr&                        rSink,
                                const com::sun::star::uno::Reference<
                                      com::sun::star::task::XInteractionHandler >& xIHdl,
                                const rtl::OUString&                               rPwd,
                                const com::sun::star::uno::Reference<
                                      com::sun::star::uno::XComponentContext >&    xContext );
}

#endif /* INCLUDED_PDFI_WRAPPER_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
