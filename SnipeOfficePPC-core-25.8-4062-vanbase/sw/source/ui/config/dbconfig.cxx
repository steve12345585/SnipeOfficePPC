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

#include <dbconfig.hxx>
#include <osl/diagnose.h>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <swdbdata.hxx>

#include <unomid.h>

using namespace utl;
using rtl::OUString;
using namespace com::sun::star::uno;

/*--------------------------------------------------------------------
     Description: Ctor
 --------------------------------------------------------------------*/

const Sequence<OUString>& SwDBConfig::GetPropertyNames()
{
    static Sequence<OUString> aNames;
    if(!aNames.getLength())
    {
        static const char* aPropNames[] =
        {
            "AddressBook/DataSourceName",        //  0
            "AddressBook/Command",              //  1
            "AddressBook/CommandType",          //  2
            "Bibliography/CurrentDataSource/DataSourceName",        //  4
            "Bibliography/CurrentDataSource/Command",              //  5
            "Bibliography/CurrentDataSource/CommandType"          //  6
        };
        const int nCount = sizeof(aPropNames)/sizeof(const char*);
        aNames.realloc(nCount);
        OUString* pNames = aNames.getArray();
        for(int i = 0; i < nCount; i++)
            pNames[i] = OUString::createFromAscii(aPropNames[i]);
    }
    return aNames;
}

SwDBConfig::SwDBConfig() :
    ConfigItem("Office.DataAccess",
        CONFIG_MODE_DELAYED_UPDATE|CONFIG_MODE_RELEASE_TREE),
    pAdrImpl(0),
    pBibImpl(0)
{
};

SwDBConfig::~SwDBConfig()
{
    delete pAdrImpl;
    delete pBibImpl;
}

void SwDBConfig::Load()
{
    const Sequence<OUString>& rNames = GetPropertyNames();
    if(!pAdrImpl)
    {

        pAdrImpl = new SwDBData;
        pAdrImpl->nCommandType = 0;
        pBibImpl = new SwDBData;
        pBibImpl->nCommandType = 0;
    }
    Sequence<Any> aValues = GetProperties(rNames);
    const Any* pValues = aValues.getConstArray();
    OSL_ENSURE(aValues.getLength() == rNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == rNames.getLength())
    {
        for(int nProp = 0; nProp < rNames.getLength(); nProp++)
        {
            switch(nProp)
            {
                case  0: pValues[nProp] >>= pAdrImpl->sDataSource;  break;
                case  1: pValues[nProp] >>= pAdrImpl->sCommand;     break;
                case  2: pValues[nProp] >>= pAdrImpl->nCommandType; break;
                case  3: pValues[nProp] >>= pBibImpl->sDataSource;  break;
                case  4: pValues[nProp] >>= pBibImpl->sCommand;     break;
                case  5: pValues[nProp] >>= pBibImpl->nCommandType; break;
            }
        }
    }
}

const SwDBData& SwDBConfig::GetAddressSource()
{
    if(!pAdrImpl)
        Load();
    return *pAdrImpl;
}

const SwDBData& SwDBConfig::GetBibliographySource()
{
    if(!pBibImpl)
        Load();
    return *pBibImpl;
}

void SwDBConfig::Commit() {}
void SwDBConfig::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& ) {}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
