/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <vcl/dialog.hxx>
#include <vcl/layout.hxx>
#include "window.h"

VclContainer::VclContainer(Window *pParent, WinBits nStyle)
    : Window(WINDOW_CONTAINER)
    , m_bLayoutDirty(true)
{
    ImplInit(pParent, nStyle, NULL);
    EnableChildTransparentMode();
    SetPaintTransparent(sal_True);
    SetBackground();
}

Size VclContainer::GetOptimalSize(WindowSizeType eType) const
{
    if (eType == WINDOWSIZE_MAXIMUM)
        return Window::GetOptimalSize(eType);
    return calculateRequisition();
}

void VclContainer::setLayoutPosSize(Window &rWindow, const Point &rPos, const Size &rSize)
{
    sal_Int32 nBorderWidth = rWindow.get_border_width();
    sal_Int32 nLeft = rWindow.get_margin_left() + nBorderWidth;
    sal_Int32 nTop = rWindow.get_margin_top() + nBorderWidth;
    sal_Int32 nRight = rWindow.get_margin_right() + nBorderWidth;
    sal_Int32 nBottom = rWindow.get_margin_bottom() + nBorderWidth;
    Point aPos(rPos.X() + nLeft, rPos.Y() + nTop);
    Size aSize(rSize.Width() - nLeft - nRight, rSize.Height() - nTop - nBottom);
    rWindow.SetPosSizePixel(aPos, aSize);
}

void VclContainer::setLayoutAllocation(Window &rChild, const Point &rAllocPos, const Size &rChildAlloc)
{
    VclAlign eHalign = rChild.get_halign();
    VclAlign eValign = rChild.get_valign();

    //typical case
    if (eHalign == VCL_ALIGN_FILL && eValign == VCL_ALIGN_FILL)
    {
        setLayoutPosSize(rChild, rAllocPos, rChildAlloc);
        return;
    }

    Point aChildPos(rAllocPos);
    Size aChildSize(rChildAlloc);
    Size aChildPreferredSize(getLayoutRequisition(rChild));

    switch (eHalign)
    {
        case VCL_ALIGN_FILL:
            break;
        case VCL_ALIGN_START:
            if (aChildPreferredSize.Width() < rChildAlloc.Width())
                aChildSize.Width() = aChildPreferredSize.Width();
            break;
        case VCL_ALIGN_END:
            if (aChildPreferredSize.Width() < rChildAlloc.Width())
                aChildSize.Width() = aChildPreferredSize.Width();
            aChildPos.X() += rChildAlloc.Width();
            aChildPos.X() -= aChildSize.Width();
            break;
        case VCL_ALIGN_CENTER:
            if (aChildPreferredSize.Width() < aChildSize.Width())
                aChildSize.Width() = aChildPreferredSize.Width();
            aChildPos.X() += (rChildAlloc.Width() - aChildSize.Width()) / 2;
            break;
    }

    switch (eValign)
    {
        case VCL_ALIGN_FILL:
            break;
        case VCL_ALIGN_START:
            if (aChildPreferredSize.Height() < rChildAlloc.Height())
                aChildSize.Height() = aChildPreferredSize.Height();
            break;
        case VCL_ALIGN_END:
            if (aChildPreferredSize.Height() < rChildAlloc.Height())
                aChildSize.Height() = aChildPreferredSize.Height();
            aChildPos.Y() += rChildAlloc.Height();
            aChildPos.Y() -= aChildSize.Height();
            break;
        case VCL_ALIGN_CENTER:
            if (aChildPreferredSize.Height() < aChildSize.Height())
                aChildSize.Height() = aChildPreferredSize.Height();
            aChildPos.Y() += (rChildAlloc.Height() - aChildSize.Height()) / 2;
            break;
    }

    setLayoutPosSize(rChild, aChildPos, aChildSize);
}

Size VclContainer::getLayoutRequisition(const Window &rWindow)
{
    sal_Int32 nBorderWidth = rWindow.get_border_width();
    sal_Int32 nLeft = rWindow.get_margin_left() + nBorderWidth;
    sal_Int32 nTop = rWindow.get_margin_top() + nBorderWidth;
    sal_Int32 nRight = rWindow.get_margin_right() + nBorderWidth;
    sal_Int32 nBottom = rWindow.get_margin_bottom() + nBorderWidth;
    Size aSize(rWindow.get_preferred_size());
    return Size(aSize.Width() + nLeft + nRight, aSize.Height() + nTop + nBottom);
}

void VclContainer::SetPosSizePixel(const Point& rAllocPos, const Size& rAllocation)
{
    bool bSizeChanged = rAllocation != GetOutputSizePixel();
    Window::SetPosSizePixel(rAllocPos, rAllocation);
    if (m_bLayoutDirty || bSizeChanged)
    {
        m_bLayoutDirty = false;
        setAllocation(rAllocation);
    }
}

void VclContainer::SetPosPixel(const Point& rAllocPos)
{
    Point aAllocPos = rAllocPos;
    sal_Int32 nBorderWidth = get_border_width();
    aAllocPos.X() += nBorderWidth + get_margin_left();
    aAllocPos.Y() += nBorderWidth + get_margin_top();

    if (aAllocPos != GetPosPixel())
        Window::SetPosPixel(aAllocPos);
}

void VclContainer::SetSizePixel(const Size& rAllocation)
{
    Size aAllocation = rAllocation;
    sal_Int32 nBorderWidth = get_border_width();
    aAllocation.Width() -= nBorderWidth*2 + get_margin_left() + get_margin_right();
    aAllocation.Height() -= nBorderWidth*2 + get_margin_top() + get_margin_bottom();
    bool bSizeChanged = aAllocation != GetSizePixel();
    if (bSizeChanged)
        Window::SetSizePixel(aAllocation);
    if (m_bLayoutDirty || bSizeChanged)
    {
        m_bLayoutDirty = false;
        setAllocation(aAllocation);
    }
}

void VclBox::accumulateMaxes(const Size &rChildSize, Size &rSize) const
{
    long nSecondaryChildDimension = getSecondaryDimension(rChildSize);
    long nSecondaryBoxDimension = getSecondaryDimension(rSize);
    setSecondaryDimension(rSize, std::max(nSecondaryChildDimension, nSecondaryBoxDimension));

    long nPrimaryChildDimension = getPrimaryDimension(rChildSize);
    long nPrimaryBoxDimension = getPrimaryDimension(rSize);
    if (m_bHomogeneous)
        setPrimaryDimension(rSize, std::max(nPrimaryBoxDimension, nPrimaryChildDimension));
    else
        setPrimaryDimension(rSize, nPrimaryBoxDimension + nPrimaryChildDimension);
}

Size VclBox::calculateRequisition() const
{
    sal_uInt16 nVisibleChildren = 0;

    Size aSize;
    for (Window *pChild = GetWindow(WINDOW_FIRSTCHILD); pChild; pChild = pChild->GetWindow(WINDOW_NEXT))
    {
        if (!pChild->IsVisible())
            continue;
        ++nVisibleChildren;
        Size aChildSize = getLayoutRequisition(*pChild);

        long nPrimaryDimension = getPrimaryDimension(aChildSize);
        nPrimaryDimension += pChild->get_padding() * 2;
        setPrimaryDimension(aChildSize, nPrimaryDimension);

        accumulateMaxes(aChildSize, aSize);
    }

    return finalizeMaxes(aSize, nVisibleChildren);
}

