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
#include "vbareplacement.hxx"
#include <vbahelper/vbahelper.hxx>
#include <tools/diagnose_ex.h>

using namespace ::ooo::vba;
using namespace ::com::sun::star;

SwVbaReplacement::SwVbaReplacement( const uno::Reference< ooo::vba::XHelperInterface >& rParent, const uno::Reference< uno::XComponentContext >& rContext, const uno::Reference< util::XPropertyReplace >& xPropertyReplace ) throw ( uno::RuntimeException ) :
    SwVbaReplacement_BASE( rParent, rContext ), mxPropertyReplace( xPropertyReplace )
{
}

SwVbaReplacement::~SwVbaReplacement()
{
}

::rtl::OUString SAL_CALL SwVbaReplacement::getText() throw (uno::RuntimeException)
{
    return mxPropertyReplace->getReplaceString();
}

void SAL_CALL SwVbaReplacement::setText( const ::rtl::OUString& _text ) throw (uno::RuntimeException)
{
    mxPropertyReplace->setReplaceString( _text );
}

void SAL_CALL SwVbaReplacement::ClearFormatting( ) throw (uno::RuntimeException)
{
    uno::Sequence< beans::PropertyValue > aPropValues;
    mxPropertyReplace->setReplaceAttributes( aPropValues );
}

rtl::OUString
SwVbaReplacement::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SwVbaReplacement"));
}

uno::Sequence< rtl::OUString >
SwVbaReplacement::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Replacement" ) );
    }
    return aServiceNames;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
