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

#include "baside2.hrc"
#include "basidesh.hrc"
#include "helpid.hrc"

#include "baside2.hxx" // unfortunately pModulWindow is needed partly...
#include "baside3.hxx"
#include "basobj.hxx"
#include "iderdll.hxx"

#include <basic/basmgr.hxx>
#include <com/sun/star/script/ModuleType.hpp>
#include <com/sun/star/script/XLibraryContainerPassword.hpp>
#include <sfx2/dispatch.hxx>
#include <sfx2/passwd.hxx>
#include <sfx2/viewfrm.hxx>
#include <svl/intitem.hxx>
#include <svl/stritem.hxx>
#include <vcl/msgbox.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;

using ::std::vector;

DBG_NAME( IDEBaseWindow )

TYPEINIT0( IDEBaseWindow )
TYPEINIT1( SbxItem, SfxPoolItem );

IDEBaseWindow::IDEBaseWindow( Window* pParent, const ScriptDocument& rDocument, ::rtl::OUString aLibName, ::rtl::OUString aName )
    :Window( pParent, WinBits( WB_3DLOOK ) )
    ,m_aDocument( rDocument )
    ,m_aLibName( aLibName )
    ,m_aName( aName )
{
    DBG_CTOR( IDEBaseWindow, 0 );
    pShellHScrollBar = 0;
    pShellVScrollBar = 0;
    nStatus = 0;
}

IDEBaseWindow::~IDEBaseWindow()
{
    DBG_DTOR( IDEBaseWindow, 0 );
    if ( pShellVScrollBar )
        pShellVScrollBar->SetScrollHdl( Link() );
    if ( pShellHScrollBar )
        pShellHScrollBar->SetScrollHdl( Link() );
}



void IDEBaseWindow::Init()
{
    DBG_CHKTHIS( IDEBaseWindow, 0 );
    if ( pShellVScrollBar )
        pShellVScrollBar->SetScrollHdl( LINK( this, IDEBaseWindow, ScrollHdl ) );
    if ( pShellHScrollBar )
        pShellHScrollBar->SetScrollHdl( LINK( this, IDEBaseWindow, ScrollHdl ) );
    DoInit();   // virtual...
}



void IDEBaseWindow::DoInit()
{
}



void IDEBaseWindow::GrabScrollBars( ScrollBar* pHScroll, ScrollBar* pVScroll )
{
    DBG_CHKTHIS( IDEBaseWindow, 0 );
    pShellHScrollBar = pHScroll;
    pShellVScrollBar = pVScroll;
//  Init(); // does not make sense, leads to flickering and errors...
}



IMPL_LINK_INLINE_START( IDEBaseWindow, ScrollHdl, ScrollBar *, pCurScrollBar )
{
    DBG_CHKTHIS( IDEBaseWindow, 0 );
    DoScroll( pCurScrollBar );
    return 0;
}
IMPL_LINK_INLINE_END( IDEBaseWindow, ScrollHdl, ScrollBar *, pCurScrollBar )



void IDEBaseWindow::ExecuteCommand( SfxRequest& )
{
    DBG_CHKTHIS( IDEBaseWindow, 0 );
}



void IDEBaseWindow::GetState( SfxItemSet& )
{
    DBG_CHKTHIS( IDEBaseWindow, 0 );
}


long IDEBaseWindow::Notify( NotifyEvent& rNEvt )
{
    long nDone = 0;

    if ( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        KeyEvent aKEvt = *rNEvt.GetKeyEvent();
        KeyCode aCode = aKEvt.GetKeyCode();
        sal_uInt16 nCode = aCode.GetCode();

        switch ( nCode )
        {
            case KEY_PAGEUP:
            case KEY_PAGEDOWN:
            {
                if ( aCode.IsMod1() )
                {
                    BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
                    if ( pIDEShell )
                        pIDEShell->NextPage( nCode == KEY_PAGEUP );

                    nDone = 1;
                }
            }
            break;
        }
    }

    return nDone ? nDone : Window::Notify( rNEvt );
}


void IDEBaseWindow::DoScroll( ScrollBar* )
{
    DBG_CHKTHIS( IDEBaseWindow, 0 );
}


void IDEBaseWindow::StoreData()
{
}

sal_Bool IDEBaseWindow::CanClose()
{
    return sal_True;
}

sal_Bool IDEBaseWindow::AllowUndo()
{
    return sal_True;
}



void IDEBaseWindow::UpdateData()
{
}