void VclBox::setAllocation(const Size &rAllocation)
{
    sal_uInt16 nVisibleChildren = 0, nExpandChildren = 0;
    for (Window *pChild = GetWindow(WINDOW_FIRSTCHILD); pChild; pChild = pChild->GetWindow(WINDOW_NEXT))
    {
        if (!pChild->IsVisible())
            continue;
        ++nVisibleChildren;
        bool bExpand = getPrimaryDimensionChildExpand(*pChild);
        if (bExpand)
            ++nExpandChildren;
    }

    if (!nVisibleChildren)
        return;

    long nAllocPrimaryDimension = getPrimaryDimension(rAllocation);

    long nHomogeneousDimension = 0, nExtraSpace = 0;
    if (m_bHomogeneous)
    {
        nHomogeneousDimension = ((nAllocPrimaryDimension -
            (nVisibleChildren - 1) * m_nSpacing)) / nVisibleChildren;
    }
    else if (nExpandChildren)
    {
        Size aRequisition = calculateRequisition();
        nExtraSpace = (getPrimaryDimension(rAllocation) - getPrimaryDimension(aRequisition)) / nExpandChildren;
    }

    for (sal_Int32 ePackType = VCL_PACK_START; ePackType <= VCL_PACK_END; ++ePackType)
    {
        Point aPos(0, 0);
        if (ePackType == VCL_PACK_END)
        {
            long nPrimaryCoordinate = getPrimaryCoordinate(aPos);
            setPrimaryCoordinate(aPos, nPrimaryCoordinate + nAllocPrimaryDimension);
        }

        for (Window *pChild = GetWindow(WINDOW_FIRSTCHILD); pChild; pChild = pChild->GetWindow(WINDOW_NEXT))
        {
            if (!pChild->IsVisible())
                continue;

            sal_Int32 ePacking = pChild->get_pack_type();

            if (ePacking != ePackType)
                continue;

            long nPadding = pChild->get_padding();

            Size aBoxSize;
            if (m_bHomogeneous)
                setPrimaryDimension(aBoxSize, nHomogeneousDimension);
            else
            {
                aBoxSize = getLayoutRequisition(*pChild);
                long nPrimaryDimension = getPrimaryDimension(aBoxSize);
                nPrimaryDimension += nPadding * 2;
                if (getPrimaryDimensionChildExpand(*pChild))
                    nPrimaryDimension += nExtraSpace;
                setPrimaryDimension(aBoxSize, nPrimaryDimension);
            }
            setSecondaryDimension(aBoxSize, getSecondaryDimension(rAllocation));

            Point aChildPos(aPos);
            Size aChildSize(aBoxSize);
            long nPrimaryCoordinate = getPrimaryCoordinate(aPos);

            bool bFill = pChild->get_fill();
            if (bFill)
            {
                setPrimaryDimension(aChildSize, std::max(static_cast<long>(1),
                    getPrimaryDimension(aBoxSize) - nPadding * 2));

                setPrimaryCoordinate(aChildPos, nPrimaryCoordinate + nPadding);
            }
            else
            {
                setPrimaryDimension(aChildSize,
                    getPrimaryDimension(getLayoutRequisition(*pChild)));

                setPrimaryCoordinate(aChildPos, nPrimaryCoordinate +
                    (getPrimaryDimension(aBoxSize) - getPrimaryDimension(aChildSize)) / 2);
            }

            long nDiff = getPrimaryDimension(aBoxSize) + m_nSpacing;
            if (ePackType == VCL_PACK_START)
                setPrimaryCoordinate(aPos, nPrimaryCoordinate + nDiff);
            else
            {
                setPrimaryCoordinate(aPos, nPrimaryCoordinate - nDiff);
                setPrimaryCoordinate(aChildPos, getPrimaryCoordinate(aChildPos) -
                    getPrimaryDimension(aBoxSize));
            }

            setLayoutAllocation(*pChild, aChildPos, aChildSize);
        }
    }
}

bool VclBox::set_property(const rtl::OString &rKey, const rtl::OString &rValue)
{
    if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("spacing")))
        set_spacing(rValue.toInt32());
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("homogeneous")))
        set_homogeneous(toBool(rValue));
    else
        return VclContainer::set_property(rKey, rValue);
    return true;
}

#define DEFAULT_CHILD_MIN_WIDTH 85
#define DEFAULT_CHILD_MIN_HEIGHT 27

Size VclBox::finalizeMaxes(const Size &rSize, sal_uInt16 nVisibleChildren) const
{
    Size aRet;

    if (nVisibleChildren)
    {
        long nPrimaryDimension = getPrimaryDimension(rSize);
        if (m_bHomogeneous)
            nPrimaryDimension *= nVisibleChildren;
        setPrimaryDimension(aRet, nPrimaryDimension + m_nSpacing * (nVisibleChildren-1));
        setSecondaryDimension(aRet, getSecondaryDimension(rSize));
    }

    return aRet;
}

Size VclButtonBox::addReqGroups(const VclButtonBox::Requisition &rReq) const
{
    Size aRet;

    long nMainGroupDimension = getPrimaryDimension(rReq.m_aMainGroupSize);
    long nSubGroupDimension = getPrimaryDimension(rReq.m_aSubGroupSize);

    setPrimaryDimension(aRet, nMainGroupDimension + nSubGroupDimension);

    setSecondaryDimension(aRet,
        std::max(getSecondaryDimension(rReq.m_aMainGroupSize),
        getSecondaryDimension(rReq.m_aSubGroupSize)));

    return aRet;
}

static long getMaxNonOutlier(const std::vector<long> &rG, long nAvgDimension)
{
    long nMaxDimensionNonOutlier = 0;
    for (std::vector<long>::const_iterator aI = rG.begin(),
        aEnd = rG.end(); aI != aEnd; ++aI)
    {
        long nPrimaryChildDimension = *aI;
        if (nPrimaryChildDimension <= nAvgDimension * 1.5)
        {
            nMaxDimensionNonOutlier = std::max(nPrimaryChildDimension,
                nMaxDimensionNonOutlier);
        }
    }
    return nMaxDimensionNonOutlier;
}

static std::vector<long> setButtonSizes(const std::vector<long> &rG,
    long nAvgDimension, long nMaxNonOutlier)
{
    std::vector<long> aVec;
    //set everything < 1.5 times the average to the same width, leave the
    //outliers un-touched
    for (std::vector<long>::const_iterator aI = rG.begin(), aEnd = rG.end();
        aI != aEnd; ++aI)
    {
        long nPrimaryChildDimension = *aI;
        if (nPrimaryChildDimension <= nAvgDimension * 1.5)
            aVec.push_back(nMaxNonOutlier);
        else
            aVec.push_back(nPrimaryChildDimension);
    }
    return aVec;
}

