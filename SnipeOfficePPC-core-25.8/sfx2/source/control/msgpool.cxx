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

#include <tools/stream.hxx>
#include <rsc/rscsfx.hxx>

// due to pSlotPool
#include "appdata.hxx"
#include <sfx2/msgpool.hxx>
#include <sfx2/minarray.hxx>
#include <sfx2/msg.hxx>
#include <sfx2/app.hxx>
#include <sfx2/objface.hxx>
#include "sfxtypes.hxx"
#include "sfx2/sfxresid.hxx"
#include "arrdecl.hxx"
#include <sfx2/module.hxx>

#include <sfx2/sfx.hrc>

SfxSlotPool::SfxSlotPool( SfxSlotPool *pParent, ResMgr* pResManager )
 : _pGroups(0)
 , _pParentPool( pParent )
 , _pResMgr( pResManager )
 , _pInterfaces(0)
 , _nCurGroup(0)
 , _nCurInterface(0)
 , _nCurMsg(0)
{
    if ( !_pResMgr )
        _pResMgr = SfxApplication::GetOrCreate()->GetOffResManager_Impl();
}

//====================================================================

SfxSlotPool::~SfxSlotPool()
{
    _pParentPool = 0;
    for ( SfxInterface *pIF = FirstInterface(); pIF; pIF = FirstInterface() )
        delete pIF;
    delete _pInterfaces;
    delete _pGroups;
}

//====================================================================
// registers the availability of the Interface of functions

void SfxSlotPool::RegisterInterface( SfxInterface& rInterface )
{
    // add to the list of SfxObjectInterface instances
    if ( _pInterfaces == NULL )
        _pInterfaces = new SfxInterfaceArr_Impl;
    _pInterfaces->push_back(&rInterface);

    // Stop at a (single) Null-slot (for syntactic reasons the interfaces
    // always contain at least one slot)
    if ( rInterface.Count() != 0 && !rInterface[0]->nSlotId )
        return;

    // possibly add Interface-id and group-ids of funcs to the list of groups
    if ( !_pGroups )
    {
        _pGroups = new SfxSlotGroupArr_Impl;

        if ( _pParentPool )
        {
            // The Groups in parent Slotpool are also known here
            _pGroups->append( *_pParentPool->_pGroups );
        }
    }

    for ( size_t nFunc = 0; nFunc < rInterface.Count(); ++nFunc )
    {
        SfxSlot *pDef = rInterface[nFunc];
        if ( pDef->GetGroupId() && /* pDef->GetGroupId() != GID_INTERN && */
             _pGroups->find(pDef->GetGroupId()) == SfxSlotGroupArr_Impl::npos )
        {
            if (pDef->GetGroupId() == GID_INTERN)
                _pGroups->insert(_pGroups->begin(), pDef->GetGroupId());
            else
                _pGroups->push_back(pDef->GetGroupId());
        }
    }
}

//====================================================================

TypeId SfxSlotPool::GetSlotType( sal_uInt16 nId ) const
{
    const SfxSlot* pSlot = (const_cast <SfxSlotPool*> (this))->GetSlot( nId );
    return pSlot ? pSlot->GetType()->Type() : 0;
}

//====================================================================
// unregisters the availability of the Interface of functions

void SfxSlotPool::ReleaseInterface( SfxInterface& rInterface )
{
    DBG_ASSERT( _pInterfaces, "releasing SfxInterface, but there are none" );
    // remove from the list of SfxInterface instances
    SfxInterfaceArr_Impl::iterator i = std::find(_pInterfaces->begin(), _pInterfaces->end(), &rInterface);
    if(i != _pInterfaces->end())
        _pInterfaces->erase(i);
}

// get the first SfxMessage for a special Id (e.g. for getting check-mode)

const SfxSlot* SfxSlotPool::GetSlot( sal_uInt16 nId )
{
    DBG_ASSERT( _pInterfaces != NULL, "no Interfaces registered" );

    // First, search their own interfaces
    for ( sal_uInt16 nInterf = 0; nInterf < _pInterfaces->size(); ++nInterf )
    {
        const SfxSlot *pDef = ((*_pInterfaces)[nInterf])->GetSlot(nId);
        if ( pDef )
            return pDef;
    }

    // Then try any of the possible existing parent
    return _pParentPool ? _pParentPool->GetSlot( nId ) : 0;
}

//--------------------------------------------------------------------

// skips to the next group

String SfxSlotPool::SeekGroup( sal_uInt16 nNo )
{
    DBG_ASSERT( _pInterfaces != NULL, "no Interfaces registered" );

    // if the group exists, use it
    if ( _pGroups && nNo < _pGroups->size() )
    {
        _nCurGroup = nNo;
        if ( _pParentPool )
        {
            // In most cases, the order of the IDs agree
            sal_uInt16 nParentCount = _pParentPool->_pGroups->size();
            if ( nNo < nParentCount && (*_pGroups)[nNo] == (*_pParentPool->_pGroups)[nNo] )
                _pParentPool->_nCurGroup = nNo;
            else
            {
                // Otherwise search. If the group is not found in the parent
                // pool, _nCurGroup is set outside the valid range
                sal_uInt16 i;
                for ( i=1; i<nParentCount; i++ )
                    if ( (*_pGroups)[nNo] == (*_pParentPool->_pGroups)[i] )
                        break;
                _pParentPool->_nCurGroup = i;
            }
        }

        SfxResId aResId( (*_pGroups)[_nCurGroup] );
        aResId.SetRT(RSC_STRING);
        if ( !aResId.GetResMgr()->IsAvailable(aResId) )
        {
            OSL_FAIL( "GroupId-Name not defined in SFX!" );
            return String();
        }

        return String( aResId );
    }

    return String();
}


