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


#ifndef SD_PRLTEMPL_HXX
#define SD_PRLTEMPL_HXX

#include "sdresid.hxx"
#include <sfx2/tabdlg.hxx>
#include <svx/tabarea.hxx>

#include "prlayout.hxx" // fuer enum PresentationObjects


class SfxObjectShell;
class SfxStyleSheetBase;
class SfxStyleSheetBasePool;

/*************************************************************************
|*
|* Vorlagen-Tab-Dialog
|*
\************************************************************************/
class SdPresLayoutTemplateDlg : public SfxTabDialog
{
private:
    const SfxObjectShell*   mpDocShell;

    XColorListRef         pColorTab;
    XGradientListRef      pGradientList;
    XHatchListRef         pHatchingList;
    XBitmapListRef        pBitmapList;
    XDashListRef          pDashList;
    XLineEndListRef       pLineEndList;

    sal_uInt16              nPageType;
    sal_uInt16              nDlgType;
    sal_uInt16              nPos;

    ChangeType          nColorTableState;
    ChangeType          nBitmapListState;
    ChangeType          nGradientListState;
    ChangeType          nHatchingListState;

    PresentationObjects ePO;

    virtual void        PageCreated( sal_uInt16 nId, SfxTabPage &rPage );

    // fuers Maping mit dem neuen SvxNumBulletItem
    SfxItemSet aInputSet;
    SfxItemSet* pOutSet;
    const SfxItemSet* pOrgSet;

    sal_uInt16 GetOutlineLevel() const;

    using SfxTabDialog::GetOutputItemSet;

public:
    SdPresLayoutTemplateDlg( SfxObjectShell* pDocSh, Window* pParent, SdResId DlgId, SfxStyleSheetBase& rStyleBase, PresentationObjects ePO, SfxStyleSheetBasePool* pSSPool );
    ~SdPresLayoutTemplateDlg();

    const SfxItemSet* GetOutputItemSet() const;
};


#endif // SD_PRLTEMPL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
