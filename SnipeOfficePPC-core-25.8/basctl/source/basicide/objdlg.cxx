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

#include "basidesh.hrc"
#include "objdlg.hrc"

#include "basidesh.hxx"
#include "iderdll.hxx"
#include "iderdll2.hxx"
#include "objdlg.hxx"

#include <sfx2/app.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/request.hxx>
#include <sfx2/viewfrm.hxx>
#include <svl/itemset.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/taskpanelist.hxx>
#include <vcl/sound.hxx>

ObjectCatalog::ObjectCatalog( Window * pParent )
    :BasicDockingWindow( pParent, IDEResId( RID_BASICIDE_OBJCAT ) )
    ,aMacroTreeList( this, IDEResId( RID_TLB_MACROS ) )
    ,aToolBox(this, IDEResId(RID_TB_TOOLBOX))
    ,aMacroDescr( this, IDEResId( RID_FT_MACRODESCR ) )
{
    FreeResource();

    aToolBox.SetOutStyle( TOOLBOX_STYLE_FLAT );
    aToolBox.SetSizePixel( aToolBox.CalcWindowSizePixel() );
    aToolBox.SetSelectHdl( LINK( this, ObjectCatalog, ToolBoxHdl ) );

    aMacroTreeList.SetStyle( WB_BORDER | WB_TABSTOP |
                             WB_HASLINES | WB_HASLINESATROOT |
                             WB_HASBUTTONS | WB_HASBUTTONSATROOT |
                             WB_HSCROLL );

    aMacroTreeList.SetSelectHdl( LINK( this, ObjectCatalog, TreeListHighlightHdl ) );
    aMacroTreeList.SetAccessibleName(String(IDEResId(RID_STR_TLB_MACROS)));
    aMacroTreeList.ScanAllEntries();
    aMacroTreeList.GrabFocus();

    CheckButtons();

    Point aPos = BasicIDEGlobals::GetExtraData()->GetObjectCatalogPos();
    Size aSize = BasicIDEGlobals::GetExtraData()->GetObjectCatalogSize();
    if ( aPos.X() == INVPOSITION )
    {
        // centered after AppWin:
        Window* pWin = GetParent();
        aPos = pWin->OutputToScreenPixel( Point( 0, 0 ) );
        Size aAppWinSz = pWin->GetSizePixel();
        Size aDlgWinSz = GetSizePixel();
        aPos.X() += aAppWinSz.Width() / 2;
        aPos.X() -= aDlgWinSz.Width() / 2;
        aPos.Y() += aAppWinSz.Height() / 2;
        aPos.Y() -= aDlgWinSz.Height() / 2;
    }
    SetPosPixel( aPos );
    if ( aSize.Width() )
        SetOutputSizePixel( aSize );

    Resize();   // so that the resize-handler arranges the controls

    // make object catalog keyboard accessible
    pParent->GetSystemWindow()->GetTaskPaneList()->AddWindow( this );
}

ObjectCatalog::~ObjectCatalog()
{
    GetParent()->GetSystemWindow()->GetTaskPaneList()->RemoveWindow( this );
}

void ObjectCatalog::Paint( const Rectangle& )
{
    String sOC = GetText();
    long nPos = GetSizePixel().Width()/2-GetTextWidth(sOC)/2;
    DrawText( Point( nPos, 10 ), String( sOC ) );
}

void ObjectCatalog::Move()
{
    BasicIDEGlobals::GetExtraData()->SetObjectCatalogPos( GetPosPixel() );
}

sal_Bool ObjectCatalog::Close()
{
    aCancelHdl.Call( this );
    return sal_True;
}

void ObjectCatalog::Resize()
{
    Size aOutSz = GetOutputSizePixel();
    BasicIDEGlobals::GetExtraData()->SetObjectCatalogSize( aOutSz );

    Point aTreePos = aMacroTreeList.GetPosPixel();
    Size aDescrSz = aMacroDescr.GetSizePixel();

    Size aTreeSz;
    long nCtrlWidth = aOutSz.Width() - 2*aTreePos.X();
    aTreeSz.Width() = nCtrlWidth;
    aTreeSz.Height() = aOutSz.Height() - aTreePos.Y() -
                        2*aTreePos.X() - aDescrSz.Height();

    if ( aTreeSz.Height() > 0 )
    {
        aMacroTreeList.SetSizePixel( aTreeSz );

        Point aDescrPos( aTreePos.X(), aTreePos.Y()+aTreeSz.Height()+aTreePos.X() );

        aMacroDescr.SetPosSizePixel( aDescrPos, Size( nCtrlWidth, aDescrSz.Height() ) );

        String aDesc = aMacroDescr.GetText();
        aMacroDescr.SetText(String());
        aMacroDescr.SetText(aDesc);
    }

    // the buttons above always stay unmodified
}