VclButtonBox::Requisition VclButtonBox::calculatePrimarySecondaryRequisitions() const
{
    Requisition aReq;

    Size aMainGroupSize(DEFAULT_CHILD_MIN_WIDTH, DEFAULT_CHILD_MIN_HEIGHT); //to-do, pull from theme
    Size aSubGroupSize(DEFAULT_CHILD_MIN_WIDTH, DEFAULT_CHILD_MIN_HEIGHT); //to-do, pull from theme

    long nMinMainGroupPrimary = getPrimaryDimension(aMainGroupSize);
    long nMinSubGroupPrimary = getPrimaryDimension(aSubGroupSize);
    long nMainGroupSecondary = getSecondaryDimension(aMainGroupSize);
    long nSubGroupSecondary = getSecondaryDimension(aSubGroupSize);

    bool bIgnoreSecondaryPacking = (m_eLayoutStyle == VCL_BUTTONBOX_SPREAD || m_eLayoutStyle == VCL_BUTTONBOX_CENTER);

    std::vector<long> aMainGroupSizes;
    std::vector<long> aSubGroupSizes;

    for (const Window *pChild = GetWindow(WINDOW_FIRSTCHILD); pChild; pChild = pChild->GetWindow(WINDOW_NEXT))
    {
        if (!pChild->IsVisible())
            continue;
        Size aChildSize = getLayoutRequisition(*pChild);
        if (bIgnoreSecondaryPacking || !pChild->get_secondary())
        {
            //set the max secondary dimension
            nMainGroupSecondary = std::max(nMainGroupSecondary, getSecondaryDimension(aChildSize));
            //collect the primary dimensions
            aMainGroupSizes.push_back(std::max(nMinMainGroupPrimary, getPrimaryDimension(aChildSize)));
        }
        else
        {
            nSubGroupSecondary = std::max(nSubGroupSecondary, getSecondaryDimension(aChildSize));
            aSubGroupSizes.push_back(std::max(nMinSubGroupPrimary, getPrimaryDimension(aChildSize)));
        }
    }

    if (m_bHomogeneous)
    {
        long nMaxMainDimension = aMainGroupSizes.empty() ? 0 :
            *std::max_element(aMainGroupSizes.begin(), aMainGroupSizes.end());
        long nMaxSubDimension = aSubGroupSizes.empty() ? 0 :
            *std::max_element(aSubGroupSizes.begin(), aSubGroupSizes.end());
        long nMaxDimension = std::max(nMaxMainDimension, nMaxSubDimension);
        aReq.m_aMainGroupDimensions.resize(aMainGroupSizes.size(), nMaxDimension);
        aReq.m_aSubGroupDimensions.resize(aSubGroupSizes.size(), nMaxDimension);
    }
    else
    {
        //Ideally set everything to the same size, but find outlier widgets
        //that are way wider than the average and leave them
        //at their natural size and set the remainder to share the
        //max size of the remaining members of the buttonbox
        long nAccDimension = std::accumulate(aMainGroupSizes.begin(),
            aMainGroupSizes.end(), 0);
        nAccDimension = std::accumulate(aSubGroupSizes.begin(),
            aSubGroupSizes.end(), nAccDimension);

        long nAvgDimension = nAccDimension /
            (aMainGroupSizes.size() + aSubGroupSizes.size());

        long nMaxMainNonOutlier = getMaxNonOutlier(aMainGroupSizes,
            nAvgDimension);
        long nMaxSubNonOutlier = getMaxNonOutlier(aSubGroupSizes,
            nAvgDimension);
        long nMaxNonOutlier = std::max(nMaxMainNonOutlier, nMaxSubNonOutlier);

        aReq.m_aMainGroupDimensions = setButtonSizes(aMainGroupSizes,
            nAvgDimension, nMaxNonOutlier);
        aReq.m_aSubGroupDimensions = setButtonSizes(aSubGroupSizes,
            nAvgDimension, nMaxNonOutlier);
    }

    if (!aReq.m_aMainGroupDimensions.empty())
    {
        setSecondaryDimension(aReq.m_aMainGroupSize, nMainGroupSecondary);
        setPrimaryDimension(aReq.m_aMainGroupSize,
            std::accumulate(aReq.m_aMainGroupDimensions.begin(),
                aReq.m_aMainGroupDimensions.end(), 0));
    }
    if (!aReq.m_aSubGroupDimensions.empty())
    {
        setSecondaryDimension(aReq.m_aSubGroupSize, nSubGroupSecondary);
        setPrimaryDimension(aReq.m_aSubGroupSize,
            std::accumulate(aReq.m_aSubGroupDimensions.begin(),
                aReq.m_aSubGroupDimensions.end(), 0));
    }

    return aReq;
}

Size VclButtonBox::addSpacing(const Size &rSize, sal_uInt16 nVisibleChildren) const
{
    Size aRet;

    if (nVisibleChildren)
    {
        long nPrimaryDimension = getPrimaryDimension(rSize);
        setPrimaryDimension(aRet,
            nPrimaryDimension + m_nSpacing * (nVisibleChildren-1));
        setSecondaryDimension(aRet, getSecondaryDimension(rSize));
    }

    return aRet;
}

Size VclButtonBox::calculateRequisition() const
{
    Requisition aReq(calculatePrimarySecondaryRequisitions());
    sal_uInt16 nVisibleChildren = aReq.m_aMainGroupDimensions.size() +
        aReq.m_aSubGroupDimensions.size();
    return addSpacing(addReqGroups(aReq), nVisibleChildren);
}

bool VclButtonBox::set_property(const rtl::OString &rKey, const rtl::OString &rValue)
{
    if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("layout-style")))
    {
        VclButtonBoxStyle eStyle = VCL_BUTTONBOX_DEFAULT_STYLE;
        if (rValue.equalsL(RTL_CONSTASCII_STRINGPARAM("start")))
            eStyle = VCL_BUTTONBOX_START;
        else if (rValue.equalsL(RTL_CONSTASCII_STRINGPARAM("spread")))
            eStyle = VCL_BUTTONBOX_SPREAD;
        else if (rValue.equalsL(RTL_CONSTASCII_STRINGPARAM("edge")))
            eStyle = VCL_BUTTONBOX_EDGE;
        else if (rValue.equalsL(RTL_CONSTASCII_STRINGPARAM("start")))
            eStyle = VCL_BUTTONBOX_START;
        else if (rValue.equalsL(RTL_CONSTASCII_STRINGPARAM("end")))
            eStyle = VCL_BUTTONBOX_END;
        else if (rValue.equalsL(RTL_CONSTASCII_STRINGPARAM("center")))
            eStyle = VCL_BUTTONBOX_CENTER;
        else
        {
            SAL_WARN("vcl.layout", "unknown layout style " << rValue.getStr());
        }
        set_layout(eStyle);
    }
    else
        return VclBox::set_property(rKey, rValue);
    return true;
}

