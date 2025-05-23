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

#include <basidesh.hrc>
#include <basidesh.hxx>
#include <basobj.hxx>

#include <basicbox.hxx>
#include <iderid.hxx>
#include <iderdll.hxx>
#include <bastypes.hxx>
#include "bastype2.hxx"
#include "basdoc.hxx"

#include "localizationmgr.hxx"
#include "managelang.hxx"
#include "dlgresid.hrc"
#include <editeng/unolingu.hxx>

#include <sfx2/dinfdlg.hxx>
#include <sfx2/dispatch.hxx>
#include <svtools/langtab.hxx>

namespace basctl
{

using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;

SFX_IMPL_TOOLBOX_CONTROL( LibBoxControl, SfxStringItem );

LibBoxControl::LibBoxControl( sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx )
    : SfxToolBoxControl( nSlotId, nId, rTbx )
{
}



LibBoxControl::~LibBoxControl()
{
}



void LibBoxControl::StateChanged( sal_uInt16, SfxItemState eState, const SfxPoolItem* pState )
{
    LibBox* pBox = (LibBox*)GetToolBox().GetItemWindow(GetId());

    DBG_ASSERT( pBox, "Box not found" );
    if ( !pBox )
        return;

    if ( eState != SFX_ITEM_AVAILABLE )
        pBox->Disable();
    else
    {
        pBox->Enable();
        pBox->Update(dynamic_cast<SfxStringItem const*>(pState));
    }
}



Window* LibBoxControl::CreateItemWindow( Window *pParent )
{
    return new LibBox( pParent, m_xFrame );
}

//=============================================================================
//= DocListenerBox
//=============================================================================

DocListenerBox::DocListenerBox( Window* pParent )
    :ListBox( pParent, WinBits( WB_BORDER | WB_DROPDOWN ) )
    ,m_aNotifier( *this )
{
}

DocListenerBox::~DocListenerBox()
{
    m_aNotifier.dispose();
}

void DocListenerBox::onDocumentCreated( const ScriptDocument& /*_rDocument*/ )
{
    FillBox();
}

void DocListenerBox::onDocumentOpened( const ScriptDocument& /*_rDocument*/ )
{
    FillBox();
}

void DocListenerBox::onDocumentSave( const ScriptDocument& /*_rDocument*/ )
{
    // not interested in
}

void DocListenerBox::onDocumentSaveDone( const ScriptDocument& /*_rDocument*/ )
{
    // not interested in
}

void DocListenerBox::onDocumentSaveAs( const ScriptDocument& /*_rDocument*/ )
{
    // not interested in
}

void DocListenerBox::onDocumentSaveAsDone( const ScriptDocument& /*_rDocument*/ )
{
    FillBox();
}

void DocListenerBox::onDocumentClosed( const ScriptDocument& /*_rDocument*/ )
{
    FillBox();
}

void DocListenerBox::onDocumentTitleChanged( const ScriptDocument& /*_rDocument*/ )
{
    // not interested in
}

void DocListenerBox::onDocumentModeChanged( const ScriptDocument& /*_rDocument*/ )
{
    // not interested in
}

//=============================================================================
//= basctl::LibBox
//=============================================================================
LibBox::LibBox( Window* pParent, const uno::Reference< frame::XFrame >& rFrame ) :
    DocListenerBox( pParent ),
    m_xFrame( rFrame )
{
    FillBox();
    bIgnoreSelect = true;   // do not yet transfer select of 0
    bFillBox = true;
    SelectEntryPos( 0 );
    aCurText = GetEntry( 0 );
    SetSizePixel( Size( 250, 200 ) );
    bIgnoreSelect = false;
}



LibBox::~LibBox()
{
    ClearBox();
}

void LibBox::Update( const SfxStringItem* pItem )
{

//  if ( !pItem  || !pItem->GetValue().Len() )
        FillBox();

    if ( pItem )
    {
        aCurText = pItem->GetValue();
        if ( aCurText.Len() == 0 )
            aCurText = String( IDEResId( RID_STR_ALL ) );
    }

    if ( GetSelectEntry() != aCurText )
        SelectEntry( aCurText );
}

void LibBox::ReleaseFocus()
{
    SfxViewShell* pCurSh = SfxViewShell::Current();
    DBG_ASSERT( pCurSh, "Current ViewShell not found!" );

    if ( pCurSh )
    {
        Window* pShellWin = pCurSh->GetWindow();
        if ( !pShellWin )
            pShellWin = Application::GetDefDialogParent();

        pShellWin->GrabFocus();
    }
}

void LibBox::FillBox()
{
    SetUpdateMode(false);
    bIgnoreSelect = true;

    aCurText = GetSelectEntry();

    SelectEntryPos( 0 );
    ClearBox();

    // create list box entries
    sal_uInt16 nPos = InsertEntry( String( IDEResId( RID_STR_ALL ) ), LISTBOX_APPEND );
    SetEntryData( nPos, new LibEntry( ScriptDocument::getApplicationScriptDocument(), LIBRARY_LOCATION_UNKNOWN, String() ) );
    InsertEntries( ScriptDocument::getApplicationScriptDocument(), LIBRARY_LOCATION_USER );
    InsertEntries( ScriptDocument::getApplicationScriptDocument(), LIBRARY_LOCATION_SHARE );

    ScriptDocuments aDocuments( ScriptDocument::getAllScriptDocuments( ScriptDocument::DocumentsSorted ) );
    for (   ScriptDocuments::const_iterator doc = aDocuments.begin();
            doc != aDocuments.end();
            ++doc
        )
    {
        InsertEntries( *doc, LIBRARY_LOCATION_DOCUMENT );
    }

    SetUpdateMode(true);

    SelectEntry( aCurText );
    if ( !GetSelectEntryCount() )
    {
        SelectEntryPos( GetEntryCount() );
        aCurText = GetSelectEntry();
    }
    bIgnoreSelect = false;
}

void LibBox::InsertEntries( const ScriptDocument& rDocument, LibraryLocation eLocation )
{
    // get a sorted list of library names
    Sequence< OUString > aLibNames = rDocument.getLibraryNames();
    sal_Int32 nLibCount = aLibNames.getLength();
    const OUString* pLibNames = aLibNames.getConstArray();

    for ( sal_Int32 i = 0 ; i < nLibCount ; ++i )
    {
        String aLibName = pLibNames[ i ];
        if ( eLocation == rDocument.getLibraryLocation( aLibName ) )
        {
            String aName( rDocument.getTitle( eLocation ) );
            String aEntryText( CreateMgrAndLibStr( aName, aLibName ) );
            sal_uInt16 nPos = InsertEntry( aEntryText, LISTBOX_APPEND );
            SetEntryData( nPos, new LibEntry( rDocument, eLocation, aLibName ) );
        }
    }
}

long LibBox::PreNotify( NotifyEvent& rNEvt )
{
    long nDone = 0;
    if( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        KeyEvent aKeyEvt = *rNEvt.GetKeyEvent();
        sal_uInt16 nKeyCode = aKeyEvt.GetKeyCode().GetCode();
        switch( nKeyCode )
        {
            case KEY_RETURN:
            {
                NotifyIDE();
                nDone = 1;
            }
            break;

            case KEY_ESCAPE:
            {
                SelectEntry( aCurText );
                ReleaseFocus();
                nDone = 1;
            }
            break;
        }
    }
    else if( rNEvt.GetType() == EVENT_GETFOCUS )
    {
        if ( bFillBox )
        {
            FillBox();
            bFillBox = false;
        }
    }
    else if( rNEvt.GetType() == EVENT_LOSEFOCUS )
    {
        if ( !HasChildPathFocus(true) )
        {
            bIgnoreSelect = true;
            bFillBox = true;
        }
    }

    return nDone ? nDone : ListBox::PreNotify( rNEvt );
}

void LibBox::Select()
{
    if ( !IsTravelSelect() )
    {
        if ( !bIgnoreSelect )
            NotifyIDE();
        else
            SelectEntry( aCurText );    // since 306... (Select after Escape)
    }
}

void LibBox::NotifyIDE()
{
    sal_uInt16 nSelPos = GetSelectEntryPos();
    if (LibEntry* pEntry = static_cast<LibEntry*>(GetEntryData(nSelPos)))
    {
        ScriptDocument aDocument( pEntry->GetDocument() );
        SfxUsrAnyItem aDocumentItem( SID_BASICIDE_ARG_DOCUMENT_MODEL, uno::makeAny( aDocument.getDocumentOrNull() ) );
        String aLibName = pEntry->GetLibName();
        SfxStringItem aLibNameItem( SID_BASICIDE_ARG_LIBNAME, aLibName );
        if (SfxDispatcher* pDispatcher = GetDispatcher())
            pDispatcher->Execute(
                SID_BASICIDE_LIBSELECTED,
                SFX_CALLMODE_SYNCHRON, &aDocumentItem, &aLibNameItem, 0L
            );
    }
    ReleaseFocus();
}

void LibBox::ClearBox()
{
    sal_uInt16 nCount = GetEntryCount();
    for ( sal_uInt16 i = 0; i < nCount; ++i )
    {
        LibEntry* pEntry = static_cast<LibEntry*>(GetEntryData( i ));
        delete pEntry;
    }
    ListBox::Clear();
}

// class LanguageBoxControl ----------------------------------------------

SFX_IMPL_TOOLBOX_CONTROL( LanguageBoxControl, SfxStringItem );

LanguageBoxControl::LanguageBoxControl( sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx )
    : SfxToolBoxControl( nSlotId, nId, rTbx )
{
}

LanguageBoxControl::~LanguageBoxControl()
{
}

void LanguageBoxControl::StateChanged( sal_uInt16 nID, SfxItemState eState, const SfxPoolItem* pItem )
{
    (void)nID;
    if (LanguageBox* pBox = static_cast<LanguageBox*>(GetToolBox().GetItemWindow(GetId())))
    {
        if (eState != SFX_ITEM_AVAILABLE)
            pBox->Disable();
        else
        {
            pBox->Enable();
            pBox->Update(dynamic_cast<SfxStringItem const*>(pItem));
        }
    }
}

Window* LanguageBoxControl::CreateItemWindow( Window *pParent )
{
    return new LanguageBox( pParent );
}

// class basctl::LanguageBox -----------------------------------------------

LanguageBox::LanguageBox( Window* pParent ) :

