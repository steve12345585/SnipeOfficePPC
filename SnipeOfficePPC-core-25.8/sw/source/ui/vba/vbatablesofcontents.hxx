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
#ifndef SW_VBA_TABLESOFCONTENTS_HXX
#define SW_VBA_TABLESOFCONTENTS_HXX

#include <vbahelper/vbacollectionimpl.hxx>
#include <ooo/vba/word/XTablesOfContents.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <com/sun/star/text/XTextDocument.hpp>
#include <ooo/vba/word/XTableOfContents.hpp>
#include <ooo/vba/word/XRange.hpp>

typedef CollTestImplHelper< ooo::vba::word::XTablesOfContents > SwVbaTablesOfContents_BASE;

class SwVbaTablesOfContents : public SwVbaTablesOfContents_BASE
{
private:
    css::uno::Reference< css::text::XTextDocument > mxTextDocument;

public:
    SwVbaTablesOfContents( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::text::XTextDocument >& xDoc ) throw ( css::uno::RuntimeException );
    virtual ~SwVbaTablesOfContents() {}

    // Methods
    virtual css::uno::Reference< ::ooo::vba::word::XTableOfContents > SAL_CALL Add( const css::uno::Reference< ::ooo::vba::word::XRange >& Range, const css::uno::Any& UseHeadingStyles, const css::uno::Any& UpperHeadingLevel, const css::uno::Any& LowerHeadingLevel, const css::uno::Any& UseFields, const css::uno::Any& TableID, const css::uno::Any& RightAlignPageNumbers, const css::uno::Any& IncludePageNumbers, const css::uno::Any& AddedStyles, const css::uno::Any& UseHyperlinks, const css::uno::Any& HidePageNumbersInWeb, const css::uno::Any& UseOutlineLevels ) throw (css::uno::RuntimeException);

    // XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

    // SwVbaTablesOfContents_BASE
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif /* SW_VBA_TABLESOFCONTENTS_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
