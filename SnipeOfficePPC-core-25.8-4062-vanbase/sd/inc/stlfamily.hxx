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

#ifndef _SD_STLFAMILY_HXX
#define _SD_STLFAMILY_HXX

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include <rtl/ref.hxx>

#include <cppuhelper/implbase7.hxx>

#include <svl/style.hxx>

#include <editeng/unoipset.hxx>

class SdStyleSheet;
class SdPage;
struct SdStyleFamilyImpl;

class SdStyleFamily : public ::cppu::WeakImplHelper7< css::container::XNameContainer, css::container::XNamed, css::container::XIndexAccess, css::lang::XSingleServiceFactory,  css::lang::XServiceInfo, css::lang::XComponent, css::beans::XPropertySet >
{
public:
    /// creates the style family for the given SfxStyleFamily
    SdStyleFamily( const rtl::Reference< SfxStyleSheetPool >& xPool, SfxStyleFamily nFamily );

    /// creates the presentation family for the given masterpage
    SdStyleFamily( const rtl::Reference< SfxStyleSheetPool >& xPool, const SdPage* pMasterPage );

    virtual ~SdStyleFamily();

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw(css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw(css::uno::RuntimeException);
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw(css::uno::RuntimeException);

    // XNamed
    virtual ::rtl::OUString SAL_CALL getName(  ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL setName( const ::rtl::OUString& aName ) throw (css::uno::RuntimeException);

    // XNameAccess
    virtual css::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw(css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException);
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames() throw(css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw(css::uno::RuntimeException);

    // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType() throw(css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements() throw(css::uno::RuntimeException);

    // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() throw(css::uno::RuntimeException) ;
    virtual css::uno::Any SAL_CALL getByIndex( sal_Int32 Index ) throw(css::lang::IndexOutOfBoundsException, css::lang::WrappedTargetException, css::uno::RuntimeException);

    // XNameContainer
    virtual void SAL_CALL insertByName( const ::rtl::OUString& aName, const css::uno::Any& aElement ) throw(css::lang::IllegalArgumentException, css::container::ElementExistException, css::lang::WrappedTargetException, css::uno::RuntimeException);
    virtual void SAL_CALL removeByName( const ::rtl::OUString& Name ) throw(css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException);

    // XNameReplace
    virtual void SAL_CALL replaceByName( const ::rtl::OUString& aName, const css::uno::Any& aElement ) throw(css::lang::IllegalArgumentException, css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException);

    // XSingleServiceFactory
    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstance(  ) throw(css::uno::Exception, css::uno::RuntimeException);
    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstanceWithArguments( const css::uno::Sequence< css::uno::Any >& aArguments ) throw(css::uno::Exception, css::uno::RuntimeException);

    // XComponent
    virtual void SAL_CALL dispose(  ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL addEventListener( const css::uno::Reference< css::lang::XEventListener >& xListener ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL removeEventListener( const css::uno::Reference< css::lang::XEventListener >& aListener ) throw (css::uno::RuntimeException);

    // XPropertySet
    virtual css::uno::Reference<css::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const css::uno::Any& aValue ) throw (css::beans::UnknownPropertyException, css::beans::PropertyVetoException, css::lang::IllegalArgumentException, css::lang::WrappedTargetException, css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw (css::beans::UnknownPropertyException, css::lang::WrappedTargetException, css::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const css::uno::Reference<css::beans::XPropertyChangeListener>& xListener ) throw (css::beans::UnknownPropertyException, css::lang::WrappedTargetException, css::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const css::uno::Reference<css::beans::XPropertyChangeListener>& aListener ) throw (css::beans::UnknownPropertyException, css::lang::WrappedTargetException, css::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener(const ::rtl::OUString& PropertyName, const css::uno::Reference<css::beans::XVetoableChangeListener>& aListener ) throw (css::beans::UnknownPropertyException, css::lang::WrappedTargetException, css::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener(const ::rtl::OUString& PropertyName,const css::uno::Reference<css::beans::XVetoableChangeListener>&aListener ) throw (css::beans::UnknownPropertyException, css::lang::WrappedTargetException, css::uno::RuntimeException);

private:
    void throwIfDisposed() const throw(css::uno::RuntimeException);
    SdStyleSheet* GetValidNewSheet( const css::uno::Any& rElement ) throw(css::lang::IllegalArgumentException);
    SdStyleSheet* GetSheetByName( const ::rtl::OUString& rName ) throw(css::container::NoSuchElementException, css::lang::WrappedTargetException );

    SfxStyleFamily mnFamily;
    rtl::Reference< SfxStyleSheetPool > mxPool;
    SdStyleFamilyImpl*  mpImpl;
};

typedef rtl::Reference< SdStyleFamily > SdStyleFamilyRef;

#endif     // _SD_STLFAMILY_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
