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

#include <svx/svxdlg.hxx>
#include <tools/shl.hxx>
#include <vcl/msgbox.hxx>
#include <sfx2/filedlghelper.hxx>
#include <sfx2/app.hxx>
#include <svl/aeitem.hxx>
#include <svtools/svtabbx.hxx>
#include "svtools/treelistentry.hxx"
#include <tools/urlobj.hxx>
#include <vcl/svapp.hxx>
#include <unotools/defaultoptions.hxx>
#include <unotools/localfilehelper.hxx>
#include <unotools/pathoptions.hxx>
#include <unotools/moduleoptions.hxx>
#include <unotools/viewoptions.hxx>

#define _SVX_OPTPATH_CXX

#include "optpath.hxx"
#include <dialmgr.hxx>
#include "optpath.hrc"
#include <cuires.hrc>
#include "helpid.hrc"
#include <comphelper/configuration.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/string.hxx>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#include <com/sun/star/ui/dialogs/XAsynchronousExecutableDialog.hpp>
#include <com/sun/star/ui/dialogs/FolderPicker.hpp>
#include <officecfg/Office/Common.hxx>
#include "optHeaderTabListbox.hxx"
#include <readonlyimage.hxx>
#include <vcl/help.hxx>

using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::ui::dialogs;
using namespace ::com::sun::star::uno;
using namespace svx;

// define ----------------------------------------------------------------

#define TAB_WIDTH1      80
#define TAB_WIDTH_MIN   10
#define TAB_WIDTH2      1000
#define ITEMID_TYPE       1
#define ITEMID_PATH       2

#define POSTFIX_INTERNAL    rtl::OUString("_internal")
#define POSTFIX_USER        rtl::OUString("_user")
#define POSTFIX_WRITABLE    rtl::OUString("_writable")
#define VAR_ONE             rtl::OUString("%1")
#define IODLG_CONFIGNAME    rtl::OUString("FilePicker_Save")

// struct OptPath_Impl ---------------------------------------------------

struct OptPath_Impl
{
    SvtDefaultOptions           m_aDefOpt;
    Image                       m_aLockImage;
    String                      m_sMultiPathDlg;
    Reference< XPropertySet >   m_xPathSettings;

    OptPath_Impl(const ResId& rLockRes)
        : m_aLockImage(rLockRes)
    {
    }
};

// struct PathUserData_Impl ----------------------------------------------

struct PathUserData_Impl
{
    sal_uInt16          nRealId;
    SfxItemState    eState;
    String          sUserPath;
    String          sWritablePath;

    PathUserData_Impl( sal_uInt16 nId ) :
        nRealId( nId ), eState( SFX_ITEM_UNKNOWN ) {}
};

struct Handle2CfgNameMapping_Impl
{
    sal_uInt16      m_nHandle;
    const char* m_pCfgName;
};

static Handle2CfgNameMapping_Impl const Hdl2CfgMap_Impl[] =
{
    { SvtPathOptions::PATH_AUTOCORRECT, "AutoCorrect" },
    { SvtPathOptions::PATH_AUTOTEXT,    "AutoText" },
    { SvtPathOptions::PATH_BACKUP,      "Backup" },
    { SvtPathOptions::PATH_GALLERY,     "Gallery" },
    { SvtPathOptions::PATH_GRAPHIC,     "Graphic" },
    { SvtPathOptions::PATH_TEMP,        "Temp" },
    { SvtPathOptions::PATH_TEMPLATE,    "Template" },
    { SvtPathOptions::PATH_WORK,        "Work" },
#if OSL_DEBUG_LEVEL > 1
    { SvtPathOptions::PATH_LINGUISTIC,        "Linguistic" },
    { SvtPathOptions::PATH_DICTIONARY,        "Dictionary" },
#endif
    { USHRT_MAX, NULL }
};

static String getCfgName_Impl( sal_uInt16 _nHandle )
{
    String sCfgName;
    sal_uInt16 nIndex = 0;
    while ( Hdl2CfgMap_Impl[ nIndex ].m_nHandle != USHRT_MAX )
    {
        if ( Hdl2CfgMap_Impl[ nIndex ].m_nHandle == _nHandle )
        {
            // config name found
            sCfgName = rtl::OUString::createFromAscii( Hdl2CfgMap_Impl[ nIndex ].m_pCfgName );
            break;
        }
        ++nIndex;
    }

    return sCfgName;
}

