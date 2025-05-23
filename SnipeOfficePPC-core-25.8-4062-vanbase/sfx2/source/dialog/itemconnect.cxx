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

#include <sfx2/itemconnect.hxx>

#include <boost/shared_ptr.hpp>
#include <list>
#include <svl/itempool.hxx>

// ============================================================================

namespace sfx {

// ============================================================================
// Helpers
// ============================================================================

namespace {

TriState lclConvertToTriState( bool bKnown, bool bIsKnownFlag, bool bIsUnknownFlag )
{
    return (bKnown && bIsKnownFlag) ? STATE_CHECK : ((!bKnown && bIsUnknownFlag) ? STATE_NOCHECK : STATE_DONTKNOW);
}

} // namespace

// ----------------------------------------------------------------------------

sal_uInt16 ItemWrapperHelper::GetWhichId( const SfxItemSet& rItemSet, sal_uInt16 nSlot )
{
    return rItemSet.GetPool()->GetWhich( nSlot );
}

bool ItemWrapperHelper::IsKnownItem( const SfxItemSet& rItemSet, sal_uInt16 nSlot )
{
    return rItemSet.GetItemState( GetWhichId( rItemSet, nSlot ), sal_True ) != SFX_ITEM_UNKNOWN;
}

const SfxPoolItem* ItemWrapperHelper::GetUniqueItem( const SfxItemSet& rItemSet, sal_uInt16 nSlot )
{
    sal_uInt16 nWhich = GetWhichId( rItemSet, nSlot );
    return (rItemSet.GetItemState( nWhich, sal_True ) >= SFX_ITEM_DEFAULT) ? rItemSet.GetItem( nWhich, sal_True ) : 0;
}

const SfxPoolItem& ItemWrapperHelper::GetDefaultItem( const SfxItemSet& rItemSet, sal_uInt16 nSlot )
{
    return rItemSet.GetPool()->GetDefaultItem( GetWhichId( rItemSet, nSlot ) );
}

void ItemWrapperHelper::RemoveDefaultItem( SfxItemSet& rDestSet, const SfxItemSet& rOldSet, sal_uInt16 nSlot )
{
    sal_uInt16 nWhich = GetWhichId( rDestSet, nSlot );
    if( rOldSet.GetItemState( nWhich, sal_False ) == SFX_ITEM_DEFAULT )
        rDestSet.ClearItem( nWhich );
}

// ============================================================================
// Base control wrapper classes
// ============================================================================

ControlWrapperBase::~ControlWrapperBase()
{
}

// ============================================================================
// Single control wrappers
// ============================================================================

DummyWindowWrapper::DummyWindowWrapper( Window& rWindow ) :
    SingleControlWrapperType( rWindow )
{
}

bool DummyWindowWrapper::IsControlDontKnow() const
{
    return false;
}

void DummyWindowWrapper::SetControlDontKnow( bool )
{
}

void* DummyWindowWrapper::GetControlValue() const
{
    return 0;
}

void DummyWindowWrapper::SetControlValue( void* )
{
}

// ----------------------------------------------------------------------------

CheckBoxWrapper::CheckBoxWrapper( CheckBox& rCheckBox ) :
        SingleControlWrapperType( rCheckBox )
{
}

bool CheckBoxWrapper::IsControlDontKnow() const
{
    return GetControl().GetState() == STATE_DONTKNOW;
}

void CheckBoxWrapper::SetControlDontKnow( bool bSet )
{
    GetControl().EnableTriState( bSet );
    GetControl().SetState( bSet ? STATE_DONTKNOW : STATE_NOCHECK );
}

sal_Bool CheckBoxWrapper::GetControlValue() const
{
    return GetControl().IsChecked();
}

void CheckBoxWrapper::SetControlValue( sal_Bool bValue )
{
    GetControl().Check( bValue );
}

// ----------------------------------------------------------------------------

ColorListBoxWrapper::ColorListBoxWrapper(ColorListBox & rListBox):
    SingleControlWrapper< ColorListBox, Color >(rListBox)
{}

ColorListBoxWrapper::~ColorListBoxWrapper()
{}

bool ColorListBoxWrapper::IsControlDontKnow() const
{
    return GetControl().GetSelectEntryCount() == 0;
}

void ColorListBoxWrapper::SetControlDontKnow( bool bSet )
{
    if( bSet ) GetControl().SetNoSelection();
}

Color ColorListBoxWrapper::GetControlValue() const
{
    return GetControl().GetSelectEntryColor();
}

void ColorListBoxWrapper::SetControlValue( Color aColor )
{
    GetControl().SelectEntry( aColor );
}

// ============================================================================
// Multi control wrappers
// ============================================================================

typedef std::vector< ControlWrapperBase* >  ControlWrpVec;
typedef ControlWrpVec::iterator             ControlWrpVecI;
typedef ControlWrpVec::const_iterator       ControlWrpVecCI;

struct MultiControlWrapperHelper_Impl
{
    ControlWrpVec       maVec;
};

MultiControlWrapperHelper::MultiControlWrapperHelper() :
    mxImpl( new MultiControlWrapperHelper_Impl )
{
}

MultiControlWrapperHelper::~MultiControlWrapperHelper()
{
}

void MultiControlWrapperHelper::RegisterControlWrapper( ControlWrapperBase& rWrapper )
{
    mxImpl->maVec.push_back( &rWrapper );
}

void MultiControlWrapperHelper::ModifyControl( TriState eEnable, TriState eShow )
{
    for( ControlWrpVecI aIt = mxImpl->maVec.begin(), aEnd = mxImpl->maVec.end(); aIt != aEnd; ++aIt )
        (*aIt)->ModifyControl( eEnable, eShow );
}

bool MultiControlWrapperHelper::IsControlDontKnow() const
{
    bool bIs = !mxImpl->maVec.empty();
    for( ControlWrpVecCI aIt = mxImpl->maVec.begin(), aEnd = mxImpl->maVec.end(); bIs && (aIt != aEnd); ++aIt )
        bIs &= (*aIt)->IsControlDontKnow();
    return bIs;
}

void MultiControlWrapperHelper::SetControlDontKnow( bool bSet )
{
    for( ControlWrpVecI aIt = mxImpl->maVec.begin(), aEnd = mxImpl->maVec.end(); aIt != aEnd; ++aIt )
        (*aIt)->SetControlDontKnow( bSet );
}

// ============================================================================
// Base connection classes
// ============================================================================

ItemConnectionBase::ItemConnectionBase( ItemConnFlags nFlags ) :
    mnFlags( nFlags )
{
}

ItemConnectionBase::~ItemConnectionBase()
{
}

bool ItemConnectionBase::IsActive() const
{
    return !(mnFlags & ITEMCONN_INACTIVE);
}

void ItemConnectionBase::DoApplyFlags( const SfxItemSet& rItemSet )
{
    if( IsActive() )
        ApplyFlags( rItemSet );
}

void ItemConnectionBase::DoReset( const SfxItemSet& rItemSet )
{
    if( IsActive() )
        Reset( rItemSet );
}

bool ItemConnectionBase::DoFillItemSet( SfxItemSet& rDestSet, const SfxItemSet& rOldSet )
{
    return IsActive() && FillItemSet( rDestSet, rOldSet );
}

TriState ItemConnectionBase::GetEnableState( bool bKnown ) const
{
    return lclConvertToTriState( bKnown, mnFlags & ITEMCONN_ENABLE_KNOWN, mnFlags & ITEMCONN_DISABLE_UNKNOWN );
}

TriState ItemConnectionBase::GetShowState( bool bKnown ) const
{
    return lclConvertToTriState( bKnown, mnFlags & ITEMCONN_SHOW_KNOWN, mnFlags & ITEMCONN_HIDE_UNKNOWN );
}

// ============================================================================
// Standard connections
// ============================================================================

DummyItemConnection::DummyItemConnection( sal_uInt16 nSlot, Window& rWindow, ItemConnFlags nFlags ) :
    ItemConnectionBase( nFlags ),
    DummyWindowWrapper( rWindow ),
    mnSlot( nSlot )
{
}

void DummyItemConnection::ApplyFlags( const SfxItemSet& rItemSet )
{
    bool bKnown = ItemWrapperHelper::IsKnownItem( rItemSet, mnSlot );
    ModifyControl( GetEnableState( bKnown ), GetShowState( bKnown ) );
}

void DummyItemConnection::Reset( const SfxItemSet& /*rItemSet*/ )
{
}

bool DummyItemConnection::FillItemSet( SfxItemSet& /*rDestSet*/, const SfxItemSet& /*rOldSet*/ )
{
    return false;   // item set not changed
}

// ============================================================================
// Array of connections
// ============================================================================

class ItemConnectionArrayImpl
{
public:
    void                        Append( ItemConnectionBase* pConnection );

