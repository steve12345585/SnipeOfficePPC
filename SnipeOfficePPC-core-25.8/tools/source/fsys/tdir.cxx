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


#define _DIR_CXX

#include <stdlib.h>
#include <cstdarg>
#include <limits.h>
#include <tools/debug.hxx>

#include "comdep.hxx"
#include <tools/fsys.hxx>

DBG_NAME( Dir )

/*************************************************************************
|*
|*    Dir::InsertPointReached()
|*
|*    Beschreibung      stellt fest, ob eingefuegt werden musz
|*
*************************************************************************/

sal_Bool Dir::ImpInsertPointReached( const DirEntry& rNewEntry,
                                 const FileStat& rNewStat,
                                 size_t nCurPos, size_t nSortIndex ) const
{
#define VALUE( nKindFlags ) \
    ( ( FSYS_KIND_FILE | FSYS_KIND_DIR | FSYS_KIND_DEV | \
        FSYS_KIND_CHAR | FSYS_KIND_BLOCK ) & nKindFlags )

    // einfache Dinge erfordern einfache Loesungen
    if ( pLst->empty() )
        return sal_True;

    FSysSort  nSort      = (*pSortLst)[ nSortIndex ];
    FileStat *pOldStat   = NULL;
    DirEntry *pCurLstObj = (*pLst)[ nCurPos ];
    if ( pStatLst )
        pOldStat = (*pStatLst)[ nCurPos ];

    switch( nSort )
    {
        case  FSYS_SORT_NAME:
        case (FSYS_SORT_NAME | FSYS_SORT_ASCENDING):
            if ( pCurLstObj->aName > rNewEntry.aName )
                return sal_True;
            if ( !(pCurLstObj->aName == rNewEntry.aName) )
                return sal_False;
            break;
        case (FSYS_SORT_NAME | FSYS_SORT_DESCENDING):
            if ( pCurLstObj->aName < rNewEntry.aName )
                return sal_True;
            if ( !(pCurLstObj->aName == rNewEntry.aName) )
                return sal_False;
            break;

        case  FSYS_SORT_EXT:
        case (FSYS_SORT_EXT | FSYS_SORT_ASCENDING):
        {
            if ( pCurLstObj->GetExtension() > rNewEntry.GetExtension() )
                return sal_True;
            if ( !(pCurLstObj->GetExtension() == rNewEntry.GetExtension()) )
                return sal_False;
            break;
        }
        case (FSYS_SORT_EXT | FSYS_SORT_DESCENDING):
        {
            if ( pCurLstObj->GetExtension() < rNewEntry.GetExtension() )
                return sal_True;
            if ( !(pCurLstObj->GetExtension() == rNewEntry.GetExtension()) )
                return sal_False;
            break;
        }

        case  FSYS_SORT_KIND:
        case (FSYS_SORT_KIND | FSYS_SORT_ASCENDING ):
            if ( VALUE(pOldStat->nKindFlags) > VALUE(rNewStat.nKindFlags) )
                return sal_True;
            if ( !(VALUE(pOldStat->nKindFlags) == VALUE(rNewStat.nKindFlags)) )
                return sal_False;
            break;
        case (FSYS_SORT_KIND | FSYS_SORT_DESCENDING):
            if ( VALUE(pOldStat->nKindFlags) < VALUE(rNewStat.nKindFlags) )
                return sal_True;
            if ( !(VALUE(pOldStat->nKindFlags) == VALUE(rNewStat.nKindFlags)) )
                return sal_False;
            break;

        case  FSYS_SORT_SIZE:
        case (FSYS_SORT_SIZE | FSYS_SORT_ASCENDING):
            if ( pOldStat->nSize > rNewStat.nSize )
                return sal_True;
            if ( !(pOldStat->nSize == rNewStat.nSize) )
                return sal_False;
            break;
        case (FSYS_SORT_SIZE | FSYS_SORT_DESCENDING):
            if ( pOldStat->nSize < rNewStat.nSize )
                return sal_True;
            if ( !(pOldStat->nSize == rNewStat.nSize) )
                return sal_False;
            break;

        case  FSYS_SORT_MODIFYED:
        case (FSYS_SORT_MODIFYED | FSYS_SORT_ASCENDING):
            if ( (pOldStat->aDateModified >= rNewStat.aDateModified) &&
                 (pOldStat->aTimeModified >  rNewStat.aTimeModified) )
                 return sal_True;
            if ( !((pOldStat->aDateModified == rNewStat.aDateModified) &&
                   (pOldStat->aTimeModified == rNewStat.aTimeModified)) )
                return sal_False;
            break;
        case (FSYS_SORT_MODIFYED | FSYS_SORT_DESCENDING):
            if ( (pOldStat->aDateModified <= rNewStat.aDateModified) &&
                 (pOldStat->aTimeModified <  rNewStat.aTimeModified) )
                 return sal_True;
            if ( !((pOldStat->aDateModified == rNewStat.aDateModified) &&
                   (pOldStat->aTimeModified == rNewStat.aTimeModified)) )
                return sal_False;
            break;

        case  FSYS_SORT_CREATED:
        case (FSYS_SORT_CREATED | FSYS_SORT_ASCENDING):
            if ( (pOldStat->aDateCreated >= rNewStat.aDateCreated) &&
                 (pOldStat->aTimeCreated >  rNewStat.aTimeCreated) )
                 return sal_True;
            if ( !((pOldStat->aDateCreated == rNewStat.aDateCreated) &&
                   (pOldStat->aTimeCreated == rNewStat.aTimeCreated)) )
                return sal_False;
            break;
        case (FSYS_SORT_CREATED | FSYS_SORT_DESCENDING):
            if ( (pOldStat->aDateCreated <= rNewStat.aDateCreated) &&
                 (pOldStat->aTimeCreated <  rNewStat.aTimeCreated) )
                 return sal_True;
            if ( !((pOldStat->aDateCreated == rNewStat.aDateCreated) &&
                   (pOldStat->aTimeCreated == rNewStat.aTimeCreated)) )
                return sal_False;
            break;

        case  FSYS_SORT_ACCESSED:
        case (FSYS_SORT_ACCESSED | FSYS_SORT_ASCENDING):
            if ( (pOldStat->aDateAccessed >= rNewStat.aDateAccessed) &&
                 (pOldStat->aTimeAccessed >  rNewStat.aTimeAccessed) )
                 return sal_True;
            if ( !((pOldStat->aDateAccessed == rNewStat.aDateAccessed) &&
                   (pOldStat->aTimeAccessed == rNewStat.aTimeAccessed)) )
                return sal_False;
            break;
        case (FSYS_SORT_ACCESSED | FSYS_SORT_DESCENDING):
            if ( (pOldStat->aDateAccessed <= rNewStat.aDateAccessed) &&
                 (pOldStat->aTimeAccessed <  rNewStat.aTimeAccessed) )
                 return sal_True;
            if ( !((pOldStat->aDateAccessed == rNewStat.aDateAccessed) &&
                   (pOldStat->aTimeAccessed == rNewStat.aTimeAccessed)) )
                return sal_False;
            break;
        default: /* Kann nicht sein */;
    }

    if ( nSortIndex == ( pSortLst->size() - 1 ) )
        return sal_True;
    else
        //Rekursion
        return ImpInsertPointReached( rNewEntry, rNewStat,
                                      nCurPos, nSortIndex + 1 );
#undef VALUE
}

