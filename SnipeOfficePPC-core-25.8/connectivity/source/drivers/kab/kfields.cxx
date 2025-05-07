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


#include "kfields.hxx"
#include "resource/common_res.hrc"
#include "resource/sharedresources.hxx"

using namespace ::connectivity::kab;
using namespace ::com::sun::star::sdbc;

namespace connectivity
{
    namespace kab
    {
// -----------------------------------------------------------------------------
// return the value of a KDE address book field, given an addressee and a field number
QString valueOfKabField(const ::KABC::Addressee &aAddressee, sal_Int32 nFieldNumber)
{
    switch (nFieldNumber)
    {
        case KAB_FIELD_REVISION:
            return aAddressee.revision().toString("yyyy-MM-dd hh:mm:ss");
        default:
            ::KABC::Field::List aFields = ::KABC::Field::allFields();
            return aFields[nFieldNumber - KAB_DATA_FIELDS]->value(aAddressee);
    }
}
// ------------------------------------------------------------------------------
// search the KDE address book field number of a given column name
sal_uInt32 findKabField(const ::rtl::OUString& columnName) throw(SQLException)
{
    QString aQtName;
    ::rtl::OUString aName;

    aQtName = KABC::Addressee::revisionLabel();
    aName = ::rtl::OUString((const sal_Unicode *) aQtName.ucs2());
    if (columnName == aName)
        return KAB_FIELD_REVISION;

    ::KABC::Field::List aFields = ::KABC::Field::allFields();
    ::KABC::Field::List::iterator aField;
    sal_uInt32 nResult;

    for (   aField = aFields.begin(), nResult = KAB_DATA_FIELDS;
            aField != aFields.end();
            ++aField, ++nResult)
    {
        aQtName = (*aField)->label();
        aName = ::rtl::OUString((const sal_Unicode *) aQtName.ucs2());

        if (columnName == aName)
            return nResult;
    }

    ::connectivity::SharedResources aResources;
    const ::rtl::OUString sError( aResources.getResourceStringWithSubstitution(
            STR_INVALID_COLUMNNAME,
            "$columnname$",columnName
         ) );
    ::dbtools::throwGenericSQLException(sError,NULL);
    // Unreachable:
    OSL_ASSERT(false);
    return 0;
}
// ------------------------------------------------------------------------------
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
