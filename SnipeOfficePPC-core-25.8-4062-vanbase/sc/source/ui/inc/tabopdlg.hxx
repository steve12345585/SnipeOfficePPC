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

#ifndef SC_TABOPDLG_HXX
#define SC_TABOPDLG_HXX

#include <vcl/fixed.hxx>
#include <vcl/group.hxx>
#include "global.hxx"
#include "address.hxx"
#include "anyrefdg.hxx"


//------------------------------------------------------------------------

enum ScTabOpErr
{
    TABOPERR_NOFORMULA = 1,
    TABOPERR_NOCOLROW,
    TABOPERR_WRONGFORMULA,
    TABOPERR_WRONGROW,
    TABOPERR_NOCOLFORMULA,
    TABOPERR_WRONGCOL,
    TABOPERR_NOROWFORMULA
};

//========================================================================

class ScTabOpDlg : public ScAnyRefDlg
{
public:
                    ScTabOpDlg( SfxBindings* pB, SfxChildWindow* pCW, Window* pParent,
                                ScDocument*     pDocument,
                                const ScRefAddress& rCursorPos );
                    ~ScTabOpDlg();

    virtual void    SetReference( const ScRange& rRef, ScDocument* pDoc );
    virtual sal_Bool    IsRefInputMode() const { return sal_True; }
    virtual void    SetActive();

    virtual sal_Bool    Close();

private:
    FixedLine       aFlVariables;
    FixedText       aFtFormulaRange;
    formula::RefEdit        aEdFormulaRange;
    formula::RefButton      aRBFormulaRange;

    FixedText       aFtRowCell;
    formula::RefEdit        aEdRowCell;
    formula::RefButton      aRBRowCell;

    FixedText       aFtColCell;
    formula::RefEdit        aEdColCell;
    formula::RefButton      aRBColCell;

    OKButton        aBtnOk;
    CancelButton    aBtnCancel;
    HelpButton      aBtnHelp;

    ScRefAddress    theFormulaCell;
    ScRefAddress    theFormulaEnd;
    ScRefAddress    theRowCell;
    ScRefAddress    theColCell;

    ScDocument*     pDoc;
    const SCTAB     nCurTab;
    formula::RefEdit*       pEdActive;
    sal_Bool            bDlgLostFocus;
    const String    errMsgNoFormula;
    const String    errMsgNoColRow;
    const String    errMsgWrongFormula;
    const String    errMsgWrongRowCol;
    const String    errMsgNoColFormula;
    const String    errMsgNoRowFormula;

#ifdef _TABOPDLG_CXX
    void    Init();
    void    RaiseError( ScTabOpErr eError );

    DECL_LINK( BtnHdl, PushButton* );
    DECL_LINK( GetFocusHdl, Control* );
    DECL_LINK( LoseFocusHdl, void* );
#endif  // _TABOPDLG_CXX
};

#endif // SC_TABOPDLG_HXX



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
