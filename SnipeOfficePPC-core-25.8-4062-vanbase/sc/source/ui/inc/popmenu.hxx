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

#ifndef SC_POPMENU_HXX
#define SC_POPMENU_HXX

#include <vcl/menu.hxx>
#include "scdllapi.h"

class SC_DLLPUBLIC ScPopupMenu : public PopupMenu
{
private:
    sal_uInt16  nSel;
    sal_Bool    bHit;
protected:
    virtual void Select();
public:
    ScPopupMenu() : nSel(0),bHit(false) {}
    ScPopupMenu(const ResId& rRes) : PopupMenu(rRes),nSel(0),bHit(false) {}
    sal_uInt16  GetSelected() const { return nSel; }
    sal_Bool    WasHit() const      { return bHit; }
};



#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
