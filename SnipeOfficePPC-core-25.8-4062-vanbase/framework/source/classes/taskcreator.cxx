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

#include <classes/taskcreator.hxx>
#include "services/taskcreatorsrv.hxx"
#include <threadhelp/readguard.hxx>
#include <loadenv/targethelper.hxx>
#include <services.h>

#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/beans/NamedValue.hpp>

#include <comphelper/configurationhelper.hxx>
#include <vcl/svapp.hxx>

namespace framework{

/*-****************************************************************************************************//**
    @short      initialize instance with neccessary informations
    @descr      We need a valid uno service manager to create or instanciate new services.
                All other informations to create frames or tasks come in on right interface methods.

    @param      xSMGR
                    points to the valid uno service manager
*//*-*****************************************************************************************************/
TaskCreator::TaskCreator( const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR )
    : ThreadHelpBase(       )
    , m_xSMGR       ( xSMGR )
{
}

/*-****************************************************************************************************//**
    @short      deinitialize instance
    @descr      We should release all used resource which are not needed any longer.
*//*-*****************************************************************************************************/
TaskCreator::~TaskCreator()
{
    m_xSMGR.clear();
}

/*-****************************************************************************************************//**
    TODO document me
*//*-*****************************************************************************************************/
css::uno::Reference< css::frame::XFrame > TaskCreator::createTask( const ::rtl::OUString& sName    ,
                                                                         sal_Bool         bVisible )
{
    /* SAFE { */
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    /* } SAFE */

    css::uno::Reference< css::lang::XSingleServiceFactory > xCreator;
    ::rtl::OUString sCreator = IMPLEMENTATIONNAME_FWK_TASKCREATOR;

    try
    {
        if (
            ( TargetHelper::matchSpecialTarget(sName, TargetHelper::E_BLANK  ) ) ||
            ( TargetHelper::matchSpecialTarget(sName, TargetHelper::E_DEFAULT) )
           )
        {
            ::comphelper::ConfigurationHelper::readDirectKey(
                comphelper::getComponentContext(xSMGR),
                "org.openoffice.Office.TabBrowse",
                "TaskCreatorService",
                "ImplementationName",
                ::comphelper::ConfigurationHelper::E_READONLY) >>= sCreator;
        }

        xCreator = css::uno::Reference< css::lang::XSingleServiceFactory >(
                    xSMGR->createInstance(sCreator), css::uno::UNO_QUERY_THROW);
    }
    catch(const css::uno::Exception&)
    {}

    // no catch here ... without an task creator service we cant open ANY document window within the office.
    // Thats IMHO not a good idea. Then we should accept the stacktrace showing us the real problem.
    // BTW: The used fallback creator service (IMPLEMENTATIONNAME_FWK_TASKCREATOR) is implemented in the same
    // library then these class here ... Why we should not be able to create it ?
    if ( ! xCreator.is())
        xCreator = css::uno::Reference< css::lang::XSingleServiceFactory >(
                    xSMGR->createInstance(IMPLEMENTATIONNAME_FWK_TASKCREATOR), css::uno::UNO_QUERY_THROW);

    css::uno::Sequence< css::uno::Any > lArgs(5);
    css::beans::NamedValue              aArg    ;

    aArg.Name    = rtl::OUString(ARGUMENT_PARENTFRAME);
    aArg.Value <<= css::uno::Reference< css::frame::XFrame >(xSMGR->createInstance(SERVICENAME_DESKTOP), css::uno::UNO_QUERY_THROW);
    lArgs[0]   <<= aArg;

    aArg.Name    = rtl::OUString(ARGUMENT_CREATETOPWINDOW);
    aArg.Value <<= sal_True;
    lArgs[1]   <<= aArg;

    aArg.Name    = rtl::OUString(ARGUMENT_MAKEVISIBLE);
    aArg.Value <<= bVisible;
    lArgs[2]   <<= aArg;

    aArg.Name    = rtl::OUString(ARGUMENT_SUPPORTPERSISTENTWINDOWSTATE);
    aArg.Value <<= sal_True;
    lArgs[3]   <<= aArg;

    aArg.Name    = rtl::OUString(ARGUMENT_FRAMENAME);
    aArg.Value <<= sName;
    lArgs[4]   <<= aArg;

    css::uno::Reference< css::frame::XFrame > xTask(xCreator->createInstanceWithArguments(lArgs), css::uno::UNO_QUERY_THROW);
    return xTask;
}

} // namespace framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
