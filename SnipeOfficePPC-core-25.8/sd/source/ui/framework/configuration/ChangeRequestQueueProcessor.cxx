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


#include "ChangeRequestQueueProcessor.hxx"
#include "ConfigurationTracer.hxx"

#include "framework/ConfigurationController.hxx"
#include "ConfigurationUpdater.hxx"

#include <vcl/svapp.hxx>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/drawing/framework/XConfiguration.hpp>
#include <com/sun/star/drawing/framework/ConfigurationChangeEvent.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing::framework;

namespace {

#if OSL_DEBUG_LEVEL >= 2

void TraceRequest (const Reference<XConfigurationChangeRequest>& rxRequest)
{
    Reference<container::XNamed> xNamed (rxRequest, UNO_QUERY);
    if (xNamed.is())
        SAL_INFO("sd.fwk", OSL_THIS_FUNC << ":    " <<
            ::rtl::OUStringToOString(xNamed->getName(), RTL_TEXTENCODING_UTF8).getStr());
}

#endif

} // end of anonymous namespace


namespace sd { namespace framework {

ChangeRequestQueueProcessor::ChangeRequestQueueProcessor (
    const ::rtl::Reference<ConfigurationController>& rpConfigurationController,
    const ::boost::shared_ptr<ConfigurationUpdater>& rpConfigurationUpdater)
    : maMutex(),
      maQueue(),
      mnUserEventId(0),
      mxConfiguration(),
      mpConfigurationController(rpConfigurationController),
      mpConfigurationUpdater(rpConfigurationUpdater)
{
}




ChangeRequestQueueProcessor::~ChangeRequestQueueProcessor (void)
{
    if (mnUserEventId != 0)
        Application::RemoveUserEvent(mnUserEventId);
}




void ChangeRequestQueueProcessor::SetConfiguration (
    const Reference<XConfiguration>& rxConfiguration)
{
    ::osl::MutexGuard aGuard (maMutex);

    mxConfiguration = rxConfiguration;
    StartProcessing();
}




void ChangeRequestQueueProcessor::AddRequest (
    const Reference<XConfigurationChangeRequest>& rxRequest)
{
    ::osl::MutexGuard aGuard (maMutex);

#if OSL_DEBUG_LEVEL >= 2
    if (maQueue.empty())
    {
        SAL_INFO("sd.fwk", OSL_THIS_FUNC << ": Adding requests to empty queue");
        ConfigurationTracer::TraceConfiguration(
            mxConfiguration, "current configuration of queue processor");
    }
    SAL_INFO("sd.fwk", OSL_THIS_FUNC << ": Adding request");
    TraceRequest(rxRequest);
#endif

    maQueue.push_back(rxRequest);
    StartProcessing();
}




void ChangeRequestQueueProcessor::StartProcessing (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    if (mnUserEventId == 0
        && mxConfiguration.is()
        && ! maQueue.empty())
    {
        SAL_INFO("sd.fwk", OSL_THIS_FUNC << ": ChangeRequestQueueProcessor scheduling processing");
        mnUserEventId = Application::PostUserEvent(
            LINK(this,ChangeRequestQueueProcessor,ProcessEvent));
    }
}




IMPL_LINK(ChangeRequestQueueProcessor, ProcessEvent, void*, pUnused)
{
    (void)pUnused;

    ::osl::MutexGuard aGuard (maMutex);

    mnUserEventId = 0;

    ProcessOneEvent();

    if ( ! maQueue.empty())
    {
        // Schedule the processing of the next event.
        StartProcessing();
    }

    return 0;
}




void ChangeRequestQueueProcessor::ProcessOneEvent (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    SAL_INFO("sd.fwk", OSL_THIS_FUNC << ": ProcessOneEvent");

    if (mxConfiguration.is()
        && ! maQueue.empty())
    {
        // Get and remove the first entry from the queue.
        Reference<XConfigurationChangeRequest> xRequest (maQueue.front());
        maQueue.pop_front();

        // Execute the change request.
        if (xRequest.is())
        {
#if OSL_DEBUG_LEVEL >= 2
            TraceRequest(xRequest);
#endif
            xRequest->execute(mxConfiguration);
        }

        if (maQueue.empty())
        {
            SAL_INFO("sd.fwk", OSL_THIS_FUNC << ": All requests are processed");
            // The queue is empty so tell the ConfigurationManager to update
            // its state.
            if (mpConfigurationUpdater.get() != NULL)
            {
#if OSL_DEBUG_LEVEL >= 2
                ConfigurationTracer::TraceConfiguration (
                    mxConfiguration, "updating to configuration");
#endif
                mpConfigurationUpdater->RequestUpdate(mxConfiguration);
            }
        }
    }
}




bool ChangeRequestQueueProcessor::IsEmpty (void) const
{
    return maQueue.empty();
}




void ChangeRequestQueueProcessor::ProcessUntilEmpty (void)
{
    while ( ! IsEmpty())
        ProcessOneEvent();
}




void ChangeRequestQueueProcessor::Clear (void)
{
    ::osl::MutexGuard aGuard (maMutex);
    maQueue.clear();
}


} } // end of namespace sd::framework::configuration

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
