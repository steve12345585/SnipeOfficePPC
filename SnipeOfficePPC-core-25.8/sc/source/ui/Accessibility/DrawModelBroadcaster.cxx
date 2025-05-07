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

#include "DrawModelBroadcaster.hxx"
#include <osl/diagnose.h>
#include <rtl/strbuf.hxx>
#include <svx/svdmodel.hxx>
#include <svx/unomod.hxx>

using namespace ::com::sun::star;

ScDrawModelBroadcaster::ScDrawModelBroadcaster( SdrModel *pDrawModel ) :
    maEventListeners( maListenerMutex ),
    mpDrawModel( pDrawModel )
{
    if (mpDrawModel)
        StartListening( *mpDrawModel );
}

ScDrawModelBroadcaster::~ScDrawModelBroadcaster()
{
    if (mpDrawModel)
        EndListening( *mpDrawModel );
}

void SAL_CALL ScDrawModelBroadcaster::addEventListener( const uno::Reference< document::XEventListener >& xListener )
    throw (uno::RuntimeException)
{
    maEventListeners.addInterface( xListener );
}

void SAL_CALL ScDrawModelBroadcaster::removeEventListener( const uno::Reference< document::XEventListener >& xListener )
    throw (uno::RuntimeException)
{
    maEventListeners.removeInterface( xListener );
}

void ScDrawModelBroadcaster::Notify( SfxBroadcaster&,
        const SfxHint& rHint )
{
    const SdrHint *pSdrHint = PTR_CAST( SdrHint, &rHint );
    if( !pSdrHint )
        return;

    document::EventObject aEvent;
    if( !SvxUnoDrawMSFactory::createEvent( mpDrawModel, pSdrHint, aEvent ) )
        return;

    ::cppu::OInterfaceIteratorHelper aIter( maEventListeners );
    while( aIter.hasMoreElements() )
    {
        uno::Reference < document::XEventListener > xListener( aIter.next(), uno::UNO_QUERY );
        try
        {
            xListener->notifyEvent( aEvent );
        }
        catch( const uno::RuntimeException& r )
        {
            (void) r;
#if OSL_DEBUG_LEVEL > 1
            rtl::OStringBuffer aError(RTL_CONSTASCII_STRINGPARAM(
                "Runtime exception caught while notifying shape.:\n"));
            aError.append(rtl::OUStringToOString(r.Message, RTL_TEXTENCODING_ASCII_US));
            OSL_FAIL( aError.getStr() );
#endif
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
