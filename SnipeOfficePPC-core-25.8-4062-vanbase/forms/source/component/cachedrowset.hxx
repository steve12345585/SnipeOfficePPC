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

#ifndef CACHEDROWSET_HXX
#define CACHEDROWSET_HXX

#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/sdbc/XConnection.hpp>

#include <comphelper/componentcontext.hxx>
#include <unotools/sharedunocomponent.hxx>

#include <memory>

//........................................................................
namespace frm
{
//........................................................................

    struct CachedRowSet_Data;
    //====================================================================
    //= CachedRowSet
    //====================================================================
    /** caches a result set obtained from a SQL statement
    */
    class CachedRowSet
    {
    public:
        CachedRowSet( const ::comphelper::ComponentContext& _rContext );
        ~CachedRowSet();

    public:
        /** executes the statement

            @return
                the result set produced by the statement. The caller takes ownership of the
                given object.

            @throws ::com::sun::star::sdbc::SQLException
                if such an exception is thrown when executing the statement
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet >
                execute();

        /// determines whether the row set properties are dirty, i.e. have changed since the last call to execute
        bool    isDirty() const;

        /// disposes the instance and frees all associated resources
        void    dispose();

        /** sets the command of a query as command to be executed

            A connection must have been set before.

            @throws Exception
        */
        void    setCommandFromQuery ( const ::rtl::OUString& _rQueryName );

        void    setCommand          ( const ::rtl::OUString& _rCommand );
        void    setEscapeProcessing ( const sal_Bool _bEscapeProcessing );
        void    setConnection       ( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection );

    private:
        ::std::auto_ptr< CachedRowSet_Data >    m_pData;
    };

//........................................................................
} // namespace frm
//........................................................................

#endif // CACHEDROWSET_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
