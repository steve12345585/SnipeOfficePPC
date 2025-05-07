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


#include "enumeration.hxx"

#include <com/sun/star/container/NoSuchElementException.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/lang/WrappedTargetException.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/RuntimeException.hpp>

using com::sun::star::container::NoSuchElementException;
using com::sun::star::container::XIndexAccess;
using com::sun::star::lang::WrappedTargetException;
using com::sun::star::uno::Any;
using com::sun::star::uno::Reference;
using com::sun::star::uno::RuntimeException;


Enumeration::Enumeration( XIndexAccess* pContainer )
    : mxContainer( pContainer ),
      mnIndex( 0 )
{
    OSL_ENSURE( mxContainer.is(), "no container?" );
}

sal_Bool Enumeration::hasMoreElements()
    throw( RuntimeException )
{
    if( ! mxContainer.is() )
        throw RuntimeException();

    return mnIndex < mxContainer->getCount();
}

Any Enumeration::nextElement()
    throw( NoSuchElementException,
           WrappedTargetException,
           RuntimeException )
{
    if( ! mxContainer.is() )
        throw RuntimeException();
    if( mnIndex >= mxContainer->getCount() )
        throw NoSuchElementException();

    return mxContainer->getByIndex( mnIndex++ );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
