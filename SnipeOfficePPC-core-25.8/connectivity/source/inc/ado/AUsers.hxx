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

#ifndef _CONNECTIVITY_ADO_USERS_HXX_
#define _CONNECTIVITY_ADO_USERS_HXX_

#include "connectivity/sdbcx/VCollection.hxx"
#include "ado/Awrapadox.hxx"
#include "ado/ACatalog.hxx"

namespace connectivity
{
    namespace sdbcx
    {
        class IRefreshableUsers;
    }
    namespace ado
    {

        class OUsers : public sdbcx::OCollection
        {
            WpADOUsers  m_aCollection;
            OCatalog*   m_pCatalog;
        public:
            virtual sdbcx::ObjectType createObject(const ::rtl::OUString& _rName);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createDescriptor();
            virtual void impl_refresh() throw(::com::sun::star::uno::RuntimeException);
            virtual sdbcx::ObjectType appendObject( const ::rtl::OUString& _rForName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor );
            virtual void dropObject(sal_Int32 _nPos,const ::rtl::OUString _sElementName);
        public:
            OUsers( OCatalog* _pParent,
                    ::osl::Mutex& _rMutex,
                    const TStringVector &_rVector,
                    const WpADOUsers&   _rCollection,
                    sal_Bool _bCase)
                    :sdbcx::OCollection(*_pParent,_bCase,_rMutex,_rVector)
                    ,m_aCollection(_rCollection)
                    ,m_pCatalog(_pParent)
            {
            }
        };
    }
}

#endif // _CONNECTIVITY_ADO_USERS_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
