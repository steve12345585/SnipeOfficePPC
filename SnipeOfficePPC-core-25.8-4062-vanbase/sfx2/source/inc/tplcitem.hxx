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
#ifndef _TPLCITEM_HXX
#define _TPLCITEM_HXX

#include <sfx2/ctrlitem.hxx>
#include <tools/link.hxx>

class SfxCommonTemplateDialog_Impl;

class SfxTemplateControllerItem: public SfxControllerItem {
    SfxCommonTemplateDialog_Impl  &rTemplateDlg;
    sal_uInt8                            nWaterCanState;
    long                            nUserEventId;

    DECL_STATIC_LINK(SfxTemplateControllerItem, SetWaterCanStateHdl_Impl,
                                SfxTemplateControllerItem*);

protected:
    virtual void    StateChanged( sal_uInt16, SfxItemState, const SfxPoolItem* pState );

public:
    SfxTemplateControllerItem( sal_uInt16 nId, SfxCommonTemplateDialog_Impl &rDlg, SfxBindings &);
    ~SfxTemplateControllerItem();
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
