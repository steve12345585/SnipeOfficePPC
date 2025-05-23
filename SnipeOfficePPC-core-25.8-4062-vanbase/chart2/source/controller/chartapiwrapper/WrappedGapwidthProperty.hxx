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
#ifndef CHART_WRAPPED_GAPWIDTH_PROPERTY_HXX
#define CHART_WRAPPED_GAPWIDTH_PROPERTY_HXX

#include "WrappedDefaultProperty.hxx"
#include "Chart2ModelContact.hxx"

#include <boost/shared_ptr.hpp>

namespace chart
{
namespace wrapper
{

class WrappedBarPositionProperty_Base : public WrappedDefaultProperty
{
public:
    WrappedBarPositionProperty_Base(
              const ::rtl::OUString& rOuterName
            , const ::rtl::OUString& rInnerSequencePropertyName
            , sal_Int32 nDefaultValue
            , ::boost::shared_ptr< Chart2ModelContact > spChart2ModelContact );
    virtual ~WrappedBarPositionProperty_Base();

    virtual void setPropertyValue( const ::com::sun::star::uno::Any& rOuterValue, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xInnerPropertySet ) const
                        throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::uno::Any getPropertyValue( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xInnerPropertySet ) const
                        throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    void setDimensionAndAxisIndex( sal_Int32 nDimensionIndex, sal_Int32 nAxisIndex );

protected:
    sal_Int32 m_nDimensionIndex;
    sal_Int32 m_nAxisIndex;
    ::boost::shared_ptr< Chart2ModelContact >   m_spChart2ModelContact;

    sal_Int32                                   m_nDefaultValue;
    ::rtl::OUString                             m_InnerSequencePropertyName;

    mutable ::com::sun::star::uno::Any          m_aOuterValue;
};

class WrappedGapwidthProperty : public WrappedBarPositionProperty_Base
{
public:
    WrappedGapwidthProperty( ::boost::shared_ptr< Chart2ModelContact > spChart2ModelContact );
    virtual ~WrappedGapwidthProperty();
};

class WrappedBarOverlapProperty : public WrappedBarPositionProperty_Base
{
public:
    WrappedBarOverlapProperty( ::boost::shared_ptr< Chart2ModelContact > spChart2ModelContact );
    virtual ~WrappedBarOverlapProperty();
};

} //  namespace wrapper
} //  namespace chart

// CHART_WRAPPED_GAPWIDTH_PROPERTY_HXX
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
