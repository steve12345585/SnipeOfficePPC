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

#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/linguistic2/SpellFailure.hpp>
#include <com/sun/star/linguistic2/XSearchableDictionaryList.hpp>
#include <comphelper/string.hxx>
#include <tools/debug.hxx>
#include <osl/mutex.hxx>

#include <vector>

#include "linguistic/spelldta.hxx"
#include "lngsvcmgr.hxx"


using namespace utl;
using namespace osl;
using namespace com::sun::star;
using namespace com::sun::star::beans;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace com::sun::star::linguistic2;

using ::rtl::OUString;

namespace linguistic
{

#define MAX_PROPOSALS   40

sal_Bool SeqHasEntry(
        const Sequence< OUString > &rSeq,
        const OUString &rTxt)
{
    sal_Bool bRes = sal_False;
    sal_Int32 nLen = rSeq.getLength();
    const OUString *pEntry = rSeq.getConstArray();
    for (sal_Int32 i = 0;  i < nLen  &&  !bRes;  ++i)
    {
        if (rTxt == pEntry[i])
            bRes = sal_True;
    }
    return bRes;
}


void SearchSimilarText( const OUString &rText, sal_Int16 nLanguage,
        Reference< XDictionaryList > &xDicList,
        std::vector< OUString > & rDicListProps )
{
    if (!xDicList.is())
        return;

    const uno::Sequence< Reference< XDictionary > >
            aDics( xDicList->getDictionaries() );
    const Reference< XDictionary >
            *pDic = aDics.getConstArray();
    sal_Int32 nDics = xDicList->getCount();

    for (sal_Int32 i = 0;  i < nDics;  i++)
    {
        Reference< XDictionary > xDic( pDic[i], UNO_QUERY );

        sal_Int16           nLang = LocaleToLanguage( xDic->getLocale() );

        if ( xDic.is() && xDic->isActive()
            && (nLang == nLanguage  ||  nLang == LANGUAGE_NONE) )
        {
#if OSL_DEBUG_LEVEL > 1
            DictionaryType  eType = xDic->getDictionaryType();
            (void) eType;
            DBG_ASSERT( eType != DictionaryType_MIXED, "unexpected dictionary type" );
#endif
            const Sequence< Reference< XDictionaryEntry > > aEntries = xDic->getEntries();
            const Reference< XDictionaryEntry > *pEntries = aEntries.getConstArray();
            sal_Int32 nLen = aEntries.getLength();
            for (sal_Int32 k = 0;  k < nLen;  ++k)
            {
                String aEntryTxt;
                if (pEntries[k].is())
                {
                    // remove characters used to determine hyphenation positions
                    aEntryTxt = comphelper::string::remove(pEntries[k]->getDictionaryWord(), '=');
                }
                if (aEntryTxt.Len() > 0  &&  LevDistance( rText, aEntryTxt ) <= 2)
                    rDicListProps.push_back( aEntryTxt );
            }
        }
    }
}


void SeqRemoveNegEntries( Sequence< OUString > &rSeq,
        Reference< XDictionaryList > &rxDicList,
        sal_Int16 nLanguage )
{
    static const OUString aEmpty;
    sal_Bool bSthRemoved = sal_False;
    sal_Int32 nLen = rSeq.getLength();
    OUString *pEntries = rSeq.getArray();
    for (sal_Int32 i = 0;  i < nLen;  ++i)
    {
        Reference< XDictionaryEntry > xNegEntry( SearchDicList( rxDicList,
                    pEntries[i], nLanguage, sal_False, sal_True ) );
        if (xNegEntry.is())
        {
            pEntries[i] = aEmpty;
            bSthRemoved = sal_True;
        }
    }
    if (bSthRemoved)
    {
        Sequence< OUString > aNew;
        // merge sequence without duplicates and empty strings in new empty sequence
        aNew = MergeProposalSeqs( aNew, rSeq, sal_False );
        rSeq = aNew;
    }
}


Sequence< OUString > MergeProposalSeqs(
            Sequence< OUString > &rAlt1,
            Sequence< OUString > &rAlt2,
            sal_Bool bAllowDuplicates )
{
    Sequence< OUString > aMerged;

    if (0 == rAlt1.getLength() && bAllowDuplicates)
        aMerged = rAlt2;
    else if (0 == rAlt2.getLength() && bAllowDuplicates)
        aMerged = rAlt1;
    else
    {
        sal_Int32 nAltCount1 = rAlt1.getLength();
        const OUString *pAlt1 = rAlt1.getConstArray();
        sal_Int32 nAltCount2 = rAlt2.getLength();
        const OUString *pAlt2 = rAlt2.getConstArray();

        sal_Int32 nCountNew = Min( nAltCount1 + nAltCount2, (sal_Int32) MAX_PROPOSALS );
        aMerged.realloc( nCountNew );
        OUString *pMerged = aMerged.getArray();

        sal_Int32 nIndex = 0;
        sal_Int32 i = 0;
        for (int j = 0;  j < 2;  j++)
        {
            sal_Int32           nCount  = j == 0 ? nAltCount1 : nAltCount2;
            const OUString  *pAlt   = j == 0 ? pAlt1 : pAlt2;
            for (i = 0;  i < nCount  &&  nIndex < MAX_PROPOSALS;  i++)
            {
                if (!pAlt[i].isEmpty() &&
                    (bAllowDuplicates || !SeqHasEntry(aMerged, pAlt[i] )))
                    pMerged[ nIndex++ ] = pAlt[ i ];
            }
        }
        aMerged.realloc( nIndex );
    }

    return aMerged;
}



SpellAlternatives::SpellAlternatives()
{
    nLanguage   = LANGUAGE_NONE;
    nType       = SpellFailure::IS_NEGATIVE_WORD;
}


SpellAlternatives::SpellAlternatives(
        const OUString &rWord, sal_Int16 nLang, sal_Int16 nFailureType,
        const Sequence< OUString > &rAlternatives ) :
    aAlt        (rAlternatives),
    aWord       (rWord),
    nType       (nFailureType),
    nLanguage   (nLang)
{
}


SpellAlternatives::~SpellAlternatives()
{
}


OUString SAL_CALL SpellAlternatives::getWord()
        throw(RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );
    return aWord;
}