#define MULTIPATH_DELIMITER     ';'

String Convert_Impl( const String& rValue )
{
    char cDelim = MULTIPATH_DELIMITER;
    sal_uInt16 nCount = comphelper::string::getTokenCount(rValue, cDelim);
    String aReturn;
    for ( sal_uInt16 i=0; i<nCount ; ++i )
    {
        String aValue = rValue.GetToken( i, cDelim );
        INetURLObject aObj( aValue );
        if ( aObj.GetProtocol() == INET_PROT_FILE )
            aReturn += String(aObj.PathToFileName());
        else if ( ::utl::LocalFileHelper::IsFileContent( aValue ) )
            aReturn += String(aObj.GetURLPath( INetURLObject::DECODE_WITH_CHARSET ));
        if ( i+1 < nCount)
            aReturn += MULTIPATH_DELIMITER;
    }

    return aReturn;
}

// class SvxControlFocusHelper ---------------------------------------------

long SvxControlFocusHelper::Notify( NotifyEvent& rNEvt )
{
    long nRet = Control::Notify( rNEvt );

    if ( m_pFocusCtrl && rNEvt.GetWindow() != m_pFocusCtrl && rNEvt.GetType() == EVENT_GETFOCUS )
        m_pFocusCtrl->GrabFocus();
    return nRet;
}

// functions -------------------------------------------------------------

sal_Bool IsMultiPath_Impl( const sal_uInt16 nIndex )
{
#if OSL_DEBUG_LEVEL > 1
    return ( SvtPathOptions::PATH_AUTOCORRECT == nIndex ||
             SvtPathOptions::PATH_AUTOTEXT == nIndex ||
             SvtPathOptions::PATH_BASIC == nIndex ||
             SvtPathOptions::PATH_GALLERY == nIndex ||
             SvtPathOptions::PATH_TEMPLATE == nIndex );
#else
    return ( SvtPathOptions::PATH_AUTOCORRECT == nIndex ||
             SvtPathOptions::PATH_AUTOTEXT == nIndex ||
             SvtPathOptions::PATH_BASIC == nIndex ||
             SvtPathOptions::PATH_GALLERY == nIndex ||
             SvtPathOptions::PATH_TEMPLATE == nIndex ||
             SvtPathOptions::PATH_LINGUISTIC == nIndex ||
             SvtPathOptions::PATH_DICTIONARY == nIndex  );
#endif
}

// class SvxPathTabPage --------------------------------------------------

SvxPathTabPage::SvxPathTabPage( Window* pParent, const SfxItemSet& rSet ) :

    SfxTabPage( pParent, CUI_RES( RID_SFXPAGE_PATH ), rSet ),

    aStdBox         ( this, CUI_RES( GB_STD ) ),
    aTypeText       ( this, CUI_RES( FT_TYPE ) ),
    aPathText       ( this, CUI_RES( FT_PATH ) ),
    aPathCtrl       ( this, CUI_RES( LB_PATH ) ),
    aStandardBtn    ( this, CUI_RES( BTN_STANDARD ) ),
    aPathBtn        ( this, CUI_RES( BTN_PATH ) ),
    pHeaderBar      ( NULL ),
    pPathBox        ( NULL ),
    pImpl           ( new OptPath_Impl( CUI_RES(IMG_LOCK) ) ),
    xDialogListener ( new ::svt::DialogClosedListener() )

