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

#ifndef SW_VBA_TABLE_HXX
#define SW_VBA_TABLE_HXX
#include <vbahelper/vbahelperinterface.hxx>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/text/XTextTable.hpp>
#include <ooo/vba/word/XRange.hpp>
#include <ooo/vba/word/XTable.hpp>

typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XTable > SwVbaTable_BASE;

class SwVbaTable : public SwVbaTable_BASE
{
    css::uno::Reference< css::text::XTextDocument > mxTextDocument;
    css::uno::Reference< css::text::XTextTable > mxTextTable;
public:
    SwVbaTable( const css::uno::Reference< ooo::vba::XHelperInterface >& rParent, const css::uno::Reference< css::uno::XComponentContext >& rContext, const css::uno::Reference< css::text::XTextDocument >& rDocument, const css::uno::Reference< css::text::XTextTable >& xTextTable) throw ( css::uno::RuntimeException);
    css::uno::Reference< css::text::XTextDocument > getDocument() const { return mxTextDocument; };
    virtual css::uno::Reference< ::ooo::vba::word::XRange > SAL_CALL Range(  ) throw (css::script::BasicErrorException, css::uno::RuntimeException);
    virtual void SAL_CALL Select(  ) throw (css::script::BasicErrorException, css::uno::RuntimeException);
    virtual void SAL_CALL Delete(  ) throw (css::script::BasicErrorException, css::uno::RuntimeException);
    virtual css::uno::Reference< ::ooo::vba::word::XRange > SAL_CALL ConvertToText( const css::uno::Any& Separator, const css::uno::Any& NestedTables ) throw (css::script::BasicErrorException, css::uno::RuntimeException);
    virtual rtl::OUString SAL_CALL getName( ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL Borders( const css::uno::Any& aIndex ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL Rows( const css::uno::Any& aIndex ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL Columns( const css::uno::Any& aIndex ) throw (css::uno::RuntimeException);

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
