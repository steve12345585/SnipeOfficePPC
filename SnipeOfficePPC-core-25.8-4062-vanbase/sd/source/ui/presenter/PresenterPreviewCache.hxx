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

#ifndef SD_PRESENTER_PRESENTER_PREVIEW_CACHE_HXX
#define SD_PRESENTER_PRESENTER_PREVIEW_CACHE_HXX

#include <com/sun/star/drawing/XSlidePreviewCache.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "cache/SlsPageCache.hxx"
#include <cppuhelper/compbase2.hxx>
#include <cppuhelper/basemutex.hxx>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace sd { namespace presenter {

namespace {
    typedef ::cppu::WeakComponentImplHelper2<
        css::lang::XInitialization,
        css::drawing::XSlidePreviewCache
    > PresenterPreviewCacheInterfaceBase;
}

/** Uno API wrapper around the slide preview cache.
*/
class PresenterPreviewCache
    : private ::boost::noncopyable,
      private ::cppu::BaseMutex,
      public PresenterPreviewCacheInterfaceBase
{
public:
    PresenterPreviewCache (const css::uno::Reference<css::uno::XComponentContext>& rxContext);
    virtual ~PresenterPreviewCache (void);

    // XInitialize

    /** Accepts no arguments.  All values that are necessary to set up a
        preview cache can be provided via methods.
    */
    virtual void SAL_CALL initialize (const css::uno::Sequence<css::uno::Any>& rArguments)
        throw(css::uno::Exception,css::uno::RuntimeException);


    // XSlidePreviewCache

    virtual void SAL_CALL setDocumentSlides (
        const css::uno::Reference<css::container::XIndexAccess>& rxSlides,
        const css::uno::Reference<css::uno::XInterface>& rxDocument)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL setVisibleRange (
        sal_Int32 nFirstVisibleSlideIndex,
        sal_Int32 nLastVisibleSlideIndex)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL setPreviewSize (
        const css::geometry::IntegerSize2D& rSize)
        throw (css::uno::RuntimeException);

    virtual css::uno::Reference<css::rendering::XBitmap> SAL_CALL
        getSlidePreview (
            sal_Int32 nSlideIndex,
            const css::uno::Reference<css::rendering::XCanvas>& rxCanvas)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL addPreviewCreationNotifyListener (
        const css::uno::Reference<css::drawing::XSlidePreviewCacheListener>& rxListener)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL removePreviewCreationNotifyListener (
        const css::uno::Reference<css::drawing::XSlidePreviewCacheListener>& rxListener)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL pause (void)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL resume (void)
        throw (css::uno::RuntimeException);

private:
    css::uno::Reference<css::uno::XComponentContext> mxComponentContext;
    class PresenterCacheContext;
    Size maPreviewSize;
    ::boost::shared_ptr<PresenterCacheContext> mpCacheContext;
    ::boost::shared_ptr<sd::slidesorter::cache::PageCache> mpCache;

    /** This method throws a DisposedException when the object has already been
        disposed.
    */
    void ThrowIfDisposed (void) throw (css::lang::DisposedException);
};

} } // end of namespace ::sd::presenter

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
