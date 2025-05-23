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

#ifndef SC_EXCSCEN_HXX
#define SC_EXCSCEN_HXX

#include <boost/ptr_container/ptr_vector.hpp>

#include <tools/solar.h>
#include <tools/string.hxx>

struct RootData;
class XclImpRoot;
class XclImpStream;

class ExcScenarioCell
{
private:
    String                      aValue;
public:
    const sal_uInt16                nCol;
    const sal_uInt16                nRow;

    ExcScenarioCell( const sal_uInt16 nC, const sal_uInt16 nR );

    inline void SetValue( const String& rVal ) { aValue = rVal; }

    inline const String& GetValue( void ) const { return aValue; }
};

class ExcScenario
{
public:

    ExcScenario( XclImpStream& rIn, const RootData& rRoot );

    ~ExcScenario();

    void Apply( const XclImpRoot& rRoot, const sal_Bool bLast = false );

protected:

    String* pName;
    String* pComment;
    String* pUserName;
    sal_uInt8 nProtected;
    const sal_uInt16 nTab;
    boost::ptr_vector<ExcScenarioCell> aEntries;
};

struct ExcScenarioList
{
    ExcScenarioList () : nLastScenario(0) {}

    void Apply( const XclImpRoot& rRoot );

    sal_uInt16 nLastScenario;
    boost::ptr_vector<ExcScenario> aEntries;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
