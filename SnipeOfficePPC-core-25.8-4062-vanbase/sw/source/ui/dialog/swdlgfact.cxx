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

#include "swdlgfact.hxx"
#include <svl/style.hxx>
#include <svx/svxids.hrc>
#include "dialog.hrc"
#include "misc.hrc"
#include "chrdlg.hrc"
#include "table.hrc"
#include "frmui.hrc"
#include "dbui.hrc"
#include "globals.hrc"
#include "fldui.hrc"
#include "envelp.hrc"
#include "dochdl.hrc"
#include <index.hrc>
#include <regionsw.hrc>
#include <fmtui.hrc>

#include <wordcountdialog.hxx>
#include "abstract.hxx" // add for SwInsertAbstractDlg
#include "addrdlg.hxx" // add for SwAddrDlg
#include "ascfldlg.hxx" // add for SwAsciiFilterDlg
#include "bookmark.hxx" //add for SwInsertBookmarkDlg
#include "break.hxx" //add for SwBreakDlg
#include "changedb.hxx" //add for SwChangeDBDlg
#include "chrdlg.hxx" // add for SwCharDlg
#include "convert.hxx" //add for SwConvertTableDlg
#include "cption.hxx" //add for SwCaptionDialog
#include "dbinsdlg.hxx" //add for  SwInsertDBColAutoPilot
#include "docfnote.hxx" //add for SwFootNoteOptionDlg
#include "docstdlg.hxx" //add for SwDocStatPage
#include "DropDownFieldDialog.hxx" //add for DropDownFieldDialog
#include "envlop.hxx" //add for SwEnvDlg
#include "label.hxx" //add for SwLabDlg
#include "drpcps.hxx" //add for SwDropCapsDlg
#include "swuipardlg.hxx" //add for SwParaDlg
#include "pattern.hxx" //add for SwBackgroundDlg
#include "rowht.hxx" //add for SwTableHeightDlg
#include "selglos.hxx" //add for SwSelGlossaryDlg
#include "splittbl.hxx" //add for SwSplitTblDlg
#include "srtdlg.hxx" //add for SwSortDlg
#include "tautofmt.hxx" //add for SwAutoFormatDlg
#include "tblnumfm.hxx" //add for SwNumFmtDlg
#include "uiborder.hxx" //add for SwBorderDlg
#include "wrap.hxx" //add for SwWrapDlg
#include "colwd.hxx" //add for SwTableWidthDlg
#include "tabledlg.hxx" //add for SwTableTabDlg
#include "fldtdlg.hxx" //add for SwFldDlg
#include "fldedt.hxx" //add for SwFldEditDlg
#include "swrenamexnameddlg.hxx" //add for SwRenameXNamedDlg
#include "swmodalredlineacceptdlg.hxx" //add for SwModalRedlineAcceptDlg
#include <frmdlg.hxx> //add for SwFrmDlg
#include <tmpdlg.hxx> //add for SwTemplateDlg
#include <glossary.hxx> //add for SwGlossaryDlg
#include <inpdlg.hxx> //add for SwFldInputDlg
#include <insfnote.hxx> //add for SwInsFootNoteDlg
#include <instable.hxx> //add for SwInsTableDlg
#include <javaedit.hxx> //add for SwJavaEditDialog
#include <linenum.hxx> //add for SwLineNumberingDlg
#include <titlepage.hxx> //add for SwTitlePageDlg
#include <mailmrge.hxx> //add for SwMailMergeDlg, SwMailMergeCreateFromDlg, SwMailMergeFieldConnectionsDlg
#include <mergetbl.hxx> //add for SwMergeTblDlg
#include <multmrk.hxx> //add for SwMultiTOXMarkDlg
#include <num.hxx> //add for SwSvxNumBulletTabDialog
#include <outline.hxx> //add for SwOutlineTabDialog
#include <column.hxx> //add for SwColumnDlg
#include <cnttab.hxx> //add for SwMultiTOXTabDialog
#include <swuicnttab.hxx> //add for SwMultiTOXTabDialog
#include <regionsw.hxx> //add for SwEditRegionDlg, SwInsertSectionTabDialog
#include <optcomp.hxx> //add for SwCompatibilityOptPage
#include <optload.hxx> //add for SwLoadOptPage
#include <optpage.hxx> //add for OptPage
#include <swuiidxmrk.hxx> //add for SwIndexMarkDlg, SwAuthMarkDlg, SwIndexMarkModalDlg, SwAuthMarkModalDlg
#include <svx/dialogs.hrc>
#include <mailmergewizard.hxx>
#include <mailconfigpage.hxx>

using namespace ::com::sun::star;

IMPL_ABSTDLG_BASE(AbstractSwWordCountFloatDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSwInsertAbstractDlg_Impl);
IMPL_ABSTDLG_BASE(SwAbstractSfxDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractSwAsciiFilterDlg_Impl);
IMPL_ABSTDLG_BASE(VclAbstractDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractSplitTableDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractSwBreakDlg_Impl); //add for SwBreakDlg
IMPL_ABSTDLG_BASE(AbstractTabDialog_Impl); //add for SwCharDlg, SwFootNoteOptionDlg, SwEnvDlg  SwParaDlg SwTableTabDlg
IMPL_ABSTDLG_BASE(AbstractSwConvertTableDlg_Impl); //add for SwConvertTableDlg
IMPL_ABSTDLG_BASE(AbstractSwInsertDBColAutoPilot_Impl); //add for SwInsertDBColAutoPilot
IMPL_ABSTDLG_BASE(AbstractDropDownFieldDialog_Impl); //add for DropDownFieldDialog
IMPL_ABSTDLG_BASE(AbstractSwLabDlg_Impl);//add for SwLabDlg
IMPL_ABSTDLG_BASE(AbstractSwSelGlossaryDlg_Impl);//add for SwSelGlossaryDlg
IMPL_ABSTDLG_BASE(AbstractSwAutoFormatDlg_Impl); //add for SwAutoFormatDlg
IMPL_ABSTDLG_BASE(AbstractSwFldDlg_Impl); //add for SwFldDlg
IMPL_ABSTDLG_BASE(AbstractSwRenameXNamedDlg_Impl); //add for SwRenameXNamedDlg
IMPL_ABSTDLG_BASE(AbstractSwModalRedlineAcceptDlg_Impl); //add for SwModalRedlineAcceptDlg
IMPL_ABSTDLG_BASE(AbstractGlossaryDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractFldInputDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractInsFootNoteDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractInsTableDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractJavaEditDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractMailMergeDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractMailMergeCreateFromDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractMailMergeFieldConnectionsDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractMultiTOXTabDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractEditRegionDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractInsertSectionTabDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractIndexMarkFloatDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractAuthMarkFloatDlg_Impl);

