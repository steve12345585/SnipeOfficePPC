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
#ifndef _CHART_POLARCOORDINATESYSTEM_HXX
#define _CHART_POLARCOORDINATESYSTEM_HXX

#include "ServiceMacros.hxx"
#include "BaseCoordinateSystem.hxx"

namespace chart
{

class PolarCoordinateSystem : public BaseCoordinateSystem
{
public:
    explicit PolarCoordinateSystem(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XComponentContext > & xContext,
        sal_Int32 nDimensionCount = 2,
        sal_Bool bSwapXAndYAxis = sal_False );
    explicit PolarCoordinateSystem( const PolarCoordinateSystem & rSource );
    virtual ~PolarCoordinateSystem();

    // ____ XCoordinateSystem ____
    virtual ::rtl::OUString SAL_CALL getCoordinateSystemType()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getViewServiceName()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XCloneable ____
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::util::XCloneable > SAL_CALL createClone()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XServiceInfo ____
    APPHELPER_XSERVICEINFO_DECL()
};

class PolarCoordinateSystem2d : public PolarCoordinateSystem
{
public:
    explicit PolarCoordinateSystem2d(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XComponentContext > & xContext );
    virtual ~PolarCoordinateSystem2d();

    /// establish methods for factory instatiation
    APPHELPER_SERVICE_FACTORY_HELPER( PolarCoordinateSystem2d )
    // ____ XServiceInfo ____
    APPHELPER_XSERVICEINFO_DECL()
};

class PolarCoordinateSystem3d : public PolarCoordinateSystem
{
public:
    explicit PolarCoordinateSystem3d(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XComponentContext > & xContext );
    virtual ~PolarCoordinateSystem3d();

    /// establish methods for factory instatiation
    APPHELPER_SERVICE_FACTORY_HELPER( PolarCoordinateSystem3d )
    // ____ XServiceInfo ____
    APPHELPER_XSERVICEINFO_DECL()
};

}  // namespace chart

// _CHART_POLARCOORDINATESYSTEM_HXX
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
