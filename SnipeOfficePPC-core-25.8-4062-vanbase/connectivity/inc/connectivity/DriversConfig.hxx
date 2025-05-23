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
#ifndef CONNECTIVITY_DRIVERSCONFIG_HXX_INCLUDED
#define CONNECTIVITY_DRIVERSCONFIG_HXX_INCLUDED

#include <comphelper/stl_types.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/uno/Sequence.h>
#include "connectivity/dbtoolsdllapi.hxx"
#include <salhelper/singletonref.hxx>
#include <comphelper/namedvaluecollection.hxx>
#include <unotools/confignode.hxx>

namespace connectivity
{
    typedef struct
    {
        ::comphelper::NamedValueCollection aProperties;
        ::comphelper::NamedValueCollection aFeatures;
        ::comphelper::NamedValueCollection aMetaData;
        ::rtl::OUString sDriverFactory;
        ::rtl::OUString sDriverTypeDisplayName;
    } TInstalledDriver;
    DECLARE_STL_USTRINGACCESS_MAP( TInstalledDriver, TInstalledDrivers);

    class DriversConfigImpl
    {
        mutable ::utl::OConfigurationTreeRoot   m_aInstalled;
        mutable TInstalledDrivers       m_aDrivers;
        void Load(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB) const;
    public:
        DriversConfigImpl();

        const TInstalledDrivers& getInstalledDrivers(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB) const { Load(_rxORB); return m_aDrivers; }
    };
    //
    // Allows to access all driver which are located in the configuration
    //
    class OOO_DLLPUBLIC_DBTOOLS DriversConfig
    {
        typedef salhelper::SingletonRef<DriversConfigImpl> OSharedConfigNode;

        const ::comphelper::NamedValueCollection& impl_get(const ::rtl::OUString& _sURL,sal_Int32 _nProps) const;
    public:
        DriversConfig(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB);
        ~DriversConfig();

        DriversConfig( const DriversConfig& );
        DriversConfig& operator=( const DriversConfig& );

        ::rtl::OUString getDriverFactoryName(const ::rtl::OUString& _sUrl) const;
        ::rtl::OUString getDriverTypeDisplayName(const ::rtl::OUString& _sUrl) const;
        const ::comphelper::NamedValueCollection& getProperties(const ::rtl::OUString& _sURL) const;
        const ::comphelper::NamedValueCollection& getFeatures(const ::rtl::OUString& _sURL) const;
        const ::comphelper::NamedValueCollection& getMetaData(const ::rtl::OUString& _sURL) const;
        ::com::sun::star::uno::Sequence< ::rtl::OUString > getURLs() const;
    private:
        OSharedConfigNode   m_aNode;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xORB;
    };
}
#endif // CONNECTIVITY_DRIVERSCONFIG_HXX_INCLUDED

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
