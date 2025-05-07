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
#include <vector>

#include "com/sun/star/container/XChild.hpp"
#include "com/sun/star/lang/NoSupportException.hpp"
#include "com/sun/star/lang/XUnoTunnel.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "com/sun/star/uno/XInterface.hpp"
#include "cppu/unotype.hxx"
#include "cppuhelper/queryinterface.hxx"
#include "cppuhelper/weak.hxx"
#include "comphelper/servicehelper.hxx"
#include "osl/mutex.hxx"
#include "rtl/ref.hxx"
#include "rtl/string.h"
#include "rtl/ustrbuf.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"

#include "access.hxx"
#include "childaccess.hxx"
#include "components.hxx"
#include "data.hxx"
#include "groupnode.hxx"
#include "localizedpropertynode.hxx"
#include "localizedvaluenode.hxx"
#include "lock.hxx"
#include "modifications.hxx"
#include "node.hxx"
#include "path.hxx"
#include "propertynode.hxx"
#include "rootaccess.hxx"
#include "setnode.hxx"
#include "type.hxx"

namespace configmgr {

namespace {

namespace css = com::sun::star;

}

namespace
{
    class theChildAccessUnoTunnelId : public rtl::Static< UnoTunnelIdInit, theChildAccessUnoTunnelId > {};
}

css::uno::Sequence< sal_Int8 > ChildAccess::getTunnelId()
{
    return theChildAccessUnoTunnelId::get().getSeq();
}

ChildAccess::ChildAccess(
    Components & components, rtl::Reference< RootAccess > const & root,
    rtl::Reference< Access > const & parent, rtl::OUString const & name,
    rtl::Reference< Node > const & node):
    Access(components), root_(root), parent_(parent), name_(name), node_(node),
    inTransaction_(false)
{
    lock_ = lock();
    assert(root.is() && parent.is() && node.is());
}

ChildAccess::ChildAccess(
    Components & components, rtl::Reference< RootAccess > const & root,
    rtl::Reference< Node > const & node):
    Access(components), root_(root), node_(node), inTransaction_(false)
{
    lock_ = lock();
    assert(root.is() && node.is());
}

Path ChildAccess::getAbsolutePath() {
    assert(getParentAccess().is());
    Path path(getParentAccess()->getAbsolutePath());
    path.push_back(name_);
    return path;
}

Path ChildAccess::getRelativePath() {
    Path path;
    rtl::Reference< Access > parent(getParentAccess());
    if (parent.is()) {
        path = parent->getRelativePath();
    }
    path.push_back(name_);
    return path;
}

rtl::OUString ChildAccess::getRelativePathRepresentation() {
    rtl::OUStringBuffer path;
    rtl::Reference< Access > parent(getParentAccess());
    if (parent.is()) {
        path.append(parent->getRelativePathRepresentation());
        if (path.getLength() != 0) {
            path.append(sal_Unicode('/'));
        }
    }
    path.append(Data::createSegment(node_->getTemplateName(), name_));
    return path.makeStringAndClear();
}

rtl::Reference< Node > ChildAccess::getNode() {
    return node_;
}

bool ChildAccess::isFinalized() {
    return node_->getFinalized() != Data::NO_LAYER ||
        (parent_.is() && parent_->isFinalized());
}

rtl::OUString ChildAccess::getNameInternal() {
    return name_;
}

rtl::Reference< RootAccess > ChildAccess::getRootAccess() {
    return root_;
}

rtl::Reference< Access > ChildAccess::getParentAccess() {
    return parent_;
}

void ChildAccess::acquire() throw () {
    Access::acquire();
}

void ChildAccess::release() throw () {
    Access::release();
}

css::uno::Reference< css::uno::XInterface > ChildAccess::getParent()
    throw (css::uno::RuntimeException)
{
    assert(thisIs(IS_ANY));
    osl::MutexGuard g(*lock_);
    checkLocalizedPropertyAccess();
    return static_cast< cppu::OWeakObject * >(parent_.get());
}

void ChildAccess::setParent(css::uno::Reference< css::uno::XInterface > const &)
    throw (css::lang::NoSupportException, css::uno::RuntimeException)
{
    assert(thisIs(IS_ANY));
    osl::MutexGuard g(*lock_);
    checkLocalizedPropertyAccess();
    throw css::lang::NoSupportException(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("setParent")),
        static_cast< cppu::OWeakObject * >(this));
}

