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
