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




// INCLUDE ---------------------------------------------------------------

#include "prnsave.hxx"
#include "global.hxx"
#include "address.hxx"

// STATIC DATA -----------------------------------------------------------

//------------------------------------------------------------------

//
//      Daten pro Tabelle
//

ScPrintSaverTab::ScPrintSaverTab() :
    mpRepeatCol(NULL),
    mpRepeatRow(NULL),
    mbEntireSheet(false)
{
}

ScPrintSaverTab::~ScPrintSaverTab()
{
    delete mpRepeatCol;
    delete mpRepeatRow;
}

void ScPrintSaverTab::SetAreas( const ScRangeVec& rRanges, sal_Bool bEntireSheet )
{
    maPrintRanges = rRanges;
    mbEntireSheet = bEntireSheet;
}

void ScPrintSaverTab::SetRepeat( const ScRange* pCol, const ScRange* pRow )
{
    delete mpRepeatCol;
    mpRepeatCol = pCol ? new ScRange(*pCol) : NULL;
    delete mpRepeatRow;
    mpRepeatRow = pRow ? new ScRange(*pRow) : NULL;
}

inline sal_Bool PtrEqual( const ScRange* p1, const ScRange* p2 )
{
    return ( !p1 && !p2 ) || ( p1 && p2 && *p1 == *p2 );
}

sal_Bool ScPrintSaverTab::operator==( const ScPrintSaverTab& rCmp ) const
{
    return
        PtrEqual( mpRepeatCol, rCmp.mpRepeatCol ) &&
        PtrEqual( mpRepeatRow, rCmp.mpRepeatRow ) &&
        (mbEntireSheet == rCmp.mbEntireSheet) &&
        (maPrintRanges == rCmp.maPrintRanges);
}

//
//      Daten fuer das ganze Dokument
//

ScPrintRangeSaver::ScPrintRangeSaver( SCTAB nCount ) :
    nTabCount( nCount )
{
    if (nCount > 0)
        pData = new ScPrintSaverTab[nCount];
    else
        pData = NULL;
}

ScPrintRangeSaver::~ScPrintRangeSaver()
{
    delete[] pData;
}

ScPrintSaverTab& ScPrintRangeSaver::GetTabData(SCTAB nTab)
{
    OSL_ENSURE(nTab<nTabCount,"ScPrintRangeSaver Tab zu gross");
    return pData[nTab];
}

const ScPrintSaverTab& ScPrintRangeSaver::GetTabData(SCTAB nTab) const
{
    OSL_ENSURE(nTab<nTabCount,"ScPrintRangeSaver Tab zu gross");
    return pData[nTab];
}

sal_Bool ScPrintRangeSaver::operator==( const ScPrintRangeSaver& rCmp ) const
{
    sal_Bool bEqual = ( nTabCount == rCmp.nTabCount );
    if (bEqual)
        for (SCTAB i=0; i<nTabCount; i++)
            if (!(pData[i]==rCmp.pData[i]))
            {
                bEqual = false;
                break;
            }
    return bEqual;
}




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
