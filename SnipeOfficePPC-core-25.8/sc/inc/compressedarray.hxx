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

#ifndef SC_COMPRESSEDARRAY_HXX
#define SC_COMPRESSEDARRAY_HXX

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif
#include "scdllapi.h"

const size_t nScCompressedArrayDelta = 4;

template< typename A, typename D > class ScCompressedArrayIterator;

/** Compressed array of row (or column) entries, e.g. heights, flags, ...

    The array stores ranges of values such that consecutive values occupy only
    one entry. Initially it consists of one DataEntry with an implied start
    row/column of 0 and an end row/column of access type maximum value.

    typename A := access type, e.g. SCROW or SCCOL, must be a POD.

    typename D := data type, e.g. sal_uInt16 or sal_uInt8 or whatever, may also be a
    struct or class.

    D::operator==() and D::operator=() must be implemented. Force template
    instantiation for a specific type in source/core/data/compressedarray.cxx

    TODO: Currently the allocated memory never shrinks, must manually invoke
    Resize() if needed.
 */

template< typename A, typename D > class ScCompressedArray
{
public:
    struct DataEntry
    {
        A   nEnd;           // start is end of previous entry + 1
        D   aValue;
            DataEntry() {}  //! uninitialized
    };

    /** Construct with nMaxAccess=MAXROW, for example. */
                                ScCompressedArray( A nMaxAccess,
                                        const D& rValue,
                                        size_t nDelta = nScCompressedArrayDelta );
    /** Construct from a plain array of D */
                                ScCompressedArray( A nMaxAccess,
                                        const D* pDataArray, size_t nDataCount );
    virtual                     ~ScCompressedArray();
    void                        Resize( size_t nNewSize );
    void                        Reset( const D& rValue );
    void                        SetValue( A nPos, const D& rValue );
    void                        SetValue( A nStart, A nEnd, const D& rValue );
    const D&                    GetValue( A nPos ) const;

    /** Get value for a row, and it's region end row */
    const D&                    GetValue( A nPos, size_t& nIndex, A& nEnd ) const;

    /** Get next value and it's region end row. If nIndex<nCount, nIndex is
        incremented first. If the resulting nIndex>=nCount, the value of the
        last entry is returned again. */
    const D&                    GetNextValue( size_t& nIndex, A& nEnd ) const;

    /** Insert rows before nStart and copy value for inserted rows from
        nStart-1, return that value. */
    const D&                    Insert( A nStart, size_t nCount );

    void                        Remove( A nStart, size_t nCount );

    /** Copy rArray.nStart+nSourceDy to this.nStart */
    void                        CopyFrom( const ScCompressedArray& rArray,
                                    A nStart, A nEnd, long nSourceDy = 0 );


    // methods public for the coupled array sum methods
    /** Obtain index into entries for nPos */
    SC_DLLPUBLIC size_t                      Search( A nPos ) const;
    /** Get number of entries */
    size_t                      GetEntryCount() const;

protected:

friend class ScCompressedArrayIterator<A,D>;

    size_t                      nCount;
    size_t                      nLimit;
    size_t                      nDelta;
    DataEntry*                  pData;
    A                           nMaxAccess;
};


template< typename A, typename D >
void ScCompressedArray<A,D>::Reset( const D& rValue )
{
    // Create a temporary copy in case we got a reference passed that points to
    // a part of the array to be reallocated.
    D aTmpVal( rValue);
    delete[] pData;
    nCount = nLimit = 1;
    pData = new DataEntry[1];
    pData[0].aValue = aTmpVal;
    pData[0].nEnd = nMaxAccess;
}


template< typename A, typename D >
void ScCompressedArray<A,D>::SetValue( A nPos, const D& rValue )
{
    SetValue( nPos, nPos, rValue);
}


template< typename A, typename D >
const D& ScCompressedArray<A,D>::GetValue( A nPos ) const
{
    size_t nIndex = Search( nPos);
    return pData[nIndex].aValue;
}


