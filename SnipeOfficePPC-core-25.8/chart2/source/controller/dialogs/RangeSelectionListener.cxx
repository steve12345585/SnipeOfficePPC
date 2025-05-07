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


#include "RangeSelectionListener.hxx"

using namespace ::com::sun::star;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;

namespace chart
{

RangeSelectionListener::RangeSelectionListener(
    RangeSelectionListenerParent & rParent,
    const ::rtl::OUString & rInitialRange,
    const Reference< frame::XModel >& xModelToLockController ) :
        m_rParent( rParent ),
        m_aRange( rInitialRange ),
        m_aControllerLockGuard( xModelToLockController )
{}

RangeSelectionListener::~RangeSelectionListener()
{}

// ____ XRangeSelectionListener ____
void SAL_CALL RangeSelectionListener::done( const sheet::RangeSelectionEvent& aEvent )
    throw (uno::RuntimeException)
{
    m_aRange = aEvent.RangeDescriptor;
    m_rParent.listeningFinished( m_aRange );
}

void SAL_CALL RangeSelectionListener::aborted( const sheet::RangeSelectionEvent& /*aEvent*/ )
    throw (uno::RuntimeException)
{
    m_rParent.listeningFinished( m_aRange );
}

// ____ XEventListener ____
void SAL_CALL RangeSelectionListener::disposing( const lang::EventObject& /*Source*/ )
    throw (uno::RuntimeException)
{
    m_rParent.disposingRangeSelection();
}

} //  namespace chart

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
