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

// CLOOKS:
#define _SPIN_HXX

#include "basidesh.hxx"

#include <tools/diagnose_ex.h>
#include <basic/basmgr.hxx>
#include <basidesh.hrc>
#include "baside2.hxx"
#include "baside3.hxx"
#include <basdoc.hxx>
#include <basicbox.hxx>
#include <editeng/sizeitem.hxx>
#include <objdlg.hxx>
#include <tbxctl.hxx>
#include <iderdll2.hxx>
#include <basidectrlr.hxx>
#include <localizationmgr.hxx>
#include <sfx2/app.hxx>
#include <sfx2/dinfdlg.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/minfitem.hxx>
#include <sfx2/objface.hxx>
#include <svl/aeitem.hxx>
#include <svl/intitem.hxx>
#include <svl/srchitem.hxx>

#define basctl_Shell
#define SFX_TYPEMAP
#include <idetemp.hxx>
#include <basslots.hxx>
#include <iderdll.hxx>
#include <svx/pszctrl.hxx>
#include <svx/insctrl.hxx>
#include <svx/srchdlg.hxx>
#include <svx/tbcontrl.hxx>
#include <com/sun/star/script/XLibraryContainerPassword.hpp>
#include <com/sun/star/container/XContainer.hpp>
#include <svx/xmlsecctrl.hxx>
#include <sfx2/viewfac.hxx>
#include <vcl/msgbox.hxx>

namespace basctl
{

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;

typedef ::cppu::WeakImplHelper1< container::XContainerListener > ContainerListenerBASE;

class ContainerListenerImpl : public ContainerListenerBASE
{
    Shell* mpShell;
public:

    ContainerListenerImpl (Shell* pShell) : mpShell(pShell) { }

    ~ContainerListenerImpl()
    { }

    void addContainerListener( const ScriptDocument& rScriptDocument, const OUString& aLibName )
    {
        try
        {
            uno::Reference< container::XContainer > xContainer( rScriptDocument.getLibrary( E_SCRIPTS, aLibName, false ), uno::UNO_QUERY );
            if ( xContainer.is() )
            {
                uno::Reference< container::XContainerListener > xContainerListener( this );
                xContainer->addContainerListener( xContainerListener );
            }
        }
        catch(const uno::Exception& ) {}
    }
    void removeContainerListener( const ScriptDocument& rScriptDocument, const OUString& aLibName )
    {
        try
        {
            uno::Reference< container::XContainer > xContainer( rScriptDocument.getLibrary( E_SCRIPTS, aLibName, false ), uno::UNO_QUERY );
            if ( xContainer.is() )
            {
                uno::Reference< container::XContainerListener > xContainerListener( this );
                xContainer->removeContainerListener( xContainerListener );
            }
        }
        catch(const uno::Exception& ) {}
    }

    // XEventListener
    virtual void SAL_CALL disposing( const lang::EventObject& ) throw( uno::RuntimeException ) {}

    // XContainerListener
    virtual void SAL_CALL elementInserted( const container::ContainerEvent& Event ) throw( uno::RuntimeException )
    {
        OUString sModuleName;
        if( mpShell && ( Event.Accessor >>= sModuleName ) )
            mpShell->FindBasWin( mpShell->m_aCurDocument, mpShell->m_aCurLibName, sModuleName, true, false );
    }
    virtual void SAL_CALL elementReplaced( const container::ContainerEvent& ) throw( com::sun::star::uno::RuntimeException ) { }
    virtual void SAL_CALL elementRemoved( const container::ContainerEvent& Event ) throw( com::sun::star::uno::RuntimeException )
    {
        OUString sModuleName;
        if( mpShell  && ( Event.Accessor >>= sModuleName ) )
        {
            ModulWindow* pWin = mpShell->FindBasWin(mpShell->m_aCurDocument, mpShell->m_aCurLibName, sModuleName, false, true);
            if( pWin )
                mpShell->RemoveWindow( pWin, true, true );
        }
    }

};

TYPEINIT1( Shell, SfxViewShell );

SFX_IMPL_NAMED_VIEWFACTORY( Shell, "Default" )
{
    SFX_VIEW_REGISTRATION( DocShell );
}


SFX_IMPL_INTERFACE( basctl_Shell, SfxViewShell, IDEResId( RID_STR_IDENAME ) )
{
    SFX_CHILDWINDOW_REGISTRATION( SID_SEARCH_DLG );
    SFX_FEATURED_CHILDWINDOW_REGISTRATION(SID_SHOW_PROPERTYBROWSER, BASICIDE_UI_FEATURE_SHOW_BROWSER);
    SFX_POPUPMENU_REGISTRATION( IDEResId( RID_POPUP_DLGED ) );
}



namespace
{

unsigned const ShellFlags = SFX_VIEW_CAN_PRINT | SFX_VIEW_NO_NEWWINDOW;

}


unsigned Shell::nShellCount = 0;

Shell::Shell( SfxViewFrame* pFrame_, SfxViewShell* /* pOldShell */ ) :
    SfxViewShell( pFrame_, ShellFlags ),
    m_aCurDocument( ScriptDocument::getApplicationScriptDocument() ),
    aHScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_HSCROLL | WB_DRAG ) ),
    aVScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_VSCROLL | WB_DRAG ) ),
    aScrollBarBox( &GetViewFrame()->GetWindow(), WinBits( WB_SIZEABLE ) ),
    pLayout(0),
    aObjectCatalog(&GetViewFrame()->GetWindow()),
    m_bAppBasicModified( false ),
    m_aNotifier( *this )
{
    m_xLibListener = new ContainerListenerImpl( this );
    Init();
    nShellCount++;
}

