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
#ifndef CHART_DATASERIESPROPERTIES_HXX
#define CHART_DATASERIESPROPERTIES_HXX

#include "PropertyHelper.hxx"
#include "FastPropertyIdRanges.hxx"
#include <com/sun/star/beans/Property.hpp>

#include <vector>

namespace chart
{

class DataSeriesProperties
{
public:
    enum
    {
        PROP_DATASERIES_ATTRIBUTED_DATA_POINTS = FAST_PROPERTY_ID_START_DATA_SERIES,
        PROP_DATASERIES_STACKING_DIRECTION,
        PROP_DATASERIES_VARY_COLORS_BY_POINT,
        PROP_DATASERIES_ATTACHED_AXIS_INDEX
    };

    static void AddPropertiesToVector(
        ::std::vector< ::com::sun::star::beans::Property > & rOutProperties );

    static void AddDefaultsToMap( tPropertyValueMap & rOutMap );

private:
    // not implemented
    DataSeriesProperties();
};

} //  namespace chart

// CHART_DATASERIESPROPERTIES_HXX
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
