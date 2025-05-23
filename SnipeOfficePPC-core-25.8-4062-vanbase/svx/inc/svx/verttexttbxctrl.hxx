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
#ifndef _SVX_VERT_TEXT_TBXCTRL_HXX
#define _SVX_VERT_TEXT_TBXCTRL_HXX

#include <sfx2/tbxctrl.hxx>
#include "svx/svxdllapi.h"

/*
  control to remove/insert cjk settings dependent vertical text toolbox item
 */
class SvxVertCTLTextTbxCtrl : public SfxToolBoxControl
{
    sal_Bool bCheckVertical; //determines whether vertical mode or CTL mode has to be checked
public:
    SvxVertCTLTextTbxCtrl( sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx );
    ~SvxVertCTLTextTbxCtrl();

    virtual void                StateChanged( sal_uInt16 nSID, SfxItemState eState,
                                              const SfxPoolItem* pState );
    void    SetVert(sal_Bool bSet) {bCheckVertical = bSet;}

};

class SVX_DLLPUBLIC SvxCTLTextTbxCtrl : public SvxVertCTLTextTbxCtrl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();
    SvxCTLTextTbxCtrl(sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx );
};

class SVX_DLLPUBLIC SvxVertTextTbxCtrl : public SvxVertCTLTextTbxCtrl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();
    SvxVertTextTbxCtrl( sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
