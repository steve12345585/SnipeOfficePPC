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

#include <svl/itempool.hxx>

#include <sfx2/ctrlitem.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/msgpool.hxx>
#include "statcach.hxx"
#include <sfx2/viewfrm.hxx>

//====================================================================

DBG_NAME(SfxControllerItem);

//--------------------------------------------------------------------
#ifdef DBG_UTIL

void SfxControllerItem::CheckConfigure_Impl( sal_uIntPtr nType )
{
    // Real Slot? (i.e. no Separator etc.)
    if ( !nId )
        return;

    // is the ID configurable at all in 'nType'?
    const SfxSlot *pSlot = SFX_SLOTPOOL().GetSlot(nId);
    DBG_ASSERTWARNING( pSlot, "SfxControllerItem: binding not existing slot" );
    if ( pSlot && !pSlot->IsMode(nType) )
    {
        DBG_WARNING( "SfxControllerItem: slot without ...Config-flag" );
        DbgOutf( "SfxControllerItem: Config-flag missing at SID %5d",
                 pSlot->GetSlotId() );
    }
}

#endif

//--------------------------------------------------------------------
// returns the next registered SfxControllerItem with the same id

SfxControllerItem* SfxControllerItem::GetItemLink()
{
    DBG_CHKTHIS(SfxControllerItem, 0);
    return pNext == this ? 0 : pNext;
}

//--------------------------------------------------------------------
// returns sal_True if this binding is really bound to a function

sal_Bool SfxControllerItem::IsBound() const
{
    DBG_CHKTHIS(SfxControllerItem, 0);
    return pNext != this;
}

//====================================================================
// registeres with the id at the bindings

void SfxControllerItem::Bind( sal_uInt16 nNewId, SfxBindings *pBindinx )
{
    DBG_CHKTHIS(SfxControllerItem, 0);
    DBG_ASSERT(pBindings || pBindinx, "No Bindings");

    if ( IsBound() ) {
        DBG_ASSERT(pBindings, "No Bindings");
        pBindings->Release(*this);
    }

    nId = nNewId;
    pNext = 0;

    if (pBindinx)
        pBindings = pBindinx;
    pBindings->Register(*this);
}

void SfxControllerItem::BindInternal_Impl( sal_uInt16 nNewId, SfxBindings *pBindinx )
{
    DBG_CHKTHIS(SfxControllerItem, 0);
    DBG_ASSERT(pBindings || pBindinx, "No Bindings");

    if ( IsBound() ) {
        DBG_ASSERT(pBindings, "No Bindings");
        pBindings->Release(*this);
    }

    nId = nNewId;
    pNext = 0;

    if (pBindinx)
        pBindings = pBindinx;
    pBindings->RegisterInternal_Impl(*this);
}


//====================================================================

void SfxControllerItem::UnBind()

/*  [Description]

    Unbinds the connection of this SfxControllerItems with the SfxBindings
    instance with which it to time is bound. From this time on it does not
    receive any status notifications (<SfxControllerItem::StateChented()>)
    anymore.

    [Cross-reference]

    <SfxControllerItem::ReBind()>
    <SfxControllerItem::ClearCache()>
*/
{
    DBG_CHKTHIS(SfxControllerItem, 0);
    DBG_ASSERT(pBindings, "No Bindings");
    DBG_ASSERT( IsBound(), "unbindings unbound SfxControllerItem" );

    pBindings->Release(*this);
    pNext = this;
}

//====================================================================

void SfxControllerItem::ReBind()

/*  [Description]

    Binds this SfxControllerItem with the SfxBindings instance again,
    with which it was last bound. From this time on it does receive status
    notifications (<SfxControllerItem::StateChented()>) again.

    [Cross-reference]

    <SfxControllerItem::UnBind()>
    <SfxControllerItem::ClearCache()>
*/

{
DBG_CHKTHIS(SfxControllerItem, 0);
    DBG_ASSERT(pBindings, "No Bindings");
    DBG_ASSERT( !IsBound(), "bindings rebound SfxControllerItem" );

    pBindings->Register(*this);
}

//--------------------------------------------------------------------

void SfxControllerItem::ClearCache()

/*  [Description]

    Clears the cache status for this SfxControllerItem. That is by the next
    status update is the <SfxPoolItem> sent in any case, even if the same was
    sent before. This is needed if a controller can be switched on and note
    that status themselves.

    [Example]

    The combined controller for adjusting the surface type and the concrete
    expression (blue color, or hatching X) can be changed in type, but is then
    notified of the next selection again, even if it the same data.

    [Cross-reference]

    <SfxControllerItem::UnBind()>
    <SfxControllerItem::ReBind()>
*/


{
    DBG_CHKTHIS(SfxControllerItem, 0);
    DBG_ASSERT(pBindings, "No Bindings");

    pBindings->ClearCache_Impl( GetId() );
}

//--------------------------------------------------------------------
// replaces the successor in the list of bindings of the same id

