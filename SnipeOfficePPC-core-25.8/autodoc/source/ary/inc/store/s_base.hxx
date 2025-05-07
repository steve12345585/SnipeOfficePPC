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

#ifndef ARY_STORE_S_BASE_HXX
#define ARY_STORE_S_BASE_HXX

// USED SERVICES
#include <deque>
#include <cosv/tpl/tpltools.hxx>




namespace ary
{
namespace stg
{


/** The basic storage container of the repository.

    @collab Storage
    Implements Storage. Not used elsewhere.

    @tpl ENTITY
    The type of *it, where it is of type c_iter, has to be ENTITY * const.
*/
template <class ENTITY>
class Base
{
  public:
    // LIFECYCLE
    typedef std::deque< ENTITY* >               impl_type;
    typedef typename impl_type::const_iterator  c_iter;


    /** @param i_nrOfReservedItems
        The number of actual items to reserve, including the item
        at index [0] that is always empty and unused.
    */
                        Base(
                            uintt               i_nrOfReservedItems );
                        ~Base();

    // OPERATORS
    ENTITY *            operator[](
                            uintt               i_index ) const;
    // OPERATIONS
    uintt               Add_Entity(             /// @return the index of the new element.
                            DYN ENTITY &        pass_newEntity );
    DYN ENTITY *        Set_Entity(             /// @return the previous value.
                            uintt               i_index,
                            DYN ENTITY &        pass_newEntity );
    // INQUIRY
    uintt               Size() const;           /// Incl. reserved size.
    uintt               ReservedSize() const;   /// Incl. zero for element at [0].

    c_iter              Begin() const;          /// @return location of index 1, because 0 is always empty.
    c_iter              BeginUnreserved() const;
    c_iter              End() const;

  private:
    // DATA
    impl_type               aData;
    uintt                   nReservedSize;
};



// IMPLEMENTATION

template <class ENTITY>
Base<ENTITY>::Base(uintt i_nrOfReservedItems)
    :   aData(i_nrOfReservedItems, 0),
        nReservedSize(i_nrOfReservedItems)
{
}

template <class ENTITY>
Base<ENTITY>::~Base()
{
    csv::erase_container_of_heap_ptrs(aData);
}


template <class ENTITY>
ENTITY *
Base<ENTITY>::operator[](uintt i_index) const
{
    if (i_index < aData.size())
        return aData[i_index];
    return 0;
}

template <class ENTITY>
uintt
Base<ENTITY>::Add_Entity(DYN ENTITY & pass_newEntity)
{
    aData.push_back(&pass_newEntity);
    return aData.size() - 1;
}

template <class ENTITY>
DYN ENTITY *
Base<ENTITY>::Set_Entity( uintt         i_index,
                          DYN ENTITY &  pass_newEntity )
{
    csv_assert(i_index != 0 AND i_index < aData.size());

    Dyn<ENTITY>
        ret(aData[i_index]);
    aData[i_index] = &pass_newEntity;
    return ret.Release();
}

template <class ENTITY>
uintt
Base<ENTITY>::Size() const
{
    return aData.size();
}

template <class ENTITY>
uintt
Base<ENTITY>::ReservedSize() const
{
    return nReservedSize;
}

template <class ENTITY>
typename Base<ENTITY>::c_iter
Base<ENTITY>::Begin() const
{
    return aData.begin() + 1;
}

template <class ENTITY>
typename Base<ENTITY>::c_iter
Base<ENTITY>::BeginUnreserved() const
{
    return aData.begin() + nReservedSize;
}

template <class ENTITY>
typename Base<ENTITY>::c_iter
Base<ENTITY>::End() const
{
    return aData.end();
}




}   // namespace stg
}   // namespace ary
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
