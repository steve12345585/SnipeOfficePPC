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


#include <tools/debug.hxx>

#include <accel.h>
#include <vcl/accel.hxx>
#include <accmgr.hxx>

// =======================================================================

DBG_NAMEEX( Accelerator )

// =======================================================================

ImplAccelManager::~ImplAccelManager()
{
    delete mpAccelList;
    delete mpSequenceList;
}

// -----------------------------------------------------------------------

sal_Bool ImplAccelManager::InsertAccel( Accelerator* pAccel )
{
    if ( !mpAccelList ) {
        mpAccelList = new ImplAccelList;
    } else {
        for ( size_t i = 0, n = mpAccelList->size(); i < n; ++i ) {
            if ( (*mpAccelList)[ i ] == pAccel ) {
                return sal_False;
            }
        }
    }

    mpAccelList->insert( mpAccelList->begin(), pAccel );
    return sal_True;
}

// -----------------------------------------------------------------------

void ImplAccelManager::RemoveAccel( Accelerator* pAccel )
{
    // do we have a list ?
    if ( !mpAccelList )
        return;

    //e.g. #i90599#. Someone starts typing a sequence in a dialog, but doesn't
    //end it, and then closes the dialog, deleting the accelerators. So if
    //we're removing an accelerator that a sub-accelerator which is in the
    //sequence list, throw away the entire sequence
    if ( mpSequenceList ) {
        for (sal_uInt16 i = 0; i < pAccel->GetItemCount(); ++i) {
            Accelerator* pSubAccel = pAccel->GetAccel( pAccel->GetItemId(i) );
            for ( size_t j = 0, n = mpSequenceList->size(); j < n; ++j ) {
                if ( (*mpSequenceList)[ j ] == pSubAccel ) {
                    EndSequence( true );
                    i = pAccel->GetItemCount();
                    break;
                }
            }
        }
    }

    // throw it away
    for ( ImplAccelList::iterator it = mpAccelList->begin();
          it != mpAccelList->end();
          ++it
    ) {
        if ( *it == pAccel ) {
            mpAccelList->erase( it );
            break;
        }
    }
}

// -----------------------------------------------------------------------

void ImplAccelManager::EndSequence( sal_Bool bCancel )
{
    // are we in a list ?
    if ( !mpSequenceList )
        return;

    // call all deactivate-handler of the accelerators in the list
    for ( size_t i = 0, n = mpSequenceList->size(); i < n; ++i )
    {
        Accelerator* pTempAccel = (*mpSequenceList)[ i ];
        sal_Bool bDel = sal_False;
        pTempAccel->mbIsCancel = bCancel;
        pTempAccel->mpDel = &bDel;
        pTempAccel->Deactivate();
        if ( !bDel )
        {
            pTempAccel->mbIsCancel = sal_False;
            pTempAccel->mpDel = NULL;
        }
    }

    // delete sequence-list
    delete mpSequenceList;
    mpSequenceList = NULL;
}

// -----------------------------------------------------------------------

sal_Bool ImplAccelManager::IsAccelKey( const KeyCode& rKeyCode, sal_uInt16 nRepeat )
{
    Accelerator* pAccel;

    // do we have accelerators ??
    if ( !mpAccelList )
        return sal_False;
    if ( mpAccelList->empty() )
        return sal_False;

    // are we in a sequence ?
    if ( mpSequenceList )
    {
        pAccel = mpSequenceList->empty() ? NULL : (*mpSequenceList)[ 0 ];
        DBG_CHKOBJ( pAccel, Accelerator, NULL );

        // not found ?
        if ( !pAccel )
        {
            // abort sequence
            FlushAccel();
            return sal_False;
        }

        // can the entry be found ?
        ImplAccelEntry* pEntry = pAccel->ImplGetAccelData( rKeyCode );
        if ( pEntry )
        {
            Accelerator* pNextAccel = pEntry->mpAccel;

            // is an accelerator coupled ?
            if ( pNextAccel )
            {
                DBG_CHKOBJ( pNextAccel, Accelerator, NULL );

                mpSequenceList->insert( mpSequenceList->begin(), pNextAccel );

                // call Activate-Handler of the new one
                pNextAccel->Activate();
                return sal_True;
            }
            else
            {
                // it is there already !
                if ( pEntry->mbEnabled )
                {
                    // stop sequence (first call deactivate-handler)
                    EndSequence();

                    // set accelerator of the actuel item
                    // and call the handler
                    sal_Bool bDel = sal_False;
                    pAccel->maCurKeyCode    = rKeyCode;
                    pAccel->mnCurId         = pEntry->mnId;
                    pAccel->mnCurRepeat     = nRepeat;
                    pAccel->mpDel           = &bDel;
                    pAccel->Select();

                    // did the accelerator survive the call
                    if ( !bDel )
                    {
                        DBG_CHKOBJ( pAccel, Accelerator, NULL );
                        pAccel->maCurKeyCode    = KeyCode();
                        pAccel->mnCurId         = 0;
                        pAccel->mnCurRepeat     = 0;
                        pAccel->mpDel           = NULL;
                    }

                    return sal_True;
                }
                else
                {
                    // stop sequence as the accelerator was disbled
                    // transfer the key (to the system)
                    FlushAccel();
                    return sal_False;
                }
            }
        }
        else
        {
            // wrong key => stop sequence
            FlushAccel();
            return sal_False;
        }
    }

    // step through the list of accelerators
    for ( size_t i = 0, n = mpAccelList->size(); i < n; ++i )
    {
        pAccel = (*mpAccelList)[ i ];
        DBG_CHKOBJ( pAccel, Accelerator, NULL );

        // is the entry contained ?
        ImplAccelEntry* pEntry = pAccel->ImplGetAccelData( rKeyCode );
        if ( pEntry )
        {
            Accelerator* pNextAccel = pEntry->mpAccel;

            // is an accelerator assigned ?
            if ( pNextAccel )
            {
                DBG_CHKOBJ( pNextAccel, Accelerator, NULL );

                // create sequence list
                mpSequenceList = new ImplAccelList;
                mpSequenceList->insert( mpSequenceList->begin(), pAccel     );
                mpSequenceList->insert( mpSequenceList->begin(), pNextAccel );

                // call activate-Handler of the new one
                pNextAccel->Activate();

                return sal_True;
            }
            else
            {
                // already assigned !
                if ( pEntry->mbEnabled )
                {
                    // first call activate/aeactivate-Handler
                    pAccel->Activate();
                    pAccel->Deactivate();

                    // define accelerator of the actual item
                    // and call the handler
                    sal_Bool bDel = sal_False;
                    pAccel->maCurKeyCode    = rKeyCode;
                    pAccel->mnCurId         = pEntry->mnId;
                    pAccel->mnCurRepeat     = nRepeat;
                    pAccel->mpDel           = &bDel;
                    pAccel->Select();

                    // if the accelerator did survive the call
                    if ( !bDel )
                    {
                        DBG_CHKOBJ( pAccel, Accelerator, NULL );
                        pAccel->maCurKeyCode    = KeyCode();
                        pAccel->mnCurId         = 0;
                        pAccel->mnCurRepeat     = 0;
                        pAccel->mpDel           = NULL;
                    }

                    return sal_True;
                }
                else
                    return sal_False;
            }
        }
    }

    return sal_False;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