void VclButtonBox::setAllocation(const Size &rAllocation)
{
    Requisition aReq(calculatePrimarySecondaryRequisitions());

    if (aReq.m_aMainGroupDimensions.empty() && aReq.m_aSubGroupDimensions.empty())
        return;

    long nAllocPrimaryDimension = getPrimaryDimension(rAllocation);

    Point aMainGroupPos, aOtherGroupPos;
    int nSpacing = m_nSpacing;

    //To-Do, other layout styles
    switch (m_eLayoutStyle)
    {
        case VCL_BUTTONBOX_START:
            if (!aReq.m_aSubGroupDimensions.empty())
            {
                long nOtherPrimaryDimension = getPrimaryDimension(
                    addSpacing(aReq.m_aSubGroupSize, aReq.m_aSubGroupDimensions.size()));
                setPrimaryCoordinate(aOtherGroupPos,
                    nAllocPrimaryDimension - nOtherPrimaryDimension);
            }
            break;
        case VCL_BUTTONBOX_SPREAD:
            if (!aReq.m_aMainGroupDimensions.empty())
            {
                long nMainPrimaryDimension = getPrimaryDimension(
                    addSpacing(aReq.m_aMainGroupSize, aReq.m_aMainGroupDimensions.size()));
                long nExtraSpace = nAllocPrimaryDimension - nMainPrimaryDimension;
                nExtraSpace += (aReq.m_aMainGroupDimensions.size()-1) * nSpacing;
                nSpacing = nExtraSpace/(aReq.m_aMainGroupDimensions.size()+1);
                setPrimaryCoordinate(aMainGroupPos, nSpacing);
            }
            break;
        default:
            SAL_WARN("vcl.layout", "todo unimplemented layout style");
        case VCL_BUTTONBOX_DEFAULT_STYLE:
        case VCL_BUTTONBOX_END:
            if (!aReq.m_aMainGroupDimensions.empty())
            {
                long nMainPrimaryDimension = getPrimaryDimension(
                    addSpacing(aReq.m_aMainGroupSize, aReq.m_aMainGroupDimensions.size()));
                setPrimaryCoordinate(aMainGroupPos,
                    nAllocPrimaryDimension - nMainPrimaryDimension);
            }
            break;
    }

    Size aChildSize;
    setSecondaryDimension(aChildSize, getSecondaryDimension(rAllocation));

    std::vector<long>::const_iterator aPrimaryI = aReq.m_aMainGroupDimensions.begin();
    std::vector<long>::const_iterator aSecondaryI = aReq.m_aSubGroupDimensions.begin();
    bool bIgnoreSecondaryPacking = (m_eLayoutStyle == VCL_BUTTONBOX_SPREAD || m_eLayoutStyle == VCL_BUTTONBOX_CENTER);
    for (Window *pChild = GetWindow(WINDOW_FIRSTCHILD); pChild; pChild = pChild->GetWindow(WINDOW_NEXT))
    {
        if (!pChild->IsVisible())
            continue;

        if (bIgnoreSecondaryPacking || !pChild->get_secondary())
        {
            long nMainGroupPrimaryDimension = *aPrimaryI++;
            setPrimaryDimension(aChildSize, nMainGroupPrimaryDimension);
            setLayoutAllocation(*pChild, aMainGroupPos, aChildSize);
            long nPrimaryCoordinate = getPrimaryCoordinate(aMainGroupPos);
            setPrimaryCoordinate(aMainGroupPos, nPrimaryCoordinate + nMainGroupPrimaryDimension + nSpacing);
        }
        else
        {
            long nSubGroupPrimaryDimension = *aSecondaryI++;
            setPrimaryDimension(aChildSize, nSubGroupPrimaryDimension);
            setLayoutAllocation(*pChild, aOtherGroupPos, aChildSize);
            long nPrimaryCoordinate = getPrimaryCoordinate(aOtherGroupPos);
            setPrimaryCoordinate(aOtherGroupPos, nPrimaryCoordinate + nSubGroupPrimaryDimension + nSpacing);
        }
    }
}

VclGrid::array_type VclGrid::assembleGrid() const
{
    ext_array_type A;

    for (Window* pChild = GetWindow(WINDOW_FIRSTCHILD); pChild;
        pChild = pChild->GetWindow(WINDOW_NEXT))
    {
        sal_Int32 nLeftAttach = pChild->get_grid_left_attach();
        sal_Int32 nWidth = pChild->get_grid_width();
        sal_Int32 nMaxXPos = nLeftAttach+nWidth-1;

        sal_Int32 nTopAttach = pChild->get_grid_top_attach();
        sal_Int32 nHeight = pChild->get_grid_height();
        sal_Int32 nMaxYPos = nTopAttach+nHeight-1;

        sal_Int32 nCurrentMaxXPos = A.shape()[0]-1;
        sal_Int32 nCurrentMaxYPos = A.shape()[1]-1;
        if (nMaxXPos > nCurrentMaxXPos || nMaxYPos > nCurrentMaxYPos)
        {
            nCurrentMaxXPos = std::max(nMaxXPos, nCurrentMaxXPos);
            nCurrentMaxYPos = std::max(nMaxYPos, nCurrentMaxYPos);
            A.resize(boost::extents[nCurrentMaxXPos+1][nCurrentMaxYPos+1]);
        }

        ExtendedGridEntry &rEntry = A[nLeftAttach][nTopAttach];
        rEntry.pChild = pChild;
        rEntry.nSpanWidth = nWidth;
        rEntry.nSpanHeight = nHeight;
        rEntry.x = nLeftAttach;
        rEntry.y = nTopAttach;

        for (sal_Int32 nSpanX = 0; nSpanX < nWidth; ++nSpanX)
        {
            for (sal_Int32 nSpanY = 0; nSpanY < nHeight; ++nSpanY)
            {
                ExtendedGridEntry &rSpan = A[nLeftAttach+nSpanX][nTopAttach+nSpanY];
                rSpan.x = nLeftAttach;
                rSpan.y = nTopAttach;
            }
        }
    }

    //see if we have any empty rows/cols
    sal_Int32 nMaxX = A.shape()[0];
    sal_Int32 nMaxY = A.shape()[1];

    std::vector<bool> aNonEmptyCols(nMaxX);
    std::vector<bool> aNonEmptyRows(nMaxY);

    for (sal_Int32 x = 0; x < nMaxX; ++x)
    {
        for (sal_Int32 y = 0; y < nMaxY; ++y)
        {
            const GridEntry &rEntry = A[x][y];
            const Window *pChild = rEntry.pChild;
            if (pChild && pChild->IsVisible())
            {
                aNonEmptyCols[x] = true;
                aNonEmptyRows[y] = true;
            }
        }
    }

    //reduce the spans of elements that span empty rows or columns
    for (sal_Int32 x = 0; x < nMaxX; ++x)
    {
        for (sal_Int32 y = 0; y < nMaxY; ++y)
        {
            ExtendedGridEntry &rSpan = A[x][y];
            //cell x/y is spanned by the widget at cell rSpan.x/rSpan.y,
            //just points back to itself if there's no cell spanning
            if ((rSpan.x == -1) || (rSpan.y == -1))
            {
                //there is no entry for this cell, i.e. this is a cell
                //with no widget in it, or spanned by any other widget
                continue;
            }
            ExtendedGridEntry &rEntry = A[rSpan.x][rSpan.y];
            if (aNonEmptyCols[x] == false)
                --rEntry.nSpanWidth;
            if (aNonEmptyRows[y] == false)
                --rEntry.nSpanHeight;
        }
    }

    sal_Int32 nNonEmptyCols = std::count(aNonEmptyCols.begin(), aNonEmptyCols.end(), true);
    sal_Int32 nNonEmptyRows = std::count(aNonEmptyRows.begin(), aNonEmptyRows.end(), true);

    //make new grid without empty rows and columns
    array_type B(boost::extents[nNonEmptyCols][nNonEmptyRows]);
    for (sal_Int32 x = 0, x2 = 0; x < nMaxX; ++x)
    {
        if (aNonEmptyCols[x] == false)
            continue;
        for (sal_Int32 y = 0, y2 = 0; y < nMaxY; ++y)
        {
            if (aNonEmptyRows[y] == false)
                continue;
            GridEntry &rEntry = A[x][y];
            B[x2][y2++] = rEntry;
        }
        ++x2;
    }

    return B;
}

