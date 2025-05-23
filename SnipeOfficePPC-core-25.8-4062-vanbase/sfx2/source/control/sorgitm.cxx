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


#include <sfx2/sfxsids.hrc>
#include "sorgitm.hxx"
// STATIC DATA -----------------------------------------------------------

TYPEINIT1_AUTOFACTORY(SfxScriptOrganizerItem, SfxStringItem);

//------------------------------------------------------------------------

SfxScriptOrganizerItem::SfxScriptOrganizerItem() :

    SfxStringItem()

{
}

//------------------------------------------------------------------------

SfxScriptOrganizerItem::SfxScriptOrganizerItem( const SfxScriptOrganizerItem& rItem ) :

    SfxStringItem( rItem ),

    aLanguage( rItem.aLanguage )

{
}

//------------------------------------------------------------------------

SfxScriptOrganizerItem::~SfxScriptOrganizerItem()
{
}

//------------------------------------------------------------------------

SfxPoolItem* SfxScriptOrganizerItem::Clone( SfxItemPool * ) const
{
    return new SfxScriptOrganizerItem( *this );
}

//------------------------------------------------------------------------

int SfxScriptOrganizerItem::operator==( const SfxPoolItem& rItem) const
{
     return rItem.Type() == Type() &&
         SfxStringItem::operator==(rItem) &&
         aLanguage == ((const SfxScriptOrganizerItem &)rItem).aLanguage;
}


bool SfxScriptOrganizerItem::QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId ) const
{
    String aValue;
    sal_Bool bIsString = sal_False;
    sal_Bool bValue = sal_False;
    nMemberId &= ~CONVERT_TWIPS;
    switch ( nMemberId )
    {
        case 0:
        case MID_SCRIPT_ORGANIZER_LANGUAGE:
            bIsString = sal_True;
            aValue = aLanguage;
            break;
        default:
            OSL_FAIL("Wrong MemberId!");
               return false;
     }

    if ( bIsString )
        rVal <<= ::rtl::OUString( aValue );
    else
        rVal <<= bValue;
    return true;
}

bool SfxScriptOrganizerItem::PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId )
{
    ::rtl::OUString aValue;
    sal_Bool bRet = false;
    nMemberId &= ~CONVERT_TWIPS;
    switch ( nMemberId )
    {
        case 0:
        case MID_SCRIPT_ORGANIZER_LANGUAGE:
            bRet = (rVal >>= aValue);
            if ( bRet )
                aLanguage = aValue;
            break;
        default:
            OSL_FAIL("Wrong MemberId!");
            return false;
    }

    return bRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
