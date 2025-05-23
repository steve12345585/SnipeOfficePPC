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

#ifndef _CONNECTIVITY_DBASE_INDEXCOLUMNS_HXX_
#define _CONNECTIVITY_DBASE_INDEXCOLUMNS_HXX_

#include "connectivity/sdbcx/VCollection.hxx"
#include "dbase/DIndex.hxx"
#include "dbase/DTable.hxx"

namespace connectivity
{
    namespace dbase
    {
        class ODbaseIndexColumns : public sdbcx::OCollection
        {
            ODbaseIndex* m_pIndex;
        protected:
            virtual sdbcx::ObjectType createObject(const ::rtl::OUString& _rName);
            virtual void impl_refresh() throw(::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createDescriptor();
            virtual sdbcx::ObjectType appendObject( const ::rtl::OUString& _rForName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor );
        public:
            ODbaseIndexColumns( ODbaseIndex* _pIndex,
                                ::osl::Mutex& _rMutex,
                                const TStringVector &_rVector)
                        : sdbcx::OCollection(*_pIndex,_pIndex->getTable()->getConnection()->getMetaData()->supportsMixedCaseQuotedIdentifiers(),_rMutex,_rVector)
                        , m_pIndex(_pIndex)
            {}

        };
    }
}
#endif // _CONNECTIVITY_DBASE_INDEXCOLUMNS_HXX_


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
