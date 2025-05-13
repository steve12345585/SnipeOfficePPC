/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#include <unotools/transliterationwrapper.hxx>

#include "autonamecache.hxx"
#include "dociter.hxx"
#include "cell.hxx"
#include "queryparam.hxx"

// -----------------------------------------------------------------------

ScAutoNameCache::ScAutoNameCache( ScDocument* pD ) :
    pDoc( pD ),
    nCurrentTab( 0 )    // doesn't matter - aNames is empty
{
}

ScAutoNameCache::~ScAutoNameCache()
{
}

const ScAutoNameAddresses& ScAutoNameCache::GetNameOccurrences( const String& rName, SCTAB nTab )
{
    if ( nTab != nCurrentTab )
    {
        // the lists are valid only for one sheet, so they are cleared when another sheet is used
        aNames.clear();
        nCurrentTab = nTab;
    }

    ScAutoNameHashMap::const_iterator aFound = aNames.find( rName );
    if ( aFound != aNames.end() )
        return aFound->second;          // already initialized

    ScAutoNameAddresses& rAddresses = aNames[rName];

    ScCellIterator aIter( pDoc, ScRange( 0, 0, nCurrentTab, MAXCOL, MAXROW, nCurrentTab ) );
    for ( ScBaseCell* pCell = aIter.GetFirst(); pCell; pCell = aIter.GetNext() )
    {
        // don't check code length here, always use the stored result
        // (AutoCalc is disabled during CompileXML)

        if ( pCell->HasStringData() )
        {
            String aStr;
            CellType eType = pCell->GetCellType();
            switch ( eType )
            {
                case CELLTYPE_STRING:
                    aStr = ((ScStringCell*)pCell)->GetString();
                break;
                case CELLTYPE_FORMULA:
                    aStr = ((ScFormulaCell*)pCell)->GetString();
                break;
                case CELLTYPE_EDIT:
                    aStr = ((ScEditCell*)pCell)->GetString();
                break;
                case CELLTYPE_NONE:
                case CELLTYPE_VALUE:
                case CELLTYPE_NOTE:
                case CELLTYPE_SYMBOLS:
#if OSL_DEBUG_LEVEL > 0
                case CELLTYPE_DESTROYED:
#endif
                    ;   // nothing, prevent compiler warning
                break;
            }
            if ( ScGlobal::GetpTransliteration()->isEqual( aStr, rName ) )
            {
                rAddresses.push_back( ScAddress( aIter.GetCol(), aIter.GetRow(), aIter.GetTab() ) );
            }
        }
    }

    return rAddresses;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
