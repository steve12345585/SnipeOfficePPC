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


#include "com/sun/star/beans/XPropertySet.hpp"
#include "com/sun/star/container/XHierarchicalNameAccess.hpp"
#include "com/sun/star/lang/XMain.hpp"
#include "com/sun/star/lang/XSingleComponentFactory.hpp"
#include "com/sun/star/reflection/XCompoundTypeDescription.hpp"
#include "com/sun/star/reflection/XInterfaceAttributeTypeDescription2.hpp"
#include "com/sun/star/reflection/XInterfaceMemberTypeDescription.hpp"
#include "com/sun/star/reflection/XInterfaceMethodTypeDescription.hpp"
#include "com/sun/star/reflection/XInterfaceTypeDescription2.hpp"
#include "com/sun/star/reflection/XPublished.hpp"
#include "com/sun/star/reflection/XServiceTypeDescription2.hpp"
#include "com/sun/star/reflection/XSingletonTypeDescription2.hpp"
#include "com/sun/star/reflection/XStructTypeDescription.hpp"
#include "com/sun/star/reflection/XTypeDescription.hpp"
#include "com/sun/star/registry/InvalidRegistryException.hpp"
#include "com/sun/star/registry/XRegistryKey.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/TypeClass.hpp"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implbase1.hxx"
#include "cppuhelper/weak.hxx"
#include "rtl/textenc.h"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"
#include "uno/environment.h"
#include "uno/lbnames.h"

#include /*MSVC trouble: <cstdlib>*/ <stdlib.h>
#include <iostream>
#include <ostream>

namespace css = com::sun::star;

namespace {

class Service: public cppu::WeakImplHelper1< css::lang::XMain > {
public:
    virtual sal_Int32 SAL_CALL
    run(css::uno::Sequence< rtl::OUString > const & arguments)
        throw (css::uno::RuntimeException);

    static rtl::OUString getImplementationName();

    static css::uno::Sequence< rtl::OUString > getSupportedServiceNames();

    static css::uno::Reference< css::uno::XInterface > SAL_CALL createInstance(
        css::uno::Reference< css::uno::XComponentContext > const & context)
        throw (css::uno::Exception);

private:
    explicit Service(
        css::uno::Reference< css::uno::XComponentContext > const & context):
        m_context(context)
    {}

    css::uno::Reference< css::uno::XComponentContext > m_context;
};

}

namespace {

std::ostream & operator <<(std::ostream & out, rtl::OUString const & value) {
    return out << rtl::OUStringToOString(value, RTL_TEXTENCODING_UTF8).getStr();
}

void assertTrue(bool argument) {
    if (!argument) {
        std::cerr
            << "assertTrue(" << argument << ") failed" << std::endl;
        /*MSVC trouble: std::*/abort();
    }
}

void assertFalse(bool argument) {
    if (argument) {
        std::cerr
            << "assertFalse(" << argument << ") failed" << std::endl;
        /*MSVC trouble: std::*/abort();
    }
}

template< typename T > void assertEqual(T const & value, T const & argument) {
    if (argument != value) {
        std::cerr
            << "assertEqual(" << value << ", " << argument << ") failed"
            << std::endl;
        /*MSVC trouble: std::*/abort();
    }
}

}

