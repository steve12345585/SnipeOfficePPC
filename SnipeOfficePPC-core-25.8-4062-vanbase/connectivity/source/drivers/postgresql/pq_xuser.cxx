/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 *  Effective License of whole file:
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 *  Parts "Copyright by Sun Microsystems, Inc" prior to August 2011:
 *
 *    The Contents of this file are made available subject to the terms of
 *    the GNU Lesser General Public License Version 2.1
 *
 *    Copyright: 2000 by Sun Microsystems, Inc.
 *
 *    Contributor(s): Joerg Budischewski
 *
 *  All parts contributed on or after August 2011:
 *
 *    Version: MPL 1.1 / GPLv3+ / LGPLv2.1+
 *
 *    The contents of this file are subject to the Mozilla Public License Version
 *    1.1 (the "License"); you may not use this file except in compliance with
 *    the License or as specified alternatively below. You may obtain a copy of
 *    the License at http://www.mozilla.org/MPL/
 *
 *    Software distributed under the License is distributed on an "AS IS" basis,
 *    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *    for the specific language governing rights and limitations under the
 *    License.
 *
 *    Major Contributor(s):
 *    [ Copyright (C) 2011 Lionel Elie Mamane <lionel@mamane.lu> ]
 *
 *    All Rights Reserved.
 *
 *    For minor contributions see the git repository.
 *
 *    Alternatively, the contents of this file may be used under the terms of
 *    either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 *    the GNU Lesser General Public License Version 2.1 or later (the "LGPLv2.1+"),
 *    in which case the provisions of the GPLv3+ or the LGPLv2.1+ are applicable
 *    instead of those above.
 *
 ************************************************************************/

#include <rtl/ustrbuf.hxx>

#include <cppuhelper/typeprovider.hxx>
#include <cppuhelper/queryinterface.hxx>

#include <com/sun/star/beans/PropertyAttribute.hpp>

#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XParameters.hpp>

#include "pq_xuser.hxx"
#include "pq_tools.hxx"
#include "pq_statics.hxx"

using osl::MutexGuard;
using osl::Mutex;

using rtl::OUString;
using rtl::OUStringBuffer;

using com::sun::star::container::XNameAccess;
using com::sun::star::container::XIndexAccess;
using com::sun::star::container::ElementExistException;
using com::sun::star::container::NoSuchElementException;

using com::sun::star::uno::Reference;
using com::sun::star::uno::Exception;
using com::sun::star::uno::UNO_QUERY;
using com::sun::star::uno::XInterface;
using com::sun::star::uno::Sequence;
using com::sun::star::uno::Any;
using com::sun::star::uno::makeAny;
using com::sun::star::uno::Type;
using com::sun::star::uno::RuntimeException;

using com::sun::star::lang::IllegalArgumentException;
using com::sun::star::lang::IndexOutOfBoundsException;

using com::sun::star::beans::XPropertySetInfo;
using com::sun::star::beans::XFastPropertySet;
using com::sun::star::beans::XMultiPropertySet;
using com::sun::star::beans::XPropertySet;
using com::sun::star::beans::Property;

using com::sun::star::sdbc::XResultSet;
using com::sun::star::sdbc::XPreparedStatement;
using com::sun::star::sdbc::XStatement;
using com::sun::star::sdbc::XParameters;
using com::sun::star::sdbc::XRow;
using com::sun::star::sdbc::SQLException;

