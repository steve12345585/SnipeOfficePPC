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

#ifndef CONNECTIVITY_SPREPAREDSTATEMENT_HXX
#define CONNECTIVITY_SPREPAREDSTATEMENT_HXX
#include "mysqlc_statement.hxx"
#include "mysqlc_resultset.hxx"

#include <com/sun/star/sdbc/XPreparedStatement.hpp>
#include <com/sun/star/sdbc/XParameters.hpp>
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#include <com/sun/star/sdbc/XPreparedBatchExecution.hpp>
#include <com/sun/star/io/XInputStream.hpp>

#include <cppconn/prepared_statement.h>

namespace connectivity
{
    namespace mysqlc
    {
        using ::com::sun::star::uno::Reference;
        using ::com::sun::star::uno::Any;
        using ::com::sun::star::uno::Type;
        using ::com::sun::star::sdbc::SQLException;
        using ::com::sun::star::uno::RuntimeException;
        using ::com::sun::star::sdbc::XResultSetMetaData;

        typedef ::cppu::ImplHelper5<    ::com::sun::star::sdbc::XPreparedStatement,
                                        ::com::sun::star::sdbc::XParameters,
                                        ::com::sun::star::sdbc::XPreparedBatchExecution,
                                        ::com::sun::star::sdbc::XResultSetMetaDataSupplier,
                                        ::com::sun::star::lang::XServiceInfo> OPreparedStatement_BASE;

        class OPreparedStatement :  public OCommonStatement,
                                    public OPreparedStatement_BASE
        {
        protected:
            unsigned int        m_paramCount;   // number of placeholders
            sal_Int32           m_nNumParams;   // Number of parameter markers for the prepared statement
            Reference< XResultSetMetaData > m_xMetaData;

            void checkParameterIndex(sal_Int32 parameter);

        protected:
            void SAL_CALL setFastPropertyValue_NoBroadcast(sal_Int32 nHandle, const Any& rValue)
                                                                                    throw (::com::sun::star::uno::Exception);
            virtual ~OPreparedStatement();
        public:
            DECLARE_SERVICE_INFO();
            OPreparedStatement(OConnection* _pConnection, sql::PreparedStatement * cppPrepStmt);

            //XInterface
            Any SAL_CALL queryInterface(const Type & rType) throw(RuntimeException);
            void SAL_CALL acquire() throw();
            void SAL_CALL release() throw();

            //XTypeProvider
            ::com::sun::star::uno::Sequence< Type > SAL_CALL getTypes()
                                                                                    throw(RuntimeException);

            // XPreparedStatement
            Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL executeQuery() throw(SQLException, RuntimeException);
            sal_Int32 SAL_CALL executeUpdate() throw(SQLException, RuntimeException);
            sal_Bool SAL_CALL execute() throw(SQLException, RuntimeException);
            Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnection() throw(SQLException, RuntimeException);

            // XStatement
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL executeQuery(const OUString& sql)
                                                throw(SQLException, RuntimeException);
            sal_Int32 SAL_CALL executeUpdate(const OUString& sql)
                                                throw(SQLException, RuntimeException);
            sal_Bool SAL_CALL execute( const OUString& sql )
                                                throw(SQLException, RuntimeException);

            // XParameters
            void SAL_CALL setNull(sal_Int32 parameter, sal_Int32 sqlType)           throw(SQLException, RuntimeException);

            void SAL_CALL setObjectNull(sal_Int32 parameter, sal_Int32 sqlType, const OUString& typeName)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL setBoolean(sal_Int32 parameter, sal_Bool x)               throw(SQLException, RuntimeException);

            void SAL_CALL setByte(sal_Int32 parameter, sal_Int8 x)                  throw(SQLException, RuntimeException);

            void SAL_CALL setShort(sal_Int32 parameter, sal_Int16 x)                throw(SQLException, RuntimeException);

            void SAL_CALL setInt(sal_Int32 parameter, sal_Int32 x)                  throw(SQLException, RuntimeException);

            void SAL_CALL setLong(sal_Int32 parameter, sal_Int64 x)                 throw(SQLException, RuntimeException);

            void SAL_CALL setFloat(sal_Int32 parameter, float x)                    throw(SQLException, RuntimeException);

            void SAL_CALL setDouble(sal_Int32 parameter, double x)                  throw(SQLException, RuntimeException);

            void SAL_CALL setString(sal_Int32 parameter, const OUString& x)         throw(SQLException, RuntimeException);

            void SAL_CALL setBytes(sal_Int32 parameter, const ::com::sun::star::uno::Sequence< sal_Int8 >& x)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL setDate(sal_Int32 parameter, const ::com::sun::star::util::Date& x)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL setTime(sal_Int32 parameter, const ::com::sun::star::util::Time& x)
                                                                                    throw(SQLException, RuntimeException);
            void SAL_CALL setTimestamp(sal_Int32 parameter, const ::com::sun::star::util::DateTime& x)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL setBinaryStream(sal_Int32 parameter, const Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL setCharacterStream(sal_Int32 parameter, const Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL setObject(sal_Int32 parameter, const Any& x)              throw(SQLException, RuntimeException);

            void SAL_CALL setObjectWithInfo(sal_Int32 parameter, const Any& x, sal_Int32 targetSqlType, sal_Int32 scale)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL setRef(sal_Int32 parameter, const Reference< ::com::sun::star::sdbc::XRef >& x)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL setBlob(sal_Int32 parameter, const Reference< ::com::sun::star::sdbc::XBlob >& x)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL setClob(sal_Int32 parameter, const Reference< ::com::sun::star::sdbc::XClob >& x)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL setArray(sal_Int32 parameter, const Reference< ::com::sun::star::sdbc::XArray >& x)
                                                                                    throw(SQLException, RuntimeException);

            void SAL_CALL clearParameters() throw(SQLException, RuntimeException);

            // XPreparedBatchExecution
            void SAL_CALL addBatch() throw(SQLException, RuntimeException);
            void SAL_CALL clearBatch() throw(SQLException, RuntimeException);
            ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL executeBatch()    throw(SQLException, RuntimeException);

            // XCloseable
            void SAL_CALL close() throw(SQLException, RuntimeException);

            // XResultSetMetaDataSupplier
            Reference< ::com::sun::star::sdbc::XResultSetMetaData > SAL_CALL getMetaData()
                                                                                    throw(SQLException, RuntimeException);
        };
    } /* mysqlc */
} /* connectivity */
#endif // CONNECTIVITY_SPREPAREDSTATEMENT_HXX

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
