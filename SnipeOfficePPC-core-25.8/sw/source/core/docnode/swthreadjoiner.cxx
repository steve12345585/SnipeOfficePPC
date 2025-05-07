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
#include <swthreadjoiner.hxx>
#include <finalthreadmanager.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <comphelper/processfactory.hxx>
#include <osl/mutex.hxx>
#include <rtl/instance.hxx>

/** Testing

    @author OD
*/
using namespace ::com::sun::star;

uno::Reference< util::XJobManager > SwThreadJoiner::mpThreadJoiner;

namespace
{
    class theJoinerMutex : public rtl::Static<osl::Mutex, theJoinerMutex> {};
}

uno::Reference< util::XJobManager >& SwThreadJoiner::GetThreadJoiner()
{
    osl::MutexGuard aGuard(theJoinerMutex::get());

    if ( !mpThreadJoiner.is() )
    {
        mpThreadJoiner =
            uno::Reference< util::XJobManager >(
                ::comphelper::getProcessServiceFactory()->createInstance(
                    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.util.JobManager")) ),
                uno::UNO_QUERY );
    }

    return mpThreadJoiner;
}

void SwThreadJoiner::ReleaseThreadJoiner()
{
    mpThreadJoiner.clear();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
