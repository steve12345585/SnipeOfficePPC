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
#include <ooo/vba/excel/XDialog.hpp>
#include "vbadialogs.hxx"
#include "vbadialog.hxx"

using namespace ::ooo::vba;
using namespace ::com::sun::star;

void
ScVbaDialogs::Dummy() throw (uno::RuntimeException)
{
}

uno::Any
ScVbaDialogs::Item( const uno::Any &aItem ) throw (uno::RuntimeException)
{
    sal_Int32 nIndex = 0;
    aItem >>= nIndex;
    uno::Reference< excel::XDialog > aDialog( new ScVbaDialog( uno::Reference< XHelperInterface >( Application(),uno::UNO_QUERY_THROW ), mxContext, m_xModel, nIndex ) );
    return uno::Any( aDialog );
}

rtl::OUString
ScVbaDialogs::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ScVbaDialogs"));
}

uno::Sequence< rtl::OUString >
ScVbaDialogs::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.excel.Dialogs" ) );
    }
    return aServiceNames;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