sal_Int32 Service::run(css::uno::Sequence< rtl::OUString > const &)
    throw (css::uno::RuntimeException)
{
    css::uno::Reference< css::lang::XMultiComponentFactory > factory(
        m_context->getServiceManager());
    assertTrue(factory.is());
    css::uno::Sequence< css::uno::Any > args(1);
    args[0] = css::uno::Reference< css::beans::XPropertySet >(
        factory, css::uno::UNO_QUERY_THROW)->getPropertyValue(
            rtl::OUString("Registry"));
    css::uno::Reference< css::container::XHierarchicalNameAccess > provider(
        factory->createInstanceWithArgumentsAndContext(
            rtl::OUString( "com.sun.star.comp.stoc.RegistryTypeDescriptionProvider"),
            args, m_context),
        css::uno::UNO_QUERY_THROW);

    // The following assumes that interface members are sorted by increasing
    // values of XInterfaceMemberTypeDescription.getPosition, the exceptions
    // of interface attributes and interface methods, the constructors of
    // services, and the exceptions of service constructors are sorted as given
    // in the UNOIDL source code:

    assertEqual< bool >(
        false,
        provider->hasByHierarchicalName(
            rtl::OUString( "[][]boolean")));
    assertEqual< bool >(
        false,
        provider->hasByHierarchicalName(
            rtl::OUString( "test.registrytdprovider.Struct3<boolean,boolean>")));
    assertEqual< bool >(
        false,
        provider->hasByHierarchicalName(
            rtl::OUString( "com.sun.star.uno.XComponentContext::getValueByName")));

    css::uno::Reference< css::reflection::XCompoundTypeDescription > exception;
    exception
        = css::uno::Reference< css::reflection::XCompoundTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "com.sun.star.uno.Exception")),
            css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_EXCEPTION, exception->getTypeClass());
    assertEqual( rtl::OUString( "com.sun.star.uno.Exception"),
        exception->getName());
    assertFalse(exception->getBaseType().is());
    exception
        = css::uno::Reference< css::reflection::XCompoundTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "com.sun.star.uno.RuntimeException")),
            css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_EXCEPTION, exception->getTypeClass());
    assertEqual( rtl::OUString( "com.sun.star.uno.RuntimeException"),
        exception->getName());
    assertEqual( rtl::OUString( "com.sun.star.uno.Exception"),
        exception->getBaseType()->getName());

    css::uno::Reference< css::reflection::XStructTypeDescription > structure;

    structure = css::uno::Reference< css::reflection::XStructTypeDescription >(
        provider->getByHierarchicalName(
            rtl::OUString( "test.registrytdprovider.Struct2")),
        css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_STRUCT, structure->getTypeClass());
    assertEqual( rtl::OUString( "test.registrytdprovider.Struct2"),
        structure->getName());
    assertFalse(structure->getBaseType().is());
    assertEqual< sal_Int32 >(1, structure->getMemberTypes().getLength());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.Struct1"),
        structure->getMemberTypes()[0]->getName());
    assertEqual< sal_Int32 >(1, structure->getMemberNames().getLength());
    assertEqual(
        rtl::OUString("s1"),
        structure->getMemberNames()[0]);
    assertEqual< sal_Int32 >(0, structure->getTypeParameters().getLength());
    assertEqual< sal_Int32 >(0, structure->getTypeArguments().getLength());

    structure = css::uno::Reference< css::reflection::XStructTypeDescription >(
        provider->getByHierarchicalName(
            rtl::OUString( "test.registrytdprovider.Struct3")),
        css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_STRUCT, structure->getTypeClass());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.Struct3"),
        structure->getName());
    assertFalse(structure->getBaseType().is());
    assertEqual< sal_Int32 >(1, structure->getMemberTypes().getLength());
    assertEqual(
        css::uno::TypeClass_UNKNOWN,
        structure->getMemberTypes()[0]->getTypeClass());
    assertEqual(
        rtl::OUString("U"),
        structure->getMemberTypes()[0]->getName());
    assertEqual< sal_Int32 >(1, structure->getMemberNames().getLength());
    assertEqual(
        rtl::OUString("s2"),
        structure->getMemberNames()[0]);
    assertEqual< sal_Int32 >(2, structure->getTypeParameters().getLength());
    assertEqual(
        rtl::OUString("T"),
        structure->getTypeParameters()[0]);
    assertEqual(
        rtl::OUString("U"),
        structure->getTypeParameters()[1]);
    assertEqual< sal_Int32 >(0, structure->getTypeArguments().getLength());

    structure = css::uno::Reference< css::reflection::XStructTypeDescription >(
        provider->getByHierarchicalName(
            rtl::OUString( "test.registrytdprovider.Struct4")),
        css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_STRUCT, structure->getTypeClass());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.Struct4"),
        structure->getName());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.Struct2"),
        structure->getBaseType()->getName());
    assertEqual< sal_Int32 >(1, structure->getMemberTypes().getLength());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.Struct3<"
                "test.registrytdprovider.Struct2,"
                "test.registrytdprovider.Struct3<boolean,any>>"),
        structure->getMemberTypes()[0]->getName());
    assertEqual< sal_Int32 >(1, structure->getMemberNames().getLength());
    assertEqual(
        rtl::OUString("s2"),
        structure->getMemberNames()[0]);
    assertEqual< sal_Int32 >(0, structure->getTypeParameters().getLength());
    assertEqual< sal_Int32 >(0, structure->getTypeArguments().getLength());

    css::uno::Reference< css::reflection::XInterfaceTypeDescription2 >
        interface;

    interface
        = css::uno::Reference< css::reflection::XInterfaceTypeDescription2 >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.XTest1")),
            css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_INTERFACE, interface->getTypeClass());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.XTest1"),
        interface->getName());
    css::uno::Sequence<
        css::uno::Reference< css::reflection::XTypeDescription > > bases(
            interface->getBaseTypes());
    assertEqual< sal_Int32 >(1, bases.getLength());
    assertEqual(
        rtl::OUString( "com.sun.star.uno.XInterface"),
        bases[0]->getName());
    css::uno::Sequence<
        css::uno::Reference< css::reflection::XTypeDescription > >
            optionalBases(interface->getOptionalBaseTypes());
    assertEqual< sal_Int32 >(1, optionalBases.getLength());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.XBase"),
        optionalBases[0]->getName());
    css::uno::Sequence<
        css::uno::Reference<
            css::reflection::XInterfaceMemberTypeDescription > > members(
                interface->getMembers());
    assertEqual< sal_Int32 >(5, members.getLength());

    css::uno::Reference< css::reflection::XInterfaceAttributeTypeDescription2 >
        attribute;
    css::uno::Sequence<
        css::uno::Reference< css::reflection::XCompoundTypeDescription > >
            getExceptions;
    css::uno::Sequence<
        css::uno::Reference< css::reflection::XCompoundTypeDescription > >
            setExceptions;
    css::uno::Reference< css::reflection::XInterfaceMethodTypeDescription >
        method;

    attribute = css::uno::Reference<
        css::reflection::XInterfaceAttributeTypeDescription2 >(
            members[0], css::uno::UNO_QUERY_THROW);
    assertEqual(
        css::uno::TypeClass_INTERFACE_ATTRIBUTE, attribute->getTypeClass());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.XTest1::a1"),
        attribute->getName());
    assertEqual(
        rtl::OUString("a1"),
        attribute->getMemberName());
    assertEqual< sal_Int32 >(3, attribute->getPosition());
    assertEqual< bool >(false, attribute->isReadOnly());
    assertEqual(
        rtl::OUString("short"),
        attribute->getType()->getName());
    assertEqual< bool >(true, attribute->isBound());
    getExceptions = attribute->getGetExceptions();
    assertEqual< sal_Int32 >(0, getExceptions.getLength());
    setExceptions = attribute->getSetExceptions();
    assertEqual< sal_Int32 >(0, setExceptions.getLength());

    attribute = css::uno::Reference<
        css::reflection::XInterfaceAttributeTypeDescription2 >(
            members[1], css::uno::UNO_QUERY_THROW);
    assertEqual(
        css::uno::TypeClass_INTERFACE_ATTRIBUTE, attribute->getTypeClass());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.XTest1::a2"),
        attribute->getName());
    assertEqual(
        rtl::OUString("a2"),
        attribute->getMemberName());
    assertEqual< sal_Int32 >(4, attribute->getPosition());
    assertEqual< bool >(false, attribute->isReadOnly());
    assertEqual(
        rtl::OUString("long"),
        attribute->getType()->getName());
    assertEqual< bool >(false, attribute->isBound());
    getExceptions = attribute->getGetExceptions();
    assertEqual< sal_Int32 >(2, getExceptions.getLength());
    assertEqual(
        rtl::OUString( "com.sun.star.lang.WrappedTargetException"),
        getExceptions[0]->getName());
    assertEqual(
        rtl::OUString( "com.sun.star.uno.RuntimeException"),
        getExceptions[1]->getName());
    setExceptions = attribute->getSetExceptions();
    assertEqual< sal_Int32 >(2, setExceptions.getLength());
    assertEqual(
        rtl::OUString( "com.sun.star.uno.Exception"),
        setExceptions[0]->getName());
    assertEqual(
        rtl::OUString( "com.sun.star.lang.WrappedTargetException"),
        setExceptions[1]->getName());

    attribute = css::uno::Reference<
        css::reflection::XInterfaceAttributeTypeDescription2 >(
            members[2], css::uno::UNO_QUERY_THROW);
    assertEqual(
        css::uno::TypeClass_INTERFACE_ATTRIBUTE, attribute->getTypeClass());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.XTest1::a3"),
        attribute->getName());
    assertEqual(
        rtl::OUString("a3"),
        attribute->getMemberName());
    assertEqual< sal_Int32 >(5, attribute->getPosition());
    assertEqual< bool >(true, attribute->isReadOnly());
    assertEqual(
        rtl::OUString("hyper"),
        attribute->getType()->getName());
    assertEqual< bool >(true, attribute->isBound());
    getExceptions = attribute->getGetExceptions();
    assertEqual< sal_Int32 >(1, getExceptions.getLength());
    assertEqual(
        rtl::OUString( "com.sun.star.lang.WrappedTargetException"),
        getExceptions[0]->getName());
    setExceptions = attribute->getSetExceptions();
    assertEqual< sal_Int32 >(0, setExceptions.getLength());

    method = css::uno::Reference<
        css::reflection::XInterfaceMethodTypeDescription >(
            members[3], css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_INTERFACE_METHOD, method->getTypeClass());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.XTest1::f1"),
        method->getName());
    assertEqual(
        rtl::OUString("f1"),
        method->getMemberName());
    assertEqual< sal_Int32 >(6, method->getPosition());
    assertEqual< bool >(false, method->isOneway());
    assertEqual< sal_Int32 >(1, method->getParameters().getLength());
    assertEqual(
        rtl::OUString("p"),
        method->getParameters()[0]->getName());
    assertEqual(
        rtl::OUString("any"),
        method->getParameters()[0]->getType()->getName());
    assertEqual< bool >(false, method->getParameters()[0]->isIn());
    assertEqual< bool >(true, method->getParameters()[0]->isOut());
    assertEqual< sal_Int32 >(0, method->getParameters()[0]->getPosition());
    assertEqual< sal_Int32 >(1, method->getExceptions().getLength());
    assertEqual(
        rtl::OUString( "com.sun.star.uno.RuntimeException"),
        method->getExceptions()[0]->getName());

    method = css::uno::Reference<
        css::reflection::XInterfaceMethodTypeDescription >(
            members[4], css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_INTERFACE_METHOD, method->getTypeClass());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.XTest1::f2"),
        method->getName());
    assertEqual(
        rtl::OUString("f2"),
        method->getMemberName());
    assertEqual< sal_Int32 >(7, method->getPosition());
    assertEqual< bool >(true, method->isOneway());
    assertEqual< sal_Int32 >(0, method->getParameters().getLength());
    assertEqual< sal_Int32 >(0, method->getExceptions().getLength());

    interface
        = css::uno::Reference< css::reflection::XInterfaceTypeDescription2 >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.XTest2")),
            css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_INTERFACE, interface->getTypeClass());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.XTest2"),
        interface->getName());
    assertEqual< sal_Int32 >(1, interface->getBaseTypes().getLength());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.Typedef2"),
        interface->getBaseTypes()[0]->getName());
    assertEqual< sal_Int32 >(0, interface->getOptionalBaseTypes().getLength());
    assertEqual< sal_Int32 >(0, interface->getMembers().getLength());

    css::uno::Reference< css::reflection::XServiceTypeDescription2 > service;

    service = css::uno::Reference< css::reflection::XServiceTypeDescription2 >(
        provider->getByHierarchicalName(
            rtl::OUString( "test.registrytdprovider.Service1")),
        css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_SERVICE, service->getTypeClass());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.Service1"),
        service->getName());
    assertEqual< sal_Int32 >(0, service->getMandatoryServices().getLength());
    assertEqual< sal_Int32 >(0, service->getOptionalServices().getLength());
    assertEqual< sal_Int32 >(0, service->getMandatoryInterfaces().getLength());
    assertEqual< sal_Int32 >(0, service->getOptionalInterfaces().getLength());
    assertEqual< bool >(true, service->isSingleInterfaceBased());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.XTest1"),
        service->getInterface()->getName());
    assertEqual< sal_Int32 >(2, service->getConstructors().getLength());
    assertEqual(
        rtl::OUString("c1"),
        service->getConstructors()[0]->getName());
    assertEqual< sal_Int32 >(
        0, service->getConstructors()[0]->getParameters().getLength());
    assertEqual< sal_Int32 >(
        0, service->getConstructors()[0]->getExceptions().getLength());
    assertEqual(
        rtl::OUString("c2"),
        service->getConstructors()[1]->getName());
    assertEqual< sal_Int32 >(
        1, service->getConstructors()[1]->getParameters().getLength());
    assertEqual(
        rtl::OUString("p"),
        service->getConstructors()[1]->getParameters()[0]->getName());
    assertEqual(
        rtl::OUString("any"),
        (service->getConstructors()[1]->getParameters()[0]->getType()->
         getName()));
    assertEqual< bool >(
        true, service->getConstructors()[1]->getParameters()[0]->isIn());
    assertEqual< bool >(
        false, service->getConstructors()[1]->getParameters()[0]->isOut());
    assertEqual< sal_Int32 >(
        0, service->getConstructors()[1]->getParameters()[0]->getPosition());
    assertEqual< bool >(
        true,
        service->getConstructors()[1]->getParameters()[0]->isRestParameter());
    assertEqual< sal_Int32 >(
        1, service->getConstructors()[1]->getExceptions().getLength());
    assertEqual(
        rtl::OUString( "com.sun.star.uno.RuntimeException"),
        service->getConstructors()[1]->getExceptions()[0]->getName());

    service = css::uno::Reference< css::reflection::XServiceTypeDescription2 >(
        provider->getByHierarchicalName(
            rtl::OUString( "test.registrytdprovider.Service2")),
        css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_SERVICE, service->getTypeClass());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.Service2"),
        service->getName());
    assertEqual< sal_Int32 >(0, service->getMandatoryServices().getLength());
    assertEqual< sal_Int32 >(0, service->getOptionalServices().getLength());
    assertEqual< sal_Int32 >(1, service->getMandatoryInterfaces().getLength());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.XTest1"),
        service->getMandatoryInterfaces()[0]->getName());
    assertEqual< sal_Int32 >(1, service->getOptionalInterfaces().getLength());
    assertEqual(
        rtl::OUString( "test.registrytdprovider.XBase"),
        service->getOptionalInterfaces()[0]->getName());
    assertEqual< bool >(false, service->isSingleInterfaceBased());
    assertFalse(service->getInterface().is());
    assertEqual< sal_Int32 >(0, service->getConstructors().getLength());

    service = css::uno::Reference< css::reflection::XServiceTypeDescription2 >(
        provider->getByHierarchicalName(
            rtl::OUString( "test.registrytdprovider.Service3")),
        css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_SERVICE, service->getTypeClass());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.Service3"),
        service->getName());
    assertEqual< sal_Int32 >(0, service->getMandatoryServices().getLength());
    assertEqual< sal_Int32 >(0, service->getOptionalServices().getLength());
    assertEqual< sal_Int32 >(0, service->getMandatoryInterfaces().getLength());
    assertEqual< sal_Int32 >(0, service->getOptionalInterfaces().getLength());
    assertEqual< bool >(true, service->isSingleInterfaceBased());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.Typedef2"),
        service->getInterface()->getName());
    assertEqual< sal_Int32 >(0, service->getConstructors().getLength());

    css::uno::Reference< css::reflection::XSingletonTypeDescription2 >
        singleton;

    singleton = css::uno::Reference<
        css::reflection::XSingletonTypeDescription2 >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Singleton1")),
            css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_SINGLETON, singleton->getTypeClass());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.Singleton1"),
        singleton->getName());
    assertFalse(singleton->getService().is());
    assertEqual< bool >(true, singleton->isInterfaceBased());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.XTest1"),
        singleton->getInterface()->getName());

    singleton = css::uno::Reference<
        css::reflection::XSingletonTypeDescription2 >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Singleton2")),
            css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_SINGLETON, singleton->getTypeClass());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.Singleton2"),
        singleton->getName());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.Service1"),
        singleton->getService()->getName());
    assertEqual< bool >(false, singleton->isInterfaceBased());
    assertFalse(singleton->getInterface().is());

    singleton = css::uno::Reference<
        css::reflection::XSingletonTypeDescription2 >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Singleton3")),
            css::uno::UNO_QUERY_THROW);
    assertEqual(css::uno::TypeClass_SINGLETON, singleton->getTypeClass());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.Singleton3"),
        singleton->getName());
    assertFalse(singleton->getService().is());
    assertEqual< bool >(true, singleton->isInterfaceBased());
    assertEqual(
        rtl::OUString(
            "test.registrytdprovider.Typedef2"),
        singleton->getInterface()->getName());

    css::uno::Reference< css::reflection::XPublished > published;
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Enum1")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertTrue(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Enum2")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertFalse(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Struct1")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertTrue(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Struct2")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertFalse(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Struct3")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertTrue(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XStructTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Struct3")),
            css::uno::UNO_QUERY_THROW)->getMemberTypes()[0],
        css::uno::UNO_QUERY);
    assertFalse(published.is());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Struct3a")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertFalse(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Exception1")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertTrue(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Exception2")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertFalse(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.XTest1")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertTrue(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.XTest2")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertFalse(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Typedef1")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertTrue(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Typedef2")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertFalse(published->isPublished());
    //TODO: check constants test.registrytdprovider.Const1 (published),
    // test.registrytdprovider.Const2 (unpublished), and
    // test.registrytdprovider.Consts1.C (no XPublished), which are not
    // accessible via provider->getByHierarchicalName (see #i31428)
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Consts1")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertTrue(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Consts2")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertFalse(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertFalse(published.is());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Service1")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertTrue(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Service2")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertFalse(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Singleton2")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertTrue(published->isPublished());
    published = css::uno::Reference< css::reflection::XPublished >(
        css::uno::Reference< css::reflection::XTypeDescription >(
            provider->getByHierarchicalName(
                rtl::OUString( "test.registrytdprovider.Singleton1")),
            css::uno::UNO_QUERY_THROW),
        css::uno::UNO_QUERY);
    assertTrue(published.is());
    assertFalse(published->isPublished());

    return 0;
}

