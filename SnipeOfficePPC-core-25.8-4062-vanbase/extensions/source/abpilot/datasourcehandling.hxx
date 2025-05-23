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

#ifndef EXTENSIONS_ABP_DATASOURCEHANDLING_HXX
#define EXTENSIONS_ABP_DATASOURCEHANDLING_HXX

#include <com/sun/star/uno/Reference.hxx>
#include "abptypes.hxx"

//========================================================================
namespace com { namespace sun { namespace star {
    namespace lang {
        class XMultiServiceFactory;
    }
    namespace beans {
        class XPropertySet;
    }
} } }

class Window;


//.........................................................................
namespace abp
{
//.........................................................................

    //=====================================================================
    //= ODataSourceContext
    //=====================================================================
    struct ODataSourceContextImpl;
    class ODataSource;
    /// a non-UNO wrapper for the data source context
    class ODataSourceContext
    {
    private:
        ODataSourceContextImpl*     m_pImpl;

    public:
        ODataSourceContext(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB
        );

        /// retrieves the names of all data sources
        void    getDataSourceNames( StringBag& _rNames ) const SAL_THROW (( ));

        /// disambiguates the given name by appending auccessive numbers
        ::rtl::OUString& disambiguate(::rtl::OUString& _rDataSourceName);

        /// creates a new MORK data source
        ODataSource createNewMORK( const ::rtl::OUString& _rName ) SAL_THROW (( ));

        /// creates a new Thunderbird data source
        ODataSource createNewThunderbird( const ::rtl::OUString& _rName ) SAL_THROW (( ));

        /// creates a new Evolution local data source
        ODataSource createNewEvolution( const ::rtl::OUString& _rName ) SAL_THROW (( ));

        /// creates a new Evolution LDAP data source
        ODataSource createNewEvolutionLdap( const ::rtl::OUString& _rName ) SAL_THROW (( ));

        /// creates a new Evolution GROUPWISE data source
        ODataSource createNewEvolutionGroupwise( const ::rtl::OUString& _rName ) SAL_THROW (( ));

        /// creates a new KDE address book data source
        ODataSource createNewKab( const ::rtl::OUString& _rName ) SAL_THROW (( ));

        /// creates a new Mac OS X address book data source
        ODataSource createNewMacab( const ::rtl::OUString& _rName ) SAL_THROW (( ));

        /// creates a new LDAP data source
        ODataSource createNewLDAP( const ::rtl::OUString& _rName ) SAL_THROW (( ));

        /// creates a new Outlook data source
        ODataSource createNewOutlook( const ::rtl::OUString& _rName ) SAL_THROW (( ));

        /// creates a new Outlook express data source
        ODataSource createNewOE( const ::rtl::OUString& _rName ) SAL_THROW (( ));

        /// creates a new dBase data source
        ODataSource createNewDBase( const ::rtl::OUString& _rName ) SAL_THROW (( ));
    };

    //=====================================================================
    //= ODataSource
    //=====================================================================
    struct ODataSourceImpl;
    struct PackageAccessControl;
    /** a non-UNO wrapper for a data source
        <p>This class allows to access data sources without the need to compile the respective file with
        exception handling enabled (hopefully :).</p>
        <p>In addition to wrapping an UNO data source, an instance of this class can handle at most
        one valid connection, as obtained from the data source.</p>
    */
    class ODataSource
    {
    private:
        ODataSourceImpl*    m_pImpl;

    public:
        // ----------------------------------------------------------------
        // - ctor/dtor/assignment
        // ----------------------------------------------------------------
        /// constructs an object which is initially invalid
        ODataSource(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB
        );

        /// copy ctor
        ODataSource( const ODataSource& _rSource );

        /// dtor
        ~ODataSource( );

        /// assignment
        ODataSource& operator=( const ODataSource& _rSource );

        // ----------------------------------------------------------------
        /// checks whether or not the object represents a valid data source
        sal_Bool    isValid() const SAL_THROW (( ));

        // ----------------------------------------------------------------
        /// removes the data source represented by the object from the data source context
        void        remove() SAL_THROW (( ));
            // TODO: put this into the context class

        /// returns the name of the data source
        ::rtl::OUString
                    getName() const SAL_THROW (( ));

        /// renames the data source
        sal_Bool    rename( const ::rtl::OUString& _rName ) SAL_THROW (( ));
            // TODO: put this into the context class

        // ----------------------------------------------------------------
        // - connection handling
        // ----------------------------------------------------------------
        /** connects to the data source represented by this object
            @param _pMessageParent
                the window to use as parent for any error messages. If this is <NULL/>, no messages are displayed
                at all.
            @see isConnected
        */
        sal_Bool    connect( Window* _pMessageParent ) SAL_THROW (( ));

        /// returns <TRUE/> if the object has a valid connection, obtained from it's data source
        sal_Bool    isConnected( ) const SAL_THROW (( ));

        /// disconnects from the data source (i.e. disposes the UNO connection hold internally)
        void        disconnect( ) SAL_THROW (( ));

        /// stores the database file
        void        store() SAL_THROW (( ));

        /// register the data source under the given name in the configuration
        void        registerDataSource( const ::rtl::OUString& _sRegisteredDataSourceName )  SAL_THROW (( ));

        // ----------------------------------------------------------------
        /** retrieves the tables names from the connection
            <p>to be called when <method>isConnection</method> returns <TRUE/> only</p>
        */
        const StringBag&    getTableNames() const SAL_THROW (( ));

        /** determines whether a given table exists
        */
        bool    hasTable( const ::rtl::OUString& _rTableName ) const;

        /// return the intern data source object
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > getDataSource() const SAL_THROW (( ));


        // ----------------------------------------------------------------
        /** set a new data source.
            <p>Available to selected clients only</p>
        */
        void        setDataSource(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxDS
            ,const ::rtl::OUString& _sName
            ,PackageAccessControl
        );

    private:
        ODataSource( ); // never implemented
    };

//.........................................................................
}   // namespace abp
//.........................................................................

#endif // EXTENSIONS_ABP_DATASOURCEHANDLING_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
