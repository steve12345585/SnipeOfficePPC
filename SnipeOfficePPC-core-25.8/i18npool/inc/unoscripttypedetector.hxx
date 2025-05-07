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
#ifndef _I18N_UNOSCRIPTTYPEDETECTOR_HXX_
#define _I18N_UNOSCRIPTTYPEDETECTOR_HXX_

#include <com/sun/star/lang/XServiceInfo.hpp>

#include <com/sun/star/i18n/XScriptTypeDetector.hpp>
#include <cppuhelper/implbase2.hxx> // helper for implementations

//  ----------------------------------------------------
//  class UnoScriptTypeDetector
//  ----------------------------------------------------
class UnoScriptTypeDetector : public cppu::WeakImplHelper2
<
    ::com::sun::star::i18n::XScriptTypeDetector,
    ::com::sun::star::lang::XServiceInfo
>
{
public:
    // Methods
    virtual sal_Int32 SAL_CALL beginOfScriptDirection( const ::rtl::OUString& Text, sal_Int32 nPos, sal_Int16 scriptDirection ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL endOfScriptDirection( const ::rtl::OUString& Text, sal_Int32 nPos, sal_Int16 scriptDirection ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int16 SAL_CALL getScriptDirection( const ::rtl::OUString& Text, sal_Int32 nPos, sal_Int16 defaultScriptDirection ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL beginOfCTLScriptType( const ::rtl::OUString& Text, sal_Int32 nPos ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL endOfCTLScriptType( const ::rtl::OUString& Text, sal_Int32 nPos ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int16 SAL_CALL getCTLScriptType( const ::rtl::OUString& Text, sal_Int32 nPos ) throw (::com::sun::star::uno::RuntimeException);


    //XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName(void)
                throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName)
                throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void)
                throw( ::com::sun::star::uno::RuntimeException );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
