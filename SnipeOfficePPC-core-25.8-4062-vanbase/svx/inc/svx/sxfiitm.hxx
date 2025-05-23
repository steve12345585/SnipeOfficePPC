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
#ifndef _SFXIITM_HXX
#define _SFXIITM_HXX

#include <svl/poolitem.hxx>
#include <tools/fract.hxx>

/*************************************************************************/
/* FractionItem                                                          */
/*************************************************************************/

class SdrFractionItem: public SfxPoolItem {
    Fraction nValue;
public:
    TYPEINFO_VISIBILITY( SVX_DLLPUBLIC );
    SdrFractionItem(sal_uInt16 nId=0): SfxPoolItem(nId) {}
    SdrFractionItem(sal_uInt16 nId, const Fraction& rVal): SfxPoolItem(nId), nValue(rVal) {}
    SdrFractionItem(sal_uInt16 nId, SvStream& rIn);
    virtual int              operator==(const SfxPoolItem&) const;
    virtual SfxItemPresentation GetPresentation(SfxItemPresentation ePresentation, SfxMapUnit eCoreMetric, SfxMapUnit ePresentationMetric, String &rText, const IntlWrapper * = 0) const;
    virtual SfxPoolItem*     Create(SvStream&, sal_uInt16 nVer) const;
    virtual SvStream&        Store(SvStream&, sal_uInt16 nItemVers) const;
    virtual SfxPoolItem*     Clone(SfxItemPool *pPool=NULL) const;
            const Fraction&  GetValue() const { return nValue; }
            void             SetValue(const Fraction& rVal) { nValue = rVal; }
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
