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

#include <sfx2/module.hxx>
#include <sfx2/dispatch.hxx>
#include <tools/shl.hxx>

#define _SVX_FONTWORK_CXX
#include <svx/svdobj.hxx>
#include <svx/svdopath.hxx>
#include <svx/svdview.hxx>
#include <svx/svdocirc.hxx>
#include <svx/xtextit.hxx>

#include <svx/dialmgr.hxx>
#include "svx/dlgutil.hxx"

#include <svx/dialogs.hrc>
#include "fontwork.hrc"
#include <svx/fontwork.hxx>
#include <editeng/outlobj.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/point/b2dpoint.hxx>

SFX_IMPL_DOCKINGWINDOW_WITHID( SvxFontWorkChildWindow, SID_FONTWORK );

// ControllerItem for Fontwork

SvxFontWorkControllerItem::SvxFontWorkControllerItem
(
    sal_uInt16 _nId,
    SvxFontWorkDialog& rDlg,
    SfxBindings& rBindings
) :

    SfxControllerItem( _nId, rBindings ),

    rFontWorkDlg( rDlg )
{
}

// StateChanged method for FontWork items

void SvxFontWorkControllerItem::StateChanged( sal_uInt16 /*nSID*/, SfxItemState /*eState*/,
                                              const SfxPoolItem* pItem )
{
    switch ( GetId() )
    {
        case SID_FORMTEXT_STYLE:
        {
            const XFormTextStyleItem* pStateItem =
                                PTR_CAST(XFormTextStyleItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextStyleItem expected");
            rFontWorkDlg.SetStyle_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_ADJUST:
        {
            const XFormTextAdjustItem* pStateItem =
                                PTR_CAST(XFormTextAdjustItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextAdjustItem expected");
            rFontWorkDlg.SetAdjust_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_DISTANCE:
        {
            const XFormTextDistanceItem* pStateItem =
                                PTR_CAST(XFormTextDistanceItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextDistanceItem erwartet");
            rFontWorkDlg.SetDistance_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_START:
        {
            const XFormTextStartItem* pStateItem =
                                PTR_CAST(XFormTextStartItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextStartItem expected");
            rFontWorkDlg.SetStart_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_MIRROR:
        {
            const XFormTextMirrorItem* pStateItem =
                                PTR_CAST(XFormTextMirrorItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextMirrorItem expected");
            rFontWorkDlg.SetMirror_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_STDFORM:
        {
            const XFormTextStdFormItem* pStateItem =
                                PTR_CAST(XFormTextStdFormItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextStdFormItem expected");
            rFontWorkDlg.SetStdForm_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_HIDEFORM:
        {
            const XFormTextHideFormItem* pStateItem =
                                PTR_CAST(XFormTextHideFormItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextHideFormItem expected");
            rFontWorkDlg.SetShowForm_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_OUTLINE:
        {
            const XFormTextOutlineItem* pStateItem =
                                PTR_CAST(XFormTextOutlineItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextOutlineItem expected");
            rFontWorkDlg.SetOutline_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_SHADOW:
        {
            const XFormTextShadowItem* pStateItem =
                                PTR_CAST(XFormTextShadowItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextShadowItem expected");
            rFontWorkDlg.SetShadow_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_SHDWCOLOR:
        {
            const XFormTextShadowColorItem* pStateItem =
                                PTR_CAST(XFormTextShadowColorItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextShadowColorItem expected");
            rFontWorkDlg.SetShadowColor_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_SHDWXVAL:
        {
            const XFormTextShadowXValItem* pStateItem =
                                PTR_CAST(XFormTextShadowXValItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextShadowXValItem expected");
            rFontWorkDlg.SetShadowXVal_Impl(pStateItem);
            break;
        }
        case SID_FORMTEXT_SHDWYVAL:
        {
            const XFormTextShadowYValItem* pStateItem =
                                PTR_CAST(XFormTextShadowYValItem, pItem);
            DBG_ASSERT(pStateItem || pItem == 0, "XFormTextShadowYValItem expected");
            rFontWorkDlg.SetShadowYVal_Impl(pStateItem);
            break;
        }
    }
}

// Derivation from SfxChildWindow as "containers" for Fontwork dialog

SvxFontWorkChildWindow::SvxFontWorkChildWindow
(
    Window* _pParent,
    sal_uInt16 nId,
    SfxBindings* pBindings,
    SfxChildWinInfo* pInfo
) :

    SfxChildWindow( _pParent, nId )

{
    pWindow = new SvxFontWorkDialog( pBindings, this, _pParent,
                                     SVX_RES( RID_SVXDLG_FONTWORK ) );
    SvxFontWorkDialog* pDlg = (SvxFontWorkDialog*) pWindow;

    eChildAlignment = SFX_ALIGN_NOALIGNMENT;

    pDlg->Initialize( pInfo );
}

// Floating Window to the attribution of text effects

SvxFontWorkDialog::SvxFontWorkDialog( SfxBindings *pBindinx,
                                      SfxChildWindow *pCW,
                                      Window* _pParent,
                                      const ResId& rResId ) :
    SfxDockingWindow( pBindinx, pCW, _pParent, rResId ),

    aFormSet        (this, ResId(VS_FORMS,*rResId.GetResMgr())),

    aTbxStyle       (this, ResId(TBX_STYLE,*rResId.GetResMgr())),
    aTbxAdjust      (this, ResId(TBX_ADJUST,*rResId.GetResMgr())),

    aFbDistance     (this, ResId(FB_DISTANCE,*rResId.GetResMgr())),
    aMtrFldDistance (this, ResId(MTR_FLD_DISTANCE,*rResId.GetResMgr())),
    aFbTextStart    (this, ResId(FB_TEXTSTART,*rResId.GetResMgr())),
    aMtrFldTextStart(this, ResId(MTR_FLD_TEXTSTART,*rResId.GetResMgr())),

    aTbxShadow      (this, ResId(TBX_SHADOW,*rResId.GetResMgr())),

    aFbShadowX      (this, ResId(FB_SHADOW_X,*rResId.GetResMgr())),
    aMtrFldShadowX  (this, ResId(MTR_FLD_SHADOW_X,*rResId.GetResMgr())),
    aFbShadowY      (this, ResId(FB_SHADOW_Y,*rResId.GetResMgr())),
    aMtrFldShadowY  (this, ResId(MTR_FLD_SHADOW_Y,*rResId.GetResMgr())),

    aShadowColorLB  (this, ResId(CLB_SHADOW_COLOR,*rResId.GetResMgr())),
    rBindings       (*pBindinx),

    nLastStyleTbxId(0),
    nLastAdjustTbxId(0),
    nLastShadowTbxId(0),
    nSaveShadowX    (0),
    nSaveShadowY    (0),
    nSaveShadowAngle(450),
    nSaveShadowSize (100),

    maImageList     (ResId(IL_FONTWORK,*rResId.GetResMgr()))
{
    FreeResource();

    ApplyImageList();

    pCtrlItems[0] = new SvxFontWorkControllerItem(SID_FORMTEXT_STYLE, *this, rBindings);
    pCtrlItems[1] = new SvxFontWorkControllerItem(SID_FORMTEXT_ADJUST, *this, rBindings);
    pCtrlItems[2] = new SvxFontWorkControllerItem(SID_FORMTEXT_DISTANCE, *this, rBindings);
    pCtrlItems[3] = new SvxFontWorkControllerItem(SID_FORMTEXT_START, *this, rBindings);
    pCtrlItems[4] = new SvxFontWorkControllerItem(SID_FORMTEXT_MIRROR, *this, rBindings);
    pCtrlItems[5] = new SvxFontWorkControllerItem(SID_FORMTEXT_STDFORM, *this, rBindings);
    pCtrlItems[6] = new SvxFontWorkControllerItem(SID_FORMTEXT_HIDEFORM, *this, rBindings);
    pCtrlItems[7] = new SvxFontWorkControllerItem(SID_FORMTEXT_OUTLINE, *this, rBindings);
    pCtrlItems[8] = new SvxFontWorkControllerItem(SID_FORMTEXT_SHADOW, *this, rBindings);
    pCtrlItems[9] = new SvxFontWorkControllerItem(SID_FORMTEXT_SHDWCOLOR, *this, rBindings);
    pCtrlItems[10] = new SvxFontWorkControllerItem(SID_FORMTEXT_SHDWXVAL, *this, rBindings);
    pCtrlItems[11] = new SvxFontWorkControllerItem(SID_FORMTEXT_SHDWYVAL, *this, rBindings);

    WinBits aNewStyle = ( aFormSet.GetStyle() | WB_VSCROLL | WB_ITEMBORDER | WB_DOUBLEBORDER );
    aFormSet.SetStyle( aNewStyle );

    Size aSize = aTbxStyle.CalcWindowSizePixel();
    aTbxStyle.SetSizePixel(aSize);
    aTbxStyle.SetSelectHdl( LINK(this, SvxFontWorkDialog, SelectStyleHdl_Impl) );

    aTbxAdjust.SetSizePixel(aSize);
    aTbxAdjust.SetSelectHdl( LINK(this, SvxFontWorkDialog, SelectAdjustHdl_Impl) );

    aTbxShadow.SetSizePixel(aSize);
    aTbxShadow.SetSelectHdl( LINK(this, SvxFontWorkDialog, SelectShadowHdl_Impl) );

    Link aLink = LINK(this, SvxFontWorkDialog, ModifyInputHdl_Impl);
    aMtrFldDistance.SetModifyHdl( aLink );
    aMtrFldTextStart.SetModifyHdl( aLink );
    aMtrFldShadowX.SetModifyHdl( aLink );
    aMtrFldShadowY.SetModifyHdl( aLink );

    // Set System metric
    const FieldUnit eDlgUnit = rBindings.GetDispatcher()->GetModule()->GetFieldUnit();
    SetFieldUnit( aMtrFldDistance, eDlgUnit, sal_True );
    SetFieldUnit( aMtrFldTextStart, eDlgUnit, sal_True );
    SetFieldUnit( aMtrFldShadowX, eDlgUnit, sal_True );
    SetFieldUnit( aMtrFldShadowY, eDlgUnit, sal_True );
    if( eDlgUnit == FUNIT_MM )
    {
        aMtrFldDistance.SetSpinSize( 50 );
        aMtrFldTextStart.SetSpinSize( 50 );
        aMtrFldShadowX.SetSpinSize( 50 );
        aMtrFldShadowY.SetSpinSize( 50 );
    }
    else
    {
        aMtrFldDistance.SetSpinSize( 10 );
        aMtrFldTextStart.SetSpinSize( 10 );
        aMtrFldShadowX.SetSpinSize( 10 );
        aMtrFldShadowY.SetSpinSize( 10 );
    }

    aShadowColorLB.SetSelectHdl( LINK(this, SvxFontWorkDialog, ColorSelectHdl_Impl) );

    aInputTimer.SetTimeout(500);
    aInputTimer.SetTimeoutHdl(LINK(this, SvxFontWorkDialog, InputTimoutHdl_Impl));

    aFormSet.SetSelectHdl( LINK(this, SvxFontWorkDialog, FormSelectHdl_Impl) );
    aFormSet.SetColCount(4);
    aFormSet.SetLineCount(2);

    Bitmap aBmp(SVX_RES(RID_SVXBMP_FONTWORK_FORM1));
    aSize.Height() = aFormSet.CalcWindowSizePixel(aBmp.GetSizePixel()).Height() + 2;
    aFormSet.SetSizePixel(aSize);
}

SvxFontWorkDialog::~SvxFontWorkDialog()
{
    for (sal_uInt16 i = 0; i < CONTROLLER_COUNT; i++)
        DELETEZ(pCtrlItems[i]);
}

void SvxFontWorkDialog::Zoom()
{
    SfxDockingWindow::Roll();
}

SfxChildAlignment SvxFontWorkDialog::CheckAlignment( SfxChildAlignment eActAlign,
                                                     SfxChildAlignment eAlign )
{
    SfxChildAlignment eAlignment;

    switch ( eAlign )
    {
        case SFX_ALIGN_TOP:
        case SFX_ALIGN_HIGHESTTOP:
        case SFX_ALIGN_LOWESTTOP:
        case SFX_ALIGN_BOTTOM:
        case SFX_ALIGN_LOWESTBOTTOM:
        case SFX_ALIGN_HIGHESTBOTTOM:
        {
            eAlignment = eActAlign;
        }
        break;

        case SFX_ALIGN_LEFT:
        case SFX_ALIGN_RIGHT:
        case SFX_ALIGN_FIRSTLEFT:
        case SFX_ALIGN_LASTLEFT:
        case SFX_ALIGN_FIRSTRIGHT:
        case SFX_ALIGN_LASTRIGHT:
        {
            eAlignment = eAlign;
        }
        break;

        default:
        {
            eAlignment = eAlign;
        }
        break;
    }

    return eAlignment;
}

// Set style buttons

void SvxFontWorkDialog::SetStyle_Impl(const XFormTextStyleItem* pItem)
{
    if ( pItem )
    {
        sal_uInt16 nId = TBI_STYLE_OFF;

        switch ( pItem->GetValue() )
        {
            case XFT_ROTATE : nId = TBI_STYLE_ROTATE;   break;
            case XFT_UPRIGHT: nId = TBI_STYLE_UPRIGHT;  break;
            case XFT_SLANTX : nId = TBI_STYLE_SLANTX;   break;
            case XFT_SLANTY : nId = TBI_STYLE_SLANTY;   break;
            default: ;//prevent warning
        }
        aTbxStyle.Enable();

        // Make sure that there is allways exactly one checked toolbox item.
        if ( pItem->GetValue() == XFT_NONE )
        {
            aTbxStyle.CheckItem(TBI_STYLE_ROTATE, sal_False);
            aTbxStyle.CheckItem(TBI_STYLE_UPRIGHT, sal_False);
            aTbxStyle.CheckItem(TBI_STYLE_SLANTX, sal_False);
            aTbxStyle.CheckItem(TBI_STYLE_SLANTY, sal_False);

            aTbxStyle.CheckItem(TBI_STYLE_OFF, sal_True);
        }
        else
        {
            aTbxStyle.CheckItem(TBI_STYLE_OFF, sal_False);
            aTbxStyle.CheckItem(nId);
        }

        nLastStyleTbxId = nId;
    }
    else
        aTbxStyle.Disable();
}

// Set adjust buttons

void SvxFontWorkDialog::SetAdjust_Impl(const XFormTextAdjustItem* pItem)
{
    if ( pItem )
    {
        sal_uInt16 nId;

        aTbxAdjust.Enable();
        aMtrFldDistance.Enable();

        if ( pItem->GetValue() == XFT_LEFT || pItem->GetValue() == XFT_RIGHT )
        {
            if ( pItem->GetValue() == XFT_LEFT )    nId = TBI_ADJUST_LEFT;
            else                                    nId = TBI_ADJUST_RIGHT;
            aMtrFldTextStart.Enable();
        }
        else
        {
            if ( pItem->GetValue() == XFT_CENTER )  nId = TBI_ADJUST_CENTER;
            else                                    nId = TBI_ADJUST_AUTOSIZE;
            aMtrFldTextStart.Disable();
        }

        if ( !aTbxAdjust.IsItemChecked(nId) )
        {
            aTbxAdjust.CheckItem(nId);
        }
        nLastAdjustTbxId = nId;
    }
    else
    {
        aTbxAdjust.Disable();
        aMtrFldTextStart.Disable();
        aMtrFldDistance.Disable();
    }
}

// Enter Distance value in the edit field

void SvxFontWorkDialog::SetDistance_Impl(const XFormTextDistanceItem* pItem)
{
    // Use HasChildPathFocus() instead of HasFocus() at SpinFields
    if ( pItem && !aMtrFldDistance.HasChildPathFocus() )
    {
        SetMetricValue( aMtrFldDistance, pItem->GetValue(), SFX_MAPUNIT_100TH_MM );
    }
}

// Enter indent value in the edit field

void SvxFontWorkDialog::SetStart_Impl(const XFormTextStartItem* pItem)
{
    // Use HasChildPathFocus() instead of HasFocus() at SpinFields
    if ( pItem && !aMtrFldTextStart.HasChildPathFocus() )
    {
        SetMetricValue( aMtrFldTextStart, pItem->GetValue(), SFX_MAPUNIT_100TH_MM );
    }
}

// Set button for reversing the direction of text

void SvxFontWorkDialog::SetMirror_Impl(const XFormTextMirrorItem* pItem)
{
    if ( pItem )
        aTbxAdjust.CheckItem(TBI_ADJUST_MIRROR, pItem->GetValue());
}

// Standard form in ValueSet View

void SvxFontWorkDialog::SetStdForm_Impl(const XFormTextStdFormItem* pItem)
{
    if ( pItem )
    {
        aFormSet.Enable();
        aFormSet.SetNoSelection();

        if ( pItem->GetValue() != XFTFORM_NONE )
            aFormSet.SelectItem(
                sal::static_int_cast< sal_uInt16 >(pItem->GetValue()));
    }
    else
        aFormSet.Disable();
}

// Set button for contour display

void SvxFontWorkDialog::SetShowForm_Impl(const XFormTextHideFormItem* pItem)
{
    if ( pItem )
        aTbxShadow.CheckItem(TBI_SHOWFORM, !pItem->GetValue());
}

// Set button for text border

void SvxFontWorkDialog::SetOutline_Impl(const XFormTextOutlineItem* pItem)
{
    if ( pItem )
        aTbxShadow.CheckItem(TBI_OUTLINE, pItem->GetValue());
}

// Set shadow buttons

void SvxFontWorkDialog::SetShadow_Impl(const XFormTextShadowItem* pItem,
                                        sal_Bool bRestoreValues)
{
    if ( pItem )
    {
        sal_uInt16 nId;

        aTbxShadow.Enable();

        if ( pItem->GetValue() == XFTSHADOW_NONE )
        {
            nId = TBI_SHADOW_OFF;
            aFbShadowX.Hide();
            aFbShadowY.Hide();
            aMtrFldShadowX.Disable();
            aMtrFldShadowY.Disable();
            aShadowColorLB.Disable();
        }
        else
        {
            aFbShadowX.Show();
            aFbShadowY.Show();
            aMtrFldShadowX.Enable();
            aMtrFldShadowY.Enable();
            aShadowColorLB.Enable();

            if ( pItem->GetValue() == XFTSHADOW_NORMAL )
            {
                nId = TBI_SHADOW_NORMAL;
                const FieldUnit eDlgUnit = rBindings.GetDispatcher()->GetModule()->GetFieldUnit();

                aMtrFldShadowX.SetUnit( eDlgUnit );
                aMtrFldShadowX.SetDecimalDigits(2);
                aMtrFldShadowX.SetMin(LONG_MIN);
                aMtrFldShadowX.SetMax(LONG_MAX);
                if( eDlgUnit == FUNIT_MM )
                    aMtrFldShadowX.SetSpinSize( 50 );
                else
                    aMtrFldShadowX.SetSpinSize( 10 );

                aMtrFldShadowY.SetUnit( eDlgUnit );
                aMtrFldShadowY.SetDecimalDigits(2);
                aMtrFldShadowY.SetMin(LONG_MIN);
                aMtrFldShadowY.SetMax(LONG_MAX);
                if( eDlgUnit == FUNIT_MM )
                    aMtrFldShadowY.SetSpinSize( 50 );
                else
                    aMtrFldShadowY.SetSpinSize( 10 );

                if ( bRestoreValues )
                {
                    SetMetricValue( aMtrFldShadowX, nSaveShadowX, SFX_MAPUNIT_100TH_MM );
                    SetMetricValue( aMtrFldShadowY, nSaveShadowY, SFX_MAPUNIT_100TH_MM );

                    XFormTextShadowXValItem aXItem( nSaveShadowX );
                    XFormTextShadowYValItem aYItem( nSaveShadowY );

                    GetBindings().GetDispatcher()->Execute(
                        SID_FORMTEXT_SHDWXVAL, SFX_CALLMODE_RECORD, &aXItem, &aYItem, 0L );
                }
            }
            else
            {
                nId = TBI_SHADOW_SLANT;

                aMtrFldShadowX.SetUnit(FUNIT_CUSTOM);
                aMtrFldShadowX.SetDecimalDigits(1);
                aMtrFldShadowX.SetMin(-1800);
                aMtrFldShadowX.SetMax( 1800);
                aMtrFldShadowX.SetSpinSize(10);

                aMtrFldShadowY.SetUnit(FUNIT_CUSTOM);
                aMtrFldShadowY.SetDecimalDigits(0);
                aMtrFldShadowY.SetMin(-999);
                aMtrFldShadowY.SetMax( 999);
                aMtrFldShadowY.SetSpinSize(10);

                if ( bRestoreValues )
                {
                    aMtrFldShadowX.SetValue(nSaveShadowAngle);
                    aMtrFldShadowY.SetValue(nSaveShadowSize);
                    XFormTextShadowXValItem aXItem(nSaveShadowAngle);
                    XFormTextShadowYValItem aYItem(nSaveShadowSize);
                    GetBindings().GetDispatcher()->Execute(
                        SID_FORMTEXT_SHDWXVAL, SFX_CALLMODE_RECORD, &aXItem, &aYItem, 0L );
                }
            }
        }

        if ( !aTbxShadow.IsItemChecked(nId) )
        {
            aTbxShadow.CheckItem(nId);
        }
        nLastShadowTbxId = nId;

        ApplyImageList();
    }
    else
    {
        aTbxShadow.Disable();
        aMtrFldShadowX.Disable();
        aMtrFldShadowY.Disable();
        aShadowColorLB.Disable();
    }
}

// Insert shadow color in listbox

void SvxFontWorkDialog::SetShadowColor_Impl(const XFormTextShadowColorItem* pItem)
{
    if ( pItem )
        aShadowColorLB.SelectEntry(pItem->GetColorValue());
}

// Enter X-value for shadow in edit field

void SvxFontWorkDialog::SetShadowXVal_Impl(const XFormTextShadowXValItem* pItem)
{
    // Use HasChildPathFocus() instead of HasFocus() at SpinFields
    if ( pItem && !aMtrFldShadowX.HasChildPathFocus() )
    {
        // #i19251#
        // sal_Int32 nValue = pItem->GetValue();

        // #i19251#
        // The two involved fields/items are used double and contain/give different
        // values regarding to the access method. Thus, here we need to separate the access
        // methos regarding to the kind of value accessed.
        if(aTbxShadow.IsItemChecked(TBI_SHADOW_SLANT))
        {
            // #i19251#
            // There is no value correction necessary at all, i think this
            // was only tried to be done without understanding that the two
            // involved fields/items are used double and contain/give different
            // values regarding to the access method.
            // nValue = nValue - ( int( float( nValue ) / 360.0 ) * 360 );
            aMtrFldShadowX.SetValue(pItem->GetValue());
        }
        else
        {
            SetMetricValue( aMtrFldShadowX, pItem->GetValue(), SFX_MAPUNIT_100TH_MM );
        }
    }
}

// Enter Y-value for shadow in edit field

void SvxFontWorkDialog::SetShadowYVal_Impl(const XFormTextShadowYValItem* pItem)
{
    // Use HasChildPathFocus() instead of HasFocus() at SpinFields
    if ( pItem && !aMtrFldShadowY.HasChildPathFocus() )
    {
        // #i19251#
        // The two involved fields/items are used double and contain/give different
        // values regarding to the access method. Thus, here we need to separate the access
        // methos regarding to the kind of value accessed.
        if(aTbxShadow.IsItemChecked(TBI_SHADOW_SLANT))
        {
            aMtrFldShadowY.SetValue(pItem->GetValue());
        }
        else
        {
            SetMetricValue( aMtrFldShadowY, pItem->GetValue(), SFX_MAPUNIT_100TH_MM );
        }
    }
}

IMPL_LINK_NOARG(SvxFontWorkDialog, SelectStyleHdl_Impl)
{
    sal_uInt16 nId = aTbxStyle.GetCurItemId();

    // Execute this block when a different toolbox item has been clicked or
    // when the off item has been clicked.  The later is necessary to
    // override the toolbox behaviour of unchecking the item after second
    // click on it: One of the items has to be checked at all times (when
    // enabled that is.)
    if (nId == TBI_STYLE_OFF || nId != nLastStyleTbxId )
    {
        XFormTextStyle eStyle = XFT_NONE;

        switch ( nId )
        {
            case TBI_STYLE_ROTATE   : eStyle = XFT_ROTATE;  break;
            case TBI_STYLE_UPRIGHT  : eStyle = XFT_UPRIGHT; break;
            case TBI_STYLE_SLANTX   : eStyle = XFT_SLANTX;  break;
            case TBI_STYLE_SLANTY   : eStyle = XFT_SLANTY;  break;
        }
        XFormTextStyleItem aItem( eStyle );
        GetBindings().GetDispatcher()->Execute( SID_FORMTEXT_STYLE, SFX_CALLMODE_RECORD, &aItem, 0L );
        SetStyle_Impl( &aItem );
        nLastStyleTbxId = nId;
    }
    return 0;
}

IMPL_LINK_NOARG(SvxFontWorkDialog, SelectAdjustHdl_Impl)
{
    sal_uInt16 nId = aTbxAdjust.GetCurItemId();

    if ( nId == TBI_ADJUST_MIRROR )
    {
        XFormTextMirrorItem aItem(aTbxAdjust.IsItemChecked(nId));
        GetBindings().GetDispatcher()->Execute( SID_FORMTEXT_MIRROR, SFX_CALLMODE_SLOT, &aItem, 0L );
    }
    else if ( nId != nLastAdjustTbxId )
    {
        XFormTextAdjust eAdjust = XFT_AUTOSIZE;

        switch ( nId )
        {
            case TBI_ADJUST_LEFT    : eAdjust = XFT_LEFT;   break;
            case TBI_ADJUST_CENTER  : eAdjust = XFT_CENTER; break;
            case TBI_ADJUST_RIGHT   : eAdjust = XFT_RIGHT;  break;
        }
        XFormTextAdjustItem aItem(eAdjust);
        GetBindings().GetDispatcher()->Execute( SID_FORMTEXT_ADJUST, SFX_CALLMODE_RECORD, &aItem, 0L );
        SetAdjust_Impl(&aItem);
        nLastAdjustTbxId = nId;
    }
    return 0;
}

IMPL_LINK_NOARG(SvxFontWorkDialog, SelectShadowHdl_Impl)
{
    sal_uInt16 nId = aTbxShadow.GetCurItemId();

    if ( nId == TBI_SHOWFORM )
    {
        XFormTextHideFormItem aItem(!aTbxShadow.IsItemChecked(nId));
        GetBindings().GetDispatcher()->Execute( SID_FORMTEXT_HIDEFORM, SFX_CALLMODE_RECORD, &aItem, 0L );
    }
    else if ( nId == TBI_OUTLINE )
    {
        XFormTextOutlineItem aItem(aTbxShadow.IsItemChecked(nId));
        GetBindings().GetDispatcher()->Execute( SID_FORMTEXT_OUTLINE, SFX_CALLMODE_RECORD, &aItem, 0L );
    }
    else if ( nId != nLastShadowTbxId )
    {
        XFormTextShadow eShadow = XFTSHADOW_NONE;

        if ( nLastShadowTbxId == TBI_SHADOW_NORMAL )
        {
            nSaveShadowX = GetCoreValue( aMtrFldShadowX, SFX_MAPUNIT_100TH_MM );
            nSaveShadowY = GetCoreValue( aMtrFldShadowY, SFX_MAPUNIT_100TH_MM );
        }
        else if ( nLastShadowTbxId == TBI_SHADOW_SLANT )
        {
            nSaveShadowAngle = static_cast<long>(aMtrFldShadowX.GetValue());
            nSaveShadowSize  = static_cast<long>(aMtrFldShadowY.GetValue());
        }
        nLastShadowTbxId = nId;

        if ( nId == TBI_SHADOW_NORMAL )     eShadow = XFTSHADOW_NORMAL;
        else if ( nId == TBI_SHADOW_SLANT ) eShadow = XFTSHADOW_SLANT;

        XFormTextShadowItem aItem(eShadow);
        GetBindings().GetDispatcher()->Execute( SID_FORMTEXT_SHADOW, SFX_CALLMODE_RECORD, &aItem, 0L );
        SetShadow_Impl(&aItem, sal_True);
    }
    return 0;
}

IMPL_LINK_NOARG_INLINE_START(SvxFontWorkDialog, ModifyInputHdl_Impl)
{
    aInputTimer.Start();
    return 0;
}
IMPL_LINK_NOARG_INLINE_END(SvxFontWorkDialog, ModifyInputHdl_Impl)

IMPL_LINK_NOARG(SvxFontWorkDialog, InputTimoutHdl_Impl)
{
    // Possibly set the Metric system again. This should be done with a
    // listen, this is however not possible at the moment due to compabillity
    // issues.
    const FieldUnit eDlgUnit = rBindings.GetDispatcher()->GetModule()->GetFieldUnit();
    if( eDlgUnit != aMtrFldDistance.GetUnit() )
    {
        SetFieldUnit( aMtrFldDistance, eDlgUnit, sal_True );
        SetFieldUnit( aMtrFldTextStart, eDlgUnit, sal_True );
        aMtrFldDistance.SetSpinSize( eDlgUnit == FUNIT_MM ? 50 : 10 );
        aMtrFldTextStart.SetSpinSize( eDlgUnit == FUNIT_MM ? 50 : 10 );
    }
    if( eDlgUnit != aMtrFldShadowX.GetUnit() &&
        aTbxShadow.IsItemChecked( TBI_SHADOW_NORMAL ) )
    {
        SetFieldUnit( aMtrFldShadowX, eDlgUnit, sal_True );
        SetFieldUnit( aMtrFldShadowY, eDlgUnit, sal_True );
        aMtrFldShadowX.SetSpinSize( eDlgUnit == FUNIT_MM ? 50 : 10 );
        aMtrFldShadowY.SetSpinSize( eDlgUnit == FUNIT_MM ? 50 : 10 );
    }

    long nValue = GetCoreValue( aMtrFldDistance, SFX_MAPUNIT_100TH_MM );
    XFormTextDistanceItem aDistItem( nValue );
    nValue = GetCoreValue( aMtrFldTextStart, SFX_MAPUNIT_100TH_MM );
    XFormTextStartItem aStartItem( nValue );

    sal_Int32 nValueX(0L);
    sal_Int32 nValueY(0L);

    // #i19251#
    // The two involved fields/items are used double and contain/give different
    // values regarding to the access method. Thus, here we need to separate the access
    // methos regarding to the kind of value accessed.
    if(nLastShadowTbxId == TBI_SHADOW_NORMAL)
    {
        nValueX = GetCoreValue( aMtrFldShadowX, SFX_MAPUNIT_100TH_MM );
        nValueY = GetCoreValue( aMtrFldShadowY, SFX_MAPUNIT_100TH_MM );
    }
    else if(nLastShadowTbxId == TBI_SHADOW_SLANT)
    {
        nValueX = static_cast<long>(aMtrFldShadowX.GetValue());
        nValueY = static_cast<long>(aMtrFldShadowY.GetValue());
    }

    XFormTextShadowXValItem aShadowXItem( nValueX );
    XFormTextShadowYValItem aShadowYItem( nValueY );

    // Slot-ID does not matter, the Exec method evaluates the entire item set
    GetBindings().GetDispatcher()->Execute( SID_FORMTEXT_DISTANCE, SFX_CALLMODE_RECORD, &aDistItem,
                                            &aStartItem, &aShadowXItem, &aShadowYItem, 0L );
    return 0;
}

IMPL_LINK_NOARG(SvxFontWorkDialog, FormSelectHdl_Impl)
{
    XFormTextStdFormItem aItem;

    if ( aFormSet.IsNoSelection() )
        aItem.SetValue(XFTFORM_NONE);
    else
        aItem.SetValue(aFormSet.GetSelectItemId());
    GetBindings().GetDispatcher()->Execute( SID_FORMTEXT_STDFORM, SFX_CALLMODE_RECORD, &aItem, 0L );
    aFormSet.SetNoSelection();
    return 0;
}

IMPL_LINK_NOARG(SvxFontWorkDialog, ColorSelectHdl_Impl)
{
    XFormTextShadowColorItem aItem( (const String &) String(),
                                    (const Color &) aShadowColorLB.GetSelectEntryColor() );
    GetBindings().GetDispatcher()->Execute( SID_FORMTEXT_SHDWCOLOR, SFX_CALLMODE_RECORD, &aItem, 0L );
    return 0;
}

void SvxFontWorkDialog::SetColorList(const XColorListRef &pList)
{
    if ( pList.is() && pList != pColorList )
    {
        pColorList = pList;
        aShadowColorLB.Clear();
        aShadowColorLB.Fill(pColorList);
    }
}

void SvxFontWorkDialog::SetActive(sal_Bool /*bActivate*/)
{
}

// Create standard fontWork Object

void SvxFontWorkDialog::CreateStdFormObj(SdrView& rView, SdrPageView& rPV,
                                         const SfxItemSet& rAttr,
                                         SdrObject& rOldObj,
                                         XFormTextStdForm eForm)
{
    SfxItemSet  aAttr(*rAttr.GetPool(), XATTR_FORMTXTSTYLE,
                                        XATTR_FORMTXTHIDEFORM);
    SdrObject*  pNewObj = NULL;
    Rectangle   aRect;
    XFormTextAdjust eAdjust = XFT_AUTOSIZE;

    aAttr.Put(rOldObj.GetMergedItemSet());

    const XFormTextStdFormItem& rOldForm = (const XFormTextStdFormItem&)
                                            aAttr.Get(XATTR_FORMTXTSTDFORM);

    aRect = rOldObj.GetSnapRect();

    if ( !rOldObj.ISA(SdrPathObj) )
    {
        Point aPos = aRect.TopLeft();
        aRect = rOldObj.GetLogicRect();
        aRect.SetPos(aPos);
    }

    aAttr.Put(rAttr);

    if ( rOldForm.GetValue() == XFTFORM_NONE )
    {
        long nW = aRect.GetWidth();
        aRect.SetSize(Size(nW, nW));
        aAttr.Put(XFormTextStyleItem(XFT_ROTATE));
    }

    Point aCenter = aRect.Center();

    switch ( eForm )
    {
        case XFTFORM_TOPCIRC:
        case XFTFORM_BOTCIRC:
        case XFTFORM_LFTCIRC:
        case XFTFORM_RGTCIRC:
        case XFTFORM_TOPARC:
        case XFTFORM_BOTARC:
        case XFTFORM_LFTARC:
        case XFTFORM_RGTARC:
        {
            long nBeg, nEnd;

            switch ( eForm )
            {
                default: ; //prevent warning
                case XFTFORM_TOPCIRC:
                    nBeg = 0;
                    nEnd = 18000;
                    break;
                case XFTFORM_BOTCIRC:
                    nBeg = 18000;
                    nEnd = 36000;
                    break;
                case XFTFORM_LFTCIRC:
                    nBeg =  9000;
                    nEnd = 27000;
                    break;
                case XFTFORM_RGTCIRC:
                    nBeg = 27000;
                    nEnd =  9000;
                    break;
                case XFTFORM_TOPARC:
                    nBeg =  4500;
                    nEnd = 13500;
                    break;
                case XFTFORM_BOTARC:
                    nBeg = 22500;
                    nEnd = 31500;
                    break;
                case XFTFORM_LFTARC:
                    nBeg = 13500;
                    nEnd = 22500;
                    break;
                case XFTFORM_RGTARC:
                    nBeg = 31500;
                    nEnd =  4500;
                    break;
            }
            pNewObj = new SdrCircObj(OBJ_CARC, aRect, nBeg, nEnd);
            break;
        }
        case XFTFORM_BUTTON1:
        {
            basegfx::B2DPolyPolygon aPolyPolygon;
            basegfx::B2DPolygon aLine;
            long nR = aRect.GetWidth() / 2;
            basegfx::B2DPolygon aTopArc(XPolygon(aCenter, -nR, nR, 50, 1750, sal_False).getB2DPolygon());
            basegfx::B2DPolygon aBottomArc(XPolygon(aCenter, -nR, nR, 1850, 3550, sal_False).getB2DPolygon());

            // Close Polygon
            aTopArc.setClosed(true);
            aBottomArc.setClosed(true);
            aPolyPolygon.append(aTopArc);

            aLine.append(aBottomArc.getB2DPoint(aBottomArc.count() - 1L));
            aLine.append(aBottomArc.getB2DPoint(0L));
            aLine.setClosed(true);

            aPolyPolygon.append(aLine);
            aPolyPolygon.append(aBottomArc);

            pNewObj = new SdrPathObj(OBJ_PATHFILL, aPolyPolygon);
            eAdjust = XFT_CENTER;
            break;
        }
        case XFTFORM_BUTTON2:
        case XFTFORM_BUTTON3:
        case XFTFORM_BUTTON4:
        {
            basegfx::B2DPolyPolygon aPolyPolygon;
            basegfx::B2DPolygon aLine;
            long nR = aRect.GetWidth() / 2;
            long nWDiff = nR / 5;
            long nHDiff;

            if ( eForm == XFTFORM_BUTTON4 )
            {
                basegfx::B2DPolygon aNewArc(XPolygon(aCenter, -nR, nR, 950, 2650, sal_False).getB2DPolygon());
                aNewArc.setClosed(true);
                aPolyPolygon.append(aNewArc);
                eAdjust = XFT_CENTER;
            }
            else
            {
                basegfx::B2DPolygon aNewArc(XPolygon(aCenter, -nR, nR, 2700, 2700).getB2DPolygon());
                aPolyPolygon.append(aNewArc);
            }

            if ( eForm == XFTFORM_BUTTON3 )
                nHDiff = -aRect.GetHeight() / 10;
            else
                nHDiff = aRect.GetHeight() / 20;

            aLine.append(basegfx::B2DPoint(aRect.Left() + nWDiff, aRect.Center().Y() + nHDiff));
            aLine.append(basegfx::B2DPoint(aRect.Right() - nWDiff, aRect.Center().Y() + nHDiff));
            aLine.setClosed(true);
            aPolyPolygon.append(aLine);

            if ( eForm == XFTFORM_BUTTON4 )
            {
                basegfx::B2DPolygon aNewArc(XPolygon(aCenter, -nR, nR, 2750, 850, sal_False).getB2DPolygon());
                aNewArc.setClosed(true);
                aPolyPolygon.append(aNewArc);
            }

            if ( eForm == XFTFORM_BUTTON3 )
            {
                nHDiff += nHDiff;
                aLine.setB2DPoint(0L, basegfx::B2DPoint(aLine.getB2DPoint(0L).getX(), aLine.getB2DPoint(0L).getY() - nHDiff));
                aLine.setB2DPoint(1L, basegfx::B2DPoint(aLine.getB2DPoint(1L).getX(), aLine.getB2DPoint(1L).getY() - nHDiff));
                aPolyPolygon.append(aLine);
            }

            pNewObj = new SdrPathObj(OBJ_PATHFILL, aPolyPolygon);
            break;
        }
        default: ; //prevent warning
    }
    if ( pNewObj )
    {
        // due to DLs changes in Outliner the object needs
        // a model to get an outliner for later calls to
        // pNewObj->SetOutlinerParaObject(pPara) (see below).
        pNewObj->SetModel(rOldObj.GetModel());

        Size aSize;
        Rectangle aSnap = pNewObj->GetSnapRect();

        aSize.Width() = aRect.Left() - aSnap.Left();
        aSize.Height() = aRect.Top() - aSnap.Top();
        pNewObj->NbcMove(aSize);

        rView.BegUndo( SVX_RESSTR( RID_SVXSTR_FONTWORK_UNDOCREATE ) );
        OutlinerParaObject* pPara = rOldObj.GetOutlinerParaObject();
        sal_Bool bHide = sal_True;

        if ( pPara != NULL )
        {
            pPara = new OutlinerParaObject(*pPara);
            pNewObj->SetOutlinerParaObject(pPara);
        }
        else
            bHide = sal_False;

        rView.ReplaceObjectAtView(&rOldObj, rPV, pNewObj, sal_True);
        pNewObj->SetLayer(rOldObj.GetLayer());
        aAttr.Put(XFormTextHideFormItem(bHide));
        aAttr.Put(XFormTextAdjustItem(eAdjust));

        XFormTextShadow eShadow = XFTSHADOW_NONE;

        if ( nLastShadowTbxId == TBI_SHADOW_NORMAL )
        {
            eShadow = XFTSHADOW_NORMAL;
            aAttr.Put(XFormTextShadowXValItem(nSaveShadowX));
            aAttr.Put(XFormTextShadowYValItem(nSaveShadowY));
        }
        else if ( nLastShadowTbxId == TBI_SHADOW_SLANT )
        {
            eShadow = XFTSHADOW_SLANT;
            aAttr.Put(XFormTextShadowXValItem(nSaveShadowAngle));
            aAttr.Put(XFormTextShadowYValItem(nSaveShadowSize));
        }

        aAttr.Put(XFormTextShadowItem(eShadow));

        rView.SetAttributes(aAttr);
        rView.EndUndo();
    }
}

void SvxFontWorkDialog::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) && (rDCEvt.GetFlags() & SETTINGS_STYLE) )
            ApplyImageList();

    SfxDockingWindow::DataChanged( rDCEvt );
}

void SvxFontWorkDialog::ApplyImageList()
{
    ResMgr* _pMgr = &DIALOG_MGR();

    sal_uInt16 nBitmapResId = RID_SVXBMP_FONTWORK_FORM1;
    sal_uInt16 nTextResId = RID_SVXSTR_FONTWORK_FORM1;

    bool bInit = aFormSet.GetItemCount() == 0;

    sal_uInt16 i;
    for( i = 1; i < 13; i++, nTextResId++, nBitmapResId++ )
    {
        if( bInit )
        {
            aFormSet.InsertItem( i, Bitmap(ResId(nBitmapResId,*_pMgr)),
                                    String(ResId(nTextResId,*_pMgr)));
        }
        else
        {
            aFormSet.SetItemImage( i, Bitmap(ResId(nBitmapResId,*_pMgr)) );
        }
    }

    ImageList& rImgLst = maImageList;

    aTbxStyle.SetImageList( rImgLst );
    aTbxAdjust.SetImageList( rImgLst );
    aTbxShadow.SetImageList( rImgLst );

    switch( nLastShadowTbxId )
    {
    case TBI_SHADOW_SLANT:
        aFbShadowX.SetImage( rImgLst.GetImage( TBI_SHADOW_ANGLE ) );
        aFbShadowY.SetImage( rImgLst.GetImage( TBI_SHADOW_SIZE ) );
        break;
    default:
        aFbShadowX.SetImage( rImgLst.GetImage( TBI_SHADOW_XDIST ) );
        aFbShadowY.SetImage( rImgLst.GetImage( TBI_SHADOW_YDIST ) );
        break;
    }

    aFbDistance.SetImage( rImgLst.GetImage( TBI_DISTANCE ) );
    aFbTextStart.SetImage( rImgLst.GetImage( TBI_TEXTSTART ) );

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