void AbstractTabDialog_Impl::SetCurPageId( sal_uInt16 nId )
{
    pDlg->SetCurPageId( nId );
}

const SfxItemSet* AbstractTabDialog_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}

const sal_uInt16* AbstractTabDialog_Impl::GetInputRanges(const SfxItemPool& pItem )
{
    return pDlg->GetInputRanges( pItem );
}

void AbstractTabDialog_Impl::SetInputSet( const SfxItemSet* pInSet )
{
     pDlg->SetInputSet( pInSet );
}

//From class Window.
void AbstractTabDialog_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}

String AbstractTabDialog_Impl::GetText() const
{
    return pDlg->GetText();
}

IMPL_LINK_NOARG(AbstractApplyTabDialog_Impl, ApplyHdl)
{
    if (pDlg->Apply())
        m_aHandler.Call(NULL);
    return 0;
}

void AbstractApplyTabDialog_Impl::SetApplyHdl( const Link& rLink )
{
    m_aHandler = rLink;
    pDlg->SetApplyHandler(LINK(this, AbstractApplyTabDialog_Impl, ApplyHdl));
}

sal_uInt8 AbstractSwInsertAbstractDlg_Impl::GetLevel() const
{
    return pDlg->GetLevel();
}
sal_uInt8 AbstractSwInsertAbstractDlg_Impl::GetPara() const
{
    return pDlg->GetPara();
}

//add for SwAddrDlg, SwDropCapsDlg ,SwBackgroundDlg, SwNumFmtDlg SwBorderDlg SwWrapDlg  SwFldEditDlg begin
const SfxItemSet* SwAbstractSfxDialog_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}

void SwAbstractSfxDialog_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}

String SwAbstractSfxDialog_Impl::GetText() const
{
    return pDlg->GetText();
}

void AbstractSwAsciiFilterDlg_Impl::FillOptions( SwAsciiOptions& rOptions )
{
    pDlg->FillOptions(rOptions);
}

sal_uInt16 AbstractSplitTableDialog_Impl::GetSplitMode()
{
    return pDlg->GetSplitMode();
}

String AbstractSwBreakDlg_Impl::GetTemplateName()
{
    return pDlg->GetTemplateName();
}

sal_uInt16 AbstractSwBreakDlg_Impl:: GetKind()
{
    return pDlg->GetKind();
}

sal_uInt16 AbstractSwBreakDlg_Impl:: GetPageNumber()
{
    return pDlg->GetPageNumber();
}

void AbstractSwConvertTableDlg_Impl::GetValues( sal_Unicode& rDelim,SwInsertTableOptions& rInsTblFlags,
                                                SwTableAutoFmt const*& prTAFmt)
{
    pDlg->GetValues(rDelim,rInsTblFlags, prTAFmt);
}

void AbstractSwInsertDBColAutoPilot_Impl::DataToDoc( const uno::Sequence< uno::Any >& rSelection,
        uno::Reference< sdbc::XDataSource> rxSource,
        uno::Reference< sdbc::XConnection> xConnection,
        uno::Reference< sdbc::XResultSet > xResultSet)
{
    pDlg->DataToDoc(rSelection, rxSource, xConnection, xResultSet);
}

rtl::OString AbstractDropDownFieldDialog_Impl::GetWindowState( sal_uLong nMask  ) const
{
    return pDlg->GetWindowState(nMask);
}

void AbstractDropDownFieldDialog_Impl::SetWindowState( const rtl::OString& rStr )
{
    pDlg->SetWindowState(rStr);
}

void AbstractSwLabDlg_Impl::SetCurPageId( sal_uInt16 nId )
{
    pDlg->SetCurPageId( nId );
}

const SfxItemSet* AbstractSwLabDlg_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}

const sal_uInt16* AbstractSwLabDlg_Impl::GetInputRanges(const SfxItemPool& pItem )
{
    return pDlg->GetInputRanges( pItem );
}

void AbstractSwLabDlg_Impl::SetInputSet( const SfxItemSet* pInSet )
{
     pDlg->SetInputSet( pInSet );
}

void AbstractSwLabDlg_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}

String AbstractSwLabDlg_Impl::GetText() const
{
    return pDlg->GetText();
}

const String& AbstractSwLabDlg_Impl::GetBusinessCardStr() const
{
    return pDlg->GetBusinessCardStr();
}

Printer * AbstractSwLabDlg_Impl::GetPrt()
{
    return pDlg->GetPrt();
}

void AbstractSwSelGlossaryDlg_Impl::InsertGlos(const String &rRegion, const String &rGlosName)
{
    pDlg->InsertGlos( rRegion, rGlosName );
}

sal_uInt16 AbstractSwSelGlossaryDlg_Impl::GetSelectedIdx() const
{
    return pDlg->GetSelectedIdx();
}

void AbstractSwSelGlossaryDlg_Impl::SelectEntryPos(sal_uInt16 nIdx)
{
    pDlg->SelectEntryPos( nIdx );
}

