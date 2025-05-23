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
#include "vbaaxistitle.hxx"

using namespace ::com::sun::star;
using namespace ::ooo::vba;

ScVbaAxisTitle::ScVbaAxisTitle( const uno::Reference< XHelperInterface >& xParent,  const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XShape >& _xTitleShape ) :  AxisTitleBase( xParent, xContext, _xTitleShape )
{
}

rtl::OUString
ScVbaAxisTitle::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ScVbaAxisTitle"));
}

uno::Sequence< rtl::OUString >
ScVbaAxisTitle::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        uno::Sequence< rtl::OUString > BaseServiceNames = AxisTitleBase::getServiceNames();
        aServiceNames.realloc( BaseServiceNames.getLength() + 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.excel.AxisTitle" ) );
        for ( sal_Int32 index = 1; index < (BaseServiceNames.getLength() + 1); ++index )
            aServiceNames[ index ] = BaseServiceNames[ index ];
    }
    return aServiceNames;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
