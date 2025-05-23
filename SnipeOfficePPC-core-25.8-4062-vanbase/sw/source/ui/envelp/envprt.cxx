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

#include <vcl/print.hxx>
#include <svtools/prnsetup.hxx>

#include "swtypes.hxx"
#include "cmdid.h"
#include "envprt.hxx"
#include "envlop.hxx"
#include "uitool.hxx"

#include "envprt.hrc"

SwEnvPrtPage::SwEnvPrtPage(Window* pParent, const SfxItemSet& rSet) :

    SfxTabPage(pParent, SW_RES(TP_ENV_PRT), rSet),

    aAlignBox    (this, SW_RES(BOX_ALIGN  )),
    aTopButton   (this, SW_RES(BTN_TOP    )),
    aBottomButton(this, SW_RES(BTN_BOTTOM )),
    aRightText   (this, SW_RES(TXT_RIGHT  )),
    aRightField  (this, SW_RES(FLD_RIGHT  )),
    aDownText    (this, SW_RES(TXT_DOWN   )),
    aDownField   (this, SW_RES(FLD_DOWN   )),
    aPrinterInfo (this, SW_RES(TXT_PRINTER)),
    aNoNameFL    (this, SW_RES(FL_NONAME )),
    aPrinterFL   (this, SW_RES(FL_PRINTER )),
    aPrtSetup    (this, SW_RES(BTN_PRTSETUP))

{
    FreeResource();
    SetExchangeSupport();

    // Metrics
    FieldUnit eUnit = ::GetDfltMetric(sal_False);
    SetMetric(aRightField, eUnit);
    SetMetric(aDownField , eUnit);

    // Install handlers
    aTopButton   .SetClickHdl(LINK(this, SwEnvPrtPage, ClickHdl));
    aBottomButton.SetClickHdl(LINK(this, SwEnvPrtPage, ClickHdl));

    aPrtSetup    .SetClickHdl(LINK(this, SwEnvPrtPage, ButtonHdl));

    // Bitmaps
    aBottomButton.GetClickHdl().Call(&aBottomButton);

    // ToolBox
    Size aSz = aAlignBox.CalcWindowSizePixel();
    aAlignBox.SetSizePixel(aSz);
    aAlignBox.SetClickHdl(LINK(this, SwEnvPrtPage, AlignHdl));
}

SwEnvPrtPage::~SwEnvPrtPage()
{
}

IMPL_LINK_NOARG(SwEnvPrtPage, ClickHdl)
{
    if (aBottomButton.IsChecked())
    {
        // Envelope from botton
        aAlignBox.SetItemImage(ITM_HOR_LEFT, Bitmap(SW_RES(BMP_HOR_LEFT_LOWER)));
        aAlignBox.SetItemImage(ITM_HOR_CNTR, Bitmap(SW_RES(BMP_HOR_CNTR_LOWER)));
        aAlignBox.SetItemImage(ITM_HOR_RGHT, Bitmap(SW_RES(BMP_HOR_RGHT_LOWER)));
        aAlignBox.SetItemImage(ITM_VER_LEFT, Bitmap(SW_RES(BMP_VER_LEFT_LOWER)));
        aAlignBox.SetItemImage(ITM_VER_CNTR, Bitmap(SW_RES(BMP_VER_CNTR_LOWER)));
        aAlignBox.SetItemImage(ITM_VER_RGHT, Bitmap(SW_RES(BMP_VER_RGHT_LOWER)));
    }
    else
    {
        // Envelope from top
        aAlignBox.SetItemImage(ITM_HOR_LEFT, Bitmap(SW_RES(BMP_HOR_LEFT_UPPER)));
        aAlignBox.SetItemImage(ITM_HOR_CNTR, Bitmap(SW_RES(BMP_HOR_CNTR_UPPER)));
        aAlignBox.SetItemImage(ITM_HOR_RGHT, Bitmap(SW_RES(BMP_HOR_RGHT_UPPER)));
        aAlignBox.SetItemImage(ITM_VER_LEFT, Bitmap(SW_RES(BMP_VER_LEFT_UPPER)));
        aAlignBox.SetItemImage(ITM_VER_CNTR, Bitmap(SW_RES(BMP_VER_CNTR_UPPER)));
        aAlignBox.SetItemImage(ITM_VER_RGHT, Bitmap(SW_RES(BMP_VER_RGHT_UPPER)));
    }
    return 0;
}