void AbstractSwAutoFormatDlg_Impl::FillAutoFmtOfIndex( SwTableAutoFmt*& rToFill ) const
{
    pDlg->FillAutoFmtOfIndex(rToFill);
}

void AbstractSwFldDlg_Impl::SetCurPageId( sal_uInt16 nId )
{
    pDlg->SetCurPageId( nId );
}

const SfxItemSet* AbstractSwFldDlg_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}

const sal_uInt16* AbstractSwFldDlg_Impl::GetInputRanges(const SfxItemPool& pItem )
{
    return pDlg->GetInputRanges( pItem );
}

void AbstractSwFldDlg_Impl::SetInputSet( const SfxItemSet* pInSet )
{
     pDlg->SetInputSet( pInSet );
}

void AbstractSwFldDlg_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}

String AbstractSwFldDlg_Impl::GetText() const
{
    return pDlg->GetText();
}

void AbstractSwFldDlg_Impl::Start( sal_Bool bShowl )
{
    pDlg->Start( bShowl );
}

void AbstractSwFldDlg_Impl::Initialize(SfxChildWinInfo *pInfo)
{
    pDlg->Initialize( pInfo );
}

void AbstractSwFldDlg_Impl::ReInitDlg()
{
    pDlg->ReInitDlg();
}

void AbstractSwFldDlg_Impl::ActivateDatabasePage()
{
    pDlg->ActivateDatabasePage();
}

Window* AbstractSwFldDlg_Impl::GetWindow()
{
    return (Window*)pDlg;
}
void    AbstractSwFldDlg_Impl::ShowPage( sal_uInt16 nId )
{
    pDlg->ShowPage(nId);
}

void AbstractSwRenameXNamedDlg_Impl::SetForbiddenChars( const String& rSet )
{
    pDlg->SetForbiddenChars( rSet );
}

void    AbstractSwRenameXNamedDlg_Impl::SetAlternativeAccess(
             ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > & xSecond,
             ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > & xThird )
{
    pDlg->SetAlternativeAccess( xSecond, xThird);
}

void    AbstractSwModalRedlineAcceptDlg_Impl::AcceptAll( sal_Bool bAccept )
{
    pDlg->AcceptAll( bAccept);
}

String AbstractGlossaryDlg_Impl::GetCurrGrpName() const
{
    return pDlg->GetCurrGrpName();
}

String AbstractGlossaryDlg_Impl::GetCurrShortName() const
{
    return pDlg->GetCurrShortName();
}

void AbstractFldInputDlg_Impl::SetWindowState( const rtl::OString& rStr )
{
    pDlg->SetWindowState( rStr );
}

rtl::OString AbstractFldInputDlg_Impl::GetWindowState( sal_uLong nMask ) const
{
    return pDlg->GetWindowState( nMask );
}

String AbstractInsFootNoteDlg_Impl::GetFontName()
{
    return pDlg->GetFontName();
}
sal_Bool AbstractInsFootNoteDlg_Impl::IsEndNote()
{
    return pDlg->IsEndNote();
}

String AbstractInsFootNoteDlg_Impl::GetStr()
{
    return pDlg->GetStr();
}
void AbstractInsFootNoteDlg_Impl::SetHelpId( const rtl::OString& sHelpId )
{
    pDlg->SetHelpId( sHelpId );
}

void AbstractInsFootNoteDlg_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}

void AbstractInsTableDlg_Impl::GetValues( String& rName, sal_uInt16& rRow, sal_uInt16& rCol,
                                SwInsertTableOptions& rInsTblFlags, String& rTableAutoFmtName,
                                SwTableAutoFmt *& prTAFmt )
{
    pDlg->GetValues( rName, rRow, rCol, rInsTblFlags, rTableAutoFmtName, prTAFmt);
}

String AbstractJavaEditDialog_Impl::GetText()
{
    return pDlg->GetText();
}

String AbstractJavaEditDialog_Impl::GetType()
{
    return pDlg->GetType();
}
sal_Bool AbstractJavaEditDialog_Impl::IsUrl()
{
    return pDlg->IsUrl();
}
sal_Bool AbstractJavaEditDialog_Impl::IsNew()
{
    return pDlg->IsNew();
}
sal_Bool AbstractJavaEditDialog_Impl::IsUpdate()
{
    return pDlg->IsUpdate();
}

sal_uInt16 AbstractMailMergeDlg_Impl::GetMergeType()
{
    return pDlg->GetMergeType();
}

const ::rtl::OUString& AbstractMailMergeDlg_Impl::GetSaveFilter() const
{
    return pDlg->GetSaveFilter();
}

const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > AbstractMailMergeDlg_Impl::GetSelection() const
{
    return pDlg->GetSelection();
}

uno::Reference< sdbc::XResultSet> AbstractMailMergeDlg_Impl::GetResultSet() const
{
    return pDlg->GetResultSet();
}
bool AbstractMailMergeDlg_Impl::IsSaveIndividualDocs() const
{
    return pDlg->IsSaveIndividualDocs();
}
bool AbstractMailMergeDlg_Impl::IsGenerateFromDataBase() const
{
    return pDlg->IsGenerateFromDataBase();
}
String AbstractMailMergeDlg_Impl::GetColumnName() const
{
    return pDlg->GetColumnName();
}
String AbstractMailMergeDlg_Impl::GetPath() const
{
    return pDlg->GetPath();
}

sal_Bool AbstractMailMergeCreateFromDlg_Impl::IsThisDocument() const
{
    return pDlg->IsThisDocument();
}

sal_Bool AbstractMailMergeFieldConnectionsDlg_Impl::IsUseExistingConnections() const
{
    return pDlg->IsUseExistingConnections();
}

SwForm* AbstractMultiTOXTabDialog_Impl::GetForm(CurTOXType eType)
{
    return pDlg->GetForm(eType);
}

