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

#ifndef _EDITSEL_HXX
#define _EDITSEL_HXX

#include <vcl/seleng.hxx>

class EditView;

class EditSelFunctionSet: public FunctionSet
{
private:
    EditView*       pCurView;

public:
                    EditSelFunctionSet();

    virtual void    BeginDrag();

    virtual void    CreateAnchor();
    virtual void    DestroyAnchor();

    virtual sal_Bool    SetCursorAtPoint( const Point& rPointPixel, sal_Bool bDontSelectAtCursor = sal_False );

    virtual sal_Bool    IsSelectionAtPoint( const Point& rPointPixel );
    virtual void    DeselectAtPoint( const Point& rPointPixel );
    virtual void    DeselectAll();

    void            SetCurView( EditView* pView )       { pCurView = pView; }
};

class EditSelectionEngine : public SelectionEngine
{
private:

public:
                    EditSelectionEngine();

    void            SetCurView( EditView* pNewView );
};

#endif // _EDITSEL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
