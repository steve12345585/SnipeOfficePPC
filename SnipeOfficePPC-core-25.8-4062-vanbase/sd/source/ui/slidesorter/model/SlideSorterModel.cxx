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


#include "model/SlideSorterModel.hxx"

#include "SlideSorter.hxx"
#include "model/SlsPageDescriptor.hxx"
#include "model/SlsPageEnumerationProvider.hxx"
#include "controller/SlideSorterController.hxx"
#include "controller/SlsProperties.hxx"
#include "controller/SlsPageSelector.hxx"
#include "controller/SlsCurrentSlideManager.hxx"
#include "controller/SlsSlotManager.hxx"
#include "view/SlideSorterView.hxx"
#include "taskpane/SlideSorterCacheDisplay.hxx"
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#include <com/sun/star/drawing/XMasterPagesSupplier.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/UnknownPropertyException.hpp>

#include "ViewShellBase.hxx"
#include "DrawViewShell.hxx"
#include "DrawDocShell.hxx"
#include "drawdoc.hxx"
#include "sdpage.hxx"
#include "FrameView.hxx"

#include <tools/diagnose_ex.h>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace sd { namespace slidesorter { namespace model {

namespace {
    class CompareToXDrawPage
    {
    public:
        CompareToXDrawPage (const Reference<drawing::XDrawPage>& rxSlide) : mxSlide(rxSlide) {}
        bool operator() (const SharedPageDescriptor& rpDescriptor)
        { return rpDescriptor.get()!=NULL && rpDescriptor->GetXDrawPage()==mxSlide; }
    private:
        Reference<drawing::XDrawPage> mxSlide;
    };

    bool PrintModel (const SlideSorterModel& rModel)
    {
        for (sal_Int32 nIndex=0,nCount=rModel.GetPageCount(); nIndex<nCount; ++nIndex)
        {
            SharedPageDescriptor pDescriptor (rModel.GetPageDescriptor(nIndex));
            if (pDescriptor)
            {
                OSL_TRACE("%d %d %d %d %x",
                    nIndex,
                    pDescriptor->GetPageIndex(),
                    pDescriptor->GetVisualState().mnPageId,
                    FromCoreIndex(pDescriptor->GetPage()->GetPageNum()),
                    pDescriptor->GetPage());
            }
            else
            {
                OSL_TRACE("%d", nIndex);
            }
        }

        return true;
    }
    bool CheckModel (const SlideSorterModel& rModel)
    {
        for (sal_Int32 nIndex=0,nCount=rModel.GetPageCount(); nIndex<nCount; ++nIndex)
        {
            SharedPageDescriptor pDescriptor (rModel.GetPageDescriptor(nIndex));
            if ( ! pDescriptor)
            {
                PrintModel(rModel);
                OSL_ASSERT(pDescriptor);
                return false;
            }
            if (nIndex != pDescriptor->GetPageIndex())
            {
                PrintModel(rModel);
                OSL_ASSERT(nIndex == pDescriptor->GetPageIndex());
                return false;
            }
            if (nIndex != pDescriptor->GetVisualState().mnPageId)
            {
                PrintModel(rModel);
                OSL_ASSERT(nIndex == pDescriptor->GetVisualState().mnPageId);
                return false;
            }
        }

        return true;
    }
}




SlideSorterModel::SlideSorterModel (SlideSorter& rSlideSorter)
    : maMutex(),
      mrSlideSorter(rSlideSorter),
      mxSlides(),
      mePageKind(PK_STANDARD),
      meEditMode(EM_PAGE),
      maPageDescriptors(0)
{
}




SlideSorterModel::~SlideSorterModel (void)
{
    ClearDescriptorList ();
}




void SlideSorterModel::Init (void)
{
}




void SlideSorterModel::Dispose (void)
{
    ClearDescriptorList ();
}




SdDrawDocument* SlideSorterModel::GetDocument (void)
{
    if (mrSlideSorter.GetViewShellBase() != NULL)
        return mrSlideSorter.GetViewShellBase()->GetDocument();
    else
         return NULL;
}