CurTOXType AbstractMultiTOXTabDialog_Impl::GetCurrentTOXType() const
{
    return pDlg->GetCurrentTOXType();
}

SwTOXDescription& AbstractMultiTOXTabDialog_Impl::GetTOXDescription(CurTOXType eTOXTypes)
{
    return pDlg->GetTOXDescription(eTOXTypes);
}

const SfxItemSet* AbstractMultiTOXTabDialog_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}

void AbstractEditRegionDlg_Impl::SelectSection(const String& rSectionName)
{
    pDlg->SelectSection(rSectionName);
}

void
AbstractInsertSectionTabDialog_Impl::SetSectionData(SwSectionData const& rSect)
{
    pDlg->SetSectionData(rSect);
}

void AbstractIndexMarkFloatDlg_Impl::ReInitDlg(SwWrtShell& rWrtShell)
{
    pDlg->ReInitDlg( rWrtShell);
}

Window* AbstractIndexMarkFloatDlg_Impl::GetWindow()
{
    return (Window*)pDlg;
}

void AbstractAuthMarkFloatDlg_Impl::ReInitDlg(SwWrtShell& rWrtShell)
{
    pDlg->ReInitDlg( rWrtShell);
}

Window* AbstractAuthMarkFloatDlg_Impl::GetWindow()
{
    return (Window*)pDlg;
}


Window* AbstractSwWordCountFloatDlg_Impl::GetWindow()
{
    return (Window*)pDlg;
}

void AbstractSwWordCountFloatDlg_Impl::UpdateCounts()
{
    pDlg->UpdateCounts();
}

void AbstractSwWordCountFloatDlg_Impl::SetCounts(const SwDocStat &rCurrCnt, const SwDocStat &rDocStat)
{
    pDlg->SetCounts(rCurrCnt, rDocStat);
}

AbstractMailMergeWizard_Impl::~AbstractMailMergeWizard_Impl()
{
    delete pDlg;
}

void AbstractMailMergeWizard_Impl::StartExecuteModal( const Link& rEndDialogHdl )
{
    aEndDlgHdl = rEndDialogHdl;
    pDlg->StartExecuteModal(
        LINK( this, AbstractMailMergeWizard_Impl, EndDialogHdl ) );
}

long AbstractMailMergeWizard_Impl::GetResult()
{
    return pDlg->GetResult();
}

IMPL_LINK( AbstractMailMergeWizard_Impl, EndDialogHdl, SwMailMergeWizard*, pDialog )
{
    OSL_ENSURE( pDialog == pDlg, "wrong dialog passed to EndDialogHdl!" );
    (void) pDialog; // unused in non-debug

    aEndDlgHdl.Call( this );
    aEndDlgHdl = Link();

    return 0L;
}

void AbstractMailMergeWizard_Impl::SetReloadDocument(const String& rURL)
{
    pDlg->SetReloadDocument(rURL);
}

const String&       AbstractMailMergeWizard_Impl::GetReloadDocument() const
{
    return pDlg->GetReloadDocument();
}
sal_Bool AbstractMailMergeWizard_Impl::ShowPage( sal_uInt16 nLevel )
{
    return pDlg->skipUntil(nLevel);
}

sal_uInt16 AbstractMailMergeWizard_Impl::GetRestartPage() const
{
    return pDlg->GetRestartPage();
}

AbstractSwInsertAbstractDlg * SwAbstractDialogFactory_Impl::CreateSwInsertAbstractDlg(Window* pParent)
{
    SwInsertAbstractDlg* pDlg = new SwInsertAbstractDlg( pParent);
    return new AbstractSwInsertAbstractDlg_Impl(pDlg);
}

SfxAbstractDialog* SwAbstractDialogFactory_Impl::CreateSfxDialog( Window* pParent,
                                                                        const SfxItemSet& rSet,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >&,
                                                                        sal_uInt32 nResId
                                                                )
{
    SfxModalDialog* pDlg=NULL;
    switch ( nResId )
    {
        case RC_DLG_ADDR :
            pDlg = new SwAddrDlg( pParent, rSet );
            break;
        case DLG_SWDROPCAPS :
            pDlg = new SwDropCapsDlg( pParent, rSet );
            break;
        case RC_SWDLG_BACKGROUND :
            pDlg = new SwBackgroundDlg( pParent, rSet );
            break;
        case RC_DLG_SWNUMFMTDLG :
            pDlg = new SwNumFmtDlg( pParent, rSet );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new SwAbstractSfxDialog_Impl( pDlg );
    return 0;
}

AbstractSwAsciiFilterDlg* SwAbstractDialogFactory_Impl::CreateSwAsciiFilterDlg( Window* pParent,
                                                                               SwDocShell& rDocSh,
                                                                                SvStream* pStream,
                                                                                int nResId )
{
    SwAsciiFilterDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_ASCII_FILTER :
            pDlg = new SwAsciiFilterDlg( pParent, rDocSh, pStream );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwAsciiFilterDlg_Impl( pDlg );
    return 0;
}

VclAbstractDialog* SwAbstractDialogFactory_Impl::CreateSwInsertBookmarkDlg( Window *pParent,
                                                                           SwWrtShell &rSh,
                                                                           SfxRequest& rReq,
                                                                           int nResId )
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INSERT_BOOKMARK :
            pDlg = new SwInsertBookmarkDlg( pParent, rSh, rReq );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}

AbstractSwBreakDlg * SwAbstractDialogFactory_Impl::CreateSwBreakDlg(Window *pParent,
                                                                     SwWrtShell &rSh)
{
    SwBreakDlg* pDlg = new SwBreakDlg(pParent, rSh);
    return new AbstractSwBreakDlg_Impl(pDlg);
}

VclAbstractDialog   * SwAbstractDialogFactory_Impl::CreateSwChangeDBDlg( SwView& rVw, int nResId  )
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_CHANGE_DB :
            pDlg = new SwChangeDBDlg( rVw );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}


