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

#include "PresenterNotesView.hxx"
#include "PresenterButton.hxx"
#include "PresenterCanvasHelper.hxx"
#include "PresenterGeometryHelper.hxx"
#include "PresenterPaintManager.hxx"
#include "PresenterScrollBar.hxx"
#include "PresenterTextView.hxx"
#include <com/sun/star/accessibility/AccessibleTextType.hpp>
#include <com/sun/star/awt/Key.hpp>
#include <com/sun/star/awt/KeyModifier.hpp>
#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/drawing/framework/XControllerManager.hpp>
#include <com/sun/star/drawing/framework/XConfigurationController.hpp>
#include <com/sun/star/drawing/framework/XPane.hpp>
#include <com/sun/star/lang/XServiceName.hpp>
#include <com/sun/star/presentation/XPresentationPage.hpp>
#include <com/sun/star/rendering/CompositeOperation.hpp>
#include <com/sun/star/rendering/XSpriteCanvas.hpp>
#include <com/sun/star/text/XTextRange.hpp>
#include <com/sun/star/util/XChangesBatch.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <boost/bind.hpp>
#include <set>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing::framework;
using ::rtl::OUString;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

static const sal_Int32 gnSpaceBelowSeparator (10);
static const sal_Int32 gnSpaceAboveSeparator (10);
static const sal_Int32 gnPartHeight (128);
/** Maximal size of memory used for bitmaps which show the notes text.
*/
static const sal_Int32 gnMaximalCacheSize (8*1024*1024);
static const double gnLineScrollFactor (1.2);

