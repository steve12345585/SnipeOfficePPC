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


#include "swabstdlg.hxx"

#include <osl/module.hxx>
#include <tools/string.hxx>
#include <vcl/unohelp.hxx>

typedef SwAbstractDialogFactory* (__LOADONCALLAPI *SwFuncPtrCreateDialogFactory)();

extern "C" { static void SAL_CALL thisModule() {} }

SwAbstractDialogFactory* SwAbstractDialogFactory::Create()
{
    SwFuncPtrCreateDialogFactory fp = 0;
    static ::osl::Module aDialogLibrary;
    static const ::rtl::OUString sLibName(::vcl::unohelper::CreateLibraryName("swui", sal_True));
    if ( aDialogLibrary.is() || aDialogLibrary.loadRelative( &thisModule, String( sLibName ),
                                                             SAL_LOADMODULE_GLOBAL | SAL_LOADMODULE_LAZY ) )
        fp = ( SwAbstractDialogFactory* (__LOADONCALLAPI*)() )
            aDialogLibrary.getFunctionSymbol( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CreateDialogFactory")));
    if ( fp )
        return fp();
    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
