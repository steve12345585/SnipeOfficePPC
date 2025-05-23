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

#include <swtypes.hxx>
#include <mergetbl.hxx>
#include <mergetbl.hrc>
#include <table.hrc>


SwMergeTblDlg::SwMergeTblDlg( Window *pParent, sal_Bool& rWithPrev )
    : SvxStandardDialog(pParent, SW_RES(DLG_MERGE_TABLE)),
    aOKPB(                  this, SW_RES(PB_OK      )),
    aCancelPB(              this, SW_RES(PB_CANCEL  )),
    aHelpPB(                this, SW_RES(PB_HELP        )),
    aMergeFL(               this, SW_RES(FL_MERGE    )),
    aMergePrevRB(           this, SW_RES(RB_MERGE_PREV)),
    aMergeNextRB(           this, SW_RES(RB_MERGE_NEXT)),
    rMergePrev(             rWithPrev )
{
    FreeResource();
    aMergePrevRB.Check();
}

void SwMergeTblDlg::Apply()
{
    rMergePrev = aMergePrevRB.IsChecked();
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