namespace sdext { namespace presenter {

//===== PresenterNotesView ====================================================

PresenterNotesView::PresenterNotesView (
    const Reference<XComponentContext>& rxComponentContext,
    const Reference<XResourceId>& rxViewId,
    const Reference<frame::XController>& rxController,
    const ::rtl::Reference<PresenterController>& rpPresenterController)
    : PresenterNotesViewInterfaceBase(m_aMutex),
      mxViewId(rxViewId),
      mpPresenterController(rpPresenterController),
      mxCanvas(),
      mxCurrentNotesPage(),
      mpScrollBar(),
      mxToolBarWindow(),
      mxToolBarCanvas(),
      mpToolBar(),
      mpCloseButton(),
      maSeparatorColor(0xffffff),
      mnSeparatorYLocation(0),
      maTextBoundingBox(),
      mpBackground(),
      mnTop(0),
      mpFont(),
      mpTextView()
{
    try
    {
        Reference<XControllerManager> xCM (rxController, UNO_QUERY_THROW);
        Reference<XConfigurationController> xCC (xCM->getConfigurationController(), UNO_QUERY_THROW);
        Reference<XPane> xPane (xCC->getResource(rxViewId->getAnchor()), UNO_QUERY_THROW);

        mxParentWindow = xPane->getWindow();
        mxCanvas = xPane->getCanvas();
        mpTextView.reset(new PresenterTextView(
            rxComponentContext,
            mxCanvas,
            mpPresenterController->GetPaintManager()->GetInvalidator(mxParentWindow)));

        const OUString sResourceURL (mxViewId->getResourceURL());
        mpFont.reset(new PresenterTheme::FontDescriptor(
            rpPresenterController->GetViewFont(sResourceURL)));
        maSeparatorColor = mpFont->mnColor;
        mpTextView->SetFont(mpFont);

        CreateToolBar(rxComponentContext, rpPresenterController);

        mpCloseButton = PresenterButton::Create(
            rxComponentContext,
            mpPresenterController,
            mpPresenterController->GetTheme(),
            mxParentWindow,
            mxCanvas,
            A2S("NotesViewCloser"));

        if (mxParentWindow.is())
        {
            mxParentWindow->addWindowListener(this);
            mxParentWindow->addPaintListener(this);
            mxParentWindow->addKeyListener(this);
            mxParentWindow->setVisible(sal_True);
        }

        mpScrollBar = new PresenterVerticalScrollBar(
            rxComponentContext,
            mxParentWindow,
            mpPresenterController->GetPaintManager(),
            ::boost::bind(&PresenterNotesView::SetTop, this, _1));
        mpScrollBar->SetBackground(
            mpPresenterController->GetViewBackground(mxViewId->getResourceURL()));

        mpScrollBar->SetCanvas(mxCanvas);

        Layout();
    }
    catch (RuntimeException&)
    {
        PresenterNotesView::disposing();
        throw;
    }
}

PresenterNotesView::~PresenterNotesView (void)
{
}

void SAL_CALL PresenterNotesView::disposing (void)
{
    if (mxParentWindow.is())
    {
        mxParentWindow->removeWindowListener(this);
        mxParentWindow->removePaintListener(this);
        mxParentWindow->removeKeyListener(this);
        mxParentWindow = NULL;
    }

    // Dispose tool bar.
    {
        Reference<XComponent> xComponent (static_cast<XWeak*>(mpToolBar.get()), UNO_QUERY);
        mpToolBar = NULL;
        if (xComponent.is())
            xComponent->dispose();
    }
    {
        Reference<XComponent> xComponent (mxToolBarCanvas, UNO_QUERY);
        mxToolBarCanvas = NULL;
        if (xComponent.is())
            xComponent->dispose();
    }
    {
        Reference<XComponent> xComponent (mxToolBarWindow, UNO_QUERY);
        mxToolBarWindow = NULL;
        if (xComponent.is())
            xComponent->dispose();
    }

    // Dispose close button
    {
        Reference<XComponent> xComponent (static_cast<XWeak*>(mpCloseButton.get()), UNO_QUERY);
        mpCloseButton = NULL;
        if (xComponent.is())
            xComponent->dispose();
    }

    // Create the tool bar.

    mpScrollBar = NULL;

    mxViewId = NULL;
}

void PresenterNotesView::CreateToolBar (
    const css::uno::Reference<css::uno::XComponentContext>& rxContext,
    const ::rtl::Reference<PresenterController>& rpPresenterController)
{
    if (rpPresenterController.get() == NULL)
        return;

    Reference<drawing::XPresenterHelper> xPresenterHelper (
        rpPresenterController->GetPresenterHelper());
    if ( ! xPresenterHelper.is())
        return;

    // Create a new window as container of the tool bar.
    mxToolBarWindow = xPresenterHelper->createWindow(
        mxParentWindow,
        sal_False,
        sal_True,
        sal_False,
        sal_False);
    mxToolBarCanvas = xPresenterHelper->createSharedCanvas (
        Reference<rendering::XSpriteCanvas>(mxCanvas, UNO_QUERY),
        mxParentWindow,
        mxCanvas,
        mxParentWindow,
        mxToolBarWindow);

    // Create the tool bar.
    mpToolBar = new PresenterToolBar(
        rxContext,
        mxToolBarWindow,
        mxToolBarCanvas,
        rpPresenterController,
        PresenterToolBar::Left);
    mpToolBar->Initialize(
        A2S("PresenterScreenSettings/ToolBars/NotesToolBar"));
}

void PresenterNotesView::SetSlide (const Reference<drawing::XDrawPage>& rxNotesPage)
{
    static const ::rtl::OUString sNotesShapeName (
        A2S("com.sun.star.presentation.NotesShape"));
    static const ::rtl::OUString sTextShapeName (
        A2S("com.sun.star.drawing.TextShape"));

    Reference<container::XIndexAccess> xIndexAccess (rxNotesPage, UNO_QUERY);
    if (xIndexAccess.is())
    {
        ::rtl::OUString sText;

        // Iterate over all shapes and find the one that holds the text.
        sal_Int32 nCount (xIndexAccess->getCount());
        for (sal_Int32 nIndex=0; nIndex<nCount; ++nIndex)
        {

            Reference<lang::XServiceName> xServiceName (
                xIndexAccess->getByIndex(nIndex), UNO_QUERY);
            if (xServiceName.is()
                && xServiceName->getServiceName().equals(sNotesShapeName))
            {
                Reference<text::XTextRange> xText (xServiceName, UNO_QUERY);
                if (xText.is())
                {
                    sText += xText->getString();
                }
            }
            else
            {
                Reference<drawing::XShapeDescriptor> xShapeDescriptor (
                    xIndexAccess->getByIndex(nIndex), UNO_QUERY);
                if (xShapeDescriptor.is())
                {
                    ::rtl::OUString sType (xShapeDescriptor->getShapeType());
                    if (sType.equals(sNotesShapeName) || sType.equals(sTextShapeName))
                    {
                        Reference<text::XTextRange> xText (
                            xIndexAccess->getByIndex(nIndex), UNO_QUERY);
                        if (xText.is())
                        {
                            sText += xText->getString();
                            mpTextView->SetText(Reference<text::XText>(xText, UNO_QUERY));
                        }
                    }
                }
            }
        }

        Layout();

        if (mpScrollBar.get() != NULL)
        {
            mpScrollBar->SetThumbPosition(0, false);
            UpdateScrollBar();
        }

        Invalidate();
    }
}

//-----  lang::XEventListener -------------------------------------------------

void SAL_CALL PresenterNotesView::disposing (const lang::EventObject& rEventObject)
    throw (RuntimeException)
{
    if (rEventObject.Source == mxParentWindow)
        mxParentWindow = NULL;
}

//----- XWindowListener -------------------------------------------------------

void SAL_CALL PresenterNotesView::windowResized (const awt::WindowEvent& rEvent)
    throw (RuntimeException)
{
    (void)rEvent;
    Layout();
}

void SAL_CALL PresenterNotesView::windowMoved (const awt::WindowEvent& rEvent)
    throw (RuntimeException)
{
    (void)rEvent;
}

void SAL_CALL PresenterNotesView::windowShown (const lang::EventObject& rEvent)
    throw (RuntimeException)
{
    (void)rEvent;
}

void SAL_CALL PresenterNotesView::windowHidden (const lang::EventObject& rEvent)
    throw (RuntimeException)
{
    (void)rEvent;
}

//----- XPaintListener --------------------------------------------------------

void SAL_CALL PresenterNotesView::windowPaint (const awt::PaintEvent& rEvent)
    throw (RuntimeException)
{
    ThrowIfDisposed();

    if ( ! mbIsPresenterViewActive)
        return;

    ::osl::MutexGuard aSolarGuard (::osl::Mutex::getGlobalMutex());
    Paint(rEvent.UpdateRect);
}

//----- XResourceId -----------------------------------------------------------

Reference<XResourceId> SAL_CALL PresenterNotesView::getResourceId (void)
    throw (RuntimeException)
{
    return mxViewId;
}

sal_Bool SAL_CALL PresenterNotesView::isAnchorOnly (void)
    throw (RuntimeException)
{
    return false;
}

//----- XDrawView -------------------------------------------------------------

void SAL_CALL PresenterNotesView::setCurrentPage (const Reference<drawing::XDrawPage>& rxSlide)
    throw (RuntimeException)
{
    // Get the associated notes page.
    mxCurrentNotesPage = NULL;
    try
    {
        Reference<presentation::XPresentationPage> xPresentationPage(rxSlide, UNO_QUERY);
        if (xPresentationPage.is())
            mxCurrentNotesPage = xPresentationPage->getNotesPage();
    }
    catch (RuntimeException&)
    {
    }

    SetSlide(mxCurrentNotesPage);
}

Reference<drawing::XDrawPage> SAL_CALL PresenterNotesView::getCurrentPage (void)
    throw (RuntimeException)
{
    return NULL;
}

//----- XKeyListener ----------------------------------------------------------

void SAL_CALL PresenterNotesView::keyPressed (const awt::KeyEvent& rEvent)
    throw (RuntimeException)
{
    switch (rEvent.KeyCode)
    {
        case awt::Key::A:
            Scroll(-gnLineScrollFactor * mpFont->mnSize);
            break;

        case awt::Key::Y:
        case awt::Key::Z:
            Scroll(+gnLineScrollFactor * mpFont->mnSize);
            break;

        case awt::Key::S:
            ChangeFontSize(-1);
            break;

        case awt::Key::G:
            ChangeFontSize(+1);
            break;

        case awt::Key::H:
            if (mpTextView)
                mpTextView->MoveCaret(
                    -1,
                    (rEvent.Modifiers == awt::KeyModifier::SHIFT)
                        ? cssa::AccessibleTextType::CHARACTER
                        : cssa::AccessibleTextType::WORD);
            break;

        case awt::Key::L:
            if (mpTextView)
                mpTextView->MoveCaret(
                    +1,
                    (rEvent.Modifiers == awt::KeyModifier::SHIFT)
                        ? cssa::AccessibleTextType::CHARACTER
                        : cssa::AccessibleTextType::WORD);
            break;
    }
}

void SAL_CALL PresenterNotesView::keyReleased (const awt::KeyEvent& rEvent)
    throw (RuntimeException)
{
    (void)rEvent;
}

//-----------------------------------------------------------------------------

void PresenterNotesView::Layout (void)
{
    if ( ! mxParentWindow.is())
        return;

    awt::Rectangle aWindowBox (mxParentWindow->getPosSize());
    geometry::RealRectangle2D aNewTextBoundingBox (0,0,aWindowBox.Width, aWindowBox.Height);

    // Size the tool bar and the horizontal separator above it.
    if (mxToolBarWindow.is())
    {
        const geometry::RealSize2D aToolBarSize (mpToolBar->GetMinimalSize());
        const sal_Int32 nToolBarHeight = sal_Int32(aToolBarSize.Height + 0.5);
        mxToolBarWindow->setPosSize(0, aWindowBox.Height - nToolBarHeight,
            sal_Int32(aToolBarSize.Width + 0.5), nToolBarHeight,
            awt::PosSize::POSSIZE);
        aNewTextBoundingBox.Y2 -= nToolBarHeight;

        mnSeparatorYLocation = aWindowBox.Height - nToolBarHeight - gnSpaceBelowSeparator;
        aNewTextBoundingBox.Y2 = mnSeparatorYLocation - gnSpaceAboveSeparator;

        // Place the close button.
        if (mpCloseButton.get() != NULL)
            mpCloseButton->SetCenter(geometry::RealPoint2D(
                (aWindowBox.Width +  aToolBarSize.Width) / 2,
                aWindowBox.Height - aToolBarSize.Height/2));
    }

    // Check whether the vertical scroll bar is necessary.
    if (mpScrollBar.get() != NULL)
    {
        bool bShowVerticalScrollbar (false);
        try
        {
            const double nTextBoxHeight (aNewTextBoundingBox.Y2 - aNewTextBoundingBox.Y1);
            const double nHeight (mpTextView->GetTotalTextHeight());
            if (nHeight > nTextBoxHeight)
            {
                bShowVerticalScrollbar = true;
                aNewTextBoundingBox.X2 -= mpScrollBar->GetSize();
            }
            mpScrollBar->SetTotalSize(nHeight);
        }
        catch(beans::UnknownPropertyException&)
        {
            OSL_ASSERT(false);
        }

        mpScrollBar->SetVisible(bShowVerticalScrollbar);
        mpScrollBar->SetPosSize(
            geometry::RealRectangle2D(
                aNewTextBoundingBox.X2,
                aNewTextBoundingBox.X1,
                aNewTextBoundingBox.X2 + mpScrollBar->GetSize(),
                aNewTextBoundingBox.Y2));
        if ( ! bShowVerticalScrollbar)
            mpScrollBar->SetThumbPosition(0, false);

        UpdateScrollBar();
    }

    // Has the text area has changed it position or size?
    if (aNewTextBoundingBox.X1 != maTextBoundingBox.X1
        || aNewTextBoundingBox.Y1 != maTextBoundingBox.Y1
        || aNewTextBoundingBox.X2 != maTextBoundingBox.X2
        || aNewTextBoundingBox.Y2 != maTextBoundingBox.Y2)
    {
        maTextBoundingBox = aNewTextBoundingBox;

        mpTextView->SetLocation(
            geometry::RealPoint2D(
                aNewTextBoundingBox.X1,
                aNewTextBoundingBox.Y1));
        mpTextView->SetSize(
            geometry::RealSize2D(
                aNewTextBoundingBox.X2 - aNewTextBoundingBox.X1,
                aNewTextBoundingBox.Y2 - aNewTextBoundingBox.Y1));
    }
}

void PresenterNotesView::Paint (const awt::Rectangle& rUpdateBox)
{
    if ( ! mxParentWindow.is())
        return;
    if ( ! mxCanvas.is())
        return;

    if (mpBackground.get() == NULL)
        mpBackground = mpPresenterController->GetViewBackground(mxViewId->getResourceURL());

    if (rUpdateBox.Y < maTextBoundingBox.Y2
        && rUpdateBox.X < maTextBoundingBox.X2)
    {
        PaintText(rUpdateBox);
    }

    mpTextView->Paint(rUpdateBox);

    if (rUpdateBox.Y + rUpdateBox.Height > maTextBoundingBox.Y2)
    {
        PaintToolBar(rUpdateBox);
    }
}

void PresenterNotesView::PaintToolBar (const awt::Rectangle& rUpdateBox)
{
    awt::Rectangle aWindowBox (mxParentWindow->getPosSize());

    rendering::ViewState aViewState (
        geometry::AffineMatrix2D(1,0,0, 0,1,0),
        NULL);
    rendering::RenderState aRenderState(
        geometry::AffineMatrix2D(1,0,0, 0,1,0),
        NULL,
        Sequence<double>(4),
        rendering::CompositeOperation::SOURCE);

    if (mpBackground.get() != NULL)
    {
        // Paint the background.
        mpPresenterController->GetCanvasHelper()->Paint(
            mpBackground,
            mxCanvas,
            rUpdateBox,
            awt::Rectangle(0,sal_Int32(maTextBoundingBox.Y2),aWindowBox.Width,aWindowBox.Height),
            awt::Rectangle());
    }

    // Paint the horizontal separator.
    OSL_ASSERT(mxViewId.is());
    PresenterCanvasHelper::SetDeviceColor(aRenderState, maSeparatorColor);

    mxCanvas->drawLine(
        geometry::RealPoint2D(0,mnSeparatorYLocation),
        geometry::RealPoint2D(aWindowBox.Width,mnSeparatorYLocation),
        aViewState,
        aRenderState);
}

void PresenterNotesView::PaintText (const awt::Rectangle& rUpdateBox)
{
    const awt::Rectangle aBox (PresenterGeometryHelper::Intersection(rUpdateBox,
            PresenterGeometryHelper::ConvertRectangle(maTextBoundingBox)));

    if (aBox.Width <= 0 || aBox.Height <= 0)
        return;

    rendering::ViewState aViewState (
        geometry::AffineMatrix2D(1,0,0, 0,1,0),
        PresenterGeometryHelper::CreatePolygon(aBox, mxCanvas->getDevice()));
    rendering::RenderState aRenderState(
        geometry::AffineMatrix2D(1,0,0, 0,1,0),
        NULL,
        Sequence<double>(3),
        rendering::CompositeOperation::SOURCE);

    if (mpBackground.get() != NULL)
    {
        // Paint the background.
        mpPresenterController->GetCanvasHelper()->Paint(
            mpBackground,
            mxCanvas,
            rUpdateBox,
            aBox,
            awt::Rectangle());
    }

    Reference<rendering::XSpriteCanvas> xSpriteCanvas (mxCanvas, UNO_QUERY);
    if (xSpriteCanvas.is())
        xSpriteCanvas->updateScreen(sal_False);
}

void PresenterNotesView::Invalidate (void)
{
    mpPresenterController->GetPaintManager()->Invalidate(
        mxParentWindow,
        PresenterGeometryHelper::ConvertRectangle(maTextBoundingBox));
}

void PresenterNotesView::Scroll (const double rnDistance)
{
    try
    {
        mnTop += rnDistance;
        mpTextView->SetOffset(0, mnTop);

        UpdateScrollBar();
        Invalidate();
    }
    catch (beans::UnknownPropertyException&)
    {}
}

void PresenterNotesView::SetTop (const double nTop)
{
    try
    {
        mnTop = nTop;
        mpTextView->SetOffset(0, mnTop);

        UpdateScrollBar();
        Invalidate();
    }
    catch (beans::UnknownPropertyException&)
    {}
}

void PresenterNotesView::ChangeFontSize (const sal_Int32 nSizeChange)
{
    const sal_Int32 nNewSize (mpFont->mnSize + nSizeChange);
    if (nNewSize > 5)
    {
        mpFont->mnSize = nNewSize;
        mpFont->mxFont = NULL;
        mpTextView->SetFont(mpFont);

        Layout();
        UpdateScrollBar();
        Invalidate();

        // Write the new font size to the configuration to make it persistent.
        try
        {
            const OUString sStyleName (mpPresenterController->GetTheme()->GetStyleName(
                mxViewId->getResourceURL()));
            ::boost::shared_ptr<PresenterConfigurationAccess> pConfiguration (
                mpPresenterController->GetTheme()->GetNodeForViewStyle(
                    sStyleName));
            if (pConfiguration.get()==NULL || ! pConfiguration->IsValid())
                return;

            pConfiguration->GoToChild(A2S("Font"));
            pConfiguration->SetProperty(A2S("Size"), Any((sal_Int32)(nNewSize+0.5)));
            pConfiguration->CommitChanges();
        }
        catch (Exception&)
        {
            OSL_ASSERT(false);
        }
    }
}

::boost::shared_ptr<PresenterTextView> PresenterNotesView::GetTextView (void) const
{
    return mpTextView;
}

void PresenterNotesView::UpdateScrollBar (void)
{
    if (mpScrollBar.get() != NULL)
    {
        try
        {
            mpScrollBar->SetTotalSize(mpTextView->GetTotalTextHeight());
        }
        catch(beans::UnknownPropertyException&)
        {
            OSL_ASSERT(false);
        }

        mpScrollBar->SetLineHeight(mpFont->mnSize*1.2);
        mpScrollBar->SetThumbPosition(mnTop, false);

        mpScrollBar->SetThumbSize(maTextBoundingBox.Y2 - maTextBoundingBox.Y1);
        mpScrollBar->CheckValues();
    }
}

void PresenterNotesView::ThrowIfDisposed (void)
    throw (::com::sun::star::lang::DisposedException)
{
    if (rBHelper.bDisposed || rBHelper.bInDispose)
    {
        throw lang::DisposedException (
            A2S("PresenterNotesView object has already been disposed"),
            static_cast<uno::XWeak*>(this));
    }
}

} } // end of namespace ::sdext::presenter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
