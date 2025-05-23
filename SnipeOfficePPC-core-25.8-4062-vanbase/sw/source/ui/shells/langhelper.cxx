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


#include <string.h>

#include <vcl/window.hxx>

#include <wrtsh.hxx>
#include <doc.hxx>
#include <docary.hxx>
#include <charfmt.hxx>

#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/request.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/editeng.hxx>
#include <editeng/editdata.hxx>
#include <editeng/outliner.hxx>
#include <editeng/editview.hxx>
#include <editeng/scripttypeitem.hxx>
#include <editeng/langitem.hxx>

#include <svl/languageoptions.hxx>
#include <svtools/langtab.hxx>
#include <svl/slstitm.hxx>
#include <svl/stritem.hxx>

#include <ndtxt.hxx>
#include <pam.hxx>
#include <view.hxx>
#include <viewopt.hxx>

#include "swabstdlg.hxx"

#include <vcl/msgbox.hxx>

#include <langhelper.hxx>

using namespace ::com::sun::star;

namespace SwLangHelper
{

    sal_uInt16 GetLanguageStatus( OutlinerView* pOLV, SfxItemSet& rSet )
    {
        ESelection aSelection = pOLV->GetSelection();
        EditView& rEditView=pOLV->GetEditView();
        EditEngine* pEditEngine=rEditView.GetEditEngine();

        // the value of used script types
        const sal_uInt16 nScriptType =pOLV->GetSelectedScriptType();
        String aScriptTypesInUse( String::CreateFromInt32( nScriptType ) );//pEditEngine->GetScriptType(aSelection)

        SvtLanguageTable aLangTable;

        // get keyboard language
        String aKeyboardLang;
        LanguageType nLang = LANGUAGE_DONTKNOW;

        Window* pWin = rEditView.GetWindow();
        if(pWin)
            nLang = pWin->GetInputLanguage();
        if (nLang != LANGUAGE_DONTKNOW && nLang != LANGUAGE_SYSTEM)
            aKeyboardLang = aLangTable.GetString( nLang );

        // get the language that is in use
        String aCurrentLang = rtl::OUString("*");
        SfxItemSet aSet(pOLV->GetAttribs());
        nLang = SwLangHelper::GetCurrentLanguage( aSet,nScriptType );
        if (nLang != LANGUAGE_DONTKNOW)
            aCurrentLang = aLangTable.GetString( nLang );

        // build sequence for status value
        uno::Sequence< ::rtl::OUString > aSeq( 4 );
        aSeq[0] = aCurrentLang;
        aSeq[1] = aScriptTypesInUse;
        aSeq[2] = aKeyboardLang;
        aSeq[3] = SwLangHelper::GetTextForLanguageGuessing( pEditEngine, aSelection );

        // set sequence as status value
        SfxStringListItem aItem( SID_LANGUAGE_STATUS );
        aItem.SetStringList( aSeq );
        rSet.Put( aItem, SID_LANGUAGE_STATUS );
        return 0;
    }

