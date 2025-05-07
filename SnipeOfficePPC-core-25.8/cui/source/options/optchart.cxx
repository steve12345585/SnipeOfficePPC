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

#include <unotools/pathoptions.hxx>
#include <cuires.hrc>
#include "optchart.hxx"
#include "optchart.hrc"
#include <dialmgr.hxx>
#include <vcl/msgbox.hxx>
#include <svx/svxids.hrc> // for SID_SCH_EDITOPTIONS

// ====================
// class ChartColorLB
// ====================
void ChartColorLB::FillBox( const SvxChartColorTable & rTab )
{
    long nCount = rTab.size();
    SetUpdateMode( sal_False );

    for( long i = 0; i < nCount; i++ )
    {
        Append( const_cast< XColorEntry * >( & rTab[ i ] ));
    }
    SetUpdateMode( sal_True );
}


// ====================
// class SvxDefaultColorOptPage
// ====================
SvxDefaultColorOptPage::SvxDefaultColorOptPage( Window* pParent, const SfxItemSet& rInAttrs ) :

    SfxTabPage( pParent, CUI_RES( RID_OPTPAGE_CHART_DEFCOLORS ), rInAttrs ),

    aGbChartColors  ( this, CUI_RES( FL_CHART_COLOR_LIST ) ),
    aLbChartColors  ( this, CUI_RES( LB_CHART_COLOR_LIST ) ),
    aGbColorBox     ( this, CUI_RES( FL_COLOR_BOX ) ),
    aValSetColorBox ( this, CUI_RES( CT_COLOR_BOX ) ),
    aPBDefault      ( this, CUI_RES( PB_RESET_TO_DEFAULT ) ),
    aPBAdd              ( this, CUI_RES( PB_ADD_CHART_COLOR ) ),
    aPBRemove           ( this, CUI_RES( PB_REMOVE_CHART_COLOR ) )
{
    FreeResource();

    aPBDefault.SetClickHdl( LINK( this, SvxDefaultColorOptPage, ResetToDefaults ) );
    aPBAdd.SetClickHdl( LINK( this, SvxDefaultColorOptPage, AddChartColor ) );
    aPBRemove.SetClickHdl( LINK( this, SvxDefaultColorOptPage, RemoveChartColor ) );
    aLbChartColors.SetSelectHdl( LINK( this, SvxDefaultColorOptPage, ListClickedHdl ) );
    aValSetColorBox.SetSelectHdl( LINK( this, SvxDefaultColorOptPage, BoxClickedHdl ) );

    aValSetColorBox.SetStyle( aValSetColorBox.GetStyle()
                                    | WB_ITEMBORDER | WB_NAMEFIELD );
    aValSetColorBox.SetColCount( 8 );
    aValSetColorBox.SetLineCount( 13 );
    aValSetColorBox.SetExtraSpacing( 0 );
    aValSetColorBox.Show();

    pChartOptions = new SvxChartOptions;
    pColorList = XColorList::CreateStdColorList();

    const SfxPoolItem* pItem = NULL;
    if ( rInAttrs.GetItemState( SID_SCH_EDITOPTIONS, sal_False, &pItem ) == SFX_ITEM_SET )
    {
        pColorConfig = (static_cast< SvxChartColorTableItem* >(pItem->Clone()) );
    }
    else
    {
        SvxChartColorTable aTable;
        aTable.useDefault();
        pColorConfig = new SvxChartColorTableItem( SID_SCH_EDITOPTIONS, aTable );
        pColorConfig->SetOptions( pChartOptions );
    }

    Construct();
}

SvxDefaultColorOptPage::~SvxDefaultColorOptPage()
{
    // save changes
    pChartOptions->SetDefaultColors( pColorConfig->GetColorList() );
    pChartOptions->Commit();

    delete pColorConfig;
    delete pChartOptions;
}

void SvxDefaultColorOptPage::Construct()
{
    if( pColorConfig )
        aLbChartColors.FillBox( pColorConfig->GetColorList() );

    FillColorBox();

    aLbChartColors.SelectEntryPos( 0 );
    ListClickedHdl( &aLbChartColors );
}


SfxTabPage* SvxDefaultColorOptPage::Create( Window* pParent, const SfxItemSet& rAttrs )
{
    return new SvxDefaultColorOptPage( pParent, rAttrs );
}

sal_Bool SvxDefaultColorOptPage::FillItemSet( SfxItemSet& rOutAttrs )
{
    if( pColorConfig )
        rOutAttrs.Put( *(static_cast< SfxPoolItem* >(pColorConfig)));

    return sal_True;
}

void SvxDefaultColorOptPage::Reset( const SfxItemSet& )
{
    aLbChartColors.SelectEntryPos( 0 );
    ListClickedHdl( &aLbChartColors );
}