    void                        ApplyFlags( const SfxItemSet& rItemSet );
    void                        Reset( const SfxItemSet& rItemSet );
    bool                        FillItemSet( SfxItemSet& rDestSet, const SfxItemSet& rOldSet );

private:
    typedef boost::shared_ptr< ItemConnectionBase > ItemConnectionRef;
    typedef std::list< ItemConnectionRef >          ItemConnectionList;
    typedef ItemConnectionList::iterator            ItemConnectionListIt;

    ItemConnectionList          maList;
};

void ItemConnectionArrayImpl::Append( ItemConnectionBase* pConnection )
{
    if( pConnection )
        maList.push_back( ItemConnectionRef( pConnection ) );
}

void ItemConnectionArrayImpl::ApplyFlags( const SfxItemSet& rItemSet )
{
    for( ItemConnectionListIt aIt = maList.begin(), aEnd = maList.end(); aIt != aEnd; ++aIt )
        (*aIt)->DoApplyFlags( rItemSet );
}

void ItemConnectionArrayImpl::Reset( const SfxItemSet& rItemSet )
{
    for( ItemConnectionListIt aIt = maList.begin(), aEnd = maList.end(); aIt != aEnd; ++aIt )
        (*aIt)->DoReset( rItemSet );
}

bool ItemConnectionArrayImpl::FillItemSet( SfxItemSet& rDestSet, const SfxItemSet& rOldSet )
{
    bool bChanged = false;
    for( ItemConnectionListIt aIt = maList.begin(), aEnd = maList.end(); aIt != aEnd; ++aIt )
        bChanged |= (*aIt)->DoFillItemSet( rDestSet, rOldSet );
    return bChanged;
}

// ----------------------------------------------------------------------------

ItemConnectionArray::ItemConnectionArray() :
    mxImpl( new ItemConnectionArrayImpl )
{
}

ItemConnectionArray::~ItemConnectionArray()
{
}

void ItemConnectionArray::AddConnection( ItemConnectionBase* pConnection )
{
    mxImpl->Append( pConnection );
}

void ItemConnectionArray::ApplyFlags( const SfxItemSet& rItemSet )
{
    mxImpl->ApplyFlags( rItemSet );
}

void ItemConnectionArray::Reset( const SfxItemSet& rItemSet )
{
    mxImpl->Reset( rItemSet );
}

bool ItemConnectionArray::FillItemSet( SfxItemSet& rDestSet, const SfxItemSet& rOldSet )
{
    return mxImpl->FillItemSet( rDestSet, rOldSet );
}

// ============================================================================

} // namespace sfx

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
