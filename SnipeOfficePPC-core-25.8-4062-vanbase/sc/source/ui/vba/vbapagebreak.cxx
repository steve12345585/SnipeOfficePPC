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
#include "vbapagebreak.hxx"
#include "vbarange.hxx"
#include <ooo/vba/excel/XlPageBreak.hpp>
using namespace ::com::sun::star;
using namespace ::ooo::vba;

template< typename Ifc1 >
ScVbaPageBreak<Ifc1>::ScVbaPageBreak( const uno::Reference< XHelperInterface >& xParent,
                    const uno::Reference< uno::XComponentContext >& xContext,
                    uno::Reference< beans::XPropertySet >& xProps,
                    sheet::TablePageBreakData aTablePageBreakData) throw (uno::RuntimeException):
            ScVbaPageBreak_BASE( xParent, xContext ),
            mxRowColPropertySet( xProps ),
            maTablePageBreakData( aTablePageBreakData )
{
}

template< typename Ifc1 >
sal_Int32 ScVbaPageBreak<Ifc1>::getType() throw (uno::RuntimeException)
{
    uno::Any aValue = mxRowColPropertySet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsStartOfNewPage" )));
    sal_Bool hasPageBreak = false;
    aValue >>= hasPageBreak;

    if( !hasPageBreak )
        return excel::XlPageBreak::xlPageBreakNone;

    if( maTablePageBreakData.ManualBreak )
        return excel::XlPageBreak::xlPageBreakManual;

    return excel::XlPageBreak::xlPageBreakAutomatic;
}

template< typename Ifc1 >
void ScVbaPageBreak<Ifc1>::setType(sal_Int32 type) throw (uno::RuntimeException)
{
    if( (type != excel::XlPageBreak::xlPageBreakNone) &&
        (type != excel::XlPageBreak::xlPageBreakManual) &&
        (type != excel::XlPageBreak::xlPageBreakAutomatic) )
    {
        DebugHelper::exception(SbERR_BAD_PARAMETER, rtl::OUString() );
    }

    if( type == excel::XlPageBreak::xlPageBreakNone )
    {
        mxRowColPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsStartOfNewPage" )), uno::makeAny(sal_False));
        return;
    }

    mxRowColPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsStartOfNewPage" )), uno::makeAny(sal_True));
    if( type == excel::XlPageBreak::xlPageBreakManual )
        maTablePageBreakData.ManualBreak = sal_True;
    else
        maTablePageBreakData.ManualBreak = false;
}

template< typename Ifc1 >
void ScVbaPageBreak<Ifc1>::Delete() throw ( script::BasicErrorException, uno::RuntimeException)
{
    mxRowColPropertySet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsStartOfNewPage" )), uno::makeAny(sal_False));
}

template< typename Ifc1 >
uno::Reference< excel::XRange> ScVbaPageBreak<Ifc1>::Location() throw ( script::BasicErrorException, uno::RuntimeException)
{
    uno::Reference< table::XCellRange > xRange( mxRowColPropertySet, uno::UNO_QUERY_THROW );
    return new ScVbaRange( ScVbaPageBreak_BASE::getParent(), ScVbaPageBreak_BASE::mxContext, xRange);
}

template< typename Ifc1 >
rtl::OUString
ScVbaPageBreak<Ifc1>::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ScVbaPageBreak"));
}

template< typename Ifc1 >
uno::Sequence< rtl::OUString >
ScVbaPageBreak<Ifc1>::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.excel.PageBreak" ) );
    }
    return aServiceNames;
}

template class ScVbaPageBreak< excel::XHPageBreak >;

/* class ScVbaHPageBreak */
rtl::OUString
ScVbaHPageBreak::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ScVbaHPageBreak"));
}

uno::Sequence< rtl::OUString >
ScVbaHPageBreak::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.excel.HPageBreak" ) );
    }
    return aServiceNames;
}

template class ScVbaPageBreak< excel::XVPageBreak >;

/* class ScVbaVPageBreak */
ScVbaVPageBreak::ScVbaVPageBreak( const css::uno::Reference< ov::XHelperInterface >& xParent,
                                  const css::uno::Reference< css::uno::XComponentContext >& xContext,
                                  css::uno::Reference< css::beans::XPropertySet >& xProps,
                                  css::sheet::TablePageBreakData aTablePageBreakData ) throw ( css::uno::RuntimeException )
:   ScVbaVPageBreak_BASE( xParent, xContext, xProps, aTablePageBreakData )
{
}

ScVbaVPageBreak::~ScVbaVPageBreak()
{
}

rtl::OUString
ScVbaVPageBreak::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ScVbaVPageBreak"));
}

uno::Sequence< rtl::OUString >
ScVbaVPageBreak::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ooo.vba.excel.VPageBreak" ) );
    }
    return aServiceNames;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
