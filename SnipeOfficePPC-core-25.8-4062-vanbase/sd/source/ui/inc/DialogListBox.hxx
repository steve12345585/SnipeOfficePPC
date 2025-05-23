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
#ifndef SD_DIALOGLISTBOX_HXX
#define SD_DIALOGLISTBOX_HXX

#include <vcl/ctrl.hxx>
#include <vcl/scrbar.hxx>

namespace sd
{

class DialogListBox : public Control
{
private:
    ScrollBar*          mpHScrollBar;
    ScrollBar*          mpVScrollBar;
    ScrollBarBox*       mpScrollBarBox;
    ::Window*           mpChild;
    bool                mbVScroll;
    bool                mbHScroll;
    bool                mbAutoHScroll;
    Size                maMinSize, maInnerSize;

protected:
    virtual void        GetFocus();
    virtual void        StateChanged( StateChangedType nType );

    long                Notify( NotifyEvent& rNEvt );

    void                ImplResizeControls();
    void                ImplCheckScrollBars();
    void                ImplInitScrollBars();
    void                ImplResizeChild();

    DECL_LINK(ScrollBarHdl, void *);

public:
                    DialogListBox( ::Window* pParent, WinBits nWinStyle );
                    ~DialogListBox();

    void            SetChildWindow( ::Window* pChild, const Size& rMinSize );

    ::Window*           GetPreferredKeyInputWindow();
    void            Resize();

};

} //  namespace sd

// SD_DIALOGLISTBOX_HXX
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
