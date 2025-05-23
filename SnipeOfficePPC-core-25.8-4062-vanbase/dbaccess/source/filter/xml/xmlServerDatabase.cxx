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

#include "xmlServerDatabase.hxx"
#include "xmlfilter.hxx"
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmlnmspe.hxx>
#include <xmloff/nmspmap.hxx>
#include "xmlEnums.hxx"
#include "xmlstrings.hrc"
#include <tools/debug.hxx>
#include <tools/diagnose_ex.h>

namespace dbaxml
{
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::xml::sax;
DBG_NAME(OXMLServerDatabase)

OXMLServerDatabase::OXMLServerDatabase( ODBFilter& rImport,
                sal_uInt16 nPrfx, const ::rtl::OUString& _sLocalName,
                const Reference< XAttributeList > & _xAttrList) :
    SvXMLImportContext( rImport, nPrfx, _sLocalName )
{
    DBG_CTOR(OXMLServerDatabase,NULL);

    OSL_ENSURE(_xAttrList.is(),"Attribute list is NULL!");
    const SvXMLNamespaceMap& rMap = rImport.GetNamespaceMap();
    const SvXMLTokenMap& rTokenMap = rImport.GetDataSourceElemTokenMap();

    Reference<XPropertySet> xDataSource = rImport.getDataSource();

    PropertyValue aProperty;

    const sal_Int16 nLength = (xDataSource.is() && _xAttrList.is()) ? _xAttrList->getLength() : 0;
    ::rtl::OUString sType,sHostName,sPortNumber,sDatabaseName;
    for(sal_Int16 i = 0; i < nLength; ++i)
    {
        ::rtl::OUString sLocalName;
        const rtl::OUString sAttrName = _xAttrList->getNameByIndex( i );
        const sal_uInt16 nPrefix = rMap.GetKeyByAttrName( sAttrName,&sLocalName );
        const rtl::OUString sValue = _xAttrList->getValueByIndex( i );

        switch( rTokenMap.Get( nPrefix, sLocalName ) )
        {
            case XML_TOK_DB_TYPE:
                sType = sValue;
                break;
            case XML_TOK_HOSTNAME:
                sHostName = sValue;
                break;
            case XML_TOK_PORT:
                sPortNumber = sValue;
                break;
            case XML_TOK_LOCAL_SOCKET:
                aProperty.Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("LocalSocket"));
                aProperty.Value <<= sValue;
                rImport.addInfo(aProperty);
                break;
            case XML_TOK_DATABASE_NAME:
                sDatabaseName = sValue;
                break;
        }
    }
    if ( !sType.isEmpty() )
    {
        ::rtl::OUStringBuffer sURL;
        if  ( sType == "sdbc:mysql:jdbc" || sType == "sdbc:mysqlc" || sType == "sdbc:mysql:mysqlc" )
        {
            sURL.append( sType );
            sURL.append( sal_Unicode( ':' ) );
            sURL.append(sHostName);
            if ( !sPortNumber.isEmpty() )
            {
                sURL.appendAscii(":");
                sURL.append(sPortNumber);
            }
            if ( !sDatabaseName.isEmpty() )
            {
                sURL.appendAscii("/");
                sURL.append(sDatabaseName);
            }
        }
        else if ( sType == "jdbc:oracle:thin" )
        {
            sURL.appendAscii("jdbc:oracle:thin:@");
            sURL.append(sHostName);
            if ( !sPortNumber.isEmpty() )
            {
                sURL.appendAscii(":");
                sURL.append(sPortNumber);
            }
            if ( !sDatabaseName.isEmpty() )
            {
                sURL.appendAscii(":");
                sURL.append(sDatabaseName);
            }
        }
        else if ( sType == "sdbc:address:ldap" )
        {
            sURL.appendAscii("sdbc:address:ldap:");
            sURL.append(sHostName);
            if ( !sPortNumber.isEmpty() )
            {
                sURL.appendAscii(":");
                sURL.append(sPortNumber);
            }
        }
        else
        {
            sURL.append(sType);
            sURL.appendAscii(":");
            sURL.append(sHostName);
            if ( !sPortNumber.isEmpty() )
            {
                sURL.appendAscii(":");
                sURL.append(sPortNumber);
            }
            if ( !sDatabaseName.isEmpty() )
            {
                sURL.appendAscii(":");
                sURL.append(sDatabaseName);
            }
        }
        try
        {
            xDataSource->setPropertyValue(PROPERTY_URL,makeAny(sURL.makeStringAndClear()));
        }
        catch(const Exception&)
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }
}
// -----------------------------------------------------------------------------

OXMLServerDatabase::~OXMLServerDatabase()
{

    DBG_DTOR(OXMLServerDatabase,NULL);
}
// -----------------------------------------------------------------------------

//----------------------------------------------------------------------------
} // namespace dbaxml
// -----------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
