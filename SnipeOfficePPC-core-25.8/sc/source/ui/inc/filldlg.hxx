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

#ifndef SC_FILLDLG_HXX
#define SC_FILLDLG_HXX

#include <vcl/dialog.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <vcl/edit.hxx>
#include "global.hxx"

class ScDocument;

//----------------------------------------------------------------------------

#include "scui_def.hxx"
//============================================================================

class ScFillSeriesDlg : public ModalDialog
{
public:
            ScFillSeriesDlg( Window*        pParent,
                             ScDocument&    rDocument,
                             FillDir        eFillDir,
                             FillCmd        eFillCmd,
                             FillDateCmd    eFillDateCmd,
                             const rtl::OUString& aStartStr,
                             double         fStep,
                             double         fMax,
                             sal_uInt16     nPossDir );
            ~ScFillSeriesDlg();

    FillDir     GetFillDir() const          { return theFillDir; }
    FillCmd     GetFillCmd() const          { return theFillCmd; }
    FillDateCmd GetFillDateCmd() const      { return theFillDateCmd; }
    double      GetStart() const            { return fStartVal; }
    double      GetStep() const             { return fIncrement; }
    double      GetMax() const              { return fEndVal; }

    rtl::OUString GetStartStr() const       { return aEdStartVal.GetText(); }

    void SetEdStartValEnabled(bool bFlag = false);

private:
    FixedText       aFtStartVal;
    Edit            aEdStartVal;
    const rtl::OUString aStartStrVal;

    FixedText       aFtEndVal;
    Edit            aEdEndVal;

    FixedText       aFtIncrement;
    Edit            aEdIncrement;
    FixedLine       aFlDirection;
    RadioButton     aBtnDown;
    RadioButton     aBtnRight;
    RadioButton     aBtnUp;
    RadioButton     aBtnLeft;

    FixedLine       aFlSep1;
    FixedLine       aFlType;
    RadioButton     aBtnArithmetic;
    RadioButton     aBtnGeometric;
    RadioButton     aBtnDate;
    RadioButton     aBtnAutoFill;

    FixedLine       aFlSep2;
    FixedLine       aFlTimeUnit;
    RadioButton     aBtnDay;
    RadioButton     aBtnDayOfWeek;
    RadioButton     aBtnMonth;
    RadioButton     aBtnYear;

    OKButton        aBtnOk;
    CancelButton    aBtnCancel;
    HelpButton      aBtnHelp;

    const rtl::OUString aErrMsgInvalidVal;

    //----------------------------------------------------------

    ScDocument& rDoc;
    FillDir     theFillDir;
    FillCmd     theFillCmd;
    FillDateCmd theFillDateCmd;
    double      fStartVal;
    double      fIncrement;
    double      fEndVal;

    bool        bStartValFlag;

#ifdef _FILLDLG_CXX
private:
    void Init( sal_uInt16 nPossDir );
    bool CheckStartVal();
    bool CheckIncrementVal();
    bool CheckEndVal();

    DECL_LINK( OKHdl, void * );
    DECL_LINK( DisableHdl, Button * );
#endif
};



#endif // SC_FILLDLG_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
