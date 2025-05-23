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

#include "DataSeriesProperties.hxx"
#include "DataPointProperties.hxx"
#include "DataPoint.hxx"
#include "macros.hxx"
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/chart2/StackingDirection.hpp>

#include <algorithm>

using namespace ::com::sun::star;

using ::rtl::OUString;
using ::com::sun::star::beans::Property;
using ::com::sun::star::uno::Reference;

namespace chart
{

void DataSeriesProperties::AddPropertiesToVector(
    ::std::vector< Property > & rOutProperties )
{
    rOutProperties.push_back(
        Property( C2U( "AttributedDataPoints" ),
                  PROP_DATASERIES_ATTRIBUTED_DATA_POINTS,
                  ::getCppuType( reinterpret_cast< const uno::Sequence< sal_Int32 > * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEVOID ));

    rOutProperties.push_back(
        Property( C2U( "StackingDirection" ),
                  PROP_DATASERIES_STACKING_DIRECTION,
                  ::getCppuType( reinterpret_cast< const chart2::StackingDirection * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    rOutProperties.push_back(
        Property( C2U( "VaryColorsByPoint" ),
                  PROP_DATASERIES_VARY_COLORS_BY_POINT,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    rOutProperties.push_back(
        Property( C2U( "AttachedAxisIndex" ),
                  PROP_DATASERIES_ATTACHED_AXIS_INDEX,
                  ::getCppuType( reinterpret_cast< const sal_Int32 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEVOID
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    // add properties of service DataPointProperties
    DataPointProperties::AddPropertiesToVector( rOutProperties );
}

void DataSeriesProperties::AddDefaultsToMap(
    tPropertyValueMap & rOutMap )
{
    PropertyHelper::setPropertyValueDefault( rOutMap, PROP_DATASERIES_STACKING_DIRECTION, chart2::StackingDirection_NO_STACKING );
    PropertyHelper::setPropertyValueDefault( rOutMap, PROP_DATASERIES_VARY_COLORS_BY_POINT, false );
    PropertyHelper::setPropertyValueDefault< sal_Int32 >( rOutMap, PROP_DATASERIES_ATTACHED_AXIS_INDEX, 0 );

    // PROP_DATASERIES_ATTRIBUTED_DATA_POINTS has no default

    // add properties of service DataPointProperties
     DataPointProperties::AddDefaultsToMap( rOutMap );
}

}  // namespace chart

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