::rtl::OUString IDEBaseWindow::GetTitle()
{
    return ::rtl::OUString();
}

::rtl::OUString IDEBaseWindow::CreateQualifiedName()
{
    ::rtl::OUStringBuffer aName;
    if ( !m_aLibName.isEmpty() )
    {
        LibraryLocation eLocation = m_aDocument.getLibraryLocation( m_aLibName );
        aName.append(m_aDocument.getTitle(eLocation));
        aName.append('.');
        aName.append(m_aLibName);
        aName.append('.');
        aName.append(GetTitle());
    }
    return aName.makeStringAndClear();
}

void IDEBaseWindow::SetReadOnly( sal_Bool )
{
}

sal_Bool IDEBaseWindow::IsReadOnly()
{
    return sal_False;
}

void IDEBaseWindow::BasicStarted()
{
}

void IDEBaseWindow::BasicStopped()
{
}

sal_Bool IDEBaseWindow::IsModified()
{
    return sal_True;
}

sal_Bool IDEBaseWindow::IsPasteAllowed()
{
    return sal_False;
}

Window* IDEBaseWindow::GetLayoutWindow()
{
    return this;
}

::svl::IUndoManager* IDEBaseWindow::GetUndoManager()
{
    return NULL;
}

void IDEBaseWindow::Deactivating()
{
}

sal_uInt16 IDEBaseWindow::GetSearchOptions()
{
    return 0;
}


BasicDockingWindow::BasicDockingWindow( Window* pParent ) :
    DockingWindow( pParent, WB_BORDER | WB_3DLOOK | WB_DOCKABLE | WB_MOVEABLE |
                            WB_SIZEABLE | WB_ROLLABLE |
                            WB_DOCKABLE | WB_CLIPCHILDREN )
{
}

BasicDockingWindow::BasicDockingWindow( Window* pParent, const ResId& rResId ) :
    DockingWindow( pParent, rResId )
{
    SetStyle( WB_BORDER | WB_3DLOOK | WB_MOVEABLE |
                            WB_SIZEABLE | WB_ROLLABLE |
                            WB_DOCKABLE | WB_CLIPCHILDREN );
}

sal_Bool BasicDockingWindow::Docking( const Point& rPos, Rectangle& rRect )
{
    ModulWindowLayout* pLayout = (ModulWindowLayout*)GetParent();
    Rectangle aTmpRec( rRect );
    sal_Bool bDock = IsDockingPrevented() ? sal_False : pLayout->IsToBeDocked( this, rPos, aTmpRec );
    if ( bDock )
    {
        rRect.SetSize( aTmpRec.GetSize() );
    }
    else    // adjust old size
    {
        if ( !aFloatingPosAndSize.IsEmpty() )
            rRect.SetSize( aFloatingPosAndSize.GetSize() );
    }
    return !bDock;  // bFloat
}



void BasicDockingWindow::EndDocking( const Rectangle& rRect, sal_Bool bFloatMode )
{
    if ( bFloatMode )
        DockingWindow::EndDocking( rRect, bFloatMode );
    else
    {
        SetFloatingMode( sal_False );
        ModulWindowLayout* pLayout = (ModulWindowLayout*)GetParent();
        pLayout->DockaWindow( this );
    }
}



void BasicDockingWindow::ToggleFloatingMode()
{
    ModulWindowLayout* pLayout = (ModulWindowLayout*)GetParent();
    if ( IsFloatingMode() )
    {
        if ( !aFloatingPosAndSize.IsEmpty() )
            SetPosSizePixel( GetParent()->ScreenToOutputPixel( aFloatingPosAndSize.TopLeft() ),
                aFloatingPosAndSize.GetSize() );
    }
    pLayout->DockaWindow( this );
}



sal_Bool BasicDockingWindow::PrepareToggleFloatingMode()
{
    if ( IsFloatingMode() )
    {
        // memorize position and size on the desktop...
        aFloatingPosAndSize.SetPos( GetParent()->OutputToScreenPixel( GetPosPixel() ) );
        aFloatingPosAndSize.SetSize( GetSizePixel() );
    }
    return sal_True;
}



void BasicDockingWindow::StartDocking()
{
    if ( IsFloatingMode() )
    {
        aFloatingPosAndSize.SetPos( GetParent()->OutputToScreenPixel( GetPosPixel() ) );
        aFloatingPosAndSize.SetSize( GetSizePixel() );
    }
}



