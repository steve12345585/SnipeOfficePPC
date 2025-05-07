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


#ifdef SD_DLLIMPLEMENTATION
#undef SD_DLLIMPLEMENTATION
#endif

#include <sfx2/objsh.hxx>
#include "sddlgfact.hxx"
#include "strings.hrc"
#include "BreakDlg.hxx"
#include "copydlg.hxx"
#include "custsdlg.hxx"
#include "dlg_char.hxx"
#include "dlgpage.hxx"
#include "dlgass.hxx"
#include "dlgfield.hxx"
#include "dlgsnap.hxx"
#include "layeroptionsdlg.hxx"
#include "ins_paste.hxx"
#include "inspagob.hxx"
#include "morphdlg.hxx"
#include "OutlineBulletDlg.hxx"
#include "paragr.hxx"
#include "present.hxx"
#include "prltempl.hxx"
#include "sdpreslt.hxx"
#include "tabtempl.hxx"
#include "tpaction.hxx"
#include "vectdlg.hxx"
#include "tpoption.hxx"
#include "prntopts.hxx"
#include "pubdlg.hxx"
#include "masterlayoutdlg.hxx"
#include "headerfooterdlg.hxx"

IMPL_ABSTDLG_BASE(VclAbstractDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractCopyDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSdCustomShowDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractTabDialog_Impl);
IMPL_ABSTDLG_BASE(SdPresLayoutTemplateDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractAssistentDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSdModifyFieldDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSdSnapLineDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSdInsertLayerDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSdInsertPasteDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSdInsertPagesObjsDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractMorphDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSdStartPresDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSdPresLayoutDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSfxDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractSdVectorizeDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSdPublishingDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractHeaderFooterDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractBulletDialog_Impl);

//AbstractCopyDlg_Impl begin
void AbstractCopyDlg_Impl::GetAttr( SfxItemSet& rOutAttrs )
{
    pDlg->GetAttr( rOutAttrs );
}
// AbstractCopyDlg_Impl end

//AbstractSdCustomShowDlg_Impl begin
sal_Bool AbstractSdCustomShowDlg_Impl::IsModified() const
{
    return pDlg->IsModified();
}
sal_Bool AbstractSdCustomShowDlg_Impl::IsCustomShow() const
{
    return pDlg->IsCustomShow();
}
// AbstractSdCustomShowDlg_Impl end

// AbstractTabDialog_Impl begin
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
//add for AbstractTabDialog_Impl end

// --------------------------------------------------------------------

// AbstractBulletDialog_Impl begin
void AbstractBulletDialog_Impl::SetCurPageId( sal_uInt16 nId )
{
    static_cast< ::sd::OutlineBulletDlg*>(pDlg)->SetCurPageId( nId );
}
const SfxItemSet* AbstractBulletDialog_Impl::GetOutputItemSet() const
{
    return static_cast< ::sd::OutlineBulletDlg*>(pDlg)->GetOutputItemSet();
}
const sal_uInt16* AbstractBulletDialog_Impl::GetInputRanges(const SfxItemPool& pItem )
{
    return static_cast< ::sd::OutlineBulletDlg*>(pDlg)->GetInputRanges( pItem );
}
void AbstractBulletDialog_Impl::SetInputSet( const SfxItemSet* pInSet )
{
     static_cast< ::sd::OutlineBulletDlg*>(pDlg)->SetInputSet( pInSet );
}
//From class Window.
void AbstractBulletDialog_Impl::SetText( const XubString& rStr )
{
    static_cast< ::sd::OutlineBulletDlg*>(pDlg)->SetText( rStr );
}
String AbstractBulletDialog_Impl::GetText() const
{
    return static_cast< ::sd::OutlineBulletDlg*>(pDlg)->GetText();
}
//add for AbstractBulletDialog_Impl end

// --------------------------------------------------------------------

void SdPresLayoutTemplateDlg_Impl::SetCurPageId( sal_uInt16 nId )
{
    pDlg->SetCurPageId( nId );
}

const SfxItemSet* SdPresLayoutTemplateDlg_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}

const sal_uInt16* SdPresLayoutTemplateDlg_Impl::GetInputRanges(const SfxItemPool& pItem )
{
    return pDlg->GetInputRanges( pItem );
}

