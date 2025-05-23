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

#ifndef _HIERARCHYDATASUPPLIER_HXX
#define _HIERARCHYDATASUPPLIER_HXX

#include <rtl/ref.hxx>
#include <com/sun/star/ucb/OpenMode.hpp>
#include <ucbhelper/resultset.hxx>

namespace hierarchy_ucp {

class HierarchyEntryData;
struct DataSupplier_Impl;
class HierarchyContent;

class HierarchyResultSetDataSupplier :
        public ::ucbhelper::ResultSetDataSupplier
{
    DataSupplier_Impl*  m_pImpl;

private:
    sal_Bool checkResult( const HierarchyEntryData& rResult );

public:
    HierarchyResultSetDataSupplier(
                    const com::sun::star::uno::Reference<
                        com::sun::star::uno::XComponentContext >& rxContext,
                    const rtl::Reference< HierarchyContent >& rContent,
                    sal_Int32 nOpenMode = com::sun::star::ucb::OpenMode::ALL );
    virtual ~HierarchyResultSetDataSupplier();

    virtual rtl::OUString queryContentIdentifierString( sal_uInt32 nIndex );
    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >
    queryContentIdentifier( sal_uInt32 nIndex );
    virtual com::sun::star::uno::Reference< com::sun::star::ucb::XContent >
    queryContent( sal_uInt32 nIndex );

    virtual sal_Bool getResult( sal_uInt32 nIndex );

    virtual sal_uInt32 totalCount();
    virtual sal_uInt32 currentCount();
    virtual sal_Bool isCountFinal();

    virtual com::sun::star::uno::Reference< com::sun::star::sdbc::XRow >
    queryPropertyValues( sal_uInt32 nIndex  );
    virtual void releasePropertyValues( sal_uInt32 nIndex );

    virtual void close();

    virtual void validate()
        throw( com::sun::star::ucb::ResultSetException );
};

} // namespace hierarchy_ucp

#endif /* !_HIERARCHYDATASUPPLIER_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
