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

#ifndef SDEXT_PRESENTER_BITMAP_CONTAINER_HXX
#define SDEXT_PRESENTER_BITMAP_CONTAINER_HXX

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/drawing/XPresenterHelper.hpp>
#include <com/sun/star/rendering/XBitmap.hpp>
#include <com/sun/star/rendering/XCanvas.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/Color.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace sdext { namespace presenter {

/** Manage a set of bitmap groups as they are used for buttons: three
    bitmaps, one for the normal state, one for a mouse over effect and one
    to show that the button has been pressed.
    A bitmap group is defined by some entries in the configuration.
*/
class PresenterBitmapContainer
    : private ::boost::noncopyable
{
public:
    /** There is one bitmap for the normal state, one for a mouse over effect and one
        to show that a button has been pressed.
    */
    class BitmapDescriptor
    {
    public:
        BitmapDescriptor (void);
        BitmapDescriptor (const ::boost::shared_ptr<BitmapDescriptor>& rpDefault);

        enum Mode {Normal, MouseOver, ButtonDown, Disabled, Mask};
        css::uno::Reference<css::rendering::XBitmap> GetNormalBitmap (void) const;
        css::uno::Reference<css::rendering::XBitmap> GetBitmap (
            const Mode eMode,
            const bool bMissingDefaultsToNormal = true) const;
        void SetBitmap (
            const Mode eMode,
            const css::uno::Reference<css::rendering::XBitmap>& rxBitmap);

        sal_Int32 mnWidth;
        sal_Int32 mnHeight;
        sal_Int32 mnXOffset;
        sal_Int32 mnYOffset;
        sal_Int32 mnXHotSpot;
        sal_Int32 mnYHotSpot;
        css::util::Color maReplacementColor;
        enum TexturingMode { Once, Repeat, Stretch };
        TexturingMode meHorizontalTexturingMode;
        TexturingMode meVerticalTexturingMode;

    private:
        css::uno::Reference<css::rendering::XBitmap> mxNormalBitmap;
        css::uno::Reference<css::rendering::XBitmap> mxMouseOverBitmap;
        css::uno::Reference<css::rendering::XBitmap> mxButtonDownBitmap;
        css::uno::Reference<css::rendering::XBitmap> mxDisabledBitmap;
        css::uno::Reference<css::rendering::XBitmap> mxMaskBitmap;
    };

    /** Create a new bitmap container from a section of the configuration.
        @param rxComponentContext
            The component context is used to create new API objects.
        @param rxCanvas
            Bitmaps are created specifically for this canvas.
        @param rsConfigurationBase
            The name of a configuration node whose sub-tree defines the
            bitmap sets.
    */
    PresenterBitmapContainer (
        const ::rtl::OUString& rsConfigurationBase,
        const ::boost::shared_ptr<PresenterBitmapContainer>& rpParentContainer,
        const css::uno::Reference<css::uno::XComponentContext>& rxComponentContext,
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const css::uno::Reference<css::drawing::XPresenterHelper>& rxPresenterHelper = NULL);
    PresenterBitmapContainer (
        const css::uno::Reference<css::container::XNameAccess>& rsRootNode,
        const ::boost::shared_ptr<PresenterBitmapContainer>& rpParentContainer,
        const css::uno::Reference<css::uno::XComponentContext>& rxComponentContext,
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const css::uno::Reference<css::drawing::XPresenterHelper>& rxPresenterHelper = NULL);
    ~PresenterBitmapContainer (void);

    void Initialize (
        const css::uno::Reference<css::uno::XComponentContext>& rxComponentContext);

    /** Return the bitmap set that is associated with the given name.
    */
    ::boost::shared_ptr<BitmapDescriptor> GetBitmap (const ::rtl::OUString& rsName) const;

    static ::boost::shared_ptr<BitmapDescriptor> LoadBitmap (
        const css::uno::Reference<css::container::XHierarchicalNameAccess>& rxNode,
        const ::rtl::OUString& rsPathToBitmapNode,
        const css::uno::Reference<css::drawing::XPresenterHelper>& rxPresenterHelper,
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const ::boost::shared_ptr<BitmapDescriptor>& rpDefaultBitmap);

private:
    ::boost::shared_ptr<PresenterBitmapContainer> mpParentContainer;
    typedef ::std::map<rtl::OUString, ::boost::shared_ptr<BitmapDescriptor> > BitmapContainer;
    BitmapContainer maIconContainer;
    css::uno::Reference<css::rendering::XCanvas> mxCanvas;
    css::uno::Reference<css::drawing::XPresenterHelper> mxPresenterHelper;

    void LoadBitmaps (
        const css::uno::Reference<css::container::XNameAccess>& rsRootNode);
    void ProcessBitmap (
        const ::rtl::OUString& rsKey,
        const css::uno::Reference<css::beans::XPropertySet>& rProperties);
    static ::boost::shared_ptr<BitmapDescriptor> LoadBitmap (
        const css::uno::Reference<css::beans::XPropertySet>& rxProperties,
        const css::uno::Reference<css::drawing::XPresenterHelper>& rxPresenterHelper,
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas,
        const ::boost::shared_ptr<PresenterBitmapContainer::BitmapDescriptor>& rpDefault);
    static BitmapDescriptor::TexturingMode
        StringToTexturingMode (const ::rtl::OUString& rsTexturingMode);
};

typedef PresenterBitmapContainer::BitmapDescriptor PresenterBitmapDescriptor;
typedef ::boost::shared_ptr<PresenterBitmapContainer::BitmapDescriptor> SharedBitmapDescriptor;

} } // end of namespace ::sdext::presenter

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