void Shell::Init()
{
    TbxControls::RegisterControl( SID_CHOOSE_CONTROLS );
    SvxPosSizeStatusBarControl::RegisterControl();
    SvxInsertStatusBarControl::RegisterControl();
    XmlSecStatusBarControl::RegisterControl( SID_SIGNATURE );
    SvxSimpleUndoRedoController::RegisterControl( SID_UNDO );
    SvxSimpleUndoRedoController::RegisterControl( SID_REDO );

    SvxSearchDialogWrapper::RegisterChildWindow(false);

    GetExtraData()->ShellInCriticalSection() = true;

    SetName( OUString( "BasicIDE" ) );
    SetHelpId( SVX_INTERFACE_BASIDE_VIEWSH );

    LibBoxControl::RegisterControl( SID_BASICIDE_LIBSELECTOR );
    LanguageBoxControl::RegisterControl( SID_BASICIDE_CURRENT_LANG );

    GetViewFrame()->GetWindow().SetBackground(
        GetViewFrame()->GetWindow().GetSettings().GetStyleSettings().GetWindowColor()
    );

    pCurWin = 0;
    m_aCurDocument = ScriptDocument::getApplicationScriptDocument();
    bCreatingWindow = false;

    pTabBar.reset(new TabBar(&GetViewFrame()->GetWindow()));
    pTabBar->SetSplitHdl( LINK( this, Shell, TabBarSplitHdl ) );
    bTabBarSplitted = false;

    nCurKey = 100;
    InitScrollBars();
    InitTabBar();

    SetCurLib( ScriptDocument::getApplicationScriptDocument(), "Standard", false, false );

    ShellCreated(this);

    GetExtraData()->ShellInCriticalSection() = false;

    // It's enough to create the controller ...
    // It will be public by using magic :-)
    new Controller(this);

    // Force updating the title ! Because it must be set to the controller
    // it has to be called directly after creating those controller.
    SetMDITitle ();

    UpdateWindows();
}

Shell::~Shell()
{
    m_aNotifier.dispose();

    ShellDestroyed(this);

    // so that on a basic saving error, the shell doesn't pop right up again
    GetExtraData()->ShellInCriticalSection() = true;

    SetWindow( 0 );
    SetCurWindow( 0 );

    for (WindowTableIt it = aWindowTable.begin(); it != aWindowTable.end(); ++it)
    {
        // no store; does already happen when the BasicManagers are destroyed
        delete it->second;
    }

    // Destroy all ContainerListeners for Basic Container.
    if (ContainerListenerImpl* pListener = static_cast<ContainerListenerImpl*>(m_xLibListener.get()))
        pListener->removeContainerListener(m_aCurDocument, m_aCurLibName);

    GetExtraData()->ShellInCriticalSection() = false;

    nShellCount--;
}

void Shell::onDocumentCreated( const ScriptDocument& /*_rDocument*/ )
{
    if (pCurWin)
        pCurWin->OnNewDocument();
    UpdateWindows();
}

