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
        nOrdNum = pFmt->GetDoc()->GetSpzFrmFmts()->size();
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
