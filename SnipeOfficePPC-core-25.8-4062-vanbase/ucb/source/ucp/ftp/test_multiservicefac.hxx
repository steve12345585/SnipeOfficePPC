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

/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/
#ifndef _TEST_MULTISERVICEFAC_HXX_
#define _TEST_MULTISERVICEFAC_HXX_

#include <cppuhelper/weak.hxx>
#include <cppuhelper/queryinterface.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>


namespace test_ftp {

    class Test_MultiServiceFactory
        : public cppu::OWeakObject,
          public com::sun::star::lang::XMultiServiceFactory
    {
    public:

        // XInterface

        virtual com::sun::star::uno::Any SAL_CALL
        queryInterface( const com::sun::star::uno::Type& rType )
            throw( com::sun::star::uno::RuntimeException );


        virtual void SAL_CALL acquire( void ) throw();

        virtual void SAL_CALL release( void ) throw();

        // XMultiServiceFactory

        virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::uno::XInterface > SAL_CALL
        createInstance(
            const ::rtl::OUString& aServiceSpecifier
        )
            throw (
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException
            );

        virtual
        ::com::sun::star::uno::Reference<
        ::com::sun::star::uno::XInterface > SAL_CALL
        createInstanceWithArguments(
            const ::rtl::OUString& ServiceSpecifier,
            const ::com::sun::star::uno::Sequence
            < ::com::sun::star::uno::Any >& Arguments
        )
            throw (
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException
            );

        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        getAvailableServiceNames(
        )
            throw (
                ::com::sun::star::uno::RuntimeException
            );
    };

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
