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

#ifndef _IDLC_ASTPARAMETER_HXX_
#define _IDLC_ASTPARAMETER_HXX_

#include <idlc/astmember.hxx>

enum Direction { DIR_IN, DIR_OUT, DIR_INOUT };

class AstParameter: public AstMember {
public:
    AstParameter(
        Direction direction, bool rest, AstType const * type,
        rtl::OString const & name, AstScope * scope):
        AstMember(NT_parameter, type, name, scope), m_direction(direction),
        m_rest(rest) {}

    virtual ~AstParameter() {}

    Direction getDirection() const { return m_direction; }

    bool isRest() const { return m_rest; }

private:
    Direction m_direction;
    bool m_rest;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
