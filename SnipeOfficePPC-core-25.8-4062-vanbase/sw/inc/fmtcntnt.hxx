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
#ifndef _FMTCNTNT_HXX
#define _FMTCNTNT_HXX

#include <svl/poolitem.hxx>
#include <hintids.hxx>
#include <format.hxx>

class SwNodeIndex;
class SwStartNode;


/// Cntnt, content of frame (header, footer, fly).
class SW_DLLPUBLIC SwFmtCntnt: public SfxPoolItem
{
    SwNodeIndex *pStartNode;

    SwFmtCntnt &operator=( const SwFmtCntnt & ); ///< Copying not allowed.

public:
    SwFmtCntnt( const SwStartNode* pStartNode = 0 );
    SwFmtCntnt( const SwFmtCntnt &rCpy );
    ~SwFmtCntnt();

    /// "Pure virtual methods" of SfxPoolItem.
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*    Clone( SfxItemPool* pPool = 0 ) const;

    const SwNodeIndex *GetCntntIdx() const { return pStartNode; }
    void SetNewCntntIdx( const SwNodeIndex *pIdx );
};

inline const SwFmtCntnt &SwAttrSet::GetCntnt(sal_Bool bInP) const
    { return (const SwFmtCntnt&)Get( RES_CNTNT,bInP); }

inline const SwFmtCntnt &SwFmt::GetCntnt(sal_Bool bInP) const
    { return aSet.GetCntnt(bInP); }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