/*************************************************************************
|*
|*    Dir::ImpSortedInsert()
|*
|*    Beschreibung      fuegt sortiert ein
|*
*************************************************************************/

void Dir::ImpSortedInsert( const DirEntry *pNewEntry, const FileStat *pNewStat )
{
    //Sonderfall, keine Sortierung gewuenscht.
    if ( !pSortLst ) {
        pLst->push_back( (DirEntry*)pNewEntry );
        return;
    }

    for ( size_t i = 0, n = pLst->size(); i < n; ++i )
    {
        if ( ImpInsertPointReached( *pNewEntry, *pNewStat, i, 0  ) )
        {
            if ( pStatLst ) {
                FileStatList::iterator it = pStatLst->begin();
                ::std::advance( it, i );
                pStatLst->insert( it, (FileStat*)pNewStat );
            }
            DirEntryList::iterator it = pLst->begin();
            ::std::advance( it, i );
            pLst->insert( it, (DirEntry*)pNewEntry );
            return;
        }
    }

    if ( pStatLst )
        pStatLst->push_back( (FileStat*)pNewStat );
    pLst->push_back( (DirEntry*)pNewEntry );
}

/*************************************************************************
|*
|*    Dir::Construct()
|*
|*    Beschreibung      gemeinsame Implementation der Ctoren
|*
*************************************************************************/

