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

#include "salhelper/simplereferenceobject.hxx"
#include "osl/diagnose.h"

#ifndef INCLUDED_NEW
#include <new>
#define INCLUDED_NEW
#endif

using salhelper::SimpleReferenceObject;

SimpleReferenceObject::~SimpleReferenceObject() SAL_THROW(())
{
    OSL_ASSERT(m_nCount == 0);
}

void * SimpleReferenceObject::operator new(std::size_t nSize)
    SAL_THROW((std::bad_alloc))
{
    return ::operator new(nSize);
}

void * SimpleReferenceObject::operator new(std::size_t nSize,
                                           std::nothrow_t const &)
    SAL_THROW(())
{
#if defined WNT
    return ::operator new(nSize);
        // WNT lacks a global nothrow operator new...
#else // WNT
    return ::operator new(nSize, std::nothrow);
#endif // WNT
}

void SimpleReferenceObject::operator delete(void * pPtr) SAL_THROW(())
{
    ::operator delete(pPtr);
}

void SimpleReferenceObject::operator delete(void * pPtr, std::nothrow_t const &)
    SAL_THROW(())
{
#if defined WNT
    ::operator delete(pPtr); // WNT lacks a global nothrow operator delete...
#else // WNT
    ::operator delete(pPtr, std::nothrow);
#endif // WNT
}

#ifdef _MSC_VER

/* This operator is supposed to be unimplemented, but that now leads
 * to compilation and/or linking errors with MSVC2008. (Don't know
 * about MSVC2010.) As it can be left unimplemented just fine with
 * gcc, presumably it is never called. So do implement it then to
 * avoid the compilation and/or linking errors, but make it crash
 * intentionally if called.
 */
void SimpleReferenceObject::operator delete[](void * /* pPtr */)
{
    free(NULL);
}
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
