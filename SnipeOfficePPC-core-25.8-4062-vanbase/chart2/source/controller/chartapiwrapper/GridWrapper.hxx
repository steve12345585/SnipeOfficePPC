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
#ifndef CHART_GRIDWRAPPER_HXX
#define CHART_GRIDWRAPPER_HXX

#include "WrappedPropertySet.hxx"
#include "ServiceMacros.hxx"
#include <cppuhelper/implbase2.hxx>
#include <comphelper/uno3.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/chart2/XDiagram.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <boost/shared_ptr.hpp>

namespace chart
{
namespace wrapper
{

class Chart2ModelContact;

class GridWrapper : public ::cppu::ImplInheritanceHelper2<
                      WrappedPropertySet
                    , com::sun::star::lang::XComponent
                    , com::sun::star::lang::XServiceInfo
                    >
{
public:
    enum tGridType
    {
        X_MAJOR_GRID,
        Y_MAJOR_GRID,
        Z_MAJOR_GRID,
        X_MINOR_GRID,
        Y_MINOR_GRID,
        Z_MINOR_GRID
    };

    GridWrapper( tGridType eType, ::boost::shared_ptr< Chart2ModelContact > spChart2ModelContact );
    virtual ~GridWrapper();

    static void getDimensionAndSubGridBool( tGridType eType, sal_Int32& rnDimensionIndex, bool& rbSubGrid );

    /// XServiceInfo declarations
    APPHELPER_XSERVICEINFO_DECL()

    // ____ XComponent ____
    virtual void SAL_CALL dispose()
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference<
                                            ::com::sun::star::lang::XEventListener >& xListener )
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference<
                                               ::com::sun::star::lang::XEventListener >& aListener )
        throw (::com::sun::star::uno::RuntimeException);

protected:
    // ____ WrappedPropertySet ____
    virtual const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& getPropertySequence();
    virtual const std::vector< WrappedProperty* > createWrappedProperties();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > getInnerPropertySet();

private:
    ::boost::shared_ptr< Chart2ModelContact >   m_spChart2ModelContact;
    ::cppu::OInterfaceContainerHelper           m_aEventListenerContainer;

    tGridType           m_eType;
};

} //  namespace wrapper
} //  namespace chart

// CHART_GRIDWRAPPER_HXX
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
