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
#ifndef _SVX_INSCTRL_HXX
#define _SVX_INSCTRL_HXX

#include <sfx2/stbitem.hxx>
#include "svx/svxdllapi.h"

// class SvxInsertToolBoxControl -----------------------------------------

class SVX_DLLPUBLIC SvxInsertStatusBarControl : public SfxStatusBarControl
{
public:
    virtual void    StateChanged( sal_uInt16 nSID, SfxItemState eState,
                                  const SfxPoolItem* pState );
    virtual void    Click();
    virtual void    Paint( const UserDrawEvent& rEvt );

    SFX_DECL_STATUSBAR_CONTROL();

    SvxInsertStatusBarControl( sal_uInt16 nSlotId, sal_uInt16 nId, StatusBar& rStb );
    ~SvxInsertStatusBarControl();

private:
    sal_Bool    bInsert;

#ifdef _SVX_INSCTRL_CXX
    SVX_DLLPRIVATE void DrawItemText_Impl();
#endif
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