template< typename A, typename D >
const D& ScCompressedArray<A,D>::GetValue( A nPos, size_t& nIndex, A& nEnd ) const
{
    nIndex = Search( nPos);
    nEnd = pData[nIndex].nEnd;
    return pData[nIndex].aValue;
}


template< typename A, typename D >
const D& ScCompressedArray<A,D>::GetNextValue( size_t& nIndex, A& nEnd ) const
{
    if (nIndex < nCount)
        ++nIndex;
    size_t nEntry = (nIndex < nCount ? nIndex : nCount-1);
    nEnd = pData[nEntry].nEnd;
    return pData[nEntry].aValue;
}


template< typename A, typename D >
size_t ScCompressedArray<A,D>::GetEntryCount() const
{
    return nCount;
}


// === ScCompressedArrayIterator =============================================

/** Iterator for ScCompressedArray.

    @ATTENTION: the iterator is not persistant if the underlying
    ScCompressedArray happens to be changed by any means, for example by
    setting new values or adding or removing or combining entries. If you do
    such things inside a loop you MUST resynchronize the iterator by calling
    <method>Resync()</method> with the row where resynchronization should
    start. After doing so, <method>GetRangeStart()</method> and
    <method>GetRangeEnd()</method> may not point to the previous access points
    anymore. Use with care.
 */

template< typename A, typename D > class ScCompressedArrayIterator
{
public:
                                ScCompressedArrayIterator(
                                        const ScCompressedArray<A,D> & rArray,
                                        A nStart, A nEnd );
    /// Set new start and end, position on start.
    void                        NewLimits( A nStart, A nEnd );
    A                           GetIterStart() const;
    A                           GetIterEnd() const;
    /// Advance by a single access point (e.g. row).
    bool                        operator ++();
    A                           GetPos() const;
                                operator bool() const;
    const D&                    operator *() const;
    /// Advance an entire range, one entry of the array.
    bool                        NextRange();
    A                           GetRangeStart() const;
    A                           GetRangeEnd() const;
    /// Resync to underlying array, calling Search().
    void                        Resync( A nPos );
    /** Set position without resyncing, avoid calling Search() if possible.
        Position obtained from steering coupled iterator is NOT checked for
        iterator bounds. */
    template< typename X >
    void                        Follow( const ScCompressedArrayIterator<A,X>& );

private:
    const ScCompressedArray<A,D> &  rArray;
    size_t                          nIndex;
    A                               nIterStart;
    A                               nIterEnd;
    A                               nCurrent;
    bool                            bEnd;
};


template< typename A, typename D >
ScCompressedArrayIterator<A,D>::ScCompressedArrayIterator(
        const ScCompressedArray<A,D> & rArrayP, A nStart, A nEnd )
    : rArray( rArrayP )
    // other values set in NewLimits()
{
    NewLimits( nStart, nEnd);
}


template< typename A, typename D >
void ScCompressedArrayIterator<A,D>::NewLimits( A nStart, A nEnd )
{
    nIterStart = nStart;
    nIterEnd = nEnd;
    nIndex = rArray.Search( nStart);
    nCurrent = GetRangeStart();
    bEnd = (nIterEnd < nIterStart);
}


template< typename A, typename D >
A ScCompressedArrayIterator<A,D>::GetIterStart() const
{
    return nIterStart;
}


template< typename A, typename D >
A ScCompressedArrayIterator<A,D>::GetIterEnd() const
{
    return nIterEnd;
}


template< typename A, typename D >
bool ScCompressedArrayIterator<A,D>::operator++()
{
    if (nCurrent < GetRangeEnd())
    {
        ++nCurrent;
        return true;
    }
    else
        return NextRange();
}


template< typename A, typename D >
A ScCompressedArrayIterator<A,D>::GetPos() const
{
    return nCurrent;
}


template< typename A, typename D >
bool ScCompressedArrayIterator<A,D>::NextRange()
{
    if (!operator bool())
        return false;

    if (rArray.pData[nIndex].nEnd >= nIterEnd)
        bEnd = true;
    else if (++nIndex >= rArray.GetEntryCount())
    {
        nIndex = rArray.GetEntryCount() - 1;
        bEnd = true;
    }
    nCurrent = bEnd ? nIterEnd : GetRangeStart();
    return operator bool();
}


