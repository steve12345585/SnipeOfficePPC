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
#include "moduldlg.hrc"

#include "moduldlg.hxx"
#include "localizationmgr.hxx"
#include "basidesh.hxx"
#include "baside3.hxx"
#include "iderdll.hxx"

#include <basic/basmgr.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/script/XLibraryContainerPassword.hpp>
#include <comphelper/processfactory.hxx>
#include <sfx2/app.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/request.hxx>
#include <sfx2/viewfrm.hxx>
#include <svl/itemset.hxx>
#include <svl/stritem.hxx>
#include <vcl/msgbox.hxx>
#include <tools/diagnose_ex.h>
#include <xmlscript/xmldlg_imexp.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::resource;

ExtBasicTreeListBox::ExtBasicTreeListBox( Window* pParent, const ResId& rRes )
    : BasicTreeListBox( pParent, rRes )
{
}

ExtBasicTreeListBox::~ExtBasicTreeListBox()
{
}

sal_Bool ExtBasicTreeListBox::EditingEntry( SvLBoxEntry* pEntry, Selection& )
{
    sal_Bool bRet = sal_False;

    if ( pEntry )
    {
        sal_uInt16 nDepth = GetModel()->GetDepth( pEntry );
        if ( nDepth >= 2 )
        {
            BasicEntryDescriptor aDesc( GetEntryDescriptor( pEntry ) );
            ScriptDocument aDocument( aDesc.GetDocument() );
            ::rtl::OUString aLibName( aDesc.GetLibName() );
            Reference< script::XLibraryContainer2 > xModLibContainer( aDocument.getLibraryContainer( E_SCRIPTS ), UNO_QUERY );
            Reference< script::XLibraryContainer2 > xDlgLibContainer( aDocument.getLibraryContainer( E_DIALOGS ), UNO_QUERY );
            if ( !( ( xModLibContainer.is() && xModLibContainer->hasByName( aLibName ) && xModLibContainer->isLibraryReadOnly( aLibName ) ) ||
                    ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aLibName ) && xDlgLibContainer->isLibraryReadOnly( aLibName ) ) ) )
            {
                // allow editing only for libraries, which are not readonly
                bRet = sal_True;
            }
        }
    }

    return bRet;
}

sal_Bool ExtBasicTreeListBox::EditedEntry( SvLBoxEntry* pEntry, const rtl::OUString& rNewText )
{
    sal_Bool bValid = BasicIDE::IsValidSbxName( rNewText );
    if ( !bValid )
    {
        ErrorBox( this, WB_OK | WB_DEF_OK, IDE_RESSTR(RID_STR_BADSBXNAME) ).Execute();
        return sal_False;
    }

    rtl::OUString aCurText( GetEntryText( pEntry ) );
    if ( aCurText == rNewText )
        // nothing to do
        return sal_True;

    BasicEntryDescriptor aDesc( GetEntryDescriptor( pEntry ) );
    ScriptDocument aDocument( aDesc.GetDocument() );
    DBG_ASSERT( aDocument.isValid(), "ExtBasicTreeListBox::EditedEntry: no document!" );
    if ( !aDocument.isValid() )
        return sal_False;
    ::rtl::OUString aLibName( aDesc.GetLibName() );
    BasicEntryType eType( aDesc.GetType() );

    bool bSuccess = ( eType == OBJ_TYPE_MODULE )
        ?   BasicIDE::RenameModule( this, aDocument, aLibName, aCurText, rNewText )
        :   BasicIDE::RenameDialog( this, aDocument, aLibName, aCurText, rNewText );

    if ( !bSuccess )
        return sal_False;

    BasicIDE::MarkDocumentModified( aDocument );

    BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
    SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
    SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
    if( pDispatcher )
    {
        SbxItem aSbxItem( SID_BASICIDE_ARG_SBX, aDocument, aLibName, rNewText, ConvertType( eType ) );
        pDispatcher->Execute( SID_BASICIDE_SBXRENAMED,
                                SFX_CALLMODE_SYNCHRON, &aSbxItem, 0L );
    }

    // OV-Bug?!
    SetEntryText( pEntry, rNewText );
    SetCurEntry( pEntry );
    SetCurEntry( pEntry );
    Select( pEntry, sal_False );
    Select( pEntry );       // so that handler is called => update edit

    return sal_True;
}


DragDropMode ExtBasicTreeListBox::NotifyStartDrag( TransferDataContainer&, SvLBoxEntry* pEntry )
{
    DragDropMode nMode_ = SV_DRAGDROP_NONE;

    if ( pEntry )
    {
        sal_uInt16 nDepth = GetModel()->GetDepth( pEntry );
        if ( nDepth >= 2 )
        {
            nMode_ = SV_DRAGDROP_CTRL_COPY;
            BasicEntryDescriptor aDesc( GetEntryDescriptor( pEntry ) );
            ScriptDocument aDocument( aDesc.GetDocument() );
            ::rtl::OUString aLibName( aDesc.GetLibName() );
            // allow MOVE mode only for libraries, which are not readonly
            Reference< script::XLibraryContainer2 > xModLibContainer( aDocument.getLibraryContainer( E_SCRIPTS ), UNO_QUERY );
            Reference< script::XLibraryContainer2 > xDlgLibContainer( aDocument.getLibraryContainer( E_DIALOGS ), UNO_QUERY );
            if ( !( ( xModLibContainer.is() && xModLibContainer->hasByName( aLibName ) && xModLibContainer->isLibraryReadOnly( aLibName ) ) ||
                    ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aLibName ) && xDlgLibContainer->isLibraryReadOnly( aLibName ) ) ) )
            {
                // Only allow copy for localized libraries
                bool bAllowMove = true;
                if ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aLibName ) )
                {
                    // Get StringResourceManager
                    Reference< container::XNameContainer > xDialogLib( aDocument.getLibrary( E_DIALOGS, aLibName, sal_True ) );
                    Reference< XStringResourceManager > xSourceMgr =
                        LocalizationMgr::getStringResourceFromDialogLibrary( xDialogLib );
                    if( xSourceMgr.is() )
                        bAllowMove = ( xSourceMgr->getLocales().getLength() == 0 );
                }
                if( bAllowMove )
                    nMode_ |= SV_DRAGDROP_CTRL_MOVE;
            }
        }
    }

    return nMode_;
}


