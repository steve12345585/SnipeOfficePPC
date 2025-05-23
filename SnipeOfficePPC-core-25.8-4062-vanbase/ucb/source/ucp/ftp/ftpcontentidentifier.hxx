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
#ifndef _FTP_FTPCONTENTIDENTIFIER_HXX_
#define _FTP_FTPCONTENTIDENTIFIER_HXX_

#include <vector>
#include "curl.hxx"
#include <curl/easy.h>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/queryinterface.hxx>
#include <com/sun/star/ucb/XContentIdentifier.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>

#include "ftpdirp.hxx"
#include "ftpurl.hxx"


namespace ftp {

    class FTPContentIdentifier
        : public cppu::OWeakObject,
          public com::sun::star::lang::XTypeProvider,
          public com::sun::star::ucb::XContentIdentifier
    {
    public:

        FTPContentIdentifier(const rtl::OUString& ident);

        ~FTPContentIdentifier();

        // XInterface

        virtual com::sun::star::uno::Any SAL_CALL
        queryInterface( const com::sun::star::uno::Type& rType )
            throw( com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL acquire( void ) throw();

        virtual void SAL_CALL release( void ) throw();

        // XTypeProvider

        virtual
        com::sun::star::uno::Sequence<com::sun::star::uno::Type> SAL_CALL
        getTypes(
        )
            throw(
                com::sun::star::uno::RuntimeException
            );

        virtual com::sun::star::uno::Sequence<sal_Int8> SAL_CALL
        getImplementationId(
        )
            throw(
                com::sun::star::uno::RuntimeException
            );


        // XContentIdentifier

        virtual ::rtl::OUString SAL_CALL
        getContentIdentifier(
        )
            throw (
                com::sun::star::uno::RuntimeException
            );

        virtual ::rtl::OUString SAL_CALL
        getContentProviderScheme(
        )
            throw (
                ::com::sun::star::uno::RuntimeException
            );


    private:

        rtl::OUString m_ident;
    };

}


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
