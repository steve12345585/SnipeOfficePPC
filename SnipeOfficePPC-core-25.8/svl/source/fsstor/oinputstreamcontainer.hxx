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

#ifndef _OINPUTSTREAMCONTAINER_HXX_
#define _OINPUTSTREAMCONTAINER_HXX_

#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/embed/XExtendedStorageStream.hpp>
#include <com/sun/star/io/XSeekable.hpp>


#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/interfacecontainer.h>

#include <osl/mutex.hxx>

class OFSInputStreamContainer : public cppu::WeakImplHelper2 < ::com::sun::star::io::XInputStream
                                                            ,::com::sun::star::embed::XExtendedStorageStream >
                            , public ::com::sun::star::io::XSeekable
{
protected:
    ::osl::Mutex m_aMutex;

    ::com::sun::star::uno::Reference < ::com::sun::star::io::XInputStream > m_xInputStream;
    ::com::sun::star::uno::Reference < ::com::sun::star::io::XSeekable > m_xSeekable;

    sal_Bool m_bSeekable;

    sal_Bool m_bDisposed;

    ::cppu::OInterfaceContainerHelper* m_pListenersContainer; // list of listeners

public:
    OFSInputStreamContainer( const ::com::sun::star::uno::Reference < ::com::sun::star::io::XInputStream >& xStream );

    virtual ~OFSInputStreamContainer();

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& rType ) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL acquire() throw();
    virtual void SAL_CALL release() throw();

    // XInputStream
    virtual sal_Int32 SAL_CALL readBytes( ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL readSomeBytes( ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nMaxBytesToRead )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL skipBytes( sal_Int32 nBytesToSkip )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL available(  )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL closeInput(  )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    //XStream
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getInputStream(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > SAL_CALL getOutputStream(  ) throw (::com::sun::star::uno::RuntimeException);

    //XSeekable
    virtual void SAL_CALL seek( sal_Int64 location ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Int64 SAL_CALL getPosition() throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Int64 SAL_CALL getLength() throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    //XComponent
    virtual void SAL_CALL dispose() throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);

};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
