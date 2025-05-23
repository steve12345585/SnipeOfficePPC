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
