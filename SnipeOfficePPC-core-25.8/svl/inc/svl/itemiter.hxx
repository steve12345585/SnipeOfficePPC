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
#ifndef _SFXITEMITER_HXX
#define _SFXITEMITER_HXX

#include "svl/svldllapi.h"
#include <tools/solar.h>
#include <svl/itemset.hxx>

class SfxPoolItem;
class SfxItemSet;
class SfxItemPool;

class SVL_DLLPUBLIC SfxItemIter
{
    // Item-Feld - Start & Ende
    const SfxItemSet&   _rSet;
    sal_uInt16              _nStt, _nEnd, _nAkt;

public:
    SfxItemIter( const SfxItemSet& rSet );
    ~SfxItemIter();

    // falls es diese gibt, returne sie, sonst 0
    const SfxPoolItem* FirstItem()
                       { _nAkt = _nStt;
                         return _rSet._nCount ? *(_rSet._aItems+_nAkt) : 0; }
    const SfxPoolItem* LastItem()
                       { _nAkt = _nEnd;
                         return _rSet._nCount ? *(_rSet._aItems+_nAkt) : 0; }
    const SfxPoolItem* GetCurItem()
                       { return _rSet._nCount ? *(_rSet._aItems+_nAkt) : 0; }
    const SfxPoolItem* NextItem();

    sal_Bool               IsAtStart() const { return _nAkt == _nStt; }
    sal_Bool               IsAtEnd() const   { return _nAkt == _nEnd; }

    sal_uInt16             GetCurPos() const { return _nAkt; }
    sal_uInt16             GetFirstPos() const { return _nStt; }
    sal_uInt16             GetLastPos() const { return _nEnd; }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
