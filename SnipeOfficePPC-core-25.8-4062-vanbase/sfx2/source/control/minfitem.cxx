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

#include "sfx2/minfitem.hxx"

// STATIC DATA -----------------------------------------------------------

TYPEINIT1(SfxMacroInfoItem, SfxPoolItem);

// -----------------------------------------------------------------------

SfxMacroInfoItem::SfxMacroInfoItem(
    sal_uInt16 nWhichId,        // Slot-ID
    const BasicManager* pMgr,
    const String &rLibName,
    const String &rModuleName,
    const String &rMethodName,
    const String &rComment) :
    SfxPoolItem(nWhichId),
    pBasicManager(pMgr),
    aLibName(rLibName),
    aModuleName(rModuleName),
    aMethodName(rMethodName),
    aCommentText(rComment)
{
}

// -----------------------------------------------------------------------

// copy constructor

SfxMacroInfoItem::SfxMacroInfoItem(const SfxMacroInfoItem& rCopy):
    SfxPoolItem(rCopy),
    pBasicManager(rCopy.pBasicManager),
    aLibName(rCopy.aLibName),
    aModuleName(rCopy.aModuleName),
    aMethodName(rCopy.aMethodName),
    aCommentText(rCopy.aCommentText)
{
}

// -----------------------------------------------------------------------

// op ==

int SfxMacroInfoItem::operator==( const SfxPoolItem& rCmp) const
{
    return SfxPoolItem::operator==(rCmp) &&
            pBasicManager == ((const SfxMacroInfoItem&)rCmp).pBasicManager &&
            aLibName == ((const SfxMacroInfoItem&)rCmp).aLibName &&
            aModuleName == ((const SfxMacroInfoItem&)rCmp).aModuleName &&
            aMethodName == ((const SfxMacroInfoItem&)rCmp).aMethodName &&
            aCommentText == ((const SfxMacroInfoItem&)rCmp).aCommentText;
}

// -----------------------------------------------------------------------

SfxPoolItem *SfxMacroInfoItem::Clone( SfxItemPool *) const
{
    return new SfxMacroInfoItem(*this);
}

// -----------------------------------------------------------------------

String SfxMacroInfoItem::GetQualifiedName() const
{
    String aMacroName = aLibName;
    aMacroName += '.';
    aMacroName += aModuleName;
    aMacroName += '.';
    aMacroName += aMethodName;
    return aMacroName;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
