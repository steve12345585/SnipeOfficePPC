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

#ifndef SW_VBA_TABLES_HXX
#define SW_VBA_TABLES_HXX

#include <ooo/vba/word/XTables.hpp>
#include <vbahelper/vbacollectionimpl.hxx>


typedef CollTestImplHelper< ov::word::XTables > SwVbaTables_BASE;

class SwVbaTables : public SwVbaTables_BASE
{
    css::uno::Reference< css::frame::XModel > mxDocument;
public:
    SwVbaTables( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::frame::XModel >& xDocument );
    // XTables
    virtual css::uno::Reference< ov::word::XTable > SAL_CALL Add( const css::uno::Reference< ::ooo::vba::word::XRange >& Range, const css::uno::Any& NumRows, const css::uno::Any& NumColumns, const css::uno::Any& DefaultTableBehavior, const css::uno::Any& AutoFitBehavior ) throw (css::script::BasicErrorException, css::uno::RuntimeException);
    // XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);
    // ScVbaCollectionBaseImpl
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
