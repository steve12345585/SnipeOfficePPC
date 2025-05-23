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


#ifndef _SD_DLGSNAP_HXX
#define _SD_DLGSNAP_HXX


#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <vcl/field.hxx>
#include <vcl/group.hxx>
#include <tools/fract.hxx>
#include <vcl/dialog.hxx>
#include "sdenumdef.hxx"
/************************************************************************/

class SfxItemSet;
namespace sd {
    class View;
}

/*************************************************************************
|*
|* Dialog zum Einstellen von Fanglinien und -punkten
|*
\************************************************************************/

class SdSnapLineDlg : public ModalDialog
{
private:
    FixedLine           aFlPos;
    FixedText           aFtX;
    MetricField         aMtrFldX;
    FixedText           aFtY;
    MetricField         aMtrFldY;
    FixedLine           aFlDir;
    ImageRadioButton    aRbPoint;
    ImageRadioButton    aRbVert;
    ImageRadioButton    aRbHorz;
    OKButton            aBtnOK;
    CancelButton        aBtnCancel;
    HelpButton          aBtnHelp;
    PushButton          aBtnDelete;
    long                nXValue;
    long                nYValue;
    FieldUnit           eUIUnit;
    Fraction            aUIScale;

    DECL_LINK( ClickHdl, Button * );

public:
    SdSnapLineDlg(Window* pWindow, const SfxItemSet& rInAttrs, ::sd::View* pView);

    void GetAttr(SfxItemSet& rOutAttrs);

    void HideRadioGroup();
    void HideDeleteBtn() { aBtnDelete.Hide(); }
    void SetInputFields(sal_Bool bEnableX, sal_Bool bEnableY);
};



#endif      // _SD_DLGSNAP_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
