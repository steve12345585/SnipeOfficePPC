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
#ifndef _SDTFCHIM_HXX
#define _SDTFCHIM_HXX

#include <svx/svddef.hxx>

class SVX_DLLPUBLIC SdrTextFixedCellHeightItem : public SfxBoolItem
{
public:

    TYPEINFO();
    SdrTextFixedCellHeightItem( sal_Bool bUseFixedCellHeight = sal_False );
    SVX_DLLPRIVATE SdrTextFixedCellHeightItem( SvStream & rStream, sal_uInt16 nVersion );

    SVX_DLLPRIVATE virtual SfxItemPresentation GetPresentation(SfxItemPresentation ePresentation,
                                    SfxMapUnit eCoreMetric, SfxMapUnit ePresentationMetric,
                                        String &rText, const IntlWrapper * = 0) const;

    SVX_DLLPRIVATE virtual SfxPoolItem*     Create( SvStream&, sal_uInt16 nItem ) const;
    SVX_DLLPRIVATE virtual SvStream&            Store( SvStream&, sal_uInt16 nVersion ) const;
    SVX_DLLPRIVATE virtual SfxPoolItem*     Clone( SfxItemPool* pPool = NULL ) const;
    SVX_DLLPRIVATE virtual  sal_uInt16          GetVersion( sal_uInt16 nFileFormatVersion ) const;

    SVX_DLLPRIVATE virtual  bool QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const;
    SVX_DLLPRIVATE virtual  bool PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
