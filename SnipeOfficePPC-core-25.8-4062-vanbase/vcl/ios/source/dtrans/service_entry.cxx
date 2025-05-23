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


#include "osl/diagnose.h"

#include "vcl/svapp.hxx"

#include "ios/saldata.hxx"
#include "ios/salinst.h"

#include "ios_clipboard.hxx"

using namespace ::osl;
using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::cppu;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::datatransfer::clipboard;


uno::Reference< XInterface > IosSalInstance::CreateClipboard( const Sequence< Any >& i_rArguments )
{
    if ( Application::IsHeadlessModeEnabled() )
        return SalInstance::CreateClipboard( i_rArguments );

    SalData* pSalData = GetSalData();
    if( ! pSalData->mxClipboard.is() )
        pSalData->mxClipboard = uno::Reference<XInterface>(static_cast< XClipboard* >(new IosClipboard()), UNO_QUERY);
    return pSalData->mxClipboard;
}

uno::Reference<XInterface> IosSalInstance::CreateDragSource()
{
    // ???
    return SalInstance::CreateDragSource();
}

uno::Reference<XInterface> IosSalInstance::CreateDropTarget()
{
    // ???
    return SalInstance::CreateDropTarget();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
