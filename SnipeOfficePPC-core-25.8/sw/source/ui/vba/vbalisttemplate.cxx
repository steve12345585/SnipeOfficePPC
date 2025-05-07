/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
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
#include "vbalisttemplate.hxx"
#include <vbahelper/vbahelper.hxx>
#include <tools/diagnose_ex.h>
#include "vbalistlevels.hxx"

using namespace ::ooo::vba;
using namespace ::com::sun::star;

SwVbaListTemplate::SwVbaListTemplate( const uno::Reference< ooo::vba::XHelperInterface >& rParent, const uno::Reference< uno::XComponentContext >& rContext, const uno::Reference< text::XTextDocument >& xTextDoc, sal_Int32 nGalleryType, sal_Int32 nTemplateType ) throw ( uno::RuntimeException ) : SwVbaListTemplate_BASE( rParent, rContext )
{
    pListHelper.reset( new SwVbaListHelper( xTextDoc, nGalleryType, nTemplateType ) );
}

SwVbaListTemplate::~SwVbaListTemplate()
{
}

uno::Any SAL_CALL
SwVbaListTemplate::ListLevels( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< XCollection > xCol( new SwVbaListLevels( mxParent, mxContext, pListHelper ) );
    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

void SwVbaListTemplate::applyListTemplate( uno::Reference< beans::XPropertySet >& xProps ) throw (uno::RuntimeException)
{
    uno::Reference< container::XIndexReplace > xNumberingRules = pListHelper->getNumberingRules();
    xProps->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("NumberingRules") ) , uno::makeAny( xNumberingRules ) );
}

rtl::OUString
SwVbaListTemplate::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SwVbaListTemplate"));
}

uno::Sequence< rtl::OUString >
SwVbaListTemplate::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.ListTemplate" ) );
    }
    return aServiceNames;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
