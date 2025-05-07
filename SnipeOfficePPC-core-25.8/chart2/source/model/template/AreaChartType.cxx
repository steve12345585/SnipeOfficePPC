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

#include "AreaChartType.hxx"
#include "macros.hxx"
#include "servicenames_charttypes.hxx"

using namespace ::com::sun::star;

namespace chart
{

AreaChartType::AreaChartType(
    const uno::Reference< uno::XComponentContext > & xContext ) :
        ChartType( xContext )
{}

AreaChartType::AreaChartType( const AreaChartType & rOther ) :
        ChartType( rOther )
{}

AreaChartType::~AreaChartType()
{}

// ____ XCloneable ____
uno::Reference< util::XCloneable > SAL_CALL AreaChartType::createClone()
    throw (uno::RuntimeException)
{
    return uno::Reference< util::XCloneable >( new AreaChartType( *this ));
}

// ____ XChartType ____
::rtl::OUString SAL_CALL AreaChartType::getChartType()
    throw (uno::RuntimeException)
{
    return CHART2_SERVICE_NAME_CHARTTYPE_AREA;
}

uno::Sequence< ::rtl::OUString > AreaChartType::getSupportedServiceNames_Static()
{
    uno::Sequence< ::rtl::OUString > aServices( 2 );
    aServices[ 0 ] = CHART2_SERVICE_NAME_CHARTTYPE_AREA;
    aServices[ 1 ] = C2U( "com.sun.star.chart2.ChartType" );
    return aServices;
}

// implement XServiceInfo methods basing upon getSupportedServiceNames_Static
APPHELPER_XSERVICEINFO_IMPL( AreaChartType,
                             C2U( "com.sun.star.comp.chart.AreaChartType" ));

} //  namespace chart

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
