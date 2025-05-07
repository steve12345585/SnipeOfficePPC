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

#ifndef _WMF_HXX
#define _WMF_HXX

#include "svtools/svtdllapi.h"
#include <svtools/fltcall.hxx>

struct WMF_EXTERNALHEADER
{
    sal_uInt16 xExt;
    sal_uInt16 yExt;

    /** One of the following values:
        <ul>
            <li>MM_TEXT</li>
            <li>MM_LOMETRIC</li>
            <li>MM_HIMETRIC</li>
            <li>MM_LOENGLISH</li>
            <li>MM_HIENGLISH</li>
            <li>MM_TWIPS</li>
            <li>MM_ISOTROPIC</li>
            <li>MM_ANISOTROPIC</li>
        </ul>
        If this value is 0, then no external mapmode has been defined,
        the internal one should then be used.
     */
    sal_uInt16 mapMode;

    WMF_EXTERNALHEADER() :
        xExt( 0 ),
        yExt( 0 ),
        mapMode( 0 )
    {
    }
};

sal_Bool ConvertWMFToGDIMetaFile( SvStream & rStreamWMF, GDIMetaFile & rGDIMetaFile, FilterConfigItem* pConfigItem = NULL, WMF_EXTERNALHEADER *pExtHeader = NULL );

SVT_DLLPUBLIC sal_Bool ReadWindowMetafile( SvStream& rStream, GDIMetaFile& rMTF, FilterConfigItem* pConfigItem );

SVT_DLLPUBLIC sal_Bool ConvertGDIMetaFileToWMF( const GDIMetaFile & rMTF, SvStream & rTargetStream, FilterConfigItem* pConfigItem = NULL, sal_Bool bPlaceable = sal_True );

sal_Bool ConvertGDIMetaFileToEMF( const GDIMetaFile & rMTF, SvStream & rTargetStream, FilterConfigItem* pConfigItem = NULL );

SVT_DLLPUBLIC sal_Bool WriteWindowMetafileBits( SvStream& rStream, const GDIMetaFile& rMTF );

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
