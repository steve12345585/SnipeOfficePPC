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

#include "filinpstr.hxx"
#include "filerror.hxx"
#include "shell.hxx"
#include "prov.hxx"


using namespace fileaccess;
using namespace com::sun::star;
using namespace com::sun::star::ucb;



XInputStream_impl::XInputStream_impl( shell* pMyShell,const rtl::OUString& aUncPath, sal_Bool bLock )
    : m_xProvider( pMyShell->m_pProvider ),
      m_aFile( aUncPath ),
      m_nErrorCode( TASKHANDLER_NO_ERROR ),
      m_nMinorErrorCode( TASKHANDLER_NO_ERROR )
{
    sal_uInt32 nFlags = osl_File_OpenFlag_Read;
    if ( !bLock )
        nFlags |= osl_File_OpenFlag_NoLock;

    osl::FileBase::RC err = m_aFile.open( nFlags );
    if( err != osl::FileBase::E_None )
    {
        m_nIsOpen = false;
        m_aFile.close();

        m_nErrorCode = TASKHANDLING_OPEN_FOR_INPUTSTREAM;
        m_nMinorErrorCode = err;
    }
    else
        m_nIsOpen = true;
}


XInputStream_impl::~XInputStream_impl()
{
    try
    {
        closeInput();
    }
    catch (io::IOException const &)
    {
        OSL_FAIL("unexpected situation");
    }
    catch (uno::RuntimeException const &)
    {
        OSL_FAIL("unexpected situation");
    }
}


sal_Int32 SAL_CALL XInputStream_impl::CtorSuccess()
{
    return m_nErrorCode;
};



sal_Int32 SAL_CALL XInputStream_impl::getMinorError()
{
    return m_nMinorErrorCode;
}


//////////////////////////////////////////////////////////////////////////////////////////
//  XTypeProvider
//////////////////////////////////////////////////////////////////////////////////////////


XTYPEPROVIDER_IMPL_3( XInputStream_impl,
                      lang::XTypeProvider,
                      io::XSeekable,
                      io::XInputStream )



uno::Any SAL_CALL
XInputStream_impl::queryInterface(
    const uno::Type& rType )
    throw( uno::RuntimeException)
{
    uno::Any aRet = cppu::queryInterface( rType,
                                          (static_cast< io::XInputStream* >(this)),
                                          (static_cast< lang::XTypeProvider* >(this)),
                                          (static_cast< io::XSeekable* >(this)) );
    return aRet.hasValue() ? aRet : OWeakObject::queryInterface( rType );
}


void SAL_CALL
XInputStream_impl::acquire(
    void )
    throw()
{
    OWeakObject::acquire();
}


void SAL_CALL
XInputStream_impl::release(
    void )
    throw()
{
    OWeakObject::release();
}



sal_Int32 SAL_CALL
XInputStream_impl::readBytes(
                 uno::Sequence< sal_Int8 >& aData,
                 sal_Int32 nBytesToRead )
    throw( io::NotConnectedException,
           io::BufferSizeExceededException,
           io::IOException,
           uno::RuntimeException)
{
    if( ! m_nIsOpen ) throw io::IOException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( OSL_LOG_PREFIX ) ), uno::Reference< uno::XInterface >() );

    aData.realloc(nBytesToRead);
        //TODO! translate memory exhaustion (if it were detectable...) into
        // io::BufferSizeExceededException

    sal_uInt64 nrc(0);
    if(m_aFile.read( aData.getArray(),sal_uInt64(nBytesToRead),nrc )
       != osl::FileBase::E_None)
        throw io::IOException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( OSL_LOG_PREFIX ) ), uno::Reference< uno::XInterface >() );

    // Shrink aData in case we read less than nBytesToRead (XInputStream
    // documentation does not tell whether this is required, and I do not know
    // if any code relies on this, so be conservative---SB):
    if (sal::static_int_cast<sal_Int32>(nrc) != nBytesToRead)
        aData.realloc(sal_Int32(nrc));
    return ( sal_Int32 ) nrc;
}

sal_Int32 SAL_CALL
XInputStream_impl::readSomeBytes(
    uno::Sequence< sal_Int8 >& aData,
    sal_Int32 nMaxBytesToRead )
    throw( io::NotConnectedException,
           io::BufferSizeExceededException,
           io::IOException,
           uno::RuntimeException)
{
    return readBytes( aData,nMaxBytesToRead );
}


void SAL_CALL
XInputStream_impl::skipBytes(
    sal_Int32 nBytesToSkip )
    throw( io::NotConnectedException,
           io::BufferSizeExceededException,
           io::IOException,
           uno::RuntimeException)
{
    m_aFile.setPos( osl_Pos_Current, sal_uInt64( nBytesToSkip ) );
}


sal_Int32 SAL_CALL
XInputStream_impl::available(
    void )
    throw( io::NotConnectedException,
           io::IOException,
           uno::RuntimeException)
{
    return 0;
}


void SAL_CALL
XInputStream_impl::closeInput(
    void )
    throw( io::NotConnectedException,
           io::IOException,
           uno::RuntimeException )
{
    if( m_nIsOpen )
    {
        osl::FileBase::RC err = m_aFile.close();
        if( err != osl::FileBase::E_None )
            throw io::IOException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( OSL_LOG_PREFIX ) ), uno::Reference< uno::XInterface >() );
        m_nIsOpen = false;
    }
}


void SAL_CALL
XInputStream_impl::seek(
    sal_Int64 location )
    throw( lang::IllegalArgumentException,
           io::IOException,
           uno::RuntimeException )
{
    if( location < 0 )
        throw lang::IllegalArgumentException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( OSL_LOG_PREFIX ) ), uno::Reference< uno::XInterface >(), 0 );
    if( osl::FileBase::E_None != m_aFile.setPos( osl_Pos_Absolut, sal_uInt64( location ) ) )
        throw io::IOException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( OSL_LOG_PREFIX ) ), uno::Reference< uno::XInterface >() );
}


sal_Int64 SAL_CALL
XInputStream_impl::getPosition(
    void )
    throw( io::IOException,
           uno::RuntimeException )
{
    sal_uInt64 uPos;
    if( osl::FileBase::E_None != m_aFile.getPos( uPos ) )
        throw io::IOException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( OSL_LOG_PREFIX ) ), uno::Reference< uno::XInterface >() );
    return sal_Int64( uPos );
}

sal_Int64 SAL_CALL
XInputStream_impl::getLength(
    void )
    throw( io::IOException,
           uno::RuntimeException )
{
    sal_uInt64 uEndPos;
    if ( m_aFile.getSize(uEndPos) != osl::FileBase::E_None )
        throw io::IOException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( OSL_LOG_PREFIX ) ), uno::Reference< uno::XInterface >() );
    else
        return sal_Int64( uEndPos );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
