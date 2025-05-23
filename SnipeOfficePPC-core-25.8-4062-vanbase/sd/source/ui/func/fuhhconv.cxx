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

#include <com/sun/star/i18n/TextConversionOption.hpp>

#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <cppuhelper/bootstrap.hxx>
#include <vcl/msgbox.hxx>
#include <svl/style.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/langitem.hxx>
#include <editeng/fontitem.hxx>

#include <fuhhconv.hxx>
#include "drawdoc.hxx"
#include "Outliner.hxx"
#include "DrawViewShell.hxx"
#include "OutlineViewShell.hxx"
#include "Window.hxx"
#include "ViewShellBase.hxx"

#include "sdresid.hxx"
#include "strings.hrc"

class SfxRequest;


using namespace ::com::sun::star;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;

namespace sd {

class ViewShell;

TYPEINIT1( FuHangulHanjaConversion, FuPoor );

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuHangulHanjaConversion::FuHangulHanjaConversion (
    ViewShell* pViewSh,
    ::sd::Window* pWin,
    ::sd::View* pView,
    SdDrawDocument* pDocument,
    SfxRequest& rReq )
       : FuPoor(pViewSh, pWin, pView, pDocument, rReq),
    pSdOutliner(NULL),
    bOwnOutliner(sal_False)
{
    if ( mpViewShell->ISA(DrawViewShell) )
    {
        bOwnOutliner = sal_True;
        pSdOutliner = new Outliner( mpDoc, OUTLINERMODE_TEXTOBJECT );
    }
    else if ( mpViewShell->ISA(OutlineViewShell) )
    {
        bOwnOutliner = sal_False;
        pSdOutliner = mpDoc->GetOutliner();
    }

    if (pSdOutliner)
       pSdOutliner->PrepareSpelling();
}



/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

FuHangulHanjaConversion::~FuHangulHanjaConversion()
{
    if (pSdOutliner)
        pSdOutliner->EndConversion();

    if (bOwnOutliner)
        delete pSdOutliner;
}

FunctionReference FuHangulHanjaConversion::Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq )
{
    FunctionReference xFunc( new FuHangulHanjaConversion( pViewSh, pWin, pView, pDoc, rReq ) );
    return xFunc;
}

/*************************************************************************
|*
|* Suchen&Ersetzen
|*
\************************************************************************/

void FuHangulHanjaConversion::StartConversion( sal_Int16 nSourceLanguage, sal_Int16 nTargetLanguage,
        const Font *pTargetFont, sal_Int32 nOptions, sal_Bool bIsInteractive )
{

    String aString( SdResId(STR_UNDO_HANGULHANJACONVERSION) );
    mpView->BegUndo( aString );

    ViewShellBase* pBase = PTR_CAST(ViewShellBase, SfxViewShell::Current());
    if (pBase != NULL)
        mpViewShell = pBase->GetMainViewShell().get();

    if( mpViewShell )
    {
        if ( pSdOutliner && mpViewShell->ISA(DrawViewShell) && !bOwnOutliner )
        {
            pSdOutliner->EndConversion();

            bOwnOutliner = sal_True;
            pSdOutliner = new Outliner( mpDoc, OUTLINERMODE_TEXTOBJECT );
            pSdOutliner->BeginConversion();
        }
        else if ( pSdOutliner && mpViewShell->ISA(OutlineViewShell) && bOwnOutliner )
        {
            pSdOutliner->EndConversion();
            delete pSdOutliner;

            bOwnOutliner = sal_False;
            pSdOutliner = mpDoc->GetOutliner();
            pSdOutliner->BeginConversion();
        }

        if (pSdOutliner)
            pSdOutliner->StartConversion(nSourceLanguage, nTargetLanguage, pTargetFont, nOptions, bIsInteractive );
    }

    // Due to changing between edit mode, notes mode, and handout mode the
    // view has most likely changed.  Get the new one.
    mpViewShell = pBase->GetMainViewShell().get();
    if (mpViewShell != NULL)
    {
        mpView = mpViewShell->GetView();
        mpWindow = mpViewShell->GetActiveWindow();
    }
    else
    {
        mpView = 0;
        mpWindow = NULL;
    }

    if (mpView != NULL)
        mpView->EndUndo();
}


