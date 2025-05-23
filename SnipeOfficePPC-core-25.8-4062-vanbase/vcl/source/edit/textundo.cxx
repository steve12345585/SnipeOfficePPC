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

#include "textundo.hxx"
#include "textund2.hxx"
#include "textundo.hrc"

#include <vcl/texteng.hxx>
#include <vcl/textview.hxx>
#include <vcl/textdata.hxx>
#include <textdoc.hxx>
#include <textdat2.hxx>
#include <svdata.hxx> // ImplGetResMgr()
#include <tools/resid.hxx>

TYPEINIT1( TextUndo, SfxUndoAction );
TYPEINIT1( TextUndoDelPara, TextUndo );
TYPEINIT1( TextUndoConnectParas, TextUndo );
TYPEINIT1( TextUndoSplitPara, TextUndo );
TYPEINIT1( TextUndoInsertChars, TextUndo );
TYPEINIT1( TextUndoRemoveChars, TextUndo );


namespace
{

// Shorten() -- inserts ellipsis (...) in the middle of a long text
void Shorten (rtl::OUString& rString)
{
    unsigned nLen = rString.getLength();
    if (nLen > 48)
    {
        // If possible, we don't break a word, hence first we look for a space.
        // Space before the ellipsis:
        int iFirst = rString.lastIndexOf(' ', 32);
        if (iFirst == -1 || unsigned(iFirst) < 16)
            iFirst = 24; // not possible
        // Space after the ellipsis:
        int iLast = rString.indexOf(' ', nLen - 16);
        if (iLast == -1 || unsigned(iLast) > nLen - 4)
            iLast = nLen - 8; // not possible
        // finally:
        rString =
            rString.copy(0, iFirst + 1) +
            "..." +
            rString.copy(iLast);
    }
}

} // namespace

//
// TextUndoManager
// ===============
//

TextUndoManager::TextUndoManager( TextEngine* p )
{
    mpTextEngine = p;
}

TextUndoManager::~TextUndoManager()
{
}

sal_Bool TextUndoManager::Undo()
{
    if ( GetUndoActionCount() == 0 )
        return sal_False;

    UndoRedoStart();

    mpTextEngine->SetIsInUndo( sal_True );
    sal_Bool bDone = SfxUndoManager::Undo();
    mpTextEngine->SetIsInUndo( sal_False );

    UndoRedoEnd();

    return bDone;
}

sal_Bool TextUndoManager::Redo()
{
    if ( GetRedoActionCount() == 0 )
        return sal_False;


    UndoRedoStart();

    mpTextEngine->SetIsInUndo( sal_True );
    sal_Bool bDone = SfxUndoManager::Redo();
    mpTextEngine->SetIsInUndo( sal_False );

    UndoRedoEnd();

    return bDone;
}

void TextUndoManager::UndoRedoStart()
{
    DBG_ASSERT( GetView(), "Undo/Redo: Active View?" );

//  if ( GetView() )
//      GetView()->HideSelection();
}

void TextUndoManager::UndoRedoEnd()
{
    if ( GetView() )
    {
        TextSelection aNewSel( GetView()->GetSelection() );
        aNewSel.GetStart() = aNewSel.GetEnd();
        GetView()->ImpSetSelection( aNewSel );
    }

    mpTextEngine->UpdateSelections();

    mpTextEngine->FormatAndUpdate( GetView() );
}


//
// TextUndo
// ========
//

TextUndo::TextUndo( TextEngine* p )
{
    mpTextEngine = p;
}

TextUndo::~TextUndo()
{
}

rtl::OUString TextUndo::GetComment() const
{
    return rtl::OUString();
}

void TextUndo::SetSelection( const TextSelection& rSel )
{
    if ( GetView() )
        GetView()->ImpSetSelection( rSel );
}


//
// TextUndoDelPara
// ===============
//

TextUndoDelPara::TextUndoDelPara( TextEngine* pTextEngine, TextNode* pNode, sal_uLong nPara )
                    : TextUndo( pTextEngine )
{
    mpNode = pNode;
    mnPara = nPara;
    mbDelObject = sal_True;
}

TextUndoDelPara::~TextUndoDelPara()
{
    if ( mbDelObject )
        delete mpNode;
}

void TextUndoDelPara::Undo()
{
    GetTextEngine()->InsertContent( mpNode, mnPara );
    mbDelObject = sal_False;    // gehoert wieder der Engine

    if ( GetView() )
    {
        TextSelection aSel( TextPaM( mnPara, 0 ), TextPaM( mnPara, mpNode->GetText().Len() ) );
        SetSelection( aSel );
    }
}

void TextUndoDelPara::Redo()
{
    // pNode stimmt nicht mehr, falls zwischendurch Undos, in denen
    // Absaetze verschmolzen sind.
    mpNode = GetDoc()->GetNodes().GetObject( mnPara );

    delete GetTEParaPortions()->GetObject( mnPara );
    GetTEParaPortions()->Remove( mnPara );

    // Node nicht loeschen, haengt im Undo!
    GetDoc()->GetNodes().Remove( mnPara );
    GetTextEngine()->ImpParagraphRemoved( mnPara );

    mbDelObject = sal_True; // gehoert wieder dem Undo

    sal_uLong nParas = GetDoc()->GetNodes().Count();
    sal_uLong n = mnPara < nParas ? mnPara : (nParas-1);
    TextNode* pN = GetDoc()->GetNodes().GetObject( n );
    TextPaM aPaM( n, pN->GetText().Len() );
    SetSelection( aPaM );
}

