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

#include <cppuhelper/typeprovider.hxx>

#include "pq_xcolumn.hxx"

using com::sun::star::uno::Any;
using com::sun::star::uno::Reference;
using com::sun::star::uno::RuntimeException;
using com::sun::star::uno::Type;
using com::sun::star::uno::Sequence;

using com::sun::star::beans::XPropertySet;

namespace pq_sdbc_driver
{
Column::Column( const ::rtl::Reference< RefCountedMutex > & refMutex,
                const Reference< com::sun::star::sdbc::XConnection > & connection,
                ConnectionSettings *pSettings)
    : ReflectionBase(
        getStatics().refl.column.implName,
        getStatics().refl.column.serviceNames,
        refMutex,
        connection,
        pSettings,
        * getStatics().refl.column.pProps )
{}

Reference< XPropertySet > Column::createDataDescriptor(  ) throw (RuntimeException)
{
    ColumnDescriptor * pColumn = new ColumnDescriptor(
        m_refMutex, m_conn, m_pSettings );
    pColumn->copyValuesFrom( this );
    return Reference< XPropertySet > ( pColumn );
}

ColumnDescriptor::ColumnDescriptor(
    const ::rtl::Reference< RefCountedMutex > & refMutex,
    const Reference< com::sun::star::sdbc::XConnection > & connection,
    ConnectionSettings *pSettings)
    : ReflectionBase(
        getStatics().refl.columnDescriptor.implName,
        getStatics().refl.columnDescriptor.serviceNames,
        refMutex,
        connection,
        pSettings,
        *getStatics().refl.columnDescriptor.pProps )
{}

Reference< XPropertySet > ColumnDescriptor::createDataDescriptor(  ) throw (RuntimeException)
{
    ColumnDescriptor * pColumn = new ColumnDescriptor(
        m_refMutex, m_conn, m_pSettings );
    pColumn->copyValuesFrom( this );

    return Reference< XPropertySet > ( pColumn );
}





}