void FuHangulHanjaConversion::ConvertStyles( sal_Int16 nTargetLanguage, const Font *pTargetFont )
{
    if( !mpDoc )
        return;

    SfxStyleSheetBasePool* pStyleSheetPool = mpDoc->GetStyleSheetPool();
    if( !pStyleSheetPool )
        return;

    SfxStyleSheetBase* pStyle = pStyleSheetPool->First();
    while( pStyle )
    {
        SfxItemSet& rSet = pStyle->GetItemSet();

        const bool bHasParent = pStyle->GetParent().Len() != 0;

        if( !bHasParent || rSet.GetItemState( EE_CHAR_LANGUAGE_CJK, sal_False ) == SFX_ITEM_SET )
            rSet.Put( SvxLanguageItem( nTargetLanguage, EE_CHAR_LANGUAGE_CJK ) );

        if( pTargetFont &&
            ( !bHasParent || rSet.GetItemState( EE_CHAR_FONTINFO_CJK, sal_False ) == SFX_ITEM_SET ) )
        {
            // set new font attribute
            SvxFontItem aFontItem( (SvxFontItem&) rSet.Get( EE_CHAR_FONTINFO_CJK ) );
            aFontItem.SetFamilyName(   pTargetFont->GetName());
            aFontItem.SetFamily(       pTargetFont->GetFamily());
            aFontItem.SetStyleName(    pTargetFont->GetStyleName());
            aFontItem.SetPitch(        pTargetFont->GetPitch());
            aFontItem.SetCharSet(      pTargetFont->GetCharSet());
            rSet.Put( aFontItem );
        }

        pStyle = pStyleSheetPool->Next();
    }

    mpDoc->SetLanguage( EE_CHAR_LANGUAGE_CJK, nTargetLanguage );
}

void FuHangulHanjaConversion::StartChineseConversion()
{
    //open ChineseTranslationDialog
    Reference< XComponentContext > xContext(
        ::cppu::defaultBootstrap_InitialComponentContext() ); //@todo get context from calc if that has one
    if(xContext.is())
    {
        Reference< lang::XMultiComponentFactory > xMCF( xContext->getServiceManager() );
        if(xMCF.is())
        {
            Reference< ui::dialogs::XExecutableDialog > xDialog(
                    xMCF->createInstanceWithContext("com.sun.star.linguistic2.ChineseTranslationDialog"
                        , xContext), UNO_QUERY);
            Reference< lang::XInitialization > xInit( xDialog, UNO_QUERY );
            if( xInit.is() )
            {
                //  initialize dialog
                Reference< awt::XWindow > xDialogParentWindow(0);
                Sequence<Any> aSeq(1);
                Any* pArray = aSeq.getArray();
                PropertyValue aParam;
                aParam.Name = "ParentWindow";
                aParam.Value <<= makeAny(xDialogParentWindow);
                pArray[0] <<= makeAny(aParam);
                xInit->initialize( aSeq );

                //execute dialog
                sal_Int16 nDialogRet = xDialog->execute();
                if( RET_OK == nDialogRet )
                {
                    //get some parameters from the dialog
                    sal_Bool bToSimplified = sal_True;
                    sal_Bool bUseVariants = sal_True;
                    sal_Bool bCommonTerms = sal_True;
                    Reference< beans::XPropertySet >  xProp( xDialog, UNO_QUERY );
                    if( xProp.is() )
                    {
                        try
                        {
                            xProp->getPropertyValue( "IsDirectionToSimplified" ) >>= bToSimplified;
                            xProp->getPropertyValue( "IsUseCharacterVariants" ) >>= bUseVariants;
                            xProp->getPropertyValue( "IsTranslateCommonTerms" ) >>= bCommonTerms;
                        }
                        catch( Exception& )
                        {
                        }
                    }

                    //execute translation
                    sal_Int16 nSourceLang = bToSimplified ? LANGUAGE_CHINESE_TRADITIONAL : LANGUAGE_CHINESE_SIMPLIFIED;
                    sal_Int16 nTargetLang = bToSimplified ? LANGUAGE_CHINESE_SIMPLIFIED : LANGUAGE_CHINESE_TRADITIONAL;
                    sal_Int32 nOptions    = bUseVariants ? i18n::TextConversionOption::USE_CHARACTER_VARIANTS : 0;
                    if( !bCommonTerms )
                        nOptions = nOptions | i18n::TextConversionOption::CHARACTER_BY_CHARACTER;

                    Font aTargetFont = mpWindow->GetDefaultFont(
                                        DEFAULTFONT_CJK_PRESENTATION,
                                        nTargetLang, DEFAULTFONT_FLAGS_ONLYONE );

                    StartConversion( nSourceLang, nTargetLang, &aTargetFont, nOptions, sal_False );
                    ConvertStyles( nTargetLang, &aTargetFont );
                }
            }
            Reference< lang::XComponent > xComponent( xDialog, UNO_QUERY );
            if( xComponent.is() )
                xComponent->dispose();
        }
    }
}
} // end of namespace

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
