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

#include "hsqldb/HTools.hxx"

//........................................................................
namespace connectivity { namespace hsqldb
{
//........................................................................

    /** === begin UNO using === **/
    /** === end UNO using === **/

    //====================================================================
    //= HTools
    //====================================================================
    //--------------------------------------------------------------------
    void HTools::appendTableFilterCrit( ::rtl::OUStringBuffer& _inout_rBuffer, const ::rtl::OUString& _rCatalog,
        const ::rtl::OUString _rSchema, const ::rtl::OUString _rName, bool _bShortForm )
    {
        _inout_rBuffer.appendAscii( " WHERE " );
        if ( !_rCatalog.isEmpty() )
        {
            _inout_rBuffer.appendAscii( _bShortForm ? "TABLE_CAT" : "TABLE_CATALOG" );
            _inout_rBuffer.appendAscii( " = '" );
            _inout_rBuffer.append     ( _rCatalog );
            _inout_rBuffer.appendAscii( "' AND " );
        }
        if ( !_rSchema.isEmpty() )
        {
            _inout_rBuffer.appendAscii( _bShortForm ? "TABLE_SCHEM" : "TABLE_SCHEMA" );
            _inout_rBuffer.appendAscii( " = '" );
            _inout_rBuffer.append     ( _rSchema );
            _inout_rBuffer.appendAscii( "' AND " );
        }
        _inout_rBuffer.appendAscii( "TABLE_NAME = '" );
        _inout_rBuffer.append     ( _rName );
        _inout_rBuffer.appendAscii( "'" );
    }

//........................................................................
} } // namespace connectivity::hsqldb
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
