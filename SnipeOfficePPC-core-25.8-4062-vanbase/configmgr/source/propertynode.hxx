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

#ifndef INCLUDED_CONFIGMGR_SOURCE_PROPERTYNODE_HXX
#define INCLUDED_CONFIGMGR_SOURCE_PROPERTYNODE_HXX

#include "sal/config.h"

#include "com/sun/star/uno/Any.hxx"
#include "rtl/ref.hxx"

#include "node.hxx"
#include "type.hxx"

namespace rtl { class OUString; }

namespace configmgr {

class Components;

class PropertyNode: public Node {
public:
    PropertyNode(
        int layer, Type staticType, bool nillable,
        com::sun::star::uno::Any const & value, bool extension);

    virtual rtl::Reference< Node > clone(bool keepTemplateName) const;

    Type getStaticType() const;

    bool isNillable() const;

    com::sun::star::uno::Any getValue(Components & components);

    void setValue(int layer, com::sun::star::uno::Any const & value);

    void setExternal(int layer, rtl::OUString const & descriptor);

    bool isExtension() const;

private:
    PropertyNode(PropertyNode const & other);

    virtual ~PropertyNode();

    virtual Kind kind() const;

    Type staticType_;
        // as specified in the component-schema (TYPE_ANY, ...,
        // TYPE_HEXBINARY_LIST; not TYPE_ERROR or TYPE_NIL)
    bool nillable_;
    bool extension_;
    rtl::OUString externalDescriptor_;
    com::sun::star::uno::Any value_;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
