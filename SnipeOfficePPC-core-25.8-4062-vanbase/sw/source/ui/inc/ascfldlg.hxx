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
#ifndef _ASCFLDLG_HXX
#define _ASCFLDLG_HXX
#include <vcl/fixed.hxx>

#include <vcl/button.hxx>
#include <vcl/lstbox.hxx>
#include <sfx2/basedlgs.hxx>
#include <svx/txencbox.hxx>
#include <svx/langbox.hxx>

class SwAsciiOptions;
class SvStream;
class SwDocShell;

class SwAsciiFilterDlg : public SfxModalDialog
{
    FixedLine           aFL;
    FixedText           aCharSetFT;
    SvxTextEncodingBox  aCharSetLB;
    FixedText           aFontFT;
    ListBox             aFontLB;
    FixedText           aLanguageFT;
    SvxLanguageBox      aLanguageLB;
    FixedText           aCRLF_FT;
    RadioButton         aCRLF_RB, aCR_RB, aLF_RB;
    OKButton            aOkPB;
    CancelButton        aCancelPB;
    HelpButton          aHelpPB;
    String              sSystemCharSet;
    sal_Bool                bSaveLineStatus;

    DECL_LINK( CharSetSelHdl, SvxTextEncodingBox* );
    DECL_LINK( LineEndHdl, RadioButton* );
    void SetCRLF( LineEnd eEnd );
    LineEnd GetCRLF() const;

public:
    // CTOR:    for import - pStream is the inputstream
    //          for export - pStream must be 0
    SwAsciiFilterDlg( Window* pParent, SwDocShell& rDocSh,
                        SvStream* pStream );
    virtual ~SwAsciiFilterDlg();

    void FillOptions( SwAsciiOptions& rOptions );
};



#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
