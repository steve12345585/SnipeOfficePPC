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
#ifndef _SFXNFLTDLG_HXX
#define _SFXNFLTDLG_HXX


#include <vcl/dialog.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
class SfxFilterMatcher;

class SfxMedium;
class SfxFilterDialog : public ModalDialog
{
private:
    FixedLine               aMimeTypeFL;
    FixedText               aMIMEType;
    FixedText               aExtension;
    FixedLine               aFilterFL;
    ListBox                 aListbox;
    OKButton                aOkBtn;
    CancelButton            aCancelBtn;
    HelpButton              aHelpBtn;

    const SfxFilterMatcher& rMatcher;

    DECL_LINK( OkHdl, Control * );

public:
    SfxFilterDialog( Window * pParent,
                     SfxMedium* pMed,
                     const SfxFilterMatcher &rMatcher,
                     const String *pDefFilter, sal_Bool bShowConvert = sal_True );

    String GetSelectEntry() const;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
