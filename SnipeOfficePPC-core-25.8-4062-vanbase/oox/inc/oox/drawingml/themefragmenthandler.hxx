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

#ifndef OOX_DRAWINGML_THEMEFRAGMENTHANDLER_HXX
#define OOX_DRAWINGML_THEMEFRAGMENTHANDLER_HXX

#include "oox/core/fragmenthandler2.hxx"
#include "oox/dllapi.h"

namespace oox {
namespace drawingml {

class Theme;

// ============================================================================

class OOX_DLLPUBLIC ThemeFragmentHandler : public ::oox::core::FragmentHandler2
{
public:
    explicit            ThemeFragmentHandler(
                            ::oox::core::XmlFilterBase& rFilter,
                            const ::rtl::OUString& rFragmentPath,
                            Theme& rTheme );
    virtual             ~ThemeFragmentHandler();

    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );

private:
    Theme&              mrTheme;
};

// ============================================================================

} // namespace drawingml
} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
