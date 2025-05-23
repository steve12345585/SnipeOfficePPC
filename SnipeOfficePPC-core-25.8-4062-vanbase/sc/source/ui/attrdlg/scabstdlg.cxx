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


#include "scabstdlg.hxx"

#include <osl/module.hxx>
#include <rtl/ustrbuf.hxx>

using ::rtl::OUStringBuffer;

typedef ScAbstractDialogFactory* (__LOADONCALLAPI *ScFuncPtrCreateDialogFactory)();

#ifndef DISABLE_DYNLOADING

extern "C" { static void SAL_CALL thisModule() {} }

#else

extern "C" ScAbstractDialogFactory* ScCreateDialogFactory();

#endif

ScAbstractDialogFactory* ScAbstractDialogFactory::Create()
{
#ifndef DISABLE_DYNLOADING
    ScFuncPtrCreateDialogFactory fp = 0;
    static ::osl::Module aDialogLibrary;

    OUStringBuffer aStrBuf;
    aStrBuf.appendAscii( SVLIBRARY("scui") );

    if ( aDialogLibrary.is() || aDialogLibrary.loadRelative( &thisModule, aStrBuf.makeStringAndClear(),
                                                             SAL_LOADMODULE_GLOBAL | SAL_LOADMODULE_LAZY ) )
        fp = ( ScAbstractDialogFactory* (__LOADONCALLAPI*)() )
            aDialogLibrary.getFunctionSymbol( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CreateDialogFactory")) );
    if ( fp )
        return fp();
    return 0;
#else
    return ScCreateDialogFactory();
#endif
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
