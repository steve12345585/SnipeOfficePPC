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

#ifndef INCLUDED_idlc_inc_idlc_aststructinstance_hxx
#define INCLUDED_idlc_inc_idlc_aststructinstance_hxx

#include "idlc/asttype.hxx"
#include "idlc/idlctypes.hxx"

class AstDeclaration;
class AstScope;

class AstStructInstance: public AstType {
public:
    AstStructInstance(
        AstType const * typeTemplate, DeclList const * typeArguments,
        AstScope * scope);

    AstType const * getTypeTemplate() const { return m_typeTemplate; }

    DeclList::const_iterator getTypeArgumentsBegin() const
    { return m_typeArguments.begin(); }

    DeclList::const_iterator getTypeArgumentsEnd() const
    { return m_typeArguments.end(); }

private:
    AstType const * m_typeTemplate;
    DeclList m_typeArguments;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