void SvxDefaultColorOptPage::FillColorBox()
{
    if( !pColorList.is() ) return;

    long nCount = pColorList->Count();
    XColorEntry* pColorEntry;

    if( nCount > 104 )
        aValSetColorBox.SetStyle( aValSetColorBox.GetStyle() | WB_VSCROLL );

    for( long i = 0; i < nCount; i++ )
    {
        pColorEntry = pColorList->GetColor( i );
        aValSetColorBox.InsertItem( (sal_uInt16) i + 1, pColorEntry->GetColor(), pColorEntry->GetName() );
    }
}


long SvxDefaultColorOptPage::GetColorIndex( const Color& rCol )
{
    if( pColorList.is() )
    {
        long nCount = pColorList->Count();
        XColorEntry* pColorEntry;

        for( long i = nCount - 1; i >= 0; i-- )         // default chart colors are at the end of the table
        {
            pColorEntry = pColorList->GetColor( i );
            if( pColorEntry && pColorEntry->GetColor() == rCol )
                return pColorList->Get( pColorEntry->GetName() );
        }
    }
    return -1L;
}



// --------------------
// event handlers
// --------------------

// ResetToDefaults
// ---------------

IMPL_LINK_NOARG(SvxDefaultColorOptPage, ResetToDefaults)
{
    if( pColorConfig )
    {
        pColorConfig->GetColorList().useDefault();

        aLbChartColors.Clear();
        aLbChartColors.FillBox( pColorConfig->GetColorList() );

        aLbChartColors.GetFocus();
        aLbChartColors.SelectEntryPos( 0 );
        aPBRemove.Enable( sal_True );
    }

    return 0L;
}

// AddChartColor
// ------------

IMPL_LINK_NOARG(SvxDefaultColorOptPage, AddChartColor)
{
    if( pColorConfig )
    {
        ColorData black = RGB_COLORDATA( 0x00, 0x00, 0x00 );

        pColorConfig->GetColorList().append (XColorEntry ( black, pColorConfig->GetColorList().getDefaultName(pColorConfig->GetColorList().size())));

        aLbChartColors.Clear();
        aLbChartColors.FillBox( pColorConfig->GetColorList() );

        aLbChartColors.GetFocus();
        aLbChartColors.SelectEntryPos( pColorConfig->GetColorList().size() - 1 );
        aPBRemove.Enable( sal_True );
    }

    return 0L;
}

// RemoveChartColor
// ----------------

IMPL_LINK( SvxDefaultColorOptPage, RemoveChartColor, PushButton*, pButton )
{
    size_t nIndex = aLbChartColors.GetSelectEntryPos();

    if (aLbChartColors.GetSelectEntryCount() == 0)
        return 0L;

    if( pColorConfig )
    {
        OSL_ENSURE(pColorConfig->GetColorList().size() > 1, "don't delete the last chart color");
        QueryBox aQuery(pButton, CUI_RES(RID_OPTQB_COLOR_CHART_DELETE));
        aQuery.SetText(String(CUI_RES(RID_OPTSTR_COLOR_CHART_DELETE)));
        if(RET_YES == aQuery.Execute())
        {

            pColorConfig->GetColorList().remove( nIndex  );

            aLbChartColors.Clear();
            aLbChartColors.FillBox( pColorConfig->GetColorList() );

            aLbChartColors.GetFocus();

            if (nIndex == aLbChartColors.GetEntryCount() && aLbChartColors.GetEntryCount() > 0)
                aLbChartColors.SelectEntryPos( pColorConfig->GetColorList().size() - 1 );
            else if (aLbChartColors.GetEntryCount() > 0)
                aLbChartColors.SelectEntryPos( nIndex );
            else
                aPBRemove.Enable( sal_False );
        }
    }

    return 0L;
}

IMPL_LINK( SvxDefaultColorOptPage, ListClickedHdl, ChartColorLB*, _pColorList )
{
    Color aCol = _pColorList->GetSelectEntryColor();

    long nIndex = GetColorIndex( aCol );

    if( nIndex == -1 )      // not found
        aValSetColorBox.SetNoSelection();
    else
        aValSetColorBox.SelectItem( (sal_uInt16)nIndex + 1 );       // ValueSet is 1-based

    return 0L;
}

IMPL_LINK_NOARG(SvxDefaultColorOptPage, BoxClickedHdl)
{
    sal_uInt16 nIdx = aLbChartColors.GetSelectEntryPos();
    if( nIdx != LISTBOX_ENTRY_NOTFOUND )
    {
        XColorEntry aEntry( aValSetColorBox.GetItemColor( aValSetColorBox.GetSelectItemId() ),
                            aLbChartColors.GetSelectEntry() );

        aLbChartColors.Modify( & aEntry, nIdx );
        pColorConfig->ReplaceColorByIndex( nIdx, aEntry );

        aLbChartColors.SelectEntryPos( nIdx );  // reselect entry
    }

    return 0L;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
