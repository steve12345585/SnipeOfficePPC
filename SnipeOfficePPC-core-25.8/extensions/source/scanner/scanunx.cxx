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

#include <scanner.hxx>
#include <sanedlg.hxx>
#include <osl/thread.hxx>
#include <boost/shared_ptr.hpp>
#include <boost/shared_ptr.hpp>

#if OSL_DEBUG_LEVEL > 1
#include <stdio.h>
#endif

BitmapTransporter::BitmapTransporter()
{
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "BitmapTransporter\n" );
#endif
}

BitmapTransporter::~BitmapTransporter()
{
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "~BitmapTransporter\n" );
#endif
}

// -----------------------------------------------------------------------------

ANY SAL_CALL BitmapTransporter::queryInterface( const Type& rType ) throw( RuntimeException )
{
    const ANY aRet( cppu::queryInterface( rType, static_cast< AWT::XBitmap* >( this ) ) );

    return( aRet.hasValue() ? aRet : OWeakObject::queryInterface( rType ) );
}

// -----------------------------------------------------------------------------

AWT::Size BitmapTransporter::getSize() throw()
{
    osl::MutexGuard aGuard( m_aProtector );
    int         nPreviousPos = m_aStream.Tell();
    AWT::Size   aRet;

    // ensure that there is at least a header
    m_aStream.Seek( STREAM_SEEK_TO_END );
    int nLen = m_aStream.Tell();
    if( nLen > 15 )
    {
        m_aStream.Seek( 4 );
        m_aStream >> aRet.Width >> aRet.Height;
    }
    else
        aRet.Width = aRet.Height = 0;

    m_aStream.Seek( nPreviousPos );

    return aRet;
}

// -----------------------------------------------------------------------------

SEQ( sal_Int8 ) BitmapTransporter::getDIB() throw()
{
    osl::MutexGuard aGuard( m_aProtector );
    int         nPreviousPos = m_aStream.Tell();

    // create return value
    m_aStream.Seek( STREAM_SEEK_TO_END );
    int nBytes = m_aStream.Tell();
    m_aStream.Seek( 0 );

    SEQ( sal_Int8 ) aValue( nBytes );
    m_aStream.Read( aValue.getArray(), nBytes );
    m_aStream.Seek( nPreviousPos );

    return aValue;
}

// --------------
// - SaneHolder -
// --------------

struct SaneHolder
{
    Sane                m_aSane;
    REF( AWT::XBitmap ) m_xBitmap;
    osl::Mutex          m_aProtector;
    ScanError           m_nError;
    bool                m_bBusy;

    SaneHolder() : m_nError(ScanError_ScanErrorNone), m_bBusy(false) {}
};

namespace
{
    typedef std::vector< boost::shared_ptr<SaneHolder> > sanevec;
    class allSanes
    {
    private:
        int mnRefCount;
    public:
        sanevec m_aSanes;
        allSanes() : mnRefCount(0) {}
        void acquire();
        void release();
    };

    void allSanes::acquire()
    {
        ++mnRefCount;
    }

    void allSanes::release()
    {
        // was unused, now because of i99835: "Scanning interface not SANE API
        // compliant" destroy all SaneHolder to get Sane Dtor called
        --mnRefCount;
        if (!mnRefCount)
            m_aSanes.clear();
    }

    struct theSaneProtector : public rtl::Static<osl::Mutex, theSaneProtector> {};
    struct theSanes : public rtl::Static<allSanes, theSanes> {};
}

// -----------------
// - ScannerThread -
// -----------------

class ScannerThread : public osl::Thread
{
    boost::shared_ptr<SaneHolder>               m_pHolder;
    REF( com::sun::star::lang::XEventListener ) m_xListener;
    ScannerManager*                             m_pManager; // just for the disposing call

public:
    virtual void run();
    virtual void onTerminated() { delete this; }
public:
    ScannerThread( boost::shared_ptr<SaneHolder> pHolder,
                   const REF( com::sun::star::lang::XEventListener )& listener,
                   ScannerManager* pManager );
    virtual ~ScannerThread();
};

// -----------------------------------------------------------------------------

ScannerThread::ScannerThread(
                             boost::shared_ptr<SaneHolder> pHolder,
                             const REF( com::sun::star::lang::XEventListener )& listener,
                             ScannerManager* pManager )
        : m_pHolder( pHolder ), m_xListener( listener ), m_pManager( pManager )
{
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "ScannerThread\n" );
#endif
}

ScannerThread::~ScannerThread()
{
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "~ScannerThread\n" );
#endif
}

void ScannerThread::run()
{
    osl::MutexGuard         aGuard( m_pHolder->m_aProtector );
    BitmapTransporter*  pTransporter = new BitmapTransporter;
    REF( XInterface )   aIf( static_cast< OWeakObject* >( pTransporter ) );

    m_pHolder->m_xBitmap = REF( AWT::XBitmap )( aIf, UNO_QUERY );

    m_pHolder->m_bBusy = true;
    if( m_pHolder->m_aSane.IsOpen() )
    {
        int nOption = m_pHolder->m_aSane.GetOptionByName( "preview" );
        if( nOption != -1 )
            m_pHolder->m_aSane.SetOptionValue( nOption, (sal_Bool)sal_False );

        m_pHolder->m_nError =
            m_pHolder->m_aSane.Start( *pTransporter ) ?
            ScanError_ScanErrorNone : ScanError_ScanCanceled;
    }
    else
        m_pHolder->m_nError = ScanError_ScannerNotAvailable;


    REF( XInterface ) xXInterface( static_cast< OWeakObject* >( m_pManager ) );
    m_xListener->disposing( com::sun::star::lang::EventObject(xXInterface) );
    m_pHolder->m_bBusy = false;
}

