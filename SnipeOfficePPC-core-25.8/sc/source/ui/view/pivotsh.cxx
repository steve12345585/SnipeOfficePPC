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




//------------------------------------------------------------------

#include "scitems.hxx"
#include <svl/srchitem.hxx>
#include <sfx2/app.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/request.hxx>
#include <svl/whiter.hxx>
#include <vcl/msgbox.hxx>

#include "sc.hrc"
#include "pivotsh.hxx"
#include "tabvwsh.hxx"
#include "docsh.hxx"
#include "scresid.hxx"
#include "document.hxx"
#include "dpobject.hxx"
#include "dpshttab.hxx"
#include "dbdocfun.hxx"
#include "uiitems.hxx"
#include "scabstdlg.hxx"
//------------------------------------------------------------------------

#define ScPivotShell
#include "scslots.hxx"

//------------------------------------------------------------------------

TYPEINIT1( ScPivotShell, SfxShell );

SFX_IMPL_INTERFACE(ScPivotShell, SfxShell, ScResId(SCSTR_PIVOTSHELL))
{
    SFX_POPUPMENU_REGISTRATION( ScResId(RID_POPUP_PIVOT) );
}


//------------------------------------------------------------------------

ScPivotShell::ScPivotShell( ScTabViewShell* pViewSh ) :
    SfxShell(pViewSh),
    pViewShell( pViewSh )
{
    SetPool( &pViewSh->GetPool() );
    ScViewData* pViewData = pViewSh->GetViewData();
    ::svl::IUndoManager* pMgr = pViewData->GetSfxDocShell()->GetUndoManager();
    SetUndoManager( pMgr );
    if ( !pViewData->GetDocument()->IsUndoEnabled() )
    {
        pMgr->SetMaxUndoActionCount( 0 );
    }
    SetHelpId( HID_SCSHELL_PIVOTSH );
    SetName(String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("Pivot")));
}

//------------------------------------------------------------------------
ScPivotShell::~ScPivotShell()
{
}

//------------------------------------------------------------------------
void ScPivotShell::Execute( SfxRequest& rReq )
{
    switch ( rReq.GetSlot() )
    {
        case SID_PIVOT_RECALC:
            pViewShell->RecalcPivotTable();
            break;

        case SID_PIVOT_KILL:
            pViewShell->DeletePivotTable();
            break;

        case SID_DP_FILTER:
        {
            ScDPObject* pDPObj = GetCurrDPObject();
            if( pDPObj )
            {
                ScQueryParam aQueryParam;
                SCTAB nSrcTab = 0;
                const ScSheetSourceDesc* pDesc = pDPObj->GetSheetDesc();
                OSL_ENSURE( pDesc, "no sheet source for DP filter dialog" );
                if( pDesc )
                {
                    aQueryParam = pDesc->GetQueryParam();
                    nSrcTab = pDesc->GetSourceRange().aStart.Tab();
                }

                ScViewData* pViewData = pViewShell->GetViewData();
                SfxItemSet aArgSet( pViewShell->GetPool(),
                    SCITEM_QUERYDATA, SCITEM_QUERYDATA );
                aArgSet.Put( ScQueryItem( SCITEM_QUERYDATA, pViewData, &aQueryParam ) );

                ScAbstractDialogFactory* pFact = ScAbstractDialogFactory::Create();
                OSL_ENSURE(pFact, "ScAbstractFactory create fail!");

                AbstractScPivotFilterDlg* pDlg = pFact->CreateScPivotFilterDlg( pViewShell->GetDialogParent(),
                                                                                aArgSet, nSrcTab,
                                                                                RID_SCDLG_PIVOTFILTER);
                OSL_ENSURE(pDlg, "Dialog create fail!");

                if( pDlg->Execute() == RET_OK )
                {
                    ScSheetSourceDesc aNewDesc(pViewData->GetDocument());
                    if( pDesc )
                        aNewDesc = *pDesc;

                    const ScQueryItem& rQueryItem = pDlg->GetOutputItem();
                    aNewDesc.SetQueryParam(rQueryItem.GetQueryData());

                    ScDPObject aNewObj( *pDPObj );
                    aNewObj.SetSheetDesc( aNewDesc );
                    ScDBDocFunc aFunc( *pViewData->GetDocShell() );
                    aFunc.DataPilotUpdate( pDPObj, &aNewObj, sal_True, false );
                    pViewData->GetView()->CursorPosChanged();       // shells may be switched
                }
                delete pDlg;
            }
        }
        break;
    }
}

//------------------------------------------------------------------------
void ScPivotShell::GetState( SfxItemSet& rSet )
{
    ScDocShell* pDocSh = pViewShell->GetViewData()->GetDocShell();
    ScDocument* pDoc = pDocSh->GetDocument();
    sal_Bool bDisable = pDocSh->IsReadOnly() || pDoc->GetChangeTrack();

    SfxWhichIter aIter(rSet);
    sal_uInt16 nWhich = aIter.FirstWhich();
    while (nWhich)
    {
        switch (nWhich)
        {
            case SID_PIVOT_RECALC:
            case SID_PIVOT_KILL:
            {
                //! move ReadOnly check to idl flags
                if ( bDisable )
                {
                    rSet.DisableItem( nWhich );
                }
            }
            break;
            case SID_DP_FILTER:
            {
                ScDPObject* pDPObj = GetCurrDPObject();
                if( bDisable || !pDPObj || !pDPObj->IsSheetData() )
                    rSet.DisableItem( nWhich );
            }
            break;
        }
        nWhich = aIter.NextWhich();
    }
}


//------------------------------------------------------------------------

ScDPObject* ScPivotShell::GetCurrDPObject()
{
    const ScViewData& rViewData = *pViewShell->GetViewData();
    return rViewData.GetDocument()->GetDPAtCursor(
        rViewData.GetCurX(), rViewData.GetCurY(), rViewData.GetTabNo() );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
