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

#ifndef _TOOLKIT_HELPER_EMPTYFONTDESCRIPTOR_HXX_
#define _TOOLKIT_HELPER_EMPTYFONTDESCRIPTOR_HXX_

#include <com/sun/star/awt/FontDescriptor.hpp>
#include <com/sun/star/awt/FontSlant.hpp>
#include <com/sun/star/awt/FontUnderline.hpp>
#include <com/sun/star/awt/FontStrikeout.hpp>

//  ----------------------------------------------------
//  class EmptyFontDescriptor
//  ----------------------------------------------------
class EmptyFontDescriptor : public ::com::sun::star::awt::FontDescriptor
{
public:
    EmptyFontDescriptor()
    {
        // Not all enums are initialized correctly in FontDescriptor-CTOR because
        // they are set to the first enum value, this is not allways the default value.
        Slant = ::com::sun::star::awt::FontSlant_DONTKNOW;
        Underline = ::com::sun::star::awt::FontUnderline::DONTKNOW;
        Strikeout = ::com::sun::star::awt::FontStrikeout::DONTKNOW;
    }
};




#endif // _TOOLKIT_HELPER_EMPTYFONTDESCRIPTOR_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
