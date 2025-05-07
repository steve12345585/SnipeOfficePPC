/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
*
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* Copyright 2000, 2011 Oracle and/or its affiliates.
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

#ifndef INCLUDED_CPPUNITTESTER_PROTECTORFACTORY_HXX
#define INCLUDED_CPPUNITTESTER_PROTECTORFACTORY_HXX

#include "sal/config.h"
#include "sal/precppunit.hxx"

#include "sal/types.h"

#include <cppunit/Protector.h>

namespace cppunittester
{
    // The type of CppUnit::Protector factory functions that can be plugged into
    // cppunittester:
    extern "C" typedef CppUnit::Protector * SAL_CALL ProtectorFactory();
}

#ifdef DISABLE_DYNLOADING
extern "C" CppUnit::Protector *unoexceptionprotector();
extern "C" CppUnit::Protector *unobootstrapprotector();
#endif

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