ExtendedEdit::ExtendedEdit( Window* pParent, IDEResId nRes ) :
    Edit( pParent, nRes )
{
    aAcc.SetSelectHdl( LINK( this, ExtendedEdit, EditAccHdl ) );
    Control::SetGetFocusHdl( LINK( this, ExtendedEdit, ImplGetFocusHdl ) );
    Control::SetLoseFocusHdl( LINK( this, ExtendedEdit, ImplLoseFocusHdl ) );
}

IMPL_LINK_NOARG(ExtendedEdit, ImplGetFocusHdl)
{
    Application::InsertAccel( &aAcc );
    aLoseFocusHdl.Call( this );
    return 0;
}


IMPL_LINK_NOARG(ExtendedEdit, ImplLoseFocusHdl)
{
    Application::RemoveAccel( &aAcc );
    return 0;
}


IMPL_LINK_INLINE_START( ExtendedEdit, EditAccHdl, Accelerator *, pAcc )
{
    aAccHdl.Call( pAcc );
    return 0;
}
IMPL_LINK_INLINE_END( ExtendedEdit, EditAccHdl, Accelerator *, pAcc )



struct TabBarDDInfo
{
    sal_uLong   npTabBar;
    sal_uInt16  nPage;

    TabBarDDInfo() { npTabBar = 0; nPage = 0; }
    TabBarDDInfo( sal_uLong _npTabBar, sal_uInt16 _nPage ) { npTabBar = _npTabBar; nPage = _nPage; }
};


BasicIDETabBar::BasicIDETabBar( Window* pParent ) :
    TabBar( pParent, WinBits( WB_3DLOOK | WB_SCROLL | WB_BORDER | WB_SIZEABLE | WB_DRAG ) )
{
    EnableEditMode( sal_True );

    SetHelpId( HID_BASICIDE_TABBAR );
}

void BasicIDETabBar::MouseButtonDown( const MouseEvent& rMEvt )
{
    if ( rMEvt.IsLeft() && ( rMEvt.GetClicks() == 2 ) && !IsInEditMode() )
    {
        BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
        SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
        SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        if( pDispatcher )
        {
            pDispatcher->Execute( SID_BASICIDE_MODULEDLG );
        }
    }
    else
    {
        TabBar::MouseButtonDown( rMEvt );
    }
}

void BasicIDETabBar::Command( const CommandEvent& rCEvt )
{
    if ( ( rCEvt.GetCommand() == COMMAND_CONTEXTMENU ) && !IsInEditMode() )
    {
        Point aPos( rCEvt.IsMouseEvent() ? rCEvt.GetMousePosPixel() : Point(1,1) );
        if ( rCEvt.IsMouseEvent() )     // select right tab
        {
            Point aP = PixelToLogic( aPos );
            MouseEvent aMouseEvent( aP, 1, MOUSE_SIMPLECLICK, MOUSE_LEFT );
            TabBar::MouseButtonDown( aMouseEvent );
        }

        PopupMenu aPopup( IDEResId( RID_POPUP_TABBAR ) );
        if ( GetPageCount() == 0 )
        {
            aPopup.EnableItem( SID_BASICIDE_DELETECURRENT, sal_False );
            aPopup.EnableItem( SID_BASICIDE_RENAMECURRENT, sal_False );
            aPopup.EnableItem( SID_BASICIDE_HIDECURPAGE, sal_False );
        }

        if ( StarBASIC::IsRunning() )
        {
            aPopup.EnableItem(SID_BASICIDE_DELETECURRENT, false);
            aPopup.EnableItem( SID_BASICIDE_RENAMECURRENT, false);
            aPopup.EnableItem(SID_BASICIDE_MODULEDLG, false);
        }

        BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
        if ( pIDEShell )
        {
            ScriptDocument aDocument( pIDEShell->GetCurDocument() );
            ::rtl::OUString aOULibName( pIDEShell->GetCurLibName() );
            Reference< script::XLibraryContainer2 > xModLibContainer( aDocument.getLibraryContainer( E_SCRIPTS ), UNO_QUERY );
            Reference< script::XLibraryContainer2 > xDlgLibContainer( aDocument.getLibraryContainer( E_DIALOGS ), UNO_QUERY );
            if ( ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) && xModLibContainer->isLibraryReadOnly( aOULibName ) ) ||
                 ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aOULibName ) && xDlgLibContainer->isLibraryReadOnly( aOULibName ) ) )
            {
                aPopup.EnableItem( aPopup.GetItemId( 0 ), sal_False );
                aPopup.EnableItem( SID_BASICIDE_DELETECURRENT, sal_False );
                aPopup.EnableItem( SID_BASICIDE_RENAMECURRENT, sal_False );
                aPopup.RemoveDisabledEntries();
            }
             if ( aDocument.isInVBAMode() )
            {
                // disable to delete or remove object modules in IDE
                BasicManager* pBasMgr = aDocument.getBasicManager();
                if ( pBasMgr )
                {
                    StarBASIC* pBasic = pBasMgr->GetLib( aOULibName );
                    if( pBasic )
                    {
                        IDEWindowTable& aIDEWindowTable = pIDEShell->GetIDEWindowTable();
                        IDEWindowTable::const_iterator it = aIDEWindowTable.find( GetCurPageId() );
                        if( it != aIDEWindowTable.end() && it->second->ISA( ModulWindow ) )
                        {
                            SbModule* pActiveModule = (SbModule*)pBasic->FindModule( it->second->GetName() );
                            if( pActiveModule && ( pActiveModule->GetModuleType() == script::ModuleType::DOCUMENT ) )
                            {
                                aPopup.EnableItem( SID_BASICIDE_DELETECURRENT, sal_False );
                                aPopup.EnableItem( SID_BASICIDE_RENAMECURRENT, sal_False );
                            }
                        }
                    }
                }
            }
        }


        SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
        SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        if ( pDispatcher )
            pDispatcher->Execute( aPopup.Execute( this, aPos ) );
    }
}