SfxAbstractTabDialog *  SwAbstractDialogFactory_Impl::CreateSwCharDlg(Window* pParent, SwView& pVw, const SfxItemSet& rCoreSet, int nResId, // add for SwCharDlg
                                                const String* pFmtStr , sal_Bool bIsDrwTxtDlg )
{

    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_CHAR :
            pDlg = new SwCharDlg( pParent, pVw, rCoreSet, pFmtStr, bIsDrwTxtDlg );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;

}

AbstractSwConvertTableDlg* SwAbstractDialogFactory_Impl::CreateSwConvertTableDlg(SwView& rView, bool bToTable)
{
    SwConvertTableDlg* pDlg = new SwConvertTableDlg(rView, bToTable);
    return new AbstractSwConvertTableDlg_Impl(pDlg);
}

VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateSwCaptionDialog ( Window *pParent, SwView &rV,int nResId)
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_CAPTION :
            pDlg = new SwCaptionDialog( pParent, rV );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}

AbstractSwInsertDBColAutoPilot* SwAbstractDialogFactory_Impl::CreateSwInsertDBColAutoPilot( SwView& rView, // add for SwInsertDBColAutoPilot
        uno::Reference< sdbc::XDataSource> rxSource,
        uno::Reference<sdbcx::XColumnsSupplier> xColSupp,
        const SwDBData& rData,  int nResId)
{
    SwInsertDBColAutoPilot* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_AP_INSERT_DB_SEL :
            pDlg = new SwInsertDBColAutoPilot( rView, rxSource, xColSupp, rData );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwInsertDBColAutoPilot_Impl( pDlg );
    return 0;
}

SfxAbstractTabDialog *  SwAbstractDialogFactory_Impl::CreateSwFootNoteOptionDlg( Window *pParent, SwWrtShell &rSh,int nResId)
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_DOC_FOOTNOTE :
            pDlg = new SwFootNoteOptionDlg( pParent, rSh );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}

AbstractDropDownFieldDialog *  SwAbstractDialogFactory_Impl::CreateDropDownFieldDialog ( Window *pParent, SwWrtShell &rSh, //add for DropDownFieldDialog
                                SwField* pField,int nResId, sal_Bool bNextButton )
{
    sw::DropDownFieldDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_FLD_DROPDOWN :
            pDlg = new sw::DropDownFieldDialog( pParent, rSh, pField, bNextButton );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractDropDownFieldDialog_Impl( pDlg );
    return 0;
}

SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateSwEnvDlg ( Window* pParent, const SfxItemSet& rSet,
                                                                 SwWrtShell* pWrtSh, Printer* pPrt,
                                                                 sal_Bool bInsert,int nResId ) //add for SwEnvDlg
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_ENV :
            pDlg = new SwEnvDlg( pParent, rSet, pWrtSh,pPrt, bInsert  );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}

AbstractSwLabDlg* SwAbstractDialogFactory_Impl::CreateSwLabDlg ( Window* pParent, const SfxItemSet& rSet, //add for SwLabDlg
                                                     SwNewDBMgr* pNewDBMgr, sal_Bool bLabel,int nResId  )
{
    SwLabDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_LAB :
            pDlg = new SwLabDlg( pParent, rSet, pNewDBMgr,bLabel  );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwLabDlg_Impl( pDlg );
    return 0;
}

SwLabDlgMethod SwAbstractDialogFactory_Impl::GetSwLabDlgStaticMethod ()
{
    return SwLabDlg::UpdateFieldInformation;
}


SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateSwParaDlg ( Window *pParent, SwView& rVw,
                                                    const SfxItemSet& rCoreSet  ,
                                                    sal_uInt8 nDialogMode,
                                                    int nResId,
                                                    const String *pCollName,
                                                    sal_Bool bDraw , sal_uInt16 nDefPage)
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_DRAWPARA :
        case DLG_PARA :
            pDlg = new SwParaDlg( pParent, rVw, rCoreSet,nDialogMode, pCollName, bDraw, nDefPage );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}

VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateVclAbstractDialog ( Window *pParent, SwWrtShell &rSh, int nResId )
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_ROW_HEIGHT :
            pDlg = new SwTableHeightDlg( pParent, rSh);
            break;
        case DLG_SORTING :
            pDlg = new SwSortDlg( pParent, rSh);
            break;
        case DLG_COLUMN :
            pDlg = new SwColumnDlg( pParent, rSh );
            break;
        case DLG_EDIT_AUTHMARK :
            pDlg = new SwAuthMarkModalDlg( pParent, rSh );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}

AbstractSplitTableDialog * SwAbstractDialogFactory_Impl::CreateSplitTblDialog ( Window *pParent, SwWrtShell &rSh )
{
    return new AbstractSplitTableDialog_Impl( new SwSplitTblDlg( pParent, rSh) );
}

AbstractSwSelGlossaryDlg * SwAbstractDialogFactory_Impl::CreateSwSelGlossaryDlg ( Window * pParent, const String &rShortName, int nResId )
{
    SwSelGlossaryDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_SEL_GLOS :
            pDlg = new SwSelGlossaryDlg( pParent, rShortName);
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwSelGlossaryDlg_Impl( pDlg );
    return 0;
}

AbstractSwAutoFormatDlg * SwAbstractDialogFactory_Impl::CreateSwAutoFormatDlg(Window* pParent,
    SwWrtShell* pShell, sal_Bool bSetAutoFmt, const SwTableAutoFmt* pSelFmt)
{
    SwAutoFormatDlg* pDlg = new SwAutoFormatDlg(pParent, pShell, bSetAutoFmt, pSelFmt);
    return new AbstractSwAutoFormatDlg_Impl(pDlg);
}

