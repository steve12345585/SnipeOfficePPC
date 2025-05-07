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


#include <vcl/svapp.hxx>
#include <comphelper/servicehelper.hxx>
#include "access.hrc"
#include <accpreview.hxx>

const sal_Char sServiceName[] = "com.sun.star.text.AccessibleTextDocumentPageView";
const sal_Char sImplementationName[] = "com.sun.star.comp.Writer.SwAccessibleDocumentPageView";

using ::com::sun::star::lang::IndexOutOfBoundsException;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;

//
// SwAccessiblePreview
//

SwAccessiblePreview::SwAccessiblePreview( SwAccessibleMap *pMp ) :
    SwAccessibleDocumentBase( pMp )
{
    SetName( GetResource( STR_ACCESS_DOC_NAME ) );
}

SwAccessiblePreview::~SwAccessiblePreview()
{
}

OUString SwAccessiblePreview::getImplementationName( )
    throw( RuntimeException )
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( sImplementationName ) );
}

sal_Bool SwAccessiblePreview::supportsService( const OUString& rServiceName )
    throw( RuntimeException )
{
    return rServiceName == sServiceName || rServiceName == sAccessibleServiceName;
}

Sequence<OUString> SwAccessiblePreview::getSupportedServiceNames( )
    throw( RuntimeException )
{
    Sequence<OUString> aSeq( 2 );
    aSeq[0] = OUString( RTL_CONSTASCII_USTRINGPARAM( sServiceName ) );
    aSeq[1] = OUString( RTL_CONSTASCII_USTRINGPARAM( sAccessibleServiceName ) );
    return aSeq;
}

namespace
{
    class theSwAccessiblePreviewImplementationId : public rtl::Static< UnoTunnelIdInit, theSwAccessiblePreviewImplementationId > {};
}

Sequence< sal_Int8 > SAL_CALL SwAccessiblePreview::getImplementationId()
        throw(RuntimeException)
{
    return theSwAccessiblePreviewImplementationId::get().getSeq();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