// ------------------
// - ScannerManager -
// ------------------

void ScannerManager::AcquireData()
{
    osl::MutexGuard aGuard( theSaneProtector::get() );
    theSanes::get().acquire();
}

void ScannerManager::ReleaseData()
{
    osl::MutexGuard aGuard( theSaneProtector::get() );
    theSanes::get().release();
}

// -----------------------------------------------------------------------------

AWT::Size ScannerManager::getSize() throw()
{
    AWT::Size aRet;
    aRet.Width = aRet.Height = 0;
    return aRet;
}

// -----------------------------------------------------------------------------

SEQ( sal_Int8 ) ScannerManager::getDIB() throw()
{
    return SEQ( sal_Int8 )();
}

// -----------------------------------------------------------------------------

SEQ( ScannerContext ) ScannerManager::getAvailableScanners() throw()
{
    osl::MutexGuard aGuard( theSaneProtector::get() );
    sanevec &rSanes = theSanes::get().m_aSanes;

    if( rSanes.empty() )
    {
        boost::shared_ptr<SaneHolder> pSaneHolder(new SaneHolder);
        if( Sane::IsSane() )
            rSanes.push_back( pSaneHolder );
    }

    if( Sane::IsSane() )
    {
        SEQ( ScannerContext ) aRet(1);
        aRet.getArray()[0].ScannerName      = ::rtl::OUString("SANE");
        aRet.getArray()[0].InternalData     = 0;
        return aRet;
    }

    return SEQ( ScannerContext )();
}

// -----------------------------------------------------------------------------

sal_Bool ScannerManager::configureScannerAndScan( ScannerContext& scanner_context,
					   const REF( com::sun::star::lang::XEventListener )& listener ) throw( ScannerException )
{
    bool bRet;
    bool bScan;
    {
        osl::MutexGuard aGuard( theSaneProtector::get() );
        sanevec &rSanes = theSanes::get().m_aSanes;

#if OSL_DEBUG_LEVEL > 1
        fprintf( stderr, "ScannerManager::configureScanner\n" );
#endif

        if( scanner_context.InternalData < 0 || (sal_uLong)scanner_context.InternalData >= rSanes.size() )
            throw ScannerException(
                ::rtl::OUString("Scanner does not exist"),
                REF( XScannerManager )( this ),
                ScanError_InvalidContext
            );

        boost::shared_ptr<SaneHolder> pHolder = rSanes[scanner_context.InternalData];
        if( pHolder->m_bBusy )
            throw ScannerException(
                ::rtl::OUString("Scanner is busy"),
                REF( XScannerManager )( this ),
                ScanError_ScanInProgress
            );

        pHolder->m_bBusy = true;
        SaneDlg aDlg( NULL, pHolder->m_aSane, listener.is() );
        bRet = aDlg.Execute();
        bScan = aDlg.getDoScan();
        pHolder->m_bBusy = false;
    }
    if ( bScan )
        startScan( scanner_context, listener );

    return bRet;
}

// -----------------------------------------------------------------------------

void ScannerManager::startScan( const ScannerContext& scanner_context,
                                const REF( com::sun::star::lang::XEventListener )& listener ) throw( ScannerException )
{
    osl::MutexGuard aGuard( theSaneProtector::get() );
    sanevec &rSanes = theSanes::get().m_aSanes;

#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "ScannerManager::startScan\n" );
#endif

    if( scanner_context.InternalData < 0 || (sal_uLong)scanner_context.InternalData >= rSanes.size() )
        throw ScannerException(
            ::rtl::OUString("Scanner does not exist"),
            REF( XScannerManager )( this ),
            ScanError_InvalidContext
            );
    boost::shared_ptr<SaneHolder> pHolder = rSanes[scanner_context.InternalData];
    if( pHolder->m_bBusy )
        throw ScannerException(
            ::rtl::OUString("Scanner is busy"),
            REF( XScannerManager )( this ),
            ScanError_ScanInProgress
            );
    pHolder->m_bBusy = true;

    ScannerThread* pThread = new ScannerThread( pHolder, listener, this );
    pThread->create();
}

// -----------------------------------------------------------------------------

ScanError ScannerManager::getError( const ScannerContext& scanner_context ) throw( ScannerException )
{
    osl::MutexGuard aGuard( theSaneProtector::get() );
    sanevec &rSanes = theSanes::get().m_aSanes;

    if( scanner_context.InternalData < 0 || (sal_uLong)scanner_context.InternalData >= rSanes.size() )
        throw ScannerException(
            ::rtl::OUString("Scanner does not exist"),
            REF( XScannerManager )( this ),
            ScanError_InvalidContext
            );

    boost::shared_ptr<SaneHolder> pHolder = rSanes[scanner_context.InternalData];

    return pHolder->m_nError;
}

// -----------------------------------------------------------------------------

REF( AWT::XBitmap ) ScannerManager::getBitmap( const ScannerContext& scanner_context ) throw( ScannerException )
{
    osl::MutexGuard aGuard( theSaneProtector::get() );
    sanevec &rSanes = theSanes::get().m_aSanes;

    if( scanner_context.InternalData < 0 || (sal_uLong)scanner_context.InternalData >= rSanes.size() )
        throw ScannerException(
            ::rtl::OUString("Scanner does not exist"),
            REF( XScannerManager )( this ),
            ScanError_InvalidContext
            );
    boost::shared_ptr<SaneHolder> pHolder = rSanes[scanner_context.InternalData];

    osl::MutexGuard aProtGuard( pHolder->m_aProtector );

    REF( AWT::XBitmap ) xRet( pHolder->m_xBitmap );
    pHolder->m_xBitmap = REF( AWT::XBitmap )();

    return xRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