void Shell::onDocumentOpened( const ScriptDocument& /*_rDocument*/ )
{
    if (pCurWin)
        pCurWin->OnNewDocument();
    UpdateWindows();
}

void Shell::onDocumentSave( const ScriptDocument& /*_rDocument*/ )
{
    StoreAllWindowData();
}

void Shell::onDocumentSaveDone( const ScriptDocument& /*_rDocument*/ )
{
    // #i115671: Update SID_SAVEDOC after saving is completed
    if (SfxBindings* pBindings = GetBindingsPtr())
        pBindings->Invalidate( SID_SAVEDOC );
}

void Shell::onDocumentSaveAs( const ScriptDocument& /*_rDocument*/ )
{
    StoreAllWindowData();
}

void Shell::onDocumentSaveAsDone( const ScriptDocument& /*_rDocument*/ )
{
    // not interested in
}

void Shell::onDocumentClosed( const ScriptDocument& _rDocument )
{
    if ( !_rDocument.isValid() )
        return;

    bool bSetCurWindow = false;
    bool bSetCurLib = ( _rDocument == m_aCurDocument );
    std::vector<BaseWindow*> aDeleteVec;

    // remove all windows which belong to this document
    for (WindowTableIt it = aWindowTable.begin(); it != aWindowTable.end(); ++it)
    {
        BaseWindow* pWin = it->second;
        if ( pWin->IsDocument( _rDocument ) )
        {
            if ( pWin->GetStatus() & (BASWIN_RUNNINGBASIC|BASWIN_INRESCHEDULE) )
            {
                pWin->AddStatus( BASWIN_TOBEKILLED );
                pWin->Hide();
                StarBASIC::Stop();
                // there's no notify
                pWin->BasicStopped();
            }
            else
                aDeleteVec.push_back( pWin );
        }
    }
    // delete windows outside main loop so we don't invalidate the original iterator
    for (std::vector<BaseWindow*>::const_iterator it = aDeleteVec.begin(); it != aDeleteVec.end(); ++it)
    {
        BaseWindow* pWin = *it;
        pWin->StoreData();
        if ( pWin == pCurWin )
            bSetCurWindow = true;
        RemoveWindow( pWin, true, false );
    }

    // remove lib info
    if (ExtraData* pData = GetExtraData())
        pData->GetLibInfos().RemoveInfoFor( _rDocument );

    if ( bSetCurLib )
        SetCurLib( ScriptDocument::getApplicationScriptDocument(), "Standard", true, false );
    else if ( bSetCurWindow )
        SetCurWindow( FindApplicationWindow(), true );
}

void Shell::onDocumentTitleChanged( const ScriptDocument& /*_rDocument*/ )
{
    if (SfxBindings* pBindings = GetBindingsPtr())
        pBindings->Invalidate( SID_BASICIDE_LIBSELECTOR, true, false );
    SetMDITitle();
}

void Shell::onDocumentModeChanged( const ScriptDocument& _rDocument )
{
    for (WindowTableIt it = aWindowTable.begin(); it != aWindowTable.end(); ++it)
    {
        BaseWindow* pWin = it->second;
        if ( pWin->IsDocument( _rDocument ) && _rDocument.isDocument() )
            pWin->SetReadOnly( _rDocument.isReadOnly() );
    }
}

void Shell::StoreAllWindowData( bool bPersistent )
{
    for (WindowTableIt it = aWindowTable.begin(); it != aWindowTable.end(); ++it)
    {
        BaseWindow* pWin = it->second;
        DBG_ASSERT( pWin, "PrepareClose: NULL-Pointer in Table?" );
        if ( !pWin->IsSuspended() )
            pWin->StoreData();
    }

    if ( bPersistent  )
    {
        SFX_APP()->SaveBasicAndDialogContainer();
        SetAppBasicModified(false);

        if (SfxBindings* pBindings = GetBindingsPtr())
        {
            pBindings->Invalidate( SID_SAVEDOC );
            pBindings->Update( SID_SAVEDOC );
        }
    }
}


