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


#include "vcl/salbtype.hxx"

#include "aqua/salcolorutils.hxx"

// =======================================================================

SalColor GetSalColor( const float* pQuartzColor )
{
    return MAKE_SALCOLOR( sal_uInt8( pQuartzColor[0] * 255.0), sal_uInt8(  pQuartzColor[1] * 255.0 ), sal_uInt8( pQuartzColor[2] * 255.0 ) );
}

void SetSalColor( const SalColor& rColor, float* pQuartzColor )
{
    pQuartzColor[0] = (float) SALCOLOR_RED(rColor) / 255.0;
    pQuartzColor[1] = (float) SALCOLOR_GREEN(rColor) / 255.0;
    pQuartzColor[2] = (float) SALCOLOR_BLUE(rColor) / 255.0;
    pQuartzColor[3] = 1.0;
}

// =======================================================================

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
