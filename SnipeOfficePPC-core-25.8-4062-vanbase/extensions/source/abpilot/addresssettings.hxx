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

#ifndef EXTENSIONS_ABP_ADDRESSSETTINGS_HXX
#define EXTENSIONS_ABP_ADDRESSSETTINGS_HXX

#include <rtl/ustring.hxx>
#include "abptypes.hxx"

//.........................................................................
namespace abp
{
//.........................................................................

    //=====================================================================
    //= AddressSourceType
    //=====================================================================
    enum AddressSourceType
    {
        AST_MORK,
        AST_THUNDERBIRD,
        AST_EVOLUTION,
        AST_EVOLUTION_GROUPWISE,
        AST_EVOLUTION_LDAP,
        AST_KAB,
        AST_MACAB,
        AST_LDAP,
        AST_OUTLOOK,
        AST_OE,

        AST_OTHER,

        AST_INVALID
    };

    //=====================================================================
    //= AddressSettings
    //=====================================================================
    struct AddressSettings
    {
        AddressSourceType   eType;
        ::rtl::OUString     sDataSourceName;
        ::rtl::OUString     sRegisteredDataSourceName;
        ::rtl::OUString     sSelectedTable;
        bool                bIgnoreNoTable;
        MapString2String    aFieldMapping;
        bool                bRegisterDataSource;
    };

//.........................................................................
}   // namespace abp
//.........................................................................

#endif // EXTENSIONS_ABP_ADDRESSSETTINGS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
