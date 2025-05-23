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
#ifndef _TBLENUM_HXX
#define _TBLENUM_HXX


// For changing table columns/rows widths/heights.
typedef sal_uInt16 TblChgWidthHeightType;

namespace nsTblChgWidthHeightType
{
    const TblChgWidthHeightType WH_COL_LEFT = 0;
    const TblChgWidthHeightType WH_COL_RIGHT = 1;
    const TblChgWidthHeightType WH_ROW_TOP = 2;
    const TblChgWidthHeightType WH_ROW_BOTTOM = 3;
    const TblChgWidthHeightType WH_CELL_LEFT = 4;
    const TblChgWidthHeightType WH_CELL_RIGHT = 5;
    const TblChgWidthHeightType WH_CELL_TOP = 6;
    const TblChgWidthHeightType WH_CELL_BOTTOM = 7;


    // The following can "or"ed into.
    const TblChgWidthHeightType WH_FLAG_INSDEL  = 0x4000;           // Insert/Del-mode: the Bigger-Flag
                                                                    // tells what happens:
                                                                    // bBigger -> box gets removed.
                                                                    // !bBigger-> box gets inserted.
    const TblChgWidthHeightType WH_FLAG_BIGGER  = 0x8000;           // Box becomes larger -> else smaller.
}

enum TblChgMode
{
    TBLFIX_CHGABS,          // Table fixed width, change neighbour.
    TBLFIX_CHGPROP,         // Table fixed width, change all neighbours.
    TBLVAR_CHGABS           // Table variable, change all neighbours.
};


enum SplitTbl_HeadlineOption
{
    HEADLINE_NONE = 0,          // Leave everything in place.
    HEADLINE_BORDERCOPY,        // von der davor obere Line der vorhergehen Line
    HEADLINE_CNTNTCOPY,         // Copy 1st line with all contents.
    HEADLINE_BOXATTRCOPY,       // Copy box attributs of 1st line.
    HEADLINE_BOXATRCOLLCOPY     // Copy box attributes and paragraph styles of 1st line.
};

enum TableMergeErr
{
    TBLMERGE_OK,
    TBLMERGE_NOSELECTION,
    TBLMERGE_TOOCOMPLEX
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
