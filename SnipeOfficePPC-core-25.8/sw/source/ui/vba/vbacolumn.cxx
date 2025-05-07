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
#include "vbacolumn.hxx"
#include <vbahelper/vbahelper.hxx>
#include <tools/diagnose_ex.h>
#include "vbatable.hxx"
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <rtl/ustrbuf.hxx>
#include "wordvbahelper.hxx"
#include "vbatablehelper.hxx"

#define RELATIVE_TABLE_WIDTH 10000

using namespace ::ooo::vba;
using namespace ::com::sun::star;

SwVbaColumn::SwVbaColumn( const uno::Reference< ooo::vba::XHelperInterface >& rParent, const uno::Reference< uno::XComponentContext >& rContext, const uno::Reference< text::XTextTable >& xTextTable, sal_Int32 nIndex ) throw ( uno::RuntimeException ) :
    SwVbaColumn_BASE( rParent, rContext ), mxTextTable( xTextTable ), mnIndex( nIndex )
{
    mxTableColumns = mxTextTable->getColumns();
}

SwVbaColumn::~SwVbaColumn()
{
}

sal_Int32 SAL_CALL
SwVbaColumn::getWidth( ) throw ( css::uno::RuntimeException )
{
    SwVbaTableHelper aTableHelper( mxTextTable );
    return aTableHelper.GetColWidth( mnIndex );
}

void SAL_CALL
SwVbaColumn::setWidth( sal_Int32 _width ) throw ( css::uno::RuntimeException )
{

    SwVbaTableHelper aTableHelper( mxTextTable );
    aTableHelper.SetColWidth( _width, mnIndex );
}

void SAL_CALL
SwVbaColumn::Select( ) throw ( uno::RuntimeException )
{
    SelectColumn( getCurrentWordDoc(mxContext), mxTextTable, mnIndex, mnIndex );
}

void SwVbaColumn::SelectColumn( const uno::Reference< frame::XModel >& xModel, const uno::Reference< text::XTextTable >& xTextTable, sal_Int32 nStartColumn, sal_Int32 nEndColumn ) throw ( uno::RuntimeException )
{
    rtl::OUStringBuffer aRangeName;
    rtl::OUString sStartCol = SwVbaTableHelper::getColumnStr( nStartColumn );
    aRangeName.append(sStartCol).append(sal_Int32( 1 ) );
    rtl::OUString sEndCol = SwVbaTableHelper::getColumnStr( nEndColumn );
    sal_Int32 nRowCount = xTextTable->getRows()->getCount();
    aRangeName.append(':').append( sEndCol ).append( sal_Int32( nRowCount ) );

    uno::Reference< table::XCellRange > xCellRange( xTextTable, uno::UNO_QUERY_THROW );
    rtl::OUString sSelRange = aRangeName.makeStringAndClear();
    uno::Reference< table::XCellRange > xSelRange = xCellRange->getCellRangeByName( sSelRange );

    uno::Reference< view::XSelectionSupplier > xSelection( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    xSelection->select( uno::makeAny( xSelRange ) );
}

rtl::OUString
SwVbaColumn::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SwVbaColumn"));
}

uno::Sequence< rtl::OUString >
SwVbaColumn::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Column" ) );
    }
    return aServiceNames;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
