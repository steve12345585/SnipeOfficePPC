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

#if defined _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable: 4917)
#endif
#include <prewin.h>
#include <postwin.h>
#include <objbase.h>
#include <strmif.h>
#include <Amvideo.h>
#include "interface.hxx"
#include <uuids.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif

#include "framegrabber.hxx"
#include "player.hxx"

#include <tools/stream.hxx>
#include <vcl/graph.hxx>
#include <unotools/localfilehelper.hxx>

#define AVMEDIA_WIN_FRAMEGRABBER_IMPLEMENTATIONNAME "com.sun.star.comp.avmedia.FrameGrabber_DirectX"
#define AVMEDIA_WIN_FRAMEGRABBER_SERVICENAME "com.sun.star.media.FrameGrabber_DirectX"

using namespace ::com::sun::star;

namespace avmedia { namespace win {

// ----------------
// - FrameGrabber -
// ----------------

FrameGrabber::FrameGrabber( const uno::Reference< lang::XMultiServiceFactory >& rxMgr ) :
    mxMgr( rxMgr )
{
    ::CoInitialize( NULL );
}

// ------------------------------------------------------------------------------

FrameGrabber::~FrameGrabber()
{
    ::CoUninitialize();
}

// ------------------------------------------------------------------------------

IMediaDet* FrameGrabber::implCreateMediaDet( const OUString& rURL ) const
{
    IMediaDet* pDet = NULL;

    if( SUCCEEDED( CoCreateInstance( CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, IID_IMediaDet, (void**) &pDet ) ) )
    {
        OUString aLocalStr;

        if( ::utl::LocalFileHelper::ConvertURLToPhysicalName( rURL, aLocalStr ) && !aLocalStr.isEmpty() )
        {
            if( !SUCCEEDED( pDet->put_Filename( ::SysAllocString( reinterpret_cast<LPCOLESTR>(aLocalStr.getStr()) ) ) ) )
            {
                pDet->Release();
                pDet = NULL;
            }
        }
    }

    return pDet;
}

// ------------------------------------------------------------------------------

bool FrameGrabber::create( const OUString& rURL )
{
    // just check if a MediaDet interface can be created with the given URL
    IMediaDet*  pDet = implCreateMediaDet( rURL );

    if( pDet )
    {
        maURL = rURL;
        pDet->Release();
        pDet = NULL;
    }
    else
        maURL = OUString();

    return( maURL.getLength() > 0 );
}

// ------------------------------------------------------------------------------

uno::Reference< graphic::XGraphic > SAL_CALL FrameGrabber::grabFrame( double fMediaTime )
    throw (uno::RuntimeException)
{
    uno::Reference< graphic::XGraphic > xRet;
    IMediaDet*                          pDet = implCreateMediaDet( maURL );

    if( pDet )
    {
        double  fLength;
        long    nStreamCount;
        bool    bFound = false;

        if( SUCCEEDED( pDet->get_OutputStreams( &nStreamCount ) ) )
        {
            for( long n = 0; ( n < nStreamCount ) && !bFound; ++n )
            {
                GUID aMajorType;

                if( SUCCEEDED( pDet->put_CurrentStream( n ) )  &&
                    SUCCEEDED( pDet->get_StreamType( &aMajorType ) ) &&
                    ( aMajorType == MEDIATYPE_Video ) )
                {
                    bFound = true;
                }
            }
        }

        if( bFound &&
            ( S_OK == pDet->get_StreamLength( &fLength ) ) &&
            ( fLength > 0.0 ) && ( fMediaTime >= 0.0 ) && ( fMediaTime <= fLength ) )
        {
            AM_MEDIA_TYPE   aMediaType;
            long            nWidth = 0, nHeight = 0, nSize = 0;

            if( SUCCEEDED( pDet->get_StreamMediaType( &aMediaType ) ) )
            {
                if( ( aMediaType.formattype == FORMAT_VideoInfo ) &&
                    ( aMediaType.cbFormat >= sizeof( VIDEOINFOHEADER ) ) )
                {
                    VIDEOINFOHEADER* pVih = reinterpret_cast< VIDEOINFOHEADER* >( aMediaType.pbFormat );

                    nWidth = pVih->bmiHeader.biWidth;
                    nHeight = pVih->bmiHeader.biHeight;

                    if( nHeight < 0 )
                        nHeight *= -1;
                }

                if( aMediaType.cbFormat != 0 )
                {
                    ::CoTaskMemFree( (PVOID) aMediaType.pbFormat );
                    aMediaType.cbFormat = 0;
                    aMediaType.pbFormat = NULL;
                }

                if( aMediaType.pUnk != NULL )
                {
                    aMediaType.pUnk->Release();
                    aMediaType.pUnk = NULL;
                }
            }

            if( ( nWidth > 0 ) && ( nHeight > 0 ) &&
                SUCCEEDED( pDet->GetBitmapBits( 0, &nSize, NULL, nWidth, nHeight ) ) &&
                ( nSize > 0  ) )
            {
                char* pBuffer = new char[ nSize ];

                try
                {
                    if( SUCCEEDED( pDet->GetBitmapBits( fMediaTime, NULL, pBuffer, nWidth, nHeight ) ) )
                    {
                        SvMemoryStream  aMemStm( pBuffer, nSize, STREAM_READ | STREAM_WRITE );
                        Bitmap          aBmp;

                        if( aBmp.Read( aMemStm, false ) && !aBmp.IsEmpty() )
                        {
                            const Graphic aGraphic( aBmp );
                            xRet = aGraphic.GetXGraphic();
                        }
                    }
                }
                catch( ... )
                {
                }

                delete [] pBuffer;
            }
        }

        pDet->Release();
    }

    return xRet;
}

// ------------------------------------------------------------------------------

OUString SAL_CALL FrameGrabber::getImplementationName(  )
    throw (uno::RuntimeException)
{
    return OUString( AVMEDIA_WIN_FRAMEGRABBER_IMPLEMENTATIONNAME );
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL FrameGrabber::supportsService( const OUString& ServiceName )
    throw (uno::RuntimeException)
{
    return ServiceName == AVMEDIA_WIN_FRAMEGRABBER_SERVICENAME;
}

// ------------------------------------------------------------------------------

uno::Sequence< OUString > SAL_CALL FrameGrabber::getSupportedServiceNames(  )
    throw (uno::RuntimeException)
{
    uno::Sequence< OUString > aRet(1);
    aRet[0] = AVMEDIA_WIN_FRAMEGRABBER_SERVICENAME ;

    return aRet;
}

} // namespace win
} // namespace avmedia

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
