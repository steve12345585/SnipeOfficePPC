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
