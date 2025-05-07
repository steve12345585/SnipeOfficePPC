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

#include "sal/config.h"

#include <cassert>

#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "rtl/ref.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"

#include "data.hxx"
#include "node.hxx"
#include "nodemap.hxx"

namespace configmgr {

namespace {

namespace css = com::sun::star;

}

NodeMap & Node::getMembers() {
    assert(false);
    throw css::uno::RuntimeException(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("this cannot happen")),
        css::uno::Reference< css::uno::XInterface >());
}

rtl::OUString Node::getTemplateName() const {
    return rtl::OUString();
}

void Node::setMandatory(int layer) {
    (void) layer; // avoid warnings
    assert(layer == Data::NO_LAYER);
}

int Node::getMandatory() const {
    return Data::NO_LAYER;
}

void Node::setLayer(int layer) {
    assert(layer >= layer_);
    layer_ = layer;
}

int Node::getLayer() const {
    return layer_;
}

void Node::setFinalized(int layer) {
    finalized_ = layer;
}

int Node::getFinalized() const {
    return finalized_;
}

rtl::Reference< Node > Node::getMember(rtl::OUString const & name) {
    NodeMap const & members = getMembers();
    NodeMap::const_iterator i(members.find(name));
    return i == members.end() ? rtl::Reference< Node >() : i->second;
}

Node::Node(int layer): layer_(layer), finalized_(Data::NO_LAYER) {}

Node::Node(const Node & other):
    SimpleReferenceObject(), layer_(other.layer_), finalized_(other.finalized_)
{}

Node::~Node() {}

void Node::clear() {}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
