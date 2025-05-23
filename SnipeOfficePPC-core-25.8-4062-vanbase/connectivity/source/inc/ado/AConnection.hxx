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
#ifndef _CONNECTIVITY_ADO_ACONNECTION_HXX_
#define _CONNECTIVITY_ADO_ACONNECTION_HXX_

#include <com/sun/star/sdbc/SQLWarning.hpp>
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include "connectivity/OSubComponent.hxx"
#include <map>
#include "connectivity/CommonTools.hxx"
#include "OTypeInfo.hxx"
#include "TConnection.hxx"
#include "ado/Awrapado.hxx"

namespace connectivity
{
    namespace ado
    {
        struct OExtendedTypeInfo
        {
            ::connectivity::OTypeInfo       aSimpleType;    // the general type info
            DataTypeEnum                    eType;

            inline ::rtl::OUString getDBName() const { return aSimpleType.aTypeName; }
        };

        class WpADOConnection;
        class ODriver;
        class OCatalog;
        typedef ::std::multimap<DataTypeEnum, OExtendedTypeInfo*>       OTypeInfoMap;
        typedef connectivity::OMetaConnection                           OConnection_BASE;


        class OConnection : public OConnection_BASE,
                            public connectivity::OSubComponent<OConnection, OConnection_BASE>
        {
            friend class connectivity::OSubComponent<OConnection, OConnection_BASE>;

        protected:
            //====================================================================
            // Data attributes
            //====================================================================
            OTypeInfoMap                m_aTypeInfo;    //  vector containing an entry
                                                                                //  for each row returned by
                                                                                //  DatabaseMetaData.getTypeInfo.
            ::com::sun::star::uno::WeakReference< ::com::sun::star::sdbcx::XTablesSupplier>      m_xCatalog;
            ODriver*                    m_pDriver;
        private:
            WpADOConnection*            m_pAdoConnection;
            OCatalog*                   m_pCatalog;
            sal_Int32                   m_nEngineType;
            sal_Bool                    m_bClosed;
            sal_Bool                    m_bAutocommit;

        protected:
            void buildTypeInfo() throw( ::com::sun::star::sdbc::SQLException);
        public:

            OConnection(ODriver*        _pDriver) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            //  OConnection(const SQLHANDLE _pConnectionHandle);
            ~OConnection();
            void construct(const ::rtl::OUString& url,const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& info);

            void closeAllStatements () throw( ::com::sun::star::sdbc::SQLException);

            //XUnoTunnel
            virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw (::com::sun::star::uno::RuntimeException);
            static ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelImplementationId();
            // XServiceInfo
            DECLARE_SERVICE_INFO();
            // OComponentHelper
            virtual void SAL_CALL disposing(void);
            // XInterface
            virtual void SAL_CALL release() throw();

            // XConnection
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XStatement > SAL_CALL createStatement(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XPreparedStatement > SAL_CALL prepareStatement( const ::rtl::OUString& sql ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XPreparedStatement > SAL_CALL prepareCall( const ::rtl::OUString& sql ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::rtl::OUString SAL_CALL nativeSQL( const ::rtl::OUString& sql ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setAutoCommit( sal_Bool autoCommit ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Bool SAL_CALL getAutoCommit(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL commit(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL rollback(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Bool SAL_CALL isClosed(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData > SAL_CALL getMetaData(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setReadOnly( sal_Bool readOnly ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Bool SAL_CALL isReadOnly(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setCatalog( const ::rtl::OUString& catalog ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::rtl::OUString SAL_CALL getCatalog(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setTransactionIsolation( sal_Int32 level ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Int32 SAL_CALL getTransactionIsolation(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > SAL_CALL getTypeMap(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setTypeMap( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& typeMap ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            // XCloseable
            virtual void SAL_CALL close(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            // XWarningsSupplier
            virtual ::com::sun::star::uno::Any SAL_CALL getWarnings(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL clearWarnings(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            //
            WpADOConnection* getConnection() { return m_pAdoConnection; }
            void setCatalog(const ::com::sun::star::uno::WeakReference< ::com::sun::star::sdbcx::XTablesSupplier>& _xCat) { m_xCatalog = _xCat; }
            void setCatalog(OCatalog* _pCatalog) { m_pCatalog = _pCatalog; }

            const OTypeInfoMap* getTypeInfo() const { return &m_aTypeInfo;}
            OCatalog* getAdoCatalog() const
            {
                if ( m_xCatalog.get().is() )
                    return m_pCatalog;
                return NULL;
            }

            sal_Int32 getEngineType()   const { return m_nEngineType; }
            ODriver*  getDriver()       const { return m_pDriver; }

            static const OExtendedTypeInfo* getTypeInfoFromType(const OTypeInfoMap& _rTypeInfo,
                               DataTypeEnum _nType,
                               const ::rtl::OUString& _sTypeName,
                               sal_Int32 _nPrecision,
                               sal_Int32 _nScale,
                               sal_Bool& _brForceToType);
        };
    }
}
#endif // _CONNECTIVITY_ADO_ACONNECTION_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