    bool SetLanguageStatus( OutlinerView* pOLV, SfxRequest &rReq, SwView &rView, SwWrtShell &rSh )
    {
        bool bRestoreSelection = false;
        SfxItemSet aEditAttr(pOLV->GetAttribs());
        ESelection   aSelection  = pOLV->GetSelection();
        EditView   & rEditView   = pOLV->GetEditView();
        EditEngine * pEditEngine = rEditView.GetEditEngine();

        // get the language
        String aNewLangTxt;

        SFX_REQUEST_ARG( rReq, pItem, SfxStringItem, SID_LANGUAGE_STATUS , sal_False );
        if (pItem)
            aNewLangTxt = pItem->GetValue();

        //!! Remember the view frame right now...
        //!! (call to GetView().GetViewFrame() will break if the
        //!! SwTextShell got destroyed meanwhile.)
        SfxViewFrame *pViewFrame = rView.GetViewFrame();

        if (aNewLangTxt.EqualsAscii( "*" ))
        {
            // open the dialog "Tools/Options/Language Settings - Language"
            SfxAbstractDialogFactory* pFact = SfxAbstractDialogFactory::Create();
            if (pFact)
            {
                VclAbstractDialog* pDlg = pFact->CreateVclDialog( rView.GetWindow(), SID_LANGUAGE_OPTIONS );
                pDlg->Execute();
                delete pDlg;
            }
        }
        else
        {
            // setting the new language...
            if (aNewLangTxt.Len() > 0)
            {
                const rtl::OUString aSelectionLangPrefix("Current_");
                const rtl::OUString aParagraphLangPrefix("Paragraph_");
                const rtl::OUString aDocumentLangPrefix("Default_");
                const String aStrNone( rtl::OUString("LANGUAGE_NONE") );
                const String aStrResetLangs( rtl::OUString("RESET_LANGUAGES") );

                xub_StrLen nPos = 0;
                bool bForSelection = true;
                bool bForParagraph = false;
                if (STRING_NOTFOUND != (nPos = aNewLangTxt.Search( aSelectionLangPrefix, 0 )))
                {
                    // ... for the current selection
                    aNewLangTxt = aNewLangTxt.Erase( nPos, aSelectionLangPrefix.getLength() );
                    bForSelection = true;
                }
                else if (STRING_NOTFOUND != (nPos = aNewLangTxt.Search( aParagraphLangPrefix , 0 )))
                {
                    // ... for the current paragraph language
                    aNewLangTxt = aNewLangTxt.Erase( nPos, aParagraphLangPrefix.getLength() );
                    bForSelection = true;
                    bForParagraph = true;
                }
                else if (STRING_NOTFOUND != (nPos = aNewLangTxt.Search( aDocumentLangPrefix , 0 )))
                {
                    // ... as default document language
                    aNewLangTxt = aNewLangTxt.Erase( nPos, aDocumentLangPrefix.getLength() );
                    bForSelection = false;
                }

                if (bForParagraph)
                {
                    bRestoreSelection = true;
                    SwLangHelper::SelectPara( rEditView, aSelection );
                    aSelection = pOLV->GetSelection();
                }
                if (!bForSelection) // document language to be changed...
                {
                    rSh.StartAction();
                    rSh.LockView( sal_True );
                    rSh.Push();

                    // prepare to apply new language to all text in document
                    rSh.SelAll();
                    rSh.ExtendedSelectAll();
                }

                if (aNewLangTxt == aStrNone)
                    SwLangHelper::SetLanguage_None( rSh, pOLV, aSelection, bForSelection, aEditAttr );
                else if (aNewLangTxt == aStrResetLangs)
                    SwLangHelper::ResetLanguages( rSh, pOLV, aSelection, bForSelection );
                else
                    SwLangHelper::SetLanguage( rSh, pOLV, aSelection, aNewLangTxt, bForSelection, aEditAttr );

                // ugly hack, as it seems that EditView/EditEngine does not update their spellchecking marks
                // when setting a new language attribute
                if (bForSelection)
                {
                    const SwViewOption* pVOpt = rView.GetWrtShellPtr()->GetViewOptions();
                    sal_uLong nCntrl = pEditEngine->GetControlWord();
                    // turn off
                    nCntrl &= ~EE_CNTRL_ONLINESPELLING;
                    pEditEngine->SetControlWord(nCntrl);

                    //turn back on
                    if (pVOpt->IsOnlineSpell())
                        nCntrl |= EE_CNTRL_ONLINESPELLING;
                    else
                        nCntrl &= ~EE_CNTRL_ONLINESPELLING;
                    pEditEngine->SetControlWord(nCntrl);

                    pEditEngine->CompleteOnlineSpelling();
                    rEditView.Invalidate();
                }

                if (!bForSelection)
                {
                    // need to release view and restore selection...
                    rSh.Pop( sal_False );
                    rSh.LockView( sal_False );
                    rSh.EndAction();
                }
            }
        }

        // invalidate slot to get the new language displayed
        pViewFrame->GetBindings().Invalidate( rReq.GetSlot() );

        rReq.Done();
        return bRestoreSelection;
    }


    void SetLanguage( SwWrtShell &rWrtSh, const String &rLangText, bool bIsForSelection, SfxItemSet &rCoreSet )
    {
        SetLanguage( rWrtSh, 0 , ESelection(), rLangText, bIsForSelection, rCoreSet );
    }

