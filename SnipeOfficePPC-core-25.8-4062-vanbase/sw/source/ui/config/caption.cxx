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


#include <osl/diagnose.h>

#include "numrule.hxx"
#include "caption.hxx"

InsCaptionOpt::InsCaptionOpt(const SwCapObjType eType, const SvGlobalName* pOleId) :
    bUseCaption(sal_False),
    eObjType(eType),
    nNumType(SVX_NUM_ARABIC),
    sNumberSeparator(RTL_CONSTASCII_USTRINGPARAM((". "))),
    nPos(1),
    nLevel(0),
    sSeparator( rtl::OUString(": ") ),
    bIgnoreSeqOpts(sal_False),
    bCopyAttributes(sal_False)
{
    if (pOleId)
        aOleId = *pOleId;
}

InsCaptionOpt::InsCaptionOpt(const InsCaptionOpt& rOpt)
{
    *this = rOpt;
}

InsCaptionOpt::~InsCaptionOpt()
{
}

InsCaptionOpt& InsCaptionOpt::operator=( const InsCaptionOpt& rOpt )
{
    bUseCaption = rOpt.bUseCaption;
    eObjType = rOpt.eObjType;
    aOleId = rOpt.aOleId;
    sCategory = rOpt.sCategory;
    nNumType = rOpt.nNumType;
    sNumberSeparator = rOpt.sNumberSeparator;
    sCaption = rOpt.sCaption;
    nPos = rOpt.nPos;
    nLevel = rOpt.nLevel;
    sSeparator = rOpt.sSeparator;
    bIgnoreSeqOpts = rOpt.bIgnoreSeqOpts;
    sCharacterStyle = rOpt.sCharacterStyle;
    bCopyAttributes = rOpt.bCopyAttributes;

    return *this;
}

sal_Bool InsCaptionOpt::operator==( const InsCaptionOpt& rOpt ) const
{
    return (eObjType == rOpt.eObjType &&
            aOleId == rOpt.aOleId); // So that identical Ole-IDs can't be added multiple
                                    // times, don't compare against anything else.


}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