long BasicIDETabBar::AllowRenaming()
{
    sal_Bool bValid = BasicIDE::IsValidSbxName( GetEditText() );

    if ( !bValid )
        ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_BADSBXNAME ) ) ).Execute();

    return bValid ? TABBAR_RENAMING_YES : TABBAR_RENAMING_NO;
}


void BasicIDETabBar::EndRenaming()
{
    if ( !IsEditModeCanceled() )
    {
        SfxUInt16Item aID( SID_BASICIDE_ARG_TABID, GetEditPageId() );
        SfxStringItem aNewName( SID_BASICIDE_ARG_MODULENAME, GetEditText() );
        BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
        SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
        SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        if( pDispatcher )
        {
            pDispatcher->Execute( SID_BASICIDE_NAMECHANGEDONTAB,
                                  SFX_CALLMODE_SYNCHRON, &aID, &aNewName, 0L );
        }
    }
}


void BasicIDETabBar::Sort()
{
    BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
    if ( pIDEShell )
    {
        IDEWindowTable& aIDEWindowTable = pIDEShell->GetIDEWindowTable();
        TabBarSortHelper aTabBarSortHelper;
        ::std::vector<TabBarSortHelper> aModuleList;
        ::std::vector<TabBarSortHelper> aDialogList;
        sal_uInt16 nPageCount = GetPageCount();
        sal_uInt16 i;

        // create module and dialog lists for sorting
        for ( i = 0; i < nPageCount; i++)
        {
            sal_uInt16 nId = GetPageId( i );
            aTabBarSortHelper.nPageId = nId;
            aTabBarSortHelper.aPageText = GetPageText( nId );
            IDEBaseWindow* pWin = aIDEWindowTable[ nId ];

            if ( pWin->IsA( TYPE( ModulWindow ) ) )
            {
                aModuleList.push_back( aTabBarSortHelper );
            }
            else if ( pWin->IsA( TYPE( DialogWindow ) ) )
            {
                aDialogList.push_back( aTabBarSortHelper );
            }
        }

        // sort module and dialog lists by page text
        ::std::sort( aModuleList.begin() , aModuleList.end() );
        ::std::sort( aDialogList.begin() , aDialogList.end() );


        sal_uInt16 nModules = sal::static_int_cast<sal_uInt16>( aModuleList.size() );
        sal_uInt16 nDialogs = sal::static_int_cast<sal_uInt16>( aDialogList.size() );

        // move module pages to new positions
        for (i = 0; i < nModules; i++)
        {
            MovePage( aModuleList[i].nPageId , i );
        }

        // move dialog pages to new positions
        for (i = 0; i < nDialogs; i++)
        {
            MovePage( aDialogList[i].nPageId , nModules + i );
        }
    }
}

