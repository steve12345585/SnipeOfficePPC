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
#ifndef _SVX_SPELLDDIALOG_HXX
#define _SVX_SPELLDDIALOG_HXX

#include <sfx2/basedlgs.hxx>
#include <vcl/fixed.hxx>
#include <vcl/edit.hxx>
#include <vcl/lstbox.hxx>
#include <svtools/stdctrl.hxx>
#include <vcl/button.hxx>
#include <vcl/menubtn.hxx>
#include <vcl/group.hxx>
#include <vcl/decoview.hxx>
#include <vcl/image.hxx>
#include <com/sun/star/uno/Reference.hxx>


#include <svx/langbox.hxx>
#include <memory>
#include <svtools/svmedit.hxx>
#include <svl/lstner.hxx>
#include <vcl/fixedhyper.hxx>
#include <vcl/xtextedt.hxx>
#include <editeng/SpellPortions.hxx>

#include <set>

namespace svx{ class SpellUndoAction_Impl;}

// forward ---------------------------------------------------------------

struct SpellDialog_Impl;

namespace com{namespace sun{namespace star{
namespace linguistic2{
    class XSpellChecker1;
}}}}
namespace svx{
class SpellDialog;
struct SpellErrorDescription;
// ------------------------------------------------------------------
class SentenceEditWindow_Impl : public MultiLineEdit
{
    using MultiLineEdit::SetText;

private:
    std::set< sal_uInt16 >      m_aIgnoreErrorsAt;
    sal_uInt16          m_nErrorStart;
    sal_uInt16          m_nErrorEnd;
    bool            m_bIsUndoEditMode;

    Link            m_aModifyLink;

    void            CallModifyLink() {m_aModifyLink.Call(this);}

    SpellDialog*    GetSpellDialog() const {return (SpellDialog*)GetParent();}
protected:
    virtual long    PreNotify( NotifyEvent& rNEvt );

public:
                    SentenceEditWindow_Impl( SpellDialog* pParent, const ResId& rResId );
                    ~SentenceEditWindow_Impl();

    void            SetModifyHdl(const Link& rLink) { m_aModifyLink = rLink;}

    void            SetAttrib( const TextAttrib& rAttr, sal_uLong nPara, sal_uInt16 nStart, sal_uInt16 nEnd );
    void            SetText( const String& rStr );

    bool            MarkNextError( bool bIgnoreCurrentError );
    void            ChangeMarkedWord(const String& rNewWord, LanguageType eLanguage);
    void            MoveErrorMarkTo(sal_uInt16 nErrorStart, sal_uInt16 nErrorEnd, bool bGrammar);
    String          GetErrorText() const;
    void            RestoreCurrentError();

    void            SetAlternatives(
                        com::sun::star::uno::Reference<com::sun::star::linguistic2::XSpellAlternatives> );

    const SpellErrorDescription* GetAlternatives();


    void            ResetModified()   { GetTextEngine()->SetModified(sal_False); m_bIsUndoEditMode = false;}
    sal_Bool            IsModified() const              { return GetTextEngine()->IsModified(); }

    bool            IsUndoEditMode() const { return m_bIsUndoEditMode;}
    void            SetUndoEditMode(bool bSet);

    svx::SpellPortions  CreateSpellPortions( bool bSetIgnoreFlag ) const;

    void            ResetUndo();
    void            Undo();
    void            AddUndoAction( SfxUndoAction *pAction, sal_Bool bTryMerg=sal_False );
    sal_uInt16      GetUndoActionCount();
    void            UndoActionStart( sal_uInt16 nId );
    void            UndoActionEnd();

    void            MoveErrorEnd(long nOffset);

    void            ResetIgnoreErrorsAt()   { m_aIgnoreErrorsAt.clear(); }
};

class HelpFixedText : public FixedText
{
    public:
        HelpFixedText( Window* pParent, const ResId& rResId );

        virtual void Paint( const Rectangle& rRect );
        long GetActualHeight( );
};

// class SvxSpellDialog ---------------------------------------------
class SpellDialogChildWindow;

class SpellDialog : public SfxModelessDialog
{
    using Window::Invalidate;

