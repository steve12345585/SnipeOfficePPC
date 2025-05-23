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

#ifndef INCLUDED_STOC_SOURCE_SIMPLEREGISTRY_TEXTUALSERVICES_HXX
#define INCLUDED_STOC_SOURCE_SIMPLEREGISTRY_TEXTUALSERVICES_HXX

#include "sal/config.h"

#include <memory>

#include "boost/noncopyable.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "rtl/ref.hxx"
#include "rtl/ustring.hxx"

namespace com { namespace sun { namespace star { namespace registry {
    class XRegistryKey;
} } } }

namespace stoc { namespace simpleregistry {

class Data;

class TextualServices: private boost::noncopyable {
public:
    explicit TextualServices(rtl::OUString const & uri);
        // throws com::sun::star::registry::InvalidRegistryException

    virtual ~TextualServices();

    inline rtl::OUString getUri() { return uri_; }

    com::sun::star::uno::Reference< com::sun::star::registry::XRegistryKey >
    getRootKey();

private:
    rtl::OUString uri_;
    rtl::Reference< Data > data_;
};

} }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