void SdPresLayoutTemplateDlg_Impl::SetInputSet( const SfxItemSet* pInSet )
{
     pDlg->SetInputSet( pInSet );
}

void SdPresLayoutTemplateDlg_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}

String SdPresLayoutTemplateDlg_Impl::GetText() const
{
    return pDlg->GetText();
}

// --------------------------------------------------------------------

//AbstractAssistentDlg_Impl begin
SfxObjectShellLock AbstractAssistentDlg_Impl::GetDocument()
{
    return pDlg->GetDocument();
}
OutputType AbstractAssistentDlg_Impl::GetOutputMedium() const
{
    return pDlg->GetOutputMedium();
}
sal_Bool AbstractAssistentDlg_Impl::IsSummary() const
{
    return pDlg->IsSummary();
}
StartType AbstractAssistentDlg_Impl::GetStartType() const
{
    return pDlg->GetStartType();
}
String AbstractAssistentDlg_Impl::GetDocPath() const
{
    return pDlg->GetDocPath();
}
sal_Bool AbstractAssistentDlg_Impl::GetStartWithFlag() const
{
    return pDlg->GetStartWithFlag();
}
sal_Bool AbstractAssistentDlg_Impl::IsDocEmpty() const
{
    return pDlg->IsDocEmpty();
}
com::sun::star::uno::Sequence< com::sun::star::beans::NamedValue > AbstractAssistentDlg_Impl::GetPassword()
{
    return pDlg->GetPassword();
}
// AbstractAssistentDlg_Impl end

//AbstractSdModifyFieldDlg_Impl begin
SvxFieldData* AbstractSdModifyFieldDlg_Impl::GetField()
{
    return pDlg->GetField();
}
SfxItemSet AbstractSdModifyFieldDlg_Impl::GetItemSet()
{
    return pDlg->GetItemSet();
}
// AbstractSdModifyFieldDlg_Impl end

//AbstractSdSnapLineDlg_Impl begin
void AbstractSdSnapLineDlg_Impl::GetAttr(SfxItemSet& rOutAttrs)
{
    pDlg->GetAttr(rOutAttrs);
}
void AbstractSdSnapLineDlg_Impl::HideRadioGroup()
{
    pDlg->HideRadioGroup();
}
void AbstractSdSnapLineDlg_Impl::HideDeleteBtn()
{
    pDlg->HideDeleteBtn();
}
void AbstractSdSnapLineDlg_Impl::SetInputFields(sal_Bool bEnableX, sal_Bool bEnableY)
{
    pDlg->SetInputFields(bEnableX, bEnableY);
}
void AbstractSdSnapLineDlg_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}
// AbstractSdSnapLineDlg_Impl end

//AbstractSdInsertLayerDlg_Impl begin
void AbstractSdInsertLayerDlg_Impl::GetAttr( SfxItemSet& rOutAttrs )
{
    pDlg->GetAttr( rOutAttrs );
}
void AbstractSdInsertLayerDlg_Impl::SetHelpId( const rtl::OString& rHelpId )
{
    pDlg->SetHelpId( rHelpId );
}
// AbstractSdInsertLayerDlg_Impl end

//AbstractSdInsertPasteDlg_Impl begin
sal_Bool AbstractSdInsertPasteDlg_Impl::IsInsertBefore() const
{
    return pDlg->IsInsertBefore();
}
// AbstractSdInsertPasteDlg_Impl end

//AbstractSdInsertPagesObjsDlg_Impl begin
::Window* AbstractSdInsertPagesObjsDlg_Impl::GetWindow()
{
    return (::Window*)pDlg;
}

std::vector<rtl::OUString> AbstractSdInsertPagesObjsDlg_Impl::GetList(const sal_uInt16 nType)
{
    return pDlg->GetList(nType);
}

sal_Bool AbstractSdInsertPagesObjsDlg_Impl::IsLink()
{
    return pDlg->IsLink();
}
sal_Bool AbstractSdInsertPagesObjsDlg_Impl::IsRemoveUnnessesaryMasterPages() const
{
    return pDlg->IsRemoveUnnessesaryMasterPages();
}
// AbstractSdInsertPagesObjsDlg_Impl end