SfxAbstractDialog * SwAbstractDialogFactory_Impl::CreateSwBorderDlg (Window* pParent, SfxItemSet& rSet, sal_uInt16 nType,int nResId )
{
    SfxModalDialog* pDlg=NULL;
    switch ( nResId )
    {
        case RC_DLG_SWBORDERDLG :
            pDlg = new SwBorderDlg( pParent, rSet, nType );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new SwAbstractSfxDialog_Impl( pDlg );
    return 0;
}

SfxAbstractDialog* SwAbstractDialogFactory_Impl::CreateSwWrapDlg ( Window* pParent, SfxItemSet& rSet, SwWrtShell* pSh, sal_Bool bDrawMode, int nResId )
{
    SfxModalDialog* pDlg=NULL;
    switch ( nResId )
    {
        case RC_DLG_SWWRAPDLG :
            pDlg = new SwWrapDlg( pParent, rSet, pSh, bDrawMode );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new SwAbstractSfxDialog_Impl( pDlg );
    return 0;
}

VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateSwTableWidthDlg(Window *pParent, SwTableFUNC &rFnc)
{
    Dialog* pDlg = new SwTableWidthDlg(pParent, rFnc);
    return new VclAbstractDialog_Impl( pDlg );
}

SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateSwTableTabDlg( Window* pParent, SfxItemPool& Pool,
                                                        const SfxItemSet* pItemSet, SwWrtShell* pSh,int nResId )
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_FORMAT_TABLE :
            pDlg = new SwTableTabDlg( pParent, Pool, pItemSet,pSh );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}

AbstractSwFldDlg * SwAbstractDialogFactory_Impl::CreateSwFldDlg ( SfxBindings* pB, SwChildWinWrapper* pCW, Window *pParent, int nResId )
{
    SwFldDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_FLD_INSERT :
            pDlg = new SwFldDlg( pB, pCW,pParent);
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwFldDlg_Impl( pDlg );
    return 0;
}

SfxAbstractDialog*   SwAbstractDialogFactory_Impl::CreateSwFldEditDlg ( SwView& rVw, int nResId )
{
    SfxModalDialog* pDlg=NULL;
    switch ( nResId )
    {
        case RC_DLG_SWFLDEDITDLG :
            pDlg = new SwFldEditDlg( rVw );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new SwAbstractSfxDialog_Impl( pDlg );
    return 0;
}

AbstractSwRenameXNamedDlg * SwAbstractDialogFactory_Impl::CreateSwRenameXNamedDlg( Window* pParent,
                                                                ::com::sun::star::uno::Reference< ::com::sun::star::container::XNamed > & xNamed,
                                                                ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > & xNameAccess,int nResId )
{
    SwRenameXNamedDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_RENAME_XNAMED :
            pDlg = new SwRenameXNamedDlg( pParent,xNamed, xNameAccess);
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwRenameXNamedDlg_Impl( pDlg );
    return 0;
}

AbstractSwModalRedlineAcceptDlg * SwAbstractDialogFactory_Impl::CreateSwModalRedlineAcceptDlg ( Window *pParent, int nResId )
{
    SwModalRedlineAcceptDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MOD_REDLINE_ACCEPT :
            pDlg = new SwModalRedlineAcceptDlg( pParent );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwModalRedlineAcceptDlg_Impl( pDlg );
    return 0;
}

VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateSwVclDialog( int nResId,
                                                Window* pParent, sal_Bool& rWithPrev ) //add for SwMergeTblDlg
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MERGE_TABLE :
            pDlg = new SwMergeTblDlg( pParent, rWithPrev );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}

SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateFrmTabDialog( int nResId,
                                                SfxViewFrame *pFrame, Window *pParent,
                                                const SfxItemSet& rCoreSet,
                                                sal_Bool            bNewFrm,
                                                sal_uInt16          nResType,
                                                sal_Bool            bFmt,
                                                sal_uInt16          nDefPage,
                                                const String*   pFmtStr ) //add for SwFrmDlg
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_FRM_GRF :
        case DLG_FRM_OLE :
        case DLG_FRM_STD :
            pDlg = new SwFrmDlg( pFrame, pParent, rCoreSet, bNewFrm, nResType, bFmt, nDefPage, pFmtStr );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}

SfxAbstractApplyTabDialog* SwAbstractDialogFactory_Impl::CreateTemplateDialog(
                                                Window*             pParent,
                                                SfxStyleSheetBase&  rBase,
                                                sal_uInt16              nRegion,
                                                sal_uInt16              nPageId,
                                                SwWrtShell*         pActShell,
                                                sal_Bool                bNew ) //add for SwTemplateDlg
{
    SfxTabDialog* pDlg = new SwTemplateDlg( pParent, rBase, nRegion, nPageId, pActShell, bNew );
    return new AbstractApplyTabDialog_Impl( pDlg );
}

AbstractGlossaryDlg* SwAbstractDialogFactory_Impl::CreateGlossaryDlg( int nResId,
                                                SfxViewFrame* pViewFrame,
                                                SwGlossaryHdl* pGlosHdl,
                                                SwWrtShell *pWrtShell) //add for SwGlossaryDlg
{
    SwGlossaryDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_RENAME_GLOS :
            pDlg = new SwGlossaryDlg( pViewFrame, pGlosHdl, pWrtShell );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractGlossaryDlg_Impl( pDlg );
    return 0;
}

AbstractFldInputDlg* SwAbstractDialogFactory_Impl::CreateFldInputDlg( int nResId,
                                                Window *pParent, SwWrtShell &rSh,
                                                SwField* pField, sal_Bool bNextButton ) //add for SwFldInputDlg
{
    SwFldInputDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_FLD_INPUT :
            pDlg = new SwFldInputDlg( pParent, rSh, pField, bNextButton );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractFldInputDlg_Impl( pDlg );
    return 0;
}

AbstractInsFootNoteDlg* SwAbstractDialogFactory_Impl::CreateInsFootNoteDlg(
    Window * pParent, SwWrtShell &rSh, sal_Bool bEd ) //add for SwInsFootNoteDlg
{
    SwInsFootNoteDlg* pDlg = new SwInsFootNoteDlg(pParent, rSh, bEd);
    return new AbstractInsFootNoteDlg_Impl( pDlg );
}

VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateTitlePageDlg ( Window *pParent )
{
    Dialog* pDlg = new SwTitlePageDlg( pParent );
    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}

VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateVclSwViewDialog( int nResId,
                                            SwView& rView, sal_Bool /*bCol*/ ) //add for SwInsRowColDlg, SwLineNumberingDlg
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_LINE_NUMBERING :
            pDlg = new SwLineNumberingDlg( &rView );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}

AbstractInsTableDlg * SwAbstractDialogFactory_Impl::CreateInsTableDlg(SwView& rView) //add for SwInsTableDlg
{
    SwInsTableDlg* pDlg = new SwInsTableDlg(rView);
    return new AbstractInsTableDlg_Impl( pDlg );
}

AbstractJavaEditDialog * SwAbstractDialogFactory_Impl::CreateJavaEditDialog(
    Window* pParent, SwWrtShell* pWrtSh) //add for SwJavaEditDialog
{
    SwJavaEditDialog* pDlg = new SwJavaEditDialog(pParent, pWrtSh);
    return new AbstractJavaEditDialog_Impl( pDlg );
}

AbstractMailMergeDlg * SwAbstractDialogFactory_Impl::CreateMailMergeDlg( int nResId,
                                                Window* pParent, SwWrtShell& rSh,
                                                 const String& rSourceName,
                                                const String& rTblName,
                                                sal_Int32 nCommandType,
                                                const uno::Reference< sdbc::XConnection>& xConnection,
                                                uno::Sequence< uno::Any >* pSelection ) //add for SwMailMergeDlg
{
    SwMailMergeDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MAILMERGE :
            pDlg = new SwMailMergeDlg( pParent, rSh, rSourceName, rTblName, nCommandType, xConnection, pSelection );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractMailMergeDlg_Impl( pDlg );
    return 0;
}

AbstractMailMergeCreateFromDlg * SwAbstractDialogFactory_Impl::CreateMailMergeCreateFromDlg( int nResId,
                                                                    Window* pParent ) //add for SwMailMergeCreateFromDlg
{
    SwMailMergeCreateFromDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MERGE_CREATE :
            pDlg = new SwMailMergeCreateFromDlg( pParent );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractMailMergeCreateFromDlg_Impl( pDlg );
    return 0;
}

AbstractMailMergeFieldConnectionsDlg * SwAbstractDialogFactory_Impl::CreateMailMergeFieldConnectionsDlg( int nResId,
                                                            Window* pParent ) //add for SwMailMergeFieldConnectionsDlg
{
    SwMailMergeFieldConnectionsDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MERGE_FIELD_CONNECTIONS :
            pDlg = new SwMailMergeFieldConnectionsDlg( pParent );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractMailMergeFieldConnectionsDlg_Impl( pDlg );
    return 0;
}

VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateMultiTOXMarkDlg( int nResId,
                                                Window* pParent, SwTOXMgr &rTOXMgr ) //add for SwMultiTOXMarkDlg
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MULTMRK :
            pDlg = new SwMultiTOXMarkDlg( pParent, rTOXMgr );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}

SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateSwTabDialog( int nResId,
                                                Window* pParent,
                                                const SfxItemSet* pSwItemSet,
                                                SwWrtShell & rWrtSh ) //add for SwSvxNumBulletTabDialog, SwOutlineTabDialog
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_SVXTEST_NUM_BULLET :
            pDlg = new SwSvxNumBulletTabDialog( pParent, pSwItemSet, rWrtSh );
            break;
        case DLG_TAB_OUTLINE :
            pDlg = new SwOutlineTabDialog( pParent, pSwItemSet, rWrtSh );
            break;

        default:
            break;
    }
    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}

AbstractMultiTOXTabDialog * SwAbstractDialogFactory_Impl::CreateMultiTOXTabDialog( int nResId,
                                                Window* pParent, const SfxItemSet& rSet,
                                                SwWrtShell &rShell,
                                                SwTOXBase* pCurTOX, sal_uInt16 nToxType,
                                                sal_Bool bGlobal ) //add for SwMultiTOXTabDialog
{
    SwMultiTOXTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MULTI_TOX :
            pDlg = new SwMultiTOXTabDialog( pParent, rSet, rShell, pCurTOX, nToxType, bGlobal );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractMultiTOXTabDialog_Impl( pDlg );
    return 0;
}

