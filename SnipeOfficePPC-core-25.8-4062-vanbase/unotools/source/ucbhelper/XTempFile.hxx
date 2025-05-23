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
#ifndef _XTEMPFILE_HXX_
#define _XTEMPFILE_HXX_

#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/io/XSeekable.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/io/XTempFile.hpp>
#include <com/sun/star/io/XTruncate.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase5.hxx>
#include <cppuhelper/propertysetmixin.hxx>
#include <osl/mutex.hxx>

class SvStream;
namespace utl { class TempFile; }

typedef  ::cppu::WeakImplHelper5<   ::com::sun::star::io::XTempFile
                                    ,   ::com::sun::star::io::XInputStream
                                                  , ::com::sun::star::io::XOutputStream
                                                  , ::com::sun::star::io::XTruncate
                                                  , ::com::sun::star::lang::XServiceInfo
                                                  >
                                    OTempFileBase;

class OTempFileService :
    public OTempFileBase,
    public ::cppu::PropertySetMixin< ::com::sun::star::io::XTempFile >
{
protected:
    ::utl::TempFile*    mpTempFile;
    ::osl::Mutex        maMutex;
    SvStream*           mpStream;
    sal_Bool            mbRemoveFile;
    sal_Bool            mbInClosed;
    sal_Bool            mbOutClosed;

    sal_Int64           mnCachedPos;
    sal_Bool            mbHasCachedPos;

    void checkError () const;
    void checkConnected ();

public:
    OTempFileService (::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & context);

    //Methods
    //  XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType )
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL acquire(  )
        throw ();
    virtual void SAL_CALL release(  )
        throw ();
    //  XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  )
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL getImplementationId(  )
        throw (::com::sun::star::uno::RuntimeException);

    //  XTempFile
    virtual ::sal_Bool SAL_CALL getRemoveFile()
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setRemoveFile( ::sal_Bool _removefile )
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getUri()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getResourceName()
        throw (::com::sun::star::uno::RuntimeException);

    // XInputStream
    virtual ::sal_Int32 SAL_CALL readBytes( ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData, ::sal_Int32 nBytesToRead )
        throw (::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL readSomeBytes( ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData, ::sal_Int32 nMaxBytesToRead )
        throw (::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL skipBytes( ::sal_Int32 nBytesToSkip )
        throw (::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL available(  )
        throw (::com::sun::star::io::NotConnectedException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL closeInput(  )
        throw (::com::sun::star::io::NotConnectedException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    // XOutputStream
    virtual void SAL_CALL writeBytes( const ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData )
        throw (::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL flush(  )
        throw (::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL closeOutput(  )
        throw (::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    // XSeekable
    virtual void SAL_CALL seek( sal_Int64 location )
        throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Int64 SAL_CALL getPosition(  )
        throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Int64 SAL_CALL getLength(  )
        throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    // XStream
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getInputStream(  )
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > SAL_CALL getOutputStream(  )
        throw (::com::sun::star::uno::RuntimeException);
    // XTruncate
    virtual void SAL_CALL truncate()
        throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw (::com::sun::star::uno::RuntimeException);

    //::com::sun::star::uno::Reference < ::com::sun::star::uno::XInterface > SAL_CALL XTempFile_createInstance( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & context);
    static ::rtl::OUString getImplementationName_Static ();
    static ::com::sun::star::uno::Sequence < ::rtl::OUString > getSupportedServiceNames_Static();

    static ::com::sun::star::uno::Reference < com::sun::star::lang::XSingleComponentFactory > createServiceFactory_Static();

private:
    OTempFileService( OTempFileService & );
    virtual ~OTempFileService ();

};
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
