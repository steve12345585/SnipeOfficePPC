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

#ifndef TOOLKIT_INC_TOOLKIT_HELPER_IMAGEALIGN_HXX
#define TOOLKIT_INC_TOOLKIT_HELPER_IMAGEALIGN_HXX

#include <sal/types.h>
#include <vcl/button.hxx>

//........................................................................
namespace toolkit
{
//........................................................................

    /** translates a VCL ImageAlign value into an css.awt.ImagePosition value
    */
    sal_Int16 translateImagePosition( ImageAlign _eVCLAlign );

    /** translates a css.awt.ImagePosition value into an VCL ImageAlign
    */
    ImageAlign translateImagePosition( sal_Int16 _nImagePosition );

    /** translates a VCL ImageAlign value into a compatible css.awt.ImageAlign value
    */
    sal_Int16 getCompatibleImageAlign( ImageAlign _eAlign );

    /** translates a css.awt.ImageAlign value into a css.awt.ImagePosition value
    */
    sal_Int16 getExtendedImagePosition( sal_Int16 _nImageAlign );

//........................................................................
} // namespace toolkit
//........................................................................

#endif // TOOLKIT_INC_TOOLKIT_HELPER_IMAGEALIGN_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
