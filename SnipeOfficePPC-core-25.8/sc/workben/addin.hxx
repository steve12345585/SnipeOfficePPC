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

#ifndef SC_ADDIN_HXX
#define SC_ADDIN_HXX

#include <com/sun/star/sheet/XAddIn.hpp>
#include <com/sun/star/lang/XServiceName.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <stardiv/starcalc/test/XTestAddIn.hpp>

#include <cppuhelper/implbase4.hxx> // helper for implementations


com::sun::star::uno::Reference<com::sun::star::uno::XInterface> ScTestAddIn_CreateInstance(
    const com::sun::star::uno::Reference<com::sun::star::lang::XMultiServiceFactory>& );


class ScTestAddIn : public cppu::WeakImplHelper4<
                        com::sun::star::sheet::XAddIn,
                        stardiv::starcalc::test::XTestAddIn,
                        com::sun::star::lang::XServiceName,
                        com::sun::star::lang::XServiceInfo >
{
private:
    com::sun::star::uno::Reference<com::sun::star::sheet::XVolatileResult>  xAlphaResult;   //! Test
    com::sun::star::uno::Reference<com::sun::star::sheet::XVolatileResult>  xNumResult;     //! Test
    com::sun::star::lang::Locale                                            aFuncLoc;

public:
                            ScTestAddIn();
    virtual                 ~ScTestAddIn();

//                          SMART_UNO_DECLARATION( ScTestAddIn, UsrObject );
//  friend Reflection *     ScTestAddIn_getReflection();
//  virtual BOOL            queryInterface( Uik, XInterfaceRef& );
//  virtual XIdlClassRef    getIdlClass(void);

    static UString          getImplementationName_Static();
    static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static();

                            // XAddIn
    virtual ::rtl::OUString SAL_CALL getProgrammaticFuntionName( const ::rtl::OUString& aDisplayName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getDisplayFunctionName( const ::rtl::OUString& aProgrammaticName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getFunctionDescription( const ::rtl::OUString& aProgrammaticName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getDisplayArgumentName( const ::rtl::OUString& aProgrammaticFunctionName, sal_Int32 nArgument ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getArgumentDescription( const ::rtl::OUString& aProgrammaticFunctionName, sal_Int32 nArgument ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getProgrammaticCategoryName( const ::rtl::OUString& aProgrammaticFunctionName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getDisplayCategoryName( const ::rtl::OUString& aProgrammaticFunctionName ) throw(::com::sun::star::uno::RuntimeException);

                            // XLocalizable
    virtual void SAL_CALL setLocale( const ::com::sun::star::lang::Locale& eLocale ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::lang::Locale SAL_CALL getLocale(  ) throw(::com::sun::star::uno::RuntimeException);

                            // XTestAddIn
    virtual sal_Int32 SAL_CALL countParams( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArgs ) throw(::com::sun::star::uno::RuntimeException);
    virtual double SAL_CALL addOne( double fValue ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL repeatStr( const ::rtl::OUString& aStr, sal_Int32 nCount ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getDateString( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xCaller, double fValue ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getColorValue( const ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange >& xRange ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > > SAL_CALL transpose( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > >& aMatrix ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< sal_Int32 > > SAL_CALL transposeInt( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< sal_Int32 > >& aMatrix ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XVolatileResult > SAL_CALL callAsync( const ::rtl::OUString& aString ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL repeatMultiple( sal_Int32 nCount, const ::com::sun::star::uno::Any& aFirst, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aFollow ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getStrOrVal( sal_Int32 nFlag ) throw(::com::sun::star::uno::RuntimeException);

                            // XServiceName
    virtual ::rtl::OUString SAL_CALL getServiceName(  ) throw(::com::sun::star::uno::RuntimeException);

                            // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
