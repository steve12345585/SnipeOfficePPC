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

#include "vbacondition.hxx"
#include <ooo/vba/excel/XlFormatConditionOperator.hpp>
#include <ooo/vba/excel/XFormatCondition.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>

using namespace ::ooo::vba;
using namespace ::com::sun::star;

const sal_Int32 ISFORMULA = 98765432;

template< typename Ifc1 >
ScVbaCondition< Ifc1 >::ScVbaCondition(  const uno::Reference< XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext > & xContext, const uno::Reference< sheet::XSheetCondition >& _xSheetCondition ) : ScVbaCondition_BASE( xParent, xContext ), mxSheetCondition( _xSheetCondition )
{
    mxAddressable.set( xParent, uno::UNO_QUERY_THROW );
}

template< typename Ifc1 >
sheet::ConditionOperator
ScVbaCondition< Ifc1 >::retrieveAPIOperator( const uno::Any& _aOperator) throw ( script::BasicErrorException )
{
    sheet::ConditionOperator aRetAPIOperator = sheet::ConditionOperator_NONE;
    sal_Int32 nOperator = 0;
    if ( (_aOperator >>= nOperator ) )
    {
        switch(nOperator)
        {
            case excel::XlFormatConditionOperator::xlBetween:
                aRetAPIOperator = sheet::ConditionOperator_BETWEEN;
                break;
            case  excel::XlFormatConditionOperator::xlNotBetween:
                aRetAPIOperator = sheet::ConditionOperator_NOT_BETWEEN;
                break;
            case excel::XlFormatConditionOperator::xlEqual:
                aRetAPIOperator = sheet::ConditionOperator_EQUAL;
                break;
            case excel::XlFormatConditionOperator::xlNotEqual:
                aRetAPIOperator = sheet::ConditionOperator_NOT_EQUAL;
                break;
            case excel::XlFormatConditionOperator::xlGreater:
                aRetAPIOperator = sheet::ConditionOperator_GREATER;
                break;
            case excel::XlFormatConditionOperator::xlLess:
                aRetAPIOperator = sheet::ConditionOperator_LESS;
                break;
            case excel::XlFormatConditionOperator::xlGreaterEqual:
                aRetAPIOperator = sheet::ConditionOperator_GREATER_EQUAL;
                break;
            case excel::XlFormatConditionOperator::xlLessEqual:
                aRetAPIOperator = sheet::ConditionOperator_LESS_EQUAL;
                break;
            default:
                aRetAPIOperator = sheet::ConditionOperator_NONE;
                break;
        }
    }
    return aRetAPIOperator;
}

template< typename Ifc1 >
rtl::OUString
ScVbaCondition< Ifc1 >::Formula1( ) throw ( script::BasicErrorException, uno::RuntimeException )
{
     return mxSheetCondition->getFormula1();
}

template< typename Ifc1 >
rtl::OUString
ScVbaCondition< Ifc1 >::Formula2( ) throw ( script::BasicErrorException, uno::RuntimeException )
{
     return mxSheetCondition->getFormula2();
}

template< typename Ifc1 >
void
ScVbaCondition< Ifc1 >::setFormula1( const uno::Any& _aFormula1) throw ( script::BasicErrorException )
{
    rtl::OUString sFormula;
    if ( (_aFormula1 >>= sFormula ))
    {
        mxSheetCondition->setFormula1( sFormula );
        table::CellRangeAddress aCellRangeAddress = mxAddressable->getRangeAddress();
        table::CellAddress aCellAddress( aCellRangeAddress.Sheet, aCellRangeAddress.StartColumn,  aCellRangeAddress.StartRow );
        mxSheetCondition->setSourcePosition(aCellAddress);
    }
}

template< typename Ifc1 >
void
ScVbaCondition< Ifc1 >::setFormula2( const uno::Any& _aFormula2) throw ( script::BasicErrorException )
{
    rtl::OUString sFormula2;
    // #TODO surely this can't be right?
    // ( from helperapi/impl/.../calc/ConditionImpl.java
    if ( (_aFormula2 >>= sFormula2 ))
        mxSheetCondition->setFormula1(sFormula2);
}

template< typename Ifc1 >
sal_Int32
ScVbaCondition< Ifc1 >::Operator(sal_Bool _bIncludeFormulaValue) throw ( script::BasicErrorException )
{
    sal_Int32 retvalue = -1;
    sheet::ConditionOperator aConditionalOperator =  mxSheetCondition->getOperator();
    switch (aConditionalOperator)
    {
        case sheet::ConditionOperator_EQUAL:
            retvalue = excel::XlFormatConditionOperator::xlEqual;
            break;
        case sheet::ConditionOperator_NOT_EQUAL:
            retvalue = excel::XlFormatConditionOperator::xlNotEqual;
            break;
        case sheet::ConditionOperator_GREATER:
            retvalue = excel::XlFormatConditionOperator::xlGreater;
            break;
        case sheet::ConditionOperator_GREATER_EQUAL:
            retvalue = excel::XlFormatConditionOperator::xlGreaterEqual;
            break;
        case sheet::ConditionOperator_LESS:
            retvalue = excel::XlFormatConditionOperator::xlLess;
            break;
        case sheet::ConditionOperator_LESS_EQUAL:
            retvalue = excel::XlFormatConditionOperator::xlLessEqual;
            break;
        case sheet::ConditionOperator_BETWEEN:
            retvalue = excel::XlFormatConditionOperator::xlBetween;
            break;
        case sheet::ConditionOperator_NOT_BETWEEN:
            retvalue = excel::XlFormatConditionOperator::xlNotBetween;
            break;
        case sheet::ConditionOperator_FORMULA:
            if (_bIncludeFormulaValue)
            {
                //#FIXME huh what's this all about
                // from helperapi/impl/.../calc/ConditionImpl
                retvalue = ISFORMULA;
                break;
            }
        case sheet::ConditionOperator_NONE:
        default:
            DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Operator not supported")));
        break;
    }
    return retvalue;
}

template class ScVbaCondition< excel::XFormatCondition >;

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
