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


#include <osl/diagnose.h>
#include "delayevent.hxx"

namespace slideshow {
namespace internal {

bool Delay::fire()
{
    OSL_ASSERT( isCharged() );
    if (isCharged()) {
        mbWasFired = true;
        maFunc();
        maFunc.clear(); // early release of payload
    }
    return true;
}

bool Delay::isCharged() const
{
    return !mbWasFired;
}

double Delay::getActivationTime( double nCurrentTime ) const
{
    return nCurrentTime + mnTimeout;
}

void Delay::dispose()
{
    // don't clear unconditionally, because it may currently be executed:
    if (isCharged()) {
        mbWasFired = true;
        maFunc.clear(); // release of payload
    }
}

} // namespace internal
} // namespace presentation

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