bool VclGrid::isNullGrid(const array_type &A) const
{
    sal_Int32 nMaxX = A.shape()[0];
    sal_Int32 nMaxY = A.shape()[1];

    if (!nMaxX || !nMaxY)
        return true;
    return false;
}

void VclGrid::calcMaxs(const array_type &A, std::vector<Value> &rWidths, std::vector<Value> &rHeights) const
{
    sal_Int32 nMaxX = A.shape()[0];
    sal_Int32 nMaxY = A.shape()[1];

    rWidths.resize(nMaxX);
    rHeights.resize(nMaxY);

    //first use the non spanning entries to set default width/heights
    for (sal_Int32 x = 0; x < nMaxX; ++x)
    {
        for (sal_Int32 y = 0; y < nMaxY; ++y)
        {
            const GridEntry &rEntry = A[x][y];
            const Window *pChild = rEntry.pChild;
            if (!pChild)
                continue;

            sal_Int32 nWidth = rEntry.nSpanWidth;
            sal_Int32 nHeight = rEntry.nSpanHeight;

            for (sal_Int32 nSpanX = 0; nSpanX < nWidth; ++nSpanX)
                rWidths[x+nSpanX].m_bExpand = rWidths[x+nSpanX].m_bExpand | pChild->get_hexpand();

            for (sal_Int32 nSpanY = 0; nSpanY < nHeight; ++nSpanY)
                rHeights[y+nSpanY].m_bExpand = rHeights[y+nSpanY].m_bExpand | pChild->get_vexpand();

            if (nWidth == 1 || nHeight == 1)
            {
                Size aChildSize = getLayoutRequisition(*pChild);
                if (nWidth == 1)
                    rWidths[x].m_nValue = std::max(rWidths[x].m_nValue, aChildSize.Width());
                if (nHeight == 1)
                    rHeights[y].m_nValue = std::max(rHeights[y].m_nValue, aChildSize.Height());
            }
        }
    }

    //now use the spanning entries and split any extra sizes across expanding rows/cols
    //where possible
    for (sal_Int32 x = 0; x < nMaxX; ++x)
    {
        for (sal_Int32 y = 0; y < nMaxY; ++y)
        {
            const GridEntry &rEntry = A[x][y];
            const Window *pChild = rEntry.pChild;
            if (!pChild)
                continue;

            sal_Int32 nWidth = rEntry.nSpanWidth;
            sal_Int32 nHeight = rEntry.nSpanHeight;

            if (nWidth == 1 && nHeight == 1)
                continue;

            Size aChildSize = getLayoutRequisition(*pChild);

            if (nWidth > 1)
            {
                sal_Int32 nExistingWidth = 0;
                for (sal_Int32 nSpanX = 0; nSpanX < nWidth; ++nSpanX)
                    nExistingWidth += rWidths[x+nSpanX].m_nValue;

                sal_Int32 nExtraWidth = aChildSize.Width() - nExistingWidth;

                if (nExtraWidth > 0)
                {
                    bool bForceExpandAll = false;
                    sal_Int32 nExpandables = 0;
                    for (sal_Int32 nSpanX = 0; nSpanX < nWidth; ++nSpanX)
                        if (rWidths[x+nSpanX].m_bExpand)
                            ++nExpandables;
                    if (nExpandables == 0)
                    {
                        nExpandables = nWidth;
                        bForceExpandAll = true;
                    }

                    for (sal_Int32 nSpanX = 0; nSpanX < nWidth; ++nSpanX)
                    {
                        if (rWidths[x+nSpanX].m_bExpand || bForceExpandAll)
                            rWidths[x+nSpanX].m_nValue += nExtraWidth/nExpandables;
                    }
                }
            }

            if (nHeight > 1)
            {
                sal_Int32 nExistingHeight = 0;
                for (sal_Int32 nSpanY = 0; nSpanY < nHeight; ++nSpanY)
                    nExistingHeight += rHeights[y+nSpanY].m_nValue;

                sal_Int32 nExtraHeight = aChildSize.Height() - nExistingHeight;

                if (nExtraHeight > 0)
                {
                    bool bForceExpandAll = false;
                    sal_Int32 nExpandables = 0;
                    for (sal_Int32 nSpanY = 0; nSpanY < nHeight; ++nSpanY)
                        if (rHeights[y+nSpanY].m_bExpand)
                            ++nExpandables;
                    if (nExpandables == 0)
                    {
                        nExpandables = nHeight;
                        bForceExpandAll = true;
                    }

                    for (sal_Int32 nSpanY = 0; nSpanY < nHeight; ++nSpanY)
                    {
                        if (rHeights[y+nSpanY].m_bExpand || bForceExpandAll)
                            rHeights[y+nSpanY].m_nValue += nExtraHeight/nExpandables;
                    }
                }
            }
        }
    }
}

bool compareValues(const VclGrid::Value &i, const VclGrid::Value &j)
{
    return i.m_nValue < j.m_nValue;
}

VclGrid::Value accumulateValues(const VclGrid::Value &i, const VclGrid::Value &j)
{
    VclGrid::Value aRet;
    aRet.m_nValue = i.m_nValue + j.m_nValue;
    aRet.m_bExpand = i.m_bExpand | j.m_bExpand;
    return aRet;
}

Size VclGrid::calculateRequisition() const
{
    array_type A = assembleGrid();

    if (isNullGrid(A))
        return Size();

    std::vector<Value> aWidths;
    std::vector<Value> aHeights;
    calcMaxs(A, aWidths, aHeights);

    long nTotalWidth = 0;
    if (get_column_homogeneous())
    {
        nTotalWidth = std::max_element(aWidths.begin(), aWidths.end(), compareValues)->m_nValue;
        nTotalWidth *= aWidths.size();
    }
    else
    {
        nTotalWidth = std::accumulate(aWidths.begin(), aWidths.end(), Value(), accumulateValues).m_nValue;
    }

    nTotalWidth += get_column_spacing() * (aWidths.size()-1);

    long nTotalHeight = 0;
    if (get_row_homogeneous())
    {
        nTotalHeight = std::max_element(aHeights.begin(), aHeights.end(), compareValues)->m_nValue;
        nTotalHeight *= aHeights.size();
    }
    else
    {
        nTotalHeight = std::accumulate(aHeights.begin(), aHeights.end(), Value(), accumulateValues).m_nValue;
    }

    nTotalHeight += get_row_spacing() * (aHeights.size()-1);

    return Size(nTotalWidth, nTotalHeight);
}

