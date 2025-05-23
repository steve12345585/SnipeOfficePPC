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
#ifndef _IDLC_ASTSERVICEMEMBER_HXX_
#define _IDLC_ASTSERVICEMEMBER_HXX_

#include <idlc/astservice.hxx>

class AstServiceMember : public AstDeclaration
{
public:
    AstServiceMember(const sal_uInt32 flags, AstService* pRealService,
                     const ::rtl::OString& name, AstScope* pScope)
        : AstDeclaration(NT_service_member, name, pScope)
        , m_flags(flags)
        , m_pRealService(pRealService)
        {}
    virtual ~AstServiceMember() {}

    AstService* getRealService()
        { return m_pRealService; }
    sal_Bool isOptional()
        { return ((m_flags & AF_OPTIONAL) == AF_OPTIONAL); }
private:
    const sal_uInt32    m_flags;
    AstService*         m_pRealService;
};

#endif // _IDLC_ASTSERVICEMEMBER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
