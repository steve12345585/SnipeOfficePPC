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
#include "vbapane.hxx"
#include <vbahelper/vbahelper.hxx>
#include <tools/diagnose_ex.h>
#include "vbaview.hxx"

using namespace ::ooo::vba;
using namespace ::com::sun::star;

SwVbaPane::SwVbaPane( const uno::Reference< ooo::vba::XHelperInterface >& rParent, const uno::Reference< uno::XComponentContext >& rContext,
    const uno::Reference< frame::XModel >& xModel ) throw ( uno::RuntimeException ) :
    SwVbaPane_BASE( rParent, rContext ), mxModel( xModel )
{
}

SwVbaPane::~SwVbaPane()
{
}

uno::Any SAL_CALL
SwVbaPane::View() throw ( css::uno::RuntimeException )
{
    return uno::makeAny( uno::Reference< word::XView >( new SwVbaView( this,  mxContext, mxModel ) ) );
}

void SAL_CALL
SwVbaPane::Close( ) throw ( css::uno::RuntimeException )
{
    rtl::OUString url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:CloseWin"));
    dispatchRequests( mxModel,url );
}

rtl::OUString
SwVbaPane::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SwVbaPane"));
}

uno::Sequence< rtl::OUString >
SwVbaPane::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Pane" ) );
    }
    return aServiceNames;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
