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


//--------------------------------------------------------------------------
#include "odbc/OFunctions.hxx"
#include <osl/process.h>

// Implib-Definitionen fuer ODBC-DLL/shared library:

namespace connectivity
{
    T3SQLAllocHandle pODBC3SQLAllocHandle;
T3SQLConnect pODBC3SQLConnect;
T3SQLDriverConnect pODBC3SQLDriverConnect;
T3SQLBrowseConnect pODBC3SQLBrowseConnect;
T3SQLDataSources pODBC3SQLDataSources;
T3SQLDrivers pODBC3SQLDrivers;
T3SQLGetInfo pODBC3SQLGetInfo;
T3SQLGetFunctions pODBC3SQLGetFunctions;
T3SQLGetTypeInfo pODBC3SQLGetTypeInfo;
T3SQLSetConnectAttr pODBC3SQLSetConnectAttr;
T3SQLGetConnectAttr pODBC3SQLGetConnectAttr;
T3SQLSetEnvAttr pODBC3SQLSetEnvAttr;
T3SQLGetEnvAttr pODBC3SQLGetEnvAttr;
T3SQLSetStmtAttr pODBC3SQLSetStmtAttr;
T3SQLGetStmtAttr pODBC3SQLGetStmtAttr;
T3SQLPrepare pODBC3SQLPrepare;
T3SQLBindParameter pODBC3SQLBindParameter;
T3SQLSetCursorName pODBC3SQLSetCursorName;
T3SQLExecute pODBC3SQLExecute;
T3SQLExecDirect pODBC3SQLExecDirect;
T3SQLDescribeParam pODBC3SQLDescribeParam;
T3SQLNumParams pODBC3SQLNumParams;
T3SQLParamData pODBC3SQLParamData;
T3SQLPutData pODBC3SQLPutData;
T3SQLRowCount pODBC3SQLRowCount;
T3SQLNumResultCols pODBC3SQLNumResultCols;
T3SQLDescribeCol pODBC3SQLDescribeCol;
T3SQLColAttribute pODBC3SQLColAttribute;
T3SQLBindCol pODBC3SQLBindCol;
T3SQLFetch pODBC3SQLFetch;
T3SQLFetchScroll pODBC3SQLFetchScroll;
T3SQLGetData pODBC3SQLGetData;
T3SQLSetPos pODBC3SQLSetPos;
T3SQLBulkOperations pODBC3SQLBulkOperations;
T3SQLMoreResults pODBC3SQLMoreResults;
T3SQLGetDiagRec pODBC3SQLGetDiagRec;
T3SQLColumnPrivileges pODBC3SQLColumnPrivileges;
T3SQLColumns pODBC3SQLColumns;
T3SQLForeignKeys pODBC3SQLForeignKeys;
T3SQLPrimaryKeys pODBC3SQLPrimaryKeys;
T3SQLProcedureColumns pODBC3SQLProcedureColumns;
T3SQLProcedures pODBC3SQLProcedures;
T3SQLSpecialColumns pODBC3SQLSpecialColumns;
T3SQLStatistics pODBC3SQLStatistics;
T3SQLTablePrivileges pODBC3SQLTablePrivileges;
T3SQLTables pODBC3SQLTables;
T3SQLFreeStmt pODBC3SQLFreeStmt;
T3SQLCloseCursor pODBC3SQLCloseCursor;
T3SQLCancel pODBC3SQLCancel;
T3SQLEndTran pODBC3SQLEndTran;
T3SQLDisconnect pODBC3SQLDisconnect;
T3SQLFreeHandle pODBC3SQLFreeHandle;
T3SQLGetCursorName pODBC3SQLGetCursorName;
T3SQLNativeSql pODBC3SQLNativeSql;

sal_Bool LoadFunctions(oslModule pODBCso);
// -------------------------------------------------------------------------
// Take care of Dynamicly loading of the DLL/shared lib and Addresses:
// Returns sal_True at success
sal_Bool LoadLibrary_ODBC3(::rtl::OUString &_rPath)
{
    static sal_Bool bLoaded = sal_False;
    static oslModule pODBCso = NULL;

    if (bLoaded)
        return sal_True;
#ifdef WNT
    _rPath = ::rtl::OUString("ODBC32.DLL");
#endif
#ifdef UNX
 #ifdef MACOSX
     _rPath = ::rtl::OUString("libiodbc.dylib");
 #else
    _rPath = ::rtl::OUString("libodbc.so.1");
    pODBCso = osl_loadModule( _rPath.pData,SAL_LOADMODULE_NOW );
    if ( !pODBCso )
        _rPath = ::rtl::OUString("libodbc.so");
 #endif   /* MACOSX */
#endif

    if ( !pODBCso )
        pODBCso = osl_loadModule( _rPath.pData,SAL_LOADMODULE_NOW );
    if( !pODBCso)
        return sal_False;

    return bLoaded = LoadFunctions(pODBCso);
}
// -------------------------------------------------------------------------

sal_Bool LoadFunctions(oslModule pODBCso)
{

    if( ( pODBC3SQLAllocHandle  =   (T3SQLAllocHandle)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLAllocHandle").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLConnect      =   (T3SQLConnect)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLConnect").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLDriverConnect =  (T3SQLDriverConnect)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLDriverConnect").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLBrowseConnect =   (T3SQLBrowseConnect)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLBrowseConnect").pData )) == NULL )
        return sal_False;
    if(( pODBC3SQLDataSources   =   (T3SQLDataSources)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLDataSources").pData )) == NULL )
        return sal_False;
    if(( pODBC3SQLDrivers       =   (T3SQLDrivers)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLDrivers").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLGetInfo      =   (T3SQLGetInfo)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLGetInfo").pData )) == NULL )
        return sal_False;
    if(( pODBC3SQLGetFunctions  =   (T3SQLGetFunctions)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLGetFunctions").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLGetTypeInfo  =   (T3SQLGetTypeInfo)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLGetTypeInfo").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLSetConnectAttr   =   (T3SQLSetConnectAttr)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLSetConnectAttr").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLGetConnectAttr   =   (T3SQLGetConnectAttr)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLGetConnectAttr").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLSetEnvAttr   =   (T3SQLSetEnvAttr)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLSetEnvAttr").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLGetEnvAttr   =   (T3SQLGetEnvAttr)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLGetEnvAttr").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLSetStmtAttr  =   (T3SQLSetStmtAttr)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLSetStmtAttr").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLGetStmtAttr  =   (T3SQLGetStmtAttr)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLGetStmtAttr").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLPrepare      =   (T3SQLPrepare)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLPrepare").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLBindParameter =  (T3SQLBindParameter)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLBindParameter").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLSetCursorName =  (T3SQLSetCursorName)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLSetCursorName").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLExecute      =   (T3SQLExecute)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLExecute").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLExecDirect   =   (T3SQLExecDirect)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLExecDirect").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLDescribeParam =   (T3SQLDescribeParam)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLDescribeParam").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLNumParams        =   (T3SQLNumParams)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLNumParams").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLParamData        =   (T3SQLParamData)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLParamData").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLPutData      =   (T3SQLPutData)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLPutData").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLRowCount     =   (T3SQLRowCount)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLRowCount").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLNumResultCols =  (T3SQLNumResultCols)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLNumResultCols").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLDescribeCol  =   (T3SQLDescribeCol)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLDescribeCol").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLColAttribute =   (T3SQLColAttribute)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLColAttribute").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLBindCol      =   (T3SQLBindCol)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLBindCol").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLFetch            =   (T3SQLFetch)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLFetch").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLFetchScroll  =   (T3SQLFetchScroll)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLFetchScroll").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLGetData      =   (T3SQLGetData)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLGetData").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLSetPos       =   (T3SQLSetPos)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLSetPos").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLBulkOperations   =   (T3SQLBulkOperations)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLBulkOperations").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLMoreResults  =   (T3SQLMoreResults)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLMoreResults").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLGetDiagRec   =   (T3SQLGetDiagRec)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLGetDiagRec").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLColumnPrivileges = (T3SQLColumnPrivileges)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLColumnPrivileges").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLColumns      =   (T3SQLColumns)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLColumns").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLForeignKeys  =   (T3SQLForeignKeys)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLForeignKeys").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLPrimaryKeys  =   (T3SQLPrimaryKeys)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLPrimaryKeys").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLProcedureColumns =  (T3SQLProcedureColumns)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLProcedureColumns").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLProcedures   =   (T3SQLProcedures)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLProcedures").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLSpecialColumns =  (T3SQLSpecialColumns)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLSpecialColumns").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLStatistics   =   (T3SQLStatistics)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLStatistics").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLTablePrivileges =    (T3SQLTablePrivileges)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLTablePrivileges").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLTables       =   (T3SQLTables)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLTables").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLFreeStmt     =   (T3SQLFreeStmt)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLFreeStmt").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLCloseCursor  =   (T3SQLCloseCursor)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLCloseCursor").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLCancel       =   (T3SQLCancel)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLCancel").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLEndTran      =   (T3SQLEndTran)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLEndTran").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLDisconnect   =   (T3SQLDisconnect)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLDisconnect").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLFreeHandle   =   (T3SQLFreeHandle)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLFreeHandle").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLGetCursorName    =   (T3SQLGetCursorName)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLGetCursorName").pData )) == NULL )
        return sal_False;
    if( ( pODBC3SQLNativeSql    =   (T3SQLNativeSql)osl_getFunctionSymbol(pODBCso, ::rtl::OUString("SQLNativeSql").pData )) == NULL )
        return sal_False;

    return sal_True;
}
// -------------------------------------------------------------------------

}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
