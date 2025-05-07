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

#include <precomp.h>
#include <tokens/tkpstam2.hxx>

// NOT FULLY DECLARED SERVICES
#include <tokens/stmstar2.hxx>
#include <tools/tkpchars.hxx>


const intt  C_nStatuslistResizeValue = 32;
const intt  C_nTopStatus = 0;

StateMachin2::StateMachin2( intt            in_nStatusSize,
                            intt            in_nInitial_StatusListSize )
    :   pStati(new StmStatu2*[in_nInitial_StatusListSize]),
        nCurrentStatus(C_nTopStatus),
        nPeekedStatus(C_nTopStatus),
        nStatusSize(in_nStatusSize),
        nNrofStati(0),
        nStatiSpace(in_nInitial_StatusListSize)
{
    csv_assert(in_nStatusSize > 0);
    csv_assert(in_nInitial_StatusListSize > 0);

    memset(pStati, 0, sizeof(StmStatu2*) * nStatiSpace);
}

intt
StateMachin2::AddStatus(StmStatu2 * let_dpStatus)
{
    if (nNrofStati == nStatiSpace)
    {
        ResizeStati();
    }
    pStati[nNrofStati] = let_dpStatus;
    return nNrofStati++;
}

void
StateMachin2::AddToken( const char *        in_sToken,
                        UINT16              in_nTokenId,
                        const INT16 *       in_aBranches,
                        INT16               in_nBoundsStatus )
{
    if (csv::no_str(in_sToken))
        return;

    nCurrentStatus = 0;
    nPeekedStatus = 0;

    for ( const char * pChar = in_sToken;
          *pChar != NULCH;
          ++pChar )
    {
        Peek(*pChar);
        StmStatu2 & rPst = Status(nPeekedStatus);
        if ( rPst.IsADefault() OR rPst.AsBounds() != 0 )
        {
            nPeekedStatus = AddStatus( new StmArrayStatu2(nStatusSize, in_aBranches, 0, false ) );
            CurrentStatus().SetBranch( *pChar, nPeekedStatus );
        }
        nCurrentStatus = nPeekedStatus;
    }   // end for
    StmArrayStatu2 & rLastStatus = CurrentStatus();
    rLastStatus.SetTokenId(in_nTokenId);
    for (intt i = 0; i < nStatusSize; i++)
    {
        if (Status(rLastStatus.NextBy(i)).AsBounds() != 0)
            rLastStatus.SetBranch(i,in_nBoundsStatus);
    }   // end for
}

StateMachin2::~StateMachin2()
{
    for (intt i = 0; i < nNrofStati; i++)
    {
        delete pStati[i];
    }
    delete [] pStati;
}

StmBoundsStatu2 &
StateMachin2::GetCharChain( UINT16 &           o_nTokenId,
                            CharacterSource &  io_rText )
{
    nCurrentStatus = C_nTopStatus;
    Peek(io_rText.CurChar());
    while (BoundsStatus() == 0)
    {
        nCurrentStatus = nPeekedStatus;
        Peek(io_rText.MoveOn());
    }
    o_nTokenId = CurrentStatus().TokenId();

    return *BoundsStatus();
}

void
StateMachin2::ResizeStati()
{
    intt nNewSize = nStatiSpace + C_nStatuslistResizeValue;
    intt i = 0;
    StatusList pNewStati = new StmStatu2*[nNewSize];

    for ( ; i < nNrofStati; i++)
    {
        pNewStati[i] = pStati[i];
    }
    memset( pNewStati+i,
            0,
            (nNewSize-i) * sizeof(StmStatu2*) );

    delete [] pStati;
    pStati = pNewStati;
    nStatiSpace = nNewSize;
}

StmStatu2 &
StateMachin2::Status(intt in_nStatusNr) const
{
    csv_assert( csv::in_range(intt(0), in_nStatusNr, intt(nNrofStati)) );
    return *pStati[in_nStatusNr];
}

StmArrayStatu2 &
StateMachin2::CurrentStatus() const
{
    StmArrayStatu2 * pCurSt = Status(nCurrentStatus).AsArray();
    if (pCurSt == 0)
    {
        csv_assert(false);
    }
    return *pCurSt;
}

StmBoundsStatu2 *
StateMachin2::BoundsStatus() const
{
    return Status(nPeekedStatus).AsBounds();
}

void
StateMachin2::Peek(intt in_nBranch)
{
    StmArrayStatu2 & rSt = CurrentStatus();
    nPeekedStatus = rSt.NextBy(in_nBranch);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