//AbstractMorphDlg_Impl begin
void AbstractMorphDlg_Impl::SaveSettings() const
{
    pDlg->SaveSettings();
}
sal_uInt16 AbstractMorphDlg_Impl::GetFadeSteps() const
{
    return pDlg->GetFadeSteps();
}
sal_Bool AbstractMorphDlg_Impl::IsAttributeFade() const
{
    return pDlg->IsAttributeFade();
}
sal_Bool AbstractMorphDlg_Impl::IsOrientationFade() const
{
    return pDlg->IsOrientationFade();
}
// AbstractMorphDlg_Impl end

//AbstractSdStartPresDlg_Impl begin
void AbstractSdStartPresDlg_Impl::GetAttr( SfxItemSet& rOutAttrs )
{
    pDlg->GetAttr( rOutAttrs );
}
// AbstractSdStartPresDlg_Impl end

//AbstractSdPresLayoutDlg_Impl begin
void AbstractSdPresLayoutDlg_Impl::GetAttr( SfxItemSet& rOutAttrs )
{
    pDlg->GetAttr( rOutAttrs );
}
// AbstractSdPresLayoutDlg_Impl end

//SfxAbstractDialog_Impl begin
const SfxItemSet* AbstractSfxDialog_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}
void AbstractSfxDialog_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}
String AbstractSfxDialog_Impl::GetText() const
{
    return pDlg->GetText();
}

//AbstractSfxSingleTabDialog_Impl end

//AbstractSdVectorizeDlg_Impl begin
const GDIMetaFile& AbstractSdVectorizeDlg_Impl::GetGDIMetaFile() const
{
    return pDlg->GetGDIMetaFile();
}
//AbstractSdVectorizeDlg_Impl end

//AbstractSdPublishingDlg_Impl begin
void AbstractSdPublishingDlg_Impl::GetParameterSequence( ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& rParams )
{
    pDlg->GetParameterSequence( rParams );
}
//AbstractSdPublishingDlg_Impl end

//AbstractHeaderFooterDialog_Impl
void AbstractHeaderFooterDialog_Impl::ApplyToAll( TabPage* pPage )
{
  pDlg->ApplyToAll( pPage );
}

void AbstractHeaderFooterDialog_Impl::Apply( TabPage* pPage )
{
  pDlg->Apply( pPage );
}

void AbstractHeaderFooterDialog_Impl::Cancel( TabPage* pPage )
{
  pDlg->Cancel( pPage );
}
//AbstractHeaderFooterDialog_Impl

//-------------- SdAbstractDialogFactory implementation--------------

//add for BreakDlg begin
VclAbstractDialog * SdAbstractDialogFactory_Impl::CreateBreakDlg(
                                            ::Window* pWindow,
                                            ::sd::DrawView* pDrView,
                                            ::sd::DrawDocShell* pShell,
                                            sal_uLong nSumActionCount,
                                            sal_uLong nObjCount ) //add for BreakDlg
{
    return new VclAbstractDialog_Impl( new ::sd::BreakDlg( pWindow, pDrView, pShell, nSumActionCount, nObjCount ) );
}
//add for BreakDlg end

//add for CopyDlg begin
AbstractCopyDlg * SdAbstractDialogFactory_Impl::CreateCopyDlg(
                                            ::Window* pWindow, const SfxItemSet& rInAttrs,
                                            const rtl::Reference<XColorList> &pColTab,
                                            ::sd::View* pView ) //add for CopyDlg
{
    return new AbstractCopyDlg_Impl( new ::sd::CopyDlg( pWindow, rInAttrs, pColTab, pView ) );
}
//add for CopyDlg end

//add for SdCustomShowDlg begin
AbstractSdCustomShowDlg * SdAbstractDialogFactory_Impl::CreateSdCustomShowDlg( ::Window* pWindow, SdDrawDocument& rDrawDoc ) //add for SdCustomShowDlg
{
    return new AbstractSdCustomShowDlg_Impl( new SdCustomShowDlg( pWindow, rDrawDoc ) );
}
//add for SdCustomShowDlg end

// add for SdCharDlg begin
SfxAbstractTabDialog *  SdAbstractDialogFactory_Impl::CreateSdTabCharDialog( ::Window* pParent, const SfxItemSet* pAttr, SfxObjectShell* pDocShell )
{
    return new AbstractTabDialog_Impl( new SdCharDlg( pParent, pAttr, pDocShell ) );
}