Locale SAL_CALL SpellAlternatives::getLocale()
        throw(RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );
    return CreateLocale( nLanguage );
}


sal_Int16 SAL_CALL SpellAlternatives::getFailureType()
        throw(RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );
    return nType;
}


sal_Int16 SAL_CALL SpellAlternatives::getAlternativesCount()
        throw(RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );
    return (sal_Int16) aAlt.getLength();
}


Sequence< OUString > SAL_CALL SpellAlternatives::getAlternatives()
        throw(RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );
    return aAlt;
}


void SAL_CALL SpellAlternatives::setAlternatives( const uno::Sequence< OUString >& rAlternatives )
throw (uno::RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );
    aAlt = rAlternatives;
}


void SAL_CALL SpellAlternatives::setFailureType( sal_Int16 nFailureType )
throw (uno::RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );
    nType = nFailureType;
}


void SpellAlternatives::SetWordLanguage(const OUString &rWord, sal_Int16 nLang)
{
    MutexGuard  aGuard( GetLinguMutex() );
    aWord = rWord;
    nLanguage = nLang;
}


void SpellAlternatives::SetFailureType(sal_Int16 nTypeP)
{
    MutexGuard  aGuard( GetLinguMutex() );
    nType = nTypeP;
}


void SpellAlternatives::SetAlternatives( const Sequence< OUString > &rAlt )
{
    MutexGuard  aGuard( GetLinguMutex() );
    aAlt = rAlt;
}


com::sun::star::uno::Reference < com::sun::star::linguistic2::XSpellAlternatives > SpellAlternatives::CreateSpellAlternatives(
        const ::rtl::OUString &rWord, sal_Int16 nLang, sal_Int16 nTypeP, const ::com::sun::star::uno::Sequence< ::rtl::OUString > &rAlt )
{
    SpellAlternatives* pAlt = new SpellAlternatives;
    pAlt->SetWordLanguage( rWord, nLang );
    pAlt->SetFailureType( nTypeP );
    pAlt->SetAlternatives( rAlt );
    return Reference < XSpellAlternatives >(pAlt);
}


}   // namespace linguistic

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
