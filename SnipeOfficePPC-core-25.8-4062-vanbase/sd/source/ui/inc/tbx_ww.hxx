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


#ifndef _SD_TBX_WW_HXX
#define _SD_TBX_WW_HXX

#include <sdresid.hxx>
#include <sfx2/tbxctrl.hxx>

class SdTbxControl : public SfxToolBoxControl
{
private:
    sal_Bool    IsCheckable( sal_uInt16 nSId );

public:
            SFX_DECL_TOOLBOX_CONTROL();

            SdTbxControl(sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx );
            ~SdTbxControl() {}

    virtual SfxPopupWindowType  GetPopupWindowType() const;
    virtual SfxPopupWindow*     CreatePopupWindow();
    virtual void StateChanged( sal_uInt16 nSId, SfxItemState eState,
                                const SfxPoolItem* pState );
};

#endif      // _SD_TBX_WW_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