void VclGrid::setAllocation(const Size& rAllocation)
{
    array_type A = assembleGrid();

    if (isNullGrid(A))
        return;

    sal_Int32 nMaxX = A.shape()[0];
    sal_Int32 nMaxY = A.shape()[1];

    Size aRequisition;
    std::vector<Value> aWidths(nMaxX);
    std::vector<Value> aHeights(nMaxY);
    if (!get_column_homogeneous() || !get_row_homogeneous())
    {
        aRequisition = calculateRequisition();
        calcMaxs(A, aWidths, aHeights);
    }

    long nAvailableWidth = rAllocation.Width() - (get_column_spacing() * nMaxX);
    if (get_column_homogeneous())
    {
        for (sal_Int32 x = 0; x < nMaxX; ++x)
            aWidths[x].m_nValue = nAvailableWidth/nMaxX;
    }
    else if (rAllocation.Width() != aRequisition.Width())
    {
        sal_Int32 nExpandables = 0;
        for (sal_Int32 x = 0; x < nMaxX; ++x)
            if (aWidths[x].m_bExpand)
                ++nExpandables;
        long nExtraWidthForExpanders = nExpandables ? (rAllocation.Width() - aRequisition.Width()) / nExpandables : 0;

        if (rAllocation.Width() < aRequisition.Width())
        {
            long nExtraWidth = (rAllocation.Width() - aRequisition.Width() - nExtraWidthForExpanders * nExpandables) / nMaxX;

            for (sal_Int32 x = 0; x < nMaxX; ++x)
                aWidths[x].m_nValue += nExtraWidth;
        }

        if (nExtraWidthForExpanders)
        {
            for (sal_Int32 x = 0; x < nMaxX; ++x)
                if (aWidths[x].m_bExpand)
                    aWidths[x].m_nValue += nExtraWidthForExpanders;
        }
    }

    long nAvailableHeight = rAllocation.Height() - (get_row_spacing() * nMaxY);
    if (get_row_homogeneous())
    {
        for (sal_Int32 y = 0; y < nMaxY; ++y)
            aHeights[y].m_nValue = nAvailableHeight/nMaxY;
    }
    else if (rAllocation.Height() != aRequisition.Height())
    {
        sal_Int32 nExpandables = 0;
        for (sal_Int32 y = 0; y < nMaxY; ++y)
            if (aHeights[y].m_bExpand)
                ++nExpandables;
        long nExtraHeightForExpanders = nExpandables ? (rAllocation.Height() - aRequisition.Height()) / nExpandables : 0;

        if (rAllocation.Height() < aRequisition.Height())
        {
            long nExtraHeight = (rAllocation.Height() - aRequisition.Height() - nExtraHeightForExpanders * nExpandables) / nMaxY;

            for (sal_Int32 y = 0; y < nMaxY; ++y)
                aHeights[y].m_nValue += nExtraHeight;
        }

        if (nExtraHeightForExpanders)
        {
            for (sal_Int32 y = 0; y < nMaxY; ++y)
                if (aHeights[y].m_bExpand)
                    aHeights[y].m_nValue += nExtraHeightForExpanders;
        }
    }

    Point aAllocPos(0, 0);
    for (sal_Int32 x = 0; x < nMaxX; ++x)
    {
        for (sal_Int32 y = 0; y < nMaxY; ++y)
        {
            GridEntry &rEntry = A[x][y];
            Window *pChild = rEntry.pChild;
            if (pChild)
            {
                Size aChildAlloc(0, 0);

                sal_Int32 nWidth = rEntry.nSpanWidth;
                for (sal_Int32 nSpanX = 0; nSpanX < nWidth; ++nSpanX)
                    aChildAlloc.Width() += aWidths[x+nSpanX].m_nValue;
                aChildAlloc.Width() += get_column_spacing()*(nWidth-1);

                sal_Int32 nHeight = rEntry.nSpanHeight;
                for (sal_Int32 nSpanY = 0; nSpanY < nHeight; ++nSpanY)
                    aChildAlloc.Height() += aHeights[y+nSpanY].m_nValue;
                aChildAlloc.Height() += get_row_spacing()*(nHeight-1);

                setLayoutAllocation(*pChild, aAllocPos, aChildAlloc);
            }
            aAllocPos.Y() += aHeights[y].m_nValue + get_row_spacing();
        }
        aAllocPos.X() += aWidths[x].m_nValue + get_column_spacing();
        aAllocPos.Y() = 0;
    }
}

bool toBool(const rtl::OString &rValue)
{
    return (rValue[0] == 't' || rValue[0] == 'T' || rValue[0] == '1');
}

bool VclGrid::set_property(const rtl::OString &rKey, const rtl::OString &rValue)
{
    if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("row-spacing")))
        set_row_spacing(rValue.toInt32());
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("column-spacing")))
        set_column_spacing(rValue.toInt32());
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("row-homogeneous")))
        set_row_homogeneous(toBool(rValue));
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("column-homogeneous")))
        set_column_homogeneous(toBool(rValue));
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("n-rows")))
        /*nothing to do*/;
    else
        return VclContainer::set_property(rKey, rValue);
    return true;
}

void setGridAttach(Window &rWidget, sal_Int32 nLeft, sal_Int32 nTop, sal_Int32 nWidth, sal_Int32 nHeight)
{
    rWidget.set_grid_left_attach(nLeft);
    rWidget.set_grid_top_attach(nTop);
    rWidget.set_grid_width(nWidth);
    rWidget.set_grid_height(nHeight);
}

const Window *VclBin::get_child() const
{
    const WindowImpl* pWindowImpl = ImplGetWindowImpl();

    return pWindowImpl->mpFirstChild;
}

Window *VclBin::get_child()
{
    return const_cast<Window*>(const_cast<const VclBin*>(this)->get_child());
}

Size VclBin::calculateRequisition() const
{
    const Window *pChild = get_child();
    if (pChild && pChild->IsVisible())
        return getLayoutRequisition(*pChild);
    return Size(0, 0);
}

void VclBin::setAllocation(const Size &rAllocation)
{
    Window *pChild = get_child();
    if (pChild && pChild->IsVisible())
        setLayoutAllocation(*pChild, Point(0, 0), rAllocation);
}

//To-Do, hook a DecorationView into VclFrame ?

Size VclFrame::calculateRequisition() const
{
    Size aRet(0, 0);

    WindowImpl* pWindowImpl = ImplGetWindowImpl();

    const Window *pChild = get_child();
    const Window *pLabel = pChild != pWindowImpl->mpLastChild ? pWindowImpl->mpLastChild : NULL;

    if (pChild && pChild->IsVisible())
        aRet = getLayoutRequisition(*pChild);

    if (pLabel && pLabel->IsVisible())
    {
        Size aLabelSize = getLayoutRequisition(*pLabel);
        aRet.Height() += aLabelSize.Height();
        aRet.Width() = std::max(aLabelSize.Width(), aRet.Width());
    }

    const FrameStyle &rFrameStyle =
        GetSettings().GetStyleSettings().GetFrameStyle();
    aRet.Width() += rFrameStyle.left + rFrameStyle.right;
    aRet.Height() += rFrameStyle.top + rFrameStyle.bottom;

    return aRet;
}

