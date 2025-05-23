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

#ifndef _CONTENT_RESULTSET_WRAPPER_HXX
#define _CONTENT_RESULTSET_WRAPPER_HXX

#include <rtl/ustring.hxx>
#include <ucbhelper/macros.hxx>
#include <osl/mutex.hxx>
#include <cppuhelper/weak.hxx>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/sdbc/XCloseable.hpp>
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/ucb/XContentAccess.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <cppuhelper/interfacecontainer.hxx>

//=========================================================================

class ContentResultSetWrapperListener;
class ContentResultSetWrapper
                : public cppu::OWeakObject
                , public com::sun::star::lang::XComponent
                , public com::sun::star::sdbc::XCloseable
                , public com::sun::star::sdbc::XResultSetMetaDataSupplier
                , public com::sun::star::beans::XPropertySet
                , public com::sun::star::ucb::XContentAccess
                , public com::sun::star::sdbc::XResultSet
                , public com::sun::star::sdbc::XRow
{
protected:

    //--------------------------------------------------------------------------
    //class PropertyChangeListenerContainer_Impl.

    struct equalStr_Impl
    {
            bool operator()( const rtl::OUString& s1, const rtl::OUString& s2 ) const
          {
            return !!( s1 == s2 );
        }
    };

    struct hashStr_Impl
    {
        size_t operator()( const rtl::OUString& rName ) const
        {
            return rName.hashCode();
        }
    };

    typedef cppu::OMultiTypeInterfaceContainerHelperVar
    < rtl::OUString , hashStr_Impl , equalStr_Impl >
    PropertyChangeListenerContainer_Impl;
    //--------------------------------------------------------------------------
    // class ReacquireableGuard

    class ReacquireableGuard
    {
    protected:
        osl::Mutex* pT;
    public:

        ReacquireableGuard(osl::Mutex * t) : pT(t)
        {
            pT->acquire();
        }

        ReacquireableGuard(osl::Mutex& t) : pT(&t)
        {
            pT->acquire();
        }

        /** Releases mutex. */
        ~ReacquireableGuard()
        {
            if (pT)
                pT->release();
        }

        /** Releases mutex. */
        void clear()
        {
            if(pT)
            {
                pT->release();
                pT = NULL;
            }
        }

        /** Reacquire mutex. */
        void reacquire()
        {
            if(pT)
            {
                pT->acquire();
            }
        }
    };

    //-----------------------------------------------------------------
    //members

    //my Mutex
    osl::Mutex              m_aMutex;

    //different Interfaces from Origin:
    com::sun::star::uno::Reference< com::sun::star::sdbc::XResultSet >
                            m_xResultSetOrigin;
    com::sun::star::uno::Reference< com::sun::star::sdbc::XRow >
                            m_xRowOrigin; //XRow-interface from m_xOrigin
                            //!! call impl_init_xRowOrigin() bevor you access this member
    com::sun::star::uno::Reference< com::sun::star::ucb::XContentAccess >
                            m_xContentAccessOrigin; //XContentAccess-interface from m_xOrigin
                            //!! call impl_init_xContentAccessOrigin() bevor you access this member
    com::sun::star::uno::Reference< com::sun::star::beans::XPropertySet >
                            m_xPropertySetOrigin; //XPropertySet-interface from m_xOrigin
                            //!! call impl_init_xPropertySetOrigin() bevor you access this member

    com::sun::star::uno::Reference< com::sun::star::beans::XPropertySetInfo >
                            m_xPropertySetInfo;
                            //call impl_initPropertySetInfo() bevor you access this member

    sal_Int32               m_nForwardOnly;

private:
    com::sun::star::uno::Reference< com::sun::star::beans::XPropertyChangeListener >
                            m_xMyListenerImpl;
    ContentResultSetWrapperListener*
                            m_pMyListenerImpl;

    com::sun::star::uno::Reference< com::sun::star::sdbc::XResultSetMetaData >
                            m_xMetaDataFromOrigin; //XResultSetMetaData from m_xOrigin

    //management of listeners
    sal_Bool                m_bDisposed; ///Dispose call ready.
    sal_Bool                m_bInDispose;///In dispose call
    osl::Mutex              m_aContainerMutex;
    cppu::OInterfaceContainerHelper*
                            m_pDisposeEventListeners;
    PropertyChangeListenerContainer_Impl*
                            m_pPropertyChangeListeners;
    PropertyChangeListenerContainer_Impl*
                            m_pVetoableChangeListeners;

    //-----------------------------------------------------------------
    //methods:
private:
    PropertyChangeListenerContainer_Impl* SAL_CALL
    impl_getPropertyChangeListenerContainer();

    PropertyChangeListenerContainer_Impl* SAL_CALL
    impl_getVetoableChangeListenerContainer();

protected:
    //-----------------------------------------------------------------

    ContentResultSetWrapper( com::sun::star::uno::Reference<
                        com::sun::star::sdbc::XResultSet > xOrigin );

    virtual ~ContentResultSetWrapper();

    void SAL_CALL impl_init();
    void SAL_CALL impl_deinit();

    //--

    void SAL_CALL impl_init_xRowOrigin();
    void SAL_CALL impl_init_xContentAccessOrigin();
    void SAL_CALL impl_init_xPropertySetOrigin();

    //--

    virtual void SAL_CALL impl_initPropertySetInfo(); //helping XPropertySet

    void SAL_CALL
    impl_EnsureNotDisposed()
        throw( com::sun::star::lang::DisposedException,
               com::sun::star::uno::RuntimeException );

    void SAL_CALL
    impl_notifyPropertyChangeListeners(
            const com::sun::star::beans::PropertyChangeEvent& rEvt );

    void SAL_CALL
    impl_notifyVetoableChangeListeners(
            const com::sun::star::beans::PropertyChangeEvent& rEvt )
            throw( com::sun::star::beans::PropertyVetoException,
                   com::sun::star::uno::RuntimeException );

    sal_Bool SAL_CALL impl_isForwardOnly();

public:

    //-----------------------------------------------------------------
    // XInterface
    //-----------------------------------------------------------------
    virtual com::sun::star::uno::Any SAL_CALL
    queryInterface( const com::sun::star::uno::Type & rType )
        throw( com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    // XComponent
    //-----------------------------------------------------------------
    virtual void SAL_CALL
    dispose() throw( com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
    addEventListener( const com::sun::star::uno::Reference<
        com::sun::star::lang::XEventListener >& Listener )
            throw( com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
    removeEventListener( const com::sun::star::uno::Reference<
        com::sun::star::lang::XEventListener >& Listener )
            throw( com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    //XCloseable
    //-----------------------------------------------------------------
    virtual void SAL_CALL
    close()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    //XResultSetMetaDataSupplier
    //-----------------------------------------------------------------
    virtual com::sun::star::uno::Reference<
                com::sun::star::sdbc::XResultSetMetaData > SAL_CALL
    getMetaData()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    // XPropertySet
    //-----------------------------------------------------------------
    virtual com::sun::star::uno::Reference<
                com::sun::star::beans::XPropertySetInfo > SAL_CALL
    getPropertySetInfo()
        throw( com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
    setPropertyValue( const rtl::OUString& aPropertyName,
                      const com::sun::star::uno::Any& aValue )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::beans::PropertyVetoException,
               com::sun::star::lang::IllegalArgumentException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Any SAL_CALL
    getPropertyValue( const rtl::OUString& PropertyName )
        throw( com::sun::star::beans::UnknownPropertyException,
        com::sun::star::lang::WrappedTargetException,
        com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
    addPropertyChangeListener( const rtl::OUString& aPropertyName,
                               const com::sun::star::uno::Reference<
                                       com::sun::star::beans::XPropertyChangeListener >& xListener )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
    removePropertyChangeListener( const rtl::OUString& aPropertyName,
                                  const com::sun::star::uno::Reference<
                                      com::sun::star::beans::XPropertyChangeListener >& aListener )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
    addVetoableChangeListener( const rtl::OUString& PropertyName,
                               const com::sun::star::uno::Reference<
                                       com::sun::star::beans::XVetoableChangeListener >& aListener )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
    removeVetoableChangeListener( const rtl::OUString& PropertyName,
                                  const com::sun::star::uno::Reference<
                                      com::sun::star::beans::XVetoableChangeListener >& aListener )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    // own methods
    //-----------------------------------------------------------------
    virtual void SAL_CALL
        impl_disposing( const com::sun::star::lang::EventObject& Source )
        throw( com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
    impl_propertyChange( const com::sun::star::beans::PropertyChangeEvent& evt )
        throw( com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
    impl_vetoableChange( const com::sun::star::beans::PropertyChangeEvent& aEvent )
        throw( com::sun::star::beans::PropertyVetoException,
               com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    // XContentAccess
    //-----------------------------------------------------------------
    virtual rtl::OUString SAL_CALL
    queryContentIdentifierString()
        throw( com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier > SAL_CALL
    queryContentIdentifier()
        throw( com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XContent > SAL_CALL
    queryContent()
        throw( com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    // XResultSet
    //-----------------------------------------------------------------
    virtual sal_Bool SAL_CALL
    next()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    isBeforeFirst()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    isAfterLast()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    isFirst()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    isLast()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL
    beforeFirst()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL
    afterLast()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    first()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    last()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Int32 SAL_CALL
    getRow()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    absolute( sal_Int32 row )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    relative( sal_Int32 rows )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    previous()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL
    refreshRow()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    rowUpdated()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    rowInserted()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL
    rowDeleted()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Reference<
                com::sun::star::uno::XInterface > SAL_CALL
    getStatement()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    // XRow
    //-----------------------------------------------------------------
    virtual sal_Bool SAL_CALL
    wasNull()
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual rtl::OUString SAL_CALL
    getString( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual sal_Bool SAL_CALL
    getBoolean( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual sal_Int8 SAL_CALL
    getByte( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual sal_Int16 SAL_CALL
    getShort( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual sal_Int32 SAL_CALL
    getInt( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual sal_Int64 SAL_CALL
    getLong( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual float SAL_CALL
    getFloat( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual double SAL_CALL
    getDouble( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL
    getBytes( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::util::Date SAL_CALL
    getDate( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::util::Time SAL_CALL
    getTime( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::util::DateTime SAL_CALL
    getTimestamp( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Reference<
                com::sun::star::io::XInputStream > SAL_CALL
    getBinaryStream( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Reference<
                com::sun::star::io::XInputStream > SAL_CALL
    getCharacterStream( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Any SAL_CALL
    getObject( sal_Int32 columnIndex,
               const com::sun::star::uno::Reference<
                   com::sun::star::container::XNameAccess >& typeMap )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Reference<
                com::sun::star::sdbc::XRef > SAL_CALL
    getRef( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Reference<
                com::sun::star::sdbc::XBlob > SAL_CALL
    getBlob( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Reference<
                com::sun::star::sdbc::XClob > SAL_CALL
    getClob( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Reference<
                com::sun::star::sdbc::XArray > SAL_CALL
    getArray( sal_Int32 columnIndex )
        throw( com::sun::star::sdbc::SQLException,
               com::sun::star::uno::RuntimeException );
};

//=========================================================================

class ContentResultSetWrapperListener
        : public cppu::OWeakObject
        , public com::sun::star::beans::XPropertyChangeListener
        , public com::sun::star::beans::XVetoableChangeListener
{
protected:
    ContentResultSetWrapper*    m_pOwner;

public:
    ContentResultSetWrapperListener( ContentResultSetWrapper* pOwner );

    virtual ~ContentResultSetWrapperListener();

    //-----------------------------------------------------------------
    // XInterface
    //-----------------------------------------------------------------
    XINTERFACE_DECL()

    //-----------------------------------------------------------------
    //XEventListener
    //-----------------------------------------------------------------
    virtual void SAL_CALL
        disposing( const com::sun::star::lang::EventObject& Source )
        throw( com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    //XPropertyChangeListener
    //-----------------------------------------------------------------
    virtual void SAL_CALL
    propertyChange( const com::sun::star::beans::PropertyChangeEvent& evt )
        throw( com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    //XVetoableChangeListener
    //-----------------------------------------------------------------
    virtual void SAL_CALL
    vetoableChange( const com::sun::star::beans::PropertyChangeEvent& aEvent )
        throw( com::sun::star::beans::PropertyVetoException,
               com::sun::star::uno::RuntimeException );

    //-----------------------------------------------------------------
    // own methods:
    void SAL_CALL impl_OwnerDies();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