    void SetLanguage( SwWrtShell &rWrtSh, OutlinerView* pOLV, ESelection aSelection, const String &rLangText, bool bIsForSelection, SfxItemSet &rCoreSet )
    {
        const LanguageType nLang = SvtLanguageTable().GetType( rLangText );
        if (nLang != LANGUAGE_DONTKNOW)
        {
            sal_uInt16 nScriptType = SvtLanguageOptions::GetScriptTypeOfLanguage( nLang );

            EditEngine* pEditEngine = pOLV ? pOLV->GetEditView().GetEditEngine() : NULL;
            OSL_ENSURE( !pOLV || pEditEngine, "OutlinerView without EditEngine???" );

            //get ScriptType
            sal_uInt16 nLangWhichId = 0;
            bool bIsSingleScriptType = true;
            switch (nScriptType)
            {
                case SCRIPTTYPE_LATIN :    nLangWhichId = pEditEngine ? EE_CHAR_LANGUAGE : RES_CHRATR_LANGUAGE; break;
                case SCRIPTTYPE_ASIAN :    nLangWhichId = pEditEngine ? EE_CHAR_LANGUAGE_CJK : RES_CHRATR_CJK_LANGUAGE; break;
                case SCRIPTTYPE_COMPLEX :  nLangWhichId = pEditEngine ? EE_CHAR_LANGUAGE_CTL : RES_CHRATR_CTL_LANGUAGE; break;
                default:
                    bIsSingleScriptType = false;
                    OSL_FAIL("unexpected case" );
            }
            if (bIsSingleScriptType)
            {
                // change language for selection or paragraph
                // (for paragraph is handled by previosuly having set the selection to the
                // whole paragraph)
                if (bIsForSelection)
                {
                    // apply language to current selection
                    if (pEditEngine)
                    {
                        rCoreSet.Put( SvxLanguageItem( nLang, nLangWhichId ));
                        pEditEngine->QuickSetAttribs( rCoreSet, aSelection);
                    }
                    else
                    {
                        rWrtSh.GetCurAttr( rCoreSet );
                        rCoreSet.Put( SvxLanguageItem( nLang, nLangWhichId ));
                        rWrtSh.SetAttr( rCoreSet );
                    }
                }
                else // change language for all text
                {
                    // set document default language
                    switch (nLangWhichId)
                    {
                         case EE_CHAR_LANGUAGE :      nLangWhichId = RES_CHRATR_LANGUAGE; break;
                         case EE_CHAR_LANGUAGE_CJK :  nLangWhichId = RES_CHRATR_CJK_LANGUAGE; break;
                         case EE_CHAR_LANGUAGE_CTL :  nLangWhichId = RES_CHRATR_CTL_LANGUAGE; break;
                    }
                    //Set the default document language
                    rWrtSh.SetDefault( SvxLanguageItem( nLang, nLangWhichId ) );

                    //Resolves: fdo#35282 Clear the language from all Text Styles, and
                    //fallback to default document language
                    const SwTxtFmtColls *pColls = rWrtSh.GetDoc()->GetTxtFmtColls();
                    for(sal_uInt16 i = 0, nCount = pColls->size(); i < nCount; ++i)
                    {
                        SwTxtFmtColl &rTxtColl = *(*pColls)[ i ];
                        rTxtColl.ResetFmtAttr(nLangWhichId);
                    }
                    //Resolves: fdo#35282 Clear the language from all Character Styles,
                    //and fallback to default document language
                    const SwCharFmts *pCharFmts = rWrtSh.GetDoc()->GetCharFmts();
                    for(sal_uInt16 i = 0, nCount = pCharFmts->size(); i < nCount; ++i)
                    {
                        SwCharFmt &rCharFmt = *(*pCharFmts)[ i ];
                        rCharFmt.ResetFmtAttr(nLangWhichId);
                    }

                    // #i102191: hard set respective language attribute in text document
                    // (for all text in the document - which should be selected by now...)
                    rWrtSh.SetAttr( SvxLanguageItem( nLang, nLangWhichId ) );
                }
            }
        }
    }