void Dir::Construct( DirEntryKind nKindFlags )
{
    pLst     = NULL;
    pSortLst = NULL;
    pStatLst = NULL;
    eAttrMask = nKindFlags;
    rtl::OString aTempName(rtl::OUStringToOString(GetName(), osl_getThreadTextEncoding()));
    if (aTempName.indexOf('*') != -1 || aTempName.indexOf('?') != -1)
    {
#if defined( WNT )
        rtl::OString aTStr(rtl::OUStringToOString(CutName(), osl_getThreadTextEncoding()));
        char* pBuffer = new char[aTStr.getLength()+1];
        strcpy( pBuffer, aTStr.getStr() );
        CharLowerBuff( pBuffer, aTStr.getLength() );
        aNameMask = WildCard( String(pBuffer, osl_getThreadTextEncoding()), ';' );
        delete [] pBuffer;
#else
        aNameMask = WildCard( CutName(), ';' );
#endif
    }
    else
        aNameMask.setGlob(String("*", osl_getThreadTextEncoding()));
}

/*************************************************************************
|*
|*    Dir::Reset()
|*
*************************************************************************/

void Dir::Reset()
{
    // ggf. alten Reader l"oschen
    if ( pReader && pReader->bInUse )
        DELETEZ(pReader);

    // alle DirEntries aus der Liste entfernen und deren Speicher freigeben
    if ( pLst )
    {
        for ( size_t i = 0, n = pLst->size(); i < n; ++i ) {
            delete (*pLst)[ i ];
        }
        pLst->clear();
    }
    else
        pLst = new DirEntryList();

    //  Alte File-Stat's Loeschen
    if ( pStatLst )
    {
        for ( size_t i = 0, n = pStatLst->size(); i < n; ++i ) {
            delete (*pStatLst)[ i ];
        }
        pStatLst->clear();
        delete pStatLst;
        pStatLst = NULL;
    }

    // Verlangen die Sortierkriterien FileStat's?
    if ( pSortLst )
    {
        for ( size_t i = 0, n = pSortLst->size(); i < n; ++i ) {
            if ( (*pSortLst)[ i ]
               & ( FSYS_SORT_KIND     | FSYS_SORT_SIZE     | FSYS_SORT_CREATED
                 | FSYS_SORT_MODIFYED | FSYS_SORT_ACCESSED
                 )
            ) {
                pStatLst = new FileStatList();
                break;
            }
        }
    }

    // ggf. einen neuen Reader aufsetzen
    if ( !pReader )
        pReader = new DirReader_Impl( *this );

    // gibt es das zu oeffnende Verzeichnis ueberhaupt?
#if !defined(UNX)   //explanation: see DirReader_Impl::Read() in unx.cxx
    if( !pReader->pDosDir )
    {
        nError = FSYS_ERR_NOTADIRECTORY;
        DELETEZ( pReader );
        return;
    }
#endif
}

/*************************************************************************
|*
|*    Dir::Scan()
|*
*************************************************************************/

