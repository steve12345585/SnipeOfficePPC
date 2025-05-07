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


#include "comphelper/flagguard.hxx"
#include "osl/diagnose.h"
#include "com/sun/star/uno/Exception.hpp"

namespace comphelper {

ScopeGuard::~ScopeGuard()
{
    if (m_func)
    {
        if (m_excHandling == IGNORE_EXCEPTIONS)
        {
            try {
                m_func();
            }
            catch (com::sun::star::uno::Exception & exc) {
                (void) exc; // avoid warning about unused variable
                OSL_FAIL(
                    rtl::OUStringToOString(
                        rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                                           "UNO exception occurred: ") ) +
                        exc.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
            }
            catch (...) {
                OSL_FAIL( "unknown exception occurred!" );
            }
        }
        else
        {
            m_func();
        }
    }
}

void ScopeGuard::dismiss()
{
    m_func.clear();
}

FlagGuard::~FlagGuard()
{
}

FlagRestorationGuard::~FlagRestorationGuard()
{
}

} // namespace comphelper

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