{
    pImpl->m_sMultiPathDlg = String( CUI_RES( STR_MULTIPATHDLG ) );
    aStandardBtn.SetClickHdl( LINK( this, SvxPathTabPage, StandardHdl_Impl ) );
    Link aLink = LINK( this, SvxPathTabPage, PathHdl_Impl );
    aPathBtn.SetClickHdl( aLink );
    Size aBoxSize = aPathCtrl.GetOutputSizePixel();
    pHeaderBar = new HeaderBar( &aPathCtrl, WB_BUTTONSTYLE | WB_BOTTOMBORDER );
    pHeaderBar->SetPosSizePixel( Point( 0, 0 ), Size( aBoxSize.Width(), 16 ) );
    pHeaderBar->SetSelectHdl( LINK( this, SvxPathTabPage, HeaderSelect_Impl ) );
    pHeaderBar->SetEndDragHdl( LINK( this, SvxPathTabPage, HeaderEndDrag_Impl ) );
    Size aSz;
    aSz.Width() = TAB_WIDTH1;
    pHeaderBar->InsertItem( ITEMID_TYPE, aTypeText.GetText(),
                            LogicToPixel( aSz, MapMode( MAP_APPFONT ) ).Width(),
                            HIB_LEFT | HIB_VCENTER | HIB_CLICKABLE | HIB_UPARROW );
    aSz.Width() = TAB_WIDTH2;
    pHeaderBar->InsertItem( ITEMID_PATH, aPathText.GetText(),
                            LogicToPixel( aSz, MapMode( MAP_APPFONT ) ).Width(),
                            HIB_LEFT | HIB_VCENTER );

    static long nTabs[] = {3, 0, TAB_WIDTH1, TAB_WIDTH1 + TAB_WIDTH2 };
    Size aHeadSize = pHeaderBar->GetSizePixel();

    WinBits nBits = WB_SORT | WB_HSCROLL | WB_CLIPCHILDREN | WB_TABSTOP;
    pPathBox = new svx::OptHeaderTabListBox( &aPathCtrl, nBits );
    aPathCtrl.SetFocusControl( pPathBox );
    pPathBox->SetDoubleClickHdl( aLink );
    pPathBox->SetSelectHdl( LINK( this, SvxPathTabPage, PathSelect_Impl ) );
    pPathBox->SetSelectionMode( MULTIPLE_SELECTION );
    pPathBox->SetPosSizePixel( Point( 0, aHeadSize.Height() ),
                               Size( aBoxSize.Width(), aBoxSize.Height() - aHeadSize.Height() ) );
    pPathBox->SetTabs( &nTabs[0], MAP_APPFONT );
    pPathBox->InitHeaderBar( pHeaderBar );
    pPathBox->SetHighlightRange();
    pPathBox->SetHelpId( HID_OPTPATH_CTL_PATH );
    pHeaderBar->SetHelpId( HID_OPTPATH_HEADERBAR );
    pPathBox->Show();
    pHeaderBar->Show();

    FreeResource();

    xDialogListener->SetDialogClosedLink( LINK( this, SvxPathTabPage, DialogClosedHdl ) );
}

// -----------------------------------------------------------------------

SvxPathTabPage::~SvxPathTabPage()
{
    // #110603# do not grab focus to a destroyed window !!!
    aPathCtrl.SetFocusControl( NULL );

    pHeaderBar->Hide();
    for ( sal_uInt16 i = 0; i < pPathBox->GetEntryCount(); ++i )
        delete (PathUserData_Impl*)pPathBox->GetEntry(i)->GetUserData();
    delete pPathBox;
    delete pHeaderBar;
    delete pImpl;
}

// -----------------------------------------------------------------------

SfxTabPage* SvxPathTabPage::Create( Window* pParent,
                                    const SfxItemSet& rAttrSet )
{
    return ( new SvxPathTabPage( pParent, rAttrSet ) );
}

// -----------------------------------------------------------------------

sal_Bool SvxPathTabPage::FillItemSet( SfxItemSet& )
{
    SvtPathOptions aPathOpt;
    for ( sal_uInt16 i = 0; i < pPathBox->GetEntryCount(); ++i )
    {
        PathUserData_Impl* pPathImpl = (PathUserData_Impl*)pPathBox->GetEntry(i)->GetUserData();
        sal_uInt16 nRealId = pPathImpl->nRealId;
        if ( pPathImpl->eState == SFX_ITEM_SET )
            SetPathList( nRealId, pPathImpl->sUserPath, pPathImpl->sWritablePath );
    }
    return sal_True;
}

// -----------------------------------------------------------------------

