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

#ifndef _STDCTRL_HXX
#define _STDCTRL_HXX

#include "svtools/svtdllapi.h"

#include <vcl/edit.hxx>
#include <vcl/fixed.hxx>

// -------------
// - FixedInfo -
// -------------

class SVT_DLLPUBLIC FixedInfo : public FixedText
{
public:
    FixedInfo( Window* pParent, WinBits nWinStyle = WB_LEFT );
    FixedInfo( Window* pParent, const ResId& rResId );
};

namespace svt
{
    // ----------------------------
    // - svt::SelectableFixedText -
    // ----------------------------

    class SVT_DLLPUBLIC SelectableFixedText : public Edit
    {
    private:
        void    Init();

    public:
                SelectableFixedText( Window* pParent, const ResId& rResId );
        virtual ~SelectableFixedText();

        virtual void    LoseFocus();
    };

} // namespace svt

#endif  // _STDCTRL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
