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

#ifndef LOGRECORD_HXX
#define LOGRECORD_HXX

#include <com/sun/star/logging/LogRecord.hpp>

#include <osl/interlck.h>

//........................................................................
namespace logging
{
//........................................................................

    //====================================================================
    //= helper
    //====================================================================
    ::com::sun::star::logging::LogRecord createLogRecord(
        const ::rtl::OUString& _rLoggerName,
        const ::rtl::OUString& _rClassName,
        const ::rtl::OUString& _rMethodName,
        const ::rtl::OUString& _rMessage,
        sal_Int32 _nLogLevel,
        oslInterlockedCount _nEventNumber
    );

    inline ::com::sun::star::logging::LogRecord createLogRecord(
        const ::rtl::OUString& _rLoggerName,
        const ::rtl::OUString& _rMessage,
        sal_Int32 _nLogLevel,
        oslInterlockedCount _nEventNumber
    )
    {
        return createLogRecord( _rLoggerName, ::rtl::OUString(), ::rtl::OUString(), _rMessage, _nLogLevel, _nEventNumber );
    }

//........................................................................
} // namespace logging
//........................................................................

#endif // LOGRECORD_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
