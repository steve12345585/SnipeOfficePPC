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
#ifndef SC_VBA_SCROLLBAR_HXX
#define SC_VBA_SCROLLBAR_HXX
#include <cppuhelper/implbase1.hxx>
#include <ooo/vba/msforms/XScrollBar.hpp>

#include "vbacontrol.hxx"
#include <vbahelper/vbahelper.hxx>

typedef cppu::ImplInheritanceHelper1< ScVbaControl, ov::msforms::XScrollBar > ScrollBarImpl_BASE;

class ScVbaScrollBar : public ScrollBarImpl_BASE
{
public:
    ScVbaScrollBar( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::uno::XInterface >& xControl, const css::uno::Reference< css::frame::XModel >& xModel, ov::AbstractGeometryAttributes* pGeomHelper  );
   // Attributes
    virtual css::uno::Any SAL_CALL getValue() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setValue( const css::uno::Any& _value ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getMax() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMax( ::sal_Int32 _max ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getMin() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMin( ::sal_Int32 _min ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getLargeChange() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setLargeChange( ::sal_Int32 _largechange ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getSmallChange() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSmallChange( ::sal_Int32 _smallchange ) throw (css::uno::RuntimeException);


    //XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif //SC_VBA_LABEL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
