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
#ifndef _SVX_FONTWORK_HXX
#define _SVX_FONTWORK_HXX

#include <vcl/toolbox.hxx>
#include <vcl/fixed.hxx>
#include <svtools/valueset.hxx>
#include <sfx2/dockwin.hxx>
#include <sfx2/ctrlitem.hxx>
#include <svx/xenum.hxx>
#include <svx/dlgctrl.hxx>
#include "svx/svxdllapi.h"

// forward ---------------------------------------------------------------

class SdrView;
class SdrPageView;
class SdrObject;

class XFormTextAdjustItem;
class XFormTextDistanceItem;
class XFormTextStartItem;
class XFormTextMirrorItem;
class XFormTextStdFormItem;
class XFormTextHideFormItem;
class XFormTextOutlineItem;
class XFormTextShadowItem;
class XFormTextShadowColorItem;
class XFormTextShadowXValItem;
class XFormTextShadowYValItem;

/*************************************************************************
|*
|* ControllerItem for Fontwork
|*
\************************************************************************/

class SvxFontWorkDialog;

class SvxFontWorkControllerItem : public SfxControllerItem
{
    SvxFontWorkDialog  &rFontWorkDlg;

protected:
    virtual void StateChanged(sal_uInt16 nSID, SfxItemState eState,
                              const SfxPoolItem* pState);

public:
    SvxFontWorkControllerItem(sal_uInt16 nId, SvxFontWorkDialog&, SfxBindings&);
};

/*************************************************************************
|*
|* Derived from SfxChildWindow as "container" for fontwork dialog
|*
\************************************************************************/

class SVX_DLLPUBLIC SvxFontWorkChildWindow : public SfxChildWindow
{
 public:
    SvxFontWorkChildWindow(Window*, sal_uInt16, SfxBindings*, SfxChildWinInfo*);
    SFX_DECL_CHILDWINDOW_WITHID(SvxFontWorkChildWindow);
};

/*************************************************************************
|*
|* Floating window for setting attributes of text effects
|*
\************************************************************************/

class SVX_DLLPUBLIC SvxFontWorkDialog : public SfxDockingWindow
{
 #define CONTROLLER_COUNT 12

    SvxFontWorkControllerItem* pCtrlItems[CONTROLLER_COUNT];

    ValueSet        aFormSet;

    ToolBox         aTbxStyle;
    ToolBox         aTbxAdjust;

    FixedImage      aFbDistance;
    MetricField     aMtrFldDistance;
    FixedImage      aFbTextStart;
    MetricField     aMtrFldTextStart;

    ToolBox         aTbxShadow;

    FixedImage      aFbShadowX;
    MetricField     aMtrFldShadowX;
    FixedImage      aFbShadowY;
    MetricField     aMtrFldShadowY;

    ColorLB         aShadowColorLB;

    SfxBindings&    rBindings;
    Timer           aInputTimer;

    sal_uInt16          nLastStyleTbxId;
    sal_uInt16          nLastAdjustTbxId;
    sal_uInt16          nLastShadowTbxId;
    long            nSaveShadowX;
    long            nSaveShadowY;
    long            nSaveShadowAngle;
    long            nSaveShadowSize;

    ImageList       maImageList;

    XColorListRef   pColorList;

#ifdef _SVX_FONTWORK_CXX
 friend class SvxFontWorkChildWindow;
 friend class SvxFontWorkControllerItem;

    DECL_LINK( SelectStyleHdl_Impl, void * );
    DECL_LINK( SelectAdjustHdl_Impl, void * );
    DECL_LINK( SelectShadowHdl_Impl, void * );

    DECL_LINK( ModifyInputHdl_Impl, void * );
    DECL_LINK( InputTimoutHdl_Impl, void * );

    DECL_LINK( FormSelectHdl_Impl, void * );
    DECL_LINK( ColorSelectHdl_Impl, void * );

    void SetStyle_Impl(const XFormTextStyleItem*);
    void SetAdjust_Impl(const XFormTextAdjustItem*);
    void SetDistance_Impl(const XFormTextDistanceItem*);
    void SetStart_Impl(const XFormTextStartItem*);
    void SetMirror_Impl(const XFormTextMirrorItem*);
    void SetStdForm_Impl(const XFormTextStdFormItem*);
    void SetShowForm_Impl(const XFormTextHideFormItem*);
    void SetOutline_Impl(const XFormTextOutlineItem*);
    void SetShadow_Impl(const XFormTextShadowItem*,
                        sal_Bool bRestoreValues = sal_False);
    void SetShadowColor_Impl(const XFormTextShadowColorItem*);
    void SetShadowXVal_Impl(const XFormTextShadowXValItem*);
    void SetShadowYVal_Impl(const XFormTextShadowYValItem*);
#endif

    virtual void DataChanged( const DataChangedEvent& rDCEvt );
    void ApplyImageList();

 protected:
    virtual void    Zoom();
    virtual SfxChildAlignment CheckAlignment( SfxChildAlignment eActAlign,
                                              SfxChildAlignment eAlign );

 public:
    SvxFontWorkDialog(  SfxBindings *pBindinx,
                        SfxChildWindow *pCW,
                        Window* pParent,
                        const ResId& rResId );
    ~SvxFontWorkDialog();

    void SetColorList(const XColorListRef &pTable);
    void SetActive(sal_Bool bActivate = sal_True);

    void CreateStdFormObj(SdrView& rView, SdrPageView& rPV,
                          const SfxItemSet& rAttr, SdrObject& rOldObj,
                          XFormTextStdForm eForm);
};

#endif      // _SVX_FONTWORK_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
