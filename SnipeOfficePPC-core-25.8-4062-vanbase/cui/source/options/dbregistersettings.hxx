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

#ifndef SVX_DBREGISTERSETTING_HXX
#define SVX_DBREGISTERSETTING_HXX

#include <comphelper/stl_types.hxx>
#include <svl/poolitem.hxx>

//........................................................................
namespace svx
{
//........................................................................

    struct DatabaseRegistration
    {
        ::rtl::OUString sLocation;
        bool            bReadOnly;

        DatabaseRegistration()
            :sLocation()
            ,bReadOnly( true )
        {
        }

        DatabaseRegistration( const ::rtl::OUString& _rLocation, const sal_Bool _bReadOnly )
            :sLocation( _rLocation )
            ,bReadOnly( _bReadOnly )
        {
        }

        bool operator==( const DatabaseRegistration& _rhs ) const
        {
            return  ( sLocation == _rhs.sLocation );
                // do not take the read-only-ness into account, this is not maintained everywhere, but only
                // properly set when filling the struct from the XDatabaseRegistrations data
        }

        bool operator!=( const DatabaseRegistration& _rhs ) const
        {
            return !( this->operator==( _rhs ) );
        }
    };

    typedef ::std::map< ::rtl::OUString, DatabaseRegistration, ::comphelper::UStringLess >   DatabaseRegistrations;

    //====================================================================
    //= DatabaseMapItem
    //====================================================================
    class DatabaseMapItem : public SfxPoolItem
    {
    protected:
        DatabaseRegistrations   m_aRegistrations;

    public:
        TYPEINFO();

        DatabaseMapItem( sal_uInt16 _nId, const DatabaseRegistrations& _rRegistrations );

        virtual int              operator==( const SfxPoolItem& ) const;
        virtual SfxPoolItem*     Clone( SfxItemPool *pPool = 0 ) const;

        const DatabaseRegistrations&
                                getRegistrations() const { return m_aRegistrations; }
    };

//........................................................................
}   // namespace svx
//........................................................................
#endif // SVX_DBREGISTERSETTING_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
