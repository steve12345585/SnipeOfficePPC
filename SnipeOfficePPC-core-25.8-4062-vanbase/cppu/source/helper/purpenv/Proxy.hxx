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

#ifndef INCLUDED_Proxy_hxx
#define INCLUDED_Proxy_hxx

#include "osl/interlck.h"

#include "uno/environment.hxx"
#include "uno/mapping.hxx"
#include "uno/dispatcher.h"

#include "cppu/helper/purpenv/Mapping.hxx"


namespace cssu = com::sun::star::uno;


class SAL_DLLPRIVATE Proxy : public uno_Interface
{
    oslInterlockedCount                 m_nRef;

    cssu::Environment                   m_from;
    cssu::Environment                   m_to;

    cssu::Mapping                       m_from_to;
    cssu::Mapping                       m_to_from;

    // mapping information
    uno_Interface                    *  m_pUnoI; // wrapped interface
    typelib_InterfaceTypeDescription *  m_pTypeDescr;
    rtl::OUString                       m_aOId;

    cppu::helper::purpenv::ProbeFun   * m_probeFun;
    void                              * m_pProbeContext;

public:
    explicit Proxy(cssu::Mapping                    const & to_from,
                   uno_Environment                        * pTo,
                   uno_Environment                        * pFrom,
                   uno_Interface                          * pUnoI,
                   typelib_InterfaceTypeDescription       * pTypeDescr,
                   rtl::OUString                    const & rOId,
                   cppu::helper::purpenv::ProbeFun        * probeFun,
                   void                                   * pProbeContext)
        SAL_THROW(());
    ~Proxy(void);

    void acquire(void);
    void release(void);

    void dispatch(
        typelib_TypeDescriptionReference  * pReturnTypeRef,
        typelib_MethodParameter           * pParams,
        sal_Int32                           nParams,
        typelib_TypeDescription     const * pMemberType,
        void                              * pReturn,
        void                              * pArgs[],
        uno_Any                          ** ppException );

};

extern "C" SAL_DLLPRIVATE void SAL_CALL Proxy_free(uno_ExtEnvironment * pEnv, void * pProxy) SAL_THROW_EXTERN_C();

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
