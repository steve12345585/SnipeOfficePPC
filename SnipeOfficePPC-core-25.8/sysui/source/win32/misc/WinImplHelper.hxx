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


#ifndef _WINIMPLHELPER_HXX_
#define _WINIMPLHELPER_HXX_

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#include <sal/types.h>

#include <rtl/ustring>

#include <windows.h>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/lang/IllegalArgumentException.hpp>

//------------------------------------------------------------------------
// deklarations
//------------------------------------------------------------------------

sal_Bool SAL_CALL IsWin2000( );

// set actions
void SAL_CALL ListboxAddItem(
    HWND hwnd, const ::com::sun::star::uno::Any& aItem, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rXInterface, sal_Int16 aArgPos )
    throw( ::com::sun::star::lang::IllegalArgumentException );

void SAL_CALL ListboxAddItems(
    HWND hwnd, const ::com::sun::star::uno::Any& aItemList, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rXInterface, sal_Int16 aArgPos )
    throw( ::com::sun::star::lang:: IllegalArgumentException );

void SAL_CALL ListboxDeleteItem(
    HWND hwnd, const ::com::sun::star::uno::Any& aPosition, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rXInterface, sal_Int16 aArgPos )
    throw( ::com::sun::star::lang::IllegalArgumentException );

void SAL_CALL ListboxDeleteItems(
    HWND hwnd, const ::com::sun::star::uno::Any& aUnused, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rXInterface, sal_Int16 aArgPos )
    throw( ::com::sun::star::lang::IllegalArgumentException );

void SAL_CALL ListboxSetSelectedItem(
    HWND hwnd, const ::com::sun::star::uno::Any& aPosition, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rXInterface, sal_Int16 aArgPos )
    throw( ::com::sun::star::lang::IllegalArgumentException );

// get actions
::com::sun::star::uno::Any SAL_CALL ListboxGetItems( HWND hwnd );
::com::sun::star::uno::Any SAL_CALL ListboxGetSelectedItem( HWND hwnd );

// checkbox helper functions
::com::sun::star::uno::Any SAL_CALL CheckboxGetState( HWND hwnd );

void SAL_CALL CheckboxSetState(
    HWND hwnd, const ::com::sun::star::uno::Any& aState, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rXInterface, sal_Int16 aArgPos )
    throw( ::com::sun::star::lang::IllegalArgumentException );

// calculates the length of '\0' separated and '\0\0'
// ending strings used in some Win32 functions
// e.g. Filter\0*.txt\0\0
// the returned length excludes the last '\0'
sal_uInt32 SAL_CALL _wcslenex( const sal_Unicode* pStr );

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
