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

#include <svtools/treelist.hxx>

DBG_NAME(SvListEntry);

SvListEntry::SvListEntry()
{
    DBG_CTOR(SvListEntry,0);
    pChildren     = 0;
    pParent     = 0;
    nListPos    = 0;
    nAbsPos     = 0;
}

SvListEntry::SvListEntry( const SvListEntry& rEntry )
{
    DBG_CTOR(SvListEntry,0);
    pChildren  = 0;
    pParent  = 0;
    nListPos &= 0x80000000;
    nListPos |= ( rEntry.nListPos & 0x7fffffff);
    nAbsPos  = rEntry.nAbsPos;
}

SvListEntry::~SvListEntry()
{
    DBG_DTOR(SvListEntry,0);
    if ( pChildren )
    {
        pChildren->DestroyAll();
        delete pChildren;
    }
#ifdef DBG_UTIL
    pChildren     = 0;
    pParent     = 0;
#endif
}

void SvListEntry::Clone( SvListEntry* pSource)
{
    DBG_CHKTHIS(SvListEntry,0);
    nListPos &= 0x80000000;
    nListPos |= ( pSource->nListPos & 0x7fffffff);
    nAbsPos     = pSource->nAbsPos;
}

void SvListEntry::SetListPositions()
{
    if( pChildren )
    {
        SvListEntry *pEntry = (SvListEntry*)pChildren->First();
        sal_uLong       nCur = 0;
        while ( pEntry )
        {
            pEntry->nListPos &= 0x80000000;
            pEntry->nListPos |= nCur;
            nCur++;
            pEntry = (SvListEntry*)pChildren->Next();
        }
    }
    nListPos &= (~0x80000000);
}


DBG_NAME(SvViewData);

SvViewData::SvViewData()
{
    DBG_CTOR(SvViewData,0);
    nFlags = 0;
    nVisPos = 0;
}

SvViewData::SvViewData( const SvViewData& rData )
{
    DBG_CTOR(SvViewData,0);
    nFlags  = rData.nFlags;
    nFlags &= ~( SVLISTENTRYFLAG_SELECTED | SVLISTENTRYFLAG_FOCUSED );
    nVisPos = rData.nVisPos;
}

SvViewData::~SvViewData()
{
    DBG_DTOR(SvViewData,0);
#ifdef DBG_UTIL
    nVisPos = 0x12345678;
    nFlags = 0x1234;
#endif
}

//=============================================================================
// SvTreeEntryList
//=============================================================================

void SvTreeEntryList::DestroyAll()
{
    SvListEntry* pPtr = (SvListEntry*)First();
    while( pPtr )
    {
        delete pPtr;
        pPtr = (SvListEntry*)Next();
    }
}

