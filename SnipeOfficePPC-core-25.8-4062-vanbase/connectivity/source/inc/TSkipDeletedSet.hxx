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
#ifndef CONNECTIVITY_SKIPDELETEDSSET_HXX
#define CONNECTIVITY_SKIPDELETEDSSET_HXX

#include "TResultSetHelper.hxx"
#include <rtl/alloc.h>
#include <boost/unordered_map.hpp>
#include <vector>
#include "connectivity/dbtoolsdllapi.hxx"

namespace connectivity
{
    /**
        the class OSkipDeletedSet supports a general method to skip deleted rows
    */
    class OOO_DLLPUBLIC_DBTOOLS OSkipDeletedSet
    {
        ::std::vector<sal_Int32>                m_aBookmarksPositions;// vector of iterators to position map, the order is the logical position
        IResultSetHelper*                       m_pHelper;            // used for moving in the resultset
        bool                                    m_bDeletedVisible;

        sal_Bool    moveAbsolute(sal_Int32 _nOffset,sal_Bool _bRetrieveData);
    public:
        OSkipDeletedSet(IResultSetHelper* _pHelper);
        ~OSkipDeletedSet();

        inline static void * SAL_CALL operator new( size_t nSize ) SAL_THROW(())
            { return ::rtl_allocateMemory( nSize ); }
        inline static void * SAL_CALL operator new( size_t,void* _pHint ) SAL_THROW(())
            { return _pHint; }
        inline static void SAL_CALL operator delete( void * pMem ) SAL_THROW(())
            { ::rtl_freeMemory( pMem ); }
        inline static void SAL_CALL operator delete( void *,void* ) SAL_THROW(())
            {  }

        /**
            skipDeleted moves the resultset to the position defined by the parameters
            it garantees that the row isn't deleted
                @param
                    IResultSetHelper::Movement  _eCursorPosition        in which direction the resultset should be moved
                    sal_Int32                   _nOffset                the position relativ to the movement
                    sal_Bool                    _bRetrieveData          is true when the current row should be filled which data
                @return
                    true when the movement was successful otherwise false
        */
        sal_Bool    skipDeleted(IResultSetHelper::Movement _eCursorPosition, sal_Int32 _nOffset, sal_Bool _bRetrieveData);
        /**
            clear the map and the vector used in this class
        */
        void        clear();
        /**
            getMappedPosition returns the mapped position of a logical position
            @param
                sal_Int32 _nBookmark    the logical position

            @return the mapped position
        */
        sal_Int32   getMappedPosition(sal_Int32 _nBookmark) const;
        /**
            insertNewPosition adds a new position to the map
            @param
                sal_Int32 _nPos the logical position
        */
        void        insertNewPosition(sal_Int32 _nPos);
        /**
            deletePosition deletes this position from the map and decrement all following positions
            @param
                sal_Int32 _nPos the logical position
        */
        void        deletePosition(sal_Int32 _nPos);
        /**
            getLastPosition returns the last position
            @return the last position
        */
        inline sal_Int32    getLastPosition() const { return m_aBookmarksPositions.size(); }
        inline void SetDeletedVisible(bool _bDeletedVisible) { m_bDeletedVisible = _bDeletedVisible; }
    };
}
#endif // CONNECTIVITY_SKIPDELETEDSSET_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