void CutLines( ::rtl::OUString& rStr, sal_Int32 nStartLine, sal_Int32 nLines, sal_Bool bEraseTrailingEmptyLines )
{
    sal_Int32 nStartPos = 0;
    sal_Int32 nLine = 0;
    while ( nLine < nStartLine )
    {
        nStartPos = searchEOL( rStr, nStartPos );
        if( nStartPos == -1 )
            break;
        nStartPos++;    // not the \n.
        nLine++;
    }

    DBG_ASSERTWARNING( nStartPos != -1, "CutLines: Startzeile nicht gefunden!" );

    if ( nStartPos != -1 )
    {
        sal_Int32 nEndPos = nStartPos;

        for ( sal_Int32 i = 0; i < nLines; i++ )
            nEndPos = searchEOL( rStr, nEndPos+1 );

        if ( nEndPos == -1 ) // might happen at the last line
            nEndPos = rStr.getLength();
        else
            nEndPos++;

        ::rtl::OUString aEndStr = rStr.copy( nEndPos );
        rStr = rStr.copy( 0, nStartPos );
        rStr += aEndStr;
    }
    if ( bEraseTrailingEmptyLines )
    {
        sal_Int32 n = nStartPos;
        sal_Int32 nLen = rStr.getLength();
        while ( ( n < nLen ) && ( rStr.getStr()[ n ] == LINE_SEP ||
                                  rStr.getStr()[ n ] == LINE_SEP_CR ) )
        {
            n++;
        }

        if ( n > nStartPos )
        {
            ::rtl::OUString aEndStr = rStr.copy( n );
            rStr = rStr.copy( 0, nStartPos );
            rStr += aEndStr;
        }
    }
}

sal_uLong CalcLineCount( SvStream& rStream )
{
    sal_uLong nLFs = 0;
    sal_uLong nCRs = 0;
    char c;

    rStream.Seek( 0 );
    rStream >> c;
    while ( !rStream.IsEof() )
    {
        if ( c == '\n' )
            nLFs++;
        else if ( c == '\r' )
            nCRs++;
        rStream >> c;
    }

    rStream.Seek( 0 );
    if ( nLFs > nCRs )
        return nLFs;
    return nCRs;
}

LibInfoKey::LibInfoKey( const ScriptDocument& rDocument, const ::rtl::OUString& rLibName )
    :m_aDocument( rDocument )
    ,m_aLibName( rLibName )
{
}

LibInfoKey::~LibInfoKey()
{
}

LibInfoKey::LibInfoKey( const LibInfoKey& rKey )
    :m_aDocument( rKey.m_aDocument )
    ,m_aLibName( rKey.m_aLibName )
{
}

LibInfoKey& LibInfoKey::operator=( const LibInfoKey& rKey )
{
    m_aDocument = rKey.m_aDocument;
    m_aLibName = rKey.m_aLibName;
    return *this;
}

bool LibInfoKey::operator==( const LibInfoKey& rKey ) const
{
    bool bRet = false;
    if ( m_aDocument == rKey.m_aDocument && m_aLibName == rKey.m_aLibName )
        bRet = true;
    return bRet;
}

LibInfoItem::LibInfoItem( const ScriptDocument& rDocument, const ::rtl::OUString& rLibName, const ::rtl::OUString& rCurrentName, sal_uInt16 nCurrentType )
    :m_aDocument( rDocument )
    ,m_aLibName( rLibName )
    ,m_aCurrentName( rCurrentName )
    ,m_nCurrentType( nCurrentType )
{
}

LibInfoItem::~LibInfoItem()
{
}

LibInfoItem::LibInfoItem( const LibInfoItem& rItem )
    :m_aDocument( rItem.m_aDocument )
    ,m_aLibName( rItem.m_aLibName )
    ,m_aCurrentName( rItem.m_aCurrentName )
    ,m_nCurrentType( rItem.m_nCurrentType )
{
}

LibInfoItem& LibInfoItem::operator=( const LibInfoItem& rItem )
{
    m_aDocument = rItem.m_aDocument;
    m_aLibName = rItem.m_aLibName;
    m_aCurrentName = rItem.m_aCurrentName;
    m_nCurrentType = rItem.m_nCurrentType;

    return *this;
}

LibInfos::LibInfos()
{
}

LibInfos::~LibInfos()
{
    LibInfoMap::iterator end = m_aLibInfoMap.end();
    for ( LibInfoMap::iterator it = m_aLibInfoMap.begin(); it != end; ++it )
        delete it->second;
    m_aLibInfoMap.clear();
}

void LibInfos::InsertInfo( LibInfoItem* pItem )
{
    LibInfoKey aKey( pItem->GetDocument(), pItem->GetLibName() );
    LibInfoMap::iterator it = m_aLibInfoMap.find( aKey );
    if ( it != m_aLibInfoMap.end() )
    {
        LibInfoItem* pI = it->second;
        m_aLibInfoMap.erase( it );
        delete pI;
    }
    m_aLibInfoMap.insert( LibInfoMap::value_type( aKey, pItem ) );
}

