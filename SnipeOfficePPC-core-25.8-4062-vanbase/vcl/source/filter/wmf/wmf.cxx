/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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


#include "winmtf.hxx"
#include "emfwr.hxx"
#include "wmfwr.hxx"
#include <vcl/wmf.hxx>
#include <comphelper/scopeguard.hxx>

// -----------------------------------------------------------------------------

sal_Bool ConvertWMFToGDIMetaFile( SvStream & rStreamWMF, GDIMetaFile & rGDIMetaFile, FilterConfigItem* pConfigItem, WMF_EXTERNALHEADER *pExtHeader )
{
    sal_uInt32 nMetaType;
    sal_uInt32 nOrgPos = rStreamWMF.Tell();
    sal_uInt16 nOrigNumberFormat = rStreamWMF.GetNumberFormatInt();
    rStreamWMF.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    rStreamWMF.Seek( 0x28 );
    rStreamWMF >> nMetaType;
    rStreamWMF.Seek( nOrgPos );
    if ( nMetaType == 0x464d4520 )
    {
        if ( EnhWMFReader( rStreamWMF, rGDIMetaFile, pConfigItem ).ReadEnhWMF() == sal_False )
            rStreamWMF.SetError( SVSTREAM_FILEFORMAT_ERROR );
    }
    else
    {
        WMFReader( rStreamWMF, rGDIMetaFile, pConfigItem, pExtHeader ).ReadWMF( );
    }
    rStreamWMF.SetNumberFormatInt( nOrigNumberFormat );
    return !rStreamWMF.GetError();
}

// -----------------------------------------------------------------------------

sal_Bool ReadWindowMetafile( SvStream& rStream, GDIMetaFile& rMTF, FilterConfigItem* pFilterConfigItem )
{
    sal_uInt32 nMetaType(0);
    sal_uInt32 nOrgPos = rStream.Tell();

    sal_uInt16 nOrigNumberFormat = rStream.GetNumberFormatInt();
    rStream.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    //exception-safe reset nOrigNumberFormat at end of scope
    const ::comphelper::ScopeGuard aScopeGuard(
        boost::bind(&SvStream::SetNumberFormatInt, ::boost::ref(rStream),
          nOrigNumberFormat));

    rStream.Seek( 0x28 );
    rStream >> nMetaType;
    rStream.Seek( nOrgPos );

    if (!rStream.good())
        return false;

    if ( nMetaType == 0x464d4520 )
    {
        if ( EnhWMFReader( rStream, rMTF, NULL ).ReadEnhWMF() == sal_False )
            rStream.SetError( SVSTREAM_FILEFORMAT_ERROR );
    }
    else
    {
        WMFReader( rStream, rMTF, pFilterConfigItem ).ReadWMF();
    }

    return rStream.good();
}

// -----------------------------------------------------------------------------

sal_Bool ConvertGDIMetaFileToWMF( const GDIMetaFile & rMTF, SvStream & rTargetStream,
                              FilterConfigItem* pConfigItem, sal_Bool bPlaceable)
{
    WMFWriter aWMFWriter;
    return aWMFWriter.WriteWMF( rMTF, rTargetStream, pConfigItem, bPlaceable );
}

// -----------------------------------------------------------------------------

sal_Bool ConvertGDIMetaFileToEMF( const GDIMetaFile & rMTF, SvStream & rTargetStream,
                              FilterConfigItem* pConfigItem )
{
    EMFWriter aEMFWriter(rTargetStream);
    return aEMFWriter.WriteEMF( rMTF, pConfigItem );
}

// -----------------------------------------------------------------------------

sal_Bool WriteWindowMetafileBits( SvStream& rStream, const GDIMetaFile& rMTF )
{
    return WMFWriter().WriteWMF( rMTF, rStream, NULL, sal_False );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
