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


#undef SC_DLLIMPLEMENTATION

#include <svl/eitem.hxx>

#include "tpprint.hxx"
#include "printopt.hxx"
#include "scmod.hxx"
#include "scresid.hxx"
#include "sc.hrc"
#include "optdlg.hrc"

// -----------------------------------------------------------------------

ScTpPrintOptions::ScTpPrintOptions( Window*           pParent,
                                    const SfxItemSet& rCoreAttrs )
    :   SfxTabPage      ( pParent,
                          ScResId( RID_SCPAGE_PRINT ),
                          rCoreAttrs ),
        aPagesFL         ( this, ScResId( FL_PAGES ) ),
        aSkipEmptyPagesCB( this, ScResId( BTN_SKIPEMPTYPAGES ) ),
        aSheetsFL        ( this, ScResId( FL_SHEETS ) ),
        aSelectedSheetsCB( this, ScResId( BTN_SELECTEDSHEETS ) )
{
    FreeResource();
}

ScTpPrintOptions::~ScTpPrintOptions()
{
}

SfxTabPage* ScTpPrintOptions::Create( Window* pParent, const SfxItemSet& rAttrSet )
{
    return new ScTpPrintOptions( pParent, rAttrSet );
}

int ScTpPrintOptions::DeactivatePage( SfxItemSet* pSetP )
{
    if ( pSetP )
        FillItemSet( *pSetP );

    return LEAVE_PAGE;
}

// -----------------------------------------------------------------------

void ScTpPrintOptions::Reset( const SfxItemSet& rCoreSet )
{
    ScPrintOptions aOptions;

    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rCoreSet.GetItemState(SID_SCPRINTOPTIONS, false , &pItem))
        aOptions = ((const ScTpPrintItem*)pItem)->GetPrintOptions();
    else
    {
        // when called from print dialog and no options set, use configuration
        aOptions = SC_MOD()->GetPrintOptions();
    }

    if ( SFX_ITEM_SET == rCoreSet.GetItemState( SID_PRINT_SELECTEDSHEET, false , &pItem ) )
    {
        sal_Bool bChecked = ( (const SfxBoolItem*)pItem )->GetValue();
        aSelectedSheetsCB.Check( bChecked );
    }
    else
    {
        aSelectedSheetsCB.Check( !aOptions.GetAllSheets() );
    }

    aSkipEmptyPagesCB.Check( aOptions.GetSkipEmpty() );
    aSkipEmptyPagesCB.SaveValue();
    aSelectedSheetsCB.SaveValue();
}

// -----------------------------------------------------------------------

sal_Bool ScTpPrintOptions::FillItemSet( SfxItemSet& rCoreAttrs )
{
    rCoreAttrs.ClearItem( SID_PRINT_SELECTEDSHEET );

    bool bSkipEmptyChanged = ( aSkipEmptyPagesCB.GetSavedValue() != aSkipEmptyPagesCB.IsChecked() );
    bool bSelectedSheetsChanged = ( aSelectedSheetsCB.GetSavedValue() != aSelectedSheetsCB.IsChecked() );

    if ( bSkipEmptyChanged || bSelectedSheetsChanged )
    {
        ScPrintOptions aOpt;
        aOpt.SetSkipEmpty( aSkipEmptyPagesCB.IsChecked() );
        aOpt.SetAllSheets( !aSelectedSheetsCB.IsChecked() );
        rCoreAttrs.Put( ScTpPrintItem( SID_SCPRINTOPTIONS, aOpt ) );
        if ( bSelectedSheetsChanged )
        {
            rCoreAttrs.Put( SfxBoolItem( SID_PRINT_SELECTEDSHEET, aSelectedSheetsCB.IsChecked() ) );
        }
        return sal_True;
    }
    else
    {
        return false;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
