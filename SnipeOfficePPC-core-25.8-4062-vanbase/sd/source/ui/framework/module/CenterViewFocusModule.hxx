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

#ifndef SD_FRAMEWORK_CENTER_VIEW_FOCUS_MODULE_HXX
#define SD_FRAMEWORK_CENTER_VIEW_FOCUS_MODULE_HXX

#include "MutexOwner.hxx"

#include <com/sun/star/drawing/framework/XConfigurationChangeListener.hpp>
#include <com/sun/star/drawing/framework/XConfigurationController.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <osl/mutex.hxx>
#include <cppuhelper/compbase1.hxx>


namespace {

typedef ::cppu::WeakComponentImplHelper1 <
    ::com::sun::star::drawing::framework::XConfigurationChangeListener
    > CenterViewFocusModuleInterfaceBase;

} // end of anonymous namespace.

namespace sd {

class ViewShellBase;

}




namespace sd { namespace framework {

/** This module waits for new views to be created for the center pane and
    then moves the center view to the top most place on the shell stack.  As
    we are moving away from the shell stack this module may become obsolete
    or has to be modified.
*/
class CenterViewFocusModule
    : private sd::MutexOwner,
      public CenterViewFocusModuleInterfaceBase
{
public:
    CenterViewFocusModule (
        ::com::sun::star::uno::Reference<com::sun::star::frame::XController>& rxController);
    virtual ~CenterViewFocusModule (void);

    virtual void SAL_CALL disposing (void);


    // XConfigurationChangeListener

    virtual void SAL_CALL notifyConfigurationChange (
        const com::sun::star::drawing::framework::ConfigurationChangeEvent& rEvent)
        throw (com::sun::star::uno::RuntimeException);

    // XEventListener

    virtual void SAL_CALL disposing (
        const com::sun::star::lang::EventObject& rEvent)
        throw (com::sun::star::uno::RuntimeException);

private:
    class ViewShellContainer;

    bool mbValid;
    ::com::sun::star::uno::Reference<com::sun::star::drawing::framework::XConfigurationController>
        mxConfigurationController;
    ViewShellBase* mpBase;
    /** This flag indicates whether in the last configuration change cycle a
        new view has been created and thus the center view has to be moved
        to the top of the shell stack.
    */
    bool mbNewViewCreated;

    /** At the end of an update of the current configuration this method
        handles a new view in the center pane by moving the associated view
        shell to the top of the shell stack.
    */
    void HandleNewView(
        const ::com::sun::star::uno::Reference<
            com::sun::star::drawing::framework::XConfiguration>& rxConfiguration);
};

} } // end of namespace sd::framework

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