SfxControllerItem* SfxControllerItem::ChangeItemLink( SfxControllerItem* pNewLink )
{
    DBG_CHKTHIS(SfxControllerItem, 0);
    SfxControllerItem* pOldLink = pNext;
    pNext = pNewLink;
    return pOldLink == this ? 0 : pOldLink;
}

//--------------------------------------------------------------------
// changes the id of unbound functions (e.g. for sub-menu-ids)

void SfxControllerItem::SetId( sal_uInt16 nItemId )
{
    DBG_CHKTHIS(SfxControllerItem, 0);
    DBG_ASSERT( !IsBound(), "changing id of bound binding" );
    nId = nItemId;
}

//--------------------------------------------------------------------

// creates a atomic item for a controller without registration.

SfxControllerItem::SfxControllerItem():
    nId(0),
    pNext(this),
    pBindings(0)
{
    DBG_CTOR(SfxControllerItem, 0);
}

//--------------------------------------------------------------------
// creates a representation of the function nId and registeres it

SfxControllerItem::SfxControllerItem( sal_uInt16 nID, SfxBindings &rBindings ):
    nId(nID),
    pNext(this),
    pBindings(&rBindings)
{
    DBG_CTOR(SfxControllerItem, 0);
    Bind(nId, &rBindings);
}

//--------------------------------------------------------------------
// unregisteres the item in the bindings

SfxControllerItem::~SfxControllerItem()
{
    if ( IsBound() )
        pBindings->Release(*this);
    DBG_DTOR(SfxControllerItem, 0);
}

//--------------------------------------------------------------------

void SfxControllerItem::StateChanged
(
    sal_uInt16,          // <SID> of the triggering slot
    SfxItemState,       // <SfxItemState> of 'pState'
    const SfxPoolItem*  // Slot-Status, NULL or IsInvalidItem()
)

/*  [Description]

    This virtual method is called by the SFx to inform the <SfxControllerItem>s
    is about that state of the slots 'NSID' has changed. The new value and the
    value determined by this status is given as 'pState' or 'eState'.

    The status of a slot may change, for example when the MDI window is
    switched or when the slot was invalidated explicitly with
    <SfxBindings::Invalidate()>.

    Achtung! Die Methode wird nicht gerufen, wenn der Slot ung"ultig wurde,
    danach jedoch wieder denselben Wert angenommen hat.

    Beware! The method is not called when the slot is invalid, however
    has again assumed the same value.

    This base class need not be called, further interim steps however
    (eg <SfxToolboxControl> ) should be called.
*/

{
    DBG_CHKTHIS(SfxControllerItem, 0);
}

//--------------------------------------------------------------------

void SfxControllerItem::DeleteFloatingWindow()
{
    DBG_CHKTHIS(SfxControllerItem, 0);
}

//--------------------------------------------------------------------

void SfxStatusForwarder::StateChanged
(
    sal_uInt16          nSID,    // <SID> of the triggering slot
    SfxItemState        eState,  // <SfxItemState> of 'pState'
    const SfxPoolItem*  pState   // Slot-Status, NULL or IsInvalidItem()
)

{
    pMaster->StateChanged( nSID, eState, pState );
}

//--------------------------------------------------------------------

SfxStatusForwarder::SfxStatusForwarder(
            sal_uInt16              nSlotId,
            SfxControllerItem&  rMaster ):
    SfxControllerItem( nSlotId, rMaster.GetBindings() ),
    pMaster( &rMaster )
{
}

//--------------------------------------------------------------------

SfxItemState SfxControllerItem::GetItemState
(
    const SfxPoolItem* pState   /*  Pointer to  <SfxPoolItem>, which
                                    Status should be queried. */
)

/*  [Description]

    Static method to determine the status of the SfxPoolItem-Pointers, to be
    used in the method <SfxControllerItem::StateChanged(const SfxPoolItem*)>

    [Return value]

    SfxItemState        SFX_ITEM_UNKNOWN
                        Enabled, but no further status information available.
                        Typical for <Slot>s, which anyway are sometimes
                        disabled, but otherwise do not change their appearance.

                        SFX_ITEM_DISABLED
                        Disabled and no further status information available.
                        All other values that may appear should be reset to
                        default.

                        SFX_ITEM_DONTCARE
                        Enabled but there were only ambiguous values available
                        (i.e. non that can be queried).

                        SFX_ITEM_AVAILABLE
                        Enabled and with available values, which are queried
                        by 'pState'. The Type is thus clearly defined in the
                        entire Program and specified through the Slot.
*/

{
    return !pState
                ? SFX_ITEM_DISABLED
                : IsInvalidItem(pState)
                    ? SFX_ITEM_DONTCARE
                    : pState->ISA(SfxVoidItem) && !pState->Which()
                        ? SFX_ITEM_UNKNOWN
                        : SFX_ITEM_AVAILABLE;
}

//------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma optimize("g",off)
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