IMPL_LINK( SwEnvPrtPage, ButtonHdl, Button *, pBtn )
{
    if (pBtn == &aPrtSetup)
    {
        // Call printer setup
        if (pPrt)
        {
            PrinterSetupDialog* pDlg = new PrinterSetupDialog(this );
            pDlg->SetPrinter(pPrt);
            pDlg->Execute();
            delete pDlg;
            GrabFocus();
            aPrinterInfo.SetText(pPrt->GetName());
        }
    }
    return 0;
}

IMPL_LINK_NOARG(SwEnvPrtPage, AlignHdl)
{
    if (aAlignBox.GetCurItemId())
    {
        for (sal_uInt16 i = ITM_HOR_LEFT; i <= ITM_VER_RGHT; i++)
            aAlignBox.CheckItem(i, sal_False);
        aAlignBox.CheckItem(aAlignBox.GetCurItemId(), sal_True);
    }
    else
    {
        // GetCurItemId() == 0 is possible!
        const SwEnvItem& rItem = (const SwEnvItem&) GetItemSet().Get(FN_ENVELOP);
        aAlignBox.CheckItem((sal_uInt16) rItem.eAlign + ITM_HOR_LEFT, sal_True);
    }
    return 0;
}

SfxTabPage* SwEnvPrtPage::Create(Window* pParent, const SfxItemSet& rSet)
{
    return new SwEnvPrtPage(pParent, rSet);
}

void SwEnvPrtPage::ActivatePage(const SfxItemSet&)
{
    if (pPrt)
        aPrinterInfo.SetText(pPrt->GetName());
}

int SwEnvPrtPage::DeactivatePage(SfxItemSet* _pSet)
{
    if( _pSet )
        FillItemSet(*_pSet);
    return SfxTabPage::LEAVE_PAGE;
}

void SwEnvPrtPage::FillItem(SwEnvItem& rItem)
{
    sal_uInt16 nID = 0;
    for (sal_uInt16 i = ITM_HOR_LEFT; i <= ITM_VER_RGHT && !nID; i++)
        if (aAlignBox.IsItemChecked(i))
            nID = i;

    rItem.eAlign          = (SwEnvAlign) (nID - ITM_HOR_LEFT);
    rItem.bPrintFromAbove = aTopButton.IsChecked();
    rItem.lShiftRight     = static_cast< sal_Int32 >(GetFldVal(aRightField));
    rItem.lShiftDown      = static_cast< sal_Int32 >(GetFldVal(aDownField ));
}

sal_Bool SwEnvPrtPage::FillItemSet(SfxItemSet& rSet)
{
    FillItem(GetParentSwEnvDlg()->aEnvItem);
    rSet.Put(GetParentSwEnvDlg()->aEnvItem);
    return sal_True;
}

void SwEnvPrtPage::Reset(const SfxItemSet& rSet)
{
    // Read item
    const SwEnvItem& rItem = (const SwEnvItem&) rSet.Get(FN_ENVELOP);
    aAlignBox.CheckItem((sal_uInt16) rItem.eAlign + ITM_HOR_LEFT);

    if (rItem.bPrintFromAbove)
        aTopButton   .Check();
    else
        aBottomButton.Check();

    SetFldVal(aRightField, rItem.lShiftRight);
    SetFldVal(aDownField , rItem.lShiftDown );

    ActivatePage(rSet);
    ClickHdl(&aTopButton);
}




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
