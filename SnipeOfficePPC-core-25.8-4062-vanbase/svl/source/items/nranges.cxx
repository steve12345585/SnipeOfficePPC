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

#include <cassert>
#include <vector>
// compiled via include from itemset.cxx only!

//========================================================================

#ifdef DBG_UTIL

#define DBG_CHECK_RANGES(NUMTYPE, pArr)                                 \
    for ( const NUMTYPE *pRange = pArr; *pRange; pRange += 2 )          \
    {                                                                   \
        DBG_ASSERT( pRange[0] <= pRange[1], "ranges must be sorted" );  \
        DBG_ASSERT( !pRange[2] || ( pRange[2] - pRange[1] ) > 1,        \
                    "ranges must be sorted and discrete" );             \
    }

#else

#define DBG_CHECK_RANGES(NUMTYPE,pArr)

#endif

//============================================================================
inline void Swap_Impl(const NUMTYPE *& rp1, const NUMTYPE *& rp2)
{
    const NUMTYPE * pTemp = rp1;
    rp1 = rp2;
    rp2 = pTemp;
}

//========================================================================

NUMTYPE InitializeRanges_Impl( NUMTYPE *&rpRanges, va_list pArgs,
                               NUMTYPE nWh1, NUMTYPE nWh2, NUMTYPE nNull )

/** <H3>Description</H3>

    Creates an sal_uInt16-ranges-array in 'rpRanges' using 'nWh1' and 'nWh2' as
    first range, 'nNull' as terminator or start of 2nd range and 'pArgs' as
    remaider.

    It returns the number of NUMTYPEs which are contained in the described
    set of NUMTYPEs.
*/

{
    NUMTYPE nSize = 0, nIns = 0;
    std::vector<NUMTYPE> aNumArr;
    aNumArr.push_back( nWh1 );
    aNumArr.push_back( nWh2 );
    DBG_ASSERT( nWh1 <= nWh2, "Ungueltiger Bereich" );
    nSize += nWh2 - nWh1 + 1;
    aNumArr.push_back( nNull );
    bool bEndOfRange = false;
    while ( 0 !=
            ( nIns =
              sal::static_int_cast< NUMTYPE >(
                  va_arg( pArgs, NUMTYPE_ARG ) ) ) )
    {
        bEndOfRange = !bEndOfRange;
        if ( bEndOfRange )
        {
            const NUMTYPE nPrev(*aNumArr.rbegin());
            DBG_ASSERT( nPrev <= nIns, "Ungueltiger Bereich" );
            nSize += nIns - nPrev + 1;
        }
        aNumArr.push_back( nIns );
    }

    assert( bEndOfRange ); // odd number of Which-IDs

    // so, jetzt sind alle Bereiche vorhanden und
    rpRanges = new NUMTYPE[ aNumArr.size() + 1 ];
    std::copy( aNumArr.begin(), aNumArr.end(), rpRanges);
    *(rpRanges + aNumArr.size()) = 0;

    return nSize;
}

//------------------------------------------------------------------------

NUMTYPE Count_Impl( const NUMTYPE *pRanges )

/** <H3>Description</H3>

    Determines the number of NUMTYPEs in an 0-terminated array of pairs of
    NUMTYPEs. The terminating 0 is not included in the count.
*/

{
    NUMTYPE nCount = 0;
    while ( *pRanges )
    {
        nCount += 2;
        pRanges += 2;
    }
    return nCount;
}

//------------------------------------------------------------------------

NUMTYPE Capacity_Impl( const NUMTYPE *pRanges )

/** <H3>Description</H3>

    Determines the total number of NUMTYPEs described in an 0-terminated
    array of pairs of NUMTYPEs, each representing an range of NUMTYPEs.
*/

{
    NUMTYPE nCount = 0;

    if ( pRanges )
    {
        while ( *pRanges )
        {
            nCount += pRanges[1] - pRanges[0] + 1;
            pRanges += 2;
        }
    }
    return nCount;
}

//------------------------------------------------------------------------

SfxNumRanges::SfxNumRanges( const SfxNumRanges &rOrig )

/** <H3>Description</H3>

    Copy-Ctor.
*/