sal_uInt16 Shell::PrepareClose( sal_Bool bUI, sal_Bool bForBrowsing )
{
    (void)bForBrowsing;

    // reset here because it's modified after printing etc. (DocInfo)
    GetViewFrame()->GetObjectShell()->SetModified(false);

    if ( StarBASIC::IsRunning() )
    {
        if( bUI )
        {
            Window *pParent = &GetViewFrame()->GetWindow();
            InfoBox( pParent, IDE_RESSTR(RID_STR_CANNOTCLOSE)).Execute();
        }
        return false;
    }
    else
    {
        bool bCanClose = true;
        for (WindowTableIt it = aWindowTable.begin(); bCanClose && (it != aWindowTable.end()); ++it)
        {
            BaseWindow* pWin = it->second;
            if ( !pWin->CanClose() )
            {
                if ( !m_aCurLibName.isEmpty() && ( pWin->IsDocument( m_aCurDocument ) || pWin->GetLibName() != m_aCurLibName ) )
                    SetCurLib( ScriptDocument::getApplicationScriptDocument(), OUString(), false );
                SetCurWindow( pWin, true );
                bCanClose = false;
            }
        }

        if ( bCanClose )
            StoreAllWindowData( false );    // don't write on the disk, that will be done later automatically

        return bCanClose;
    }
}

void Shell::InitScrollBars()
{
    aVScrollBar.SetLineSize( 300 );
    aVScrollBar.SetPageSize( 2000 );
    aHScrollBar.SetLineSize( 300 );
    aHScrollBar.SetPageSize( 2000 );
    aHScrollBar.Enable();
    aVScrollBar.Enable();
    aVScrollBar.Show();
    aHScrollBar.Show();
    aScrollBarBox.Show();
}



void Shell::InitTabBar()
{
    pTabBar->Enable();
    pTabBar->Show();
    pTabBar->SetSelectHdl( LINK( this, Shell, TabBarHdl ) );
}


Size Shell::GetOptimalSizePixel() const
{
    return Size( 400, 300 );
}



void Shell::OuterResizePixel( const Point &rPos, const Size &rSize )
{
    AdjustPosSizePixel( rPos, rSize );
}


IMPL_LINK_INLINE_START( Shell, TabBarSplitHdl, TabBar *, pTBar )
{
    (void)pTBar;
    bTabBarSplitted = true;
    ArrangeTabBar();

    return 0;
}
IMPL_LINK_INLINE_END( Shell, TabBarSplitHdl, TabBar *, pTBar )



IMPL_LINK( Shell, TabBarHdl, TabBar *, pCurTabBar )
{
    sal_uInt16 nCurId = pCurTabBar->GetCurPageId();
    BaseWindow* pWin = aWindowTable[ nCurId ];
    DBG_ASSERT( pWin, "Eintrag in TabBar passt zu keinem Fenster!" );
    SetCurWindow( pWin );

    return 0;
}



bool Shell::NextPage( bool bPrev )
{
    bool bRet = false;
    sal_uInt16 nPos = pTabBar->GetPagePos( pTabBar->GetCurPageId() );

    if ( bPrev )
        --nPos;
    else
        ++nPos;

    if ( nPos < pTabBar->GetPageCount() )
    {
        BaseWindow* pWin = aWindowTable[ pTabBar->GetPageId( nPos ) ];
        SetCurWindow( pWin, true );
        bRet = true;
    }

    return bRet;
}



void Shell::ArrangeTabBar()
{
    long nBoxPos = aScrollBarBox.GetPosPixel().X() - 1;
    long nPos = pTabBar->GetSplitSize();
    if ( nPos <= nBoxPos )
    {
        Point aPnt( pTabBar->GetPosPixel() );
        long nH = aHScrollBar.GetSizePixel().Height();
        pTabBar->SetPosSizePixel( aPnt, Size( nPos, nH ) );
        long nScrlStart = aPnt.X() + nPos;
        aHScrollBar.SetPosSizePixel( Point( nScrlStart, aPnt.Y() ), Size( nBoxPos - nScrlStart + 2, nH ) );
        aHScrollBar.Update();
    }
}



::svl::IUndoManager* Shell::GetUndoManager()
{
    ::svl::IUndoManager* pMgr = NULL;
    if( pCurWin )
        pMgr = pCurWin->GetUndoManager();

    return pMgr;
}



