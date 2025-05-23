/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include <editeng/editeng.hxx>
#include <editeng/outlobj.hxx>
#include <svx/svdocapt.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdundo.hxx>
#include <svx/svdview.hxx>
#include <editeng/editobj.hxx>
#include <vcl/cursor.hxx>
#include <sfx2/objsh.hxx>
#include <editeng/writingmodeitem.hxx>

#include "global.hxx"
#include "drwlayer.hxx"
#include "userdat.hxx"
#include "tabvwsh.hxx"          // oder GetDocument irgendwo
#include "document.hxx"
#include "editutil.hxx"
#include "futext.hxx"
#include "docsh.hxx"
#include "postit.hxx"
#include "globstr.hrc"
#include "attrib.hxx"
#include "scitems.hxx"
#include "drawview.hxx"
#include "undocell.hxx"

// ------------------------------------------------------------------------------------
//  Editieren von Notiz-Legendenobjekten muss immer ueber StopEditMode beendet werden,
//  damit die Aenderungen ins Dokument uebernommen werden!
//  (Fontwork-Execute in drawsh und drtxtob passiert nicht fuer Legendenobjekte)
//  bTextDirection=sal_True means that this function is called from SID_TEXTDIRECTION_XXX(drtxtob.cxx).
// ------------------------------------------------------------------------------------

void FuText::StopEditMode(sal_Bool /*bTextDirection*/)
{
    SdrObject* pObject = pView->GetTextEditObject();
    if( !pObject ) return;

    // relock the internal layer that has been unlocked in FuText::SetInEditMode()
    if ( pObject->GetLayer() == SC_LAYER_INTERN )
        pView->LockInternalLayer();

    ScViewData& rViewData = *pViewShell->GetViewData();
    ScDocument& rDoc = *rViewData.GetDocument();
    ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
    OSL_ENSURE( pDrawLayer && (pDrawLayer == pDrDoc), "FuText::StopEditMode - missing or different drawing layers" );

    ScAddress aNotePos;
    ScPostIt* pNote = 0;
    if( const ScDrawObjData* pCaptData = ScDrawLayer::GetNoteCaptionData( pObject, rViewData.GetTabNo() ) )
    {
        aNotePos = pCaptData->maStart;
        pNote = rDoc.GetNotes( aNotePos.Tab() )->findByAddress( aNotePos );
        OSL_ENSURE( pNote && (pNote->GetCaption() == pObject), "FuText::StopEditMode - missing or invalid cell note" );
    }

    ScDocShell* pDocShell = rViewData.GetDocShell();
    ::svl::IUndoManager* pUndoMgr = rDoc.IsUndoEnabled() ? pDocShell->GetUndoManager() : 0;
    bool bNewNote = false;
    if( pNote && pUndoMgr )
    {
        /*  Put all undo actions already collected (e.g. create caption object)
            and all following undo actions (text changed) together into a ListAction. */
        String aUndoStr = ScGlobal::GetRscString( STR_UNDO_EDITNOTE );
        pUndoMgr->EnterListAction( aUndoStr, aUndoStr );
        if( SdrUndoGroup* pCalcUndo = pDrawLayer->GetCalcUndo() )
        {
            /*  Note has been created before editing, if first undo action is
                an insert action. Needed below to decide whether to drop the
                undo if editing a new note has been cancelled. */
            bNewNote = (pCalcUndo->GetActionCount() > 0) && pCalcUndo->GetAction( 0 )->ISA( SdrUndoNewObj );
            // create a "insert note" undo action if needed
            if( bNewNote )
                pUndoMgr->AddUndoAction( new ScUndoReplaceNote( *pDocShell, aNotePos, pNote->GetNoteData(), true, pCalcUndo ) );
            else
                pUndoMgr->AddUndoAction( pCalcUndo );
        }
    }

    if( pNote )
        rDoc.LockStreamValid(true);     // only the affected sheet is invalidated below

    /*  SdrObjEditView::SdrEndTextEdit() may try to delete the entire drawing
        object, if it does not contain text and has invisible border and fill.
        This must not happen for note caption objects. They will be removed
        below together with the cell note if the text is empty (independent of
        border and area formatting). It is possible to prevent automatic
        deletion by passing sal_True to this function. The return value changes
        from SDRENDTEXTEDIT_DELETED to SDRENDTEXTEDIT_SHOULDBEDELETED in this
        case. */
    /*SdrEndTextEditKind eResult =*/ pView->SdrEndTextEdit( pNote != 0 );

    // or ScEndTextEdit (with drawview.hxx)
    pViewShell->SetDrawTextUndo( 0 );

    Cursor* pCur = pWindow->GetCursor();
    if( pCur && pCur->IsVisible() )
        pCur->Hide();

    if( pNote )
    {
        // hide the caption object if it is in hidden state
        pNote->ShowCaptionTemp( aNotePos, false );

        // update author and date
        pNote->AutoStamp();

        /*  If the entire text has been cleared, the cell note and its caption
            object have to be removed. */
        SdrTextObj* pTextObject = dynamic_cast< SdrTextObj* >( pObject );
        bool bDeleteNote = !pTextObject || !pTextObject->HasText();
        if( bDeleteNote )
        {
            if( pUndoMgr )
            {
                // collect the "remove object" drawing undo action created by DeleteNote()
                pDrawLayer->BeginCalcUndo();
                // rescue note data before deletion
                ScNoteData aNoteData( pNote->GetNoteData() );
                // delete note from document (removes caption, but does not delete it)
                rDoc.GetNotes( aNotePos.Tab() )->erase( aNotePos );
                // create undo action for removed note
                pUndoMgr->AddUndoAction( new ScUndoReplaceNote( *pDocShell, aNotePos, aNoteData, false, pDrawLayer->GetCalcUndo() ) );
            }
            else
            {
                rDoc.GetNotes( aNotePos.Tab() )->erase( aNotePos );
            }
            // ScDocument::DeleteNote has deleted the note that pNote points to
            pNote = 0;
        }

        // finalize the undo list action
        if( pUndoMgr )
        {
            pUndoMgr->LeaveListAction();

            /*  #i94039# Update the default name "Edit Note" of the undo action
                if the note has been created before editing or is deleted due
                to deleted text. If the note has been created *and* is deleted,
                the last undo action can be removed completely. Note: The
                function LeaveListAction() removes the last action by itself,
                if it is empty (when result is SDRENDTEXTEDIT_UNCHANGED). */
            if( bNewNote && bDeleteNote )
            {
                pUndoMgr->RemoveLastUndoAction();
            }
            else if( bNewNote || bDeleteNote )
            {
                SfxListUndoAction* pAction = dynamic_cast< SfxListUndoAction* >( pUndoMgr->GetUndoAction() );
                OSL_ENSURE( pAction, "FuText::StopEditMode - list undo action expected" );
                if( pAction )
                    pAction->SetComment( ScGlobal::GetRscString( bNewNote ? STR_UNDO_INSERTNOTE : STR_UNDO_DELETENOTE ) );
            }
        }

        // invalidate stream positions only for the affected sheet
        rDoc.LockStreamValid(false);
        if (rDoc.IsStreamValid(aNotePos.Tab()))
            rDoc.SetStreamValid(aNotePos.Tab(), false);
    }
}

// Called following an EndDragObj() to update the new note rectangle position
void FuText::StopDragMode(SdrObject* /*pObject*/)
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
