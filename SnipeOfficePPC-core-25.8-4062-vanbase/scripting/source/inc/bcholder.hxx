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

#ifndef SCRIPTING_BCHOLDER_HXX
#define SCRIPTING_BCHOLDER_HXX

#include <osl/mutex.hxx>
#include <cppuhelper/interfacecontainer.h>


//.........................................................................
namespace scripting_helper
{
//.........................................................................

    //  ----------------------------------------------------
    //  class OMutexHolder
    //  ----------------------------------------------------

    class OMutexHolder
    {
    protected:
        ::osl::Mutex m_aMutex;
    };

    //  ----------------------------------------------------
    //  class OBroadcastHelperHolder
    //  ----------------------------------------------------

    class OBroadcastHelperHolder
    {
    protected:
        ::cppu::OBroadcastHelper    m_aBHelper;

    public:
        OBroadcastHelperHolder( ::osl::Mutex& rMutex ) : m_aBHelper( rMutex ) { }

        ::cppu::OBroadcastHelper&       GetBroadcastHelper()        { return m_aBHelper; }
        const ::cppu::OBroadcastHelper& GetBroadcastHelper() const  { return m_aBHelper; }
    };

//.........................................................................
}   // namespace scripting_helper
//.........................................................................

#endif // SCRIPTING_BCHOLDER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
