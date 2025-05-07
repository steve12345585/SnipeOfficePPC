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


#ifndef _SVDLAYER
#include <svx/svdlayer.hxx>
#endif
#include <sfx2/dispatch.hxx>
#include <sfx2/viewfrm.hxx>


#include "strings.hrc"
#include "glob.hxx"
#include "glob.hrc"         // STR_BCKGRND, STR_BCKGRNDOBJ
#include "app.hrc"          // SID_SWITCHPAGE

#include "unmodpg.hxx"
#include "sdpage.hxx"
#include "sdresid.hxx"
#include "drawdoc.hxx"


TYPEINIT1(ModifyPageUndoAction, SdUndoAction);


/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

ModifyPageUndoAction::ModifyPageUndoAction(
    SdDrawDocument* pTheDoc,
    SdPage* pThePage,
    String aTheNewName,
    AutoLayout  eTheNewAutoLayout,
    sal_Bool bTheNewBckgrndVisible,
    sal_Bool bTheNewBckgrndObjsVisible)
:   SdUndoAction(pTheDoc)
{
    DBG_ASSERT(pThePage, "Undo ohne Seite ???");

    mpPage                  = pThePage;
    maNewName               = aTheNewName;
    meNewAutoLayout         = eTheNewAutoLayout;
    mbNewBckgrndVisible     = bTheNewBckgrndVisible;
    mbNewBckgrndObjsVisible = bTheNewBckgrndObjsVisible;

    meOldAutoLayout         = mpPage->GetAutoLayout();

    if (!mpPage->IsMasterPage())
    {
        maOldName = mpPage->GetName();
        SdrLayerAdmin& rLayerAdmin = mpDoc->GetLayerAdmin();
        sal_uInt8 aBckgrnd = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRND)), sal_False);
        sal_uInt8 aBckgrndObj = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRNDOBJ)), sal_False);
        SetOfByte aVisibleLayers = mpPage->TRG_GetMasterPageVisibleLayers();

        mbOldBckgrndVisible = aVisibleLayers.IsSet(aBckgrnd);
        mbOldBckgrndObjsVisible = aVisibleLayers.IsSet(aBckgrndObj);
    }

    maComment = String(SdResId(STR_UNDO_MODIFY_PAGE));
}

/*************************************************************************
|*
|* Undo()
|*
\************************************************************************/
#include <svx/svdviter.hxx>
#include <svx/svdview.hxx>
void ModifyPageUndoAction::Undo()
{
    // invalidate Selection, there could be objects deleted in tis UNDO
    // which are no longer allowed to be selected then.
      SdrViewIter aIter(mpPage);
    SdrView* pView = aIter.FirstView();

    while(pView)
    {
        if(pView->AreObjectsMarked())
            pView->UnmarkAll();
        pView = aIter.NextView();
    }

    mpPage->SetAutoLayout( meOldAutoLayout );

    if (!mpPage->IsMasterPage())
    {
        if (mpPage->GetName() != maOldName)
        {
            mpPage->SetName(maOldName);

            if (mpPage->GetPageKind() == PK_STANDARD)
            {
                SdPage* pNotesPage = (SdPage*)mpDoc->GetPage(mpPage->GetPageNum() + 1);
                pNotesPage->SetName(maOldName);
            }
        }

        SdrLayerAdmin& rLayerAdmin = mpDoc->GetLayerAdmin();
        sal_uInt8 aBckgrnd = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRND)), sal_False);
        sal_uInt8 aBckgrndObj = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRNDOBJ)), sal_False);
        SetOfByte aVisibleLayers;
        aVisibleLayers.Set(aBckgrnd, mbOldBckgrndVisible);
        aVisibleLayers.Set(aBckgrndObj, mbOldBckgrndObjsVisible);
        mpPage->TRG_SetMasterPageVisibleLayers(aVisibleLayers);
    }

    // Redisplay
    SfxViewFrame::Current()->GetDispatcher()->Execute(
        SID_SWITCHPAGE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
}

/*************************************************************************
|*
|* Redo()
|*
\************************************************************************/

void ModifyPageUndoAction::Redo()
{
    // invalidate Selection, there could be objects deleted in tis UNDO
    // which are no longer allowed to be selected then.
      SdrViewIter aIter(mpPage);
    SdrView* pView = aIter.FirstView();

    while(pView)
    {
        if(pView->AreObjectsMarked())
            pView->UnmarkAll();
        pView = aIter.NextView();
    }

    mpPage->meAutoLayout = meNewAutoLayout;

    if (!mpPage->IsMasterPage())
    {
        if (mpPage->GetName() != maNewName)
        {
            mpPage->SetName(maNewName);

            if (mpPage->GetPageKind() == PK_STANDARD)
            {
                SdPage* pNotesPage = (SdPage*)mpDoc->GetPage(mpPage->GetPageNum() + 1);
                pNotesPage->SetName(maNewName);
            }
        }

        SdrLayerAdmin& rLayerAdmin = mpDoc->GetLayerAdmin();
        sal_uInt8 aBckgrnd = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRND)), sal_False);
        sal_uInt8 aBckgrndObj = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRNDOBJ)), sal_False);
        SetOfByte aVisibleLayers;
        aVisibleLayers.Set(aBckgrnd, mbNewBckgrndVisible);
        aVisibleLayers.Set(aBckgrndObj, mbNewBckgrndObjsVisible);
        mpPage->TRG_SetMasterPageVisibleLayers(aVisibleLayers);
    }

    // Redisplay
    SfxViewFrame::Current()->GetDispatcher()->Execute(
        SID_SWITCHPAGE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

ModifyPageUndoAction::~ModifyPageUndoAction()
{
}

/*************************************************************************
|*
|* Kommentar liefern
|*
\************************************************************************/

rtl::OUString ModifyPageUndoAction::GetComment() const
{
    return maComment;
}

// --------------------------------------------------------------------

RenameLayoutTemplateUndoAction::RenameLayoutTemplateUndoAction( SdDrawDocument* pDocument, const String& rOldLayoutName, const String& rNewLayoutName )
: SdUndoAction(pDocument)
, maOldName( rOldLayoutName )
, maNewName( rNewLayoutName )
, maComment(SdResId(STR_TITLE_RENAMESLIDE))
{
    sal_uInt16 nPos = maOldName.SearchAscii( SD_LT_SEPARATOR );
    if( nPos != (sal_uInt16)-1 )
        maOldName.Erase(nPos);
}

void RenameLayoutTemplateUndoAction::Undo()
{
    String aLayoutName( maNewName );
    aLayoutName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( SD_LT_SEPARATOR ));
    aLayoutName.Append( String(SdResId(STR_LAYOUT_OUTLINE))) ;

    mpDoc->RenameLayoutTemplate( aLayoutName, maOldName );
}

void RenameLayoutTemplateUndoAction::Redo()
{
    String aLayoutName( maOldName );
    aLayoutName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( SD_LT_SEPARATOR ));
    aLayoutName.Append( String(SdResId(STR_LAYOUT_OUTLINE))) ;

    mpDoc->RenameLayoutTemplate( aLayoutName, maNewName );
}

rtl::OUString RenameLayoutTemplateUndoAction::GetComment() const
{
    return  maComment;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
