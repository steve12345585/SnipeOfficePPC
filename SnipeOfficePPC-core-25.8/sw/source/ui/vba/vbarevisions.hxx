/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
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
#ifndef SW_VBA_PANES_HXX
#define SW_VBA_PANES_HXX

#include <vbahelper/vbacollectionimpl.hxx>
#include <ooo/vba/word/XRevisions.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <com/sun/star/text/XTextRange.hpp>


typedef CollTestImplHelper< ooo::vba::word::XRevisions > SwVbaRevisions_BASE;

class SwVbaRevisions : public SwVbaRevisions_BASE
{
private:
    css::uno::Reference< css::frame::XModel > mxModel;

public:
    SwVbaRevisions( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XTextRange >& xTextRange );

    SwVbaRevisions( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess );
    virtual ~SwVbaRevisions() {}

    // Methods
    virtual void SAL_CALL AcceptAll(  ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL RejectAll(  ) throw (css::uno::RuntimeException);

    // XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

    // SwVbaRevisions_BASE
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif /* SW_VBA_PANES_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
