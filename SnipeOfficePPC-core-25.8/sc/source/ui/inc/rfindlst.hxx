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

#ifndef SC_RFINDLST_HXX
#define SC_RFINDLST_HXX

#include <tools/color.hxx>
#include "global.hxx"
#include "address.hxx"

//==================================================================

struct ScRangeFindData
{
    ScRange     aRef;
    sal_uInt16      nFlags;
    xub_StrLen  nSelStart;
    xub_StrLen  nSelEnd;

    ScRangeFindData( const ScRange& rR, sal_uInt16 nF, xub_StrLen nS, xub_StrLen nE ) :
        aRef(rR), nFlags(nF), nSelStart(nS), nSelEnd(nE) {}
};

class ScRangeFindList
{
    std::vector<ScRangeFindData> maEntries;
    String      aDocName;
    bool        bHidden;

public:
            ScRangeFindList(const String& rName);

    sal_uLong   Count() const                       { return maEntries.size(); }
    void    Insert( const ScRangeFindData &rNew )       { maEntries.push_back(rNew); }

    ScRangeFindData* GetObject( sal_uLong nIndex ) { return &(maEntries[nIndex]); }

    void    SetHidden( sal_Bool bSet )              { bHidden = bSet; }

    const String&   GetDocName() const          { return aDocName; }
    bool            IsHidden() const            { return bHidden; }

    static ColorData GetColorName( size_t nIndex );
};



#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