bool SlideSorterModel::SetEditMode (EditMode eEditMode)
{
    bool bEditModeChanged = false;
    if (meEditMode != eEditMode)
    {
        meEditMode = eEditMode;
        UpdatePageList();
        ClearDescriptorList();
        bEditModeChanged = true;
    }
    return bEditModeChanged;
}




EditMode SlideSorterModel::GetEditMode (void) const
{
    return meEditMode;
}




PageKind SlideSorterModel::GetPageType (void) const
{
    return mePageKind;
}




sal_Int32 SlideSorterModel::GetPageCount (void) const
{
    return maPageDescriptors.size();
}




SharedPageDescriptor SlideSorterModel::GetPageDescriptor (
    const sal_Int32 nPageIndex,
    const bool bCreate) const
{
    ::osl::MutexGuard aGuard (maMutex);

    SharedPageDescriptor pDescriptor;

    if (nPageIndex>=0 && nPageIndex<GetPageCount())
    {
        pDescriptor = maPageDescriptors[nPageIndex];
        if (pDescriptor == NULL && bCreate && mxSlides.is())
        {
            SdPage* pPage = GetPage(nPageIndex);
            pDescriptor.reset(new PageDescriptor (
                Reference<drawing::XDrawPage>(mxSlides->getByIndex(nPageIndex),UNO_QUERY),
                pPage,
                nPageIndex));
            maPageDescriptors[nPageIndex] = pDescriptor;
        }
    }

    return pDescriptor;
}




