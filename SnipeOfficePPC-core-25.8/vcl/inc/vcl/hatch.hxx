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

#ifndef _SV_HATCH_HXX
#define _SV_HATCH_HXX

#include <tools/color.hxx>
#include <vcl/dllapi.h>

#include <vcl/vclenum.hxx>

// --------------
// - Impl_Hatch -
// --------------

class SvStream;

struct ImplHatch
{
    sal_uLong               mnRefCount;
    Color               maColor;
    HatchStyle          meStyle;
    long                mnDistance;
    sal_uInt16              mnAngle;

                        ImplHatch();
                        ImplHatch( const ImplHatch& rImplHatch );

    friend SvStream&    operator>>( SvStream& rIStm, ImplHatch& rImplHatch );
    friend SvStream&    operator<<( SvStream& rOStm, const ImplHatch& rImplHatch );
};

// ---------
// - Hatch -
// ---------

class VCL_DLLPUBLIC Hatch
{
private:

    ImplHatch*          mpImplHatch;
    SAL_DLLPRIVATE void ImplMakeUnique();

public:

                    Hatch();
                    Hatch( const Hatch& rHatch );
                    Hatch( HatchStyle eStyle, const Color& rHatchColor, long nDistance, sal_uInt16 nAngle10 = 0 );
                    ~Hatch();

    Hatch&          operator=( const Hatch& rHatch );
    sal_Bool            operator==( const Hatch& rHatch ) const;
    sal_Bool            operator!=( const Hatch& rHatch ) const { return !(Hatch::operator==( rHatch ) ); }
    sal_Bool            IsSameInstance( const Hatch& rHatch ) const { return( mpImplHatch == rHatch.mpImplHatch ); }

    HatchStyle      GetStyle() const { return mpImplHatch->meStyle; }

    void            SetColor( const Color& rColor  );
    const Color&    GetColor() const { return mpImplHatch->maColor; }

    void            SetDistance( long nDistance  );
    long            GetDistance() const { return mpImplHatch->mnDistance; }

    void            SetAngle( sal_uInt16 nAngle10 );
    sal_uInt16          GetAngle() const { return mpImplHatch->mnAngle; }

    friend VCL_DLLPUBLIC SvStream& operator>>( SvStream& rIStm, Hatch& rHatch );
    friend VCL_DLLPUBLIC SvStream& operator<<( SvStream& rOStm, const Hatch& rHatch );
};

#endif  // _SV_HATCH_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
