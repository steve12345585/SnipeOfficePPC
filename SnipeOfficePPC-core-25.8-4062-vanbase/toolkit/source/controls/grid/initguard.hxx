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

#ifndef SVTOOLS_INITGUARD_HXX
#define SVTOOLS_INITGUARD_HXX

#include <com/sun/star/lang/NotInitializedException.hpp>

#include <comphelper/componentguard.hxx>

//......................................................................................................................
namespace toolkit
{
//......................................................................................................................

    //==================================================================================================================
    //= InitGuard
    //==================================================================================================================
    template < class IMPL >
    class InitGuard : public ::comphelper::ComponentGuard
    {
    public:
        InitGuard( IMPL& i_component, ::cppu::OBroadcastHelper & i_broadcastHelper )
            :comphelper::ComponentGuard( i_component, i_broadcastHelper )
        {
            if ( !i_component.isInitialized() )
                throw ::com::sun::star::lang::NotInitializedException( ::rtl::OUString(), *&i_component );
        }

        ~InitGuard()
        {
        }
    };

//......................................................................................................................
} // namespace toolkit
//......................................................................................................................

#endif // SVTOOLS_INITGUARD_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