    DocListenerBox( pParent ),

    m_sNotLocalizedStr( IDEResId( RID_STR_TRANSLATION_NOTLOCALIZED ) ),
    m_sDefaultLanguageStr( IDEResId( RID_STR_TRANSLATION_DEFAULT ) ),

    m_bIgnoreSelect( false )

{
    SetSizePixel( Size( 210, 200 ) );

    FillBox();
}

LanguageBox::~LanguageBox()
{
    ClearBox();
}

void LanguageBox::FillBox()
{
    SetUpdateMode(false);
    m_bIgnoreSelect = true;
    m_sCurrentText = GetSelectEntry();
    ClearBox();

    boost::shared_ptr<LocalizationMgr> pCurMgr(GetShell()->GetCurLocalizationMgr());
    if ( pCurMgr->isLibraryLocalized() )
    {
        Enable();
        SvtLanguageTable aLangTable;
        Locale aDefaultLocale = pCurMgr->getStringResourceManager()->getDefaultLocale();
        Locale aCurrentLocale = pCurMgr->getStringResourceManager()->getCurrentLocale();
        Sequence< Locale > aLocaleSeq = pCurMgr->getStringResourceManager()->getLocales();
        const Locale* pLocale = aLocaleSeq.getConstArray();
        sal_Int32 i, nCount = aLocaleSeq.getLength();
        sal_uInt16 nSelPos = LISTBOX_ENTRY_NOTFOUND;
        for ( i = 0;  i < nCount;  ++i )
        {
            bool bIsDefault = localesAreEqual( aDefaultLocale, pLocale[i] );
            bool bIsCurrent = localesAreEqual( aCurrentLocale, pLocale[i] );
            LanguageType eLangType = LanguageTag( pLocale[i] ).getLanguageType();
            String sLanguage = aLangTable.GetString( eLangType );
            if ( bIsDefault )
            {
                sLanguage += ' ';
                sLanguage += m_sDefaultLanguageStr;
            }
            sal_uInt16 nPos = InsertEntry( sLanguage );
            SetEntryData( nPos, new LanguageEntry( sLanguage, pLocale[i], bIsDefault ) );

            if ( bIsCurrent )
                nSelPos = nPos;
        }

        if ( nSelPos != LISTBOX_ENTRY_NOTFOUND )
        {
            SelectEntryPos( nSelPos );
            m_sCurrentText = GetSelectEntry();
        }
    }
    else
    {
        InsertEntry( m_sNotLocalizedStr );
        SelectEntryPos(0);
        Disable();
    }

    SetUpdateMode(true);
    m_bIgnoreSelect = false;
}

void LanguageBox::ClearBox()
{
    sal_uInt16 nCount = GetEntryCount();
    for ( sal_uInt16 i = 0; i < nCount; ++i )
    {
        LanguageEntry* pEntry = (LanguageEntry*)GetEntryData(i);
        delete pEntry;
    }
    ListBox::Clear();
}

void LanguageBox::SetLanguage()
{
    LanguageEntry* pEntry = (LanguageEntry*)GetEntryData( GetSelectEntryPos() );
    if ( pEntry )
        GetShell()->GetCurLocalizationMgr()->handleSetCurrentLocale( pEntry->m_aLocale );
}

void LanguageBox::Select()
{
    if ( !m_bIgnoreSelect )
        SetLanguage();
    else
        SelectEntry( m_sCurrentText );  // Select after Escape
}

long LanguageBox::PreNotify( NotifyEvent& rNEvt )
{
    long nDone = 0;
    if( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        sal_uInt16 nKeyCode = rNEvt.GetKeyEvent()->GetKeyCode().GetCode();
        switch( nKeyCode )
        {
            case KEY_RETURN:
            {
                SetLanguage();
                nDone = 1;
            }
            break;

            case KEY_ESCAPE:
            {
                SelectEntry( m_sCurrentText );
                nDone = 1;
            }
            break;
        }
    }
    else if( rNEvt.GetType() == EVENT_GETFOCUS )
    {
    }
    else if( rNEvt.GetType() == EVENT_LOSEFOCUS )
    {
    }

    return nDone ? nDone : ListBox::PreNotify( rNEvt );
}

void LanguageBox::Update( const SfxStringItem* pItem )
{
    FillBox();

    if ( pItem && pItem->GetValue().Len() > 0 )
    {
        m_sCurrentText = pItem->GetValue();
        if ( GetSelectEntry() != m_sCurrentText )
            SelectEntry( m_sCurrentText );
    }
}


} // namespace basctl

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
