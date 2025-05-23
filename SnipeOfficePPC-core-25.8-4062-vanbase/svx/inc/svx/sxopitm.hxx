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
#ifndef _SXOPITM_HXX
#define _SXOPITM_HXX

#include <svx/svddef.hxx>
#include <svx/sdynitm.hxx>

//------------------------------
// class SdrObjPrintableItem
//------------------------------
class SdrObjPrintableItem: public SdrYesNoItem {
public:
    SdrObjPrintableItem(bool bOn=false): SdrYesNoItem(SDRATTR_OBJPRINTABLE,bOn) {}
    SdrObjPrintableItem(SvStream& rIn): SdrYesNoItem(SDRATTR_OBJPRINTABLE,rIn) {}
};

class SdrObjVisibleItem: public SdrYesNoItem {
public:
    SdrObjVisibleItem(bool bOn=true): SdrYesNoItem(SDRATTR_OBJVISIBLE,bOn) {}
    SdrObjVisibleItem(SvStream& rIn): SdrYesNoItem(SDRATTR_OBJVISIBLE,rIn) {}
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
