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
#ifndef SC_VBA_CHARTOBJECT_HXX
#define SC_VBA_CHARTOBJECT_HXX
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/table/XTableChart.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/document/XEmbeddedObjectSupplier.hpp>
#include <ooo/vba/excel/XChartObject.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <memory>

typedef InheritedHelperInterfaceImpl1<ov::excel::XChartObject > ChartObjectImpl_BASE;

class ScVbaChartObject : public ChartObjectImpl_BASE
{

    css::uno::Reference< css::table::XTableChart  > xTableChart;
    css::uno::Reference< css::document::XEmbeddedObjectSupplier > xEmbeddedObjectSupplier;
    css::uno::Reference< css::beans::XPropertySet > xPropertySet;
    css::uno::Reference< css::drawing::XDrawPageSupplier > xDrawPageSupplier;
    css::uno::Reference< css::drawing::XDrawPage > xDrawPage;
    css::uno::Reference< css::drawing::XShape > xShape;
    css::uno::Reference< css::container::XNamed > xNamed;
    rtl::OUString sPersistName;
    SAL_WNODEPRECATED_DECLARATIONS_PUSH
    std::auto_ptr<ov::ShapeHelper> oShapeHelper;
    SAL_WNODEPRECATED_DECLARATIONS_POP
    css::uno::Reference< css::container::XNamed > xNamedShape;
    rtl::OUString getPersistName();
    css::uno::Reference< css::drawing::XShape > setShape() throw ( css::script::BasicErrorException );
public:
    ScVbaChartObject( const css::uno::Reference< ov::XHelperInterface >& _xParent, const css::uno::Reference< css::uno::XComponentContext >& _xContext, const css::uno::Reference< css::table::XTableChart >& _xTableChart, const css::uno::Reference< css::drawing::XDrawPageSupplier >& _xDrawPageSupplier );
    virtual ::rtl::OUString SAL_CALL getName() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setName( const ::rtl::OUString& sName ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< ov::excel::XChart > SAL_CALL getChart() throw (css::uno::RuntimeException);
    virtual void SAL_CALL Delete() throw ( css::script::BasicErrorException );
    virtual void Activate() throw ( css::script::BasicErrorException );
    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif //SC_VBA_WINDOW_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
