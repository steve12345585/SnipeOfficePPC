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
#ifndef _NUMBERINGTYPELISTBOX_HXX
#define _NUMBERINGTYPELISTBOX_HXX

#include <vcl/lstbox.hxx>
#include "swdllapi.h"

#define INSERT_NUM_TYPE_NO_NUMBERING                0x01
#define INSERT_NUM_TYPE_PAGE_STYLE_NUMBERING        0x02
#define INSERT_NUM_TYPE_BITMAP                      0x04
#define INSERT_NUM_TYPE_BULLET                      0x08
#define INSERT_NUM_EXTENDED_TYPES                   0x10

struct SwNumberingTypeListBox_Impl;

class SW_DLLPUBLIC SwNumberingTypeListBox : public ListBox
{
    SwNumberingTypeListBox_Impl* pImpl;

public:
    SwNumberingTypeListBox( Window* pWin, const ResId& rResId,
        sal_uInt16 nTypeFlags = INSERT_NUM_TYPE_PAGE_STYLE_NUMBERING|INSERT_NUM_TYPE_NO_NUMBERING|INSERT_NUM_EXTENDED_TYPES );
    SwNumberingTypeListBox( Window* pWin, WinBits nStyle = WB_BORDER );
    ~SwNumberingTypeListBox();

    virtual bool set_property(const rtl::OString &rKey, const rtl::OString &rValue);

    void        Reload(sal_uInt16 nTypeFlags);

    sal_Int16   GetSelectedNumberingType();
    sal_Bool    SelectNumberingType(sal_Int16 nType);
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
