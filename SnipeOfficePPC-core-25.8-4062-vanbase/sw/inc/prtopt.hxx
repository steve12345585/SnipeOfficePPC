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
#ifndef _PRTOPT_HXX
#define _PRTOPT_HXX

#include <unotools/configitem.hxx>
#include <printdata.hxx>

class SwPrintOptions : public SwPrintData, public utl::ConfigItem
{
    sal_Bool            bIsWeb;

    com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();
public:
    SwPrintOptions(sal_Bool bWeb);
    virtual ~SwPrintOptions();

    virtual void Commit();
    virtual void Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );
    virtual void            doSetModified( ) { bModified = sal_True; SetModified();}

    SwPrintOptions& operator=(const SwPrintData& rData)
    {
        SwPrintData::operator=( rData );
        SetModified();
        return *this;
    }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
