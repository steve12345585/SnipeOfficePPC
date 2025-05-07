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

#include "trace.hxx"
#include <tools/debug.hxx>
#include <rtl/strbuf.hxx>

#if defined(DBG_UTIL)

//==============================================================================

//------------------------------------------------------------------------------
::osl::Mutex Tracer::s_aMapSafety;
::std::map< ::oslThreadIdentifier, sal_Int32, ::std::less< oslThreadIdentifier > >
        Tracer::s_aThreadIndents;

//------------------------------------------------------------------------------
Tracer::Tracer(const char* _pBlockDescription)
    :m_sBlockDescription(_pBlockDescription)
{
    ::osl::MutexGuard aGuard(s_aMapSafety);
    sal_uInt32 nIndent = s_aThreadIndents[ ::osl::Thread::getCurrentIdentifier() ]++;

    rtl::OStringBuffer sMessage;
    sMessage.append(static_cast<sal_Int32>(
        ::osl::Thread::getCurrentIdentifier()));
    sMessage.append('\t');
    while (nIndent--)
        sMessage.append('\t');
    sMessage.append(m_sBlockDescription);
    sMessage.append(RTL_CONSTASCII_STRINGPARAM(" {"));
    OSL_TRACE(sMessage.getStr());
}

//------------------------------------------------------------------------------
Tracer::~Tracer()
{
    ::osl::MutexGuard aGuard(s_aMapSafety);
    sal_Int32 nIndent = --s_aThreadIndents[ ::osl::Thread::getCurrentIdentifier() ];

    rtl::OStringBuffer sMessage;
    sMessage.append(static_cast<sal_Int32>(
        ::osl::Thread::getCurrentIdentifier()));
    sMessage.append('\t');
    while (nIndent--)
        sMessage.append('\t');
    sMessage.append(RTL_CONSTASCII_STRINGPARAM("} // "));
    sMessage.append(m_sBlockDescription);
    OSL_TRACE(sMessage.getStr());
}

//------------------------------------------------------------------------------
void Tracer::TraceString(const char* _pMessage)
{
    ::osl::MutexGuard aGuard(s_aMapSafety);
    sal_Int32 nIndent = s_aThreadIndents[ ::osl::Thread::getCurrentIdentifier() ];

    rtl::OStringBuffer sMessage;
    sMessage.append(static_cast<sal_Int32>(
        ::osl::Thread::getCurrentIdentifier()));
    sMessage.append('\t');
    while (nIndent--)
        sMessage.append('\t');
    sMessage.append(m_sBlockDescription);
    sMessage.append(RTL_CONSTASCII_STRINGPARAM(": "));
    sMessage.append(_pMessage);
    OSL_TRACE(sMessage.getStr());
}

//------------------------------------------------------------------------------
void Tracer::TraceString1StringParam(const char* _pMessage, const char* _pParam)
{
    ::osl::MutexGuard aGuard(s_aMapSafety);
    sal_Int32 nIndent = s_aThreadIndents[ ::osl::Thread::getCurrentIdentifier() ];

    rtl::OStringBuffer sMessage;
    sMessage.append(static_cast<sal_Int32>(
        ::osl::Thread::getCurrentIdentifier()));
    sMessage.append('\t');
    while (nIndent--)
        sMessage.append('\t');
    sMessage.append(m_sBlockDescription);
    sMessage.append(RTL_CONSTASCII_STRINGPARAM(": "));
    sMessage.append(_pMessage);
    OSL_TRACE(sMessage.getStr(), _pParam);
}
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