SfxAbstractTabDialog *  SdAbstractDialogFactory_Impl::CreateSdTabPageDialog( ::Window* pParent, const SfxItemSet* pAttr, SfxObjectShell* pDocShell, sal_Bool bAreaPage )
{
    return new AbstractTabDialog_Impl( new SdPageDlg( pDocShell, pParent, pAttr, bAreaPage ) );
}
// add for SdCharDlg end

//add for AssistentDlg begin
AbstractAssistentDlg * SdAbstractDialogFactory_Impl::CreateAssistentDlg( ::Window* pParent, sal_Bool bAutoPilot)
{
    return new AbstractAssistentDlg_Impl( new AssistentDlg( pParent, bAutoPilot ) );
}
//add for AssistentDlg end

//add for SdModifyFieldDlg begin
AbstractSdModifyFieldDlg * SdAbstractDialogFactory_Impl::CreateSdModifyFieldDlg( ::Window* pWindow, const SvxFieldData* pInField, const SfxItemSet& rSet )
{
    return new AbstractSdModifyFieldDlg_Impl( new SdModifyFieldDlg( pWindow, pInField, rSet ) );
}
//add for SdModifyFieldDlg end

//add for SdSnapLineDlg begin
AbstractSdSnapLineDlg * SdAbstractDialogFactory_Impl::CreateSdSnapLineDlg( ::Window* pWindow, const SfxItemSet& rInAttrs, ::sd::View* pView)
{
    return new AbstractSdSnapLineDlg_Impl( new SdSnapLineDlg( pWindow, rInAttrs, pView ) );
}
//add for SdSnapLineDlg end

//add for SdInsertLayerDlg begin
AbstractSdInsertLayerDlg * SdAbstractDialogFactory_Impl::CreateSdInsertLayerDlg( ::Window* pWindow, const SfxItemSet& rInAttrs, bool bDeletable, String aStr ) //add for SdInsertLayerDlg
{
    return new AbstractSdInsertLayerDlg_Impl( new SdInsertLayerDlg( pWindow, rInAttrs, bDeletable, aStr ) );
}
//add for SdInsertLayerDlg end

//add for SdInsertPasteDlg begin
AbstractSdInsertPasteDlg * SdAbstractDialogFactory_Impl::CreateSdInsertPasteDlg( ::Window* pWindow )
{
    return new AbstractSdInsertPasteDlg_Impl( new SdInsertPasteDlg( pWindow ) );
}
//add for SdInsertPasteDlg end

//add for SdInsertPagesObjsDlg begin
AbstractSdInsertPagesObjsDlg * SdAbstractDialogFactory_Impl::CreateSdInsertPagesObjsDlg( ::Window* pParent, const SdDrawDocument* pDoc, SfxMedium* pSfxMedium, const String& rFileName )
{
    return new AbstractSdInsertPagesObjsDlg_Impl( new SdInsertPagesObjsDlg( pParent, pDoc, pSfxMedium, rFileName ) );
}
//add for SdInsertPagesObjsDlg end

//add for MorphDlg begin
AbstractMorphDlg * SdAbstractDialogFactory_Impl::CreateMorphDlg( ::Window* pParent, const SdrObject* pObj1, const SdrObject* pObj2)
{
    return new AbstractMorphDlg_Impl( new ::sd::MorphDlg( pParent, pObj1, pObj2 ) );
}
//add for MorphDlg end

// add for OutlineBulletDlg begin
SfxAbstractTabDialog *  SdAbstractDialogFactory_Impl::CreateSdOutlineBulletTabDlg( ::Window* pParent, const SfxItemSet* pAttr, ::sd::View* pView )
{
    return new AbstractBulletDialog_Impl( new ::sd::OutlineBulletDlg( pParent, pAttr, pView ) );
}

SfxAbstractTabDialog *  SdAbstractDialogFactory_Impl::CreateSdParagraphTabDlg( ::Window* pParent, const SfxItemSet* pAttr )
{
    return new AbstractTabDialog_Impl( new SdParagraphDlg( pParent, pAttr ) );
}
// add for OutlineBulletDlg end

// add for SdStartPresentationDlg begin
AbstractSdStartPresDlg *  SdAbstractDialogFactory_Impl::CreateSdStartPresentationDlg( ::Window* pWindow, const SfxItemSet& rInAttrs,
                                                                                     const std::vector<String> &rPageNames, List* pCSList )
{
    return new AbstractSdStartPresDlg_Impl( new SdStartPresentationDlg( pWindow, rInAttrs, rPageNames, pCSList ) );
}
// add for SdStartPresentationDlg end

