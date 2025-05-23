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
#include "cppuhelper/implementationentry.hxx"
#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "com/sun/star/registry/XRegistryKey.hpp"
#include "comphelper/servicedecl.hxx"

// =============================================================================
// component exports
// =============================================================================
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace sdecl = comphelper::service_decl;

// reference service helper(s)
namespace  range
{
extern sdecl::ServiceDecl const serviceDecl;
}
namespace  workbook
{
extern sdecl::ServiceDecl const serviceDecl;
}
namespace  worksheet
{
extern sdecl::ServiceDecl const serviceDecl;
}
namespace window
{
extern sdecl::ServiceDecl const serviceDecl;
}
namespace globals
{
extern sdecl::ServiceDecl const serviceDecl;
}
namespace hyperlink
{
extern sdecl::ServiceDecl const serviceDecl;
}
namespace application
{
extern sdecl::ServiceDecl const serviceDecl;
}
namespace vbaeventshelper
{
extern sdecl::ServiceDecl const serviceDecl;
}
namespace textframe
{
extern sdecl::ServiceDecl const serviceDecl;
}

extern "C"
{
    SAL_DLLPUBLIC_EXPORT void * SAL_CALL vbaobj_component_getFactory(
        const sal_Char * pImplName, lang::XMultiServiceFactory * pServiceManager,
        registry::XRegistryKey * pRegistryKey )
    {
        OSL_TRACE("In vbaobj_component_getFactory for %s", pImplName );
    void* pRet =  component_getFactoryHelper(
            pImplName, pServiceManager, pRegistryKey, range::serviceDecl, workbook::serviceDecl, worksheet::serviceDecl, globals::serviceDecl, window::serviceDecl, hyperlink::serviceDecl, application::serviceDecl );
    if( !pRet )
        pRet = component_getFactoryHelper( pImplName, pServiceManager, pRegistryKey, vbaeventshelper::serviceDecl, textframe::serviceDecl );
    OSL_TRACE("Ret is 0x%x", pRet);
    return pRet;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
