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

#include "scdlgfact.hxx"
#include "sal/types.h"

namespace scui
{
    static ScAbstractDialogFactory_Impl* pFactory=NULL;
    ScAbstractDialogFactory_Impl* GetFactory()
    {
        if ( !pFactory )
            pFactory = new ScAbstractDialogFactory_Impl;
        //if ( !pSwResMgr)
        //  ScDialogsResMgr::GetResMgr();
        return pFactory;
    }
}

#ifdef DISABLE_DYNLOADING
#define CreateDialogFactory ScCreateDialogFactory
#endif

extern "C"
{
    SAL_DLLPUBLIC_EXPORT ScAbstractDialogFactory* CreateDialogFactory()
    {
        return ::scui::GetFactory();
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