// add for SdPresLayoutTemplateDlg begin
SfxAbstractTabDialog *  SdAbstractDialogFactory_Impl::CreateSdPresLayoutTemplateDlg( SfxObjectShell* pDocSh, ::Window* pParent, SdResId DlgId, SfxStyleSheetBase& rStyleBase, PresentationObjects ePO, SfxStyleSheetBasePool* pSSPool )
{
    return new SdPresLayoutTemplateDlg_Impl( new SdPresLayoutTemplateDlg( pDocSh, pParent, DlgId, rStyleBase, ePO, pSSPool ) );
}

// add for SdPresLayoutDlg begin
AbstractSdPresLayoutDlg *  SdAbstractDialogFactory_Impl::CreateSdPresLayoutDlg(  ::sd::DrawDocShell* pDocShell, ::sd::ViewShell* pViewShell, ::Window* pWindow, const SfxItemSet& rInAttrs)
{
    return new AbstractSdPresLayoutDlg_Impl( new SdPresLayoutDlg( pDocShell, pViewShell, pWindow, rInAttrs ) );
}
// add for SdPresLayoutDlg end

// add for SdTabTemplateDlg begin
SfxAbstractTabDialog *  SdAbstractDialogFactory_Impl::CreateSdTabTemplateDlg( ::Window* pParent, const SfxObjectShell* pDocShell, SfxStyleSheetBase& rStyleBase, SdrModel* pModel, SdrView* pView )
{
    return new AbstractTabDialog_Impl( new SdTabTemplateDlg( pParent, pDocShell, rStyleBase, pModel, pView ) );
}
// add for SdTabTemplateDlg end

SfxAbstractDialog* SdAbstractDialogFactory_Impl::CreatSdActionDialog( ::Window* pParent, const SfxItemSet* pAttr, ::sd::View* pView )
{
    return new AbstractSfxDialog_Impl( new SdActionDlg( pParent, pAttr, pView ) );
}

// add for SdVectorizeDlg begin
AbstractSdVectorizeDlg *  SdAbstractDialogFactory_Impl::CreateSdVectorizeDlg( ::Window* pParent, const Bitmap& rBmp, ::sd::DrawDocShell* pDocShell )
{
    return new AbstractSdVectorizeDlg_Impl( new SdVectorizeDlg( pParent, rBmp, pDocShell ) );
}
// add for SdVectorizeDlg end

// add for SdPublishingDlg begin
AbstractSdPublishingDlg *  SdAbstractDialogFactory_Impl::CreateSdPublishingDlg( ::Window* pWindow, DocumentType eDocType)
{
    return new AbstractSdPublishingDlg_Impl( new SdPublishingDlg( pWindow, eDocType ) );
}
// add for SdPublishingDlg end

// Factories for TabPages
CreateTabPage SdAbstractDialogFactory_Impl::GetSdOptionsContentsTabPageCreatorFunc()
{
    return SdTpOptionsContents::Create;
}

CreateTabPage SdAbstractDialogFactory_Impl::GetSdPrintOptionsTabPageCreatorFunc()
{
    return SdPrintOptions::Create;
}

CreateTabPage SdAbstractDialogFactory_Impl::GetSdOptionsMiscTabPageCreatorFunc()
{
    return SdTpOptionsMisc::Create;
}

CreateTabPage SdAbstractDialogFactory_Impl::GetSdOptionsSnapTabPageCreatorFunc()
{
    return SdTpOptionsSnap::Create;
}

VclAbstractDialog* SdAbstractDialogFactory_Impl::CreateMasterLayoutDialog( ::Window* pParent,
    SdDrawDocument* pDoc, SdPage* pCurrentPage )
{
  return new VclAbstractDialog_Impl( new ::sd::MasterLayoutDialog( pParent, pDoc, pCurrentPage ));
}

AbstractHeaderFooterDialog* SdAbstractDialogFactory_Impl::CreateHeaderFooterDialog( ViewShell* pViewShell,
  ::Window* pParent, SdDrawDocument* pDoc, SdPage* pCurrentPage )
{
    return new AbstractHeaderFooterDialog_Impl( new ::sd::HeaderFooterDialog( (::sd::ViewShell*)pViewShell, pParent, pDoc, pCurrentPage ));
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
