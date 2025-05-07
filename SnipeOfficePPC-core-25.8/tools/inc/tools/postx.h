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

#ifndef _POSTX_H
#define _POSTX_H

#if defined __cplusplus
}
#endif

/* X-Types */
#undef Window
#undef BYTE
#undef INT8
#undef INT64
#undef BOOL
#undef Font
#undef Cursor
#undef String
#undef KeyCode
#undef Region
#undef Icon
#undef Time
#undef Boolean

#undef Min
#undef Max
#undef DestroyAll
#undef Success

#undef Printer
/* #undef FontInfo */
#undef Orientation

#undef GetToken
#undef ReleaseToken
#undef InitializeToken
#undef NextRequest

#ifdef KeyPress
#if KeyPress != 2
Error KeyPress must be Equal 2
#endif
#undef KeyPress
#endif
#define XLIB_KeyPress 2

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