void VclFrame::setAllocation(const Size &rAllocation)
{
    //SetBackground( Color(0xFF, 0x00, 0xFF) );

    const FrameStyle &rFrameStyle =
        GetSettings().GetStyleSettings().GetFrameStyle();
    Size aAllocation(rAllocation.Width() - rFrameStyle.left - rFrameStyle.right,
        rAllocation.Height() - rFrameStyle.top - rFrameStyle.bottom);
    Point aChildPos(rFrameStyle.left, rFrameStyle.top);

    WindowImpl* pWindowImpl = ImplGetWindowImpl();

    //The label widget is the last (of two) children
    Window *pChild = get_child();
    Window *pLabel = pChild != pWindowImpl->mpLastChild ? pWindowImpl->mpLastChild : NULL;

    if (pLabel && pLabel->IsVisible())
    {
        Size aLabelSize = getLayoutRequisition(*pLabel);
        aLabelSize.Height() = std::min(aLabelSize.Height(), aAllocation.Height());
        aLabelSize.Width() = std::min(aLabelSize.Width(), aAllocation.Width());
        setLayoutAllocation(*pLabel, aChildPos, aLabelSize);
        aAllocation.Height() -= aLabelSize.Height();
        aChildPos.Y() += aLabelSize.Height();
    }

    if (pChild && pChild->IsVisible())
        setLayoutAllocation(*pChild, aChildPos, aAllocation);
}

const Window *VclFrame::get_label_widget() const
{
    //The label widget is the last (of two) children
    const Window *pChild = get_child();
    const WindowImpl* pWindowImpl = ImplGetWindowImpl();
    return pChild != pWindowImpl->mpLastChild ? pWindowImpl->mpLastChild : NULL;
}

Window *VclFrame::get_label_widget()
{
    return const_cast<Window*>(const_cast<const VclFrame*>(this)->get_label_widget());
}

void VclFrame::set_label(const rtl::OUString &rLabel)
{
    //The label widget is the last (of two) children
    Window *pLabel = get_label_widget();
    assert(pLabel);
    pLabel->SetText(rLabel);
}

Size VclAlignment::calculateRequisition() const
{
    Size aRet(m_nLeftPadding + m_nRightPadding,
        m_nTopPadding + m_nBottomPadding);

    const Window *pChild = get_child();
    if (pChild && pChild->IsVisible())
    {
        Size aChildSize = getLayoutRequisition(*pChild);
        aRet.Width() += aChildSize.Width();
        aRet.Height() += aChildSize.Height();
    }

    return aRet;
}

void VclAlignment::setAllocation(const Size &rAllocation)
{
    Window *pChild = get_child();
    if (!pChild || !pChild->IsVisible())
        return;

    Point aChildPos(m_nLeftPadding, m_nTopPadding);

    Size aAllocation;
    aAllocation.Width() = rAllocation.Width() - (m_nLeftPadding + m_nRightPadding);
    aAllocation.Height() = rAllocation.Height() - (m_nTopPadding + m_nBottomPadding);

    setLayoutAllocation(*pChild, aChildPos, aAllocation);
}

bool VclAlignment::set_property(const rtl::OString &rKey, const rtl::OString &rValue)
{
    if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("bottom-padding")))
        m_nBottomPadding = rValue.toInt32();
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("left-padding")))
        m_nLeftPadding = rValue.toInt32();
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("right-padding")))
        m_nRightPadding = rValue.toInt32();
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("top-padding")))
        m_nTopPadding = rValue.toInt32();
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("xalign")))
        m_fXAlign = rValue.toFloat();
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("xscale")))
        m_fXScale = rValue.toFloat();
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("yalign")))
        m_fYAlign = rValue.toFloat();
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("yscale")))
        m_fYScale = rValue.toFloat();
    else
        return VclBin::set_property(rKey, rValue);
    return true;
}

const Window *VclExpander::get_child() const
{
    const WindowImpl* pWindowImpl = ImplGetWindowImpl();

    assert(pWindowImpl->mpFirstChild == &m_aDisclosureButton);

    return pWindowImpl->mpFirstChild->GetWindow(WINDOW_NEXT);
}

Window *VclExpander::get_child()
{
    return const_cast<Window*>(const_cast<const VclExpander*>(this)->get_child());
}

Size VclExpander::calculateRequisition() const
{
    Size aRet(0, 0);

    WindowImpl* pWindowImpl = ImplGetWindowImpl();

    const Window *pChild = get_child();
    const Window *pLabel = pChild != pWindowImpl->mpLastChild ? pWindowImpl->mpLastChild : NULL;

    if (pChild && pChild->IsVisible() && m_aDisclosureButton.IsChecked())
        aRet = getLayoutRequisition(*pChild);

    Size aExpanderSize = getLayoutRequisition(m_aDisclosureButton);

    if (pLabel && pLabel->IsVisible())
    {
        Size aLabelSize = getLayoutRequisition(*pLabel);
        aExpanderSize.Height() = std::max(aExpanderSize.Height(), aLabelSize.Height());
        aExpanderSize.Width() += aLabelSize.Width();
    }

    aRet.Height() += aExpanderSize.Height();
    aRet.Width() = std::max(aExpanderSize.Width(), aRet.Width());

    const FrameStyle &rFrameStyle =
        GetSettings().GetStyleSettings().GetFrameStyle();
    aRet.Width() += rFrameStyle.left + rFrameStyle.right;
    aRet.Height() += rFrameStyle.top + rFrameStyle.bottom;

    return aRet;
}

void VclExpander::setAllocation(const Size &rAllocation)
{
    const FrameStyle &rFrameStyle =
        GetSettings().GetStyleSettings().GetFrameStyle();
    Size aAllocation(rAllocation.Width() - rFrameStyle.left - rFrameStyle.right,
        rAllocation.Height() - rFrameStyle.top - rFrameStyle.bottom);
    Point aChildPos(rFrameStyle.left, rFrameStyle.top);

    WindowImpl* pWindowImpl = ImplGetWindowImpl();

    //The label widget is the last (of two) children
    Window *pChild = get_child();
    Window *pLabel = pChild != pWindowImpl->mpLastChild ? pWindowImpl->mpLastChild : NULL;

    Size aButtonSize = getLayoutRequisition(m_aDisclosureButton);
    Size aLabelSize;
    Size aExpanderSize = aButtonSize;
    if (pLabel && pLabel->IsVisible())
    {
        aLabelSize = getLayoutRequisition(*pLabel);
        aExpanderSize.Height() = std::max(aExpanderSize.Height(), aLabelSize.Height());
        aExpanderSize.Width() += aLabelSize.Width();
    }

    aExpanderSize.Height() = std::min(aExpanderSize.Height(), aAllocation.Height());
    aExpanderSize.Width() = std::min(aExpanderSize.Width(), aAllocation.Width());

    aButtonSize.Height() = std::min(aButtonSize.Height(), aExpanderSize.Height());
    aButtonSize.Width() = std::min(aButtonSize.Width(), aExpanderSize.Width());

    long nExtraExpanderHeight = aExpanderSize.Height() - aButtonSize.Height();
    Point aButtonPos(aChildPos.X(), aChildPos.Y() + nExtraExpanderHeight/2);
    setLayoutAllocation(m_aDisclosureButton, aButtonPos, aButtonSize);

    if (pLabel && pLabel->IsVisible())
    {
        aLabelSize.Height() = std::min(aLabelSize.Height(), aExpanderSize.Height());
        aLabelSize.Width() = std::min(aLabelSize.Width(),
            aExpanderSize.Width() - aButtonSize.Width());

        long nExtraLabelHeight = aExpanderSize.Height() - aLabelSize.Height();
        Point aLabelPos(aChildPos.X() + aButtonSize.Width(), aChildPos.Y() + nExtraLabelHeight/2);
        setLayoutAllocation(*pLabel, aLabelPos, aLabelSize);
    }

    aAllocation.Height() -= aExpanderSize.Height();
    aChildPos.Y() += aExpanderSize.Height();

    if (pChild && pChild->IsVisible())
    {
        if (!m_aDisclosureButton.IsChecked())
            aAllocation = Size();
        setLayoutAllocation(*pChild, aChildPos, aAllocation);
    }
}

