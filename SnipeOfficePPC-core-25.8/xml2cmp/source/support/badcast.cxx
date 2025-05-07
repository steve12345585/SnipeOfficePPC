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



/* Include this for instantiating bad-cast,
   due to problems with WNT-STL-headers.
*/

// If anybody who happens to read this has any idea what dark magic
// this is supposed to achieve, and whether it is needed or in any way
// relevant with MSVC2008 (or eventually newer), please add a
// comment...

#if defined(WNT) && !defined(__MINGW32__)

#define _NTSDK
#include<typeinfo>

_STD_BEGIN
bad_cast G_Dummy_Inst_bad_cast;
_STD_END
#endif // WNT



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
