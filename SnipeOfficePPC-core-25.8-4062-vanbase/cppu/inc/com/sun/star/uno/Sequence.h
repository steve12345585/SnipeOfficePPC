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
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#define _COM_SUN_STAR_UNO_SEQUENCE_H_

#include "typelib/typedescription.h"
#include "uno/sequence2.h"
#include "com/sun/star/uno/Type.h"
#include "rtl/alloc.h"

#if ! defined EXCEPTIONS_OFF
#include <new>
#endif


namespace rtl
{
class ByteSequence;
}

namespace com
{
namespace sun
{
namespace star
{
namespace uno
{

/** Template C++ class representing an IDL sequence. Template argument is the
    sequence element type.  C++ Sequences are reference counted and shared,
    so the sequence keeps a handle to its data.  To keep value semantics,
    copies are only generated if the sequence is to be modified (new handle).

    @tparam E element type of sequence
*/
template< class E >
class Sequence
{
    /** sequence handle
    */
    uno_Sequence * _pSequence;

public:
    /// @cond INTERNAL

    // these are here to force memory de/allocation to sal lib.
    inline static void * SAL_CALL operator new ( ::size_t nSize )
        SAL_THROW(())
        { return ::rtl_allocateMemory( nSize ); }
    inline static void SAL_CALL operator delete ( void * pMem )
        SAL_THROW(())
        { ::rtl_freeMemory( pMem ); }
    inline static void * SAL_CALL operator new ( ::size_t, void * pMem )
        SAL_THROW(())
        { return pMem; }
    inline static void SAL_CALL operator delete ( void *, void * )
        SAL_THROW(())
        {}

    /** Static pointer to typelib type of sequence.
        Don't use directly, call getCppuType().
    */
    static typelib_TypeDescriptionReference * s_pType;

    /// @endcond

    /** typedefs the element type of the sequence
    */
    typedef E ElementType;

    /** Default constructor: Creates an empty sequence.
    */
    inline Sequence() SAL_THROW(());

    /** Copy constructor: Creates a copy of given sequence.

        @param rSeq another sequence of same type
    */
    inline Sequence( const Sequence< E > & rSeq ) SAL_THROW(());

    /** Constructor: Takes over ownership of given sequence.

        @param pSequence a sequence
        @param dummy SAL_NO_ACQUIRE to force obvious distinction to other
        constructors
    */
    inline Sequence( uno_Sequence * pSequence, __sal_NoAcquire dummy )
        SAL_THROW(());

    /** Constructor: Creates a copy of given elements.

        @param pElements an array of elements
        @param len length of array
    */
    inline Sequence( const E * pElements, sal_Int32 len );

    /** Constructor: Creates a default constructed sequence of given length.

        @param len initial sequence length
    */
    inline explicit Sequence( sal_Int32 len );

    /** Destructor: Releases sequence handle. Last handle will destruct
        elements and free memory.
    */
    inline ~Sequence() SAL_THROW(());

    /** Assignment operator: Acquires given sequence handle and releases
        previously set handle.

        @param rSeq another sequence of same type
        @return this sequence
    */
    inline Sequence< E > & SAL_CALL operator = ( const Sequence< E > & rSeq )
        SAL_THROW(());

    /** Gets length of the sequence.

        @return length of sequence
    */
    inline sal_Int32 SAL_CALL getLength() const SAL_THROW(())
        { return _pSequence->nElements; }

    /** Tests whether the sequence has elements, i.e. elements count is
        greater than zero.

        @return true, if elements count is greater than zero
    */
    inline sal_Bool SAL_CALL hasElements() const SAL_THROW(())
        { return (_pSequence->nElements > 0); }

    /** Gets a pointer to elements array for reading.
        If the sequence has a length of 0, then the returned pointer is
        undefined.

        @return pointer to elements array
    */
    inline const E * SAL_CALL getConstArray() const SAL_THROW(())
        { return reinterpret_cast< const E * >( _pSequence->elements ); }

