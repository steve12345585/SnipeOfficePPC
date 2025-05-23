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
#ifndef CHART2_DISPOSEHELPER_HXX
#define CHART2_DISPOSEHELPER_HXX

#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/lang/XComponent.hpp>

#include <functional>
#include <algorithm>
#include <utility>

namespace chart
{
namespace DisposeHelper
{

template< class T >
    void Dispose( const T & xIntf )
{
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > xComp(
        xIntf, ::com::sun::star::uno::UNO_QUERY );
    if( xComp.is())
        xComp->dispose();
}

template< class Intf >
    void DisposeAndClear( ::com::sun::star::uno::Reference< Intf > & rIntf )
{
    Dispose< ::com::sun::star::uno::Reference< Intf > >( rIntf );
    rIntf.set( 0 );
}

template< class T >
    struct DisposeFunctor : public ::std::unary_function< T, void >
{
    void operator() ( const T & xIntf )
    {
        Dispose< T >( xIntf );
    }
};

template< typename T >
    struct DisposeFirstOfPairFunctor : public ::std::unary_function< T, void >
{
    void operator() ( const T & rElem )
    {
        Dispose< typename T::first_type >( rElem.first );
    }
};

template< typename T >
    struct DisposeSecondOfPairFunctor : public ::std::unary_function< T, void >
{
    void operator() ( const T & rElem )
    {
        Dispose< typename T::second_type >( rElem.second );
    }
};

template< class Container >
    void DisposeAllElements( Container & rContainer )
{
    ::std::for_each( rContainer.begin(), rContainer.end(), DisposeFunctor< typename Container::value_type >());
}

template< class Map >
    void DisposeAllMapElements( Map & rContainer )
{
    ::std::for_each( rContainer.begin(), rContainer.end(), DisposeSecondOfPairFunctor< typename Map::value_type >());
}

} //  namespace DisposeHelper
} //  namespace chart

// CHART2_DISPOSEHELPER_HXX
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
