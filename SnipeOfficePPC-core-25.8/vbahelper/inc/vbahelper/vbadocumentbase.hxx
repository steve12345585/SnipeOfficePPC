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
#ifndef VBA_DOCUMENTBASE_HXX
#define VBA_DOCUMENTBASE_HXX

#include <com/sun/star/frame/XModel.hpp>
#include <ooo/vba/XDocumentBase.hpp>
#include <vbahelper/vbahelperinterface.hxx>

typedef InheritedHelperInterfaceImpl1< ooo::vba::XDocumentBase > VbaDocumentBase_BASE;

class VBAHELPER_DLLPUBLIC VbaDocumentBase : public VbaDocumentBase_BASE
{
protected:
    css::uno::Reference< css::frame::XModel > mxModel;
    css::uno::Reference< css::uno::XInterface > mxVBProject;
protected:
    virtual css::uno::Reference< css::frame::XModel >  getModel() { return mxModel; }
    VbaDocumentBase(    const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext);
public:
    VbaDocumentBase(    const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext,
            css::uno::Reference< css::frame::XModel > xModel );
    VbaDocumentBase(    css::uno::Sequence< css::uno::Any > const& aArgs, css::uno::Reference< css::uno::XComponentContext >const& xContext );
    virtual ~VbaDocumentBase() {}

    // Attributes
    virtual ::rtl::OUString SAL_CALL getName() throw (css::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getPath() throw (css::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getFullName() throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getSaved() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSaved( sal_Bool bSave ) throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getVBProject() throw (css::uno::RuntimeException);

    // Methods
    virtual void SAL_CALL Close( const css::uno::Any &bSaveChanges,
                                 const css::uno::Any &aFileName,
                                 const css::uno::Any &bRouteWorkbook ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL Protect( const css::uno::Any &aPassword ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL Unprotect( const css::uno::Any &aPassword ) throw (css::uno::RuntimeException);
    virtual void SAL_CALL Save() throw (css::uno::RuntimeException);
    virtual void SAL_CALL Activate() throw (css::uno::RuntimeException);

    // XHelperInterface
    virtual rtl::OUString getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif /* VBA_DOCUMENTBASE_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
