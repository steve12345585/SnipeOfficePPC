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


// System - Includes ---------------------------------------------------------



// INCLUDE -------------------------------------------------------------------

#include "global.hxx"
#include "reffact.hxx"
#include "document.hxx"
#include "docsh.hxx"
#include "scresid.hxx"
#include "globstr.hrc"
#include "highred.hrc"

#include "highred.hxx"
#include <vcl/msgbox.hxx>
#include <sfx2/app.hxx>

// defines -------------------------------------------------------------------

#define ABS_SREF          SCA_VALID \
                        | SCA_COL_ABSOLUTE | SCA_ROW_ABSOLUTE | SCA_TAB_ABSOLUTE
#define ABS_DREF          ABS_SREF \
                        | SCA_COL2_ABSOLUTE | SCA_ROW2_ABSOLUTE | SCA_TAB2_ABSOLUTE
#define ABS_SREF3D      ABS_SREF | SCA_TAB_3D
#define ABS_DREF3D      ABS_DREF | SCA_TAB_3D



#define ERRORBOX(s) ErrorBox(this,WinBits(WB_OK|WB_DEF_OK),s).Execute();

inline void EnableDisable( Window& rWin, sal_Bool bEnable )
{
    if (bEnable)
        rWin.Enable();
    else
        rWin.Disable();
}

//============================================================================
//  class ScHighlightChgDlg

//----------------------------------------------------------------------------
ScHighlightChgDlg::ScHighlightChgDlg( SfxBindings* pB, SfxChildWindow* pCW, Window* pParent,
                      ScViewData*       ptrViewData)

    :   ScAnyRefDlg ( pB, pCW, pParent, RID_SCDLG_HIGHLIGHT_CHANGES ),
        //
        aHighlightBox   ( this, ScResId( CB_HIGHLIGHT)),
        aFlFilter       ( this, ScResId( FL_FILTER)),
        aFilterCtr      ( this),
        aCbAccept       ( this, ScResId( CB_HIGHLIGHT_ACCEPT)),
        aCbReject       ( this, ScResId( CB_HIGHLIGHT_REJECT)),
        aOkButton       ( this, ScResId( BTN_OK ) ),
        aCancelButton   ( this, ScResId( BTN_CANCEL ) ),
        aHelpButton     ( this, ScResId( BTN_HELP ) ),
        aEdAssign       ( this, this, ScResId( ED_ASSIGN ) ),
        aRbAssign       ( this, ScResId( RB_ASSIGN ), &aEdAssign, this ),
        //
        pViewData       ( ptrViewData ),
        pDoc            ( ptrViewData->GetDocument() ),
        aLocalRangeName ( *(pDoc->GetRangeName()) )
{
    FreeResource();

    Point aFlFilterPt( aFlFilter.GetPosPixel() );
    aFlFilterPt.Y() += aFlFilter.GetSizePixel().Height();
    aFilterCtr.SetPosPixel( aFlFilterPt );
    MinSize=aFilterCtr.GetSizePixel();
    MinSize.Height()+=2;
    MinSize.Width()+=2;
    aOkButton.SetClickHdl(LINK( this, ScHighlightChgDlg, OKBtnHdl));
    aHighlightBox.SetClickHdl(LINK( this, ScHighlightChgDlg, HighLightHandle ));
    aFilterCtr.SetRefHdl(LINK( this, ScHighlightChgDlg, RefHandle ));
    aFilterCtr.HideRange(false);
    aFilterCtr.Show();
    SetDispatcherLock( true );

    Init();

}

ScHighlightChgDlg::~ScHighlightChgDlg()
{
    SetDispatcherLock( false );
}

void ScHighlightChgDlg::Init()
{
    String  aAreaStr;
    ScRange aRange;

    OSL_ENSURE( pViewData && pDoc, "ViewData oder Document nicht gefunden!" );

    ScChangeTrack* pChanges=pDoc->GetChangeTrack();
    if(pChanges!=NULL)
    {
        aChangeViewSet.SetTheAuthorToShow(pChanges->GetUser());
        aFilterCtr.ClearAuthors();
        const std::set<rtl::OUString>& rUserColl = pChanges->GetUserCollection();
        std::set<rtl::OUString>::const_iterator it = rUserColl.begin(), itEnd = rUserColl.end();
        for (; it != itEnd; ++it)
            aFilterCtr.InsertAuthor(*it);
    }


    ScChangeViewSettings* pViewSettings=pDoc->GetChangeViewSettings();

    if(pViewSettings!=NULL)
        aChangeViewSet=*pViewSettings;
    aHighlightBox.Check(aChangeViewSet.ShowChanges());
    aFilterCtr.CheckDate(aChangeViewSet.HasDate());
    aFilterCtr.SetFirstDate(aChangeViewSet.GetTheFirstDateTime());
    aFilterCtr.SetFirstTime(aChangeViewSet.GetTheFirstDateTime());
    aFilterCtr.SetLastDate(aChangeViewSet.GetTheLastDateTime());
    aFilterCtr.SetLastTime(aChangeViewSet.GetTheLastDateTime());
    aFilterCtr.SetDateMode((sal_uInt16)aChangeViewSet.GetTheDateMode());
    aFilterCtr.CheckAuthor(aChangeViewSet.HasAuthor());
    aFilterCtr.CheckComment(aChangeViewSet.HasComment());
    aFilterCtr.SetComment(aChangeViewSet.GetTheComment());

    aCbAccept.Check(aChangeViewSet.IsShowAccepted());
    aCbReject.Check(aChangeViewSet.IsShowRejected());

    String aString=aChangeViewSet.GetTheAuthorToShow();
    if(aString.Len()!=0)
    {
        aFilterCtr.SelectAuthor(aString);
    }
    else
    {
        aFilterCtr.SelectedAuthorPos(0);
    }

    aFilterCtr.CheckRange(aChangeViewSet.HasRange());

    if ( !aChangeViewSet.GetTheRangeList().empty() )
    {
        String aRefStr;
        const ScRange* pRangeEntry = aChangeViewSet.GetTheRangeList().front();
        pRangeEntry->Format( aRefStr, ABS_DREF3D, pDoc );
        aFilterCtr.SetRange(aRefStr);
    }
    aFilterCtr.Enable(sal_True,sal_True);
    HighLightHandle(&aHighlightBox);
}

