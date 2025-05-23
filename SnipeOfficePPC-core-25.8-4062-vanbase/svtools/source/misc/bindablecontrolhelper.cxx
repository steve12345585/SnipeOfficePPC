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

#include "bindablecontrolhelper.hxx"
#include <com/sun/star/form/binding/XBindableValue.hpp>
#include <com/sun/star/form/binding/XValueBinding.hpp>
#include <com/sun/star/form/binding/XListEntrySink.hpp>
#include <com/sun/star/form/binding/XListEntrySource.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/sheet/XCellRangeReferrer.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/beans/NamedValue.hpp>

//........................................................................
namespace svt
{
//........................................................................

#ifndef C2U
#define C2U(cChar)  rtl::OUString::createFromAscii(cChar)
#endif

    using namespace ::com::sun::star;

bool lcl_isNamedRange( const rtl::OUString& sAddress, const uno::Reference< frame::XModel >& xModel, table::CellRangeAddress& aAddress )
{
    bool bRes = false;
    const static rtl::OUString sNamedRanges( RTL_CONSTASCII_USTRINGPARAM("NamedRanges"));
    uno::Reference< sheet::XCellRangeReferrer > xReferrer;
    try
    {
        uno::Reference< beans::XPropertySet > xPropSet( xModel, uno::UNO_QUERY_THROW );
        uno::Reference< container::XNameAccess > xNamed( xPropSet->getPropertyValue( sNamedRanges ), uno::UNO_QUERY_THROW );
        xReferrer.set ( xNamed->getByName( sAddress ), uno::UNO_QUERY );
    }
    catch( uno::Exception& /*e*/ )
    {
        // do nothing
    }
    if ( xReferrer.is() )
    {
        uno::Reference< sheet::XCellRangeAddressable > xRangeAddressable( xReferrer->getReferredCells(), uno::UNO_QUERY );
        if ( xRangeAddressable.is() )
        {
            aAddress = xRangeAddressable->getRangeAddress();
            bRes = true;
        }
    }
    return bRes;
}


void
BindableControlHelper::ApplyListSourceAndBindableData( const com::sun::star::uno::Reference< com::sun::star::frame::XModel >& xModel, const com::sun::star::uno::Reference< com::sun::star::uno::XInterface >& rObj, const rtl::OUString& rsCtrlSource, const rtl::OUString& rsRowSource, sal_uInt16 nRefTab )
{
// XBindable etc.
    uno::Reference< lang::XMultiServiceFactory > xFac;
    if ( xModel.is() )
        xFac.set( xModel, uno::UNO_QUERY );
    uno::Reference< form::binding::XBindableValue > xBindable( rObj, uno::UNO_QUERY );
    if (  xFac.is() && rsCtrlSource.getLength() && xBindable.is() )
    {

         // OOo address structures
         // RefCell - convert from XL
         // pretend we converted the imported string address into the
         // appropriate address structure
         uno::Reference< beans::XPropertySet > xConvertor( xFac->createInstance( C2U( "com.sun.star.table.CellAddressConversion" )), uno::UNO_QUERY );
         table::CellAddress aAddress;
         if ( xConvertor.is() )
         {
             // we need this service to properly convert XL notation also
             // Should be easy to extend
             xConvertor->setPropertyValue( C2U( "ReferenceSheet" ), uno::makeAny( nRefTab ) );
             xConvertor->setPropertyValue( C2U( "XLA1Representation" ), uno::makeAny( rsCtrlSource ) );
             xConvertor->getPropertyValue( C2U( "Address" ) ) >>= aAddress;
         }

         beans::NamedValue aArg1;
         aArg1.Name = C2U("BoundCell");
         aArg1.Value <<= aAddress;

         uno::Sequence< uno::Any > aArgs(1);
         aArgs[ 0 ]  <<= aArg1;

         uno::Reference< form::binding::XValueBinding > xBinding( xFac->createInstanceWithArguments( C2U("com.sun.star.table.CellValueBinding" ), aArgs ), uno::UNO_QUERY );
         xBindable->setValueBinding( xBinding );
    }
    else if ( xBindable.is() ) // reset it
        xBindable->setValueBinding( uno::Reference< form::binding::XValueBinding >() );
    uno::Reference< form::binding::XListEntrySink > xListEntrySink( rObj, uno::UNO_QUERY );
    if (  xFac.is() && rsRowSource.getLength() && xListEntrySink.is() )
    {

         // OOo address structures
         // RefCell - convert from XL
         // pretend we converted the imported string address into the
         // appropriate address structure
         uno::Reference< beans::XPropertySet > xConvertor( xFac->createInstance( C2U( "com.sun.star.table.CellRangeAddressConversion" )), uno::UNO_QUERY );
         table::CellRangeAddress aAddress;
         if ( xConvertor.is() )
         {
             if ( !lcl_isNamedRange( rsRowSource, xModel, aAddress ) )
             {
                 // we need this service to properly convert XL notation also
                 // Should be easy to extend
                 xConvertor->setPropertyValue( C2U( "XLA1Representation" ), uno::makeAny( rsRowSource ) );
                 xConvertor->getPropertyValue( C2U( "Address" ) ) >>= aAddress;
             }
         }

         beans::NamedValue aArg1;
         aArg1.Name = C2U("CellRange");
         aArg1.Value <<= aAddress;

         uno::Sequence< uno::Any > aArgs(1);
         aArgs[ 0 ]  <<= aArg1;

         uno::Reference< form::binding::XListEntrySource > xSource( xFac->createInstanceWithArguments( C2U("com.sun.star.table.CellRangeListSource" ), aArgs ), uno::UNO_QUERY );
         xListEntrySink->setListEntrySource( xSource );
    }
    else if (  xListEntrySink.is() ) // reset
         xListEntrySink->setListEntrySource( uno::Reference< form::binding::XListEntrySource >()  );

}

//........................................................................
} // namespace svt
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
