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

#include "PresenterFrameworkObserver.hxx"
#include <com/sun/star/lang/IllegalArgumentException.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing::framework;

using ::rtl::OUString;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

namespace sdext { namespace presenter {

PresenterFrameworkObserver::PresenterFrameworkObserver (
    const css::uno::Reference<css::drawing::framework::XConfigurationController>&rxController,
    const OUString& rsEventName,
    const Predicate& rPredicate,
    const Action& rAction)
    : PresenterFrameworkObserverInterfaceBase(m_aMutex),
      mxConfigurationController(rxController),
      maPredicate(rPredicate),
      maAction(rAction)
{
    if ( ! mxConfigurationController.is())
        throw lang::IllegalArgumentException();

    if (mxConfigurationController->hasPendingRequests())
    {
        if (!rsEventName.isEmpty())
        {
            mxConfigurationController->addConfigurationChangeListener(
                this,
                rsEventName,
                Any());
        }
        mxConfigurationController->addConfigurationChangeListener(
            this,
            A2S("ConfigurationUpdateEnd"),
            Any());
    }
    else
    {
        rAction(maPredicate());
    }
}

PresenterFrameworkObserver::~PresenterFrameworkObserver (void)
{
}

void PresenterFrameworkObserver::RunOnUpdateEnd (
    const css::uno::Reference<css::drawing::framework::XConfigurationController>&rxController,
    const Action& rAction)
{
    new PresenterFrameworkObserver(
        rxController,
        OUString(),
        &PresenterFrameworkObserver::True,
        rAction);
}

bool PresenterFrameworkObserver::True (void)
{
    return true;
}

void SAL_CALL PresenterFrameworkObserver::disposing (void)
{
    if ( ! maAction.empty())
        maAction(false);
    Shutdown();
}

void PresenterFrameworkObserver::Shutdown (void)
{
    maAction = Action();
    maPredicate = Predicate();

    if (mxConfigurationController != NULL)
    {
        mxConfigurationController->removeConfigurationChangeListener(this);
        mxConfigurationController = NULL;
    }
}

void SAL_CALL PresenterFrameworkObserver::disposing (const lang::EventObject& rEvent)
    throw (RuntimeException)
{
    if ( ! rEvent.Source.is())
        return;

    if (rEvent.Source == mxConfigurationController)
    {
        mxConfigurationController = NULL;
        if ( ! maAction.empty())
            maAction(false);
    }
}

void SAL_CALL PresenterFrameworkObserver::notifyConfigurationChange (
    const ConfigurationChangeEvent& rEvent)
    throw (RuntimeException)
{
    bool bDispose(false);

    Action aAction (maAction);
    Predicate aPredicate (maPredicate);
    if (rEvent.Type.equals(A2S("ConfigurationUpdateEnd")))
    {
        Shutdown();
        aAction(aPredicate);
        bDispose = true;
    }
    else if (aPredicate())
    {
        Shutdown();
        aAction(true);
        bDispose = true;
    }

    if (bDispose)
    {
        maAction.clear();
        dispose();
    }
}

} }  // end of namespace ::sdext::presenter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