void Shell::SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId&,
                                        const SfxHint& rHint, const TypeId& )
{
    if (GetShell())
    {
        if (SfxSimpleHint const* pSimpleHint = dynamic_cast<SfxSimpleHint const*>(&rHint))
        {
            switch (pSimpleHint->GetId())
            {
                case SFX_HINT_DYING:
                {
                    EndListening( rBC, true /* log off all */ );
                    aObjectCatalog.UpdateEntries();
                }
                break;
            }

            if (SbxHint const* pSbxHint = dynamic_cast<SbxHint const*>(&rHint))
            {
                sal_uLong nHintId = pSbxHint->GetId();
                if (    ( nHintId == SBX_HINT_BASICSTART ) ||
                        ( nHintId == SBX_HINT_BASICSTOP ) )
                {
                    if (SfxBindings* pBindings = GetBindingsPtr())
                    {
                        pBindings->Invalidate( SID_BASICRUN );
                        pBindings->Update( SID_BASICRUN );
                        pBindings->Invalidate( SID_BASICCOMPILE );
                        pBindings->Update( SID_BASICCOMPILE );
                        pBindings->Invalidate( SID_BASICSTEPOVER );
                        pBindings->Update( SID_BASICSTEPOVER );
                        pBindings->Invalidate( SID_BASICSTEPINTO );
                        pBindings->Update( SID_BASICSTEPINTO );
                        pBindings->Invalidate( SID_BASICSTEPOUT );
                        pBindings->Update( SID_BASICSTEPOUT );
                        pBindings->Invalidate( SID_BASICSTOP );
                        pBindings->Update( SID_BASICSTOP );
                        pBindings->Invalidate( SID_BASICIDE_TOGGLEBRKPNT );
                        pBindings->Update( SID_BASICIDE_TOGGLEBRKPNT );
                        pBindings->Invalidate( SID_BASICIDE_MANAGEBRKPNTS );
                        pBindings->Update( SID_BASICIDE_MANAGEBRKPNTS );
                        pBindings->Invalidate( SID_BASICIDE_MODULEDLG );
                        pBindings->Update( SID_BASICIDE_MODULEDLG );
                        pBindings->Invalidate( SID_BASICLOAD );
                        pBindings->Update( SID_BASICLOAD );
                    }

                    if ( nHintId == SBX_HINT_BASICSTOP )
                    {
                        // not only at error/break or explicit stoppage,
                        // if the update is turned off due to a programming bug
                        BasicStopped();
                        if (pLayout)
                            pLayout->UpdateDebug(true); // clear...
                        if( m_pCurLocalizationMgr )
                            m_pCurLocalizationMgr->handleBasicStopped();
                    }
                    else if( m_pCurLocalizationMgr )
                    {
                        m_pCurLocalizationMgr->handleBasicStarted();
                    }

                    for (WindowTableIt it = aWindowTable.begin(); it != aWindowTable.end(); ++it)
                    {
                        BaseWindow* pWin = it->second;
                        if ( nHintId == SBX_HINT_BASICSTART )
                            pWin->BasicStarted();
                        else
                            pWin->BasicStopped();
                    }
                }
            }
        }
    }
}



void Shell::CheckWindows()
{
    bool bSetCurWindow = false;
    std::vector<BaseWindow*> aDeleteVec;
    for (WindowTableIt it = aWindowTable.begin(); it != aWindowTable.end(); ++it)
    {
        BaseWindow* pWin = it->second;
        if ( pWin->GetStatus() & BASWIN_TOBEKILLED )
            aDeleteVec.push_back( pWin );
    }
    for ( std::vector<BaseWindow*>::const_iterator it = aDeleteVec.begin(); it != aDeleteVec.end(); ++it )
    {
        BaseWindow* pWin = *it;
        pWin->StoreData();
        if ( pWin == pCurWin )
            bSetCurWindow = true;
        RemoveWindow( pWin, true, false );
    }
    if ( bSetCurWindow )
        SetCurWindow( FindApplicationWindow(), true );
}



void Shell::RemoveWindows( const ScriptDocument& rDocument, const OUString& rLibName, bool bDestroy )
{
    bool bChangeCurWindow = pCurWin ? false : true;
    std::vector<BaseWindow*> aDeleteVec;
    for (WindowTableIt it = aWindowTable.begin(); it != aWindowTable.end(); ++it)
    {
        BaseWindow* pWin = it->second;
        if ( pWin->IsDocument( rDocument ) && pWin->GetLibName() == rLibName )
            aDeleteVec.push_back( pWin );
    }
    for ( std::vector<BaseWindow*>::const_iterator it = aDeleteVec.begin(); it != aDeleteVec.end(); ++it )
    {
        BaseWindow* pWin = *it;
        if ( pWin == pCurWin )
            bChangeCurWindow = true;
        pWin->StoreData();
        RemoveWindow( pWin, bDestroy, false );
    }
    if ( bChangeCurWindow )
        SetCurWindow( FindApplicationWindow(), true );
}



