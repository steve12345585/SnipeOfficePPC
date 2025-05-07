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
#ifndef _I18N_NATIVENUMBERSUPPLIER_HXX_
#define _I18N_NATIVENUMBERSUPPLIER_HXX_

#include <com/sun/star/i18n/XNativeNumberSupplier.hpp>
#include <com/sun/star/i18n/NativeNumberMode.hpp>
#include <com/sun/star/i18n/NativeNumberXmlAttributes.hpp>
#include <cppuhelper/implbase2.hxx> // helper for implementations
#include <com/sun/star/lang/XServiceInfo.hpp>

namespace com { namespace sun { namespace star { namespace i18n {

//      ----------------------------------------------------
//      class NativeNumberSupplier
//      ----------------------------------------------------
class NativeNumberSupplier : public cppu::WeakImplHelper2
<
        com::sun::star::i18n::XNativeNumberSupplier,
        com::sun::star::lang::XServiceInfo
>
{
public:
        NativeNumberSupplier(sal_Bool _useOffset = sal_False) : useOffset(_useOffset) {}

        // Methods
        virtual ::rtl::OUString SAL_CALL getNativeNumberString( const ::rtl::OUString& aNumberString,
                const ::com::sun::star::lang::Locale& aLocale, sal_Int16 nNativeNumberMode )
                throw (::com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL isValidNatNum( const ::com::sun::star::lang::Locale& aLocale,
                sal_Int16 nNativeNumberMode )
                throw (::com::sun::star::uno::RuntimeException);

        virtual ::com::sun::star::i18n::NativeNumberXmlAttributes SAL_CALL convertToXmlAttributes(
                const ::com::sun::star::lang::Locale& aLocale, sal_Int16 nNativeNumberMode )
                throw (::com::sun::star::uno::RuntimeException);

        virtual sal_Int16 SAL_CALL convertFromXmlAttributes(
                const ::com::sun::star::i18n::NativeNumberXmlAttributes& aAttr )
                throw (::com::sun::star::uno::RuntimeException);

        //XServiceInfo
        virtual rtl::OUString SAL_CALL getImplementationName()
                throw( com::sun::star::uno::RuntimeException );
        virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName)
                throw( com::sun::star::uno::RuntimeException );
        virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames()
                throw( com::sun::star::uno::RuntimeException );

        // following methods are not for XNativeNumberSupplier, they are for calling from transliterations
        ::rtl::OUString SAL_CALL getNativeNumberString( const ::rtl::OUString& aNumberString,
                const ::com::sun::star::lang::Locale& aLocale, sal_Int16 nNativeNumberMode,
                com::sun::star::uno::Sequence< sal_Int32 >& offset  )
                throw (::com::sun::star::uno::RuntimeException);
        sal_Unicode SAL_CALL getNativeNumberChar( const sal_Unicode inChar,
                const ::com::sun::star::lang::Locale& aLocale, sal_Int16 nNativeNumberMode )
                throw(com::sun::star::uno::RuntimeException) ;

private:
        ::com::sun::star::lang::Locale aLocale;
        sal_Bool useOffset;
};

} } } }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
