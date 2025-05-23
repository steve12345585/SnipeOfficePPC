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

#ifndef _DBAUI_COMMON_TYPES_HXX_
#define _DBAUI_COMMON_TYPES_HXX_

#include <comphelper/stl_types.hxx>
#include <unotools/sharedunocomponent.hxx>

namespace com { namespace sun { namespace star {
    namespace sdbc {
        class XConnection;
    }
} } }

//.........................................................................
namespace dbaui
{
//.........................................................................

    DECLARE_STL_STDKEY_SET( ::rtl::OUString, StringBag );
    DECLARE_STL_VECTOR( sal_Int8, ByteVector );
    DECLARE_STL_VECTOR( ::rtl::OUString, StringArray );

    typedef ::utl::SharedUNOComponent< ::com::sun::star::sdbc::XConnection > SharedConnection;

//.........................................................................
}   // namespace dbaui
//.........................................................................

#endif // _DBAUI_COMMON_TYPES_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