sal_Int64 ChildAccess::getSomething(
    css::uno::Sequence< sal_Int8 > const & aIdentifier)
    throw (css::uno::RuntimeException)
{
    assert(thisIs(IS_ANY));
    osl::MutexGuard g(*lock_);
    checkLocalizedPropertyAccess();
    return aIdentifier == getTunnelId()
        ? reinterpret_cast< sal_Int64 >(this) : 0;
}

void ChildAccess::bind(
    rtl::Reference< RootAccess > const & root,
    rtl::Reference< Access > const & parent, rtl::OUString const & name)
    throw ()
{
    assert(!parent_.is() && root.is() && parent.is() && !name.isEmpty());
    root_ = root;
    parent_ = parent;
    name_ = name;
}

void ChildAccess::unbind() throw () {
    assert(parent_.is());
    parent_->releaseChild(name_);
    parent_.clear();
    inTransaction_ = true;
}

void ChildAccess::committed() {
    inTransaction_ = false;
}

void ChildAccess::setNode(rtl::Reference< Node > const & node) {
    node_ = node;
}

void ChildAccess::setProperty(
    css::uno::Any const & value, Modifications * localModifications)
{
    assert(localModifications != 0);
    Type type = TYPE_ERROR;
    bool nillable = false;
    switch (node_->kind()) {
    case Node::KIND_PROPERTY:
        {
            PropertyNode * prop = dynamic_cast< PropertyNode * >(node_.get());
            type = prop->getStaticType();
            nillable = prop->isNillable();
        }
        break;
    case Node::KIND_LOCALIZED_PROPERTY:
        {
            rtl::OUString locale(getRootAccess()->getLocale());
            if (!Components::allLocales(locale)) {
                rtl::Reference< ChildAccess > child(getChild(locale));
                if (child.is()) {
                    child->setProperty(value, localModifications);
                } else {
                    insertLocalizedValueChild(
                        locale, value, localModifications);
                }
                return;
            }
        }
        break;
    case Node::KIND_LOCALIZED_VALUE:
        {
            LocalizedPropertyNode * locprop =
                dynamic_cast< LocalizedPropertyNode * >(getParentNode().get());
            type = locprop->getStaticType();
            nillable = locprop->isNillable();
        }
        break;
    default:
        break;
    }
    checkValue(value, type, nillable);
    getParentAccess()->markChildAsModified(this);
    changedValue_.reset(new css::uno::Any(value));
    localModifications->add(getRelativePath());
}

namespace
{
    rtl::OUString lcl_StripSegment(const rtl::OUString &rLocale)
    {
        sal_Int32 i = !rLocale.isEmpty() ? rLocale.getLength() - 1 : 0;
        while (i > 0 && rLocale[i] != '-' && rLocale[i] != '_')
            --i;
        return rLocale.copy(0, i);
    }
}

