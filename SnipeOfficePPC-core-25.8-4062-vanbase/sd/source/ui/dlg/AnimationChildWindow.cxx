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


#include "AnimationChildWindow.hxx"

#include "app.hrc"
#include "animobjs.hxx"
#include "animobjs.hrc"
#include <sfx2/app.hxx>
#include <sfx2/childwin.hxx>
#include <sfx2/dockwin.hxx>

namespace sd {

SFX_IMPL_DOCKINGWINDOW_WITHID(AnimationChildWindow, SID_ANIMATION_OBJECTS)

/*************************************************************************
|*
|* Ableitung vom SfxChildWindow als "Behaelter" fuer Animator
|*
\************************************************************************/

AnimationChildWindow::AnimationChildWindow(
    ::Window* _pParent,
    sal_uInt16 nId,
    SfxBindings* pBindings,
    SfxChildWinInfo* pInfo )
    : SfxChildWindow( _pParent, nId )
{
    AnimationWindow* pAnimWin = new AnimationWindow(
        pBindings, this, _pParent, SdResId( FLT_WIN_ANIMATION ) );
    pWindow = pAnimWin;

    eChildAlignment = SFX_ALIGN_NOALIGNMENT;

    pAnimWin->Initialize( pInfo );

    SetHideNotDelete( sal_True );
}

} // end of namespace sd

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
