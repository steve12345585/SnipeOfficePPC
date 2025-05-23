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


#ifndef _SD_DLGPAGE_HXX
#define _SD_DLGPAGE_HXX

#include <sfx2/tabdlg.hxx>
#include "dlgpage.hrc"

class SfxObjectShell;

typedef sal_uInt16 ChangeType;

/*************************************************************************
|*
|* Seite einrichten-Tab-Dialog
|*
\************************************************************************/
class SdPageDlg : public SfxTabDialog
{
private:
    const SfxObjectShell* mpDocShell;

    XColorListRef         mpColorList;
    XGradientListRef      mpGradientList;
    XHatchListRef         mpHatchingList;
    XBitmapListRef        mpBitmapList;
public:

    SdPageDlg( SfxObjectShell* pDocSh, Window* pParent, const SfxItemSet* pAttr, sal_Bool bAreaPage = sal_True );
    ~SdPageDlg() {};

    virtual void PageCreated(sal_uInt16 nId, SfxTabPage& rPage);
};

#endif // _SD_DLGPAGE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
