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

// ============================================================================

#ifndef SCUI_ASCIIOPT_HXX
#define SCUI_ASCIIOPT_HXX


#include "asciiopt.hxx"
#include "svx/langbox.hxx"

// ============================================================================

class ScImportAsciiDlg : public ModalDialog
{
    SvStream*                   mpDatStream;
    sal_uLong                       mnStreamPos;
    sal_uLong*                      mpRowPosArray;
    sal_uLong                       mnRowPosCount;

    rtl::OUString               maPreviewLine[ CSV_PREVIEW_LINES ];

    FixedLine                   aFlFieldOpt;
    FixedText                   aFtCharSet;
    SvxTextEncodingBox          aLbCharSet;
    FixedText                   aFtCustomLang;
    SvxLanguageBox              aLbCustomLang;

    FixedText                   aFtRow;
    NumericField                aNfRow;

    FixedLine                   aFlSepOpt;
    RadioButton                 aRbFixed;
    RadioButton                 aRbSeparated;

    CheckBox                    aCkbTab;
    CheckBox                    aCkbSemicolon;
    CheckBox                    aCkbComma;
    CheckBox                    aCkbSpace;
    CheckBox                    aCkbOther;
    Edit                        aEdOther;
    CheckBox                    aCkbAsOnce;

    FixedLine                   aFlOtherOpt;

    FixedText                   aFtTextSep;
    ComboBox                    aCbTextSep;

    CheckBox                    aCkbQuotedAsText;
    CheckBox                    aCkbDetectNumber;

    FixedLine                   aFlWidth;
    FixedText                   aFtType;
    ListBox                     aLbType;

    ScCsvTableBox               maTableBox;

    OKButton                    aBtnOk;
    CancelButton                aBtnCancel;
    HelpButton                  aBtnHelp;

    String                      aCharSetUser;
    String                      aColumnUser;
    String                      aTextSepList;
    String                      maFieldSeparators;  // selected field separators
    sal_Unicode                 mcTextSep;
    String                      maStrTextToColumns;

    CharSet                     meCharSet;          /// Selected char set.
    bool                        mbCharSetSystem;    /// Is System char set selected?
    ScImportAsciiCall           meCall;             /// How the dialog is called (see asciiopt.hxx)

public:
                                ScImportAsciiDlg(
                                    Window* pParent, String aDatName,
                                    SvStream* pInStream, ScImportAsciiCall eCall );
                                ~ScImportAsciiDlg();

    void                        GetOptions( ScAsciiOptions& rOpt );
    void                        SaveParameters();

private:
    /** Sets the selected char set data to meCharSet and mbCharSetSystem. */
    void                        SetSelectedCharSet();
    /** Set separators in ui from maFieldSeparators    */
    void                        SetSeparators();
    /** Returns all separator characters in a string. */
    String                      GetSeparators() const;

    /** Enables or disables all separator checkboxes and edit fields. */
    void                        SetupSeparatorCtrls();


    bool                        GetLine( sal_uLong nLine, rtl::OUString &rText );
    void                        UpdateVertical();
    inline bool                 Seek( sal_uLong nPos ); // synced to and from mnStreamPos

                                DECL_LINK( CharSetHdl, SvxTextEncodingBox* );
                                DECL_LINK( FirstRowHdl, NumericField* );
                                DECL_LINK( RbSepFixHdl, RadioButton* );
                                DECL_LINK( SeparatorHdl, Control* );
                                DECL_LINK( LbColTypeHdl, ListBox* );
                                DECL_LINK(UpdateTextHdl, void *);
                                DECL_LINK( ColTypeHdl, ScCsvTableBox* );
};


inline bool ScImportAsciiDlg::Seek(sal_uLong nPos)
{
    bool bSuccess = true;
    if (nPos != mnStreamPos && mpDatStream)
    {
        if (mpDatStream->Seek( nPos ) != nPos)
            bSuccess = false;
        else
            mnStreamPos = nPos;
    }
    return bSuccess;
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