bool VclExpander::set_property(const rtl::OString &rKey, const rtl::OString &rValue)
{
    if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("expanded")))
        m_aDisclosureButton.Check(toBool(rValue));
    else if (rKey.equalsL(RTL_CONSTASCII_STRINGPARAM("resize-toplevel")))
        m_bResizeTopLevel = toBool(rValue);
    else
        return VclBin::set_property(rKey, rValue);
    return true;
}

IMPL_LINK( VclExpander, ClickHdl, DisclosureButton*, pBtn )
{
    Window *pChild = get_child();
    if (pChild)
    {
        pChild->Show(pBtn->IsChecked());
        queue_resize();
        Dialog* pResizeDialog = m_bResizeTopLevel ? GetParentDialog() : NULL;
        if (pResizeDialog)
            pResizeDialog->setInitialLayoutSize();
    }
    return 0;
}

const Window *VclScrolledWindow::get_child() const
{
    assert(GetChildCount() == 3);
    const WindowImpl* pWindowImpl = ImplGetWindowImpl();
    return pWindowImpl->mpLastChild;
}

Window *VclScrolledWindow::get_child()
{
    return const_cast<Window*>(const_cast<const VclScrolledWindow*>(this)->get_child());
}

Size VclScrolledWindow::calculateRequisition() const
{
    Size aRet(0, 0);

    const Window *pChild = get_child();
    if (pChild && pChild->IsVisible())
        aRet = getLayoutRequisition(*pChild);

    if (m_aVScroll.IsVisible())
        aRet.Width() += getLayoutRequisition(m_aVScroll).Width();

    if (m_aHScroll.IsVisible())
        aRet.Height() += getLayoutRequisition(m_aVScroll).Height();

    return aRet;
}

void VclScrolledWindow::setAllocation(const Size &rAllocation)
{
    Size aChildAllocation(rAllocation);
    Size aChildReq;

    Window *pChild = get_child();
    if (pChild && pChild->IsVisible())
        aChildReq = getLayoutRequisition(*pChild);

    if (m_aVScroll.IsVisible())
    {
        long nScrollBarWidth = getLayoutRequisition(m_aVScroll).Width();
        Point aScrollPos(rAllocation.Width() - nScrollBarWidth, 0);
        Size aScrollSize(nScrollBarWidth, rAllocation.Height());
        setLayoutAllocation(m_aVScroll, aScrollPos, aScrollSize);
        aChildAllocation.Width() -= nScrollBarWidth;
        aChildAllocation.Height() = aChildReq.Height();
    }

    if (m_aHScroll.IsVisible())
    {
        long nScrollBarHeight = getLayoutRequisition(m_aHScroll).Height();
        Point aScrollPos(0, rAllocation.Height() - nScrollBarHeight);
        Size aScrollSize(rAllocation.Width(), nScrollBarHeight);
        setLayoutAllocation(m_aHScroll, aScrollPos, aScrollSize);
        aChildAllocation.Height() -= nScrollBarHeight;
        aChildAllocation.Width() = aChildReq.Width();
    }

    if (pChild && pChild->IsVisible())
    {
        Point aChildPos(pChild->GetPosPixel());
        if (!m_aHScroll.IsVisible())
            aChildPos.X() = 0;
        if (!m_aVScroll.IsVisible())
            aChildPos.Y() = 0;
        setLayoutAllocation(*pChild, aChildPos, aChildAllocation);
    }
}

Size VclScrolledWindow::getVisibleChildSize() const
{
    Size aRet(GetSizePixel());
    if (m_aVScroll.IsVisible())
        aRet.Width() -= m_aVScroll.GetSizePixel().Width();
    if (m_aHScroll.IsVisible())
        aRet.Height() -= m_aHScroll.GetSizePixel().Height();
    return aRet;
}

bool VclScrolledWindow::set_property(const rtl::OString &rKey, const rtl::OString &rValue)
{
    bool bRet = VclBin::set_property(rKey, rValue);
    m_aVScroll.Show(GetStyle() & WB_VERT);
    m_aHScroll.Show(GetStyle() & WB_HORZ);
    return bRet;
}

Size getLegacyBestSizeForChildren(const Window &rWindow)
{
    Rectangle aBounds;

    for (const Window* pChild = rWindow.GetWindow(WINDOW_FIRSTCHILD); pChild;
        pChild = pChild->GetWindow(WINDOW_NEXT))
    {
        if (!pChild->IsVisible())
            continue;

        Rectangle aChildBounds(pChild->GetPosPixel(), pChild->GetSizePixel());
        aBounds.Union(aChildBounds);
    }

    if (aBounds.IsEmpty())
        return rWindow.GetSizePixel();

    Size aRet(aBounds.GetSize());
    Point aTopLeft(aBounds.TopLeft());
    aRet.Width() += aTopLeft.X()*2;
    aRet.Height() += aTopLeft.Y()*2;

    return aRet;
}

Window* getNonLayoutParent(Window *pWindow)
{
    while (pWindow)
    {
        pWindow = pWindow->GetParent();
        if (!pWindow || !isContainerWindow(*pWindow))
            break;
    }
    return pWindow;
}

Window* getNonLayoutRealParent(Window *pWindow)
{
    while (pWindow)
    {
        pWindow = pWindow->ImplGetParent();
        if (!pWindow || !isContainerWindow(*pWindow))
            break;
    }
    return pWindow;
}

bool isVisibleInLayout(const Window *pWindow)
{
    bool bVisible = true;
    while (bVisible)
    {
        bVisible = pWindow->IsVisible();
        pWindow = pWindow->GetParent();
        if (!pWindow || !isContainerWindow(*pWindow))
            break;
    }
    return bVisible;
}

bool isEnabledInLayout(const Window *pWindow)
{
    bool bEnabled = true;
    while (bEnabled)
    {
        bEnabled = pWindow->IsEnabled();
        pWindow = pWindow->GetParent();
        if (!pWindow || !isContainerWindow(*pWindow))
            break;
    }
    return bEnabled;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
