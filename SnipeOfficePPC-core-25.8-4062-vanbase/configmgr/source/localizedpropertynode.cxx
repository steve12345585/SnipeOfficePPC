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

#include "localizedpropertynode.hxx"
#include "node.hxx"
#include "nodemap.hxx"
#include "type.hxx"

namespace configmgr {

LocalizedPropertyNode::LocalizedPropertyNode(
    int layer, Type staticType, bool nillable):
    Node(layer), staticType_(staticType), nillable_(nillable)
{}

rtl::Reference< Node > LocalizedPropertyNode::clone(bool) const {
    return new LocalizedPropertyNode(*this);
}

NodeMap & LocalizedPropertyNode::getMembers() {
    return members_;
}

Type LocalizedPropertyNode::getStaticType() const {
    return staticType_;
}

bool LocalizedPropertyNode::isNillable() const {
    return nillable_;
}

LocalizedPropertyNode::LocalizedPropertyNode(
    LocalizedPropertyNode const & other):
    Node(other), staticType_(other.staticType_), nillable_(other.nillable_)
{
    cloneNodeMap(other.members_, &members_);
}

LocalizedPropertyNode::~LocalizedPropertyNode() {}

Node::Kind LocalizedPropertyNode::kind() const {
    return KIND_LOCALIZED_PROPERTY;
}

void LocalizedPropertyNode::clear() {
    members_.clear();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
