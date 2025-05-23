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

#include <avmedia/mediaitem.hxx>

#include <com/sun/star/uno/Sequence.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/XStorage.hpp>
#include <com/sun/star/embed/XTransactedObject.hpp>
#include <com/sun/star/document/XStorageBasedDocument.hpp>
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#include <com/sun/star/uri/UriReferenceFactory.hpp>
#include <com/sun/star/uri/XUriReference.hpp>
#include <com/sun/star/uri/XUriReferenceFactory.hpp>

#include <rtl/ustrbuf.hxx>

#include <ucbhelper/content.hxx>

#include <comphelper/processfactory.hxx>
#include <comphelper/storagehelper.hxx>

using namespace ::com::sun::star;

namespace avmedia
{

// -------------
// - MediaItem -
// -------------

TYPEINIT1_AUTOFACTORY( MediaItem, ::SfxPoolItem );

struct MediaItem::Impl
{
    OUString                m_URL;
    OUString                m_TempFileURL;
    sal_uInt32              m_nMaskSet;
    MediaState              m_eState;
    double                  m_fTime;
    double                  m_fDuration;
    sal_Int16               m_nVolumeDB;
    bool                    m_bLoop;
    bool                    m_bMute;
    ::com::sun::star::media::ZoomLevel m_eZoom;

    Impl(sal_uInt32 const nMaskSet)
        : m_nMaskSet( nMaskSet )
        , m_eState( MEDIASTATE_STOP )
        , m_fTime( 0.0 )
        , m_fDuration( 0.0 )
        , m_nVolumeDB( 0 )
        , m_bLoop( false )
        , m_bMute( false )
        , m_eZoom( ::com::sun::star::media::ZoomLevel_NOT_AVAILABLE )
    {
    }
    Impl(Impl const& rOther)
        : m_URL( rOther.m_URL )
        , m_TempFileURL( rOther.m_TempFileURL )
        , m_nMaskSet( rOther.m_nMaskSet )
        , m_eState( rOther.m_eState )
        , m_fTime( rOther.m_fTime )
        , m_fDuration( rOther.m_fDuration )
        , m_nVolumeDB( rOther.m_nVolumeDB )
        , m_bLoop( rOther.m_bLoop )
        , m_bMute( rOther.m_bMute )
        , m_eZoom( rOther.m_eZoom )
    {
    }
};

// ------------------------------------------------------------------------------

MediaItem::MediaItem( sal_uInt16 const i_nWhich, sal_uInt32 const nMaskSet )
    : SfxPoolItem( i_nWhich )
    , m_pImpl( new Impl(nMaskSet) )
{
}

// ------------------------------------------------------------------------------

MediaItem::MediaItem( const MediaItem& rItem )
    : SfxPoolItem( rItem )
    , m_pImpl( new Impl(*rItem.m_pImpl) )
{
}

// ------------------------------------------------------------------------------

MediaItem::~MediaItem()
{
}

// ------------------------------------------------------------------------------

int MediaItem::operator==( const SfxPoolItem& rItem ) const
{
    assert( SfxPoolItem::operator==(rItem));
    MediaItem const& rOther(static_cast< const MediaItem& >(rItem));
    return m_pImpl->m_nMaskSet == rOther.m_pImpl->m_nMaskSet
        && m_pImpl->m_URL == rOther.m_pImpl->m_URL
        && m_pImpl->m_eState == rOther.m_pImpl->m_eState
        && m_pImpl->m_fDuration == rOther.m_pImpl->m_fDuration
        && m_pImpl->m_fTime == rOther.m_pImpl->m_fTime
        && m_pImpl->m_nVolumeDB == rOther.m_pImpl->m_nVolumeDB
        && m_pImpl->m_bLoop == rOther.m_pImpl->m_bLoop
        && m_pImpl->m_bMute == rOther.m_pImpl->m_bMute
        && m_pImpl->m_eZoom == rOther.m_pImpl->m_eZoom;
}

// ------------------------------------------------------------------------------

SfxPoolItem* MediaItem::Clone( SfxItemPool* ) const
{
    return new MediaItem( *this );
}

//------------------------------------------------------------------------

SfxItemPresentation MediaItem::GetPresentation( SfxItemPresentation,
                                                  SfxMapUnit,
                                                  SfxMapUnit,
                                                  XubString& rText,
                                                  const IntlWrapper * ) const
{
    rText.Erase();
    return SFX_ITEM_PRESENTATION_NONE;
}

//------------------------------------------------------------------------

bool MediaItem::QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 ) const
{
    uno::Sequence< uno::Any > aSeq( 9 );

    aSeq[ 0 ] <<= m_pImpl->m_URL;
    aSeq[ 1 ] <<= m_pImpl->m_nMaskSet;
    aSeq[ 2 ] <<= static_cast< sal_Int32 >( m_pImpl->m_eState );
    aSeq[ 3 ] <<= m_pImpl->m_fTime;
    aSeq[ 4 ] <<= m_pImpl->m_fDuration;
    aSeq[ 5 ] <<= m_pImpl->m_nVolumeDB;
    aSeq[ 6 ] <<= m_pImpl->m_bLoop;
    aSeq[ 7 ] <<= m_pImpl->m_bMute;
    aSeq[ 8 ] <<= m_pImpl->m_eZoom;

    rVal <<= aSeq;

    return true;
}