//----------------------------------------------------------------------------
// Uebergabe eines mit der Maus selektierten Tabellenbereiches, der dann als
// neue Selektion im Referenz-Edit angezeigt wird.

void ScHighlightChgDlg::SetReference( const ScRange& rRef, ScDocument* pDocP )
{
    if ( aEdAssign.IsVisible() )
    {
        if ( rRef.aStart != rRef.aEnd )
            RefInputStart(&aEdAssign);
        String aRefStr;
        rRef.Format( aRefStr, ABS_DREF3D, pDocP, pDocP->GetAddressConvention() );
        aEdAssign.SetRefString( aRefStr );
        aFilterCtr.SetRange(aRefStr);
    }
}

//----------------------------------------------------------------------------
sal_Bool ScHighlightChgDlg::Close()
{
    return DoClose( ScHighlightChgDlgWrapper::GetChildWindowId() );
}

void ScHighlightChgDlg::RefInputDone( sal_Bool bForced)
{
    ScAnyRefDlg::RefInputDone(bForced);
    if(bForced || !aRbAssign.IsVisible())
    {
        aFilterCtr.SetRange(aEdAssign.GetText());
        aFilterCtr.SetFocusToRange();
        aEdAssign.Hide();
        aRbAssign.Hide();
    }
}

void ScHighlightChgDlg::SetActive()
{
}

sal_Bool ScHighlightChgDlg::IsRefInputMode() const
{
    return aEdAssign.IsVisible();
}

IMPL_LINK( ScHighlightChgDlg, HighLightHandle, CheckBox*, pCb )
{
    if(pCb!=NULL)
    {
        if(aHighlightBox.IsChecked())
        {
            aFilterCtr.Enable(sal_True,sal_True);
            aCbAccept.Enable();
            aCbReject.Enable();
        }
        else
        {
            aFilterCtr.Disable(sal_True);
            aCbAccept.Disable();
            aCbReject.Disable();
        }
    }
    return 0;
}

IMPL_LINK( ScHighlightChgDlg, RefHandle, SvxTPFilter*, pRef )
{
    if(pRef!=NULL)
    {
        SetDispatcherLock( true );
        aEdAssign.Show();
        aRbAssign.Show();
        aEdAssign.SetText(aFilterCtr.GetRange());
        ScAnyRefDlg::RefInputStart(&aEdAssign,&aRbAssign);
    }
    return 0;
}

IMPL_LINK( ScHighlightChgDlg, OKBtnHdl, PushButton*, pOKBtn )
{
    if ( pOKBtn == &aOkButton)
    {
        aChangeViewSet.SetShowChanges(aHighlightBox.IsChecked());
        aChangeViewSet.SetHasDate(aFilterCtr.IsDate());
        ScChgsDateMode eMode = (ScChgsDateMode) aFilterCtr.GetDateMode();
        aChangeViewSet.SetTheDateMode( eMode );
        Date aFirstDate( aFilterCtr.GetFirstDate() );
        Time aFirstTime( aFilterCtr.GetFirstTime() );
        Date aLastDate( aFilterCtr.GetLastDate() );
        Time aLastTime( aFilterCtr.GetLastTime() );
        aChangeViewSet.SetTheFirstDateTime( DateTime( aFirstDate, aFirstTime ) );
        aChangeViewSet.SetTheLastDateTime( DateTime( aLastDate, aLastTime ) );
        aChangeViewSet.SetHasAuthor(aFilterCtr.IsAuthor());
        aChangeViewSet.SetTheAuthorToShow(aFilterCtr.GetSelectedAuthor());
        aChangeViewSet.SetHasRange(aFilterCtr.IsRange());
        aChangeViewSet.SetShowAccepted(aCbAccept.IsChecked());
        aChangeViewSet.SetShowRejected(aCbReject.IsChecked());
        aChangeViewSet.SetHasComment(aFilterCtr.IsComment());
        aChangeViewSet.SetTheComment(aFilterCtr.GetComment());
        ScRangeList aLocalRangeList;
        aLocalRangeList.Parse(aFilterCtr.GetRange(), pDoc);
        aChangeViewSet.SetTheRangeList(aLocalRangeList);
        aChangeViewSet.AdjustDateMode( *pDoc );
        pDoc->SetChangeViewSettings(aChangeViewSet);
        pViewData->GetDocShell()->PostPaintGridAll();
        Close();
    }
    return 0;
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
