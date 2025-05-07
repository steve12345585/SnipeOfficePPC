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



//------------------------------------------------------------------

#include "delcodlg.hxx"
#include "scresid.hxx"
#include "miscdlgs.hrc"


//==================================================================

sal_Bool   ScDeleteContentsDlg::bPreviousAllCheck = false;
sal_uInt16 ScDeleteContentsDlg::nPreviousChecks   = (IDF_DATETIME | IDF_STRING  |
                                                 IDF_NOTE     | IDF_FORMULA |
                                                 IDF_VALUE);

//-----------------------------------------------------------------------

ScDeleteContentsDlg::ScDeleteContentsDlg( Window* pParent,
                                          sal_uInt16  nCheckDefaults ) :
    ModalDialog     ( pParent, ScResId( RID_SCDLG_DELCONT ) ),
    //
    aFlFrame        ( this, ScResId( FL_FRAME ) ),
    aBtnDelAll      ( this, ScResId( BTN_DELALL ) ),
    aBtnDelStrings  ( this, ScResId( BTN_DELSTRINGS ) ),
    aBtnDelNumbers  ( this, ScResId( BTN_DELNUMBERS ) ),
    aBtnDelDateTime ( this, ScResId( BTN_DELDATETIME ) ),
    aBtnDelFormulas ( this, ScResId( BTN_DELFORMULAS ) ),
    aBtnDelNotes    ( this, ScResId( BTN_DELNOTES ) ),
    aBtnDelAttrs    ( this, ScResId( BTN_DELATTRS ) ),
    aBtnDelObjects  ( this, ScResId( BTN_DELOBJECTS ) ),
    aBtnOk          ( this, ScResId( BTN_OK ) ),
    aBtnCancel      ( this, ScResId( BTN_CANCEL ) ),
    aBtnHelp        ( this, ScResId( BTN_HELP ) ),
    bObjectsDisabled( false )
{
    if ( nCheckDefaults != 0 )
    {
        ScDeleteContentsDlg::nPreviousChecks = nCheckDefaults;
        ScDeleteContentsDlg::bPreviousAllCheck = false;
    }

    aBtnDelAll.Check     ( ScDeleteContentsDlg::bPreviousAllCheck );
    aBtnDelStrings.Check ( IS_SET( IDF_STRING,
                                   ScDeleteContentsDlg::nPreviousChecks ) );
    aBtnDelNumbers.Check ( IS_SET( IDF_VALUE,
                                   ScDeleteContentsDlg::nPreviousChecks ) );
    aBtnDelDateTime.Check( IS_SET( IDF_DATETIME,
                                   ScDeleteContentsDlg::nPreviousChecks ) );
    aBtnDelFormulas.Check( IS_SET( IDF_FORMULA,
                                   ScDeleteContentsDlg::nPreviousChecks ) );
    aBtnDelNotes.Check   ( IS_SET( IDF_NOTE,
                                   ScDeleteContentsDlg::nPreviousChecks ) );
    aBtnDelAttrs.Check   ( IS_SET( IDF_ATTRIB,
                                   ScDeleteContentsDlg::nPreviousChecks ) );
    aBtnDelObjects.Check ( IS_SET( IDF_OBJECTS,
                                   ScDeleteContentsDlg::nPreviousChecks ) );

    DisableChecks( aBtnDelAll.IsChecked() );

    aBtnDelAll.SetClickHdl( LINK( this, ScDeleteContentsDlg, DelAllHdl ) );

    //-------------
    FreeResource();
}

//------------------------------------------------------------------------

sal_uInt16 ScDeleteContentsDlg::GetDelContentsCmdBits() const
{
    ScDeleteContentsDlg::nPreviousChecks = 0;

    if ( aBtnDelStrings.IsChecked() )
        ScDeleteContentsDlg::nPreviousChecks  = IDF_STRING;
    if ( aBtnDelNumbers.IsChecked() )
        ScDeleteContentsDlg::nPreviousChecks |= IDF_VALUE;
    if ( aBtnDelDateTime.IsChecked())
        ScDeleteContentsDlg::nPreviousChecks |= IDF_DATETIME;
    if ( aBtnDelFormulas.IsChecked())
        ScDeleteContentsDlg::nPreviousChecks |= IDF_FORMULA;
    if ( aBtnDelNotes.IsChecked()   )
        ScDeleteContentsDlg::nPreviousChecks |= IDF_NOTE;
    if ( aBtnDelAttrs.IsChecked()   )
        ScDeleteContentsDlg::nPreviousChecks |= IDF_ATTRIB;
    if ( aBtnDelObjects.IsChecked() )
        ScDeleteContentsDlg::nPreviousChecks |= IDF_OBJECTS;

    ScDeleteContentsDlg::bPreviousAllCheck = aBtnDelAll.IsChecked();

    return ( (ScDeleteContentsDlg::bPreviousAllCheck)
                ? IDF_ALL
                : ScDeleteContentsDlg::nPreviousChecks );
}

//------------------------------------------------------------------------

void ScDeleteContentsDlg::DisableChecks( sal_Bool bDelAllChecked )
{
    if ( bDelAllChecked )
    {
        aBtnDelStrings.Disable();
        aBtnDelNumbers.Disable();
        aBtnDelDateTime.Disable();
        aBtnDelFormulas.Disable();
        aBtnDelNotes.Disable();
        aBtnDelAttrs.Disable();
        aBtnDelObjects.Disable();
    }
    else
    {
        aBtnDelStrings.Enable();
        aBtnDelNumbers.Enable();
        aBtnDelDateTime.Enable();
        aBtnDelFormulas.Enable();
        aBtnDelNotes.Enable();
        aBtnDelAttrs.Enable();
        if (bObjectsDisabled)
            aBtnDelObjects.Disable();
        else
            aBtnDelObjects.Enable();
    }
}

//------------------------------------------------------------------------

void ScDeleteContentsDlg::DisableObjects()
{
    bObjectsDisabled = sal_True;
    aBtnDelObjects.Check(false);
    aBtnDelObjects.Disable();
}

//------------------------------------------------------------------------

IMPL_LINK_NOARG_INLINE_START(ScDeleteContentsDlg, DelAllHdl)
{
    DisableChecks( aBtnDelAll.IsChecked() );

    return 0;
}
IMPL_LINK_NOARG_INLINE_END(ScDeleteContentsDlg, DelAllHdl)

ScDeleteContentsDlg::~ScDeleteContentsDlg()
{
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