{
    if ( rOrig._pRanges )
    {
        NUMTYPE nCount = Count_Impl( rOrig._pRanges ) + 1;
        _pRanges = new NUMTYPE[nCount];
        memcpy( _pRanges, rOrig._pRanges, sizeof(NUMTYPE) * nCount );
    }
    else
        _pRanges = 0;
}

//------------------------------------------------------------------------

SfxNumRanges::SfxNumRanges( NUMTYPE nWhich1, NUMTYPE nWhich2 )

/** <H3>Description</H3>

    Constructs an SfxNumRanges-instance from one range of NUMTYPEs.

    precondition:
        nWhich1 <= nWhich2
*/

:   _pRanges( new NUMTYPE[3] )
{
    _pRanges[0] = nWhich1;
    _pRanges[1] = nWhich2;
    _pRanges[2] = 0;
}

//------------------------------------------------------------------------

SfxNumRanges::SfxNumRanges( const NUMTYPE* pArr )

/** <H3>Description</H3>

    Constcurts an SfxNumRanges-instance from an sorted ranges of NUMTYPEs,
    terminates with on 0.

    precondition: for each n >= 0 && n < (sizeof(pArr)-1)
        pArr[2n] <= pArr[2n+1] && ( pArr[2n+2]-pArr[2n+1] ) > 1
*/

{
    DBG_CHECK_RANGES(NUMTYPE, pArr);
    NUMTYPE nCount = Count_Impl(pArr) + 1;
    _pRanges = new NUMTYPE[ nCount ];
    memcpy( _pRanges, pArr, sizeof(NUMTYPE) * nCount );
}

//------------------------------------------------------------------------

sal_Bool SfxNumRanges::operator==( const SfxNumRanges &rOther ) const
{
    // Object pointers equal?
    if ( this == &rOther )
        return sal_True;

    // Ranges pointers equal?
    if ( _pRanges == rOther._pRanges )
        return sal_True;

    // Counts equal?
    NUMTYPE nCount = Count();
    if ( nCount != rOther.Count() )
        return sal_False;

    // Check arrays.
    NUMTYPE n = 0;
    while( _pRanges[ n ] != 0 )
    {
        // Elements at current position equal?
        if ( _pRanges[ n ] != rOther._pRanges[ n ] )
            return sal_False;

        ++n;
    }

    return sal_True;
}

//------------------------------------------------------------------------

SfxNumRanges& SfxNumRanges::operator =
(
    const SfxNumRanges &rRanges
)

/** <H3>Description</H3>

    Assigns ranges from 'rRanges' to '*this'.
*/

{
    // special case: assign itself
    if ( &rRanges == this )
        return *this;

    delete[] _pRanges;

    // special case: 'rRanges' is empty
    if ( rRanges.IsEmpty() )
        _pRanges = 0;
    else
    {
        // copy ranges
        NUMTYPE nCount = Count_Impl( rRanges._pRanges ) + 1;
        _pRanges = new NUMTYPE[ nCount ];
        memcpy( _pRanges, rRanges._pRanges, sizeof(NUMTYPE) * nCount );
    }
    return *this;
}

//------------------------------------------------------------------------

SfxNumRanges& SfxNumRanges::operator +=
(
    const SfxNumRanges &rRanges
)

/** <H3>Description</H3>

    Merges *this with 'rRanges'.

    for each NUMTYPE n:
        this->Contains( n ) || rRanges.Contains( n ) => this'->Contains( n )
        !this->Contains( n ) && !rRanges.Contains( n ) => !this'->Contains( n )
*/