SvTreeEntryList::SvTreeEntryList(SvTreeEntryList& rList)
{
    maEntryList.clear();
    maCurrent = 0;
    for ( size_t i = 0, n = rList.size(); i < n; ++i ) {
        maEntryList.push_back( rList[ i ] );
    }
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvTreeList::SvTreeList()
{
    nEntryCount = 0;
    bAbsPositionsValid = sal_False;
    nRefCount = 1;
    pRootItem = new SvListEntry;
    eSortMode = SortNone;
}


/*************************************************************************
|*
|*    SvTreeList::~SvTreeList
|*
*************************************************************************/

SvTreeList::~SvTreeList()
{
    Clear();
    delete pRootItem;
#ifdef DBG_UTIL
    pRootItem = 0;
#endif
}

/*************************************************************************
|*
|*    SvTreeList::Broadcast
|*
*************************************************************************/

void SvTreeList::Broadcast(
    sal_uInt16 nActionId,
    SvListEntry* pEntry1,
    SvListEntry* pEntry2,
    sal_uLong nPos
) {
    sal_uLong nViewCount = aViewList.size();
    for( sal_uLong nCurView = 0; nCurView < nViewCount; nCurView++ )
    {
        SvListView* pView = aViewList[ nCurView ];
        if( pView )
            pView->ModelNotification( nActionId, pEntry1, pEntry2, nPos );
    }
}

void SvTreeList::InsertView( SvListView* pView )
{
    for ( sal_uLong i = 0, n = aViewList.size(); i < n; ++i ) {
        if ( aViewList[ i ] == pView ) {
            return;
        }
    }
    aViewList.push_back( pView );
    nRefCount++;
}

void SvTreeList::RemoveView( SvListView* pView )
{
    for ( SvListView_impl::iterator it = aViewList.begin(); it != aViewList.end(); ++it ) {
        if ( *it == pView ) {
            aViewList.erase( it );
            nRefCount--;
            break;
        }
    }
}


// an entry is visible if all parents are expanded
sal_Bool SvTreeList::IsEntryVisible( const SvListView* pView, SvListEntry* pEntry ) const
{
    DBG_ASSERT(pView&&pEntry,"IsVisible:Invalid Params");
    sal_Bool bRetVal=sal_False;
    do
    {
        if ( pEntry == pRootItem )
        {
            bRetVal=sal_True;
            break;
        }
        pEntry = pEntry->pParent;
    }  while( pView->IsExpanded( pEntry ) );
    return bRetVal;
}

sal_uInt16 SvTreeList::GetDepth( SvListEntry* pEntry ) const
{
    DBG_ASSERT(pEntry&&pEntry!=pRootItem,"GetDepth:Bad Entry");
    sal_uInt16 nDepth = 0;
    while( pEntry->pParent != pRootItem )
    {
        nDepth++;
        pEntry = pEntry->pParent;
    }
    return nDepth;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

void SvTreeList::Clear()
{
    Broadcast( LISTACTION_CLEARING );
    SvTreeEntryList* pRootList = pRootItem->pChildren;
    if ( pRootList )
    {
        SvListEntry* pEntry = (SvListEntry*)(pRootList->First());
        while( pEntry )
        {
            delete pEntry;
            pEntry = (SvListEntry*)(pRootList->Next());
        }
        delete pRootItem->pChildren;
        pRootItem->pChildren = 0;
    }
    nEntryCount = 0;
    Broadcast( LISTACTION_CLEARED );
}


/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

sal_Bool SvTreeList::IsChild( SvListEntry* pParent, SvListEntry* pChild ) const
{
    if ( !pParent )
        pParent = pRootItem;

    sal_Bool bIsChild = sal_False;
    SvTreeEntryList* pList = pParent->pChildren;
    if ( !pList )
        return sal_False;
    SvListEntry* pActualChild = (SvListEntry*)(pList->First());
    while( !bIsChild && pActualChild )
    {
        if ( pActualChild == pChild )
            bIsChild = sal_True;
        else
        {
            if ( pActualChild->pChildren )
                bIsChild = IsChild( pActualChild, pChild );
            pActualChild = (SvListEntry*)(pList->Next());
        }
    }
    return bIsChild;
}

sal_uLong SvTreeList::Move(SvListEntry* pSrcEntry,SvListEntry* pTargetParent,sal_uLong nListPos)
{
    // pDest may be 0!
    DBG_ASSERT(pSrcEntry,"Entry?");
    if ( !pTargetParent )
        pTargetParent = pRootItem;
    DBG_ASSERT(pSrcEntry!=pTargetParent,"Move:Source=Target");

    Broadcast( LISTACTION_MOVING, pSrcEntry, pTargetParent, nListPos );

    if ( !pTargetParent->pChildren )
        pTargetParent->pChildren = new SvTreeEntryList;
    if ( pSrcEntry == pTargetParent )
        return pSrcEntry->GetChildListPos();

    bAbsPositionsValid = sal_False;

    SvTreeEntryList* pDstList = pTargetParent->pChildren;
    SvTreeEntryList* pSrcList = pSrcEntry->pParent->pChildren;

    // insert dummy pointer, as nListPos might become invalid because of the
    // following Remove.
    SvListEntry* pDummy = 0;
    pDstList->insert( pDummy, nListPos );

    // delete
    pSrcList->remove( pSrcEntry );
    // does parent still have children?
    if ( pSrcList->empty() )
    {
        // no children, thus delete child list
        SvListEntry* pParent = pSrcEntry->pParent;
        pParent->pChildren = 0;
        delete pSrcList;
        pSrcList = 0;
    }

    // move parent umsetzen (do this only now, because we need the parent for
    // deleting the old child list!)
    pSrcEntry->pParent = pTargetParent;

    pDstList->replace( pSrcEntry, pDummy );

    // correct list position in target list
    SetListPositions( pDstList );
    if ( pSrcList && (sal_uLong)pSrcList != (sal_uLong)pDstList )
        SetListPositions( pSrcList );

#ifdef CHECK_INTEGRITY
    CheckIntegrity();
#endif

    sal_uLong nRetVal = pDstList->GetPos( pSrcEntry );
    DBG_ASSERT(nRetVal==pSrcEntry->GetChildListPos(),"ListPos not valid");
    Broadcast( LISTACTION_MOVED,pSrcEntry,pTargetParent,nRetVal);
    return nRetVal;
}

sal_uLong SvTreeList::Copy(SvListEntry* pSrcEntry,SvListEntry* pTargetParent,sal_uLong nListPos)
{
    // pDest may be 0!
    DBG_ASSERT(pSrcEntry,"Entry?");
    if ( !pTargetParent )
        pTargetParent = pRootItem;
    if ( !pTargetParent->pChildren )
        pTargetParent->pChildren = new SvTreeEntryList;

    bAbsPositionsValid = sal_False;

    sal_uLong nCloneCount = 0;
    SvListEntry* pClonedEntry = Clone( pSrcEntry, nCloneCount );
    nEntryCount += nCloneCount;

    SvTreeEntryList* pDstList = pTargetParent->pChildren;
    pClonedEntry->pParent = pTargetParent;      // move parent
    pDstList->insert( pClonedEntry, nListPos ); // insert
    SetListPositions( pDstList ); // correct list position in target list

#ifdef CHECK_INTEGRITY
    CheckIntegrity();
#endif
    Broadcast( LISTACTION_INSERTED_TREE, pClonedEntry );
    sal_uLong nRetVal = pDstList->GetPos( pClonedEntry );
    return nRetVal;
}



/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

void SvTreeList::Move( SvListEntry* pSrcEntry, SvListEntry* pDstEntry )
{
    SvListEntry* pParent;
    sal_uLong nPos;

    if ( !pDstEntry )
    {
        pParent = pRootItem;
        nPos = 0UL;
    }
    else
    {
        pParent = pDstEntry->pParent;
        nPos = pDstEntry->GetChildListPos();
        nPos++;  // (On screen:) insert _below_ pDstEntry
    }
    Move( pSrcEntry, pParent, nPos );
}

void SvTreeList::InsertTree(SvListEntry* pSrcEntry,
    SvListEntry* pTargetParent,sal_uLong nListPos)
{
    DBG_ASSERT(pSrcEntry,"InsertTree:Entry?");
    if ( !pSrcEntry )
        return;

    if ( !pTargetParent )
        pTargetParent = pRootItem;
    if ( !pTargetParent->pChildren )
        pTargetParent->pChildren = new SvTreeEntryList;

    // take sorting into account
    GetInsertionPos( pSrcEntry, pTargetParent, nListPos );

    bAbsPositionsValid = sal_False;

    pSrcEntry->pParent = pTargetParent; // move parent
    SvTreeEntryList* pDstList = pTargetParent->pChildren;
    pDstList->insert( pSrcEntry, nListPos ); // insert
    SetListPositions(pDstList); // correct list position in target list
    nEntryCount += GetChildCount( pSrcEntry );
    nEntryCount++; // the parent is new, too

#ifdef CHECK_INTEGRITY
CheckIntegrity();
#endif
    Broadcast(LISTACTION_INSERTED_TREE, pSrcEntry );
}

SvListEntry* SvTreeList::CloneEntry( SvListEntry* pSource ) const
{
    if( aCloneLink.IsSet() )
        return (SvListEntry*)aCloneLink.Call( pSource );
    SvListEntry* pEntry = CreateEntry();
    pSource->Clone( pEntry );
    return pSource;
}

SvListEntry* SvTreeList::CreateEntry() const
{
    return new SvListEntry;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvListEntry* SvTreeList::Clone( SvListEntry* pEntry, sal_uLong& nCloneCount ) const
{
    SvListEntry* pClonedEntry = CloneEntry( pEntry );
    nCloneCount = 1;
    SvTreeEntryList* pChildren = pEntry->pChildren;
    if ( pChildren )
        pClonedEntry->pChildren=CloneChildren(pChildren,pClonedEntry,nCloneCount);
    return pClonedEntry;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvTreeEntryList* SvTreeList::CloneChildren( SvTreeEntryList* pChildren,
                                      SvListEntry* pNewParent,
                                      sal_uLong& nCloneCount ) const
{
    DBG_ASSERT(!pChildren->empty(),"Children?");
    SvTreeEntryList* pClonedChildren = new SvTreeEntryList;
    SvListEntry* pChild = (SvListEntry*)pChildren->First();
    while ( pChild )
    {
        SvListEntry* pNewChild = CloneEntry( pChild );
        nCloneCount++;
        pNewChild->pParent = pNewParent;
        SvTreeEntryList* pSubChildren = pChild->pChildren;
        if ( pSubChildren )
        {
            pSubChildren = CloneChildren( pSubChildren, pNewChild, nCloneCount );
            pNewChild->pChildren = pSubChildren;
        }

        pClonedChildren->push_back( pNewChild );
        pChild = (SvListEntry*)pChildren->Next();
    }
    return pClonedChildren;
}


/*************************************************************************
|*
|*    SvTreeList::GetChildCount
|*
*************************************************************************/

sal_uLong SvTreeList::GetChildCount( SvListEntry* pParent ) const
{
    if ( !pParent )
        return GetEntryCount();

    if ( !pParent || !pParent->pChildren)
        return 0;
    sal_uLong nCount = 0;
    sal_uInt16 nRefDepth = GetDepth( pParent );
    sal_uInt16 nActDepth = nRefDepth;
    do
    {
        pParent = Next( pParent, &nActDepth );
        nCount++;
    } while( pParent && nRefDepth < nActDepth );
    nCount--;
    return nCount;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

sal_uLong SvTreeList::GetVisibleChildCount(const SvListView* pView, SvListEntry* pParent) const
{
    DBG_ASSERT(pView,"GetVisChildCount:No View");
    if ( !pParent )
        pParent = pRootItem;
    if ( !pParent || !pView->IsExpanded(pParent) || !pParent->pChildren )
        return 0;
    sal_uLong nCount = 0;
    sal_uInt16 nRefDepth = GetDepth( pParent );
    sal_uInt16 nActDepth = nRefDepth;
    do
    {
        pParent = NextVisible( pView, pParent, &nActDepth );
        nCount++;
    } while( pParent && nRefDepth < nActDepth );
    nCount--;
    return nCount;
}

sal_uLong SvTreeList::GetChildSelectionCount(const SvListView* pView,SvListEntry* pParent) const
{
    DBG_ASSERT(pView,"GetChildSelCount:No View");
    if ( !pParent )
        pParent = pRootItem;
    if ( !pParent || !pParent->pChildren)
        return 0;
    sal_uLong nCount = 0;
    sal_uInt16 nRefDepth = GetDepth( pParent );
    sal_uInt16 nActDepth = nRefDepth;
    do
    {
        pParent = Next( pParent, &nActDepth );
        if( pParent && pView->IsSelected( pParent ) && nRefDepth < nActDepth)
            nCount++;
    } while( pParent && nRefDepth < nActDepth );
//  nCount--;
    return nCount;
}


/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvListEntry* SvTreeList::First() const
{
    if ( nEntryCount )
        return (SvListEntry*)(*pRootItem->pChildren)[ 0 ];
    else
        return 0;
}

/*************************************************************************
|*
|*    SvTreeList::Next
|*
*************************************************************************/
SvListEntry* SvTreeList::Next( SvListEntry* pActEntry, sal_uInt16* pDepth ) const
{
    DBG_ASSERT( pActEntry && pActEntry->pParent, "SvTreeList::Next: invalid entry/parent!" );
    if ( !pActEntry || !pActEntry->pParent )
        return NULL;

    sal_uInt16 nDepth = 0;
    int bWithDepth = sal_False;
    if ( pDepth )
    {
        nDepth = *pDepth;
        bWithDepth = sal_True;
    }

    SvTreeEntryList* pActualList = pActEntry->pParent->pChildren;
    sal_uLong nActualPos = pActEntry->GetChildListPos();

    if ( pActEntry->pChildren /* && pActEntry->pChildren->Count() */ )
    {
        nDepth++;
        pActEntry = (SvListEntry*)(*pActEntry->pChildren)[ 0 ];
        if ( bWithDepth )
            *pDepth = nDepth;
        return pActEntry;
    }

    if ( pActualList->size() > ( nActualPos + 1 ) )
    {
        pActEntry = (SvListEntry*)(*pActualList)[ nActualPos + 1 ];
        if ( bWithDepth )
            *pDepth = nDepth;
        return pActEntry;
    }

    SvListEntry* pParent = pActEntry->pParent;
    nDepth--;
    while( pParent != pRootItem && pParent != 0 )
    {
        DBG_ASSERT(pParent!=0,"TreeData corrupt!");
        pActualList = pParent->pParent->pChildren;
        DBG_ASSERT(pActualList,"TreeData corrupt!");
        nActualPos = pParent->GetChildListPos();
        if ( pActualList->size() > ( nActualPos + 1 ) )
        {
            pActEntry = (SvListEntry*)(*pActualList)[ nActualPos + 1 ];
            if ( bWithDepth )
                *pDepth = nDepth;
            return pActEntry;
        }
        pParent = pParent->pParent;
        nDepth--;
    }
    return 0;
}

/*************************************************************************
|*
|*    SvTreeList::Prev
|*
*************************************************************************/
SvListEntry* SvTreeList::Prev( SvListEntry* pActEntry, sal_uInt16* pDepth ) const
{
    DBG_ASSERT(pActEntry!=0,"Entry?");

    sal_uInt16 nDepth = 0;
    int bWithDepth = sal_False;
    if ( pDepth )
    {
        nDepth = *pDepth;
        bWithDepth = sal_True;
    }

    SvTreeEntryList* pActualList = pActEntry->pParent->pChildren;
    sal_uLong nActualPos = pActEntry->GetChildListPos();

    if ( nActualPos > 0 )
    {
        pActEntry = (SvListEntry*)(*pActualList)[ nActualPos - 1 ];
        while( pActEntry->pChildren )
        {
            pActualList = pActEntry->pChildren;
            nDepth++;
            pActEntry = (SvListEntry*)(pActualList->last());
        }
        if ( bWithDepth )
            *pDepth = nDepth;
        return pActEntry;
    }
    if ( pActEntry->pParent == pRootItem )
        return 0;

    pActEntry = pActEntry->pParent;

    if ( pActEntry )
    {
        nDepth--;
        if ( bWithDepth )
            *pDepth = nDepth;
        return pActEntry;
    }
    return 0;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvListEntry* SvTreeList::Last() const
{
    SvTreeEntryList* pActList = pRootItem->pChildren;
//  if ( pActList->Count() == 0 )
//      return 0;
    SvListEntry* pEntry = 0;
    while( pActList )
    {
        pEntry = (SvListEntry*)(pActList->last());
        pActList = pEntry->pChildren;
//      if ( pActList->Count() == 0 )
//          pActList = 0;
    }
    return pEntry;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

sal_uLong SvTreeList::GetVisiblePos( const SvListView* pView, SvListEntry* pEntry ) const
{
    DBG_ASSERT(pView&&pEntry,"View/Entry?");

    if ( !pView->bVisPositionsValid )
    {
        // to make GetVisibleCount refresh the positions
        ((SvListView*)pView)->nVisibleCount = 0;
        GetVisibleCount( const_cast<SvListView*>(pView) );
    }
    const SvViewData* pViewData = pView->GetViewData( pEntry );
    return pViewData->nVisPos;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

sal_uLong SvTreeList::GetVisibleCount( SvListView* pView ) const
{
    DBG_ASSERT(pView,"GetVisCount:No View");
    if( !pView->HasViewData() )
        return 0;
    if ( pView->nVisibleCount )
        return pView->nVisibleCount;

    sal_uLong nPos = 0;
    SvListEntry* pEntry = First();  // first entry is always visible
    while ( pEntry )
    {
        SvViewData* pViewData = pView->GetViewData( pEntry );
        pViewData->nVisPos = nPos;
        nPos++;
        pEntry = NextVisible( pView, pEntry );
    }
#ifdef DBG_UTIL
    if( nPos > 10000000 )
    {
        OSL_FAIL("nVisibleCount bad");
    }
#endif
    ((SvListView*)pView)->nVisibleCount = nPos;
    ((SvListView*)pView)->bVisPositionsValid = sal_True;
    return nPos;
}


/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

// For performance reasons, this function assumes that the passed entry is
// already visible.

SvListEntry* SvTreeList::NextVisible(const SvListView* pView,SvListEntry* pActEntry,sal_uInt16* pActDepth) const
{
    DBG_ASSERT(pView,"NextVisible:No View");
    if ( !pActEntry )
        return 0;

    sal_uInt16 nDepth = 0;
    int bWithDepth = sal_False;
    if ( pActDepth )
    {
        nDepth = *pActDepth;
        bWithDepth = sal_True;
    }

    SvTreeEntryList* pActualList = pActEntry->pParent->pChildren;
    sal_uLong nActualPos = pActEntry->GetChildListPos();

    if ( pView->IsExpanded(pActEntry) )
    {
        DBG_ASSERT(pActEntry->pChildren,"Children?");
        nDepth++;
        pActEntry = (SvListEntry*)(*pActEntry->pChildren)[ 0 ];
        if ( bWithDepth )
            *pActDepth = nDepth;
        return pActEntry;
    }

    nActualPos++;
    if ( pActualList->size() > nActualPos  )
    {
        pActEntry = (SvListEntry*)(*pActualList)[ nActualPos ];
        if ( bWithDepth )
            *pActDepth = nDepth;
        return pActEntry;
    }

    SvListEntry* pParent = pActEntry->pParent;
    nDepth--;
    while( pParent != pRootItem )
    {
        pActualList = pParent->pParent->pChildren;
        nActualPos = pParent->GetChildListPos();
        nActualPos++;
        if ( pActualList->size() > nActualPos )
        {
            pActEntry = (SvListEntry*)(*pActualList)[ nActualPos ];
            if ( bWithDepth )
                *pActDepth = nDepth;
            return pActEntry;
        }
        pParent = pParent->pParent;
        nDepth--;
    }
    return 0;
}


/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

// For performance reasons, this function assumes that the passed entry is
// already visible.

SvListEntry* SvTreeList::PrevVisible(const SvListView* pView, SvListEntry* pActEntry, sal_uInt16* pActDepth) const
{
    DBG_ASSERT(pView&&pActEntry,"PrevVis:View/Entry?");

    sal_uInt16 nDepth = 0;
    int bWithDepth = sal_False;
    if ( pActDepth )
    {
        nDepth = *pActDepth;
        bWithDepth = sal_True;
    }

    SvTreeEntryList* pActualList = pActEntry->pParent->pChildren;
    sal_uLong nActualPos = pActEntry->GetChildListPos();

    if ( nActualPos > 0 )
    {
        pActEntry = (SvListEntry*)(*pActualList)[ nActualPos - 1 ];
        while( pView->IsExpanded(pActEntry) )
        {
            pActualList = pActEntry->pChildren;
            nDepth++;
            pActEntry = (SvListEntry*)(pActualList->last());
        }
        if ( bWithDepth )
            *pActDepth = nDepth;
        return pActEntry;
    }

    if ( pActEntry->pParent == pRootItem )
        return 0;

    pActEntry = pActEntry->pParent;
    if ( pActEntry )
    {
        nDepth--;
        if ( bWithDepth )
            *pActDepth = nDepth;
        return pActEntry;
    }
    return 0;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvListEntry* SvTreeList::LastVisible( const SvListView* pView, sal_uInt16* pDepth) const
{
    DBG_ASSERT(pView,"LastVis:No View");
    SvListEntry* pEntry = Last();
    while( pEntry && !IsEntryVisible( pView, pEntry ) )
        pEntry = PrevVisible( pView, pEntry );
    if ( pEntry && pDepth )
        *pDepth = GetDepth( pEntry );
    return pEntry;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvListEntry* SvTreeList::NextVisible(const SvListView* pView,SvListEntry* pEntry,sal_uInt16& nDelta) const
{
    DBG_ASSERT(pView&&pEntry&&IsEntryVisible(pView,pEntry),"NextVis:Wrong Prms/!Vis");

    sal_uLong nVisPos = GetVisiblePos( pView, pEntry );
    // nDelta entries existent?
    // example: 0,1,2,3,4,5,6,7,8,9 nVisPos=5 nDelta=7
    //           nNewDelta = 10-nVisPos-1 == 4
    if (  nVisPos+nDelta >= pView->nVisibleCount )
    {
        nDelta = (sal_uInt16)(pView->nVisibleCount-nVisPos);
        nDelta--;
    }
    sal_uInt16 nDeltaTmp = nDelta;
    while( nDeltaTmp )
    {
        pEntry = NextVisible( pView, pEntry );
        nDeltaTmp--;
        DBG_ASSERT(pEntry,"Entry?");
    }
    return pEntry;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvListEntry* SvTreeList::PrevVisible( const SvListView* pView, SvListEntry* pEntry, sal_uInt16& nDelta ) const
{
    DBG_ASSERT(pView&&pEntry&&IsEntryVisible(pView,pEntry),"PrevVis:Parms/!Vis");

    sal_uLong nVisPos = GetVisiblePos( pView, pEntry );
    // nDelta entries existent?
    // example: 0,1,2,3,4,5,6,7,8,9 nVisPos=8 nDelta=20
    //           nNewDelta = nNewVisPos
    if (  nDelta > nVisPos )
        nDelta = (sal_uInt16)nVisPos;
    sal_uInt16 nDeltaTmp = nDelta;
    while( nDeltaTmp )
    {
        pEntry = PrevVisible( pView, pEntry );
        nDeltaTmp--;
        DBG_ASSERT(pEntry,"Entry?");
    }
    return pEntry;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvListEntry* SvTreeList::FirstSelected( const SvListView* pView) const
{
    DBG_ASSERT(pView,"FirstSel:No View");
    if( !pView )
        return 0;
    SvListEntry* pActSelEntry = First();
    while( pActSelEntry && !pView->IsSelected(pActSelEntry) )
        pActSelEntry = NextVisible( pView, pActSelEntry );
    return pActSelEntry;
}


SvListEntry* SvTreeList::FirstChild( SvListEntry* pParent ) const
{
    if ( !pParent )
        pParent = pRootItem;
    SvListEntry* pResult;
    if ( pParent->pChildren )
        pResult = (SvListEntry*)(*pParent->pChildren)[ 0 ];
    else
        pResult = 0;
    return pResult;
}

SvListEntry* SvTreeList::NextSibling( SvListEntry* pEntry ) const
{
    DBG_ASSERT(pEntry,"Entry?");
    if( !pEntry )
        return 0;
    SvTreeEntryList* pList = pEntry->pParent->pChildren;
    sal_uLong nPos = pEntry->GetChildListPos();
    nPos++;
    pEntry = (SvListEntry*)(*pList)[ nPos ];
    return pEntry;
}

SvListEntry* SvTreeList::PrevSibling( SvListEntry* pEntry ) const
{
    DBG_ASSERT(pEntry,"Entry?");
    if( !pEntry )
        return 0;

    SvTreeEntryList* pList = pEntry->pParent->pChildren;
    sal_uLong nPos = pEntry->GetChildListPos();
    if ( nPos == 0 )
        return 0;
    nPos--;
    pEntry = (SvListEntry*)(*pList)[ nPos ];
    return pEntry;
}


SvListEntry* SvTreeList::LastSibling( SvListEntry* pEntry ) const
{
    DBG_ASSERT(pEntry,"LastSibling:Entry?");
    if( !pEntry )
        return 0;
    SvListEntry* pSib = 0;
    SvTreeEntryList* pSibs = pEntry->pParent->pChildren;
    if ( pSibs )
        pSib = (SvListEntry*)(pSibs->last());
    return pSib;
}


/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvListEntry* SvTreeList::NextSelected( const SvListView* pView, SvListEntry* pEntry ) const
{
    DBG_ASSERT(pView&&pEntry,"NextSel:View/Entry?");
    pEntry = Next( pEntry );
    while( pEntry && !pView->IsSelected(pEntry) )
        pEntry = Next( pEntry );
    return pEntry;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvListEntry* SvTreeList::PrevSelected( const SvListView* pView, SvListEntry* pEntry) const
{
    DBG_ASSERT(pView&&pEntry,"PrevSel:View/Entry?");
    pEntry = Prev( pEntry );
    while( pEntry && !pView->IsSelected(pEntry) )
        pEntry = Prev( pEntry );

    return pEntry;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

SvListEntry* SvTreeList::LastSelected( const SvListView* pView ) const
{
    DBG_ASSERT(pView,"LastSel:No View");
    SvListEntry* pEntry = Last();
    while( pEntry && !pView->IsSelected(pEntry) )
        pEntry = Prev( pEntry );
    return pEntry;
}

/*************************************************************************
|*
|*    SvTreeList::Insert
|*
*************************************************************************/
sal_uLong SvTreeList::Insert( SvListEntry* pEntry,SvListEntry* pParent,sal_uLong nPos )
{
    DBG_ASSERT( pEntry,"Entry?");

    if ( !pParent )
        pParent = pRootItem;


    SvTreeEntryList* pList = pParent->pChildren;
    if ( !pList )
    {
        // parent gets the first child
        pList = new SvTreeEntryList;
        pParent->pChildren = pList;
    }

    // take sorting into account
    GetInsertionPos( pEntry, pParent, nPos );

    bAbsPositionsValid = sal_False;
    pEntry->pParent = pParent;

    pList->insert( pEntry, nPos );
    nEntryCount++;
    if( nPos != ULONG_MAX && (nPos != (pList->size()-1)) )
        SetListPositions( pList );
    else
        pEntry->nListPos = pList->size()-1;

#ifdef CHECK_INTEGRITY
    CheckIntegrity();
#endif
    Broadcast( LISTACTION_INSERTED, pEntry );
    return nPos; // pEntry->nListPos;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

sal_uLong SvTreeList::GetAbsPos( SvListEntry* pEntry) const
{
    if ( !bAbsPositionsValid )
        ((SvTreeList*)this)->SetAbsolutePositions();
    return pEntry->nAbsPos;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

void SvTreeList::SetAbsolutePositions()
{
    sal_uLong nPos = 0;
    SvListEntry* pEntry = First();
    while ( pEntry )
    {
        pEntry->nAbsPos = nPos;
        nPos++;
        pEntry = Next( pEntry );
    }
    bAbsPositionsValid = sal_True;
#ifdef CHECK_INTEGRITY
CheckIntegrity();
#endif
}


/*************************************************************************
|*
|*    SvTreeList::Expand
|*
*************************************************************************/

void SvTreeList::Expand( SvListView* pView, SvListEntry* pEntry )
{
    DBG_ASSERT(pEntry&&pView,"Expand:View/Entry?");
    if ( pView->IsExpanded(pEntry) )
        return;

    DBG_ASSERT(pEntry->pChildren,"Expand:No children!");

    SvViewData* pViewData = pView->GetViewData(pEntry);
    pViewData->nFlags |= SVLISTENTRYFLAG_EXPANDED;
    SvListEntry* pParent = pEntry->pParent;
    // if parent is visible, invalidate status data
    if ( pView->IsExpanded( pParent ) )
    {
        pView->bVisPositionsValid = sal_False;
        pView->nVisibleCount = 0;
    }
#ifdef CHECK_INTEGRITY
CheckIntegrity();
#endif
}

/*************************************************************************
|*
|*    SvTreeList::Collapse
|*
*************************************************************************/

void SvTreeList::Collapse( SvListView* pView, SvListEntry* pEntry )
{
    DBG_ASSERT(pView&&pEntry,"Collapse:View/Entry?");
    if ( !pView->IsExpanded(pEntry) )
        return;

    DBG_ASSERT(pEntry->pChildren,"Collapse:No children!");

    SvViewData* pViewData = pView->GetViewData( pEntry );
    pViewData->nFlags &=(~SVLISTENTRYFLAG_EXPANDED);

    SvListEntry* pParent = pEntry->pParent;
    if ( pView->IsExpanded(pParent) )
    {
        pView->nVisibleCount = 0;
        pView->bVisPositionsValid = sal_False;
    }
#ifdef CHECK_INTEGRITY
CheckIntegrity();
#endif
}


/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

sal_Bool SvTreeList::Select( SvListView* pView, SvListEntry* pEntry, sal_Bool bSelect )
{
    DBG_ASSERT(pView&&pEntry,"Select:View/Entry?");
    SvViewData* pViewData = pView->GetViewData( pEntry );
    if ( bSelect )
    {
        if ( pViewData->IsSelected() || !pViewData->IsSelectable() )
            return sal_False;
        else
        {
            pViewData->nFlags |= SVLISTENTRYFLAG_SELECTED;
            pView->nSelectionCount++;
        }
    }
    else
    {
        if ( !pViewData->IsSelected() )
            return sal_False;
        else
        {
            pViewData->nFlags &= ~( SVLISTENTRYFLAG_SELECTED );
            pView->nSelectionCount--;
        }
    }
#ifdef CHECK_INTEGRITY
    CheckIntegrity();
#endif
    return sal_True;
}

/*************************************************************************
|*
|*    SvTreeList::Remove
|*
*************************************************************************/
sal_Bool SvTreeList::Remove( SvListEntry* pEntry )
{
    DBG_ASSERT(pEntry,"Cannot remove root, use clear");

    if( !pEntry->pParent )
    {
        OSL_FAIL("Removing entry not in model!");
        // Under certain circumstances (which?), the explorer deletes entries
        // from the view that it hasn't inserted into the view. We don't want
        // to crash, so we catch this case here.
        return sal_False;
    }

    Broadcast( LISTACTION_REMOVING, pEntry );
    sal_uLong nRemoved = 1 + GetChildCount(pEntry);
    bAbsPositionsValid = sal_False;

    SvListEntry* pParent = pEntry->pParent;
    SvTreeEntryList* pList = pParent->pChildren;
    DBG_ASSERT(pList,"Remove:No Childlist");
    sal_Bool bLastEntry = sal_False;

    if ( pEntry->HasChildListPos() )
    {
        size_t nListPos = pEntry->GetChildListPos();
        bLastEntry = (nListPos == (pList->size()-1) ) ? sal_True : sal_False;
        pList->remove( nListPos );
    }
    else
    {
        pList->remove( pEntry );
    }


    // moved to end of method because it is used later with Broadcast
    // delete pEntry; // loescht auch alle Children

    if ( pList->empty() )
    {
        pParent->pChildren = 0;
        delete pList;
    }
    else
    {
        if( !bLastEntry )
            SetListPositions( pList );
    }
    nEntryCount -= nRemoved;

#ifdef CHECK_INTEGRITY
    CheckIntegrity();
#endif
    Broadcast( LISTACTION_REMOVED, pEntry );

    delete pEntry; // deletes any children as well
    return sal_True;
}

/*************************************************************************
|*
|*    SvTreeList::
|*
*************************************************************************/

void SvTreeList::SelectAll( SvListView* pView, sal_Bool bSelect )
{
    DBG_ASSERT(pView,"SelectAll:NoView");
    SvListEntry* pEntry = First();
    while ( pEntry )
    {
        SvViewData* pViewData = pView->GetViewData( pEntry );
        if ( bSelect )
            pViewData->nFlags |= SVLISTENTRYFLAG_SELECTED;
        else
            pViewData->nFlags &= (~SVLISTENTRYFLAG_SELECTED);

        pEntry = Next( pEntry );
    }
    if ( bSelect )
        pView->nSelectionCount = nEntryCount;
    else
        pView->nSelectionCount = 0;
#ifdef CHECK_INTEGRITY
CheckIntegrity();
#endif
}


SvListEntry* SvTreeList::GetEntryAtAbsPos( sal_uLong nAbsPos ) const
{
    SvListEntry* pEntry = First();
    while ( nAbsPos && pEntry )
    {
        pEntry = Next( pEntry );
        nAbsPos--;
    }
    return pEntry;
}

SvListEntry* SvTreeList::GetEntryAtVisPos( const SvListView* pView, sal_uLong nVisPos ) const
{
    DBG_ASSERT(pView,"GetEntryAtVisPos:No View");
    SvListEntry* pEntry = First();
    while ( nVisPos && pEntry )
    {
        pEntry = NextVisible( pView, pEntry );
        nVisPos--;
    }
    return pEntry;
}

void SvTreeList::SetListPositions( SvTreeEntryList* pList )
{
    if( !pList->empty() )
    {
        SvListEntry* pEntry = (SvListEntry*)(*pList)[ 0 ];
        if( pEntry->pParent )
            pEntry->pParent->InvalidateChildrensListPositions();
    }
}

void SvTreeList::InvalidateEntry( SvListEntry* pEntry )
{
    Broadcast( LISTACTION_INVALIDATE_ENTRY, pEntry );
}

void lcl_CheckList( SvTreeEntryList* pList )
{
    SvListEntry* pEntry = (SvListEntry*)(pList->First());
    sal_uLong nPos = 0;
    while ( pEntry )
    {
        DBG_ASSERT(pEntry->GetChildListPos()==nPos,"Wrong ListPos");
        pEntry = (SvListEntry*)(pList->Next());
        nPos++;
    }
}

SvListEntry* SvTreeList::GetRootLevelParent( SvListEntry* pEntry ) const
{
    DBG_ASSERT(pEntry,"GetRootLevelParent:No Entry");
    SvListEntry* pCurParent = 0;
    if ( pEntry )
    {
        pCurParent = pEntry->pParent;
        if ( pCurParent == pRootItem )
            return pEntry; // is its own parent
        while( pCurParent && pCurParent->pParent != pRootItem )
            pCurParent = pCurParent->pParent;
    }
    return pCurParent;
}




//*************************************************************************
//*************************************************************************
//*************************************************************************
//*************************************************************************
//*************************************************************************
//*************************************************************************
//*************************************************************************
//*************************************************************************

DBG_NAME(SvListView);

SvListView::SvListView()
{
    DBG_CTOR(SvListView,0);
    pModel = 0;
    nSelectionCount = 0;
    nVisibleCount = 0;
    bVisPositionsValid = sal_False;
}


SvListView::~SvListView()
{
    DBG_DTOR(SvListView,0);
    ClearTable();
}

void SvListView::InitTable()
{
    DBG_CHKTHIS(SvListView,0);
    DBG_ASSERT(pModel,"InitTable:No Model");
    DBG_ASSERT(!nSelectionCount&&!nVisibleCount&&!bVisPositionsValid,"InitTable: Not cleared!");

    if( maDataTable.size() )
    {
        DBG_ASSERT(maDataTable.size()==1,"InitTable: TableCount != 1");
        // Delete the view data allocated to the Clear in the root.
        // Attention: The model belonging to the root entry (and thus the entry
        // itself) might already be deleted.
        maDataTable.clear();
    }

    SvListEntry* pEntry;
    SvViewData* pViewData;

    // insert root entry
    pEntry = pModel->pRootItem;
    pViewData = new SvViewData;
    pViewData->nFlags = SVLISTENTRYFLAG_EXPANDED;
    maDataTable.insert( pEntry, pViewData );
    // now all the other entries
    pEntry = pModel->First();
    while( pEntry )
    {
        pViewData = CreateViewData( pEntry );
        DBG_ASSERT(pViewData,"InitTable:No ViewData");
        InitViewData( pViewData, pEntry );
        maDataTable.insert( pEntry, pViewData );
        pEntry = pModel->Next( pEntry );
    }
}

SvViewData* SvListView::CreateViewData( SvListEntry* )
{
    DBG_CHKTHIS(SvListView,0);
    return new SvViewData;
}

void SvListView::ClearTable()
{
    DBG_CHKTHIS(SvListView,0);
    maDataTable.clear();
}

void SvListView::Clear()
{
    ClearTable();
    nSelectionCount = 0;
    nVisibleCount = 0;
    bVisPositionsValid = sal_False;
    if( pModel )
    {
        // insert root entry
        SvListEntry* pEntry = pModel->pRootItem;
        SvViewData* pViewData = new SvViewData;
        pViewData->nFlags = SVLISTENTRYFLAG_EXPANDED;
        maDataTable.insert( pEntry, pViewData );
    }
}

void SvListView::SetModel( SvTreeList* pNewModel )
{
    DBG_CHKTHIS(SvListView,0);
    sal_Bool bBroadcastCleared = sal_False;
    if ( pModel )
    {
        pModel->RemoveView( this );
        bBroadcastCleared = sal_True;
        ModelNotification( LISTACTION_CLEARING,0,0,0 );
        if ( pModel->GetRefCount() == 0 )
            delete pModel;
    }
    pModel = pNewModel;
    InitTable();
    pNewModel->InsertView( this );
    if( bBroadcastCleared )
        ModelNotification( LISTACTION_CLEARED,0,0,0 );
}


void SvListView::ModelHasCleared()
{
    DBG_CHKTHIS(SvListView,0);
}

void SvListView::ModelHasInserted( SvListEntry* )
{
    DBG_CHKTHIS(SvListView,0);
}

void SvListView::ModelHasInsertedTree( SvListEntry* )
{
    DBG_CHKTHIS(SvListView,0);
}

void SvListView::ModelIsMoving( SvListEntry* /*  pSource */ ,
    SvListEntry* /* pTargetParent */ ,  sal_uLong /* nPos */    )
{
    DBG_CHKTHIS(SvListView,0);
}


void SvListView::ModelHasMoved( SvListEntry* )
{
    DBG_CHKTHIS(SvListView,0);
}

void SvListView::ModelIsRemoving( SvListEntry* )
{
    DBG_CHKTHIS(SvListView,0);
}

void SvListView::ModelHasRemoved( SvListEntry* )
{
    DBG_CHKTHIS(SvListView,0);
}

void SvListView::ModelHasEntryInvalidated( SvListEntry*)
{
    DBG_CHKTHIS(SvListView,0);
}

void SvListView::ActionMoving( SvListEntry* pEntry,SvListEntry*,sal_uLong)
{
    DBG_CHKTHIS(SvListView,0);
    SvListEntry* pParent = pEntry->pParent;
    DBG_ASSERT(pParent,"Model not consistent");
    if( pParent != pModel->pRootItem && pParent->pChildren->size() == 1 )
    {
        SvViewData* pViewData = maDataTable.find( pParent )->second;
        pViewData->nFlags &= (~SVLISTENTRYFLAG_EXPANDED);
    }
    // vorlaeufig
    nVisibleCount = 0;
    bVisPositionsValid = sal_False;
}

void SvListView::ActionMoved( SvListEntry* /* pEntry */ ,
                            SvListEntry* /* pTargetPrnt */ ,
                            sal_uLong /* nChildPos */ )
{
    DBG_CHKTHIS(SvListView,0);
    nVisibleCount = 0;
    bVisPositionsValid = sal_False;
}

void SvListView::ActionInserted( SvListEntry* pEntry )
{
    DBG_CHKTHIS(SvListView,0);
    DBG_ASSERT(pEntry,"Insert:No Entry");
    SvViewData* pData = CreateViewData( pEntry );
    InitViewData( pData, pEntry );
    #ifdef DBG_UTIL
    std::pair<SvDataTable::iterator, bool> aSuccess =
    #endif
        maDataTable.insert( pEntry, pData );
    DBG_ASSERT(aSuccess.second,"Entry already in View");
    if ( nVisibleCount && pModel->IsEntryVisible( this, pEntry ))
    {
        nVisibleCount = 0;
        bVisPositionsValid = sal_False;
    }
}

void SvListView::ActionInsertedTree( SvListEntry* pEntry )
{
    DBG_CHKTHIS(SvListView,0);
    if ( pModel->IsEntryVisible( this, pEntry ))
    {
        nVisibleCount = 0;
        bVisPositionsValid = sal_False;
    }
    // iterate over entry and its children
    SvListEntry* pCurEntry = pEntry;
    sal_uInt16 nRefDepth = pModel->GetDepth( pCurEntry );
    while( pCurEntry )
    {
        DBG_ASSERT(maDataTable.find(pCurEntry) != maDataTable.end(),"Entry already in Table");
        SvViewData* pViewData = CreateViewData( pCurEntry );
        DBG_ASSERT(pViewData,"No ViewData");
        InitViewData( pViewData, pEntry );
        maDataTable.insert( pCurEntry, pViewData );
        pCurEntry = pModel->Next( pCurEntry );
        if ( pCurEntry && pModel->GetDepth(pCurEntry) <= nRefDepth)
            pCurEntry = 0;
    }
}

void SvListView::RemoveViewData( SvListEntry* pParent )
{
    SvTreeEntryList* pChildren = pParent->pChildren;
    if( pChildren )
    {
        SvListEntry* pCur = (SvListEntry*)pChildren->First();
        while( pCur )
        {
            maDataTable.erase(pCur);
            if( pCur->HasChildren())
                RemoveViewData( pCur );
            pCur = (SvListEntry*)pChildren->Next();
        }
    }
}



void SvListView::ActionRemoving( SvListEntry* pEntry )
{
    DBG_CHKTHIS(SvListView,0);
    DBG_ASSERT(pEntry,"Remove:No Entry");

    SvViewData* pViewData = maDataTable.find( pEntry )->second;
    sal_uLong nSelRemoved = 0;
    if ( pViewData->IsSelected() )
        nSelRemoved = 1 + pModel->GetChildSelectionCount( this, pEntry );
    nSelectionCount -= nSelRemoved;
    sal_uLong nVisibleRemoved = 0;
    if ( pModel->IsEntryVisible( this, pEntry ) )
        nVisibleRemoved = 1 + pModel->GetVisibleChildCount( this, pEntry );
    if( nVisibleCount )
    {
#ifdef DBG_UTIL
        if( nVisibleCount < nVisibleRemoved )
        {
            OSL_FAIL("nVisibleRemoved bad");
        }
#endif
        nVisibleCount -= nVisibleRemoved;
    }
    bVisPositionsValid = sal_False;

    maDataTable.erase(pEntry);
    RemoveViewData( pEntry );

    SvListEntry* pCurEntry = pEntry->pParent;
    if ( pCurEntry && pCurEntry != pModel->pRootItem &&
         pCurEntry->pChildren->size() == 1 )
    {
        pViewData = maDataTable.find(pCurEntry)->second;
        pViewData->nFlags &= (~SVLISTENTRYFLAG_EXPANDED);
    }
}

void SvListView::ActionRemoved( SvListEntry* /* pEntry  */ )
{
    DBG_CHKTHIS(SvListView,0);
}

void SvListView::ActionClear()
{
    DBG_CHKTHIS(SvListView,0);
    Clear();
}

void SvListView::ModelNotification( sal_uInt16 nActionId, SvListEntry* pEntry1,
                        SvListEntry* pEntry2, sal_uLong nPos )
{
    DBG_CHKTHIS(SvListView,0);
    switch( nActionId )
    {
        case LISTACTION_INSERTED:
            ActionInserted( pEntry1 );
            ModelHasInserted( pEntry1 );
            break;
        case LISTACTION_INSERTED_TREE:
            ActionInsertedTree( pEntry1 );
            ModelHasInsertedTree( pEntry1 );
            break;
        case LISTACTION_REMOVING:
            ModelIsRemoving( pEntry1 );
            ActionRemoving( pEntry1 );
            break;
        case LISTACTION_REMOVED:
            ActionRemoved( pEntry1 );
            ModelHasRemoved( pEntry1 );
            break;
        case LISTACTION_MOVING:
            ModelIsMoving( pEntry1, pEntry2, nPos );
            ActionMoving( pEntry1, pEntry2, nPos );
            break;
        case LISTACTION_MOVED:
            ActionMoved( pEntry1, pEntry2, nPos );
            ModelHasMoved( pEntry1 );
            break;
        case LISTACTION_CLEARING:
            ActionClear();
            ModelHasCleared(); // sic! for compatibility reasons!
            break;
        case LISTACTION_CLEARED:
            break;
        case LISTACTION_INVALIDATE_ENTRY:
            // no action for the base class
            ModelHasEntryInvalidated( pEntry1 );
            break;
        case LISTACTION_RESORTED:
            bVisPositionsValid = sal_False;
            break;
        case LISTACTION_RESORTING:
            break;
        default:
            OSL_FAIL("unknown ActionId");
    }
}

void SvListView::InitViewData( SvViewData*, SvListEntry* )
{
}

StringCompare SvTreeList::Compare( SvListEntry* pLeft, SvListEntry* pRight) const
{
    if( aCompareLink.IsSet())
    {
        SvSortData aSortData;
        aSortData.pLeft = pLeft;
        aSortData.pRight = pRight;
        return (StringCompare)aCompareLink.Call( &aSortData );
    }
    return COMPARE_EQUAL;
}

void SvTreeList::Resort()
{
    Broadcast( LISTACTION_RESORTING );
    bAbsPositionsValid = sal_False;
    ResortChildren( pRootItem );
    Broadcast( LISTACTION_RESORTED );
}

void SvTreeList::ResortChildren( SvListEntry* pParent )
{
    DBG_ASSERT(pParent,"Parent not set");
    SvTreeEntryList* pChildList = pParent->pChildren;
    if( !pChildList )
        return;
    SvTreeEntryList aList( *pChildList );
    pChildList->clear();

    size_t nCount = aList.size();
    for( size_t nCur = 0; nCur < nCount; nCur++ )
    {
        SvListEntry* pCurEntry = (SvListEntry*)aList[ nCur ];
        sal_uLong nListPos = ULONG_MAX;
        GetInsertionPos( pCurEntry, pParent, nListPos );
        pChildList->insert( pCurEntry, nListPos );
        if( pCurEntry->pChildren )
            ResortChildren( pCurEntry );
    }
    SetListPositions( (SvTreeEntryList*)pChildList );
}

void SvTreeList::GetInsertionPos( SvListEntry* pEntry, SvListEntry* pParent,
    sal_uLong& rPos )
{
    DBG_ASSERT(pEntry,"No Entry");

    if( eSortMode == SortNone )
        return;

    rPos = ULONG_MAX;
    SvTreeEntryList* pChildList = GetChildList( pParent );

    if( pChildList && !pChildList->empty() )
    {
        long i = 0;
        long j = pChildList->size()-1;
        long k;
        StringCompare eCompare = COMPARE_GREATER;

        do
        {
            k = (i+j)/2;
            SvListEntry* pTempEntry = (SvListEntry*)(*pChildList)[ k ];
            eCompare = Compare( pEntry, pTempEntry );
            if( eSortMode == SortDescending && eCompare != COMPARE_EQUAL )
            {
                if( eCompare == COMPARE_LESS )
                    eCompare = COMPARE_GREATER;
                else
                    eCompare = COMPARE_LESS;
            }
            if( eCompare == COMPARE_GREATER )
                i = k + 1;
            else
                j = k - 1;
        } while( (eCompare != COMPARE_EQUAL) && (i <= j) );

        if( eCompare != COMPARE_EQUAL )
        {
            if(i > ((long)pChildList->size() - 1)) // not found, end of list
                rPos = ULONG_MAX;
            else
                rPos = i;              // not found, middle of list
        }
        else
            rPos = k;
    }
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