css::uno::Any ChildAccess::asValue() {
    if (changedValue_.get() != 0) {
        return *changedValue_;
    }
    switch (node_->kind()) {
    case Node::KIND_PROPERTY:
        return dynamic_cast< PropertyNode * >(node_.get())->getValue(
            getComponents());
    case Node::KIND_LOCALIZED_PROPERTY:
        {
            rtl::OUString sLocale(getRootAccess()->getLocale());
            if (!Components::allLocales(sLocale))
            {
                rtl::Reference< ChildAccess > child;
                // Find best match using an adaption of RFC 4647 lookup matching
                // rules, removing "-" or "_" delimited segments from the end
                while (1)
                {
                    child = getChild(sLocale);
                    if (child.is())
                        break;
                    rtl::OUString sTmpLocale = lcl_StripSegment(sLocale);
                    if (sTmpLocale.isEmpty())
                        break;
                    sLocale = sTmpLocale;
                }

                //Resolves: fdo#33638 Look for the first entry with the same
                //first segment as the requested language tag, before falling
                //back to en-US, etc.
                typedef std::vector< rtl::Reference< ChildAccess > > ChildVector;
                if (!child.is())
                {
                    const ChildVector &rAllChildren = getAllChildren();
                    for (ChildVector::const_iterator aI = rAllChildren.begin(),
                         aEnd = rAllChildren.end(); aI != aEnd; ++aI)
                    {
                        rtl::OUString sLanguage = lcl_StripSegment((*aI)->getNameInternal());
                        if (sLocale == sLanguage)
                        {
                            child = *aI;
                            break;
                        }
                    }
                }

                // defaults are the "en-US" locale, the "en" locale, the empty
                // string locale, the first child (if any), or a nil value (even
                // though it may be illegal for the given property), in that
                // order:
                if (!child.is())
                {
                    child = getChild(
                        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("en-US")));
                    if (!child.is())
                    {
                        child = getChild(
                            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("en")));
                        if (!child.is())
                        {
                            child = getChild(rtl::OUString());
                            if (!child.is())
                            {
                                ChildVector all(getAllChildren());
                                if (!all.empty())
                                    child = all.front();
                            }
                        }
                    }
                }
                return child.is() ? child->asValue() : css::uno::Any();
            }
        }
        break;
    case Node::KIND_LOCALIZED_VALUE:
        return dynamic_cast< LocalizedValueNode * >(node_.get())->getValue();
    default:
        break;
    }
    return css::uno::makeAny(
        css::uno::Reference< css::uno::XInterface >(
            static_cast< cppu::OWeakObject * >(this)));
}

void ChildAccess::commitChanges(bool valid, Modifications * globalModifications)
{
    assert(globalModifications != 0);
    commitChildChanges(valid, globalModifications);
    if (valid && changedValue_.get() != 0) {
        Path path(getAbsolutePath());
        getComponents().addModification(path);
        globalModifications->add(path);
        switch (node_->kind()) {
        case Node::KIND_PROPERTY:
            dynamic_cast< PropertyNode * >(node_.get())->setValue(
                Data::NO_LAYER, *changedValue_);
            break;
        case Node::KIND_LOCALIZED_VALUE:
            dynamic_cast< LocalizedValueNode * >(node_.get())->setValue(
                Data::NO_LAYER, *changedValue_);
            break;
        default:
            assert(false); // this cannot happen
            break;
        }
    }
    changedValue_.reset();
}

ChildAccess::~ChildAccess() {
    osl::MutexGuard g(*lock_);
    if (parent_.is()) {
        parent_->releaseChild(name_);
    }
}

void ChildAccess::addTypes(std::vector< css::uno::Type > * types) const {
    assert(types != 0);
    types->push_back(cppu::UnoType< css::container::XChild >::get());
    types->push_back(cppu::UnoType< css::lang::XUnoTunnel >::get());
}

void ChildAccess::addSupportedServiceNames(
    std::vector< rtl::OUString > * services)
{
    assert(services != 0);
    services->push_back(
        getParentNode()->kind() == Node::KIND_GROUP
        ? rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.configuration.GroupElement"))
        : rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.configuration.SetElement")));
}

css::uno::Any ChildAccess::queryInterface(css::uno::Type const & aType)
    throw (css::uno::RuntimeException)
{
    assert(thisIs(IS_ANY));
    osl::MutexGuard g(*lock_);
    checkLocalizedPropertyAccess();
    css::uno::Any res(Access::queryInterface(aType));
    return res.hasValue()
        ? res
        : cppu::queryInterface(
            aType, static_cast< css::container::XChild * >(this),
            static_cast< css::lang::XUnoTunnel * >(this));
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
