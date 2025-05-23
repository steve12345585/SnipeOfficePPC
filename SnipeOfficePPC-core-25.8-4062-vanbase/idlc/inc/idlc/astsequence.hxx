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
#ifndef _IDLC_ASTSEQUENCE_HXX_
#define _IDLC_ASTSEQUENCE_HXX_

#include <idlc/asttype.hxx>

class AstSequence : public AstType
{
public:
    AstSequence(AstType* pMemberType, AstScope* pScope)
        : AstType(NT_sequence, ::rtl::OString("[]")+pMemberType->getScopedName(), pScope)
        , m_pMemberType(pMemberType)
        , m_pRelativName(NULL)
    {}
    virtual ~AstSequence()
    {
        if ( m_pRelativName )
            delete m_pRelativName;
    }

    AstType* getMemberType() const
        { return m_pMemberType; }

    virtual bool isUnsigned() const
    { return m_pMemberType != 0 && m_pMemberType->isUnsigned(); }

    virtual const sal_Char* getRelativName() const;
private:
    AstType*        m_pMemberType;
    mutable ::rtl::OString* m_pRelativName;
};

#endif // _IDLC_ASTSEQUENCE_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
