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
#ifndef _SDTFSITM_HXX
#define _SDTFSITM_HXX

#include <svl/eitem.hxx>
#include <svx/svddef.hxx>
#include "svx/svxdllapi.h"

enum SdrFitToSizeType {
    SDRTEXTFIT_NONE,         // - no fit-to-size
    SDRTEXTFIT_PROPORTIONAL, // - resize all glyhs proportionally
                             //   (might scale anisotrophically)
    SDRTEXTFIT_ALLLINES,     // - like SDRTEXTFIT_PROPORTIONAL, but
                             //   scales each line separately
    SDRTEXTFIT_AUTOFIT};     // - mimics PPT's automatic adaption of
                             //   font size to text rect - comparable
                             //   to SDRTEXTFIT_PROPORTIONAL, but
                             //   scales isotrophically

// No AutoGrow and no automatic line breaks for
// SDRTEXTFIT_PROPORTIONAL and SDRTEXTFIT_ALLLINES.
// No automatic line breaks for AutoGrowingWidth as well (only if
// TextMaxFrameWidth is reached).

//--------------------------------
// class SdrTextFitToSizeTypeItem
//--------------------------------
class SVX_DLLPUBLIC SdrTextFitToSizeTypeItem: public SfxEnumItem {
public:
    TYPEINFO();
    SdrTextFitToSizeTypeItem(SdrFitToSizeType eFit=SDRTEXTFIT_NONE): SfxEnumItem(SDRATTR_TEXT_FITTOSIZE,(sal_uInt16)eFit) {}
    SdrTextFitToSizeTypeItem(SvStream& rIn)                        : SfxEnumItem(SDRATTR_TEXT_FITTOSIZE,rIn)  {}
    virtual SfxPoolItem*     Clone(SfxItemPool* pPool=NULL) const;
    virtual SfxPoolItem*     Create(SvStream& rIn, sal_uInt16 nVer) const;
    virtual sal_uInt16           GetValueCount() const; // { return 4; }
            SdrFitToSizeType GetValue() const      { return (SdrFitToSizeType)SfxEnumItem::GetValue(); }

    virtual bool QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const;
    virtual bool PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 );

    virtual rtl::OUString GetValueTextByPos(sal_uInt16 nPos) const;
    virtual SfxItemPresentation GetPresentation(SfxItemPresentation ePres, SfxMapUnit eCoreMetric, SfxMapUnit ePresMetric, String& rText, const IntlWrapper * = 0) const;
    virtual int              HasBoolValue() const;
    virtual sal_Bool             GetBoolValue() const;
    virtual void             SetBoolValue(sal_Bool bVal);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
