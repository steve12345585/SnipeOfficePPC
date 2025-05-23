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
#ifndef CONNECTIVITY_TSORTINDEX_HXX
#define CONNECTIVITY_TSORTINDEX_HXX

#include "connectivity/dbtoolsdllapi.hxx"
#include "TKeyValue.hxx"

namespace connectivity
{
    typedef enum
    {
        SQL_ORDERBYKEY_NONE,        // do not sort
        SQL_ORDERBYKEY_DOUBLE,      // numeric key
        SQL_ORDERBYKEY_STRING       // String Key
    } OKeyType;

    typedef enum
    {
        SQL_ASC     = 1,            // ascending
        SQL_DESC    = -1            // otherwise
    } TAscendingOrder;

    class OKeySet;
    class OKeyValue;                // simple class which holds a sal_Int32 and a ::std::vector<ORowSetValueDecoratorRef>

    /**
        The class OSortIndex can be used to implement a sorted index.
        This can depend on the fields which should be sorted.
    */
    class OOO_DLLPUBLIC_DBTOOLS OSortIndex
    {
    public:
        typedef ::std::vector< ::std::pair<sal_Int32,OKeyValue*> >  TIntValuePairVector;
        typedef ::std::vector<OKeyType>                             TKeyTypeVector;

    private:
        TIntValuePairVector             m_aKeyValues;
        TKeyTypeVector                  m_aKeyType;
        ::std::vector<TAscendingOrder>  m_aAscending;
        sal_Bool                        m_bFrozen;

    public:

        OSortIndex( const ::std::vector<OKeyType>& _aKeyType,
                    const ::std::vector<TAscendingOrder>& _aAscending);

        ~OSortIndex();

        inline static void * SAL_CALL operator new( size_t nSize ) SAL_THROW(())
            { return ::rtl_allocateMemory( nSize ); }
        inline static void * SAL_CALL operator new( size_t,void* _pHint ) SAL_THROW(())
            { return _pHint; }
        inline static void SAL_CALL operator delete( void * pMem ) SAL_THROW(())
            { ::rtl_freeMemory( pMem ); }
        inline static void SAL_CALL operator delete( void *,void* ) SAL_THROW(())
            {  }


        /**
            AddKeyValue appends a new value.
            @param
                pKeyValue   the keyvalue to be appended
            ATTENTION: when the sortindex is already frozen the parameter will be deleted
        */
        void AddKeyValue(OKeyValue * pKeyValue);

        /**
            Freeze freezes the sortindex so that new values could only be appended by their value
        */
        void Freeze();

        /**
            CreateKeySet creates the keyset which vaalues could be used to travel in your table/result
            The returned keyset is frozen.
        */
        ::rtl::Reference<OKeySet> CreateKeySet();



        // look at the name
        sal_Bool IsFrozen() const { return m_bFrozen; }
        // returns the current size of the keyvalues
        size_t Count()   const { return m_aKeyValues.size(); }

        inline const ::std::vector<OKeyType>& getKeyType() const { return m_aKeyType; }
        inline TAscendingOrder getAscending(::std::vector<TAscendingOrder>::size_type _nPos) const { return m_aAscending[_nPos]; }

    };

    /**
        The class OKeySet is a refcountable vector which also has a state.
        This state gives information about if the keyset is fixed.
    */
    class OOO_DLLPUBLIC_DBTOOLS OKeySet : public ORefVector<sal_Int32>
    {
        sal_Bool m_bFrozen;
    public:
        OKeySet()
            : ORefVector<sal_Int32>()
            , m_bFrozen(sal_False){}
        OKeySet(Vector::size_type _nSize)
            : ORefVector<sal_Int32>(_nSize)
            , m_bFrozen(sal_False){}

        sal_Bool    isFrozen() const                        { return m_bFrozen; }
        void        setFrozen(sal_Bool _bFrozen=sal_True)   { m_bFrozen = _bFrozen; }
    };
}
#endif // CONNECTIVITY_TSORTINDEX_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
