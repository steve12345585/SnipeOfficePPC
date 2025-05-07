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


#include "tools/AsynchronousCall.hxx"

#include "DrawViewShell.hxx"

namespace sd { namespace tools {

AsynchronousCall::AsynchronousCall (void)
    : maTimer(),
      mpFunction()
{
    Link aCallback (LINK(this,AsynchronousCall,TimerCallback));
    maTimer.SetTimeoutHdl(aCallback);
}




AsynchronousCall::~AsynchronousCall (void)
{
    mpFunction.reset();
    maTimer.Stop();
}




void AsynchronousCall::Post (
    const AsynchronousFunction& rFunction,
    sal_uInt32 nTimeoutInMilliseconds)
{
    mpFunction.reset(new AsynchronousFunction(rFunction));
    maTimer.SetTimeout(nTimeoutInMilliseconds);
    maTimer.Start();
}




IMPL_LINK(AsynchronousCall,TimerCallback,Timer*,pTimer)
{
    if (pTimer == &maTimer)
    {
        SAL_WNODEPRECATED_DECLARATIONS_PUSH
        ::std::auto_ptr<AsynchronousFunction> pFunction (mpFunction);
        SAL_WNODEPRECATED_DECLARATIONS_POP
        mpFunction.reset();
        (*pFunction)();
    }
    return 0;
}


} } // end of namespace ::sd::tools

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
