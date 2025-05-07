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


#include <i18npool/lang.h>
#include <sortopt.hxx>
#include <boost/foreach.hpp>

/*--------------------------------------------------------------------
    Description: Sort Key
 --------------------------------------------------------------------*/
SwSortKey::SwSortKey() :
    eSortOrder( SRT_ASCENDING ),
    nColumnId( 0 ),
    bIsNumeric( sal_True )
{
}

SwSortKey::SwSortKey(sal_uInt16 nId, const String& rSrtType, SwSortOrder eOrder) :
    sSortType( rSrtType ),
    eSortOrder( eOrder ),
    nColumnId( nId ),
    bIsNumeric( 0 == rSrtType.Len() )
{
}

SwSortKey::SwSortKey(const SwSortKey& rOld) :
    sSortType( rOld.sSortType ),
    eSortOrder( rOld.eSortOrder ),
    nColumnId( rOld.nColumnId ),
    bIsNumeric( rOld.bIsNumeric )
{
}

/*--------------------------------------------------------------------
    Description: Sorting options for Sorting
 --------------------------------------------------------------------*/
SwSortOptions::SwSortOptions()
    : eDirection( SRT_ROWS ),
    cDeli( 9 ),
    nLanguage( LANGUAGE_SYSTEM ),
    bTable( sal_False ),
    bIgnoreCase( sal_False )
{
}

SwSortOptions::SwSortOptions(const SwSortOptions& rOpt) :
    eDirection( rOpt.eDirection ),
    cDeli( rOpt.cDeli ),
    nLanguage( rOpt.nLanguage ),
    bTable( rOpt.bTable ),
    bIgnoreCase( rOpt.bIgnoreCase )
{
    for( sal_uInt16 i=0; i < rOpt.aKeys.size(); ++i )
    {
        SwSortKey* pNew = new SwSortKey(*rOpt.aKeys[i]);
        aKeys.push_back( pNew );
    }
}

SwSortOptions::~SwSortOptions()
{
    BOOST_FOREACH(SwSortKey *pKey, aKeys)
        delete pKey;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
