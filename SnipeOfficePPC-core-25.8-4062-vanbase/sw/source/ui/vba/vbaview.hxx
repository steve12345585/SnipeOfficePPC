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
#ifndef SW_VBA_VIEW_HXX
#define SW_VBA_VIEW_HXX

#include <ooo/vba/word/XView.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <com/sun/star/text/XTextViewCursor.hpp>
#include <com/sun/star/text/XTextRange.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XView > SwVbaView_BASE;

class SwVbaView : public SwVbaView_BASE
{
private:
    css::uno::Reference< css::frame::XModel > mxModel;
    css::uno::Reference< css::text::XTextViewCursor > mxViewCursor;
    css::uno::Reference< css::beans::XPropertySet > mxViewSettings;

    css::uno::Reference< css::text::XTextRange > getHFTextRange( sal_Int32 nType ) throw (css::uno::RuntimeException);
    css::uno::Reference< css::text::XTextRange > getFirstObjectPosition( const css::uno::Reference< css::text::XText >& xText ) throw (css::uno::RuntimeException);

public:
    SwVbaView( const css::uno::Reference< ooo::vba::XHelperInterface >& rParent, const css::uno::Reference< css::uno::XComponentContext >& rContext,
        const css::uno::Reference< css::frame::XModel >& rModel ) throw ( css::uno::RuntimeException );
    virtual ~SwVbaView();

   // XView
    virtual ::sal_Int32 SAL_CALL getSeekView() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSeekView( ::sal_Int32 _seekview ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getSplitSpecial() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSplitSpecial( ::sal_Int32 _splitspecial ) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL getTableGridLines() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setTableGridLines( ::sal_Bool _tablegridlines ) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getType() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setType( ::sal_Int32 _type ) throw (css::uno::RuntimeException);

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SW_VBA_VIEW_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
