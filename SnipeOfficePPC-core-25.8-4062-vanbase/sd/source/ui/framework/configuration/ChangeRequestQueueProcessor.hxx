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

#ifndef SD_FRAMEWORK_CHANGE_REQUEST_QUEUE_PROCESSOR_HXX
#define SD_FRAMEWORK_CHANGE_REQUEST_QUEUE_PROCESSOR_HXX

#include "ChangeRequestQueue.hxx"
#include <osl/mutex.hxx>
#include <rtl/ref.hxx>
#include <com/sun/star/drawing/framework/XConfigurationChangeRequest.hpp>
#include <com/sun/star/drawing/framework/ConfigurationChangeEvent.hpp>

#include <cppuhelper/interfacecontainer.hxx>
#include <tools/link.hxx>

#include <boost/shared_ptr.hpp>

namespace sd { namespace framework {

class ConfigurationController;
class ConfigurationUpdater;

/** The ChangeRequestQueueProcessor ownes the ChangeRequestQueue and
    processes the configuration change requests.

    When after processing one entry the queue is empty then the
    XConfigurationController::update() method is called so that the changes
    made to the local XConfiguration reference are reflected by the UI.

    Queue entries are processed asynchronously by calling PostUserEvent().
*/
class ChangeRequestQueueProcessor
{
public:
    /** The queue processor is created with a reference to an
        ConfigurationController so that its UpdateConfiguration() method can
        be called when the queue becomes empty.
    */
    ChangeRequestQueueProcessor (
        const ::rtl::Reference<ConfigurationController>& rxController,
        const ::boost::shared_ptr<ConfigurationUpdater>& rpUpdater);
    ~ChangeRequestQueueProcessor (void);

    /** Sets the configuration who will be changed by subsequent change
        requests.  This method should be called only by the configuration
        controller who owns the configuration.
    */
    void SetConfiguration (
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::drawing::framework::XConfiguration>& rxConfiguration);

    /** The given request is appended to the end of the queue and will
        eventually be processed when all other entries in front of it have
        been processed.
    */
    void AddRequest (const ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::framework::XConfigurationChangeRequest>& rxRequest);

    /** Returns </sal_True> when the queue is empty.
    */
    bool IsEmpty (void) const;

    /** Process all events in the queue synchronously.

        <p>This method is typically called when the framework is shut down
        to establish an empty configuration.</p>
    */
    void ProcessUntilEmpty (void);

    /** Process the first event in queue.
    */
    void ProcessOneEvent (void);

    /** Remove all events from the queue.

        <p>This method is typically called when the framework is shut down
        to avoid the processing of still pending activation requests.</p>
    */
    void Clear (void);

private:
    mutable ::osl::Mutex maMutex;

    ChangeRequestQueue maQueue;

    /** The id returned by the last PostUserEvent() call.  This id is stored
        so that a pending user event can be removed whent he queue processor
        is destroyed.
    */
    sal_uIntPtr mnUserEventId;

    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::framework::XConfiguration> mxConfiguration;

    ::rtl::Reference<ConfigurationController> mpConfigurationController;

    ::boost::shared_ptr<ConfigurationUpdater> mpConfigurationUpdater;

    /** Initiate the processing of the entries in the queue.  The actual
        processing starts asynchronously.
    */
    void StartProcessing (void);

    /** Callback function for the PostUserEvent() call.
    */
    DECL_LINK(ProcessEvent,void*);
};


} } // end of namespace sd::framework

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
