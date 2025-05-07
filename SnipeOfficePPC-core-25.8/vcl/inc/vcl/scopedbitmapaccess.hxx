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

#ifndef VCL_SCOPEDBITMAPACCESS_HXX_INCLUDED
#define VCL_SCOPEDBITMAPACCESS_HXX_INCLUDED

namespace vcl
{

/** This template handles BitmapAccess the RAII way.

    Please don't use directly, but through the ready-made typedefs
    ScopedReadAccess and ScopedWriteAccess in classes Bitmap and
    AlphaMask.

    Use as follows:
    Bitmap aBitmap;
    Bitmap::ScopedReadAccess pReadAccess( aBitmap );
    pReadAccess->SetPixel()...

    Bitmap aBitmap2;
    Bitmap::ScopedWriteAccess pWriteAccess( bCond ? aBitmap2.AcquireWriteAccess() : 0, aBitmap2 );
    if ( pWriteAccess )...

    @attention for practical reasons, ScopedBitmapAccess stores a
    reference to the provided bitmap, thus, make sure that the bitmap
    specified at construction time lives at least as long as the
    ScopedBitmapAccess.
 */
template < class Access, class Bitmap, Access* (Bitmap::* Acquire)() > class ScopedBitmapAccess
{
    typedef ScopedBitmapAccess< Access, Bitmap, Acquire > self_type;
    typedef bool (self_type::* unspecified_bool_type)() const;

public:
    explicit ScopedBitmapAccess( Bitmap& rBitmap ) :
        mpAccess( 0 ),
        mrBitmap( rBitmap )
    {
        mpAccess = (mrBitmap.*Acquire)();
    }

    ScopedBitmapAccess( Access* pAccess, Bitmap& rBitmap ) :
        mpAccess( pAccess ),
        mrBitmap( rBitmap )
    {
    }

    ~ScopedBitmapAccess()
    {
        mrBitmap.ReleaseAccess( mpAccess );
    }

    bool operator!() const { return !mpAccess; }
    operator unspecified_bool_type() const
    {
        return mpAccess ? &self_type::operator! : 0;
    }

    Access*         get() { return mpAccess; }
    const Access*   get() const { return mpAccess; }

    Access*         operator->() { return mpAccess; }
    const Access*   operator->() const { return mpAccess; }

    Access&         operator*() { return *mpAccess; }
    const Access&   operator*() const { return *mpAccess; }

private:
    Access*     mpAccess;
    Bitmap&     mrBitmap;
};

}

#endif // VCL_SCOPEDBITMAPACCESS_HXX_INCLUDED

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
