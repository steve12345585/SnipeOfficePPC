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
#ifndef SW_VBA_LISTLEVEL_HXX
#define SW_VBA_LISTLEVEL_HXX

#include <ooo/vba/word/XListLevel.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/text/XTextDocument.hpp>
#include "vbalisthelper.hxx"


typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XListLevel > SwVbaListLevel_BASE;

class SwVbaListLevel : public SwVbaListLevel_BASE
{
private:
    SwVbaListHelperRef pListHelper;
    sal_Int32 mnLevel;

public:
    SwVbaListLevel( const css::uno::Reference< ooo::vba::XHelperInterface >& rParent, const css::uno::Reference< css::uno::XComponentContext >& rContext, SwVbaListHelperRef pHelper, sal_Int32 nLevel ) throw ( css::uno::RuntimeException );
    virtual ~SwVbaListLevel();

    // Attributes
    virtual ::sal_Int32 SAL_CALL getAlignment() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setAlignment( ::sal_Int32 _alignment ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< ::ooo::vba::word::XFont > SAL_CALL getFont() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setFont( const css::uno::Reference< ::ooo::vba::word::XFont >& _font ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getIndex() throw (css::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getLinkedStyle() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setLinkedStyle( const ::rtl::OUString& _linkedstyle ) throw (css::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getNumberFormat() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setNumberFormat( const ::rtl::OUString& _numberformat ) throw (css::uno::RuntimeException);
    virtual float SAL_CALL getNumberPosition() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setNumberPosition( float _numberposition ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getNumberStyle() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setNumberStyle( ::sal_Int32 _numberstyle ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getResetOnHigher() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setResetOnHigher( ::sal_Int32 _resetonhigher ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getStartAt() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setStartAt( ::sal_Int32 _startat ) throw (css::uno::RuntimeException);
    virtual float SAL_CALL getTabPosition() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setTabPosition( float _tabposition ) throw (css::uno::RuntimeException);
    virtual float SAL_CALL getTextPosition() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setTextPosition( float _textposition ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getTrailingCharacter() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setTrailingCharacter( ::sal_Int32 _trailingcharacter ) throw (css::uno::RuntimeException);

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SW_VBA_LISTLEVEL_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