sal_uInt16 Dir::Scan( sal_uInt16 nCount )
{

    sal_uInt16 nRead = 0; // Anzahl in dieser Runde gelesener Eintr"age
    FSysFailOnErrorImpl();

    // noch nicht fertig gewesen
    if ( pReader )
    {
        // frischer Reader?
        if ( pLst->empty() )
        {
            // dann ggf. Laufwerke scannen
            pReader->bInUse = sal_True;
            nRead = pReader->Init();
        }

        // weiterlesen...
        while ( nRead <= nCount && !pReader->bReady )
            nRead = nRead + pReader->Read();

        // fertig?
        if ( pReader && pReader->bReady )
            DELETEZ( pReader );
    }

    // Anzahl der gelesenen zur"uckgeben
    return nRead;
}

/*************************************************************************
|*
|*    Dir::Dir()
|*
*************************************************************************/

Dir::Dir( const DirEntry& rDirEntry, DirEntryKind nKindFlags ):
    DirEntry( rDirEntry ),
    pReader( 0 )
{
    DBG_CTOR( Dir, NULL );

    Construct( nKindFlags );
    Reset();
}

/*************************************************************************
|*
|*    Dir::~Dir()
|*
*************************************************************************/

Dir::~Dir()
{
    DBG_DTOR( Dir, NULL );

    // alle DirEntries aus der Liste entfernen und deren Speicher freigeben
    if ( pLst )
    {
        for ( size_t i = 0, n = pLst->size(); i < n; ++i ) {
            delete (*pLst)[ i ];
        }
        pLst->clear();
        delete pLst;
    }

    // alle Sorts aus der Liste entfernen und deren Speicher freigeben
    if ( pSortLst )
    {
        pSortLst->clear();
        delete pSortLst;
    }

    // alle FileStat's aus der Liste entfernen und deren Speicher freigeben
    if ( pStatLst )
    {
        for ( size_t i = 0, n = pStatLst->size(); i < n; ++i ) {
            delete (*pStatLst)[ i ];
        }
        pStatLst->clear();
        delete pStatLst;
    }

    // ggf. laufenden Reader freigeben
    delete pReader;
}

/*************************************************************************
|*
|*    Dir::operator[]()
|*
*************************************************************************/

DirEntry& Dir::operator[] ( size_t nIndex ) const
{
    DBG_ASSERT( nIndex < Count(), "Dir::operator[] : nIndex > Count()" );

    DirEntry *pEntry = (*pLst)[ nIndex ];
    return *pEntry;
}

/*************************************************************************
|*
|*    Dir::operator+= ()
|*
*************************************************************************/

Dir& Dir::operator+=( const Dir& rDir )
{
    // ggf. erst den Rest lesen
    if ( pReader )
        Scan( USHRT_MAX );
    DBG_ASSERT( !rDir.pReader, "Dir::+= with incomplete Dir" );

    // ggf. initiale Liste erzeugen
    if ( !pLst )
        pLst = new DirEntryList();

    //Verlangen die Sortierkriterien FileStat's?
    sal_Bool bStat = sal_False;
    if ( pSortLst ) {
        for ( size_t i = 0, n = pSortLst->size(); i < n && !bStat; ++i ) {
            if ( (*pSortLst)[ i ]
               & ( FSYS_SORT_CREATED  | FSYS_SORT_MODIFYED | FSYS_SORT_SIZE
                 | FSYS_SORT_ACCESSED | FSYS_SORT_KIND
                 )
            ) {
                bStat = sal_True;
            }
        }
    }
    FileStat* stat = NULL;
    for ( size_t nNr = 0; nNr < rDir.Count(); nNr++ )
    {
        if ( bStat )
        {
            if ( rDir.pStatLst )
                stat = new FileStat( *(*rDir.pStatLst)[ nNr ] );
            else
                stat = new FileStat( rDir[nNr] );
        }
        ImpSortedInsert( new DirEntry( rDir[nNr] ), stat );
    }
    return *this;
}

/*************************************************************************
|*
|*    Dir::Count()
|*
*************************************************************************/


size_t Dir::Count( sal_Bool bUpdated ) const
{
    // ggf. erst den Rest lesen
    if ( bUpdated && pReader )
        ((Dir*)this)->Scan( USHRT_MAX );

    return pLst == NULL ? 0 : pLst->size();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