//------------------------------------------------------------------------

bool MediaItem::PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 )
{
    uno::Sequence< uno::Any >   aSeq;
    bool                        bRet = false;

    if( ( rVal >>= aSeq ) && ( aSeq.getLength() == 9 ) )
    {
        sal_Int32 nInt32 = 0;

        aSeq[ 0 ] >>= m_pImpl->m_URL;
        aSeq[ 1 ] >>= m_pImpl->m_nMaskSet;
        aSeq[ 2 ] >>= nInt32;
        m_pImpl->m_eState = static_cast< MediaState >( nInt32 );
        aSeq[ 3 ] >>= m_pImpl->m_fTime;
        aSeq[ 4 ] >>= m_pImpl->m_fDuration;
        aSeq[ 5 ] >>= m_pImpl->m_nVolumeDB;
        aSeq[ 6 ] >>= m_pImpl->m_bLoop;
        aSeq[ 7 ] >>= m_pImpl->m_bMute;
        aSeq[ 8 ] >>= m_pImpl->m_eZoom;

        bRet = true;
    }

    return bRet;
}

//------------------------------------------------------------------------

void MediaItem::merge( const MediaItem& rMediaItem )
{
    const sal_uInt32 nMaskSet = rMediaItem.getMaskSet();

    if( AVMEDIA_SETMASK_URL & nMaskSet )
        setURL( rMediaItem.getURL(), &rMediaItem.getTempURL() );

    if( AVMEDIA_SETMASK_STATE & nMaskSet )
        setState( rMediaItem.getState() );

    if( AVMEDIA_SETMASK_DURATION & nMaskSet )
        setDuration( rMediaItem.getDuration() );

    if( AVMEDIA_SETMASK_TIME & nMaskSet )
        setTime( rMediaItem.getTime() );

    if( AVMEDIA_SETMASK_LOOP & nMaskSet )
        setLoop( rMediaItem.isLoop() );

    if( AVMEDIA_SETMASK_MUTE & nMaskSet )
        setMute( rMediaItem.isMute() );

    if( AVMEDIA_SETMASK_VOLUMEDB & nMaskSet )
        setVolumeDB( rMediaItem.getVolumeDB() );

    if( AVMEDIA_SETMASK_ZOOM & nMaskSet )
        setZoom( rMediaItem.getZoom() );
}

//------------------------------------------------------------------------