void SvxPathTabPage::Reset( const SfxItemSet& )
{
    pPathBox->Clear();
    SvtPathOptions aPathOpt; //! deprecated

    for( sal_uInt16 i = 0; i <= (sal_uInt16)SvtPathOptions::PATH_WORK; ++i )
    {
        // only writer uses autotext
        if ( i == SvtPathOptions::PATH_AUTOTEXT
            && !SvtModuleOptions().IsModuleInstalled( SvtModuleOptions::E_SWRITER ) )
            continue;

        switch (i)
        {
            case SvtPathOptions::PATH_AUTOCORRECT:
            case SvtPathOptions::PATH_AUTOTEXT:
            case SvtPathOptions::PATH_BACKUP:
            case SvtPathOptions::PATH_GALLERY:
            case SvtPathOptions::PATH_GRAPHIC:
            case SvtPathOptions::PATH_TEMP:
            case SvtPathOptions::PATH_TEMPLATE:
#if OSL_DEBUG_LEVEL > 1
            case SvtPathOptions::PATH_LINGUISTIC:
            case SvtPathOptions::PATH_DICTIONARY:
#endif
            case SvtPathOptions::PATH_WORK:
            {
                String aStr( CUI_RES( RID_SVXSTR_PATH_NAME_START + i ) );
                String sInternal, sUser, sWritable;
                sal_Bool bReadOnly = sal_False;
                GetPathList( i, sInternal, sUser, sWritable, bReadOnly );
                String sTmpPath = sUser;
                if ( sTmpPath.Len() > 0 && sWritable.Len() > 0 )
                    sTmpPath += MULTIPATH_DELIMITER;
                sTmpPath += sWritable;
                String aValue( sTmpPath );
                aStr += '\t';
                aStr += Convert_Impl( aValue );
                SvTreeListEntry* pEntry = pPathBox->InsertEntry( aStr );
                if ( bReadOnly )
                {
                    pPathBox->SetCollapsedEntryBmp( pEntry, pImpl->m_aLockImage );
                }
                PathUserData_Impl* pPathImpl = new PathUserData_Impl(i);
                pPathImpl->sUserPath = sUser;
                pPathImpl->sWritablePath = sWritable;
                pEntry->SetUserData( pPathImpl );
            }
        }
    }

    String aUserData = GetUserData();
    if ( aUserData.Len() )
    {
        // restore column width
        pHeaderBar->SetItemSize( ITEMID_TYPE, aUserData.GetToken(0).ToInt32() );
        HeaderEndDrag_Impl( NULL );
        // restore sort direction
        sal_Bool bUp = (sal_Bool)(sal_uInt16)aUserData.GetToken(1).ToInt32();
        HeaderBarItemBits nBits = pHeaderBar->GetItemBits(ITEMID_TYPE);

        if ( bUp )
        {
            nBits &= ~HIB_UPARROW;
            nBits |= HIB_DOWNARROW;
        }
        else
        {
            nBits &= ~HIB_DOWNARROW;
            nBits |= HIB_UPARROW;
        }
        pHeaderBar->SetItemBits( ITEMID_TYPE, nBits );
        HeaderSelect_Impl( NULL );
    }
    PathSelect_Impl( NULL );
}

// -----------------------------------------------------------------------