rtl::OUString Service::getImplementationName() {
    return rtl::OUString("test.registrytdprovider.impl");
}

css::uno::Sequence< rtl::OUString > Service::getSupportedServiceNames() {
    return css::uno::Sequence< rtl::OUString >();
}

css::uno::Reference< css::uno::XInterface > Service::createInstance(
    css::uno::Reference< css::uno::XComponentContext > const & context)
    throw (css::uno::Exception)
{
    return static_cast< cppu::OWeakObject * >(new Service(context));
}

extern "C" SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory(char const * implName,
                                                void * serviceManager, void *) {
    void * p = 0;
    if (serviceManager != 0) {
        css::uno::Reference< css::lang::XSingleComponentFactory > f;
        if (Service::getImplementationName().equalsAscii(implName)) {
            f = cppu::createSingleComponentFactory(
                &Service::createInstance, Service::getImplementationName(),
                Service::getSupportedServiceNames());
        }
        if (f.is()) {
            f->acquire();
            p = f.get();
        }
    }
    return p;
}

namespace {

bool writeInfo(void * registryKey, rtl::OUString const & implementationName,
               css::uno::Sequence< rtl::OUString > const & serviceNames) {
    rtl::OUString keyName("/");
    keyName += implementationName;
    keyName += rtl::OUString("/UNO/SERVICES");
    css::uno::Reference< css::registry::XRegistryKey > key;
    try {
        key = static_cast< css::registry::XRegistryKey * >(registryKey)->
            createKey(keyName);
    } catch (css::registry::InvalidRegistryException &) {}
    if (!key.is()) {
        return false;
    }
    bool success = true;
    for (sal_Int32 i = 0; i < serviceNames.getLength(); ++i) {
        try {
            key->createKey(serviceNames[i]);
        } catch (css::registry::InvalidRegistryException &) {
            success = false;
            break;
        }
    }
    return success;
}

}

extern "C" sal_Bool SAL_CALL component_writeInfo(void *, void * registryKey) {
    return registryKey
        && writeInfo(registryKey, Service::getImplementationName(),
                     Service::getSupportedServiceNames());
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
