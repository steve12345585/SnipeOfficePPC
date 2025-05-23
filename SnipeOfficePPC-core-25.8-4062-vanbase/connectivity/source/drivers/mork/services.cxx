/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sal/config.h"

#include "cppuhelper/factory.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "sal/types.h"

#include "MorkDriver.hxx"

namespace
{

static cppu::ImplementationEntry const services[] =
{
    {
        &connectivity::mork::create, &connectivity::mork::getImplementationName,
        &connectivity::mork::getSupportedServiceNames,
        &cppu::createSingleComponentFactory, 0, 0
    },
    { 0, 0, 0, 0, 0, 0 }
};

}

extern "C" SAL_DLLPUBLIC_EXPORT void *SAL_CALL component_getFactory(
    char const *pImplName, void *pServiceManager, void *pRegistryKey)
{
    return cppu::component_getFactoryHelper(
               pImplName, pServiceManager, pRegistryKey, services);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
