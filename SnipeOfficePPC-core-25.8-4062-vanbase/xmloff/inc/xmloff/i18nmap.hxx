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

#ifndef _XMLOFF_I18NMAP_HXX
#define _XMLOFF_I18NMAP_HXX

#include <sal/config.h>
#include "xmloff/dllapi.h"
#include <rtl/ustring.hxx>
#include <tools/solar.h>
#include <map>

class SvI18NMapEntry_Key
{
    sal_uInt16      nKind;
    rtl::OUString   aName;
public:
    SvI18NMapEntry_Key( sal_uInt16 nKnd, const rtl::OUString& rName ) :
        nKind( nKnd ),
        aName( rName )
    {
    }

    sal_Bool operator==( const SvI18NMapEntry_Key& r ) const
    {
        return nKind == r.nKind &&
               aName == r.aName;
    }

    sal_Bool operator<( const SvI18NMapEntry_Key& r ) const
    {
        return nKind < r.nKind ||
               ( nKind == r.nKind &&
                 aName < r.aName);
    }
};

typedef std::map<SvI18NMapEntry_Key, rtl::OUString> SvI18NMap_Impl;

class XMLOFF_DLLPUBLIC SvI18NMap
{
    SvI18NMap_Impl m_aMap;

public:
    // Add a name mapping
    bool Add( sal_uInt16 nKind, const rtl::OUString& rName,
              const rtl::OUString& rNewName );

    // Return a mapped name. If the name could not be found, return the
    // original name.
    const rtl::OUString& Get( sal_uInt16 nKind,
                                const rtl::OUString& rName ) const;
};

#endif  //  _XMLOFF_I18NMAP_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