void SvxPathTabPage::FillUserData()
{
    String aUserData = String::CreateFromInt32( pHeaderBar->GetItemSize( ITEMID_TYPE ) );
    aUserData += ';';
    HeaderBarItemBits nBits = pHeaderBar->GetItemBits( ITEMID_TYPE );
    sal_Bool bUp = ( ( nBits & HIB_UPARROW ) == HIB_UPARROW );
    aUserData += bUp ? '1' : '0';
    SetUserData( aUserData );
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxPathTabPage, PathSelect_Impl)
{
    sal_uInt16 nSelCount = 0;
    SvTreeListEntry* pEntry = pPathBox->FirstSelected();

    //the entry image indicates whether the path is write protected
    Image aEntryImage;
    if(pEntry)
        aEntryImage = pPathBox->GetCollapsedEntryBmp( pEntry );
    sal_Bool bEnable = !aEntryImage;
    while ( pEntry && ( nSelCount < 2 ) )
    {
        nSelCount++;
        pEntry = pPathBox->NextSelected( pEntry );
    }

    aPathBtn.Enable( 1 == nSelCount && bEnable);
    aStandardBtn.Enable( nSelCount > 0 && bEnable);
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxPathTabPage, StandardHdl_Impl)
{
    SvTreeListEntry* pEntry = pPathBox->FirstSelected();
    while ( pEntry )
    {
        PathUserData_Impl* pPathImpl = (PathUserData_Impl*)pEntry->GetUserData();
        String aOldPath = pImpl->m_aDefOpt.GetDefaultPath( pPathImpl->nRealId );

        if ( aOldPath.Len() )
        {
            String sInternal, sUser, sWritable, sTemp;
            sal_Bool bReadOnly = sal_False;
            GetPathList( pPathImpl->nRealId, sInternal, sUser, sWritable, bReadOnly );

            sal_uInt16 i;
            sal_uInt16 nOldCount = comphelper::string::getTokenCount(aOldPath, MULTIPATH_DELIMITER);
            sal_uInt16 nIntCount = comphelper::string::getTokenCount(sInternal, MULTIPATH_DELIMITER);
            for ( i = 0; i < nOldCount; ++i )
            {
                bool bFound = false;
                String sOnePath = aOldPath.GetToken( i, MULTIPATH_DELIMITER );
                for ( sal_uInt16 j = 0; !bFound && j < nIntCount; ++j )
                {
                    if ( sInternal.GetToken( i, MULTIPATH_DELIMITER ) == sOnePath )
                        bFound = true;
                }
                if ( !bFound )
                {
                    if ( sTemp.Len() > 0 )
                        sTemp += MULTIPATH_DELIMITER;
                    sTemp += sOnePath;
                }
            }

            String sUserPath, sWritablePath;
            nOldCount = comphelper::string::getTokenCount(sTemp, MULTIPATH_DELIMITER);
            for ( i = 0; nOldCount > 0 && i < nOldCount - 1; ++i )
            {
                if ( sUserPath.Len() > 0 )
                    sUserPath += MULTIPATH_DELIMITER;
                sUserPath += sTemp.GetToken( i, MULTIPATH_DELIMITER );
            }
            sWritablePath = sTemp.GetToken( nOldCount - 1, MULTIPATH_DELIMITER );

            pPathBox->SetEntryText( Convert_Impl( sTemp ), pEntry, 1 );
            pPathImpl->eState = SFX_ITEM_SET;
            pPathImpl->sUserPath = sUserPath;
            pPathImpl->sWritablePath = sWritablePath;
        }
        pEntry = pPathBox->NextSelected( pEntry );
    }
    return 0;
}

// -----------------------------------------------------------------------

