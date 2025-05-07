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

#ifndef SC_SORTPARAM_HXX
#define SC_SORTPARAM_HXX

#define DEFSORT 3

#include <vector>

#include "address.hxx"
#include <tools/solar.h>
#include <com/sun/star/lang/Locale.hpp>
#include "scdllapi.h"

struct ScSubTotalParam;
struct ScQueryParam;

struct ScSortKeyState
{
    bool     bDoSort;
    SCCOLROW nField;
    bool     bAscending;
};

typedef ::std::vector<ScSortKeyState> ScSortKeyStateVec;

struct SC_DLLPUBLIC ScSortParam
{
    SCCOL       nCol1;
    SCROW       nRow1;
    SCCOL       nCol2;
    SCROW       nRow2;
    sal_uInt16  nUserIndex;
    bool        bHasHeader;
    bool        bByRow;
    bool        bCaseSens;
    bool        bNaturalSort;
    bool        bUserDef;
    bool        bIncludePattern;
    bool        bInplace;
    SCTAB       nDestTab;
    SCCOL       nDestCol;
    SCROW       nDestRow;
    ScSortKeyStateVec maKeyState;
    ::com::sun::star::lang::Locale aCollatorLocale;
    ::rtl::OUString aCollatorAlgorithm;
    sal_uInt16  nCompatHeader;


    ScSortParam();
    ScSortParam( const ScSortParam& r );
    /// SubTotals sort
    ScSortParam( const ScSubTotalParam& rSub, const ScSortParam& rOld );
    /// TopTen sort
    ScSortParam( const ScQueryParam&, SCCOL nCol );

    ScSortParam&    operator=  ( const ScSortParam& r );
    bool            operator== ( const ScSortParam& rOther ) const;
    void            Clear       ();
    void            MoveToDest();

    inline sal_uInt16 GetSortKeyCount() const { return maKeyState.size(); }
};


#endif // SC_SORTPARAM_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