//--------------------------------------------------------------------

sal_uInt16 SfxSlotPool::GetGroupCount()
{
    return _pGroups->size();
}


//--------------------------------------------------------------------

// internal search loop

const SfxSlot* SfxSlotPool::SeekSlot( sal_uInt16 nStartInterface )
{
    DBG_ASSERT( _pInterfaces != NULL, "no Interfaces registered" );

    // The numbering starts at the interfaces of the parent pool
    sal_uInt16 nFirstInterface = _pParentPool ? _pParentPool->_pInterfaces->size() : 0;

    // have reached the end of the Parent-Pools?
    if ( nStartInterface < nFirstInterface &&
         _pParentPool->_nCurGroup >= _pParentPool->_pGroups->size() )
        nStartInterface = nFirstInterface;

    // Is the Interface still in the Parent-Pool?
    if ( nStartInterface < nFirstInterface )
    {
        DBG_ASSERT( _pParentPool, "No parent pool!" );
        _nCurInterface = nStartInterface;
        return _pParentPool->SeekSlot( nStartInterface );
    }

    // find the first func-def with the current group id
    sal_uInt16 nCount = _pInterfaces->size() + nFirstInterface;
    for ( _nCurInterface = nStartInterface;
            _nCurInterface < nCount;
          ++_nCurInterface )
    {
        SfxInterface* pInterface = (*_pInterfaces)[_nCurInterface-nFirstInterface];
        for ( _nCurMsg = 0;
              _nCurMsg < pInterface->Count();
              ++_nCurMsg )
        {
            const SfxSlot* pMsg = (*pInterface)[_nCurMsg];
            if ( pMsg->GetGroupId() == _pGroups->at(_nCurGroup) )
                return pMsg;
        }
    }

    return 0;
}

//--------------------------------------------------------------------
// skips to the next func in the current group

const SfxSlot* SfxSlotPool::NextSlot()
{
    DBG_ASSERT( _pInterfaces != NULL, "no Interfaces registered" );

    // The numbering starts at the interfaces of the parent pool
    sal_uInt16 nFirstInterface = _pParentPool ? _pParentPool->_pInterfaces->size() : 0;

    if ( _nCurInterface < nFirstInterface && _nCurGroup >= _pParentPool->_pGroups->size() )
        _nCurInterface = nFirstInterface;

    if ( _nCurInterface < nFirstInterface )
    {
        DBG_ASSERT( _pParentPool, "No parent pool!" );
        const SfxSlot *pSlot = _pParentPool->NextSlot();
        _nCurInterface = _pParentPool->_nCurInterface;
        if ( pSlot )
            return pSlot;
        if ( _nCurInterface == nFirstInterface )
            // parent pool is ready
            return SeekSlot( nFirstInterface );
    }

    sal_uInt16 nInterface = _nCurInterface - nFirstInterface;
    // possibly we are already at the end
    if ( nInterface >= _pInterfaces->size() )
        return 0;

    // look for further matching func-defs within the same Interface
    SfxInterface* pInterface = (*_pInterfaces)[nInterface];
    while ( ++_nCurMsg < pInterface->Count() )
    {
        SfxSlot* pMsg = (*pInterface)[_nCurMsg];
        if ( pMsg->GetGroupId() == _pGroups->at(_nCurGroup) )
            return pMsg;
    }

    return SeekSlot(++_nCurInterface );
}


//--------------------------------------------------------------------
// Query SlotName with help text
//--------------------------------------------------------------------

SfxInterface* SfxSlotPool::FirstInterface()
{
    _nCurInterface = 0;
    if ( !_pInterfaces || !_pInterfaces->size() )
        return 0;
    return _pParentPool ? _pParentPool->FirstInterface() : (*_pInterfaces)[0];
}


//--------------------------------------------------------------------

const SfxSlot* SfxSlotPool::GetUnoSlot( const String& rName )
{
    const SfxSlot *pSlot = NULL;
    for ( sal_uInt16 nInterface=0; nInterface<_pInterfaces->size(); ++nInterface )
    {
        pSlot = (*_pInterfaces)[nInterface]->GetSlot( rName );
        if ( pSlot )
            break;
    }

    if ( !pSlot && _pParentPool )
        pSlot = _pParentPool->GetUnoSlot( rName );

    return pSlot;
}

SfxSlotPool& SfxSlotPool::GetSlotPool( SfxViewFrame *pFrame )
{
    SfxModule *pMod = SfxModule::GetActiveModule( pFrame );
    if ( pMod && pMod->GetSlotPool() )
        return *pMod->GetSlotPool();
    else
        return *SFX_APP()->Get_Impl()->pSlotPool;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
