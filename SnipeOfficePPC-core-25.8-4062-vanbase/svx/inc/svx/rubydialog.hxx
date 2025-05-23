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


#ifndef _SVX_RUBYDLG_HXX_
#define _SVX_RUBYDLG_HXX_

#include <sfx2/childwin.hxx>
#include <sfx2/basedlgs.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/fixed.hxx>
#include <vcl/button.hxx>
#include <vcl/edit.hxx>
#include <vcl/scrbar.hxx>
#include <com/sun/star/uno/Reference.h>
#include "svx/svxdllapi.h"

namespace com{namespace sun{namespace star{
    namespace view{
        class XSelectionChangeListener;
    }
}}}


class SvxRubyDialog;
class RubyPreview : public Window
{
protected:
    virtual void Paint( const Rectangle& rRect );
    SvxRubyDialog&  rParentDlg;

public:
    RubyPreview(SvxRubyDialog& rParent, const ResId& rResId);
};

class SVX_DLLPUBLIC SvxRubyChildWindow : public SfxChildWindow
{
 public:

    SvxRubyChildWindow( Window*, sal_uInt16, SfxBindings*, SfxChildWinInfo* );

    SFX_DECL_CHILDWINDOW( SvxRubyChildWindow );

};
class SvxRubyData_Impl;
class RubyEdit  : public Edit
{
    Link    aScrollHdl;
    Link    aJumpHdl;
    virtual void        GetFocus();
    virtual long        PreNotify( NotifyEvent& rNEvt );
public:
    RubyEdit(Window* pParent, const ResId& rResId) :
        Edit(pParent, rResId){}
    void    SetScrollHdl(Link& rLink) {aScrollHdl = rLink;}
    void    SetJumpHdl(Link& rLink) {aJumpHdl = rLink;}
};


class SvxRubyDialog : public SfxModelessDialog
{
    using Window::SetText;
    using Window::GetText;

    friend class RubyPreview;

    FixedText               aLeftFT;
    RubyEdit                aLeft1ED;
    FixedText               aRightFT;
    RubyEdit                aRight1ED;
    RubyEdit                aLeft2ED;
    RubyEdit                aRight2ED;
    RubyEdit                aLeft3ED;
    RubyEdit                aRight3ED;
    RubyEdit                aLeft4ED;
    RubyEdit                aRight4ED;

    RubyEdit*               aEditArr[8];
    ScrollBar           aScrollSB;

    CheckBox            aAutoDetectionCB;

    FixedText           aAdjustFT;
    ListBox             aAdjustLB;

    FixedText           aPositionFT;
    ListBox             aPositionLB;

    FixedText           aCharStyleFT;
    ListBox             aCharStyleLB;
    PushButton          aStylistPB;

    FixedText           aPreviewFT;
    RubyPreview         aPreviewWin;

    OKButton            aApplyPB;
    PushButton          aClosePB;
    HelpButton          aHelpPB;

    long                nLastPos;
    long                nCurrentEdit;

    sal_Bool                bModified;

    com::sun::star::uno::Reference<com::sun::star::view::XSelectionChangeListener> xImpl;
    SfxBindings*    pBindings;
    SvxRubyData_Impl* pImpl;

    DECL_LINK(ApplyHdl_Impl, void *);
    DECL_LINK(CloseHdl_Impl, void *);
    DECL_LINK(StylistHdl_Impl, void *);
    DECL_LINK(AutomaticHdl_Impl, CheckBox*);
    DECL_LINK(ScrollHdl_Impl, ScrollBar*);
    DECL_LINK(PositionHdl_Impl, ListBox*);
    DECL_LINK(AdjustHdl_Impl, ListBox*);
    DECL_LINK(CharStyleHdl_Impl, void *);
    DECL_LINK(EditModifyHdl_Impl, Edit*);
    DECL_LINK(EditScrollHdl_Impl, sal_Int32*);
    DECL_LINK(EditJumpHdl_Impl, sal_Int32*);

    void                SetText(sal_Int32 nPos, Edit& rLeft, Edit& rRight);
    void                GetText();
    void                ClearCharStyleList();
    void                AssertOneEntry();

    void                Update();
    virtual sal_Bool        Close();

    long                GetLastPos() const {return nLastPos;}
    void                SetLastPos(long nSet) {nLastPos = nSet;}

    sal_Bool                IsModified() const {return bModified;}
    void                SetModified(sal_Bool bSet) {bModified = bSet;}

    void EnableControls(sal_Bool bEnable)
        {
            aLeftFT.Enable(bEnable);
            aRightFT.Enable(bEnable);
            aLeft1ED.Enable(bEnable);
            aRight1ED.Enable(bEnable);
            aLeft2ED.Enable(bEnable);
            aRight2ED.Enable(bEnable);
            aLeft3ED.Enable(bEnable);
            aRight3ED.Enable(bEnable);
            aLeft4ED.Enable(bEnable);
            aRight4ED.Enable(bEnable);
            aScrollSB.Enable(bEnable);
            aAutoDetectionCB.Enable(bEnable);
            aAdjustFT.Enable(bEnable);
            aAdjustLB.Enable(bEnable);
            aCharStyleFT.Enable(bEnable);
            aCharStyleLB.Enable(bEnable);
            aStylistPB.Enable(bEnable);
            aPreviewFT.Enable(bEnable);
            aPreviewWin.Enable(bEnable);
            aApplyPB.Enable(bEnable);
        }

    void                GetCurrentText(String& rBase, String& rRuby);

    void                UpdateColors( void );
protected:
    virtual void        DataChanged( const DataChangedEvent& rDCEvt );
public:

                        SvxRubyDialog( SfxBindings *pBindings, SfxChildWindow *pCW,
                                    Window* pParent, const ResId& rResId );
    virtual             ~SvxRubyDialog();

    virtual void        Activate();
    virtual void        Deactivate();
};

#endif // _SVX_RUBYDLG_HXX_


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
