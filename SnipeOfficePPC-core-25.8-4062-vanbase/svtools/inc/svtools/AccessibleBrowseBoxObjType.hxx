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


#ifndef _SVTOOLS_ACCESSIBLEBROWSEBOXOBJTYPE_HXX
#define _SVTOOLS_ACCESSIBLEBROWSEBOXOBJTYPE_HXX

// ============================================================================

namespace svt {

// ============================================================================

/** This enumeration contains a constant for each kind of accessible object of
    a BrowseBox. */
enum AccessibleBrowseBoxObjType
{
    BBTYPE_BROWSEBOX,           /// The BrowseBox itself.
    BBTYPE_TABLE,               /// The data table.
    BBTYPE_ROWHEADERBAR,        /// The row header bar ("handle column").
    BBTYPE_COLUMNHEADERBAR,     /// The horizontal column header bar.
    BBTYPE_TABLECELL,           /// A cell of the data table.
    BBTYPE_ROWHEADERCELL,       /// A cell of the row header bar.
    BBTYPE_COLUMNHEADERCELL,    /// A cell of the column header bar.
    BBTYPE_CHECKBOXCELL         /// A cell with a checkbox.
};

// ============================================================================

} // namespace svt

// ============================================================================

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
