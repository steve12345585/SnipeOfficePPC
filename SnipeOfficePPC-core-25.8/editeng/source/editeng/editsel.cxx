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


#include <eeng_pch.hxx>

#include <editsel.hxx>
#include <impedit.hxx>
#include <editeng/editview.hxx>

//  ----------------------------------------------------------------------
//  class EditSelFunctionSet
//  ----------------------------------------------------------------------
EditSelFunctionSet::EditSelFunctionSet()
{
    pCurView = NULL;
}

void EditSelFunctionSet::CreateAnchor()
{
    if ( pCurView )
        pCurView->pImpEditView->CreateAnchor();
}

void EditSelFunctionSet::DestroyAnchor()
{
    // Only with multiple selection
}

sal_Bool EditSelFunctionSet::SetCursorAtPoint( const Point& rPointPixel, sal_Bool )
{
    if ( pCurView )
        return pCurView->pImpEditView->SetCursorAtPoint( rPointPixel );

    return sal_False;
}

sal_Bool EditSelFunctionSet::IsSelectionAtPoint( const Point& rPointPixel )
{
    if ( pCurView )
        return pCurView->pImpEditView->IsSelectionAtPoint( rPointPixel );

    return sal_False;
}

void EditSelFunctionSet::DeselectAtPoint( const Point& )
{
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !   Implement when multiple selection is possible   !
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void EditSelFunctionSet::BeginDrag()
{
    // Only with multiple selection
}


void EditSelFunctionSet::DeselectAll()
{
    if ( pCurView )
        pCurView->pImpEditView->DeselectAll();
}

//  ----------------------------------------------------------------------
//  class EditSelectionEngine
//  ----------------------------------------------------------------------
EditSelectionEngine::EditSelectionEngine() : SelectionEngine( (Window*)0 )
{
    SetSelectionMode( RANGE_SELECTION );
    EnableDrag( sal_True );
}

void EditSelectionEngine::SetCurView( EditView* pNewView )
{
    if ( GetFunctionSet() )
        ((EditSelFunctionSet*)GetFunctionSet())->SetCurView( pNewView );

    if ( pNewView )
        SetWindow( pNewView->GetWindow() );
    else
        SetWindow( (Window*)0 );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
