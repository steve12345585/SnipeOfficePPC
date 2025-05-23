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

#include <cmdid.h>
#include <swtypes.hxx>
#include <popbox.hxx>
#include <navipi.hxx>

SwHelpToolBox::SwHelpToolBox( SwNavigationPI* pParent, const ResId& rResId )
    : ToolBox( pParent, rResId ),
    DropTargetHelper( this )
{
}

void SwHelpToolBox::MouseButtonDown(const MouseEvent &rEvt)
{
    // If doubleclick is detected use doubleclick handler
    if(rEvt.GetButtons() == MOUSE_RIGHT &&
        0 == GetItemId(rEvt.GetPosPixel()))
    {
        aRightClickLink.Call((MouseEvent *)&rEvt);
    }
    else
        ToolBox::MouseButtonDown(rEvt);
}

long SwHelpToolBox::DoubleClick( ToolBox* pCaller )
{
    // No doubleclick on button
    if( 0 == pCaller->GetCurItemId() && aDoubleClickLink.Call(0) )
        return sal_True;
    return sal_False;
}

SwHelpToolBox::~SwHelpToolBox() {}

sal_Int8 SwHelpToolBox::AcceptDrop( const AcceptDropEvent& rEvt )
{
    return ((SwNavigationPI*)GetParent())->AcceptDrop( rEvt );
}

sal_Int8 SwHelpToolBox::ExecuteDrop( const ExecuteDropEvent& rEvt )
{
    return ((SwNavigationPI*)GetParent())->ExecuteDrop( rEvt );
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
