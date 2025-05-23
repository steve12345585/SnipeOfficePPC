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
#ifndef _SWBASESH_HXX
#define _SWBASESH_HXX

#include <shellid.hxx>

#include <tools/link.hxx>
#include <vcl/timer.hxx>
#include <sfx2/module.hxx>
#include <sfx2/shell.hxx>
#include <viewopt.hxx>

#include <mdiexp.hxx>
#include <set>

class SwWrtShell;
class SwView;
class SfxItemSet;

struct DBTextStruct_Impl;
class SwBaseShell: public SfxShell
{
    SwView      &rView;

    // DragMode
    static FlyMode eFrameMode;

    // Bug 75078 - if in GetState the asynch call of GetGraphic returns
    //              synch, the set the state directly into the itemset
    SfxItemSet*         pGetStateSet;

    // Update-Timer for graphic
    std::set<sal_uInt16> aGrfUpdateSlots;

    DECL_LINK( GraphicArrivedHdl, void* );

protected:
    SwWrtShell&         GetShell();
    SwWrtShell*         GetShellPtr();

    inline SwView&      GetView()                       { return rView; }
    inline void         SetGetStateSet( SfxItemSet* p ) { pGetStateSet = p; }
    inline sal_Bool     AddGrfUpdateSlot( sal_uInt16 nSlot ){ return aGrfUpdateSlots.insert( nSlot ).second; }

    DECL_STATIC_LINK(   SwBaseShell, InsertDBTextHdl, DBTextStruct_Impl* );

    void                InsertURLButton( const String& rURL, const String& rTarget, const String& rTxt );
    void                InsertTable( SfxRequest& _rRequest );

public:
    SwBaseShell(SwView &rShell);
    virtual     ~SwBaseShell();

    SFX_DECL_INTERFACE(SW_BASESHELL)
    TYPEINFO();

    void        ExecDelete(SfxRequest &);

    void        ExecClpbrd(SfxRequest &);
    void        StateClpbrd(SfxItemSet &);

    void        ExecUndo(SfxRequest &);
    void        StateUndo(SfxItemSet &);

    void        Execute(SfxRequest &);
    void        GetState(SfxItemSet &);
    void        StateStyle(SfxItemSet &);

    void        ExecuteGallery(SfxRequest&);
    void        GetGalleryState(SfxItemSet&);

    void        ExecDlg(SfxRequest &);

    void        StateStatusLine(SfxItemSet &rSet);
    void        ExecTxtCtrl(SfxRequest& rReq);
    void        GetTxtFontCtrlState(SfxItemSet& rSet);
    void        GetTxtCtrlState(SfxItemSet& rSet);
    void        GetBorderState(SfxItemSet &rSet);
    void        GetBckColState(SfxItemSet &rSet);

    void        ExecBckCol(SfxRequest& rReq);
    void        SetWrapMode( sal_uInt16 nSlot );

    void        StateDisableItems(SfxItemSet &);

    void        EditRegionDialog(SfxRequest& rReq);
    void        InsertRegionDialog(SfxRequest& rReq);

    void        ExecField(SfxRequest& rReq);

    static void    SetFrmMode( FlyMode eMode, SwWrtShell *pShell );  // with update!
    static void   _SetFrmMode( FlyMode eMode )   { eFrameMode = eMode; }
    static FlyMode  GetFrmMode()                 { return eFrameMode;  }
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
