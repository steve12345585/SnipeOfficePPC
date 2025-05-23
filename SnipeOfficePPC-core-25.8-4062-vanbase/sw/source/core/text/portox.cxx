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

#include <SwPortionHandler.hxx>
#include "viewopt.hxx"  // SwViewOptions

#include "portox.hxx"
#include "inftxt.hxx"       // GetTxtSize()

/*************************************************************************
 *               virtual SwToxPortion::Paint()
 *************************************************************************/

void SwToxPortion::Paint( const SwTxtPaintInfo &rInf ) const
{
    if( Width() )
    {
        rInf.DrawViewOpt( *this, POR_TOX );
        SwTxtPortion::Paint( rInf );
    }
}

/*************************************************************************
 *                      class SwIsoToxPortion
 *************************************************************************/

SwLinePortion *SwIsoToxPortion::Compress() { return this; }

SwIsoToxPortion::SwIsoToxPortion() : nViewWidth(0)
{
    SetLen(1);
    SetWhichPor( POR_ISOTOX );
}

/*************************************************************************
 *               virtual SwIsoToxPortion::GetViewWidth()
 *************************************************************************/

KSHORT SwIsoToxPortion::GetViewWidth( const SwTxtSizeInfo &rInf ) const
{
    // Although we are const, nViewWidth should be calculated in the last
    // moment possible
    SwIsoToxPortion* pThis = (SwIsoToxPortion*)this;
    // nViewWidth need to be calculated
    if( !Width() && rInf.OnWin() &&
        !rInf.GetOpt().IsPagePreview() &&
            !rInf.GetOpt().IsReadonly() && SwViewOption::IsFieldShadings()   )
    {
        if( !nViewWidth )
            pThis->nViewWidth = rInf.GetTxtSize(OUString(' ')).Width();
    }
    else
        pThis->nViewWidth = 0;
    return nViewWidth;
}

/*************************************************************************
 *                 virtual SwIsoToxPortion::Format()
 *************************************************************************/

sal_Bool SwIsoToxPortion::Format( SwTxtFormatInfo &rInf )
{
    const sal_Bool bFull = SwLinePortion::Format( rInf );
    return bFull;
}

/*************************************************************************
 *               virtual SwIsoToxPortion::Paint()
 *************************************************************************/

void SwIsoToxPortion::Paint( const SwTxtPaintInfo &rInf ) const
{
    if( Width() )
        rInf.DrawViewOpt( *this, POR_TOX );
}

/*************************************************************************
 *              virtual SwIsoToxPortion::HandlePortion()
 *************************************************************************/

void SwIsoToxPortion::HandlePortion( SwPortionHandler& rPH ) const
{
    String aString;
    rPH.Special( GetLen(), aString, GetWhichPor() );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
