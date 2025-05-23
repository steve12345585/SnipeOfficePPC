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

#ifndef SD_FU_PAGE_HXX
#define SD_FU_PAGE_HXX

#include "fupoor.hxx"

class SfxItemSet;
class SdBackgroundObjUndoAction;
class SdPage;

namespace sd {
class DrawViewShell;

class FuPage
    : public FuPoor
{
 public:
    TYPEINFO();

    static FunctionReference Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq );
    virtual void DoExecute( SfxRequest& rReq );

    virtual void Activate();           // Function aktivieren
    virtual void Deactivate();         // Function deaktivieren

    const SfxItemSet* ExecuteDialog( Window* pParent );

protected:
    virtual ~FuPage (void);

private:
    FuPage (
        ViewShell* pViewSh,
        ::sd::Window* pWin,
        ::sd::View* pView,
        SdDrawDocument* pDoc,
        SfxRequest& rReq );

    void ApplyItemSet( const SfxItemSet* pArgs );

    SfxRequest&                 mrReq;
    const SfxItemSet*           mpArgs;
    SdBackgroundObjUndoAction*  mpBackgroundObjUndoAction;
    Size                        maSize;
    bool                        mbPageBckgrdDeleted;
    bool                        mbMasterPage;
    bool                        mbDisplayBackgroundTabPage;
    SdPage*                     mpPage;
    DrawViewShell*              mpDrawViewShell;
};

} // end of namespace sd

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
