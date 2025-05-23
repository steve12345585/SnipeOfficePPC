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
#ifndef SW_VBA_ROW_HXX
#define SW_VBA_ROW_HXX

#include <ooo/vba/word/XRow.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/table/XTableRows.hpp>
#include <com/sun/star/text/XTextTable.hpp>

typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XRow > SwVbaRow_BASE;

class SwVbaRow : public SwVbaRow_BASE
{
private:
    css::uno::Reference< css::text::XTextTable > mxTextTable;
    css::uno::Reference< css::table::XTableRows > mxTableRows;
    css::uno::Reference< css::beans::XPropertySet > mxRowProps;
    sal_Int32 mnIndex;

public:
    SwVbaRow( const css::uno::Reference< ooo::vba::XHelperInterface >& rParent, const css::uno::Reference< css::uno::XComponentContext >& rContext, const css::uno::Reference< css::text::XTextTable >& xTextTable, sal_Int32 nIndex ) throw ( css::uno::RuntimeException );
    virtual ~SwVbaRow();

    // Attributes
    virtual css::uno::Any SAL_CALL getHeight() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setHeight( const css::uno::Any& _height ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getHeightRule() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setHeightRule( ::sal_Int32 _heightrule ) throw (css::uno::RuntimeException);

    // Methods
    virtual void SAL_CALL Select(  ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL SetHeight( float height, sal_Int32 heightrule ) throw (css::uno::RuntimeException);

    static void SelectRow( const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XTextTable >& xTextTable, sal_Int32 nStartRow, sal_Int32 nEndRow ) throw (css::uno::RuntimeException);

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SW_VBA_ROW_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