sal_uInt32 MediaItem::getMaskSet() const
{
    return m_pImpl->m_nMaskSet;
}

//------------------------------------------------------------------------

void MediaItem::setURL( const OUString& rURL,
        OUString const*const pTempURL)
{
    m_pImpl->m_URL = rURL;
    m_pImpl->m_nMaskSet |= AVMEDIA_SETMASK_URL;
    m_pImpl->m_TempFileURL = (pTempURL) ? *pTempURL : OUString();
}

//------------------------------------------------------------------------

const OUString& MediaItem::getURL() const
{
    return m_pImpl->m_URL;
}

const OUString& MediaItem::getTempURL() const
{
    return m_pImpl->m_TempFileURL;
}

//------------------------------------------------------------------------

void MediaItem::setState( MediaState eState )
{
    m_pImpl->m_eState = eState;
    m_pImpl->m_nMaskSet |= AVMEDIA_SETMASK_STATE;
}

//------------------------------------------------------------------------

MediaState MediaItem::getState() const
{
    return m_pImpl->m_eState;
}

//------------------------------------------------------------------------

void MediaItem::setDuration( double fDuration )
{
    m_pImpl->m_fDuration = fDuration;
    m_pImpl->m_nMaskSet |= AVMEDIA_SETMASK_DURATION;
}

//------------------------------------------------------------------------

double MediaItem::getDuration() const
{
    return m_pImpl->m_fDuration;
}

//------------------------------------------------------------------------

void MediaItem::setTime( double fTime )
{
    m_pImpl->m_fTime = fTime;
    m_pImpl->m_nMaskSet |= AVMEDIA_SETMASK_TIME;
}

//------------------------------------------------------------------------

double MediaItem::getTime() const
{
    return m_pImpl->m_fTime;
}

//------------------------------------------------------------------------

void MediaItem::setLoop( bool bLoop )
{
    m_pImpl->m_bLoop = bLoop;
    m_pImpl->m_nMaskSet |= AVMEDIA_SETMASK_LOOP;
}

//------------------------------------------------------------------------

bool MediaItem::isLoop() const
{
    return m_pImpl->m_bLoop;
}

//------------------------------------------------------------------------

void MediaItem::setMute( bool bMute )
{
    m_pImpl->m_bMute = bMute;
    m_pImpl->m_nMaskSet |= AVMEDIA_SETMASK_MUTE;
}

//------------------------------------------------------------------------

bool MediaItem::isMute() const
{
    return m_pImpl->m_bMute;
}

//------------------------------------------------------------------------

void MediaItem::setVolumeDB( sal_Int16 nDB )
{
    m_pImpl->m_nVolumeDB = nDB;
    m_pImpl->m_nMaskSet |= AVMEDIA_SETMASK_VOLUMEDB;
}

//------------------------------------------------------------------------

sal_Int16 MediaItem::getVolumeDB() const
{
    return m_pImpl->m_nVolumeDB;
}

//------------------------------------------------------------------------

void MediaItem::setZoom( ::com::sun::star::media::ZoomLevel eZoom )
{
    m_pImpl->m_eZoom = eZoom;
    m_pImpl->m_nMaskSet |= AVMEDIA_SETMASK_ZOOM;
}

//------------------------------------------------------------------------

::com::sun::star::media::ZoomLevel MediaItem::getZoom() const
{
    return m_pImpl->m_eZoom;
}

//------------------------------------------------------------------------

static OUString lcl_GetFilename(OUString const& rSourceURL)
{
    uno::Reference<uri::XUriReferenceFactory> const xUriFactory(
        uri::UriReferenceFactory::create(
            comphelper::getProcessComponentContext()));
    uno::Reference<uri::XUriReference> const xSourceURI(
        xUriFactory->parse(rSourceURL), uno::UNO_SET_THROW);

    OUString filename;
    {
        sal_Int32 const nSegments(xSourceURI->getPathSegmentCount());
        if (0 < nSegments)
        {
            filename = xSourceURI->getPathSegment(nSegments - 1);
        }
    }
    if (!::comphelper::OStorageHelper::IsValidZipEntryFileName(
                filename, false) || !filename.getLength())
    {
        filename = "media";
    }
    return filename;
}