void Shell::UpdateWindows()
{
    // remove all windows that may not be displayed
    bool bChangeCurWindow = pCurWin ? false : true;
    if ( !m_aCurLibName.isEmpty() )
    {
        std::vector<BaseWindow*> aDeleteVec;
        for (WindowTableIt it = aWindowTable.begin(); it != aWindowTable.end(); ++it)
        {
            BaseWindow* pWin = it->second;
            if ( !pWin->IsDocument( m_aCurDocument ) || pWin->GetLibName() != m_aCurLibName )
            {
                if ( pWin == pCurWin )
                    bChangeCurWindow = true;
                pWin->StoreData();
                // The request of RUNNING prevents the crash when in reschedule.
                // Window is frozen at first, later the windows should be changed
                // anyway to be marked as hidden instead of being deleted.
                if ( !(pWin->GetStatus() & ( BASWIN_TOBEKILLED | BASWIN_RUNNINGBASIC | BASWIN_SUSPENDED ) ) )
                    aDeleteVec.push_back( pWin );
            }
        }
        for ( std::vector<BaseWindow*>::const_iterator it = aDeleteVec.begin(); it != aDeleteVec.end(); ++it )
        {
            RemoveWindow( *it, false, false );
        }
    }

    if ( bCreatingWindow )
        return;

    BaseWindow* pNextActiveWindow = 0;

    // show all windows that are to be shown
    ScriptDocuments aDocuments( ScriptDocument::getAllScriptDocuments( ScriptDocument::AllWithApplication ) );
    for (   ScriptDocuments::const_iterator doc = aDocuments.begin();
            doc != aDocuments.end();
            ++doc
        )
    {
        StartListening( *doc->getBasicManager(), true /* log on only once */ );

        // libraries
        Sequence< OUString > aLibNames( doc->getLibraryNames() );
        sal_Int32 nLibCount = aLibNames.getLength();
        const OUString* pLibNames = aLibNames.getConstArray();

        for ( sal_Int32 i = 0 ; i < nLibCount ; i++ )
        {
            OUString aLibName = pLibNames[ i ];

            if ( m_aCurLibName.isEmpty() || ( *doc == m_aCurDocument && aLibName == m_aCurLibName ) )
            {
                // check, if library is password protected and not verified
                bool bProtected = false;
                Reference< script::XLibraryContainer > xModLibContainer( doc->getLibraryContainer( E_SCRIPTS ) );
                if ( xModLibContainer.is() && xModLibContainer->hasByName( aLibName ) )
                {
                    Reference< script::XLibraryContainerPassword > xPasswd( xModLibContainer, UNO_QUERY );
                    if ( xPasswd.is() && xPasswd->isLibraryPasswordProtected( aLibName ) && !xPasswd->isLibraryPasswordVerified( aLibName ) )
                    {
                        bProtected = true;
                    }
                }

                if ( !bProtected )
                {
                    LibInfos::Item const* pLibInfoItem = 0;
                    if (ExtraData* pData = GetExtraData())
                        pLibInfoItem = pData->GetLibInfos().GetInfo(*doc, aLibName);

                    // modules
                    if ( xModLibContainer.is() && xModLibContainer->hasByName( aLibName ) )
                    {
                        StarBASIC* pLib = doc->getBasicManager()->GetLib( aLibName );
                        if ( pLib )
                            ImplStartListening( pLib );

                        try
                        {
                            Sequence< OUString > aModNames( doc->getObjectNames( E_SCRIPTS, aLibName ) );
                            sal_Int32 nModCount = aModNames.getLength();
                            const OUString* pModNames = aModNames.getConstArray();

                            for ( sal_Int32 j = 0 ; j < nModCount ; j++ )
                            {
                                OUString aModName = pModNames[ j ];
                                ModulWindow* pWin = FindBasWin( *doc, aLibName, aModName, false );
                                if ( !pWin )
                                    pWin = CreateBasWin( *doc, aLibName, aModName );
                                if ( !pNextActiveWindow && pLibInfoItem && pLibInfoItem->GetCurrentName() == aModName &&
                                     pLibInfoItem->GetCurrentType() == TYPE_MODULE )
                                {
                                    pNextActiveWindow = (BaseWindow*)pWin;
                                }
                            }
                        }
                        catch (const container::NoSuchElementException& )
                        {
                            DBG_UNHANDLED_EXCEPTION();
                        }
                    }

                    // dialogs
                    Reference< script::XLibraryContainer > xDlgLibContainer( doc->getLibraryContainer( E_DIALOGS ) );
                    if ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aLibName ) )
                    {
                        try
                        {
                            Sequence< OUString > aDlgNames = doc->getObjectNames( E_DIALOGS, aLibName );
                            sal_Int32 nDlgCount = aDlgNames.getLength();
                            const OUString* pDlgNames = aDlgNames.getConstArray();

                            for ( sal_Int32 j = 0 ; j < nDlgCount ; j++ )
                            {
                                OUString aDlgName = pDlgNames[ j ];
                                // this find only looks for non-suspended windows;
                                // suspended windows are handled in CreateDlgWin
                                DialogWindow* pWin = FindDlgWin( *doc, aLibName, aDlgName, false );
                                if ( !pWin )
                                    pWin = CreateDlgWin( *doc, aLibName, aDlgName );
                                if ( !pNextActiveWindow && pLibInfoItem && pLibInfoItem->GetCurrentName() == aDlgName &&
                                     pLibInfoItem->GetCurrentType() == TYPE_DIALOG )
                                {
                                    pNextActiveWindow = (BaseWindow*)pWin;
                                }
                            }
                        }
                        catch (const container::NoSuchElementException& )
                        {
                            DBG_UNHANDLED_EXCEPTION();
                        }
                    }
                }
            }
        }
    }

    if ( bChangeCurWindow )
    {
        if ( !pNextActiveWindow )
        {
            pNextActiveWindow = FindApplicationWindow();
        }
        SetCurWindow( pNextActiveWindow, true );
    }
}