    /** Gets a pointer to elements array for reading and writing.
        In general if the sequence has a handle acquired by other sequences
        (reference count > 1), then a new sequence is created copy constructing
        all elements to keep value semantics!
        If the sequence has a length of 0, then the returned pointer is
        undefined.

        @return pointer to elements array
    */
    inline E * SAL_CALL getArray();

    /** Non-const index operator: Obtains a reference to element indexed at
        given position.
        The implementation does not check for array bounds!
        In general if the sequence has a handle acquired by other sequences
        (reference count > 1), then a new sequence is created copy constructing
        all elements to keep value semantics!

        @param nIndex index
        @return non-const C++ reference to element
    */
    inline E & SAL_CALL operator [] ( sal_Int32 nIndex );

    /** Const index operator: Obtains a reference to element indexed at
        given position.  The implementation does not check for array bounds!

        @param nIndex index
        @return const C++ reference to element
    */
    inline const E & SAL_CALL operator [] ( sal_Int32 nIndex ) const
        SAL_THROW(());

    /** Equality operator: Compares two sequences.

        @param rSeq another sequence of same type (right side)
        @return true if both sequences are equal, false otherwise
    */
    inline sal_Bool SAL_CALL operator == ( const Sequence< E > & rSeq ) const
        SAL_THROW(());

    /** Unequality operator: Compares two sequences.

        @param rSeq another sequence of same type (right side)
        @return false if both sequences are equal, true otherwise
    */
    inline sal_Bool SAL_CALL operator != ( const Sequence< E > & rSeq ) const
        SAL_THROW(());

    /** Reallocates sequence to new length.
        If the new length is smaller than the former, then upper elements will
        be destructed (and their memory freed).  If the new length is greater
        than the former, then upper (new) elements are default constructed.
        If the sequence has a handle acquired by other sequences
        (reference count > 1), then the remaining elements are copy constructed
        to a new sequence handle to keep value semantics!

        @param nSize new size of sequence
    */
    inline void SAL_CALL realloc( sal_Int32 nSize );

    /** Provides UNacquired sequence handle.

        @return UNacquired sequence handle
    */
    inline uno_Sequence * SAL_CALL get() const SAL_THROW(())
        { return _pSequence; }
};

/** Creates a UNO byte sequence from a SAL byte sequence.

    @param rByteSequence a byte sequence
    @return a UNO byte sequence
*/
inline ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL toUnoSequence(
    const ::rtl::ByteSequence & rByteSequence ) SAL_THROW(());

}
}
}
}

/** Gets the meta type of IDL sequence.

    There are cases (involving templates) where uses of getCppuType are known to
    not compile.  Use cppu::UnoType or cppu::getTypeFavourUnsigned instead.

    The dummy parameter is just a typed pointer for function signature.

    @tparam E element type of sequence
    @return type of IDL sequence
*/
template< class E >
inline const ::com::sun::star::uno::Type &
SAL_CALL getCppuType( const ::com::sun::star::uno::Sequence< E > * )
    SAL_THROW(());

/** Gets the meta type of IDL sequence.
    This function has been introduced, because one cannot get the (templated)
    cppu type out of C++ array types.  Array types have special
    getCppuArrayTypeN() functions.

    @attention
    the given element type must be the same as the template argument type!
    @tparam E element type of sequence
    @param rElementType element type of sequence
    @return type of IDL sequence
*/
template< class E >
inline const ::com::sun::star::uno::Type &
SAL_CALL getCppuSequenceType( const ::com::sun::star::uno::Type & rElementType )
    SAL_THROW(());

/** Gets the meta type of IDL sequence< char >.
    This function has been introduced due to ambiguities with unsigned short.

    The dummy parameter is just a typed pointer for function signature.

    @return type of IDL sequence< char >
*/
inline const ::com::sun::star::uno::Type &
SAL_CALL getCharSequenceCppuType() SAL_THROW(());

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