    void SetLanguage_None( SwWrtShell &rWrtSh, bool bIsForSelection, SfxItemSet &rCoreSet )
    {
        SetLanguage_None( rWrtSh,0,ESelection(),bIsForSelection,rCoreSet );
    }

    void SetLanguage_None( SwWrtShell &rWrtSh, OutlinerView* pOLV, ESelection aSelection, bool bIsForSelection, SfxItemSet &rCoreSet )
    {
        // EditEngine IDs
        const sal_uInt16 aLangWhichId_EE[3] =
        {
            EE_CHAR_LANGUAGE,
            EE_CHAR_LANGUAGE_CJK,
            EE_CHAR_LANGUAGE_CTL
        };

        // Writewr IDs
        const sal_uInt16 aLangWhichId_Writer[3] =
        {
            RES_CHRATR_LANGUAGE,
            RES_CHRATR_CJK_LANGUAGE,
            RES_CHRATR_CTL_LANGUAGE
        };

        if (bIsForSelection)
        {
            // change language for selection or paragraph
            // (for paragraph is handled by previosuly having set the selection to the
            // whole paragraph)

            EditEngine* pEditEngine = pOLV ? pOLV->GetEditView().GetEditEngine() : NULL;
            OSL_ENSURE( !pOLV || pEditEngine, "OutlinerView without EditEngine???" );
            if (pEditEngine)
            {
                for (sal_uInt16 i = 0; i < 3; ++i)
                    rCoreSet.Put( SvxLanguageItem( LANGUAGE_NONE, aLangWhichId_EE[i] ));
                pEditEngine->QuickSetAttribs( rCoreSet, aSelection);
            }
            else
            {
                rWrtSh.GetCurAttr( rCoreSet );
                for (sal_uInt16 i = 0; i < 3; ++i)
                    rCoreSet.Put( SvxLanguageItem( LANGUAGE_NONE, aLangWhichId_Writer[i] ));
                rWrtSh.SetAttr( rCoreSet );
            }
        }
        else // change language for all text
        {
            std::set<sal_uInt16> aAttribs;
            for (sal_uInt16 i = 0; i < 3; ++i)
            {
                rWrtSh.SetDefault( SvxLanguageItem( LANGUAGE_NONE, aLangWhichId_Writer[i] ) );
                aAttribs.insert( aLangWhichId_Writer[i] );
            }

            // set all language attributes to default
            // (for all text in the document - which should be selected by now...)
            rWrtSh.ResetAttr( aAttribs );
        }
    }

    void ResetLanguages( SwWrtShell &rWrtSh, bool bIsForSelection )
    {
        ResetLanguages( rWrtSh, 0 , ESelection(), bIsForSelection );
    }

    void ResetLanguages( SwWrtShell &rWrtSh, OutlinerView* pOLV, ESelection aSelection, bool bIsForSelection )
    {
        (void) bIsForSelection;
        (void) aSelection;

        // reset language for current selection.
        // The selection should already have been expanded to the whole paragraph or
        // to all text in the document if those are the ranges where to reset
        // the language attributes

        if (pOLV)
        {
            EditView &rEditView = pOLV->GetEditView();
            rEditView.RemoveAttribs( true, EE_CHAR_LANGUAGE );
            rEditView.RemoveAttribs( true, EE_CHAR_LANGUAGE_CJK );
            rEditView.RemoveAttribs( true, EE_CHAR_LANGUAGE_CTL );
        }
        else
        {
            std::set<sal_uInt16> aAttribs;
            aAttribs.insert( RES_CHRATR_LANGUAGE );
            aAttribs.insert( RES_CHRATR_CJK_LANGUAGE );
            aAttribs.insert( RES_CHRATR_CTL_LANGUAGE );
            rWrtSh.ResetAttr( aAttribs );
        }
    }