void SvxPathTabPage::ChangeCurrentEntry( const String& _rFolder )
{
    SvTreeListEntry* pEntry = pPathBox->GetCurEntry();
    if ( !pEntry )
    {
        SAL_WARN( "cui.options", "SvxPathTabPage::ChangeCurrentEntry(): no entry" );
        return;
    }

    String sInternal, sUser, sWritable;
    PathUserData_Impl* pPathImpl = (PathUserData_Impl*)pEntry->GetUserData();
    sal_Bool bReadOnly = sal_False;
    GetPathList( pPathImpl->nRealId, sInternal, sUser, sWritable, bReadOnly );
    sUser = pPathImpl->sUserPath;
    sWritable = pPathImpl->sWritablePath;
    sal_uInt16 nPos = pPathImpl->nRealId;

    // old path is an URL?
    INetURLObject aObj( sWritable );
    bool bURL = ( aObj.GetProtocol() != INET_PROT_NOT_VALID );
    rtl::OUString aPathStr( _rFolder );
    INetURLObject aNewObj( aPathStr );
    aNewObj.removeFinalSlash();

    // then the new path also an URL else system path
    String sNewPathStr = bURL ? aPathStr : aNewObj.getFSysPath( INetURLObject::FSYS_DETECT );

    bool bChanged =
#ifdef UNX
// Unix is case sensitive
        ( sNewPathStr != sWritable );
#else
        ( sNewPathStr.CompareIgnoreCaseToAscii( sWritable ) != COMPARE_EQUAL );
#endif

    if ( bChanged )
    {
        pPathBox->SetEntryText( Convert_Impl( sNewPathStr ), pEntry, 1 );
        nPos = (sal_uInt16)pPathBox->GetModel()->GetAbsPos( pEntry );
        pPathImpl = (PathUserData_Impl*)pPathBox->GetEntry(nPos)->GetUserData();
        pPathImpl->eState = SFX_ITEM_SET;
        pPathImpl->sWritablePath = sNewPathStr;
        if ( SvtPathOptions::PATH_WORK == pPathImpl->nRealId )
        {
            // Remove view options entry so the new work path
            // will be used for the next open dialog.
            SvtViewOptions aDlgOpt( E_DIALOG, IODLG_CONFIGNAME );
            aDlgOpt.Delete();
            // Reset also last used dir in the sfx application instance
            SfxApplication *pSfxApp = SFX_APP();
            pSfxApp->ResetLastDir();

            // Set configuration flag to notify file picker that it's necessary
            // to take over the path provided.
            boost::shared_ptr< comphelper::ConfigurationChanges > batch(
                comphelper::ConfigurationChanges::create());
            officecfg::Office::Common::Path::Info::WorkPathChanged::set(
                true, batch);
            batch->commit();
        }
    }
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxPathTabPage, PathHdl_Impl)
{
    SvTreeListEntry* pEntry = pPathBox->GetCurEntry();
    sal_uInt16 nPos = ( pEntry != NULL ) ? ( (PathUserData_Impl*)pEntry->GetUserData() )->nRealId : 0;
    String sInternal, sUser, sWritable;
    if ( pEntry )
    {
        PathUserData_Impl* pPathImpl = (PathUserData_Impl*)pEntry->GetUserData();
        sal_Bool bReadOnly = sal_False;
        GetPathList( pPathImpl->nRealId, sInternal, sUser, sWritable, bReadOnly );
        sUser = pPathImpl->sUserPath;
        sWritable = pPathImpl->sWritablePath;
    }

    if(pEntry && !(!((OptHeaderTabListBox*)pPathBox)->GetCollapsedEntryBmp(pEntry)))
        return 0;

    if ( IsMultiPath_Impl( nPos ) )
    {
        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        if ( pFact )
        {
            AbstractSvxMultiPathDialog* pMultiDlg =
                pFact->CreateSvxMultiPathDialog( this );
            DBG_ASSERT( pMultiDlg, "Dialogdiet fail!" );
            pMultiDlg->EnableRadioButtonMode();

            String sPath( sUser );
            if ( sPath.Len() > 0 )
                sPath += MULTIPATH_DELIMITER;
            sPath += sWritable;
            pMultiDlg->SetPath( sPath );

            String sPathName = pPathBox->GetEntryText( pEntry, 0 );
            String sNewTitle( pImpl->m_sMultiPathDlg );
            sNewTitle.SearchAndReplace( VAR_ONE, sPathName );
            pMultiDlg->SetTitle( sNewTitle );

            if ( pMultiDlg->Execute() == RET_OK && pEntry )
            {
                sUser.Erase();
                sWritable.Erase();
                String sFullPath;
                String sNewPath = pMultiDlg->GetPath();
                char cDelim = MULTIPATH_DELIMITER;
                sal_uInt16 nCount = comphelper::string::getTokenCount(sNewPath, cDelim);
                if ( nCount > 0 )
                {
                    sal_uInt16 i = 0;
                    for ( ; i < nCount - 1; ++i )
                    {
                        if ( sUser.Len() > 0 )
                            sUser += cDelim;
                        sUser += sNewPath.GetToken( i, cDelim );
                    }
                    if ( sFullPath.Len() > 0 )
                        sFullPath += cDelim;
                    sFullPath += sUser;
                    sWritable += sNewPath.GetToken( i, cDelim );
                    if ( sFullPath.Len() > 0 )
                        sFullPath += cDelim;
                    sFullPath += sWritable;
                }

                pPathBox->SetEntryText( Convert_Impl( sFullPath ), pEntry, 1 );
                // save modified flag
                PathUserData_Impl* pPathImpl = (PathUserData_Impl*)pEntry->GetUserData();
                pPathImpl->eState = SFX_ITEM_SET;
                pPathImpl->sUserPath = sUser;
                pPathImpl->sWritablePath = sWritable;
            }
            delete pMultiDlg;
        }
    }
    else if ( pEntry )
    {
        try
        {
            Reference < XComponentContext > xContext( ::comphelper::getProcessComponentContext() );
            xFolderPicker = FolderPicker::create(xContext);;

            INetURLObject aURL( sWritable, INET_PROT_FILE );
            xFolderPicker->setDisplayDirectory( aURL.GetMainURL( INetURLObject::NO_DECODE ) );

            Reference< XAsynchronousExecutableDialog > xAsyncDlg( xFolderPicker, UNO_QUERY );
            if ( xAsyncDlg.is() )
                xAsyncDlg->startExecuteModal( xDialogListener.get() );
            else
            {
                short nRet = xFolderPicker->execute();
                if ( ExecutableDialogResults::OK != nRet )
                    return 0;

                String sFolder( xFolderPicker->getDirectory() );
                ChangeCurrentEntry( sFolder );
            }
        }
        catch( Exception& )
        {
            SAL_WARN( "cui.options", "SvxPathTabPage::PathHdl_Impl: exception from folder picker" );
        }
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxPathTabPage, HeaderSelect_Impl, HeaderBar*, pBar )
{
    if ( pBar && pBar->GetCurItemId() != ITEMID_TYPE )
        return 0;

    HeaderBarItemBits nBits = pHeaderBar->GetItemBits(ITEMID_TYPE);
    sal_Bool bUp = ( ( nBits & HIB_UPARROW ) == HIB_UPARROW );
    SvSortMode eMode = SortAscending;

    if ( bUp )
    {
        nBits &= ~HIB_UPARROW;
        nBits |= HIB_DOWNARROW;
        eMode = SortDescending;
    }
    else
    {
        nBits &= ~HIB_DOWNARROW;
        nBits |= HIB_UPARROW;
    }
    pHeaderBar->SetItemBits( ITEMID_TYPE, nBits );
    SvTreeList* pModel = pPathBox->GetModel();
    pModel->SetSortMode( eMode );
    pModel->Resort();
    return 1;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxPathTabPage, HeaderEndDrag_Impl, HeaderBar*, pBar )
{
    if ( pBar && !pBar->GetCurItemId() )
        return 0;

    if ( !pHeaderBar->IsItemMode() )
    {
        Size aSz;
        sal_uInt16 nTabs = pHeaderBar->GetItemCount();
        long nTmpSz = 0;
        long nWidth = pHeaderBar->GetItemSize(ITEMID_TYPE);
        long nBarWidth = pHeaderBar->GetSizePixel().Width();

        if(nWidth < TAB_WIDTH_MIN)
            pHeaderBar->SetItemSize( ITEMID_TYPE, TAB_WIDTH_MIN);
        else if ( ( nBarWidth - nWidth ) < TAB_WIDTH_MIN )
            pHeaderBar->SetItemSize( ITEMID_TYPE, nBarWidth - TAB_WIDTH_MIN );

        for ( sal_uInt16 i = 1; i <= nTabs; ++i )
        {
            long _nWidth = pHeaderBar->GetItemSize(i);
            aSz.Width() =  _nWidth + nTmpSz;
            nTmpSz += _nWidth;
            pPathBox->SetTab( i, PixelToLogic( aSz, MapMode(MAP_APPFONT) ).Width(), MAP_APPFONT );
        }
    }
    return 1;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxPathTabPage, DialogClosedHdl, DialogClosedEvent*, pEvt )
{
    if ( RET_OK == pEvt->DialogResult )
    {
        DBG_ASSERT( xFolderPicker.is() == sal_True, "SvxPathTabPage::DialogClosedHdl(): no folder picker" );

        String sURL = String( xFolderPicker->getDirectory() );
        ChangeCurrentEntry( sURL );
    }
    return 0L;
}

// -----------------------------------------------------------------------

void SvxPathTabPage::GetPathList(
    sal_uInt16 _nPathHandle, String& _rInternalPath,
    String& _rUserPath, String& _rWritablePath, sal_Bool& _rReadOnly )
{
    String sCfgName = getCfgName_Impl( _nPathHandle );

    // load PathSettings service if necessary
    if ( !pImpl->m_xPathSettings.is() )
    {
        Reference< XMultiServiceFactory > xSMgr = comphelper::getProcessServiceFactory();
        pImpl->m_xPathSettings = Reference< XPropertySet >( xSMgr->createInstance(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.util.PathSettings") ) ), UNO_QUERY );
    }

    try
    {
        if ( pImpl->m_xPathSettings.is() )
        {
            // load internal paths
            String sProp( sCfgName );
            sProp = sCfgName;
            sProp += POSTFIX_INTERNAL;
            Any aAny = pImpl->m_xPathSettings->getPropertyValue( sProp );
            Sequence< ::rtl::OUString > aPathSeq;
            if ( aAny >>= aPathSeq )
            {
                long i, nCount = aPathSeq.getLength();
                const ::rtl::OUString* pPaths = aPathSeq.getConstArray();

                for ( i = 0; i < nCount; ++i )
                {
                    if ( _rInternalPath.Len() > 0 )
                        _rInternalPath += ';';
                    _rInternalPath += String( pPaths[i] );
                }
            }
            // load user paths
            sProp = sCfgName;
            sProp += POSTFIX_USER;
            aAny = pImpl->m_xPathSettings->getPropertyValue( sProp );
            if ( aAny >>= aPathSeq )
            {
                long i, nCount = aPathSeq.getLength();
                const ::rtl::OUString* pPaths = aPathSeq.getConstArray();

                for ( i = 0; i < nCount; ++i )
                {
                    if ( _rUserPath.Len() > 0 )
                        _rUserPath += ';';
                    _rUserPath += String( pPaths[i] );
                }
            }
            // then the writable path
            sProp = sCfgName;
            sProp += POSTFIX_WRITABLE;
            aAny = pImpl->m_xPathSettings->getPropertyValue( sProp );
            ::rtl::OUString sWritablePath;
            if ( aAny >>= sWritablePath )
                _rWritablePath = String( sWritablePath );

            // and the readonly flag
            sProp = sCfgName;
            Reference< XPropertySetInfo > xInfo = pImpl->m_xPathSettings->getPropertySetInfo();
            Property aProp = xInfo->getPropertyByName( sProp );
            _rReadOnly = ( ( aProp.Attributes & PropertyAttribute::READONLY ) == PropertyAttribute::READONLY );
        }
    }
    catch( const Exception& )
    {
        OSL_FAIL( "SvxPathTabPage::GetPathList(): caught an exception!" );
    }
}

// -----------------------------------------------------------------------

void SvxPathTabPage::SetPathList(
    sal_uInt16 _nPathHandle, const String& _rUserPath, const String& _rWritablePath )
{
    String sCfgName = getCfgName_Impl( _nPathHandle );

    // load PathSettings service if necessary
    if ( !pImpl->m_xPathSettings.is() )
    {
        Reference< XMultiServiceFactory > xSMgr = comphelper::getProcessServiceFactory();
        pImpl->m_xPathSettings = Reference< XPropertySet >( xSMgr->createInstance(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.util.PathSettings") ) ), UNO_QUERY );
    }

    try
    {
        if ( pImpl->m_xPathSettings.is() )
        {
            // save user paths
            char cDelim = MULTIPATH_DELIMITER;
            sal_uInt16 nCount = comphelper::string::getTokenCount(_rUserPath, cDelim);
            Sequence< ::rtl::OUString > aPathSeq( nCount );
            ::rtl::OUString* pArray = aPathSeq.getArray();
            for ( sal_uInt16 i = 0; i < nCount; ++i )
                pArray[i] = ::rtl::OUString( _rUserPath.GetToken( i, cDelim ) );
            String sProp( sCfgName );
            sProp += POSTFIX_USER;
            Any aValue = makeAny( aPathSeq );
            pImpl->m_xPathSettings->setPropertyValue( sProp, aValue );

            // then the writable path
            aValue = makeAny( ::rtl::OUString( _rWritablePath ) );
            sProp = sCfgName;
            sProp += POSTFIX_WRITABLE;
            pImpl->m_xPathSettings->setPropertyValue( sProp, aValue );
        }
    }
    catch( const Exception& e )
    {
        SAL_WARN("cui", "caught: " << e.Message);
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
