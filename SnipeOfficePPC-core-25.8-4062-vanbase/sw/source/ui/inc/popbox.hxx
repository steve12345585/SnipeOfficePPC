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
#ifndef _POPBOX_HXX
#define _POPBOX_HXX


#include <vcl/toolbox.hxx>
#include <svtools/transfer.hxx>

class SwNavigationPI;

class SwHelpToolBox: public ToolBox, public DropTargetHelper
{
    Link aDoubleClickLink;
    Link aRightClickLink;       // link gets mouse event as parameter !!!

    using ToolBox::DoubleClick;

protected:
    virtual void MouseButtonDown(const MouseEvent &rEvt);
    virtual long DoubleClick(ToolBox *);
    virtual sal_Int8    AcceptDrop( const AcceptDropEvent& rEvt );
    virtual sal_Int8    ExecuteDrop( const ExecuteDropEvent& rEvt );

public:
    SwHelpToolBox(SwNavigationPI* pParent, const ResId &);
    ~SwHelpToolBox();
    void SetDoubleClickLink(const Link &);      // inline
    void SetRightClickLink(const Link &);       // inline
};

inline void SwHelpToolBox::SetDoubleClickLink(const Link &rLink) {
    aDoubleClickLink = rLink;
}

inline void SwHelpToolBox::SetRightClickLink(const Link &rLink) {
    aRightClickLink = rLink;
}


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
