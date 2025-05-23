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

#ifndef CONNECTIVITY_ROWFUNCTIONPARSER_HXX_INCLUDED
#define CONNECTIVITY_ROWFUNCTIONPARSER_HXX_INCLUDED

#include <sal/config.h>
#include <boost/shared_ptr.hpp>
#include "FDatabaseMetaDataResultSet.hxx"
#include <vector>
#include "connectivity/dbtoolsdllapi.hxx"

// -------------------------------------------------------------------------
namespace connectivity
{
// -------------------------------------------------------------------------
struct OOO_DLLPUBLIC_DBTOOLS RowEquation
{
    sal_Int32   nOperation;
    ORowSetValueDecoratorRef    nPara[ 3 ];

    RowEquation() :
        nOperation  ( 0 )
        {
        }
};

enum ExpressionFunct
{
    FUNC_CONST,

    ENUM_FUNC_EQUATION,

    UNARY_FUNC_COLUMN,
    ENUM_FUNC_AND,
    ENUM_FUNC_OR
};

#define EXPRESSION_FLAG_SUMANGLE_MODE 1

class OOO_DLLPUBLIC_DBTOOLS SAL_NO_VTABLE ExpressionNode
{
public:
    virtual ~ExpressionNode(){}

    /** Operator to calculate function value.

        This method calculates the function value.
    */
    virtual ORowSetValueDecoratorRef evaluate(const ODatabaseMetaDataResultSet::ORow& _aRow ) const = 0;

    virtual void fill(const ODatabaseMetaDataResultSet::ORow& _aRow ) const = 0;

    /** Operator to retrieve the type of expression node
    */
    virtual ExpressionFunct getType() const = 0;

    /** Operator to retrieve the ms version of expression
    */
    virtual ODatabaseMetaDataResultSet::ORow fillNode(
        std::vector< RowEquation >& rEquations, ExpressionNode* pOptionalArg, sal_uInt32 nFlags ) = 0;
};
typedef ::boost::shared_ptr< ExpressionNode > ExpressionNodeSharedPtr;

/** This exception is thrown, when the arithmetic expression
    parser failed to parse a string.
    */
struct OOO_DLLPUBLIC_DBTOOLS ParseError
{
    ParseError() {}
    ParseError( const char* ) {}
};

class OOO_DLLPUBLIC_DBTOOLS FunctionParser
{
public:

    /** Parse a string

        The following grammar is accepted by this method:
        <code>

        number_digit = '0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'

        number = number number_digit | number_digit

        equal_function = '='
        ternary_function = 'if'

        string_reference = 'a-z,A-Z,0-9' ' '
        modifier_reference = '$' '0-9' ' '

        basic_expression =
            number |
            string_reference |
            additive_expression equal_function additive_expression |
            unary_function '(' number_digit ')'
            ternary_function '(' additive_expression ',' additive_expression ',
                               ' additive_expression ')' | '(' additive_expression ')'

        </code>

        @param rFunction
        The string to parse

        @throws ParseError if an invalid expression is given.

        @return the generated function object.
       */

    static ExpressionNodeSharedPtr parseFunction( const ::rtl::OUString& _sFunction);

private:
    // disabled constructor/destructor, since this is
    // supposed to be a singleton
    FunctionParser();

    // default: disabled copy/assignment
    FunctionParser(const FunctionParser&);
    FunctionParser& operator=( const FunctionParser& );
};

// -------------------------------------------------------------------------
} // namespace connectivity
// -------------------------------------------------------------------------
#endif /* CONNECTIVITY_ROWFUNCTIONPARSER_HXX_INCLUDED */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
