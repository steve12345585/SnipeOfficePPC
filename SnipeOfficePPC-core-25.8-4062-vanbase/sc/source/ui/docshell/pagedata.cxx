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

#include <string.h>

#include "pagedata.hxx"

//============================================================================

ScPrintRangeData::ScPrintRangeData()
{
    nPagesX = nPagesY = 0;
    pPageEndX = NULL;
    pPageEndY = NULL;
    bTopDown = bAutomatic = sal_True;
    nFirstPage = 1;
}

ScPrintRangeData::~ScPrintRangeData()
{
    delete[] pPageEndX;
    delete[] pPageEndY;
}

void ScPrintRangeData::SetPagesX( size_t nCount, const SCCOL* pData )
{
    delete[] pPageEndX;
    if ( nCount )
    {
        pPageEndX = new SCCOL[nCount];
        memcpy( pPageEndX, pData, nCount * sizeof(SCCOL) );
    }
    else
        pPageEndX = NULL;
    nPagesX = nCount;
}

void ScPrintRangeData::SetPagesY( size_t nCount, const SCROW* pData )
{
    delete[] pPageEndY;
    if ( nCount )
    {
        pPageEndY = new SCROW[nCount];
        memcpy( pPageEndY, pData, nCount * sizeof(SCROW) );
    }
    else
        pPageEndY = NULL;
    nPagesY = nCount;
}

//============================================================================

ScPageBreakData::ScPageBreakData(size_t nMax)
{
    nUsed = 0;
    if (nMax)
        pData = new ScPrintRangeData[nMax];
    else
        pData = NULL;
    nAlloc = nMax;
}

ScPageBreakData::~ScPageBreakData()
{
    delete[] pData;
}

ScPrintRangeData& ScPageBreakData::GetData(size_t nPos)
{
    OSL_ENSURE(nPos < nAlloc, "ScPageBreakData::GetData bumm");

    if ( nPos >= nUsed )
    {
        OSL_ENSURE(nPos == nUsed, "ScPageBreakData::GetData falsche Reihenfolge");
        nUsed = nPos+1;
    }

    return pData[nPos];
}

sal_Bool ScPageBreakData::IsEqual( const ScPageBreakData& rOther ) const
{
    if ( nUsed != rOther.nUsed )
        return false;

    for (sal_uInt16 i=0; i<nUsed; i++)
        if ( pData[i].GetPrintRange() != rOther.pData[i].GetPrintRange() )
            return false;

    //! ScPrintRangeData komplett vergleichen ??

    return sal_True;
}

void ScPageBreakData::AddPages()
{
    if ( nUsed > 1 )
    {
        long nPage = pData[0].GetFirstPage();
        for (sal_uInt16 i=0; sal::static_int_cast<size_t>(i+1)<nUsed; i++)
        {
            nPage += ((long)pData[i].GetPagesX())*pData[i].GetPagesY();
            pData[i+1].SetFirstPage( nPage );
        }
    }
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
