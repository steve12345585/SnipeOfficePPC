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
#ifndef SC_VBA_COMMENTS_HXX
#define SC_VBA_COMMENTS_HXX

#include <cppuhelper/implbase1.hxx>

#include <ooo/vba/excel/XComments.hpp>

#include "excelvbahelper.hxx"
#include <vbahelper/vbacollectionimpl.hxx>
#include "vbacomment.hxx"

typedef CollTestImplHelper< ov::excel::XComments > ScVbaComments_BASE;

class ScVbaComments : public ScVbaComments_BASE
{
public:
    ScVbaComments(
        const css::uno::Reference< ov::XHelperInterface >& xParent,
        const css::uno::Reference< css::uno::XComponentContext > & xContext,
        const css::uno::Reference< css::frame::XModel >& xModel,
        const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess );

    virtual ~ScVbaComments() {}

    // XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

    // ScVbaComments_BASE
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();

private:
    css::uno::Reference< css::frame::XModel > mxModel;
};

#endif /* SC_VBA_COMMENTS_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