sal_Int32 SlideSorterModel::GetIndex (const Reference<drawing::XDrawPage>& rxSlide) const
{
    ::osl::MutexGuard aGuard (maMutex);

    // First try to guess the right index.
    Reference<beans::XPropertySet> xSet (rxSlide, UNO_QUERY);
    if (xSet.is())
    {
        try
        {
            const Any aNumber (xSet->getPropertyValue("Number"));
            sal_Int16 nNumber (-1);
            aNumber >>= nNumber;
            nNumber -= 1;
            SharedPageDescriptor pDescriptor (GetPageDescriptor(nNumber, false));
            if (pDescriptor.get() != NULL
                && pDescriptor->GetXDrawPage() == rxSlide)
            {
                return nNumber;
            }
        }
        catch (uno::Exception&)
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    // Guess was wrong, iterate over all slides and search for the right
    // one.
    const sal_Int32 nCount (maPageDescriptors.size());
    for (sal_Int32 nIndex=0; nIndex<nCount; ++nIndex)
    {
        SharedPageDescriptor pDescriptor (maPageDescriptors[nIndex]);

        // Make sure that the descriptor exists.  Without it the given slide
        // can not be found.
        if (pDescriptor.get() == NULL)
        {
            // Call GetPageDescriptor() to create the missing descriptor.
            pDescriptor = GetPageDescriptor(nIndex,true);
        }

        if (pDescriptor->GetXDrawPage() == rxSlide)
            return nIndex;
    }

    return  -1;
}




sal_Int32 SlideSorterModel::GetIndex (const SdrPage* pPage) const
{
    if (pPage == NULL)
        return -1;

    ::osl::MutexGuard aGuard (maMutex);

    // First try to guess the right index.
    sal_Int16 nNumber ((pPage->GetPageNum()-1)/2);
    SharedPageDescriptor pDescriptor (GetPageDescriptor(nNumber, false));
    if (pDescriptor.get() != NULL
        && pDescriptor->GetPage() == pPage)
    {
        return nNumber;
    }

    // Guess was wrong, iterate over all slides and search for the right
    // one.
    const sal_Int32 nCount (maPageDescriptors.size());
    for (sal_Int32 nIndex=0; nIndex<nCount; ++nIndex)
    {
        pDescriptor = maPageDescriptors[nIndex];

        // Make sure that the descriptor exists.  Without it the given slide
        // can not be found.
        if (pDescriptor.get() == NULL)
        {
            // Call GetPageDescriptor() to create the missing descriptor.
            pDescriptor = GetPageDescriptor(nIndex, true);
        }

        if (pDescriptor->GetPage() == pPage)
            return nIndex;
    }

    return  -1;
}




sal_uInt16 SlideSorterModel::GetCoreIndex (const sal_Int32 nIndex) const
{
    SharedPageDescriptor pDescriptor (GetPageDescriptor(nIndex));
    if (pDescriptor)
        return pDescriptor->GetPage()->GetPageNum();
    else
        return mxSlides->getCount()*2+1;
}




/** For now this method uses a trivial algorithm: throw away all descriptors
    and create them anew (on demand).  The main problem that we are facing
    when designing a better algorithm is that we can not compare pointers to
    pages stored in the PageDescriptor objects and those obtained from the
    document: pages may have been deleted and others may have been created
    at the exact same memory locations.
*/
void SlideSorterModel::Resync (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    // Check if document and this model really differ.
    bool bIsUpToDate (true);
    SdDrawDocument* pDocument = GetDocument();
    if (pDocument!=NULL && maPageDescriptors.size()==pDocument->GetSdPageCount(mePageKind))
    {
        for (sal_Int32 nIndex=0,nCount=maPageDescriptors.size(); nIndex<nCount; ++nIndex)
        {
            if (maPageDescriptors[nIndex]
                && maPageDescriptors[nIndex]->GetPage()
                != GetPage(nIndex))
            {
                OSL_TRACE("page %d differs", nIndex);
                bIsUpToDate = false;
                break;
            }
        }
    }
    else
    {
        bIsUpToDate = false;
        OSL_TRACE("models differ");
    }

    if ( ! bIsUpToDate)
    {
        SynchronizeDocumentSelection(); // Try to make the current selection persistent.
        ClearDescriptorList ();
        AdaptSize();
        SynchronizeModelSelection();
        mrSlideSorter.GetController().GetPageSelector().CountSelectedPages();
    }
    CheckModel(*this);
}




void SlideSorterModel::ClearDescriptorList (void)
{
    DescriptorContainer aDescriptors;

    {
        ::osl::MutexGuard aGuard (maMutex);
        aDescriptors.swap(maPageDescriptors);
    }

    for (DescriptorContainer::iterator iDescriptor=aDescriptors.begin(), iEnd=aDescriptors.end();
         iDescriptor!=iEnd;
         ++iDescriptor)
    {
        if (iDescriptor->get() != NULL)
        {
            if ( ! iDescriptor->unique())
            {
                OSL_TRACE("SlideSorterModel::ClearDescriptorList: trying to delete page descriptor  that is still used with count %d", iDescriptor->use_count());
                // No assertion here because that can hang the office when
                // opening a dialog from here.
            }
            iDescriptor->reset();
        }
    }
}




void SlideSorterModel::SynchronizeDocumentSelection (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    PageEnumeration aAllPages (PageEnumerationProvider::CreateAllPagesEnumeration(*this));
    while (aAllPages.HasMoreElements())
    {
        SharedPageDescriptor pDescriptor (aAllPages.GetNextElement());
        pDescriptor->GetPage()->SetSelected(pDescriptor->HasState(PageDescriptor::ST_Selected));
    }
}




void SlideSorterModel::SynchronizeModelSelection (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    PageEnumeration aAllPages (PageEnumerationProvider::CreateAllPagesEnumeration(*this));
    while (aAllPages.HasMoreElements())
    {
        SharedPageDescriptor pDescriptor (aAllPages.GetNextElement());
        pDescriptor->SetState(PageDescriptor::ST_Selected, pDescriptor->GetPage()->IsSelected());
    }
}




::osl::Mutex& SlideSorterModel::GetMutex (void)
{
    return maMutex;
}




void SlideSorterModel::SetDocumentSlides (
    const Reference<container::XIndexAccess>& rxSlides)
{
    ::osl::MutexGuard aGuard (maMutex);

    // Reset the current page so to cause everbody to release references to it.
    mrSlideSorter.GetController().GetCurrentSlideManager()->NotifyCurrentSlideChange(-1);

    mxSlides = rxSlides;
    Resync();

    ViewShell* pViewShell = mrSlideSorter.GetViewShell();
    if (pViewShell != NULL)
    {
        SdPage* pPage = pViewShell->getCurrentPage();
        if (pPage != NULL)
            mrSlideSorter.GetController().GetCurrentSlideManager()->NotifyCurrentSlideChange(
                pPage);
        else
        {
            // No current page.  This can only be when the slide sorter is
            // the main view shell.  Get current slide form frame view.
            const FrameView* pFrameView = pViewShell->GetFrameView();
            if (pFrameView != NULL)
                mrSlideSorter.GetController().GetCurrentSlideManager()->NotifyCurrentSlideChange(
                    pFrameView->GetSelectedPage());
            else
            {
                // No frame view.  As a last resort use the first slide as
                // current slide.
                mrSlideSorter.GetController().GetCurrentSlideManager()->NotifyCurrentSlideChange(
                    sal_Int32(0));
            }
        }
    }

    mrSlideSorter.GetController().GetSlotManager()->NotifyEditModeChange();
}




Reference<container::XIndexAccess> SlideSorterModel::GetDocumentSlides (void) const
{
    ::osl::MutexGuard aGuard (maMutex);
    return mxSlides;
}




void SlideSorterModel::UpdatePageList (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    Reference<container::XIndexAccess> xPages;

    // Get the list of pages according to the edit mode.
    Reference<frame::XController> xController (mrSlideSorter.GetXController());
    if (xController.is())
    {
        switch (meEditMode)
        {
            case EM_MASTERPAGE:
            {
                Reference<drawing::XMasterPagesSupplier> xSupplier (
                    xController->getModel(), UNO_QUERY);
                if (xSupplier.is())
                {
                    xPages = Reference<container::XIndexAccess>(
                        xSupplier->getMasterPages(), UNO_QUERY);
                }
            }
            break;

            case EM_PAGE:
            {
                Reference<drawing::XDrawPagesSupplier> xSupplier (
                    xController->getModel(), UNO_QUERY);
                if (xSupplier.is())
                {
                    xPages = Reference<container::XIndexAccess>(
                        xSupplier->getDrawPages(), UNO_QUERY);
                }
            }
            break;

            default:
                // We should never get here.
                OSL_ASSERT(false);
                break;
        }
    }

    mrSlideSorter.GetController().SetDocumentSlides(xPages);
}




void SlideSorterModel::AdaptSize (void)
{
    if (mxSlides.is())
        maPageDescriptors.resize(mxSlides->getCount());
    else
        maPageDescriptors.resize(0);
}




bool SlideSorterModel::IsReadOnly (void) const
{
    if (mrSlideSorter.GetViewShellBase() != NULL
        && mrSlideSorter.GetViewShellBase()->GetDocShell())
        return mrSlideSorter.GetViewShellBase()->GetDocShell()->IsReadOnly();
    else
        return true;
}




void SlideSorterModel::SaveCurrentSelection (void)
{
    PageEnumeration aPages (PageEnumerationProvider::CreateAllPagesEnumeration(*this));
    while (aPages.HasMoreElements())
    {
        SharedPageDescriptor pDescriptor (aPages.GetNextElement());
        pDescriptor->SetState(
            PageDescriptor::ST_WasSelected,
            pDescriptor->HasState(PageDescriptor::ST_Selected));
    }
}




Region SlideSorterModel::RestoreSelection (void)
{
    Region aRepaintRegion;
    PageEnumeration aPages (PageEnumerationProvider::CreateAllPagesEnumeration(*this));
    while (aPages.HasMoreElements())
    {
        SharedPageDescriptor pDescriptor (aPages.GetNextElement());
        if (pDescriptor->SetState(
            PageDescriptor::ST_Selected,
            pDescriptor->HasState(PageDescriptor::ST_WasSelected)))
        {
            aRepaintRegion.Union(pDescriptor->GetBoundingBox());
        }
    }
    return aRepaintRegion;
}




bool SlideSorterModel::NotifyPageEvent (const SdrPage* pSdrPage)
{
    ::osl::MutexGuard aGuard (maMutex);

    SdPage* pPage = const_cast<SdPage*>(dynamic_cast<const SdPage*>(pSdrPage));
    if (pPage == NULL)
        return false;

    // We are only interested in pages that are currently served by this
    // model.
    if (pPage->GetPageKind() != mePageKind)
        return false;
    if (pPage->IsMasterPage() != (meEditMode==EM_MASTERPAGE))
        return false;

    if (pPage->IsInserted())
        InsertSlide(pPage);
    else
        DeleteSlide(pPage);
    CheckModel(*this);

    return true;
}




void SlideSorterModel::InsertSlide (SdPage* pPage)
{
    // Find the index at which to insert the given page.
    sal_uInt16 nCoreIndex (pPage->GetPageNum());
    sal_Int32 nIndex (FromCoreIndex(nCoreIndex));
    if (pPage != GetPage(nIndex))
        return;

    // Check that the pages in the document before and after the given page
    // are present in this model.
    if (nIndex>0)
        if (GetPage(nIndex-1) != GetPageDescriptor(nIndex-1)->GetPage())
            return;
    if (size_t(nIndex)<maPageDescriptors.size()-1)
        if (GetPage(nIndex+1) != GetPageDescriptor(nIndex)->GetPage())
            return;

    // Insert the given page at index nIndex
    maPageDescriptors.insert(
        maPageDescriptors.begin()+nIndex,
        SharedPageDescriptor(
            new PageDescriptor (
                Reference<drawing::XDrawPage>(mxSlides->getByIndex(nIndex),UNO_QUERY),
                pPage,
                nIndex)));

    // Update page indices.
    UpdateIndices(nIndex+1);
    OSL_TRACE("page inserted");
}




void SlideSorterModel::DeleteSlide (const SdPage* pPage)
{
    const sal_Int32 nIndex (GetIndex(pPage));
    if (maPageDescriptors[nIndex])
        if (maPageDescriptors[nIndex]->GetPage() != pPage)
            return;

    maPageDescriptors.erase(maPageDescriptors.begin()+nIndex);
    UpdateIndices(nIndex);
    OSL_TRACE("page removed");
}




void SlideSorterModel::UpdateIndices (const sal_Int32 nFirstIndex)
{
    for (sal_Int32 nDescriptorIndex=0,nCount=maPageDescriptors.size();
         nDescriptorIndex<nCount;
         ++nDescriptorIndex)
    {
        SharedPageDescriptor& rpDescriptor (maPageDescriptors[nDescriptorIndex]);
        if (rpDescriptor)
        {
            if (nDescriptorIndex < nFirstIndex)
            {
                if (rpDescriptor->GetPageIndex()!=nDescriptorIndex)
                {
                    OSL_ASSERT(rpDescriptor->GetPageIndex()==nDescriptorIndex);
                }
            }
            else
            {
                rpDescriptor->SetPageIndex(nDescriptorIndex);
            }
        }
    }
}




SdPage* SlideSorterModel::GetPage (const sal_Int32 nSdIndex) const
{
    SdDrawDocument* pModel = const_cast<SlideSorterModel*>(this)->GetDocument();
    if (pModel != NULL)
    {
        if (meEditMode == EM_PAGE)
            return pModel->GetSdPage ((sal_uInt16)nSdIndex, mePageKind);
        else
            return pModel->GetMasterSdPage ((sal_uInt16)nSdIndex, mePageKind);
    }
    else
        return NULL;
}


} } } // end of namespace ::sd::slidesorter::model

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