    /// @returns : the language for the selected text that is set for the
    ///     specified attribute (script type).
    ///     If there are more than one languages used LANGUAGE_DONTKNOW will be returned.
    /// @param nLangWhichId : one of
    ///     RES_CHRATR_LANGUAGE, RES_CHRATR_CJK_LANGUAGE, RES_CHRATR_CTL_LANGUAGE,
    LanguageType GetLanguage( SwWrtShell &rSh, sal_uInt16 nLangWhichId )
    {
        SfxItemSet aSet( rSh.GetAttrPool(), nLangWhichId, nLangWhichId );
        rSh.GetCurAttr( aSet );

        return GetLanguage(aSet,nLangWhichId);
    }

    LanguageType GetLanguage( SfxItemSet aSet, sal_uInt16 nLangWhichId )
    {

        LanguageType nLang = LANGUAGE_SYSTEM;

        const SfxPoolItem *pItem = 0;
        SfxItemState nState = aSet.GetItemState( nLangWhichId, sal_True, &pItem );
        if (nState > SFX_ITEM_DEFAULT && pItem)
        {
            // the item is set and can be used
            nLang = (dynamic_cast< const SvxLanguageItem* >(pItem))->GetLanguage();
        }
        else if (nState == SFX_ITEM_DEFAULT)
        {
            // since the attribute is not set: retrieve the default value
            nLang = (dynamic_cast< const SvxLanguageItem& >(aSet.GetPool()->GetDefaultItem( nLangWhichId ))).GetLanguage();
        }
        else if (nState == SFX_ITEM_DONTCARE)
        {
            // there is more than one language...
            nLang = LANGUAGE_DONTKNOW;
        }
        OSL_ENSURE( nLang != LANGUAGE_SYSTEM, "failed to get the language?" );

        return nLang;
    }

    /// @returns: the language in use for the selected text.
    ///     'In use' means the language(s) matching the script type(s) of the
    ///     selected text. Or in other words, the language a spell checker would use.
    ///     If there is more than one language LANGUAGE_DONTKNOW will be returned.
    LanguageType GetCurrentLanguage( SwWrtShell &rSh )
    {
        // get all script types used in current selection
        const sal_uInt16 nScriptType = rSh.GetScriptType();

        //set language attribute to use according to the script type
        sal_uInt16 nLangWhichId = 0;
        bool bIsSingleScriptType = true;
        switch (nScriptType)
        {
             case SCRIPTTYPE_LATIN :    nLangWhichId = RES_CHRATR_LANGUAGE; break;
             case SCRIPTTYPE_ASIAN :    nLangWhichId = RES_CHRATR_CJK_LANGUAGE; break;
             case SCRIPTTYPE_COMPLEX :  nLangWhichId = RES_CHRATR_CTL_LANGUAGE; break;
             default: bIsSingleScriptType = false; break;
        }

        // get language according to the script type(s) in use
        LanguageType nCurrentLang = LANGUAGE_SYSTEM;
        if (bIsSingleScriptType)
            nCurrentLang = GetLanguage( rSh, nLangWhichId );
        else
        {
            // check if all script types are set to LANGUAGE_NONE and return
            // that if this is the case. Otherwise, having multiple script types
            // in use always means there are several languages in use...
            const sal_uInt16 aScriptTypes[3] =
            {
                RES_CHRATR_LANGUAGE,
                RES_CHRATR_CJK_LANGUAGE,
                RES_CHRATR_CTL_LANGUAGE
            };
            nCurrentLang = LANGUAGE_NONE;
            for (sal_uInt16 i = 0; i < 3; ++i)
            {
                LanguageType nTmpLang = GetLanguage( rSh, aScriptTypes[i] );
                if (nTmpLang != LANGUAGE_NONE)
                {
                    nCurrentLang = LANGUAGE_DONTKNOW;
                    break;
                }
            }
        }
        OSL_ENSURE( nCurrentLang != LANGUAGE_SYSTEM, "failed to get the language?" );

        return nCurrentLang;
    }

