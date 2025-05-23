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


#undef SC_DLLIMPLEMENTATION



//------------------------------------------------------------------

#include "crdlg.hxx"
#include "scresid.hxx"
#include "miscdlgs.hrc"


//==================================================================

ScColOrRowDlg::ScColOrRowDlg( Window*       pParent,
                              const String& rStrTitle,
                              const String& rStrLabel,
                              sal_Bool          bColDefault )

    :   ModalDialog     ( pParent, ScResId( RID_SCDLG_COLORROW ) ),
        //
        aFlFrame        ( this, ScResId( FL_FRAME ) ),
        aBtnRows        ( this, ScResId( BTN_GROUP_ROWS ) ),
        aBtnCols        ( this, ScResId( BTN_GROUP_COLS ) ),
        aBtnOk          ( this, ScResId( BTN_OK ) ),
        aBtnCancel      ( this, ScResId( BTN_CANCEL ) ),
        aBtnHelp        ( this, ScResId( BTN_HELP ) )
{
    SetText( rStrTitle );
    aFlFrame.SetText( rStrLabel );

    if ( bColDefault )
        aBtnCols.Check();
    else
        aBtnRows.Check();

    aBtnOk.SetClickHdl( LINK( this, ScColOrRowDlg, OkHdl ) );

    FreeResource();
}

//------------------------------------------------------------------------

ScColOrRowDlg::~ScColOrRowDlg()
{
}

//------------------------------------------------------------------------

IMPL_LINK_NOARG_INLINE_START(ScColOrRowDlg, OkHdl)
{
    EndDialog( aBtnCols.IsChecked() ? SCRET_COLS : SCRET_ROWS );
    return 0;
}
IMPL_LINK_NOARG_INLINE_END(ScColOrRowDlg, OkHdl)



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
