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




#include "editable.hxx"
#include "document.hxx"
#include "viewfunc.hxx"
#include "globstr.hrc"
#include "markdata.hxx"

//------------------------------------------------------------------

ScEditableTester::ScEditableTester() :
    bIsEditable( sal_True ),
    bOnlyMatrix( sal_True )
{
}

ScEditableTester::ScEditableTester( ScDocument* pDoc, SCTAB nTab,
                        SCCOL nStartCol, SCROW nStartRow, SCCOL nEndCol, SCROW nEndRow ) :
    bIsEditable( sal_True ),
    bOnlyMatrix( sal_True )
{
    TestBlock( pDoc, nTab, nStartCol, nStartRow, nEndCol, nEndRow );
}

ScEditableTester::ScEditableTester( ScDocument* pDoc,
                        SCCOL nStartCol, SCROW nStartRow, SCCOL nEndCol, SCROW nEndRow,
                        const ScMarkData& rMark ) :
    bIsEditable( sal_True ),
    bOnlyMatrix( sal_True )
{
    TestSelectedBlock( pDoc, nStartCol, nStartRow, nEndCol, nEndRow, rMark );
}

ScEditableTester::ScEditableTester( ScDocument* pDoc, const ScRange& rRange ) :
    bIsEditable( sal_True ),
    bOnlyMatrix( sal_True )
{
    TestRange( pDoc, rRange );
}

ScEditableTester::ScEditableTester( ScDocument* pDoc, const ScMarkData& rMark ) :
    bIsEditable( sal_True ),
    bOnlyMatrix( sal_True )
{
    TestSelection( pDoc, rMark );
}

ScEditableTester::ScEditableTester( ScViewFunc* pView ) :
    bIsEditable( sal_True ),
    bOnlyMatrix( sal_True )
{
    TestView( pView );
}

//------------------------------------------------------------------

void ScEditableTester::TestBlock( ScDocument* pDoc, SCTAB nTab,
                        SCCOL nStartCol, SCROW nStartRow, SCCOL nEndCol, SCROW nEndRow )
{
    if ( bIsEditable || bOnlyMatrix )
    {
        bool bThisMatrix;
        if ( !pDoc->IsBlockEditable( nTab, nStartCol, nStartRow, nEndCol, nEndRow, &bThisMatrix ) )
        {
            bIsEditable = false;
            if ( !bThisMatrix )
                bOnlyMatrix = false;
        }
    }
}

void ScEditableTester::TestSelectedBlock( ScDocument* pDoc,
                        SCCOL nStartCol, SCROW nStartRow, SCCOL nEndCol, SCROW nEndRow,
                        const ScMarkData& rMark )
{
    SCTAB nTabCount = pDoc->GetTableCount();
    ScMarkData::const_iterator itr = rMark.begin(), itrEnd = rMark.end();
    for (; itr != itrEnd && *itr < nTabCount; ++itr)
        TestBlock( pDoc, *itr, nStartCol, nStartRow, nEndCol, nEndRow );
}

void ScEditableTester::TestRange( ScDocument* pDoc, const ScRange& rRange )
{
    SCCOL nStartCol = rRange.aStart.Col();
    SCROW nStartRow = rRange.aStart.Row();
    SCTAB nStartTab = rRange.aStart.Tab();
    SCCOL nEndCol = rRange.aEnd.Col();
    SCROW nEndRow = rRange.aEnd.Row();
    SCTAB nEndTab = rRange.aEnd.Tab();
    for (SCTAB nTab=nStartTab; nTab<=nEndTab; nTab++)
        TestBlock( pDoc, nTab, nStartCol, nStartRow, nEndCol, nEndRow );
}

void ScEditableTester::TestSelection( ScDocument* pDoc, const ScMarkData& rMark )
{
    if ( bIsEditable || bOnlyMatrix )
    {
        bool bThisMatrix;
        if ( !pDoc->IsSelectionEditable( rMark, &bThisMatrix ) )
        {
            bIsEditable = false;
            if ( !bThisMatrix )
                bOnlyMatrix = false;
        }
    }
}

void ScEditableTester::TestView( ScViewFunc* pView )
{
    if ( bIsEditable || bOnlyMatrix )
    {
        bool bThisMatrix;
        if ( !pView->SelectionEditable( &bThisMatrix ) )
        {
            bIsEditable = false;
            if ( !bThisMatrix )
                bOnlyMatrix = false;
        }
    }
}

//------------------------------------------------------------------

sal_uInt16 ScEditableTester::GetMessageId() const
{
    if (bIsEditable)
        return 0;
    else if (bOnlyMatrix)
        return STR_MATRIXFRAGMENTERR;
    else
        return STR_PROTECTIONERR;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
