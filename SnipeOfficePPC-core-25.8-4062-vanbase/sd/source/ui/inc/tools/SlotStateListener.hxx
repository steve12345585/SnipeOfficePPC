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

#ifndef SD_TOOLS_SLOT_STATE_LISTENER_HXX
#define SD_TOOLS_SLOT_STATE_LISTENER_HXX

#include "MutexOwner.hxx"
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/FeatureStateEvent.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <cppuhelper/compbase1.hxx>
#include <tools/link.hxx>

namespace sd { namespace tools {


typedef cppu::WeakComponentImplHelper1<
    ::com::sun::star::frame::XStatusListener
    > SlotStateListenerInterfaceBase;


/** Listen for state changes of slots.  This class has been created in order
    to be informed when the support for vertical writing changes but it can
    be used to relay state changes of other slots as well.
*/
class SlotStateListener
    : protected MutexOwner,
      public SlotStateListenerInterfaceBase
{
public:
    /** This convenience version of the constructor takes all parameters
        that are necessary to observe a single slot.  See descriptions of
        the SetCallback(), ConnectToFrame(), and ObserveSlot() methods for
        explanations about the parameters.
    */
    SlotStateListener (
        Link& rCallback,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::frame::XDispatchProvider>& rxDispatchProvider,
        const ::rtl::OUString& rSlotName);

    /** The constructor de-registers all remaining listeners.  Usually a prior
        dispose() call should have done that already.
    */
    virtual ~SlotStateListener (void);

    /** Set the callback to the given value.  Whenever one of the observed
        slots changes its state this callback is informed about it.
        Changing the callback does not release the listeners.
        @throws DisposedException
    */
    void SetCallback (const Link& rCallback);

    /** Set the frame whose slots shall be observed.  When an object of this
        class is already observing slots of another frame then these
        listeners are released first.
        @throws DisposedException
    */
    void ConnectToDispatchProvider (
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::frame::XDispatchProvider>& rxDispatchProvider);

    /** Observe the slot specified by the given name.  Note that
        ConnectToFrame() has to have been called earlier.
        @param rSlotName
            The name of the slot to observe.  An example is
            ".uno:VerticalTextState".
        @throws DisposedException
    */
    void ObserveSlot (const ::rtl::OUString& rSlotName);

    //=====  frame::XStatusListener  ==========================================

    /** Called by slot state change broadcasters.  In turn the callback is
        informed about the state chage.
        @throws DisposedException
    */
    virtual void SAL_CALL
        statusChanged (
            const ::com::sun::star::frame::FeatureStateEvent& rState)
        throw (::com::sun::star::uno::RuntimeException);

    //=====  lang::XEventListener  ============================================

    virtual void SAL_CALL
        disposing(const com::sun::star::lang::EventObject& rEvent)
        throw(com::sun::star::uno::RuntimeException);

protected:
    /** This method is called by the WeakComponentImplHelper base class in
        reaction to a XComponent::dispose() call.  It releases all currently
        active listeners.
    */
    virtual void SAL_CALL disposing (void);

private:
    Link maCallback;

    /** Remember the URLs that describe slots whose state changes we are
        listening to.
    */
    typedef ::std::vector<com::sun::star::util::URL> RegisteredURLList;
    RegisteredURLList maRegisteredURLList;

    ::com::sun::star::uno::WeakReference<
        ::com::sun::star::frame::XDispatchProvider> mxDispatchProviderWeak;

    /** Deregister all currently active state change listeners.
    */
    void ReleaseListeners (void);

    /** This method throws a DisposedException when the object has already been
        disposed.
    */
    void ThrowIfDisposed (void)
        throw (::com::sun::star::lang::DisposedException);

    /** Transform the given string into a URL object.
    */
    ::com::sun::star::util::URL MakeURL (const ::rtl::OUString& rSlotName) const;

    /** Return an XDispatch object for the given URL.
    */
    ::com::sun::star::uno::Reference<com::sun::star::frame::XDispatch>
        GetDispatch (
            const ::com::sun::star::util::URL& rURL) const;
};

} } // end of namespace ::sd::tools

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
