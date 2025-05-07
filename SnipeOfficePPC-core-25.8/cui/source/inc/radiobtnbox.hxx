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
#ifndef _SVX_RADIOBTNBOX_HXX
#define _SVX_RADIOBTNBOX_HXX

// include ---------------------------------------------------------------

#include <svx/simptabl.hxx>

namespace svx {

// class SvxRadioButtonListBox -------------------------------------------

class SvxRadioButtonListBox : public SvxSimpleTable
{
    using SvxSimpleTable::SetTabs;

private:
    Point               m_aCurMousePoint;

protected:
    virtual void        SetTabs();
    virtual void        MouseButtonUp( const MouseEvent& _rMEvt );
    virtual void        KeyInput( const KeyEvent& rKEvt );

public:
    SvxRadioButtonListBox(SvxSimpleTableContainer& rParent, WinBits nBits = WB_BORDER);
    ~SvxRadioButtonListBox();

    void                HandleEntryChecked( SvLBoxEntry* _pEntry );

    const Point&        GetCurMousePoint() const;
};

} // end of namespace ::svx

#endif // #ifndef _SVX_RADIOBTNBOX_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