sal_Bool ExtBasicTreeListBox::NotifyAcceptDrop( SvLBoxEntry* pEntry )
{
    // don't drop on a BasicManager (nDepth == 0)
    sal_uInt16 nDepth = pEntry ? GetModel()->GetDepth( pEntry ) : 0;
    sal_Bool bValid = nDepth ? sal_True : sal_False;

    // don't drop in the same library
    SvLBoxEntry* pSelected = FirstSelected();
    if ( ( nDepth == 1 ) && ( pEntry == GetParent( pSelected ) ) )
        bValid = sal_False;
    else if ( ( nDepth == 2 ) && ( GetParent( pEntry ) == GetParent( pSelected ) ) )
        bValid = sal_False;

    // don't drop on a library, which is not loaded, readonly or password protected
    // or which already has a module/dialog with this name
    if ( bValid && ( nDepth > 0 ) )
    {
        // get source module/dialog name
        BasicEntryDescriptor aSourceDesc( GetEntryDescriptor( pSelected ) );
        ::rtl::OUString aSourceName( aSourceDesc.GetName() );
        BasicEntryType eSourceType( aSourceDesc.GetType() );

        // get target shell and target library name
        BasicEntryDescriptor aDestDesc( GetEntryDescriptor( pEntry ) );
        const ScriptDocument& rDestDoc( aDestDesc.GetDocument() );
        ::rtl::OUString aDestLibName( aDestDesc.GetLibName() );

        // check if module library is not loaded, readonly or password protected
        Reference< script::XLibraryContainer2 > xModLibContainer( rDestDoc.getLibraryContainer( E_SCRIPTS ), UNO_QUERY );
        if ( xModLibContainer.is() && xModLibContainer->hasByName( aDestLibName ) )
        {
            if ( !xModLibContainer->isLibraryLoaded( aDestLibName ) )
                bValid = sal_False;

            if ( xModLibContainer->isLibraryReadOnly( aDestLibName ) )
                bValid = sal_False;

            Reference< script::XLibraryContainerPassword > xPasswd( xModLibContainer, UNO_QUERY );
            if ( xPasswd.is() && xPasswd->isLibraryPasswordProtected( aDestLibName ) && !xPasswd->isLibraryPasswordVerified( aDestLibName ) )
                bValid = sal_False;
        }

        // check if dialog library is not loaded or readonly
        Reference< script::XLibraryContainer2 > xDlgLibContainer( rDestDoc.getLibraryContainer( E_DIALOGS ), UNO_QUERY );
        if ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aDestLibName ) )
        {
            if ( !xDlgLibContainer->isLibraryLoaded( aDestLibName ) )
                bValid = sal_False;

            if ( xDlgLibContainer->isLibraryReadOnly( aDestLibName ) )
                bValid = sal_False;
        }

        // check, if module/dialog with this name is already existing in target library
        if ( ( eSourceType == OBJ_TYPE_MODULE && rDestDoc.hasModule( aDestLibName, aSourceName ) ) ||
            ( eSourceType == OBJ_TYPE_DIALOG && rDestDoc.hasDialog( aDestLibName, aSourceName ) ) )
        {
            bValid = sal_False;
        }
    }

    return bValid;
}

sal_Bool ExtBasicTreeListBox::NotifyMoving( SvLBoxEntry* pTarget, SvLBoxEntry* pEntry,
                        SvLBoxEntry*& rpNewParent, sal_uLong& rNewChildPos )
{
    return NotifyCopyingMoving( pTarget, pEntry,
                                    rpNewParent, rNewChildPos, sal_True );
}

sal_Bool ExtBasicTreeListBox::NotifyCopying( SvLBoxEntry* pTarget, SvLBoxEntry* pEntry,
                        SvLBoxEntry*& rpNewParent, sal_uLong& rNewChildPos )
{
//  return sal_False;   // how do I copy an SBX?!
    return NotifyCopyingMoving( pTarget, pEntry,
                                    rpNewParent, rNewChildPos, sal_False );
}

