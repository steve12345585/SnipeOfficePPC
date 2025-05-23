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

#ifndef SD_FRAMEWORK_SHELL_STACK_GUARD_HXX
#define SD_FRAMEWORK_SHELL_STACK_GUARD_HXX

#include <cppuhelper/basemutex.hxx>

#include "framework/ConfigurationController.hxx"

#include <com/sun/star/drawing/framework/XConfigurationChangeListener.hpp>
#include <com/sun/star/drawing/framework/XConfigurationController.hpp>
#include <com/sun/star/frame/XController.hpp>

#include <vcl/timer.hxx>
#include <cppuhelper/compbase1.hxx>
#include <boost/scoped_ptr.hpp>


namespace {

typedef ::cppu::WeakComponentImplHelper1 <
    css::drawing::framework::XConfigurationChangeListener
    > ShellStackGuardInterfaceBase;

} // end of anonymous namespace.

namespace sd {

class ViewShellBase;

}




namespace sd { namespace framework {

/** This module locks updates of the current configuration in situations
    when the shell stack must not be modified.

    On every start of a configuration update the ShellStackGuard checks the
    printer.  If it is printing the configuration update is locked.  It then
    polls the printer and unlocks updates when printing finishes.

    When in the future there are no resources left that use shells then this
    module can be removed.
*/
class ShellStackGuard
    : private ::cppu::BaseMutex,
      public ShellStackGuardInterfaceBase
{
public:
    ShellStackGuard (css::uno::Reference<css::frame::XController>& rxController);
    virtual ~ShellStackGuard (void);

    virtual void SAL_CALL disposing (void);


    // XConfigurationChangeListener

    virtual void SAL_CALL notifyConfigurationChange (
        const css::drawing::framework::ConfigurationChangeEvent& rEvent)
        throw (css::uno::RuntimeException);

    // XEventListener

    virtual void SAL_CALL disposing (const css::lang::EventObject& rEvent)
        throw (css::uno::RuntimeException);

private:
    css::uno::Reference<css::drawing::framework::XConfigurationController>
        mxConfigurationController;
    ViewShellBase* mpBase;
    ::boost::scoped_ptr<ConfigurationController::Lock> mpUpdateLock;
    Timer maPrinterPollingTimer;

    DECL_LINK(TimeoutHandler, Timer*);

    /** Return <TRUE/> when the printer is printing.  Return <FALSE/> when
        the printer is not printing, or there is no printer, or someting
        else went wrong.
    */
    bool IsPrinting (void) const;
};

} } // end of namespace sd::framework

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