void LibInfos::RemoveInfoFor( const ScriptDocument& _rDocument )
{
    for (   LibInfoMap::iterator it = m_aLibInfoMap.begin();
            it != m_aLibInfoMap.end();
        )
    {
        if ( it->first.GetDocument() != _rDocument )
        {
            ++it;
            continue;
        }

        LibInfoItem* pItem = it->second;

        LibInfoMap::iterator next_it = it; ++next_it;
        m_aLibInfoMap.erase( it );
        it = next_it;

        delete pItem;
    }
}

LibInfoItem* LibInfos::GetInfo( const LibInfoKey& rKey )
{
    LibInfoItem* pItem = 0;
    LibInfoMap::iterator it = m_aLibInfoMap.find( rKey );
    if ( it != m_aLibInfoMap.end() )
        pItem = it->second;
    return pItem;
}

bool QueryDel( const ::rtl::OUString& rName, const ResId& rId, Window* pParent )
{
    ::rtl::OUString aQuery(rId.toString());
    ::rtl::OUStringBuffer aNameBuf( rName );
    aNameBuf.append('\'');
    aNameBuf.insert(sal_Int32(0), sal_Unicode('\''));
    aQuery = aQuery.replaceAll("XX", aNameBuf.makeStringAndClear());
    QueryBox aQueryBox( pParent, WB_YES_NO | WB_DEF_YES, aQuery );
    return ( aQueryBox.Execute() == RET_YES );
}

bool QueryDelMacro( const ::rtl::OUString& rName, Window* pParent )
{
    return QueryDel( rName, IDEResId( RID_STR_QUERYDELMACRO ), pParent );
}

bool QueryReplaceMacro( const ::rtl::OUString& rName, Window* pParent )
{
    return QueryDel( rName, IDEResId( RID_STR_QUERYREPLACEMACRO ), pParent );
}

bool QueryDelDialog( const ::rtl::OUString& rName, Window* pParent )
{
    return QueryDel( rName, IDEResId( RID_STR_QUERYDELDIALOG ), pParent );
}

bool QueryDelLib( const ::rtl::OUString& rName, bool bRef, Window* pParent )
{
    return QueryDel( rName, IDEResId( bRef ? RID_STR_QUERYDELLIBREF : RID_STR_QUERYDELLIB ), pParent );
}

bool QueryDelModule( const ::rtl::OUString& rName, Window* pParent )
{
    return QueryDel( rName, IDEResId( RID_STR_QUERYDELMODULE ), pParent );
}

bool QueryPassword( const Reference< script::XLibraryContainer >& xLibContainer, const ::rtl::OUString& rLibName, ::rtl::OUString& rPassword, bool bRepeat, bool bNewTitle )
{
    bool bOK = false;
    sal_uInt16 nRet = 0;

    do
    {
        // password dialog
        SfxPasswordDialog* pDlg = new SfxPasswordDialog( Application::GetDefDialogParent() );
        pDlg->SetMinLen( 1 );

        // set new title
        if ( bNewTitle )
        {
            ::rtl::OUString aTitle(IDE_RESSTR(RID_STR_ENTERPASSWORD));
            aTitle = aTitle.replaceAll("XX", rLibName);
            pDlg->SetText( aTitle );
        }

        // execute dialog
        nRet = pDlg->Execute();

        // verify password
        if ( nRet == RET_OK )
        {
            if ( xLibContainer.is() && xLibContainer->hasByName( rLibName ) )
            {
                Reference< script::XLibraryContainerPassword > xPasswd( xLibContainer, UNO_QUERY );
                if ( xPasswd.is() && xPasswd->isLibraryPasswordProtected( rLibName ) && !xPasswd->isLibraryPasswordVerified( rLibName ) )
                {
                    rPassword = pDlg->GetPassword();
                    //                    ::rtl::OUString aOUPassword( rPassword );
                    bOK = xPasswd->verifyLibraryPassword( rLibName, rPassword );

                    if ( !bOK )
                    {
                        ErrorBox aErrorBox( Application::GetDefDialogParent(), WB_OK, IDE_RESSTR(RID_STR_WRONGPASSWORD) );
                        aErrorBox.Execute();
                    }
                }
            }
        }

        delete pDlg;
    }
    while ( bRepeat && !bOK && nRet == RET_OK );

    return bOK;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
