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


#include "WeakListenerAdapter.hxx"

using namespace ::com::sun::star;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;

namespace chart
{

WeakModifyListenerAdapter::WeakModifyListenerAdapter(
    const uno::WeakReference< util::XModifyListener > & xListener ) :
        WeakListenerAdapter< ::com::sun::star::util::XModifyListener >( xListener )
{}

WeakModifyListenerAdapter::~WeakModifyListenerAdapter()
{}

void SAL_CALL WeakModifyListenerAdapter::modified( const lang::EventObject& aEvent )
    throw (uno::RuntimeException)
{
    Reference< util::XModifyListener > xModListener( getListener() );
    if( xModListener.is())
        xModListener->modified( aEvent );
}

// --------------------------------------------------------------------------------

WeakSelectionChangeListenerAdapter::WeakSelectionChangeListenerAdapter(
    const Reference< view::XSelectionChangeListener > & xListener ) :
        WeakListenerAdapter< ::com::sun::star::view::XSelectionChangeListener >( xListener )
{}

WeakSelectionChangeListenerAdapter::~WeakSelectionChangeListenerAdapter()
{}

void SAL_CALL WeakSelectionChangeListenerAdapter::selectionChanged( const lang::EventObject& aEvent )
    throw (uno::RuntimeException)
{
    Reference< view::XSelectionChangeListener > xSelChgListener( getListener() );
    if( xSelChgListener.is())
        xSelChgListener->selectionChanged( aEvent );
}

} //  namespace chart

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
