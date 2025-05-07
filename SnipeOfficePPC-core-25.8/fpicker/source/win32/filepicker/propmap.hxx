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

#ifndef _PROPMAP_HXX_
#define _PROPMAP_HXX_

#include <boost/unordered_map.hpp>
#include <rtl/ustring.hxx>
#include <sal/types.h>

typedef ::boost::unordered_map< ::rtl::OUString                    ,
                         void*                              ,
                         ::rtl::OUStringHash                ,
                         ::std::equal_to< ::rtl::OUString > > TPropMapBase;

class TPropMap
{
    private:

        TPropMapBase m_aMap;

    public:

        template< class TValueType >
        void put(const ::rtl::OUString& sKey  ,
                 const TValueType&      rValue)
        {
            void* pValue = (void*)&rValue;
            m_aMap[sKey] = pValue;
        }

        template< class TValueType >
        void put_copy(const ::rtl::OUString& sKey  ,
                      const TValueType&      rValue)
        {
            TValueType* pCopy = new TValueType(rValue);
            m_aMap[sKey] = (void*)pCopy;
        }

        template< class TValueType >
        sal_Bool get(const ::rtl::OUString& sKey  ,
                           TValueType**     pValue)
        {
            TPropMapBase::iterator pIt = m_aMap.find(sKey);
            if (pIt == m_aMap.end())
                return sal_False;

            void*  pItem  = pIt->second;
                  *pValue = (TValueType*)pItem;
            return (pItem != 0);
        }

        template< class TValueType >
        sal_Bool get_copy(const ::rtl::OUString& sKey  ,
                                TValueType&      rValue)
        {
            TPropMapBase::iterator pIt = m_aMap.find(sKey);
            if (pIt == m_aMap.end())
                return sal_False;

            void* pValue = pIt->second;
            if ( ! pValue)
                return sal_False;

            rValue = *((TValueType*)pValue);
            //delete pValue;
            m_aMap.erase(pIt);
            return sal_True;
        }

        void clear()
        {
            m_aMap.clear();
        }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