rtl::OUString TextUndoDelPara::GetComment () const
{
    return ResId(STR_TEXTUNDO_DELPARA, *ImplGetResMgr());
}


//
// TextUndoConnectParas
// ====================
//

TextUndoConnectParas::TextUndoConnectParas( TextEngine* pTextEngine, sal_uLong nPara, sal_uInt16 nPos )
                    :   TextUndo( pTextEngine )
{
    mnPara = nPara;
    mnSepPos = nPos;
}

TextUndoConnectParas::~TextUndoConnectParas()
{
}

void TextUndoConnectParas::Undo()
{
    TextPaM aPaM = GetTextEngine()->SplitContent( mnPara, mnSepPos );
    SetSelection( aPaM );
}

void TextUndoConnectParas::Redo()
{
    TextPaM aPaM = GetTextEngine()->ConnectContents( mnPara );
    SetSelection( aPaM );
}

rtl::OUString TextUndoConnectParas::GetComment () const
{
    return ResId(STR_TEXTUNDO_CONNECTPARAS, *ImplGetResMgr());
}


//
// TextUndoSplitPara
// =================
//

TextUndoSplitPara::TextUndoSplitPara( TextEngine* pTextEngine, sal_uLong nPara, sal_uInt16 nPos )
                    : TextUndo( pTextEngine )
{
    mnPara = nPara;
    mnSepPos = nPos;
}

TextUndoSplitPara::~TextUndoSplitPara()
{
}

void TextUndoSplitPara::Undo()
{
    TextPaM aPaM = GetTextEngine()->ConnectContents( mnPara );
    SetSelection( aPaM );
}

void TextUndoSplitPara::Redo()
{
    TextPaM aPaM = GetTextEngine()->SplitContent( mnPara, mnSepPos );
    SetSelection( aPaM );
}

rtl::OUString TextUndoSplitPara::GetComment () const
{
    return ResId(STR_TEXTUNDO_SPLITPARA, *ImplGetResMgr());
}


//
// TextUndoInsertChars
// ===================
//

TextUndoInsertChars::TextUndoInsertChars( TextEngine* pTextEngine, const TextPaM& rTextPaM, const XubString& rStr )
                    : TextUndo( pTextEngine ),
                        maTextPaM( rTextPaM ), maText( rStr )
{
}

void TextUndoInsertChars::Undo()
{
    TextSelection aSel( maTextPaM, maTextPaM );
    aSel.GetEnd().GetIndex() = aSel.GetEnd().GetIndex() + maText.Len();
    TextPaM aPaM = GetTextEngine()->ImpDeleteText( aSel );
    SetSelection( aPaM );
}

void TextUndoInsertChars::Redo()
{
    TextSelection aSel( maTextPaM, maTextPaM );
    GetTextEngine()->ImpInsertText( aSel, maText );
    TextPaM aNewPaM( maTextPaM );
    aNewPaM.GetIndex() = aNewPaM.GetIndex() + maText.Len();
    SetSelection( TextSelection( aSel.GetStart(), aNewPaM ) );
}

sal_Bool TextUndoInsertChars::Merge( SfxUndoAction* pNextAction )
{
    if ( !pNextAction->ISA( TextUndoInsertChars ) )
        return sal_False;

    TextUndoInsertChars* pNext = (TextUndoInsertChars*)pNextAction;

    if ( maTextPaM.GetPara() != pNext->maTextPaM.GetPara() )
        return sal_False;

    if ( ( maTextPaM.GetIndex() + maText.Len() ) == pNext->maTextPaM.GetIndex() )
    {
        maText += pNext->maText;
        return sal_True;
    }
    return sal_False;
}

rtl::OUString TextUndoInsertChars::GetComment () const
{
    // multiple lines?
    rtl::OUString sText(maText);
    Shorten(sText);
    return rtl::OUString(ResId(STR_TEXTUNDO_INSERTCHARS, *ImplGetResMgr())).replaceAll("$1", sText);
}



//
// TextUndoRemoveChars
// ===================
//

TextUndoRemoveChars::TextUndoRemoveChars( TextEngine* pTextEngine, const TextPaM& rTextPaM, const XubString& rStr )
                    : TextUndo( pTextEngine ),
                        maTextPaM( rTextPaM ), maText( rStr )
{
}

void TextUndoRemoveChars::Undo()
{
    TextSelection aSel( maTextPaM, maTextPaM );
    GetTextEngine()->ImpInsertText( aSel, maText );
    aSel.GetEnd().GetIndex() = aSel.GetEnd().GetIndex() + maText.Len();
    SetSelection( aSel );
}

void TextUndoRemoveChars::Redo()
{
    TextSelection aSel( maTextPaM, maTextPaM );
    aSel.GetEnd().GetIndex() = aSel.GetEnd().GetIndex() + maText.Len();
    TextPaM aPaM = GetTextEngine()->ImpDeleteText( aSel );
    SetSelection( aPaM );
}

rtl::OUString TextUndoRemoveChars::GetComment () const
{
    // multiple lines?
    rtl::OUString sText(maText);
    Shorten(sText);
    return rtl::OUString(ResId(STR_TEXTUNDO_REMOVECHARS, *ImplGetResMgr())).replaceAll("$1", sText);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
