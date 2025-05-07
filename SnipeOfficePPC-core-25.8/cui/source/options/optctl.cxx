/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include "optctl.hxx"
#include <dialmgr.hxx>
#include "optctl.hrc"
#include <cuires.hrc>
#include <svl/ctloptions.hxx>

// class SvxCTLOptionsPage -----------------------------------------------------

IMPL_LINK_NOARG(SvxCTLOptionsPage, SequenceCheckingCB_Hdl)
{
    sal_Bool bIsSequenceChecking = m_aSequenceCheckingCB.IsChecked();
    m_aRestrictedCB.Enable( bIsSequenceChecking );
    m_aTypeReplaceCB.Enable( bIsSequenceChecking );
    // #i48117#: by default restricted and type&replace have to be switched on
    if(bIsSequenceChecking)
    {
        m_aTypeReplaceCB.Check( sal_True );
        m_aRestrictedCB.Check( sal_True );
    }
    return 0;
}

SvxCTLOptionsPage::SvxCTLOptionsPage( Window* pParent, const SfxItemSet& rSet ) :

    SfxTabPage( pParent, CUI_RES( RID_SVXPAGE_OPTIONS_CTL ), rSet ),

    m_aSequenceCheckingFL   ( this, CUI_RES( FL_SEQUENCECHECKING ) ),
    m_aSequenceCheckingCB   ( this, CUI_RES( CB_SEQUENCECHECKING ) ),
    m_aRestrictedCB         ( this, CUI_RES( CB_RESTRICTED ) ),
    m_aTypeReplaceCB        ( this, CUI_RES( CB_TYPE_REPLACE ) ),
    m_aCursorControlFL      ( this, CUI_RES( FL_CURSORCONTROL ) ),
    m_aMovementFT           ( this, CUI_RES( FT_MOVEMENT ) ),
    m_aMovementLogicalRB    ( this, CUI_RES( RB_MOVEMENT_LOGICAL ) ),
    m_aMovementVisualRB     ( this, CUI_RES( RB_MOVEMENT_VISUAL ) ),
    m_aGeneralFL            ( this, CUI_RES( FL_GENERAL ) ),
    m_aNumeralsFT           ( this, CUI_RES( FT_NUMERALS ) ),
    m_aNumeralsLB           ( this, CUI_RES( LB_NUMERALS ) )

{
    FreeResource();

    m_aSequenceCheckingCB.SetClickHdl( LINK( this, SvxCTLOptionsPage, SequenceCheckingCB_Hdl ) );

    m_aNumeralsLB.SetDropDownLineCount( m_aNumeralsLB.GetEntryCount() );
}
// -----------------------------------------------------------------------------
SvxCTLOptionsPage::~SvxCTLOptionsPage()
{
}
// -----------------------------------------------------------------------------
SfxTabPage* SvxCTLOptionsPage::Create( Window* pParent, const SfxItemSet& rAttrSet )
{
    return new SvxCTLOptionsPage( pParent, rAttrSet );
}
// -----------------------------------------------------------------------------
sal_Bool SvxCTLOptionsPage::FillItemSet( SfxItemSet& )
{
    sal_Bool bModified = sal_False;
    SvtCTLOptions aCTLOptions;

    // Sequence checking
    sal_Bool bChecked = m_aSequenceCheckingCB.IsChecked();
    if ( bChecked != m_aSequenceCheckingCB.GetSavedValue() )
    {
        aCTLOptions.SetCTLSequenceChecking( bChecked );
        bModified = sal_True;
    }

    bChecked = m_aRestrictedCB.IsChecked();
    if( bChecked != m_aRestrictedCB.GetSavedValue() )
    {
        aCTLOptions.SetCTLSequenceCheckingRestricted( bChecked );
        bModified = sal_True;
    }
    bChecked = m_aTypeReplaceCB.IsChecked();
    if( bChecked != m_aTypeReplaceCB.GetSavedValue())
    {
        aCTLOptions.SetCTLSequenceCheckingTypeAndReplace(bChecked);
        bModified = sal_True;
    }

    sal_Bool bLogicalChecked = m_aMovementLogicalRB.IsChecked();
    sal_Bool bVisualChecked = m_aMovementVisualRB.IsChecked();
    if ( bLogicalChecked != m_aMovementLogicalRB.GetSavedValue() ||
         bVisualChecked != m_aMovementVisualRB.GetSavedValue() )
    {
        SvtCTLOptions::CursorMovement eMovement =
            bLogicalChecked ? SvtCTLOptions::MOVEMENT_LOGICAL : SvtCTLOptions::MOVEMENT_VISUAL;
        aCTLOptions.SetCTLCursorMovement( eMovement );
        bModified = sal_True;
    }

    sal_uInt16 nPos = m_aNumeralsLB.GetSelectEntryPos();
    if ( nPos != m_aNumeralsLB.GetSavedValue() )
    {
        aCTLOptions.SetCTLTextNumerals( (SvtCTLOptions::TextNumerals)nPos );
        bModified = sal_True;
    }

    return bModified;
}
// -----------------------------------------------------------------------------
void SvxCTLOptionsPage::Reset( const SfxItemSet& )
{
    SvtCTLOptions aCTLOptions;

    m_aSequenceCheckingCB.Check( aCTLOptions.IsCTLSequenceChecking() );
    m_aRestrictedCB.Check( aCTLOptions.IsCTLSequenceCheckingRestricted() );
    m_aTypeReplaceCB.Check( aCTLOptions.IsCTLSequenceCheckingTypeAndReplace() );

    SvtCTLOptions::CursorMovement eMovement = aCTLOptions.GetCTLCursorMovement();
    switch ( eMovement )
    {
        case SvtCTLOptions::MOVEMENT_LOGICAL :
            m_aMovementLogicalRB.Check();
            break;

        case SvtCTLOptions::MOVEMENT_VISUAL :
            m_aMovementVisualRB.Check();
            break;

        default:
            SAL_WARN( "cui.options", "SvxCTLOptionsPage::Reset(): invalid movement enum" );
    }

    sal_uInt16 nPos = (sal_uInt16)aCTLOptions.GetCTLTextNumerals();
    DBG_ASSERT( nPos < m_aNumeralsLB.GetEntryCount(), "SvxCTLOptionsPage::Reset(): invalid numerals enum" );
    m_aNumeralsLB.SelectEntryPos( nPos );

    m_aSequenceCheckingCB.SaveValue();
    m_aRestrictedCB.SaveValue();
    m_aTypeReplaceCB.SaveValue();
    m_aMovementLogicalRB.SaveValue();
    m_aMovementVisualRB.SaveValue();
    m_aNumeralsLB.SaveValue();

    sal_Bool bIsSequenceChecking = m_aSequenceCheckingCB.IsChecked();
    m_aRestrictedCB.Enable( bIsSequenceChecking );
    m_aTypeReplaceCB.Enable( bIsSequenceChecking );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
