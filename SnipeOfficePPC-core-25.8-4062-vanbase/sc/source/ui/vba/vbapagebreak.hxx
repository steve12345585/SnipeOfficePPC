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
#ifndef SC_VBA_PAGEBREAK_HXX
#define SC_VBA_PAGEBREAK_HXX

#include <cppuhelper/implbase1.hxx>
#include <ooo/vba/excel/XPageBreak.hpp>
#include <ooo/vba/excel/XHPageBreak.hpp>
#include <ooo/vba/excel/XVPageBreak.hpp>
#include <ooo/vba/excel/XRange.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/script/BasicErrorException.hpp>
#include <com/sun/star/sheet/TablePageBreakData.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <vbahelper/vbahelperinterface.hxx>

template< typename Ifc1 >
class ScVbaPageBreak : public InheritedHelperInterfaceImpl1< Ifc1 >
{
typedef InheritedHelperInterfaceImpl1< Ifc1 > ScVbaPageBreak_BASE;
protected:
    css::uno::Reference< css::beans::XPropertySet > mxRowColPropertySet;
    css::sheet::TablePageBreakData maTablePageBreakData;
public:
    ScVbaPageBreak( const css::uno::Reference< ov::XHelperInterface >& xParent,
                    const css::uno::Reference< css::uno::XComponentContext >& xContext,
                    css::uno::Reference< css::beans::XPropertySet >& xProps,
                    css::sheet::TablePageBreakData aTablePageBreakData) throw (css::uno::RuntimeException);
    virtual ~ScVbaPageBreak(){}

    virtual sal_Int32 SAL_CALL getType( ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL setType(sal_Int32 type) throw (css::uno::RuntimeException);

    virtual void SAL_CALL Delete() throw ( css::script::BasicErrorException, css::uno::RuntimeException);
    virtual css::uno::Reference< ov::excel::XRange> SAL_CALL Location() throw ( css::script::BasicErrorException, css::uno::RuntimeException);

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};


typedef ScVbaPageBreak < ov::excel::XHPageBreak > ScVbaHPageBreak_BASE;

class ScVbaHPageBreak :  public ScVbaHPageBreak_BASE
{
public:
    ScVbaHPageBreak( const css::uno::Reference< ov::XHelperInterface >& xParent,
                    const css::uno::Reference< css::uno::XComponentContext >& xContext,
                    css::uno::Reference< css::beans::XPropertySet >& xProps,
                    css::sheet::TablePageBreakData aTablePageBreakData) throw (css::uno::RuntimeException):
              ScVbaHPageBreak_BASE( xParent,xContext,xProps,aTablePageBreakData ){}

    virtual ~ScVbaHPageBreak(){}

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

//VPageBreak
typedef ScVbaPageBreak < ov::excel::XVPageBreak > ScVbaVPageBreak_BASE;

class ScVbaVPageBreak :  public ScVbaVPageBreak_BASE
{
public:
    ScVbaVPageBreak( const css::uno::Reference< ov::XHelperInterface >& xParent,
                     const css::uno::Reference< css::uno::XComponentContext >& xContext,
                     css::uno::Reference< css::beans::XPropertySet >& xProps,
                     css::sheet::TablePageBreakData aTablePageBreakData) throw (css::uno::RuntimeException);

    virtual ~ScVbaVPageBreak();

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