static uno::Reference<io::XStream>
lcl_CreateStream(uno::Reference<embed::XStorage> const& xStorage,
        OUString const& rFilename)
{
    OUString filename(rFilename);

    if (xStorage->hasByName(filename))
    {
        OUString basename;
        OUString suffix;
        sal_Int32 const nIndex(rFilename.lastIndexOf(sal_Unicode('.')));
        if (0 < nIndex)
        {
            basename = rFilename.copy(0, nIndex);
            suffix = rFilename.copy(nIndex);
        }
        sal_Int32 count(0); // sigh... try to generate non-existent name
        do
        {
            ++count;
            filename = basename + OUString::valueOf(count) + suffix;
        }
        while (xStorage->hasByName(filename));
    }

    uno::Reference<io::XStream> const xStream(
        xStorage->openStreamElement(filename,
            embed::ElementModes::WRITE | embed::ElementModes::TRUNCATE),
        uno::UNO_SET_THROW);
    uno::Reference< beans::XPropertySet > const xStreamProps(xStream,
        uno::UNO_QUERY);
    if (xStreamProps.is()) { // this is NOT supported in FileSystemStorage
        xStreamProps->setPropertyValue("MediaType", uno::makeAny(OUString(
            //FIXME how to detect real media type?
            //but currently xmloff has this one hardcoded anyway...
            "application/vnd.sun.star.media")));
        xStreamProps->setPropertyValue( // turn off compression
            "Compressed", uno::makeAny(sal_False));
    }
    return xStream;
}

bool EmbedMedia(uno::Reference<frame::XModel> const& xModel,
        OUString const& rSourceURL, OUString & o_rEmbeddedURL)
{
    try
    {
        ::ucbhelper::Content sourceContent(rSourceURL,
                uno::Reference<ucb::XCommandEnvironment>(),
                comphelper::getProcessComponentContext());

        uno::Reference<document::XStorageBasedDocument> const xSBD(xModel,
                uno::UNO_QUERY_THROW);
        uno::Reference<embed::XStorage> const xStorage(
                xSBD->getDocumentStorage(), uno::UNO_QUERY_THROW);

        OUString const media("Media");
        uno::Reference<embed::XStorage> const xSubStorage(
            xStorage->openStorageElement(media, embed::ElementModes::WRITE));

        OUString filename(lcl_GetFilename(rSourceURL));

        uno::Reference<io::XStream> const xStream(
            lcl_CreateStream(xSubStorage, filename), uno::UNO_SET_THROW);
        uno::Reference<io::XOutputStream> const xOutStream(
            xStream->getOutputStream(), uno::UNO_SET_THROW);

        if (!sourceContent.openStream(xOutStream)) // copy file to storage
        {
            SAL_INFO("avmedia", "openStream to storage failed");
            return false;
        }

        uno::Reference<embed::XTransactedObject> const xSubTransaction(
            xSubStorage, uno::UNO_QUERY);
        if (xSubTransaction.is()) {
            xSubTransaction->commit();
        }
        uno::Reference<embed::XTransactedObject> const xTransaction(
            xStorage, uno::UNO_QUERY);
        if (xTransaction.is()) {
            xTransaction->commit();
        }

        OUStringBuffer buf("vnd.sun.star.Package:");
        buf.append(media);
        buf.append(sal_Unicode('/'));
        buf.append(filename);
        o_rEmbeddedURL = buf.makeStringAndClear();
        return true;
    }
    catch (uno::Exception const&)
    {
        SAL_WARN("avmedia",
                "Exception while trying to embed media");
    }
    return false;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