{
    // special cases: one is empty
    if ( rRanges.IsEmpty() )
        return *this;
    if ( IsEmpty() )
        return *this = rRanges;

    // First, run thru _pRanges and rRanges._pRanges and determine the size of
    // the new, merged ranges:
    NUMTYPE nCount = 0;
    const NUMTYPE * pRA = _pRanges;
    const NUMTYPE * pRB = rRanges._pRanges;

    for (;;)
    {
        // The first pair of pRA has a lower lower bound than the first pair
        // of pRB:
        if (pRA[0] > pRB[0])
            Swap_Impl(pRA, pRB);

        // We are done with the merging if at least pRA is exhausted:
        if (!pRA[0])
            break;

        for (;;)
        {
            // Skip those pairs in pRB that completely lie in the first pair
            // of pRA:
            while (pRB[1] <= pRA[1])
            {
                pRB += 2;

                // Watch out for exhaustion of pRB:
                if (!pRB[0])
                {
                    Swap_Impl(pRA, pRB);
                    goto count_rest;
                }
            }

            // If the next pair of pRA does not at least touch the current new
            // pair, we are done with the current new pair:
            if (pRB[0] > pRA[1] + 1)
                break;

            // The next pair of pRB extends the current new pair; first,
            // extend the current new pair (we are done if pRB is then
            // exhausted); second, switch the roles of pRA and pRB in order to
            // merge in those following pairs of the original pRA that will
            // lie in the (now larger) current new pair or will even extend it
            // further:
            pRA += 2;
            if (!pRA[0])
                goto count_rest;
            Swap_Impl(pRA, pRB);
        }

        // Done with the current new pair:
        pRA += 2;
        nCount += 2;
    }

    // Only pRB has more pairs available, pRA is already exhausted:
count_rest:
    for (; pRB[0]; pRB += 2)
        nCount += 2;

    // Now, create new ranges of the correct size and, on a second run thru
    // _pRanges and rRanges._pRanges, copy the merged pairs into the new
    // ranges:
    NUMTYPE * pNew = new NUMTYPE[nCount + 1];
    pRA = _pRanges;
    pRB = rRanges._pRanges;
    NUMTYPE * pRN = pNew;

    for (;;)
    {
        // The first pair of pRA has a lower lower bound than the first pair
        // of pRB:
        if (pRA[0] > pRB[0])
            Swap_Impl(pRA, pRB);

        // We are done with the merging if at least pRA is exhausted:
        if (!pRA[0])
            break;

        // Lower bound of current new pair is already known:
        *pRN++ = pRA[0];

        for (;;)
        {
            // Skip those pairs in pRB that completely lie in the first pair
            // of pRA:
            while (pRB[1] <= pRA[1])
            {
                pRB += 2;

                // Watch out for exhaustion of pRB:
                if (!pRB[0])
                {
                    Swap_Impl(pRA, pRB);
                    ++pRB;
                    goto copy_rest;
                }
            }

            // If the next pair of pRA does not at least touch the current new
            // pair, we are done with the current new pair:
            if (pRB[0] > pRA[1] + 1)
                break;

            // The next pair of pRB extends the current new pair; first,
            // extend the current new pair (we are done if pRB is then
            // exhausted); second, switch the roles of pRA and pRB in order to
            // merge in those following pairs of the original pRA that will
            // lie in the (now larger) current new pair or will even extend it
            // further:
            pRA += 2;
            if (!pRA[0])
            {
                ++pRB;
                goto copy_rest;
            }
            Swap_Impl(pRA, pRB);
        }

        // Done with the current new pair, now upper bound is also known:
        *pRN++ = pRA[1];
        pRA += 2;
    }

    // Only pRB has more pairs available (which are copied to the new ranges
    // unchanged), pRA is already exhausted:
copy_rest:
    for (; *pRB;)
        *pRN++ = *pRB++;
    *pRN = 0;

    delete[] _pRanges;
    _pRanges = pNew;

    return *this;
}

//------------------------------------------------------------------------

SfxNumRanges& SfxNumRanges::operator -=
(
    const SfxNumRanges &rRanges
)

/** <H3>Description</H3>

    Removes 'rRanges' from '*this'.

    for each NUMTYPE n:
        this->Contains( n ) && rRanges.Contains( n ) => !this'->Contains( n )
        this->Contains( n ) && !rRanges.Contains( n ) => this'->Contains( n )
        !this->Contains( n ) => !this'->Contains( n )
*/

