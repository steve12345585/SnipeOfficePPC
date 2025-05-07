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

#ifndef _NDTYP_HXX
#define _NDTYP_HXX

#include <tools/solar.h>

// IDs for different nodes. The member indicating the type of node is
// in base class.
const sal_uInt8 ND_ENDNODE      = 0x01;
const sal_uInt8 ND_STARTNODE    = 0x02;
const sal_uInt8 ND_TABLEDUMMY   = 0x04; //(ND_TABLENODE & ~ND_STARTNODE);
const sal_uInt8 ND_TEXTNODE     = 0x08;
const sal_uInt8 ND_GRFNODE      = 0x10;
const sal_uInt8 ND_OLENODE      = 0x20;
const sal_uInt8 ND_SECTIONDUMMY = 0x40; //(ND_SECTIONNODE & ~ND_STARTNODE);

const sal_uInt8 ND_TABLENODE    = ND_TABLEDUMMY | ND_STARTNODE;
const sal_uInt8 ND_SECTIONNODE  = ND_SECTIONDUMMY | ND_STARTNODE;

// NoTxtNode (if any of the 2 bits are set).
const sal_uInt8 ND_NOTXTNODE    = ND_GRFNODE | ND_OLENODE;
// ContentNode (if any of the 3 bits are set).
const sal_uInt8 ND_CONTENTNODE  = ND_TEXTNODE | ND_NOTXTNODE;

// Special types of StartNodes that are not derivations but keep
// "sections" together.
enum SwStartNodeType
{
    SwNormalStartNode = 0,
    SwTableBoxStartNode,
    SwFlyStartNode,
    SwFootnoteStartNode,
    SwHeaderStartNode,
    SwFooterStartNode
};

// is the node the first and/or last node of a section?
// This information is used for the export filters. Our layout never have a
// distance before or after if the node is the first or last in a section.
const sal_uInt8 ND_HAS_PREV_LAYNODE = 0x01;
const sal_uInt8 ND_HAS_NEXT_LAYNODE = 0x02;



#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
