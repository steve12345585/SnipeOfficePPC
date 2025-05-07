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
#include <osl/mutex.hxx>

#include "nthesdta.hxx"
#include <linguistic/misc.hxx>

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

///////////////////////////////////////////////////////////////////////////

Meaning::Meaning(
#if 0
            const OUString &rTerm, sal_Int16 nLang,
            const PropertyHelper_Thes &rHelper ) :
#else
            const OUString &rTerm, sal_Int16 nLang) :
#endif

    aSyn        ( Sequence< OUString >(1) ),
    aTerm       (rTerm),
    nLanguage   (nLang)

{
#if 0
    // this is for future use by a german thesaurus when one exists
    bIsGermanPreReform = rHelper.IsGermanPreReform;
#endif
}


Meaning::~Meaning()
{
}


OUString SAL_CALL Meaning::getMeaning()
        throw(RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );
    return aTerm;
}


Sequence< OUString > SAL_CALL Meaning::querySynonyms()
        throw(RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );
        return aSyn;
}


void Meaning::SetSynonyms( const Sequence< OUString > &rSyn )
{
        MutexGuard      aGuard( GetLinguMutex() );
        aSyn = rSyn;
}

void Meaning::SetMeaning( const OUString &rTerm )
{
        MutexGuard      aGuard( GetLinguMutex() );
        aTerm = rTerm;
}

///////////////////////////////////////////////////////////////////////////

}   // namespace linguistic






/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
