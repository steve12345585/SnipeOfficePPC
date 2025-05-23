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

#ifndef COMPHELPER_FLAGGUARD_HXX
#define COMPHELPER_FLAGGUARD_HXX

#include "comphelper/scopeguard.hxx"

//......................................................................................................................
namespace comphelper
{
//......................................................................................................................

    //==================================================================================================================
    //= FlagRestorationGuard
    //==================================================================================================================
    class COMPHELPER_DLLPUBLIC FlagRestorationGuard : public ScopeGuard
    {
    public:
        FlagRestorationGuard( bool& i_flagRef, bool i_temporaryValue, exc_handling i_excHandling = IGNORE_EXCEPTIONS )
            :ScopeGuard( ::boost::bind( RestoreFlag, ::boost::ref( i_flagRef ), !!i_flagRef ), i_excHandling )
        {
            i_flagRef = i_temporaryValue;
        }

        ~FlagRestorationGuard();

    private:
        static void RestoreFlag( bool& i_flagRef, bool i_originalValue )
        {
            i_flagRef = i_originalValue;
        }
    };

    //==================================================================================================================
    //= FlagGuard
    //==================================================================================================================
    class COMPHELPER_DLLPUBLIC FlagGuard : public ScopeGuard
    {
    public:
        explicit FlagGuard( bool& i_flagRef, exc_handling i_excHandling = IGNORE_EXCEPTIONS )
            :ScopeGuard( ::boost::bind( ResetFlag, ::boost::ref( i_flagRef ) ), i_excHandling )
        {
            i_flagRef = true;
        }

        ~FlagGuard();

    private:
        static void ResetFlag( bool& i_flagRef )
        {
            i_flagRef = false;
        }
    };

//......................................................................................................................
} // namespace comphelper
//......................................................................................................................

#endif // COMPHELPER_FLAGGUARD_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
