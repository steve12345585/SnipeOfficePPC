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
#ifndef _FRACT_HXX
#define _FRACT_HXX

#include "tools/toolsdllapi.h"
#include <tools/solar.h>

class SvStream;
// ------------
// - Fraction -
// ------------

class TOOLS_DLLPUBLIC Fraction
{
private:
    long            nNumerator;
    long            nDenominator;

public:
                    Fraction() { nNumerator = 0; nDenominator = 1; }
                    Fraction( const Fraction & rFrac );
                    Fraction( long nNum, long nDen=1 );
                    Fraction( double dVal );

    bool            IsValid() const;

    long            GetNumerator() const { return nNumerator; }
    long            GetDenominator() const { return nDenominator; }

    operator        long() const;
    operator        double() const;

    Fraction&       operator=( const Fraction& rfrFrac );

    Fraction&       operator+=( const Fraction& rfrFrac );
    Fraction&       operator-=( const Fraction& rfrFrac );
    Fraction&       operator*=( const Fraction& rfrFrac );
    Fraction&       operator/=( const Fraction& rfrFrac );

    void            ReduceInaccurate( unsigned nSignificantBits );

    friend inline   Fraction operator+( const Fraction& rVal1, const Fraction& rVal2 );
    friend inline   Fraction operator-( const Fraction& rVal1, const Fraction& rVal2 );
    friend inline   Fraction operator*( const Fraction& rVal1, const Fraction& rVal2 );
    friend inline   Fraction operator/( const Fraction& rVal1, const Fraction& rVal2 );

    TOOLS_DLLPUBLIC friend          bool operator==( const Fraction& rVal1, const Fraction& rVal2 );
    friend inline   bool operator!=( const Fraction& rVal1, const Fraction& rVal2 );
    TOOLS_DLLPUBLIC friend          bool operator< ( const Fraction& rVal1, const Fraction& rVal2 );
    TOOLS_DLLPUBLIC friend          bool operator> ( const Fraction& rVal1, const Fraction& rVal2 );
    friend inline   bool operator<=( const Fraction& rVal1, const Fraction& rVal2 );
    friend inline   bool operator>=( const Fraction& rVal1, const Fraction& rVal2 );

    TOOLS_DLLPUBLIC friend SvStream& operator>>( SvStream& rIStream, Fraction& rFract );
    TOOLS_DLLPUBLIC friend SvStream& operator<<( SvStream& rOStream, const Fraction& rFract );
};

inline Fraction::Fraction( const Fraction& rFrac )
{
    nNumerator   = rFrac.nNumerator;
    nDenominator = rFrac.nDenominator;
}

inline Fraction& Fraction::operator=( const Fraction& rFrac )
{
    nNumerator   = rFrac.nNumerator;
    nDenominator = rFrac.nDenominator;
    return *this;
}

inline bool Fraction::IsValid() const
{
    return (nDenominator > 0);
}

inline Fraction::operator long() const
{
    if ( nDenominator > 0 )
        return (nNumerator / nDenominator);
    else
        return 0;
}

inline Fraction operator+( const Fraction& rVal1, const Fraction& rVal2 )
{
    Fraction aErg( rVal1 );
    aErg += rVal2;
    return aErg;
}

inline Fraction operator-( const Fraction& rVal1, const Fraction& rVal2 )
{
    Fraction aErg( rVal1 );
    aErg -= rVal2;
    return aErg;
}

inline Fraction operator*( const Fraction& rVal1, const Fraction& rVal2 )
{
    Fraction aErg( rVal1 );
    aErg *= rVal2;
    return aErg;
}

inline Fraction operator/( const Fraction& rVal1, const Fraction& rVal2 )
{
    Fraction aErg( rVal1 );
    aErg /= rVal2;
    return aErg;
}

inline bool operator !=( const Fraction& rVal1, const Fraction& rVal2 )
{
    return !(rVal1 == rVal2);
}

inline bool operator <=( const Fraction& rVal1, const Fraction& rVal2 )
{
    return !(rVal1 > rVal2);
}

inline bool operator >=( const Fraction& rVal1, const Fraction& rVal2 )
{
    return !(rVal1 < rVal2);
}

#endif // _FRACT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