void BasicIDEShell::CopyDialogResources( Reference< io::XInputStreamProvider >& io_xISP,
                                         const ScriptDocument& rSourceDoc, const ::rtl::OUString& rSourceLibName, const ScriptDocument& rDestDoc,
                                         const ::rtl::OUString& rDestLibName, const ::rtl::OUString& rDlgName )
{
    if ( !io_xISP.is() )
        return;

    // Get StringResourceManager
    Reference< container::XNameContainer > xSourceDialogLib( rSourceDoc.getLibrary( E_DIALOGS, rSourceLibName, sal_True ) );
    Reference< XStringResourceManager > xSourceMgr =
        LocalizationMgr::getStringResourceFromDialogLibrary( xSourceDialogLib );
    if( !xSourceMgr.is() )
        return;
    bool bSourceLocalized = ( xSourceMgr->getLocales().getLength() > 0 );

    Reference< container::XNameContainer > xDestDialogLib( rDestDoc.getLibrary( E_DIALOGS, rDestLibName, sal_True ) );
    Reference< XStringResourceManager > xDestMgr =
        LocalizationMgr::getStringResourceFromDialogLibrary( xDestDialogLib );
    if( !xDestMgr.is() )
        return;
    bool bDestLocalized = ( xDestMgr->getLocales().getLength() > 0 );

    if( !bSourceLocalized && !bDestLocalized )
        return;

    // create dialog model
    Reference< lang::XMultiServiceFactory > xMSF = ::comphelper::getProcessServiceFactory();
    Reference< container::XNameContainer > xDialogModel = Reference< container::XNameContainer >( xMSF->createInstance
        ( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlDialogModel" ) ) ), UNO_QUERY );
    Reference< io::XInputStream > xInput( io_xISP->createInputStream() );
    Reference< XComponentContext > xContext;
    Reference< beans::XPropertySet > xProps( xMSF, UNO_QUERY );
    OSL_ASSERT( xProps.is() );
    OSL_VERIFY( xProps->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DefaultContext")) ) >>= xContext );
    ::xmlscript::importDialogModel( xInput, xDialogModel, xContext, rSourceDoc.isDocument() ? rSourceDoc.getDocument() : Reference< frame::XModel >() );

    if( xDialogModel.is() )
    {
        if( bSourceLocalized && bDestLocalized )
        {
            Reference< resource::XStringResourceResolver > xSourceStringResolver( xSourceMgr, UNO_QUERY );
            LocalizationMgr::copyResourceForDroppedDialog( xDialogModel, rDlgName, xDestMgr, xSourceStringResolver );
        }
        else if( bSourceLocalized )
        {
            LocalizationMgr::resetResourceForDialog( xDialogModel, xSourceMgr );
        }
        else if( bDestLocalized )
        {
            LocalizationMgr::setResourceIDsForDialog( xDialogModel, xDestMgr );
        }
        io_xISP = ::xmlscript::exportDialogModel( xDialogModel, xContext, rDestDoc.isDocument() ? rDestDoc.getDocument() : Reference< frame::XModel >() );
    }
}


sal_Bool ExtBasicTreeListBox::NotifyCopyingMoving( SvLBoxEntry* pTarget, SvLBoxEntry* pEntry,
                        SvLBoxEntry*& rpNewParent, sal_uLong& rNewChildPos, sal_Bool bMove )
{
    (void)pEntry;
    DBG_ASSERT( pEntry, "Kein Eintrag?" );  // ASS is ok here, should not be reached
    DBG_ASSERT( pTarget, "Kein Ziel?" );    // with NULL (right at the beginning)
    sal_uInt16 nDepth = GetModel()->GetDepth( pTarget );
    DBG_ASSERT( nDepth, "Tiefe?" );
    if ( nDepth == 1 )
    {
        // Target = Basic => put module/dialog under the Basic
        rpNewParent = pTarget;
        rNewChildPos = 0;
    }
    else if ( nDepth >= 2 )
    {
        // Target = module/dialog => put module/dialog under the superordinate Basic
        rpNewParent = GetParent( pTarget );
        rNewChildPos = GetModel()->GetRelPos( pTarget ) + 1;
    }

    // get target shell and target library name
    BasicEntryDescriptor aDestDesc( GetEntryDescriptor( rpNewParent ) );
    const ScriptDocument& rDestDoc( aDestDesc.GetDocument() );
    ::rtl::OUString aDestLibName( aDestDesc.GetLibName() );

    // get source shell, library name and module/dialog name
    BasicEntryDescriptor aSourceDesc( GetEntryDescriptor( FirstSelected() ) );
    const ScriptDocument rSourceDoc( aSourceDesc.GetDocument() );
    ::rtl::OUString aSourceLibName( aSourceDesc.GetLibName() );
    ::rtl::OUString aSourceName( aSourceDesc.GetName() );
    BasicEntryType eType( aSourceDesc.GetType() );

    // get dispatcher
    BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
    SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
    SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;

    if ( bMove )    // move
    {
        // remove source module/dialog window
        if ( rSourceDoc != rDestDoc || aSourceLibName != aDestLibName )
        {
            if( pDispatcher )
            {
                SbxItem aSbxItem( SID_BASICIDE_ARG_SBX, rSourceDoc, aSourceLibName, aSourceName, ConvertType( eType ) );
                pDispatcher->Execute( SID_BASICIDE_SBXDELETED,
                                      SFX_CALLMODE_SYNCHRON, &aSbxItem, 0L );
            }
        }

        try
        {
            if ( eType == OBJ_TYPE_MODULE ) // module
            {
                // get module
                ::rtl::OUString aModule;
                if ( rSourceDoc.getModule( aSourceLibName, aSourceName, aModule ) )
                {
                    // remove module from source library
                    if ( rSourceDoc.removeModule( aSourceLibName, aSourceName ) )
                    {
                        BasicIDE::MarkDocumentModified( rSourceDoc );

                        // insert module into target library
                        if ( rDestDoc.insertModule( aDestLibName, aSourceName, aModule ) )
                            BasicIDE::MarkDocumentModified( rDestDoc );
                    }
                }
            }
            else if ( eType == OBJ_TYPE_DIALOG )    // dialog
            {
                // get dialog
                Reference< io::XInputStreamProvider > xISP;
                if ( rSourceDoc.getDialog( aSourceLibName, aSourceName, xISP ) )
                {
                    BasicIDEShell::CopyDialogResources( xISP, rSourceDoc,
                        aSourceLibName, rDestDoc, aDestLibName, aSourceName );

                    // remove dialog from source library
                    if ( BasicIDE::RemoveDialog( rSourceDoc, aSourceLibName, aSourceName ) )
                    {
                        BasicIDE::MarkDocumentModified( rSourceDoc );

                        // insert dialog into target library
                        if ( rDestDoc.insertDialog( aDestLibName, aSourceName, xISP ) )
                            BasicIDE::MarkDocumentModified( rDestDoc );
                    }
                }
            }
        }
        catch (const uno::Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }
    else    // copy
    {
        try
        {
            if ( eType == OBJ_TYPE_MODULE ) // module
            {
                // get module
                ::rtl::OUString aModule;
                if ( rSourceDoc.getModule( aSourceLibName, aSourceName, aModule ) )
                {
                    // insert module into target library
                    if ( rDestDoc.insertModule( aDestLibName, aSourceName, aModule ) )
                        BasicIDE::MarkDocumentModified( rDestDoc );
                }
            }
            else if ( eType == OBJ_TYPE_DIALOG )    // dialog
            {
                // get dialog
                Reference< io::XInputStreamProvider > xISP;
                if ( rSourceDoc.getDialog( aSourceLibName, aSourceName, xISP ) )
                {
                    BasicIDEShell::CopyDialogResources( xISP, rSourceDoc,
                        aSourceLibName, rDestDoc, aDestLibName, aSourceName );

                    // insert dialog into target library
                    if ( rDestDoc.insertDialog( aDestLibName, aSourceName, xISP ) )
                        BasicIDE::MarkDocumentModified( rDestDoc );
                }
            }
        }
        catch ( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    // create target module/dialog window
    if ( rSourceDoc != rDestDoc || aSourceLibName != aDestLibName )
    {
        if( pDispatcher )
        {
            SbxItem aSbxItem( SID_BASICIDE_ARG_SBX, rDestDoc, aDestLibName, aSourceName, ConvertType( eType ) );
            pDispatcher->Execute( SID_BASICIDE_SBXINSERTED,
                                  SFX_CALLMODE_SYNCHRON, &aSbxItem, 0L );
        }
    }

    return 2;   // open...
}

OrganizeDialog::OrganizeDialog( Window* pParent, sal_Int16 tabId, BasicEntryDescriptor& rDesc )
    :TabDialog( pParent, IDEResId( RID_TD_ORGANIZE ) )
    ,aTabCtrl( this, IDEResId( RID_TC_ORGANIZE ) )
    ,m_aCurEntry( rDesc )
{
    FreeResource();
    aTabCtrl.SetActivatePageHdl( LINK( this, OrganizeDialog, ActivatePageHdl ) );
    if( tabId == 0 )
    {
        aTabCtrl.SetCurPageId( RID_TP_MOD );
    }
    else if ( tabId == 1 )
    {
        aTabCtrl.SetCurPageId( RID_TP_DLG );
    }
    else
    {
        aTabCtrl.SetCurPageId( RID_TP_LIB );
    }

    ActivatePageHdl( &aTabCtrl );

    BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
    SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
    SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
    if( pDispatcher )
    {
        pDispatcher->Execute( SID_BASICIDE_STOREALLMODULESOURCES );
    }
}

OrganizeDialog::~OrganizeDialog()
{
    for ( sal_uInt16 i = 0; i < aTabCtrl.GetPageCount(); i++ )
        delete aTabCtrl.GetTabPage( aTabCtrl.GetPageId( i ) );
};

short OrganizeDialog::Execute()
{
    Window* pPrevDlgParent = Application::GetDefDialogParent();
    Application::SetDefDialogParent( this );
    short nRet = TabDialog::Execute();
    Application::SetDefDialogParent( pPrevDlgParent );
    return nRet;
}


IMPL_LINK( OrganizeDialog, ActivatePageHdl, TabControl *, pTabCtrl )
{
    sal_uInt16 nId = pTabCtrl->GetCurPageId();

    if ( !pTabCtrl->GetTabPage( nId ) )
    {
        TabPage* pNewTabPage = 0;
        switch ( nId )
        {
            case RID_TP_MOD:
            {
                pNewTabPage = new ObjectPage( pTabCtrl, IDEResId( RID_TP_MODULS ), BROWSEMODE_MODULES );
                ((ObjectPage*)pNewTabPage)->SetTabDlg( this );
                ((ObjectPage*)pNewTabPage)->SetCurrentEntry( m_aCurEntry );
            }
            break;
            case RID_TP_DLG:
            {
                pNewTabPage = new ObjectPage( pTabCtrl, IDEResId( RID_TP_DLGS ), BROWSEMODE_DIALOGS );
                ((ObjectPage*)pNewTabPage)->SetTabDlg( this );
                ((ObjectPage*)pNewTabPage)->SetCurrentEntry( m_aCurEntry );
            }
            break;
            case RID_TP_LIB:
            {
                pNewTabPage = new LibPage( pTabCtrl );
                ((LibPage*)pNewTabPage)->SetTabDlg( this );
            }
            break;
            default:    OSL_FAIL( "PageHdl: Unbekannte ID!" );
        }
        DBG_ASSERT( pNewTabPage, "Keine Page!" );
        pTabCtrl->SetTabPage( nId, pNewTabPage );
    }
    return 0;
}

ObjectPage::ObjectPage( Window * pParent, const ResId& rResId, sal_uInt16 nMode ) :
        TabPage(        pParent, rResId ),
        aLibText(       this,   IDEResId( RID_STR_LIB ) ),
        aBasicBox(      this,   IDEResId( RID_TRLBOX ) ),
        aEditButton(    this,   IDEResId( RID_PB_EDIT ) ),
        aCloseButton(   this,   IDEResId( RID_PB_CLOSE ) ),
        aNewModButton(  this,   IDEResId( RID_PB_NEWMOD ) ),
        aNewDlgButton(  this,   IDEResId( RID_PB_NEWDLG ) ),
        aDelButton(     this,   IDEResId( RID_PB_DELETE ) )
{
    FreeResource();
    pTabDlg = 0;

    aEditButton.SetClickHdl( LINK( this, ObjectPage, ButtonHdl ) );
    aDelButton.SetClickHdl( LINK( this, ObjectPage, ButtonHdl ) );
    aCloseButton.SetClickHdl( LINK( this, ObjectPage, ButtonHdl ) );
    aBasicBox.SetSelectHdl( LINK( this, ObjectPage, BasicBoxHighlightHdl ) );

    if( nMode & BROWSEMODE_MODULES )
    {
        aNewModButton.SetClickHdl( LINK( this, ObjectPage, ButtonHdl ) );
        aNewDlgButton.Hide();
    }
    else if ( nMode & BROWSEMODE_DIALOGS )
    {
        aNewDlgButton.SetClickHdl( LINK( this, ObjectPage, ButtonHdl ) );
        aNewModButton.Hide();
    }

    aBasicBox.SetDragDropMode( SV_DRAGDROP_CTRL_MOVE | SV_DRAGDROP_CTRL_COPY );
    aBasicBox.EnableInplaceEditing( sal_True );
    aBasicBox.SetMode( nMode );
    aBasicBox.SetStyle( WB_BORDER | WB_TABSTOP |
                        WB_HASLINES | WB_HASLINESATROOT |
                        WB_HASBUTTONS | WB_HASBUTTONSATROOT |
                        WB_HSCROLL );
    aBasicBox.ScanAllEntries();

    aEditButton.GrabFocus();
    CheckButtons();
}

void ObjectPage::SetCurrentEntry( BasicEntryDescriptor& rDesc )
{
    aBasicBox.SetCurrentEntry( rDesc );
}

void ObjectPage::ActivatePage()
{
    aBasicBox.UpdateEntries();
}

void ObjectPage::DeactivatePage()
{
}

void ObjectPage::CheckButtons()
{
    // enable/disable edit button
    SvLBoxEntry* pCurEntry = aBasicBox.GetCurEntry();
    BasicEntryDescriptor aDesc( aBasicBox.GetEntryDescriptor( pCurEntry ) );
    ScriptDocument aDocument( aDesc.GetDocument() );
    ::rtl::OUString aLibName( aDesc.GetLibName() );
    ::rtl::OUString aLibSubName( aDesc.GetLibSubName() );
    sal_Bool bVBAEnabled = aDocument.isInVBAMode();
    sal_uInt16 nMode = aBasicBox.GetMode();

    sal_uInt16 nDepth = pCurEntry ? aBasicBox.GetModel()->GetDepth( pCurEntry ) : 0;
    if ( nDepth >= 2 )
    {
        if( bVBAEnabled && ( nMode & BROWSEMODE_MODULES ) && ( nDepth == 2 ) )
            aEditButton.Disable();
        else
        aEditButton.Enable();
    }
    else
        aEditButton.Disable();

    // enable/disable new module/dialog buttons
    LibraryLocation eLocation( aDesc.GetLocation() );
    sal_Bool bReadOnly = sal_False;
    if ( nDepth > 0 )
    {
        Reference< script::XLibraryContainer2 > xModLibContainer( aDocument.getLibraryContainer( E_SCRIPTS ), UNO_QUERY );
        Reference< script::XLibraryContainer2 > xDlgLibContainer( aDocument.getLibraryContainer( E_DIALOGS ), UNO_QUERY );
        if ( ( xModLibContainer.is() && xModLibContainer->hasByName( aLibName ) && xModLibContainer->isLibraryReadOnly( aLibName ) ) ||
             ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aLibName ) && xDlgLibContainer->isLibraryReadOnly( aLibName ) ) )
        {
            bReadOnly = sal_True;
        }
    }
    if ( bReadOnly || eLocation == LIBRARY_LOCATION_SHARE )
    {
        aNewModButton.Disable();
        aNewDlgButton.Disable();
    }
    else
    {
        aNewModButton.Enable();
        aNewDlgButton.Enable();
    }

    // enable/disable delete button
    if ( nDepth >= 2 && !bReadOnly && eLocation != LIBRARY_LOCATION_SHARE )
    {
        if( bVBAEnabled && ( nMode & BROWSEMODE_MODULES ) && ( ( nDepth == 2 ) || aLibSubName == IDE_RESSTR(RID_STR_DOCUMENT_OBJECTS) ) )
            aDelButton.Disable();
        else
        aDelButton.Enable();
    }
    else
        aDelButton.Disable();
}

IMPL_LINK( ObjectPage, BasicBoxHighlightHdl, BasicTreeListBox *, pBox )
{
    if ( !pBox->IsSelected( pBox->GetHdlEntry() ) )
        return 0;

    CheckButtons();
    return 0;
}

IMPL_LINK( ObjectPage, ButtonHdl, Button *, pButton )
{
    if ( pButton == &aEditButton )
    {
        SfxAllItemSet aArgs( SFX_APP()->GetPool() );
        SfxRequest aRequest( SID_BASICIDE_APPEAR, SFX_CALLMODE_SYNCHRON, aArgs );
        SFX_APP()->ExecuteSlot( aRequest );

        BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
        SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
        SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        SvLBoxEntry* pCurEntry = aBasicBox.GetCurEntry();
        DBG_ASSERT( pCurEntry, "Entry?!" );
        if ( aBasicBox.GetModel()->GetDepth( pCurEntry ) >= 2 )
        {
            BasicEntryDescriptor aDesc( aBasicBox.GetEntryDescriptor( pCurEntry ) );
            if ( pDispatcher )
            {
                ::rtl::OUString aModName( aDesc.GetName() );
                // extract the module name from the string like "Sheet1 (Example1)"
                if( aDesc.GetLibSubName() == IDE_RESSTR(RID_STR_DOCUMENT_OBJECTS) )
                {
                    sal_Int32 nIndex = 0;
                    aModName = aModName.getToken( 0, ' ', nIndex );
                }
                SbxItem aSbxItem( SID_BASICIDE_ARG_SBX, aDesc.GetDocument(), aDesc.GetLibName(),
                                  aModName, aBasicBox.ConvertType( aDesc.GetType() ) );
                pDispatcher->Execute( SID_BASICIDE_SHOWSBX, SFX_CALLMODE_SYNCHRON, &aSbxItem, 0L );
            }
        }
        else    // Nur Lib selektiert
        {
            DBG_ASSERT( aBasicBox.GetModel()->GetDepth( pCurEntry ) == 1, "Kein LibEntry?!" );
            ScriptDocument aDocument( ScriptDocument::getApplicationScriptDocument() );
            SvLBoxEntry* pParentEntry = aBasicBox.GetParent( pCurEntry );
            if ( pParentEntry )
            {
                BasicDocumentEntry* pBasicDocumentEntry = (BasicDocumentEntry*)pParentEntry->GetUserData();
                if ( pBasicDocumentEntry )
                    aDocument = pBasicDocumentEntry->GetDocument();
            }
            SfxUsrAnyItem aDocItem( SID_BASICIDE_ARG_DOCUMENT_MODEL, makeAny( aDocument.getDocumentOrNull() ) );
            ::rtl::OUString aLibName( aBasicBox.GetEntryText( pCurEntry ) );
            SfxStringItem aLibNameItem( SID_BASICIDE_ARG_LIBNAME, aLibName );
            if ( pDispatcher )
            {
                pDispatcher->Execute( SID_BASICIDE_LIBSELECTED, SFX_CALLMODE_ASYNCHRON, &aDocItem, &aLibNameItem, 0L );
            }
        }
        EndTabDialog( 1 );
    }
    else if ( pButton == &aNewModButton )
        NewModule();
    else if ( pButton == &aNewDlgButton )
        NewDialog();
    else if ( pButton == &aDelButton )
        DeleteCurrent();
    else if ( pButton == &aCloseButton )
        EndTabDialog( 0 );

    return 0;
}

bool ObjectPage::GetSelection( ScriptDocument& rDocument, ::rtl::OUString& rLibName )
{
    bool bRet = false;

    SvLBoxEntry* pCurEntry = aBasicBox.GetCurEntry();
    BasicEntryDescriptor aDesc( aBasicBox.GetEntryDescriptor( pCurEntry ) );
    rDocument = aDesc.GetDocument();
    rLibName = aDesc.GetLibName();
    if ( rLibName.isEmpty() )
        rLibName = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Standard"));

    DBG_ASSERT( rDocument.isAlive(), "ObjectPage::GetSelection: no or dead ScriptDocument in the selection!" );
    if ( !rDocument.isAlive() )
        return false;

    // check if the module library is loaded
    sal_Bool bOK = sal_True;
    ::rtl::OUString aLibName( rLibName );
    Reference< script::XLibraryContainer > xModLibContainer( rDocument.getLibraryContainer( E_SCRIPTS  ) );
    if ( xModLibContainer.is() && xModLibContainer->hasByName( aLibName ) && !xModLibContainer->isLibraryLoaded( aLibName ) )
    {
        // check password
        Reference< script::XLibraryContainerPassword > xPasswd( xModLibContainer, UNO_QUERY );
        if ( xPasswd.is() && xPasswd->isLibraryPasswordProtected( aLibName ) && !xPasswd->isLibraryPasswordVerified( aLibName ) )
        {
            ::rtl::OUString aPassword;
            bOK = QueryPassword( xModLibContainer, rLibName, aPassword );
        }

        // load library
        if ( bOK )
            xModLibContainer->loadLibrary( aLibName );
    }

    // check if the dialog library is loaded
    Reference< script::XLibraryContainer > xDlgLibContainer( rDocument.getLibraryContainer( E_DIALOGS ) );
    if ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aLibName ) && !xDlgLibContainer->isLibraryLoaded( aLibName ) )
    {
        // load library
        if ( bOK )
            xDlgLibContainer->loadLibrary( aLibName );
    }

    if ( bOK )
        bRet = true;

    return bRet;
}

void ObjectPage::NewModule()
{
    ScriptDocument aDocument( ScriptDocument::getApplicationScriptDocument() );
    ::rtl::OUString aLibName;

    if ( GetSelection( aDocument, aLibName ) )
    {
        ::rtl::OUString aModName;
        createModImpl( static_cast<Window*>( this ), aDocument,
                    aBasicBox, aLibName, aModName, true );
    }
}

void ObjectPage::NewDialog()
{
    ScriptDocument aDocument( ScriptDocument::getApplicationScriptDocument() );
    ::rtl::OUString aLibName;

    if ( GetSelection( aDocument, aLibName ) )
    {
        aDocument.getOrCreateLibrary( E_DIALOGS, aLibName );

        boost::scoped_ptr< NewObjectDialog > xNewDlg(
            new NewObjectDialog(this, NEWOBJECTMODE_DLG, true));
        xNewDlg->SetObjectName( aDocument.createObjectName( E_DIALOGS, aLibName ) );

        if (xNewDlg->Execute() != 0)
        {
            ::rtl::OUString aDlgName( xNewDlg->GetObjectName() );
            if (aDlgName.isEmpty())
                aDlgName = aDocument.createObjectName( E_DIALOGS, aLibName);

            if ( aDocument.hasDialog( aLibName, aDlgName ) )
            {
                ErrorBox( this, WB_OK | WB_DEF_OK,
                          IDE_RESSTR(RID_STR_SBXNAMEALLREADYUSED2) ).Execute();
            }
            else
            {
                Reference< io::XInputStreamProvider > xISP;
                if ( !aDocument.createDialog( aLibName, aDlgName, xISP ) )
                    return;

                SbxItem aSbxItem( SID_BASICIDE_ARG_SBX, aDocument, aLibName, aDlgName, BASICIDE_TYPE_DIALOG );
                BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
                SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
                SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
                if( pDispatcher )
                {
                    pDispatcher->Execute( SID_BASICIDE_SBXINSERTED,
                                            SFX_CALLMODE_SYNCHRON, &aSbxItem, 0L );
                }
                LibraryLocation eLocation = aDocument.getLibraryLocation( aLibName );
                SvLBoxEntry* pRootEntry = aBasicBox.FindRootEntry( aDocument, eLocation );
                if ( pRootEntry )
                {
                    if ( !aBasicBox.IsExpanded( pRootEntry ) )
                        aBasicBox.Expand( pRootEntry );
                    SvLBoxEntry* pLibEntry = aBasicBox.FindEntry( pRootEntry, aLibName, OBJ_TYPE_LIBRARY );
                    DBG_ASSERT( pLibEntry, "Libeintrag nicht gefunden!" );
                    if ( pLibEntry )
                    {
                        if ( !aBasicBox.IsExpanded( pLibEntry ) )
                            aBasicBox.Expand( pLibEntry );
                        SvLBoxEntry* pEntry = aBasicBox.FindEntry( pLibEntry, aDlgName, OBJ_TYPE_DIALOG );
                        if ( !pEntry )
                        {
                            SAL_WNODEPRECATED_DECLARATIONS_PUSH
                            pEntry = aBasicBox.AddEntry(
                                aDlgName,
                                Image( IDEResId( RID_IMG_DIALOG ) ),
                                pLibEntry, false,
                                std::auto_ptr< BasicEntry >( new BasicEntry( OBJ_TYPE_DIALOG ) ) );
                            SAL_WNODEPRECATED_DECLARATIONS_POP
                            DBG_ASSERT( pEntry, "InsertEntry fehlgeschlagen!" );
                        }
                        aBasicBox.SetCurEntry( pEntry );
                        aBasicBox.Select( aBasicBox.GetCurEntry() );        // OV-Bug?!
                    }
                }
            }
        }
    }
}

void ObjectPage::DeleteCurrent()
{
    SvLBoxEntry* pCurEntry = aBasicBox.GetCurEntry();
    DBG_ASSERT( pCurEntry, "Kein aktueller Eintrag!" );
    BasicEntryDescriptor aDesc( aBasicBox.GetEntryDescriptor( pCurEntry ) );
    ScriptDocument aDocument( aDesc.GetDocument() );
    DBG_ASSERT( aDocument.isAlive(), "ObjectPage::DeleteCurrent: no document!" );
    if ( !aDocument.isAlive() )
        return;
    ::rtl::OUString aLibName( aDesc.GetLibName() );
    ::rtl::OUString aName( aDesc.GetName() );
    BasicEntryType eType( aDesc.GetType() );

    if ( ( eType == OBJ_TYPE_MODULE && QueryDelModule( aName, this ) ) ||
         ( eType == OBJ_TYPE_DIALOG && QueryDelDialog( aName, this ) ) )
    {
        aBasicBox.GetModel()->Remove( pCurEntry );
        if ( aBasicBox.GetCurEntry() )  // OV-Bug ?
            aBasicBox.Select( aBasicBox.GetCurEntry() );
        BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
        SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
        SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        if( pDispatcher )
        {
            SbxItem aSbxItem( SID_BASICIDE_ARG_SBX, aDocument, aLibName, aName, aBasicBox.ConvertType( eType ) );
            pDispatcher->Execute( SID_BASICIDE_SBXDELETED,
                                  SFX_CALLMODE_SYNCHRON, &aSbxItem, 0L );
        }

        try
        {
            bool bSuccess = false;
            if ( eType == OBJ_TYPE_MODULE )
                bSuccess = aDocument.removeModule( aLibName, aName );
            else if ( eType == OBJ_TYPE_DIALOG )
                bSuccess = BasicIDE::RemoveDialog( aDocument, aLibName, aName );

            if ( bSuccess )
                BasicIDE::MarkDocumentModified( aDocument );
        }
        catch (const container::NoSuchElementException& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }
}



void ObjectPage::EndTabDialog( sal_uInt16 nRet )
{
    DBG_ASSERT( pTabDlg, "TabDlg nicht gesetzt!" );
    if ( pTabDlg )
        pTabDlg->EndDialog( nRet );
}


LibDialog::LibDialog( Window* pParent )
    : ModalDialog( pParent, IDEResId( RID_DLG_LIBS ) ),
        aOKButton(      this, IDEResId( RID_PB_OK ) ),
        aCancelButton(  this, IDEResId( RID_PB_CANCEL ) ),
        aStorageName(   this, IDEResId( RID_FT_STORAGENAME ) ),
        aLibBox(        this, IDEResId( RID_CTRL_LIBS ) ),
        aFixedLine(     this, IDEResId( RID_FL_OPTIONS ) ),
        aReferenceBox(  this, IDEResId( RID_CB_REF ) ),
        aReplaceBox(    this, IDEResId( RID_CB_REPL ) )
{
    SetText( IDE_RESSTR(RID_STR_APPENDLIBS) );
    FreeResource();
}


LibDialog::~LibDialog()
{
}

void LibDialog::SetStorageName( const ::rtl::OUString& rName )
{
    ::rtl::OUString aName( IDE_RESSTR(RID_STR_FILENAME) );
    aName += rName;
    aStorageName.SetText( aName );
}

// Helper function
SbModule* createModImpl( Window* pWin, const ScriptDocument& rDocument,
    BasicTreeListBox& rBasicBox, const ::rtl::OUString& rLibName, ::rtl::OUString aModName, bool bMain )
{
    OSL_ENSURE( rDocument.isAlive(), "createModImpl: invalid document!" );
    if ( !rDocument.isAlive() )
        return NULL;

    SbModule* pModule = NULL;

    ::rtl::OUString aLibName( rLibName );
    if ( aLibName.isEmpty() )
        aLibName = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Standard"));
    rDocument.getOrCreateLibrary( E_SCRIPTS, aLibName );
    if ( aModName.isEmpty() )
        aModName = rDocument.createObjectName( E_SCRIPTS, aLibName );

    boost::scoped_ptr< NewObjectDialog > xNewDlg(
        new NewObjectDialog( pWin, NEWOBJECTMODE_MOD, true ) );
    xNewDlg->SetObjectName( aModName );

    if (xNewDlg->Execute() != 0)
    {
        if ( xNewDlg->GetObjectName().Len() )
            aModName = xNewDlg->GetObjectName();

        try
        {
            ::rtl::OUString sModuleCode;
            // the module has existed
            if( rDocument.hasModule( aLibName, aModName ) )
                return NULL;
            rDocument.createModule( aLibName, aModName, bMain, sModuleCode );
            BasicManager* pBasMgr = rDocument.getBasicManager();
            StarBASIC* pBasic = pBasMgr? pBasMgr->GetLib( aLibName ) : 0;
                if ( pBasic )
                    pModule = pBasic->FindModule( aModName );
                SbxItem aSbxItem( SID_BASICIDE_ARG_SBX, rDocument, aLibName, aModName, BASICIDE_TYPE_MODULE );
            BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
            SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
            SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
            if( pDispatcher )
            {
                pDispatcher->Execute( SID_BASICIDE_SBXINSERTED,
                                      SFX_CALLMODE_SYNCHRON, &aSbxItem, 0L );
            }
            LibraryLocation eLocation = rDocument.getLibraryLocation( aLibName );
            SvLBoxEntry* pRootEntry = rBasicBox.FindRootEntry( rDocument, eLocation );
            if ( pRootEntry )
            {
                if ( !rBasicBox.IsExpanded( pRootEntry ) )
                    rBasicBox.Expand( pRootEntry );
                SvLBoxEntry* pLibEntry = rBasicBox.FindEntry( pRootEntry, aLibName, OBJ_TYPE_LIBRARY );
                DBG_ASSERT( pLibEntry, "Libeintrag nicht gefunden!" );
                if ( pLibEntry )
                {
                    if ( !rBasicBox.IsExpanded( pLibEntry ) )
                        rBasicBox.Expand( pLibEntry );
                    SvLBoxEntry* pSubRootEntry = pLibEntry;
                    if( pBasic && rDocument.isInVBAMode() )
                    {
                        // add the new module in the "Modules" entry
                        SvLBoxEntry* pLibSubEntry = rBasicBox.FindEntry( pLibEntry, IDE_RESSTR(RID_STR_NORMAL_MODULES) , OBJ_TYPE_NORMAL_MODULES );
                        if( pLibSubEntry )
                        {
                            if( !rBasicBox.IsExpanded( pLibSubEntry ) )
                                rBasicBox.Expand( pLibSubEntry );
                            pSubRootEntry = pLibSubEntry;
                        }
                    }

                    SvLBoxEntry* pEntry = rBasicBox.FindEntry( pSubRootEntry, aModName, OBJ_TYPE_MODULE );
                    if ( !pEntry )
                    {
                        SAL_WNODEPRECATED_DECLARATIONS_PUSH
                        pEntry = rBasicBox.AddEntry(
                            aModName,
                            Image( IDEResId( RID_IMG_MODULE ) ),
                            pSubRootEntry, false,
                            std::auto_ptr< BasicEntry >( new BasicEntry( OBJ_TYPE_MODULE ) ) );
                        SAL_WNODEPRECATED_DECLARATIONS_POP
                        DBG_ASSERT( pEntry, "InsertEntry fehlgeschlagen!" );
                    }
                    rBasicBox.SetCurEntry( pEntry );
                    rBasicBox.Select( rBasicBox.GetCurEntry() );        // OV-Bug?!
                }
            }
        }
        catch (const container::ElementExistException& )
        {
            ErrorBox( pWin, WB_OK | WB_DEF_OK,
                      IDE_RESSTR(RID_STR_SBXNAMEALLREADYUSED2) ).Execute();
        }
        catch (const container::NoSuchElementException& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }
    return pModule;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
