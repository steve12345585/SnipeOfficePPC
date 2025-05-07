/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
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
#ifndef _IDLC_ASTCONSTANT_HXX_
#define _IDLC_ASTCONSTANT_HXX_

#include <idlc/astdeclaration.hxx>
#include <idlc/astexpression.hxx>

namespace typereg { class Writer; }

class AstConstant : public AstDeclaration
{
public:
    AstConstant(const ExprType type, const NodeType nodeType,
                AstExpression* pExpr, const ::rtl::OString& name, AstScope* pScope);
    AstConstant(const ExprType type, AstExpression* pExpr,
                const ::rtl::OString& name, AstScope* pScope);
    virtual ~AstConstant();

    AstExpression* getConstValue()
        { return m_pConstValue; }
    ExprType getConstValueType() const
        { return m_constValueType; }

    sal_Bool dumpBlob(
        typereg::Writer & rBlob, sal_uInt16 index, bool published);
private:
    AstExpression*                  m_pConstValue;
    const ExprType  m_constValueType;
};

#endif // _IDLC_ASTCONSTANT_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
