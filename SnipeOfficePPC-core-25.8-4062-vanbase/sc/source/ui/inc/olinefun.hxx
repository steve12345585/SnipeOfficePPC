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

#ifndef SC_OLINEFUN_HXX
#define SC_OLINEFUN_HXX

#include "global.hxx"

class ScDocShell;
class ScRange;


// ---------------------------------------------------------------------------

class ScOutlineDocFunc
{
private:
    ScDocShell& rDocShell;

public:
                ScOutlineDocFunc( ScDocShell& rDocSh ): rDocShell(rDocSh) {}
                ~ScOutlineDocFunc() {}

    sal_Bool        MakeOutline( const ScRange& rRange, sal_Bool bColumns, sal_Bool bRecord, sal_Bool bApi );
    sal_Bool        RemoveOutline( const ScRange& rRange, sal_Bool bColumns, sal_Bool bRecord, sal_Bool bApi );
    sal_Bool        RemoveAllOutlines( SCTAB nTab, sal_Bool bRecord, sal_Bool bApi );
    sal_Bool        AutoOutline( const ScRange& rRange, sal_Bool bRecord, sal_Bool bApi );

    sal_Bool        SelectLevel( SCTAB nTab, sal_Bool bColumns, sal_uInt16 nLevel,
                                    sal_Bool bRecord, sal_Bool bPaint, sal_Bool bApi );

    sal_Bool        ShowMarkedOutlines( const ScRange& rRange, sal_Bool bRecord );
    sal_Bool        HideMarkedOutlines( const ScRange& rRange, sal_Bool bRecord, sal_Bool bApi );

    sal_Bool        ShowOutline( SCTAB nTab, sal_Bool bColumns, sal_uInt16 nLevel, sal_uInt16 nEntry,
                                    sal_Bool bRecord, sal_Bool bPaint, sal_Bool bApi );
    sal_Bool        HideOutline( SCTAB nTab, sal_Bool bColumns, sal_uInt16 nLevel, sal_uInt16 nEntry,
                                    sal_Bool bRecord, sal_Bool bPaint, sal_Bool bApi );
};



#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
