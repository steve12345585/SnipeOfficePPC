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
#ifndef _SXMFSITM_HXX
#define _SXMFSITM_HXX

#include <svx/svddef.hxx>
#include <svl/stritem.hxx>

// Formatstring (aehnl. Calc), z.B. "#.###,00km"
// ueberschreibt SdrMeasureUnitItem und SdrMeasureShowUnitItem (n.i.)
class SdrMeasureFormatStringItem: public SfxStringItem {
public:
    SdrMeasureFormatStringItem()                   : SfxStringItem() { SetWhich(SDRATTR_MEASUREFORMATSTRING); }
    SdrMeasureFormatStringItem(const String& rStr) : SfxStringItem(SDRATTR_MEASUREFORMATSTRING,rStr) {}
    SdrMeasureFormatStringItem(SvStream& rIn)      : SfxStringItem(SDRATTR_MEASUREFORMATSTRING,rIn)  {}
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
