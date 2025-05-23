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

#ifndef _COMPHELPER_PROPERTY_MULTIPLEX_HXX_
#define _COMPHELPER_PROPERTY_MULTIPLEX_HXX_

#include <comphelper/propstate.hxx>
#include <cppuhelper/implbase1.hxx>
#include "comphelper/comphelperdllapi.h"

//=========================================================================
//= property helper classes
//=========================================================================

//.........................................................................
namespace comphelper
{
//.........................................................................

    class OPropertyChangeMultiplexer;

    //==================================================================
    //= OPropertyChangeListener
    //==================================================================
    /// simple listener adapter for property sets
    class COMPHELPER_DLLPUBLIC OPropertyChangeListener
    {
        friend class OPropertyChangeMultiplexer;

        OPropertyChangeMultiplexer* m_pAdapter;
        ::osl::Mutex&               m_rMutex;

    public:
        OPropertyChangeListener(::osl::Mutex& _rMutex)
            : m_pAdapter(NULL), m_rMutex(_rMutex) { }
        virtual ~OPropertyChangeListener();

        virtual void _propertyChanged(const ::com::sun::star::beans::PropertyChangeEvent& _rEvent) throw( ::com::sun::star::uno::RuntimeException) = 0;
        virtual void _disposing(const ::com::sun::star::lang::EventObject& _rSource) throw( ::com::sun::star::uno::RuntimeException);

    protected:
        /** If the derivee also owns the mutex which we know as reference, then call this within your
            derivee's dtor.
        */
        void    disposeAdapter();

        // pseudo-private. Making it private now could break compatibility
        void    setAdapter( OPropertyChangeMultiplexer* _pAdapter );
    };

    //==================================================================
    //= OPropertyChangeMultiplexer
    //==================================================================
    /// multiplexer for property changes
    class COMPHELPER_DLLPUBLIC OPropertyChangeMultiplexer   :public cppu::WeakImplHelper1< ::com::sun::star::beans::XPropertyChangeListener>
    {
        friend class OPropertyChangeListener;
         ::com::sun::star::uno::Sequence< ::rtl::OUString >     m_aProperties;
         ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>   m_xSet;
        OPropertyChangeListener*                    m_pListener;
        sal_Int32                                   m_nLockCount;
        sal_Bool                                    m_bListening        : 1;
        sal_Bool                                    m_bAutoSetRelease   : 1;


        virtual ~OPropertyChangeMultiplexer();
    public:
        OPropertyChangeMultiplexer(OPropertyChangeListener* _pListener, const  ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxSet, sal_Bool _bAutoReleaseSet = sal_True);

    // XEventListener
        virtual void SAL_CALL disposing( const  ::com::sun::star::lang::EventObject& Source ) throw( ::com::sun::star::uno::RuntimeException);

    // XPropertyChangeListener
        virtual void SAL_CALL propertyChange( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw( ::com::sun::star::uno::RuntimeException);

        /// incremental lock
        void        lock();
        /// incremental unlock
        void        unlock();
        /// get the lock count
        sal_Int32   locked() const { return m_nLockCount; }

        void addProperty(const ::rtl::OUString& aPropertyName);
        void dispose();
    };

//.........................................................................
}   // namespace comphelper
//.........................................................................

#endif // _COMPHELPER_PROPERTY_MULTIPLEX_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
