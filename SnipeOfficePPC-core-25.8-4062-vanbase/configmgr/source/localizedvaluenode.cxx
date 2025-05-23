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

#include "com/sun/star/uno/Any.hxx"
#include "rtl/ref.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"

#include "localizedvaluenode.hxx"
#include "node.hxx"

namespace configmgr {

LocalizedValueNode::LocalizedValueNode(int layer, css::uno::Any const & value):
    Node(layer), value_(value)
{}

rtl::Reference< Node > LocalizedValueNode::clone(bool) const {
    return new LocalizedValueNode(*this);
}

rtl::OUString LocalizedValueNode::getTemplateName() const {
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("*"));
}

css::uno::Any LocalizedValueNode::getValue() const {
    return value_;
}

void LocalizedValueNode::setValue(int layer, css::uno::Any const & value) {
    setLayer(layer);
    value_ = value;
}

LocalizedValueNode::LocalizedValueNode(LocalizedValueNode const & other):
    Node(other), value_(other.value_)
{}

LocalizedValueNode::~LocalizedValueNode() {}

Node::Kind LocalizedValueNode::kind() const {
    return KIND_LOCALIZED_VALUE;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