{
    // special cases: one is empty
    if ( rRanges.IsEmpty() || IsEmpty() )
        return *this;

    // differentiate 'rRanges' in a temporary copy of '*this'
    // (size is computed for maximal possibly split-count plus terminating 0)
    NUMTYPE nThisSize = Count_Impl(_pRanges);
    NUMTYPE nTargetSize = 1 + (  nThisSize + Count_Impl(rRanges._pRanges) );
    NUMTYPE *pTarget = new NUMTYPE[ nTargetSize ];
    memset( pTarget, 0, sizeof(NUMTYPE)*nTargetSize );
    memcpy( pTarget, _pRanges, sizeof(NUMTYPE)*nThisSize );

    NUMTYPE nPos1 = 0, nPos2 = 0, nTargetPos = 0;
    while( _pRanges[ nPos1 ] )
    {
        NUMTYPE l1 = _pRanges[ nPos1 ];      // lower bound of interval 1
        NUMTYPE u1 = _pRanges[ nPos1+1 ];    // upper bound of interval 1
        NUMTYPE l2 = rRanges._pRanges[ nPos2 ];      // lower bound of interval 2
        NUMTYPE u2 = rRanges._pRanges[ nPos2+1 ];    // upper bound of interval 2

        // boundary cases
        // * subtrahend is empty -> copy the minuend
        if( !l2 )
        {
            pTarget[ nTargetPos ] = l1;
            pTarget[ nTargetPos+1 ] = u1;
            nTargetPos += 2;
            nPos1 += 2;
            continue;
        }
        // * next subtrahend interval is completely higher -> copy the minuend
        if( u1 < l2 )
        {
            pTarget[ nTargetPos ] = l1;
            pTarget[ nTargetPos+1 ] = u1;
            nTargetPos += 2;
            nPos1 += 2;
            continue;
        }

        // * next subtrahend interval is completely lower -> try next
        if( u2 < l1 )
        {
            nPos2 += 2;
            continue;
        }

        // intersecting cases
        // * subtrahend cuts out from the beginning of the minuend
        if( l2 <= l1 && u2 <= u1 )
        {
            // reduce minuend interval, try again (minuend might be affected by other subtrahend intervals)
            _pRanges[ nPos1 ] = u2 + 1;
            nPos2 += 2; // this cannot hurt any longer
            continue;
        }

        // * subtrahend cuts out from the end of the minuend
        if( l1 <= l2 && u1 <= u2 )
        {
            // copy remaining part of minuend (cannot be affected by other intervals)
            if( l1 < l2 ) // anything left at all?
            {
                pTarget[ nTargetPos ] = l1;
                pTarget[ nTargetPos+1 ] = l2 - 1;
                nTargetPos += 2;
                // do not increment nPos2, might affect next minuend interval, too
            }
            nPos1 += 2; // nothing left at all
            continue;
        }

        // * subtrahend completely deletes minuend (larger or same at both ends)
        if( l1 >= l2 && u1 <= u2 )
        {
            nPos1 += 2; // minuend deleted
            // do not increment nPos2, might affect next minuend interval, too
            continue;
        }

        // * subtrahend divides minuend into two pieces
        if( l1 <= l2 && u1 >= u2 ) // >= and <= since they may be something left only at one side
        {
            // left side
            if( l1 < l2 ) // anything left at all
            {
                pTarget[ nTargetPos ] = l1;
                pTarget[ nTargetPos+1 ] = l2 - 1;
                nTargetPos += 2;
            }

            // right side
            if( u1 > u2 ) // anything left at all
            {
                // reduce minuend interval, try again (minuend might be affected by other subtrahend itnervals )
                _pRanges[ nPos1 ] = u2 + 1;
            }

            // subtrahend is completely used
            nPos2 += 2;
            continue;
        }

        // we should never be here
        OSL_FAIL( "SfxNumRanges::operator-=: internal error" );
    } // while

    pTarget[ nTargetPos ] = 0;

    // assign the differentiated ranges
    delete[] _pRanges;

    NUMTYPE nUShorts = Count_Impl(pTarget) + 1;
    if ( 1 != nUShorts )
    {
        _pRanges = new NUMTYPE[ nUShorts ];
        memcpy( _pRanges, pTarget, nUShorts * sizeof(NUMTYPE) );
    }
    else
        _pRanges = 0;

    delete [] pTarget;
    return *this;
}