    friend class SentenceEditWindow_Impl;
private:

    FixedText       aLanguageFT;
    SvxLanguageBox  aLanguageLB;

    HelpFixedText   aExplainFT;
    FixedHyperlink  aExplainLink;

    FixedText       aNotInDictFT;
    SentenceEditWindow_Impl  aSentenceED;

    FixedText       aSuggestionFT;
    ListBox         aSuggestionLB;

    PushButton      aIgnorePB;
    PushButton      aIgnoreAllPB;
    PushButton      aIgnoreRulePB;
    MenuButton      aAddToDictMB;
    PushButton      aAddToDictPB;

    PushButton      aChangePB;
    PushButton      aChangeAllPB;
    PushButton      aAutoCorrPB;

    CheckBox        aCheckGrammarCB;

    HelpButton      aHelpPB;
    PushButton      aOptionsPB;
    PushButton      aUndoPB;
    PushButton      aClosePB;

    GroupBox        aBackgroundGB;

    Image           aVendorImage;

    String          aResumeST;
    String          aIgnoreOnceST;
    String          aNoSuggestionsST;

    const String    m_sTitleSpelling;
    const String    m_sTitleSpellingGrammar;
    const String    m_sTitleSpellingGrammarVendor;

    Size            aOldWordEDSize;
    Link            aDialogUndoLink;

    bool            bModified;
    bool            bFocusLocked;

    svx::SpellDialogChildWindow& rParent;
    svx::SpellPortions           m_aSavedSentence;

    SpellDialog_Impl* pImpl;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XSpellChecker1 >     xSpell;

    DECL_LINK(ChangeHdl, void *);
    DECL_LINK(ChangeAllHdl, void *);
    DECL_LINK( IgnoreAllHdl, Button * );
    DECL_LINK(IgnoreHdl, void *);
    DECL_LINK( CheckGrammarHdl, CheckBox* );
    DECL_LINK( ExtClickHdl, Button * );
    DECL_LINK(CancelHdl, void *);
    DECL_LINK( ModifyHdl, SentenceEditWindow_Impl *);
    DECL_LINK(UndoHdl, void *);
    DECL_LINK( AddToDictSelectHdl, MenuButton* );
    DECL_LINK( AddToDictClickHdl, PushButton* );
    DECL_LINK( LanguageSelectHdl, SvxLanguageBox* );
    DECL_LINK( DialogUndoHdl, SpellUndoAction_Impl* );
    DECL_LINK( HandleHyperlink, FixedHyperlink * );

    DECL_STATIC_LINK( SpellDialog, InitHdl, SpellDialog * );

    int             AddToDictionaryExecute( sal_uInt16 ItemId, PopupMenu *pMenu );
    void            StartSpellOptDlg_Impl();
    int             InitUserDicts();
    void            UpdateBoxes_Impl();
    void            Init_Impl();
    void            SpellContinue_Impl(bool UseSavedSentence = false, bool bIgnoreCurrentError = false );
    void            LockFocusChanges( bool bLock ) {bFocusLocked = bLock;}
    void            Impl_Restore();

    void            SetSelectedLang_Impl( LanguageType nLang );
    LanguageType    GetSelectedLang_Impl() const;

    /** Retrieves the next sentence.
     */
    bool            GetNextSentence_Impl(bool bUseSavedSentence, bool bRechek /*for rechecking the current sentence*/);
    /** Corrects all errors that have been selected to be changed always
     */
    bool            ApplyChangeAllList_Impl(SpellPortions& rSentence, bool& bHasReplaced);
    void            SetTitle_Impl(LanguageType nLang);

protected:
    virtual void    Paint( const Rectangle& rRect );
    virtual long    Notify( NotifyEvent& rNEvt );

    String getReplacementString() const;

public:
    SpellDialog(
        svx::SpellDialogChildWindow* pChildWindow,
        Window * pParent,
        SfxBindings* pBindings);
    ~SpellDialog();

    void            SetLanguage( sal_uInt16 nLang );
    virtual sal_Bool    Close();

    void            InvalidateDialog();
};
} //namespace svx

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
