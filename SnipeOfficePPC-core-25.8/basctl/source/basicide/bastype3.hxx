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
#ifndef _BASTYPE3_HXX
#define _BASTYPE3_HXX

#include <svheader.hxx>

#include <svtools/svmedit.hxx>

#include <iderid.hxx>

class EditorWindow;

#ifndef NO_SPECIALEDIT

class ExtendedEdit : public Edit
{
private:
    Accelerator     aAcc;
    Link            aAccHdl;
    Link            aGotFocusHdl;
    Link            aLoseFocusHdl;

protected:
    DECL_LINK( EditAccHdl, Accelerator * );
    DECL_LINK(ImplGetFocusHdl, void *);
    DECL_LINK(ImplLoseFocusHdl, void *);

public:
                    ExtendedEdit( Window* pParent, IDEResId nRes );

    void            SetAccHdl( const Link& rLink )          { aAccHdl = rLink; }
    void            SetLoseFocusHdl( const Link& rLink )    { aLoseFocusHdl = rLink; }
    void            SetGotFocusHdl( const Link& rLink )     { aGotFocusHdl = rLink; }
    Accelerator&    GetAccelerator()                        { return aAcc; }
};

#endif  //NO_SPECIALEDIT

#endif  // _BASTYPE3_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
