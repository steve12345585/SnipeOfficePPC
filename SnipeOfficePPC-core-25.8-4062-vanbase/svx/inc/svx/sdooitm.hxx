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
#ifndef _SDOOITM_HXX
#define _SDOOITM_HXX

#include <svl/eitem.hxx>
#include "svx/svxdllapi.h"


//------------------------------------------------------------
// class SdrOnOffItem
// here GetValueTextByVal() returns "an" or "aus" instead
// of "TRUE" or "FALSE"
//------------------------------------------------------------
class SVX_DLLPUBLIC SdrOnOffItem: public SfxBoolItem {
public:
    TYPEINFO();
    SdrOnOffItem(): SfxBoolItem() {}
    SdrOnOffItem(sal_uInt16 nId, sal_Bool bOn=sal_False): SfxBoolItem(nId,bOn) {}
    SdrOnOffItem(sal_uInt16 nId, SvStream& rIn):  SfxBoolItem(nId,rIn) {}
    virtual SfxPoolItem* Clone(SfxItemPool* pPool=NULL) const;
    virtual SfxPoolItem* Create(SvStream& rIn, sal_uInt16 nVer) const;

    virtual rtl::OUString GetValueTextByVal(sal_Bool bVal) const;

    virtual SfxItemPresentation GetPresentation(SfxItemPresentation ePres, SfxMapUnit eCoreMetric, SfxMapUnit ePresMetric, String& rText, const IntlWrapper * = 0) const;
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