AbstractEditRegionDlg * SwAbstractDialogFactory_Impl::CreateEditRegionDlg( int nResId,
                                                Window* pParent, SwWrtShell& rWrtSh ) //add for SwEditRegionDlg
{
    SwEditRegionDlg* pDlg=NULL;
    switch ( nResId )
    {
        case MD_EDIT_REGION :
            pDlg = new SwEditRegionDlg( pParent, rWrtSh );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractEditRegionDlg_Impl( pDlg );
    return 0;
}

AbstractInsertSectionTabDialog * SwAbstractDialogFactory_Impl::CreateInsertSectionTabDialog( int nResId,
                                                    Window* pParent, const SfxItemSet& rSet, SwWrtShell& rSh) //add for SwInsertSectionTabDialog
{
    SwInsertSectionTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INSERT_SECTION :
            pDlg = new SwInsertSectionTabDialog( pParent, rSet, rSh );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractInsertSectionTabDialog_Impl( pDlg );
    return 0;
}

AbstractMarkFloatDlg * SwAbstractDialogFactory_Impl::CreateIndexMarkFloatDlg(
                                                    SfxBindings* pBindings,
                                                       SfxChildWindow* pChild,
                                                       Window *pParent,
                                                    SfxChildWinInfo* pInfo,
                                                       sal_Bool bNew ) //add for SwIndexMarkFloatDlg
{
    SwIndexMarkFloatDlg* pDlg = new SwIndexMarkFloatDlg(pBindings, pChild, pParent, pInfo, bNew);
    return new AbstractIndexMarkFloatDlg_Impl(pDlg);
}

AbstractMarkFloatDlg * SwAbstractDialogFactory_Impl::CreateAuthMarkFloatDlg(
                                                    SfxBindings* pBindings,
                                                       SfxChildWindow* pChild,
                                                       Window *pParent,
                                                    SfxChildWinInfo* pInfo,
                                                       sal_Bool bNew) //add for SwAuthMarkFloatDlg
{
    SwAuthMarkFloatDlg* pDlg = new SwAuthMarkFloatDlg( pBindings, pChild, pParent, pInfo, bNew );
    return new AbstractAuthMarkFloatDlg_Impl( pDlg );
}

AbstractSwWordCountFloatDlg * SwAbstractDialogFactory_Impl::CreateSwWordCountDialog(
                                                                              SfxBindings* pBindings,
                                                                              SfxChildWindow* pChild,
                                                                              Window *pParent,
                                                                              SfxChildWinInfo* pInfo)
{
    SwWordCountFloatDlg* pDlg = new SwWordCountFloatDlg( pBindings, pChild, pParent, pInfo );
    return new AbstractSwWordCountFloatDlg_Impl( pDlg );
}

//add for SwIndexMarkModalDlg begin
VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateIndexMarkModalDlg(
                                                Window *pParent, SwWrtShell& rSh, SwTOXMark* pCurTOXMark ) //add for SwIndexMarkModalDlg
{
    Dialog* pDlg = new SwIndexMarkModalDlg( pParent, rSh, pCurTOXMark );
    return new VclAbstractDialog_Impl( pDlg );
}

//add for SwIndexMarkModalDlg end
AbstractMailMergeWizard*    SwAbstractDialogFactory_Impl::CreateMailMergeWizard(
                                    SwView& rView, SwMailMergeConfigItem& rConfigItem)
{
    return new AbstractMailMergeWizard_Impl( new SwMailMergeWizard(rView, rConfigItem));
}

//add for static func in SwGlossaryDlg
GlossaryGetCurrGroup    SwAbstractDialogFactory_Impl::GetGlossaryCurrGroupFunc( sal_uInt16 nId )
{
    switch ( nId )
    {
        case DLG_RENAME_GLOS :
            return SwGlossaryDlg::GetCurrGroup;
        default:
            break;
    }
    return 0;
}
GlossarySetActGroup SwAbstractDialogFactory_Impl::SetGlossaryActGroupFunc( sal_uInt16 nId )
{
    switch ( nId )
    {
        case DLG_RENAME_GLOS :
            return SwGlossaryDlg::SetActGroup;
        default:
            break;
    }
    return 0;
}


//------------------ Factories for TabPages
CreateTabPage SwAbstractDialogFactory_Impl::GetTabPageCreatorFunc( sal_uInt16 nId )
{
    CreateTabPage pRet = 0;
    switch ( nId )
    {
        case TP_OPTCOMPATIBILITY_PAGE :
        case RID_SW_TP_OPTCOMPATIBILITY_PAGE :
            pRet = SwCompatibilityOptPage::Create;
            break;
        case TP_OPTLOAD_PAGE :
        case RID_SW_TP_OPTLOAD_PAGE :
            pRet = SwLoadOptPage::Create;
            break;
        case TP_OPTCAPTION_PAGE:
        case RID_SW_TP_OPTCAPTION_PAGE:
            return SwCaptionOptPage::Create;
        case TP_CONTENT_OPT :
        case RID_SW_TP_CONTENT_OPT:
        case RID_SW_TP_HTML_CONTENT_OPT:
            pRet = SwContentOptPage::Create;
            break;
        case TP_OPTSHDWCRSR :
        case RID_SW_TP_OPTSHDWCRSR:
        case RID_SW_TP_HTML_OPTSHDWCRSR:
            pRet = SwShdwCrsrOptionsTabPage::Create;
            break;
        case RID_SW_TP_REDLINE_OPT :
        case TP_REDLINE_OPT :
            pRet = SwRedlineOptionsTabPage::Create;
            break;
        case RID_SW_TP_OPTTEST_PAGE :
        case TP_OPTTEST_PAGE :
#ifdef DBG_UTIL
            pRet = SwTestTabPage::Create;
#endif
            break;
        case TP_OPTPRINT_PAGE :
        case RID_SW_TP_HTML_OPTPRINT_PAGE:
        case RID_SW_TP_OPTPRINT_PAGE:
            pRet = SwAddPrinterTabPage::Create;
            break;
        case TP_STD_FONT :
        case RID_SW_TP_STD_FONT:
        case RID_SW_TP_STD_FONT_CJK:
        case RID_SW_TP_STD_FONT_CTL:
            pRet = SwStdFontTabPage::Create;
            break;
        case TP_OPTTABLE_PAGE :
        case RID_SW_TP_HTML_OPTTABLE_PAGE:
        case RID_SW_TP_OPTTABLE_PAGE:
            pRet = SwTableOptionsTabPage::Create;
            break;
        case TP_DOC_STAT :
            pRet = SwDocStatPage::Create;
            break;
        case RID_SW_TP_MAILCONFIG:
            pRet = SwMailConfigPage::Create;
        break;
        case RID_SW_TP_COMPARISON_OPT :
        case TP_COMPARISON_OPT :
            pRet = SwCompareOptionsTabPage::Create;
        break;
    }

    return pRet;
}

GetTabPageRanges SwAbstractDialogFactory_Impl::GetTabPageRangesFunc( sal_uInt16 nId )
{
    switch ( nId )
    {
    case 1 : //RID_SVXPAGE_TEXTANIMATION :
            //return SvxTextAnimationPage::GetRanges;
            break;
        default:
            break;
    }

    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
