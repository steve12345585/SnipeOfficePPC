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


#include "doc.hxx"
#include "node.hxx"
#include <docary.hxx>
#include <fmtanchr.hxx>
#include "flypos.hxx"
#include "frmfmt.hxx"
#include "dcontact.hxx"
#include "dview.hxx"
#include "flyfrm.hxx"
#include "dflyobj.hxx"
#include "ndindex.hxx"
#include "switerator.hxx"

SwPosFlyFrm::SwPosFlyFrm( const SwNodeIndex& rIdx, const SwFrmFmt* pFmt,
                            sal_uInt16 nArrPos )
    : pFrmFmt( pFmt ), pNdIdx( (SwNodeIndex*) &rIdx )
{
    bool bFnd = false;
    const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
    if (FLY_AT_PAGE == rAnchor.GetAnchorId())
    {
        pNdIdx = new SwNodeIndex( rIdx );
    }
    else if( pFmt->GetDoc()->GetCurrentViewShell() )    //swmod 071108//swmod 071225
    {
        if( RES_FLYFRMFMT == pFmt->Which() )
        {
            // Let's see if we have an SdrObject for this
            SwFlyFrm* pFly = SwIterator<SwFlyFrm,SwFmt>::FirstElement(*pFmt);
            if( pFly )
            {
                nOrdNum = pFly->GetVirtDrawObj()->GetOrdNum();
                bFnd = true;
            }
        }
        else if( RES_DRAWFRMFMT == pFmt->Which() )
        {
            // Let's see if we have an SdrObject for this
            SwDrawContact* pContact = SwIterator<SwDrawContact,SwFmt>::FirstElement(*pFmt);
            if( pContact )
            {
                nOrdNum = pContact->GetMaster()->GetOrdNum();
                bFnd = true;
            }
        }
    }

    if( !bFnd )
    {
        nOrdNum = pFmt->GetDoc()->GetSpzFrmFmts()->Count();
        nOrdNum += nArrPos;
    }
}

SwPosFlyFrm::~SwPosFlyFrm()
{
    const SwFmtAnchor& rAnchor = pFrmFmt->GetAnchor();
    if (FLY_AT_PAGE == rAnchor.GetAnchorId())
    {
        delete pNdIdx;
    }
}

sal_Bool SwPosFlyFrm::operator==( const SwPosFlyFrm& )
{
    return sal_False;   // FlyFrames can sit at the same position
}

sal_Bool SwPosFlyFrm::operator<( const SwPosFlyFrm& rPosFly )
{
    if( pNdIdx->GetIndex() == rPosFly.pNdIdx->GetIndex() )
    {
        // In this case, the order number decides!
        return nOrdNum < rPosFly.nOrdNum;
    }
    return pNdIdx->GetIndex() < rPosFly.pNdIdx->GetIndex();
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