//------------------------------------------------------------------------

SfxNumRanges& SfxNumRanges::operator /=
(
    const SfxNumRanges &rRanges
)

/** <H3>Description</H3>

    Determines intersection of '*this' with 'rRanges'.

    for each NUMTYPE n:
        this->Contains( n ) && rRanges.Contains( n ) => this'->Contains( n )
        !this->Contains( n ) => !this'->Contains( n )
        !rRanges.Contains( n ) => !this'->Contains( n )
*/

{
    // boundary cases
    // * first set is empty -> nothing to be done
    // * second set is empty -> delete first set
    if( rRanges.IsEmpty() )
    {
        delete[] _pRanges;

        _pRanges = new NUMTYPE[1];
        _pRanges[0] = 0;

        return *this;
    }

    // intersect 'rRanges' in a temporary copy of '*this'
    // (size is computed for maximal possibly split-count plus terminating 0)
    NUMTYPE nThisSize = Count_Impl(_pRanges);
    NUMTYPE nTargetSize = 1 + (  nThisSize + Count_Impl(rRanges._pRanges) );
    NUMTYPE *pTarget = new NUMTYPE[ nTargetSize ];
    memset( pTarget, 0, sizeof(NUMTYPE)*nTargetSize );
    memcpy( pTarget, _pRanges, sizeof(NUMTYPE)*nThisSize );

    NUMTYPE nPos1 = 0, nPos2 = 0, nTargetPos = 0;
    while( _pRanges[ nPos1 ] != 0 && rRanges._pRanges[ nPos2 ] != 0 )
    {
        NUMTYPE l1 = _pRanges[ nPos1 ];      // lower bound of interval 1
        NUMTYPE u1 = _pRanges[ nPos1+1 ];    // upper bound of interval 1
        NUMTYPE l2 = rRanges._pRanges[ nPos2 ];      // lower bound of interval 2
        NUMTYPE u2 = rRanges._pRanges[ nPos2+1 ];    // upper bound of interval 2

        if( u1 < l2 )
        {
            // current interval in s1 is completely before ci in s2
            nPos1 += 2;
            continue;
        }
        if( u2 < l1 )
        {
            // ci in s2 is completely before ci in s1
            nPos2 += 2;
            continue;
        }

        // assert: there exists an intersection between ci1 and ci2

        if( l1 <= l2 )
        {
            // c1 "is more to the left" than c2

            if( u1 <= u2 )
            {
                pTarget[ nTargetPos ] = l2;
                pTarget[ nTargetPos+1 ] = u1;
                nTargetPos += 2;
                nPos1 += 2;
                continue;
            }
            else
            {
                pTarget[ nTargetPos ] = l2;
                pTarget[ nTargetPos+1 ] = u2;
                nTargetPos += 2;
                nPos2 += 2;
            }
        }
        else
        {
            // c2 "is more to the left" than c1"

            if( u1 > u2 )
            {
                pTarget[ nTargetPos ] = l1;
                pTarget[ nTargetPos+1 ] = u2;
                nTargetPos += 2;
                nPos2 += 2;
            }
            else
            {
                pTarget[ nTargetPos ] = l1;
                pTarget[ nTargetPos+1 ] = u1;
                nTargetPos += 2;
                nPos1 += 2;
            }
        }
    }; // while
    pTarget[ nTargetPos ] = 0;

    // assign the intersected ranges
    delete[] _pRanges;

    NUMTYPE nUShorts = Count_Impl(pTarget) + 1;
    if ( 1 != nUShorts )
    {
        _pRanges = new NUMTYPE[ nUShorts ];
        memcpy( _pRanges, pTarget, nUShorts * sizeof(NUMTYPE) );
    }
    else
        _pRanges = 0;

    delete [] pTarget;
    return *this;
}

//------------------------------------------------------------------------

NUMTYPE SfxNumRanges::Count() const

/** <H3>Description</H3>

    Determines the number of USHORTs in the set described by the ranges
    of USHORTs in '*this'.
*/

{
    return Capacity_Impl( _pRanges );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