IMPL_LINK( ObjectCatalog, ToolBoxHdl, ToolBox*, pToolBox )
{
    sal_uInt16 nCurItem = pToolBox->GetCurItemId();
    switch ( nCurItem )
    {
        case TBITEM_SHOW:
        {
            SfxAllItemSet aArgs( SFX_APP()->GetPool() );
            SfxRequest aRequest( SID_BASICIDE_APPEAR, SFX_CALLMODE_SYNCHRON, aArgs );
            SFX_APP()->ExecuteSlot( aRequest );

            SvLBoxEntry* pCurEntry = aMacroTreeList.GetCurEntry();
            DBG_ASSERT( pCurEntry, "Entry?!" );
            BasicEntryDescriptor aDesc( aMacroTreeList.GetEntryDescriptor( pCurEntry ) );
            BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
            SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
            SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
            if ( aDesc.GetType() == OBJ_TYPE_MODULE ||
                 aDesc.GetType() == OBJ_TYPE_DIALOG ||
                 aDesc.GetType() == OBJ_TYPE_METHOD )
            {
                if( pDispatcher )
                {
                    SbxItem aSbxItem( SID_BASICIDE_ARG_SBX, aDesc.GetDocument(), aDesc.GetLibName(), aDesc.GetName(),
                                      aDesc.GetMethodName(), aMacroTreeList.ConvertType( aDesc.GetType() ) );
                    pDispatcher->Execute( SID_BASICIDE_SHOWSBX,
                                          SFX_CALLMODE_SYNCHRON, &aSbxItem, 0L );
                }
            }
            else
            {
                ErrorBox( this, WB_OK, String( IDEResId( RID_STR_OBJNOTFOUND ) ) ).Execute();
                aMacroTreeList.GetModel()->Remove( pCurEntry );
                CheckButtons();
            }
        }
        break;
    }

    return 0;
}



void ObjectCatalog::CheckButtons()
{
    SvLBoxEntry* pCurEntry = aMacroTreeList.GetCurEntry();
    BasicEntryType eType = pCurEntry ? ((BasicEntry*)pCurEntry->GetUserData())->GetType() : OBJ_TYPE_UNKNOWN;
    if ( eType == OBJ_TYPE_DIALOG || eType == OBJ_TYPE_MODULE || eType == OBJ_TYPE_METHOD )
        aToolBox.EnableItem( TBITEM_SHOW, sal_True );
    else
        aToolBox.EnableItem( TBITEM_SHOW, sal_False );
}



IMPL_LINK_INLINE_START( ObjectCatalog, TreeListHighlightHdl, SvTreeListBox *, pBox )
{
    if ( pBox->IsSelected( pBox->GetHdlEntry() ) )
        UpdateFields();
    return 0;
}
IMPL_LINK_INLINE_END( ObjectCatalog, TreeListHighlightHdl, SvTreeListBox *, pBox )


void ObjectCatalog::UpdateFields()
{
    SvLBoxEntry* pCurEntry = aMacroTreeList.GetCurEntry();
    if ( pCurEntry )
    {
        CheckButtons();
        aMacroDescr.SetText( String() );
        SbxVariable* pVar = aMacroTreeList.FindVariable( pCurEntry );
        if ( pVar )
        {
            SbxInfoRef xInfo = pVar->GetInfo();
            if ( xInfo.Is() )
                aMacroDescr.SetText( xInfo->GetComment() );
        }
    }
}


void ObjectCatalog::UpdateEntries()
{
    aMacroTreeList.UpdateEntries();
}

void ObjectCatalog::SetCurrentEntry (IDEBaseWindow* pCurWin)
{
    BasicEntryDescriptor aDesc;
    if (pCurWin)
        aDesc = pCurWin->CreateEntryDescriptor();
    aMacroTreeList.SetCurrentEntry(aDesc);
}

ObjectCatalogToolBox_Impl::ObjectCatalogToolBox_Impl(
    Window * pParent, ResId const & rResId)
    : ToolBox(pParent, rResId)
    , m_aImagesNormal(GetImageList())
{
    setImages();
}

// virtual
void ObjectCatalogToolBox_Impl::DataChanged(DataChangedEvent const & rDCEvt)
{
    ToolBox::DataChanged(rDCEvt);
    if ((rDCEvt.GetType() == DATACHANGED_SETTINGS
         || rDCEvt.GetType() == DATACHANGED_DISPLAY)
        && (rDCEvt.GetFlags() & SETTINGS_STYLE) != 0)
        setImages();
}

void ObjectCatalogToolBox_Impl::setImages()
{
    SetImageList(m_aImagesNormal);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
