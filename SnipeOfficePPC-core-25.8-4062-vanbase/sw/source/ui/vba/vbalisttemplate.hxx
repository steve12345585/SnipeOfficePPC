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
#ifndef SW_VBA_LISTTEMPLATE_HXX
#define SW_VBA_LISTTEMPLATE_HXX

#include <ooo/vba/word/XListTemplate.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/text/XTextDocument.hpp>
#include "vbalisthelper.hxx"


typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XListTemplate > SwVbaListTemplate_BASE;

class SwVbaListTemplate : public SwVbaListTemplate_BASE
{
private:
    SwVbaListHelperRef pListHelper;

public:
    SwVbaListTemplate( const css::uno::Reference< ooo::vba::XHelperInterface >& rParent, const css::uno::Reference< css::uno::XComponentContext >& rContext, const css::uno::Reference< css::text::XTextDocument >& xTextDoc, sal_Int32 nGalleryType, sal_Int32 nTemplateType ) throw ( css::uno::RuntimeException );
    virtual ~SwVbaListTemplate();

    void applyListTemplate( css::uno::Reference< css::beans::XPropertySet >& xProps ) throw ( css::uno::RuntimeException );

    // Methods
    virtual css::uno::Any SAL_CALL ListLevels( const css::uno::Any& index ) throw ( css::uno::RuntimeException );

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SW_VBA_LISTTEMPLATE_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
