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

#include "HelperCollections.hxx"

#include "dbastrings.hrc"

namespace dbaccess
{
    using namespace dbtools;
    using namespace comphelper;
    using namespace connectivity;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::sdbc;
    using namespace ::com::sun::star::sdb;
    using namespace ::com::sun::star::sdbcx;
    using namespace ::com::sun::star::container;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::script;
    using namespace ::cppu;
    using namespace ::osl;

    OPrivateColumns::OPrivateColumns(const ::rtl::Reference< ::connectivity::OSQLColumns>& _rColumns,
                        sal_Bool _bCase,
                        ::cppu::OWeakObject& _rParent,
                        ::osl::Mutex& _rMutex,
                        const ::std::vector< ::rtl::OUString> &_rVector,
                        sal_Bool _bUseAsIndex
                    ) : sdbcx::OCollection(_rParent,_bCase,_rMutex,_rVector,_bUseAsIndex)
                        ,m_aColumns(_rColumns)
    {
    }

    OPrivateColumns* OPrivateColumns::createWithIntrinsicNames( const ::rtl::Reference< ::connectivity::OSQLColumns >& _rColumns,
        sal_Bool _bCase, ::cppu::OWeakObject& _rParent, ::osl::Mutex& _rMutex )
    {
        ::std::vector< ::rtl::OUString > aNames; aNames.reserve( _rColumns->get().size() );

        ::rtl::OUString sColumName;
        for (   ::connectivity::OSQLColumns::Vector::const_iterator column = _rColumns->get().begin();
                column != _rColumns->get().end();
                ++column
            )
        {
            Reference< XPropertySet > xColumn( *column, UNO_QUERY_THROW );
            xColumn->getPropertyValue( PROPERTY_NAME ) >>= sColumName;
            aNames.push_back( sColumName );
        }
        return new OPrivateColumns( _rColumns, _bCase, _rParent, _rMutex, aNames, sal_False );
    }

    void SAL_CALL OPrivateColumns::disposing(void)
    {
        m_aColumns = NULL;
        clear_NoDispose();
            // we're not owner of the objects we're holding, instead the object we got in our ctor is
            // So we're not allowed to dispose our elements.
        OPrivateColumns_Base::disposing();
    }

    connectivity::sdbcx::ObjectType OPrivateColumns::createObject(const ::rtl::OUString& _rName)
    {
        if ( m_aColumns.is() )
        {
            ::connectivity::OSQLColumns::Vector::const_iterator aIter = find(m_aColumns->get().begin(),m_aColumns->get().end(),_rName,isCaseSensitive());
            if(aIter == m_aColumns->get().end())
                aIter = findRealName(m_aColumns->get().begin(),m_aColumns->get().end(),_rName,isCaseSensitive());

            if(aIter != m_aColumns->get().end())
                return connectivity::sdbcx::ObjectType(*aIter,UNO_QUERY);

            OSL_FAIL("Column not found in collection!");
        }
        return NULL;
    }

    connectivity::sdbcx::ObjectType OPrivateTables::createObject(const ::rtl::OUString& _rName)
    {
        if ( !m_aTables.empty() )
        {
            OSQLTables::iterator aIter = m_aTables.find(_rName);
            OSL_ENSURE(aIter != m_aTables.end(),"Table not found!");
            OSL_ENSURE(aIter->second.is(),"Table is null!");
            (void)aIter;
            return connectivity::sdbcx::ObjectType(m_aTables.find(_rName)->second,UNO_QUERY);
        }
        return NULL;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
