/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef SW_WORDCOUNTDIALOG_HXX
#define SW_WORDCOUNTDIALOG_HXX
#include <sfx2/basedlgs.hxx>
#include <svtools/stdctrl.hxx>
#include <vcl/button.hxx>
struct SwDocStat;
#include <sfx2/childwin.hxx>
#include "swabstdlg.hxx"

class SwWrtShell;

class SwWordCountDialog
{
    FixedLine       aCurrentFL;
    FixedText       aCurrentWordFT;
    FixedInfo       aCurrentWordFI;
    FixedText       aCurrentCharacterFT;
    FixedInfo       aCurrentCharacterFI;
    FixedText       aCurrentCharacterExcludingSpacesFT;
    FixedInfo       aCurrentCharacterExcludingSpacesFI;

    FixedLine       aDocFL;
    FixedText       aDocWordFT;
    FixedInfo       aDocWordFI;
    FixedText       aDocCharacterFT;
    FixedInfo       aDocCharacterFI;
    FixedText       aDocCharacterExcludingSpacesFT;
    FixedInfo       aDocCharacterExcludingSpacesFI;

    FixedLine       aBottomFL;

    OKButton        aOK;
    HelpButton      aHelp;

    void InitControls();

public:
    SwWordCountDialog(Window* pParent);
    ~SwWordCountDialog();

    void    SetValues(const SwDocStat& rCurrent, const SwDocStat& rDoc);

    SW_DLLPRIVATE DECL_LINK( OkHdl,     void* );
};

class SwWordCountFloatDlg : public SfxModelessDialog
{
    SwWordCountDialog   aDlg;
    virtual void    Activate();
    public:
    SwWordCountFloatDlg(     SfxBindings* pBindings,
                             SfxChildWindow* pChild,
                             Window *pParent,
                             SfxChildWinInfo* pInfo);
    void    UpdateCounts();
};

class SwWordCountWrapper : public SfxChildWindow
{
    AbstractSwWordCountFloatDlg*   pAbstDlg;
protected:
    SwWordCountWrapper(    Window *pParentWindow,
                            sal_uInt16 nId,
                            SfxBindings* pBindings,
                            SfxChildWinInfo* pInfo );

    SFX_DECL_CHILDWINDOW(SwWordCountWrapper);

public:
    void    UpdateCounts();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
