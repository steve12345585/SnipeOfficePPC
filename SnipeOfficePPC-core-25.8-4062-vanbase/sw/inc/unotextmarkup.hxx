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

#ifndef _UNOTEXTMARKUP_HXX
#define _UNOTEXTMARKUP_HXX

#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/text/XTextMarkup.hpp>
#include <com/sun/star/text/XMultiTextMarkup.hpp>
#include <calbck.hxx>
#include <modeltoviewhelper.hxx>

#include <map>

namespace com { namespace sun { namespace star { namespace container {
    class XStringKeyMap;
} } } }

class SwTxtNode;
class SfxPoolItem;

/** Implementation of the css::text::XTextMarkup interface
 */
class SwXTextMarkup:
    public ::cppu::WeakImplHelper2
    <
        ::com::sun::star::text::XTextMarkup,
        ::com::sun::star::text::XMultiTextMarkup
    >,
    public SwClient
{
public:
    SwXTextMarkup( SwTxtNode& rTxtNode, const ModelToViewHelper& rConversionMap );
    virtual ~SwXTextMarkup();

    // ::com::sun::star::text::XTextMarkup:
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XStringKeyMap > SAL_CALL getMarkupInfoContainer() throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL commitTextMarkup(::sal_Int32 nType, const ::rtl::OUString & aIdentifier, ::sal_Int32 nStart, ::sal_Int32 nLength, const ::com::sun::star::uno::Reference< ::com::sun::star::container::XStringKeyMap > & xMarkupInfoContainer) throw (::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::text::XMultiTextMarkup:
    virtual void SAL_CALL commitMultiTextMarkup( const ::com::sun::star::uno::Sequence< ::com::sun::star::text::TextMarkupDescriptor >& aMarkups ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

private:
    SwXTextMarkup( const SwXTextMarkup & ); // not defined
    SwXTextMarkup & operator =( const SwXTextMarkup & ); // not defined

protected:
    //SwClient
    virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

    SwTxtNode* mpTxtNode;
    const ModelToViewHelper maConversionMap;
};


/** Implementation of the ::com::sun::star::container::XStringKeyMap interface
 */
class SwXStringKeyMap:
    public ::cppu::WeakImplHelper1<
        ::com::sun::star::container::XStringKeyMap>
{
public:
    SwXStringKeyMap();

    // ::com::sun::star::container::XStringKeyMap:
    virtual ::com::sun::star::uno::Any SAL_CALL getValue(const ::rtl::OUString & aKey) throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::container::NoSuchElementException);
    virtual ::sal_Bool SAL_CALL hasValue(const ::rtl::OUString & aKey) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL insertValue(const ::rtl::OUString & aKey, const ::com::sun::star::uno::Any & aValue) throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::container::ElementExistException);
    virtual ::sal_Int32 SAL_CALL getCount() throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getKeyByIndex(::sal_Int32 nIndex) throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IndexOutOfBoundsException);
    virtual ::com::sun::star::uno::Any SAL_CALL getValueByIndex(::sal_Int32 nIndex) throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IndexOutOfBoundsException);

private:
    SwXStringKeyMap(SwXStringKeyMap &); // not defined
    void operator =(SwXStringKeyMap &); // not defined

    virtual ~SwXStringKeyMap() {}

    std::map< rtl::OUString, ::com::sun::star::uno::Any > maMap;
};

#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
