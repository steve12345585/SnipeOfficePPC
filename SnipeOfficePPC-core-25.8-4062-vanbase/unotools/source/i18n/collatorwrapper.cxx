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

#include <unotools/collatorwrapper.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/i18n/Collator.hpp>

using namespace ::com::sun::star;

CollatorWrapper::CollatorWrapper ( const uno::Reference< uno::XComponentContext > &rxContext )
{
    mxInternationalCollator = i18n::Collator::create( rxContext );
}

CollatorWrapper::~CollatorWrapper()
{
}

sal_Int32
CollatorWrapper::compareString (const ::rtl::OUString& s1, const ::rtl::OUString& s2) const
{
    try
    {
        if (mxInternationalCollator.is())
            return mxInternationalCollator->compareString (s1, s2);
    }
    catch (const uno::RuntimeException&)
    {
        SAL_WARN( "unotools.i18n","CollatorWrapper: compareString failed");
    }

    return 0;
}

uno::Sequence< ::rtl::OUString >
CollatorWrapper::listCollatorAlgorithms (const lang::Locale& rLocale) const
{
    try
    {
        if (mxInternationalCollator.is())
            return mxInternationalCollator->listCollatorAlgorithms (rLocale);
    }
    catch (const uno::RuntimeException&)
    {
        SAL_WARN( "unotools.i18n","CollatorWrapper: listCollatorAlgorithms failed");
    }

    return uno::Sequence< ::rtl::OUString > ();
}

sal_Int32
CollatorWrapper::loadDefaultCollator (const lang::Locale& rLocale, sal_Int32 nOptions)
{
    try
    {
        if (mxInternationalCollator.is())
            return mxInternationalCollator->loadDefaultCollator (rLocale, nOptions);
    }
    catch (const uno::RuntimeException&)
    {
        SAL_WARN( "unotools.i18n","CollatorWrapper: loadDefaultCollator failed");
    }

    return 0;
}

sal_Int32
CollatorWrapper::loadCollatorAlgorithm (const ::rtl::OUString& rAlgorithm,
        const lang::Locale& rLocale, sal_Int32 nOptions)
{
    try
    {
        if (mxInternationalCollator.is())
            return mxInternationalCollator->loadCollatorAlgorithm (
                                                        rAlgorithm, rLocale, nOptions);
    }
    catch (const uno::RuntimeException&)
    {
        SAL_WARN( "unotools.i18n","CollatorWrapper: loadCollatorAlgorithm failed");
    }

    return 0;

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
