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
#ifndef SW_VBA_CELLS_HXX
#define SW_VBA_CELLS_HXX

#include <vbahelper/vbacollectionimpl.hxx>
#include <ooo/vba/word/XCells.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <com/sun/star/text/XTextTable.hpp>

typedef CollTestImplHelper< ooo::vba::word::XCells > SwVbaCells_BASE;

class SwVbaCells : public SwVbaCells_BASE
{
private:
    css::uno::Reference< css::text::XTextTable > mxTextTable;
    sal_Int32 mnTop;
    sal_Int32 mnBottom;

public:
    SwVbaCells( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::text::XTextTable >& xTextTable, sal_Int32 nLeft, sal_Int32 nTop, sal_Int32 nRight, sal_Int32 nBottom ) throw ( css::uno::RuntimeException );
    virtual ~SwVbaCells() {}

    // Attributes
    virtual ::sal_Int32 SAL_CALL getWidth() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setWidth( ::sal_Int32 _width ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getHeight() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setHeight( const css::uno::Any& _height ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getHeightRule() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setHeightRule( ::sal_Int32 _heightrule ) throw (css::uno::RuntimeException);

    // Methods
    virtual void SAL_CALL SetWidth( float width, sal_Int32 rulestyle ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL SetHeight( float height, sal_Int32 heightrule ) throw (css::uno::RuntimeException);

    // XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

    // SwVbaCells_BASE
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif /* SW_VBA_CELLS_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