    /// @returns: the language in use for the selected text.
    ///     'In use' means the language(s) matching the script type(s) of the
    ///     selected text. Or in other words, the language a spell checker would use.
    ///     If there is more than one language LANGUAGE_DONTKNOW will be returned.
    LanguageType GetCurrentLanguage( SfxItemSet aSet, sal_uInt16 nScriptType )
    {
        //set language attribute to use according to the script type
        sal_uInt16 nLangWhichId = 0;
        bool bIsSingleScriptType = true;
        switch (nScriptType)
        {
             case SCRIPTTYPE_LATIN :    nLangWhichId = EE_CHAR_LANGUAGE; break;
             case SCRIPTTYPE_ASIAN :    nLangWhichId = EE_CHAR_LANGUAGE_CJK; break;
             case SCRIPTTYPE_COMPLEX :  nLangWhichId = EE_CHAR_LANGUAGE_CTL; break;
             default: bIsSingleScriptType = false;
        }

        // get language according to the script type(s) in use
        LanguageType nCurrentLang = LANGUAGE_SYSTEM;
        if (bIsSingleScriptType)
            nCurrentLang = GetLanguage( aSet, nLangWhichId );
        else
        {
            // check if all script types are set to LANGUAGE_NONE and return
            // that if this is the case. Otherwise, having multiple script types
            // in use always means there are several languages in use...
            const sal_uInt16 aScriptTypes[3] =
            {
                EE_CHAR_LANGUAGE,
                EE_CHAR_LANGUAGE_CJK,
                EE_CHAR_LANGUAGE_CTL
            };
            nCurrentLang = LANGUAGE_NONE;
            for (sal_uInt16 i = 0; i < 3; ++i)
            {
                LanguageType nTmpLang = GetLanguage( aSet, aScriptTypes[i] );
                if (nTmpLang != LANGUAGE_NONE)
                {
                    nCurrentLang = LANGUAGE_DONTKNOW;
                    break;
                }
            }
        }
        OSL_ENSURE( nCurrentLang != LANGUAGE_SYSTEM, "failed to get the language?" );

        return nCurrentLang;
    }

    String GetTextForLanguageGuessing( SwWrtShell &rSh )
    {
        // string for guessing language
        String aText;
        SwPaM *pCrsr = rSh.GetCrsr();
        SwTxtNode *pNode = pCrsr->GetNode()->GetTxtNode();
        if (pNode)
        {
            aText = pNode->GetTxt();
            if (aText.Len() > 0)
            {
                xub_StrLen nStt = 0;
                xub_StrLen nEnd = pCrsr->GetPoint()->nContent.GetIndex();
                // at most 100 chars to the left...
                nStt = nEnd > 100 ? nEnd - 100 : 0;
                // ... and 100 to the right of the cursor position
                nEnd = aText.Len() - nEnd > 100 ? nEnd + 100 : aText.Len();
                aText = aText.Copy( nStt, nEnd - nStt );
            }
        }
        return aText;
    }

    String GetTextForLanguageGuessing( EditEngine* rEditEngine, ESelection aDocSelection )
    {
        // string for guessing language
        String aText;

        aText = rEditEngine->GetText(aDocSelection);
        if (aText.Len() > 0)
        {
            xub_StrLen nStt = 0;
            xub_StrLen nEnd = aDocSelection.nEndPos;
            // at most 100 chars to the left...
            nStt = nEnd > 100 ? nEnd - 100 : 0;
            // ... and 100 to the right of the cursor position
            nEnd = aText.Len() - nEnd > 100 ? nEnd + 100 : aText.Len();
            aText = aText.Copy( nStt, nEnd - nStt );
        }

        return aText;
    }


    void SelectPara( EditView &rEditView, const ESelection &rCurSel )
    {
        ESelection aParaSel( rCurSel.nStartPara, 0, rCurSel.nStartPara, EE_TEXTPOS_ALL );
        rEditView.SetSelection( aParaSel );
    }

    void SelectCurrentPara( SwWrtShell &rWrtSh )
    {
        // select current para
        if (!rWrtSh.IsSttPara())
            rWrtSh.MovePara( fnParaCurr, fnParaStart );
        if (!rWrtSh.HasMark())
            rWrtSh.SetMark();
        rWrtSh.SwapPam();
        if (!rWrtSh.IsEndPara())
            rWrtSh.MovePara( fnParaCurr, fnParaEnd );
    #if OSL_DEBUG_LEVEL > 1
        String aSelTxt;
        rWrtSh.GetSelectedText( aSelTxt );
        (void) aSelTxt;
    #endif
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