namespace pq_sdbc_driver
{

#define ASCII_STR(x) OUString( RTL_CONSTASCII_USTRINGPARAM( x ) )

User::User( const ::rtl::Reference< RefCountedMutex > & refMutex,
            const Reference< com::sun::star::sdbc::XConnection > & connection,
            ConnectionSettings *pSettings )
    : ReflectionBase(
        getStatics().refl.user.implName,
        getStatics().refl.user.serviceNames,
        refMutex,
        connection,
        pSettings,
        * getStatics().refl.user.pProps )
{}

Reference< XPropertySet > User::createDataDescriptor(  ) throw (RuntimeException)
{
    UserDescriptor * pUser = new UserDescriptor( m_refMutex, m_conn, m_pSettings );
    pUser->copyValuesFrom( this );

    return Reference< XPropertySet > ( pUser );
}


Sequence<Type > User::getTypes() throw( RuntimeException )
{
    static cppu::OTypeCollection *pCollection;
    if( ! pCollection )
    {
        MutexGuard guard( osl::Mutex::getGlobalMutex() );
        if( !pCollection )
        {
            static cppu::OTypeCollection collection(
                getCppuType( (Reference< com::sun::star::sdbcx::XUser> *) 0 ),
                ReflectionBase::getTypes());
            pCollection = &collection;
        }
    }
    return pCollection->getTypes();
}

Sequence< sal_Int8> User::getImplementationId() throw( RuntimeException )
{
    return getStatics().refl.user.implementationId;
}

Any User::queryInterface( const Type & reqType ) throw (RuntimeException)
{
    Any ret;

    ret = ReflectionBase::queryInterface( reqType );
    if( ! ret.hasValue() )
        ret = ::cppu::queryInterface(
            reqType,
            static_cast< com::sun::star::sdbcx::XUser * > ( this ) );
    return ret;
}


void User::changePassword(
    const ::rtl::OUString& oldPassword, const ::rtl::OUString& newPassword )
    throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException)
{
    (void) oldPassword;
    rtl::OUStringBuffer buf(128);
    buf.appendAscii( RTL_CONSTASCII_STRINGPARAM( "ALTER USER " ) );
    bufferQuoteIdentifier( buf, extractStringProperty( this, getStatics().NAME ), m_pSettings );
    buf.appendAscii( RTL_CONSTASCII_STRINGPARAM( " PASSWORD " ) );
    bufferQuoteConstant( buf, newPassword, m_pSettings );
    Reference< XStatement > stmt = m_conn->createStatement();
    DisposeGuard guard( stmt );
    stmt->executeUpdate( buf.makeStringAndClear() );
}

sal_Int32 User::getPrivileges( const ::rtl::OUString& objName, sal_Int32 objType )
    throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException)
{
    sal_Int32 ret = 0xffffffff;
    if( isLog( m_pSettings, LogLevel::INFO ) )
    {
        Statics & st = getStatics();
        rtl::OUString user = extractStringProperty( this, st.NAME );

        rtl::OUStringBuffer buf( 128 );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("User::getPrivileges[") );
        buf.append( extractStringProperty( this, st.NAME ) );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM( "] got called for " ) );
        buf.append( objName );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM( "(type=" ) );
        buf.append( objType );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM( ")" ) );
        log( m_pSettings, LogLevel::INFO, buf.makeStringAndClear() );
    }
    // all privileges
    return ret;
}

sal_Int32 User::getGrantablePrivileges( const ::rtl::OUString& objName, sal_Int32 objType )
    throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException)
{
    (void) objName; (void) objType;
    // all privileges
    return 0xffffffff;
}

void User::grantPrivileges( const ::rtl::OUString& objName, sal_Int32 objType, sal_Int32 objPrivileges )
    throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException)
{
    (void) objName; (void) objType; (void) objPrivileges;
    throw com::sun::star::sdbc::SQLException(
        ASCII_STR( "pq_driver: privilege change not implemented yet" ),
        *this, OUString(), 1, Any() );
}

void User::revokePrivileges( const ::rtl::OUString& objName, sal_Int32 objType, sal_Int32 objPrivileges )
    throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException)
{
    (void) objName; (void) objType; (void) objPrivileges;
    throw com::sun::star::sdbc::SQLException(
        ASCII_STR( "pq_driver: privilege change not implemented yet" ),
        *this, OUString(), 1, Any() );
}

//______________________________________________________________________________________
UserDescriptor::UserDescriptor(
    const ::rtl::Reference< RefCountedMutex > & refMutex,
    const Reference< com::sun::star::sdbc::XConnection > & connection,
    ConnectionSettings *pSettings )
    : ReflectionBase(
        getStatics().refl.userDescriptor.implName,
        getStatics().refl.userDescriptor.serviceNames,
        refMutex,
        connection,
        pSettings,
        * getStatics().refl.userDescriptor.pProps )
{}

Reference< XPropertySet > UserDescriptor::createDataDescriptor(  ) throw (RuntimeException)
{
    UserDescriptor * pUser = new UserDescriptor( m_refMutex, m_conn, m_pSettings );
    pUser->copyValuesFrom( this );

    return Reference< XPropertySet > ( pUser );
}


}