template< typename A, typename D >
ScCompressedArrayIterator<A,D>::operator bool() const
{
    return !bEnd;
}


template< typename A, typename D >
const D& ScCompressedArrayIterator<A,D>::operator*() const
{
    return rArray.pData[nIndex].aValue;
}


template< typename A, typename D >
A ScCompressedArrayIterator<A,D>::GetRangeStart() const
{
    if (nIndex == 0)
        return nIterStart > 0 ? nIterStart : 0;
    else
        return nIterStart > rArray.pData[nIndex-1].nEnd ? nIterStart :
            rArray.pData[nIndex-1].nEnd + 1;
}


template< typename A, typename D >
A ScCompressedArrayIterator<A,D>::GetRangeEnd() const
{
    return nIterEnd < rArray.pData[nIndex].nEnd ? nIterEnd :
        rArray.pData[nIndex].nEnd;
}


template< typename A, typename D >
void ScCompressedArrayIterator<A,D>::Resync( A nPos )
{
    if (nPos < nIterStart)
        nPos = nIterStart;
    else if (nPos > nIterEnd)
        nPos = nIterEnd;
    nCurrent = nPos;
    bEnd = (nIterEnd < nIterStart);
    nIndex = rArray.Search( nPos);
}


// === ScBitMaskCompressedArray ==============================================

/** The data type represents bits, managable by bitwise operations.
 */

template< typename A, typename D > class ScBitMaskCompressedArray : public ScCompressedArray<A,D>
{
public:
                                ScBitMaskCompressedArray( A nMaxAccessP,
                                        const D& rValue,
                                        size_t nDeltaP = nScCompressedArrayDelta )
                                    : ScCompressedArray<A,D>( nMaxAccessP, rValue, nDeltaP)
                                    {}
                                ScBitMaskCompressedArray( A nMaxAccessP,
                                        const D* pDataArray, size_t nDataCount )
                                    : ScCompressedArray<A,D>( nMaxAccessP,
                                            pDataArray, nDataCount)
                                    {}
    void                        AndValue( A nPos, const D& rValueToAnd );
    void                        OrValue( A nPos, const D& rValueToOr );
    void                        AndValue( A nStart, A nEnd, const D& rValueToAnd );
    void                        OrValue( A nStart, A nEnd, const D& rValueToOr );

    /** Copy values from rArray and bitwise AND them with rValueToAnd. */
    void                        CopyFromAnded(
                                    const ScBitMaskCompressedArray& rArray,
                                    A nStart, A nEnd, const D& rValueToAnd,
                                    long nSourceDy = 0 );

    /** Return the first row where an entry meets the condition:
        ((aValue & rBitMask) == rMaskedCompare), searching between nStart and
        nEnd. If no entry meets this condition, ::std::numeric_limits<A>::max()
        is returned. */
    SC_DLLPUBLIC A                           GetFirstForCondition( A nStart, A nEnd,
                                    const D& rBitMask,
                                    const D& rMaskedCompare ) const;

    /** Return the last row where an entry meets the condition:
        ((aValue & rBitMask) != 0), start searching at nStart. If no entry
        meets this condition, ::std::numeric_limits<A>::max() is returned. */
    A                           GetLastAnyBitAccess( A nStart,
                                    const D& rBitMask ) const;
};


template< typename A, typename D >
void ScBitMaskCompressedArray<A,D>::AndValue( A nPos, const D& rValueToAnd )
{
    const D& rValue = this->GetValue( nPos);
    if ((rValue & rValueToAnd) != rValue)
        this->SetValue( nPos, rValue & rValueToAnd);
}


template< typename A, typename D >
void ScBitMaskCompressedArray<A,D>::OrValue( A nPos, const D& rValueToOr )
{
    const D& rValue = this->GetValue( nPos);
    if ((rValue | rValueToOr) != rValue)
        this->SetValue( nPos, rValue | rValueToOr);
}


#endif // SC_COMPRESSEDARRAY_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