void Shell::RemoveWindow( BaseWindow* pWindow_, bool bDestroy, bool bAllowChangeCurWindow )
{
    DBG_ASSERT( pWindow_, "Kann keinen NULL-Pointer loeschen!" );
    sal_uLong nKey = GetWindowId( pWindow_ );
    pTabBar->RemovePage( (sal_uInt16)nKey );
    aWindowTable.erase( nKey );
    if ( pWindow_ == pCurWin )
    {
        if ( bAllowChangeCurWindow )
        {
            SetCurWindow( FindApplicationWindow(), true );
        }
        else
        {
            SetCurWindow( NULL, false );
        }
    }
    if ( bDestroy )
    {
        if ( !( pWindow_->GetStatus() & BASWIN_INRESCHEDULE ) )
        {
            delete pWindow_;
        }
        else
        {
            pWindow_->AddStatus( BASWIN_TOBEKILLED );
            pWindow_->Hide();
            // In normal mode stop basic in windows to be deleted
            // In VBA stop basic only if the running script is trying to delete
            // its parent module
            bool bStop = true;
            if ( pWindow_->GetDocument().isInVBAMode() )
            {
                SbModule* pMod = StarBASIC::GetActiveModule();
                if ( !pMod || ( pMod && ( !pMod->GetName().equals(pWindow_->GetName()) ) ) )
                {
                    bStop = false;
                }
            }
            if ( bStop )
            {
                StarBASIC::Stop();
                // there will be no notify...
                pWindow_->BasicStopped();
            }
            aWindowTable[ nKey ] = pWindow_;   // jump in again
        }
    }
    else
    {
        pWindow_->AddStatus( BASWIN_SUSPENDED );
        pWindow_->Deactivating();
        aWindowTable[ nKey ] = pWindow_;   // jump in again
    }

}



sal_uInt16 Shell::InsertWindowInTable( BaseWindow* pNewWin )
{
    nCurKey++;
    aWindowTable[ nCurKey ] = pNewWin;
    return nCurKey;
}



