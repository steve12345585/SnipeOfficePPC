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
#ifndef _IDLC_ASTBASETYPE_HXX_
#define _IDLC_ASTBASETYPE_HXX_

#include <idlc/asttype.hxx>
#include <idlc/astexpression.hxx>


class AstBaseType : public AstType
{
public:
    AstBaseType(const ExprType type, const ::rtl::OString& name, AstScope* pScope)
        : AstType(NT_predefined, name, pScope)
        , m_exprType(type)
        {}

    virtual ~AstBaseType() {}

    virtual bool isUnsigned() const {
        switch (m_exprType) {
        case ET_ushort:
        case ET_ulong:
        case ET_uhyper:
            return true;
        default:
            return false;
        }
    }

    ExprType getExprType() const
        { return m_exprType; }
private:
    const ExprType m_exprType;
};

#endif // _IDLC_ASTBASETYPE_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
