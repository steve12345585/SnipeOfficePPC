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


#undef SC_DLLIMPLEMENTATION



#include "tpsubt.hxx"
#include "scresid.hxx"
#include "subtdlg.hxx"
#include "subtdlg.hrc"


//==================================================================

ScSubTotalDlg::ScSubTotalDlg( Window*           pParent,
                              const SfxItemSet* pArgSet ) :
        SfxTabDialog( pParent,
                      ScResId( RID_SCDLG_SUBTOTALS ),
                      pArgSet ),
        aBtnRemove  ( this, ScResId( BTN_REMOVE ) )
{
    AddTabPage( PAGE_GROUP1,  ScTpSubTotalGroup1::Create,  0 );
    AddTabPage( PAGE_GROUP2,  ScTpSubTotalGroup2::Create,  0 );
    AddTabPage( PAGE_GROUP3,  ScTpSubTotalGroup3::Create,  0 );
    AddTabPage( PAGE_OPTIONS, ScTpSubTotalOptions::Create, 0 );
    aBtnRemove.SetClickHdl( LINK( this, ScSubTotalDlg, RemoveHdl ) );
    FreeResource();
}

//------------------------------------------------------------------------

IMPL_LINK_INLINE_START( ScSubTotalDlg, RemoveHdl, PushButton *, pBtn )
{
    if ( pBtn == &aBtnRemove )
    {
        EndDialog( SCRET_REMOVE );
    }
    return 0;
}
IMPL_LINK_INLINE_END( ScSubTotalDlg, RemoveHdl, PushButton *, pBtn )

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