void Shell::InvalidateBasicIDESlots()
{
    // only those that have an optic effect...

    if (GetShell())
    {
        if (SfxBindings* pBindings = GetBindingsPtr())
        {
            pBindings->Invalidate( SID_UNDO );
            pBindings->Invalidate( SID_REDO );
            pBindings->Invalidate( SID_SAVEDOC );
            pBindings->Invalidate( SID_SIGNATURE );
            pBindings->Invalidate( SID_BASICIDE_CHOOSEMACRO );
            pBindings->Invalidate( SID_BASICIDE_MODULEDLG );
            pBindings->Invalidate( SID_BASICIDE_OBJCAT );
            pBindings->Invalidate( SID_BASICSTOP );
            pBindings->Invalidate( SID_BASICRUN );
            pBindings->Invalidate( SID_BASICCOMPILE );
            pBindings->Invalidate( SID_BASICLOAD );
            pBindings->Invalidate( SID_BASICSAVEAS );
            pBindings->Invalidate( SID_BASICIDE_MATCHGROUP );
            pBindings->Invalidate( SID_BASICSTEPINTO );
            pBindings->Invalidate( SID_BASICSTEPOVER );
            pBindings->Invalidate( SID_BASICSTEPOUT );
            pBindings->Invalidate( SID_BASICIDE_TOGGLEBRKPNT );
            pBindings->Invalidate( SID_BASICIDE_MANAGEBRKPNTS );
            pBindings->Invalidate( SID_BASICIDE_ADDWATCH );
            pBindings->Invalidate( SID_BASICIDE_REMOVEWATCH );
            pBindings->Invalidate( SID_CHOOSE_CONTROLS );
            pBindings->Invalidate( SID_PRINTDOC );
            pBindings->Invalidate( SID_PRINTDOCDIRECT );
            pBindings->Invalidate( SID_SETUPPRINTER );
            pBindings->Invalidate( SID_DIALOG_TESTMODE );

            pBindings->Invalidate( SID_DOC_MODIFIED );
            pBindings->Invalidate( SID_BASICIDE_STAT_TITLE );
            pBindings->Invalidate( SID_BASICIDE_STAT_POS );
            pBindings->Invalidate( SID_ATTR_INSERT );
            pBindings->Invalidate( SID_ATTR_SIZE );
        }
    }
}

void Shell::EnableScrollbars( bool bEnable )
{
    aHScrollBar.Enable(bEnable);
    aVScrollBar.Enable(bEnable);
}

void Shell::SetCurLib( const ScriptDocument& rDocument, OUString aLibName, bool bUpdateWindows, bool bCheck )
{
    if ( !bCheck || ( rDocument != m_aCurDocument || aLibName != m_aCurLibName ) )
    {
        ContainerListenerImpl* pListener = static_cast< ContainerListenerImpl* >( m_xLibListener.get() );

        m_aCurDocument = rDocument;
        m_aCurLibName = aLibName;

        if ( pListener )
        {
            pListener->removeContainerListener( m_aCurDocument, m_aCurLibName );
            pListener->addContainerListener( m_aCurDocument, aLibName );
        }

        if ( bUpdateWindows )
            UpdateWindows();

        SetMDITitle();

        SetCurLibForLocalization( rDocument, aLibName );

        if (SfxBindings* pBindings = GetBindingsPtr())
        {
            pBindings->Invalidate( SID_BASICIDE_LIBSELECTOR );
            pBindings->Invalidate( SID_BASICIDE_CURRENT_LANG );
            pBindings->Invalidate( SID_BASICIDE_MANAGE_LANG );
        }
    }
}

void Shell::SetCurLibForLocalization( const ScriptDocument& rDocument, OUString aLibName )
{
    // Create LocalizationMgr
    Reference< resource::XStringResourceManager > xStringResourceManager;
    try
    {
        if( !aLibName.isEmpty() )
        {
            Reference< container::XNameContainer > xDialogLib( rDocument.getLibrary( E_DIALOGS, aLibName, true ) );
            xStringResourceManager = LocalizationMgr::getStringResourceFromDialogLibrary( xDialogLib );
        }
    }
    catch (const container::NoSuchElementException& )
    {}

    m_pCurLocalizationMgr = boost::shared_ptr<LocalizationMgr>(new LocalizationMgr(this, rDocument, aLibName, xStringResourceManager));
    m_pCurLocalizationMgr->handleTranslationbar();
}

void Shell::ImplStartListening( StarBASIC* pBasic )
{
    StartListening( pBasic->GetBroadcaster(), true /* log on only once */ );
}

} // namespace basctl

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
