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


// System - Includes -----------------------------------------------------



// INCLUDE ---------------------------------------------------------------

#include "undoutil.hxx"

#include "docsh.hxx"
#include "tabvwsh.hxx"
#include "document.hxx"
#include "dbdata.hxx"
#include "globstr.hrc"
#include "globalnames.hxx"
#include "global.hxx"
#include "markdata.hxx"

void ScUndoUtil::MarkSimpleBlock( ScDocShell* pDocShell,
                                SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                                SCCOL nEndX, SCROW nEndY, SCTAB nEndZ )
{
    if ( pDocShell->IsPaintLocked() )
        return;

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
    if (pViewShell)
    {
        SCTAB nViewTab = pViewShell->GetViewData()->GetTabNo();
        if ( nViewTab < nStartZ || nViewTab > nEndZ )
            pViewShell->SetTabNo( nStartZ );

        pViewShell->DoneBlockMode();
        pViewShell->MoveCursorAbs( nStartX, nStartY, SC_FOLLOW_JUMP, false, false );
        pViewShell->InitOwnBlockMode();
        pViewShell->GetViewData()->GetMarkData().
                SetMarkArea( ScRange( nStartX, nStartY, nStartZ, nEndX, nEndY, nEndZ ) );
        pViewShell->MarkDataChanged();
    }
}


void ScUndoUtil::MarkSimpleBlock( ScDocShell* pDocShell,
                                const ScAddress& rBlockStart,
                                const ScAddress& rBlockEnd )
{
    MarkSimpleBlock( pDocShell, rBlockStart.Col(), rBlockStart.Row(), rBlockStart.Tab(),
                                rBlockEnd.Col(), rBlockEnd.Row(), rBlockEnd.Tab() );
}


void ScUndoUtil::MarkSimpleBlock( ScDocShell* pDocShell,
                                const ScRange& rRange )
{
    MarkSimpleBlock( pDocShell, rRange.aStart.Col(), rRange.aStart.Row(), rRange.aStart.Tab(),
                                rRange.aEnd.Col(),   rRange.aEnd.Row(),   rRange.aEnd.Tab()   );
}



ScDBData* ScUndoUtil::GetOldDBData( ScDBData* pUndoData, ScDocument* pDoc, SCTAB nTab,
                                    SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2 )
{
    ScDBData* pRet = pDoc->GetDBAtArea( nTab, nCol1, nRow1, nCol2, nRow2 );

    if (!pRet)
    {
        bool bWasTemp = false;
        if ( pUndoData )
        {
            const ::rtl::OUString& aName = pUndoData->GetName();
            if ( aName == STR_DB_LOCAL_NONAME )
                bWasTemp = true;
        }
        OSL_ENSURE(bWasTemp, "Undo: didn't find database range");
        (void)bWasTemp;
        pRet = pDoc->GetAnonymousDBData(nTab);
        if (!pRet)
        {
            pRet = new ScDBData( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(STR_DB_LOCAL_NONAME)), nTab,
                                nCol1,nRow1, nCol2,nRow2, sal_True,
                                pDoc->HasColHeader( nCol1,nRow1,nCol2,nRow2,nTab ) );
            pDoc->SetAnonymousDBData(nTab,pRet);
        }
    }

    return pRet;
}


void ScUndoUtil::PaintMore( ScDocShell* pDocShell,
                                const ScRange& rRange )
{
    SCCOL nCol1 = rRange.aStart.Col();
    SCROW nRow1 = rRange.aStart.Row();
    SCCOL nCol2 = rRange.aEnd.Col();
    SCROW nRow2 = rRange.aEnd.Row();
    if (nCol1 > 0) --nCol1;
    if (nRow1 > 0) --nRow1;
    if (nCol2<MAXCOL) ++nCol2;
    if (nRow2<MAXROW) ++nRow2;

    pDocShell->PostPaint( nCol1,nRow1,rRange.aStart.Tab(),
                          nCol2,nRow2,rRange.aEnd.Tab(), PAINT_GRID );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
