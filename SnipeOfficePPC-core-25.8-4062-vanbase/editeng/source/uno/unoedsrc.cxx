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

#include <svl/brdcst.hxx>

#include <editeng/unoedsrc.hxx>


//------------------------------------------------------------------------

void SvxEditSource::addRange( SvxUnoTextRangeBase* )
{
}

//------------------------------------------------------------------------

void SvxEditSource::removeRange( SvxUnoTextRangeBase* )
{
}

//------------------------------------------------------------------------

const SvxUnoTextRangeBaseList& SvxEditSource::getRanges() const
{
    static SvxUnoTextRangeBaseList gList;
    return gList;
}

//------------------------------------------------------------------------

SvxTextForwarder::~SvxTextForwarder()
{
}

//------------------------------------------------------------------------

SvxViewForwarder::~SvxViewForwarder()
{
}

//------------------------------------------------------------------------

SvxEditSource::~SvxEditSource()
{
}

SvxViewForwarder* SvxEditSource::GetViewForwarder()
{
    return NULL;
}

SvxEditViewForwarder* SvxEditSource::GetEditViewForwarder( sal_Bool )
{
    return NULL;
}

SfxBroadcaster& SvxEditSource::GetBroadcaster() const
{
    OSL_FAIL("SvxEditSource::GetBroadcaster called for implementation missing this feature!");

    static SfxBroadcaster aBroadcaster;

    return aBroadcaster;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
