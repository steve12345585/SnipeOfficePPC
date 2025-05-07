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
#include <svl/brdcst.hxx>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <vector>

#ifndef DELETEZ
#define DELETEZ(pPtr) { delete pPtr; pPtr = 0; }
#endif


struct SfxPoolVersion_Impl
{
    sal_uInt16          _nVer;
    sal_uInt16          _nStart, _nEnd;
    const sal_uInt16*         _pMap;

                    SfxPoolVersion_Impl( sal_uInt16 nVer, sal_uInt16 nStart, sal_uInt16 nEnd,
                                         const sal_uInt16 *pMap )
                    :   _nVer( nVer ),
                        _nStart( nStart ),
                        _nEnd( nEnd ),
                        _pMap( pMap )
                    {}
                    SfxPoolVersion_Impl( const SfxPoolVersion_Impl &rOrig )
                    :   _nVer( rOrig._nVer ),
                        _nStart( rOrig._nStart ),
                        _nEnd( rOrig._nEnd ),
                        _pMap( rOrig._pMap )
                    {}
};

typedef std::vector<SfxPoolItem*> SfxPoolItemArrayBase_Impl;

typedef boost::shared_ptr< SfxPoolVersion_Impl > SfxPoolVersion_ImplPtr;
typedef std::deque< SfxPoolVersion_ImplPtr > SfxPoolVersionArr_Impl;

struct SfxPoolItemArray_Impl: public SfxPoolItemArrayBase_Impl
{
    size_t  nFirstFree;

    SfxPoolItemArray_Impl ()
        : nFirstFree( 0 )
    {}
};

class SfxStyleSheetIterator;

struct SfxItemPool_Impl
{
    SfxBroadcaster                  aBC;
    std::vector<SfxPoolItemArray_Impl*> maPoolItems;
    std::vector<SfxItemPoolUser*> maSfxItemPoolUsers; /// ObjectUser section
    rtl::OUString                   aName;
    SfxPoolItem**                   ppPoolDefaults;
    SfxPoolItem**                   ppStaticDefaults;
    SfxItemPool*                    mpMaster;
    SfxItemPool*                    mpSecondary;
    sal_uInt16*                     mpPoolRanges;
    SfxPoolVersionArr_Impl          aVersions;
    sal_uInt16                      mnStart;
    sal_uInt16                      mnEnd;
    sal_uInt16                      mnFileFormatVersion;
    sal_uInt16                          nVersion;
    sal_uInt16                          nLoadingVersion;
    sal_uInt16                          nInitRefCount; // 1, beim Laden ggf. 2
    sal_uInt16                          nVerStart, nVerEnd; // WhichRange in Versions
    sal_uInt16                          nStoringStart, nStoringEnd; // zu speichernder Range
    sal_uInt8                           nMajorVer, nMinorVer; // Pool selbst
    SfxMapUnit                      eDefMetric;
    bool                            bInSetItem;
    bool                            bStreaming; // in Load() bzw. Store()
    bool                            mbPersistentRefCounts;

    SfxItemPool_Impl( SfxItemPool* pMaster, const rtl::OUString& rName, sal_uInt16 nStart, sal_uInt16 nEnd )
        : maPoolItems(nEnd - nStart + 1, static_cast<SfxPoolItemArray_Impl*>(NULL))
        , aName(rName)
        , ppPoolDefaults(new SfxPoolItem* [nEnd - nStart + 1])
        , ppStaticDefaults(0)
        , mpMaster(pMaster)
        , mpSecondary(NULL)
        , mpPoolRanges(NULL)
        , mnStart(nStart)
        , mnEnd(nEnd)
        , mnFileFormatVersion(0)
        , nLoadingVersion(0)
        , nInitRefCount(0)
        , nVerStart(0)
        , nVerEnd(0)
        , nStoringStart(0)
        , nStoringEnd(0)
        , nMajorVer(0)
        , nMinorVer(0)
        , bInSetItem(false)
        , bStreaming(false)
    {
        DBG_ASSERT(mnStart, "Start-Which-Id must be greater 0" );
        memset( ppPoolDefaults, 0, sizeof( SfxPoolItem* ) * (nEnd - nStart + 1));
    }

    ~SfxItemPool_Impl()
    {
        DeleteItems();
    }

    void DeleteItems()
    {
        std::vector<SfxPoolItemArray_Impl*>::iterator itr = maPoolItems.begin(), itrEnd = maPoolItems.end();
        for (; itr != itrEnd; ++itr)
            delete *itr;
        maPoolItems.clear();

        delete[] mpPoolRanges;
        mpPoolRanges = NULL;
        delete[] ppPoolDefaults;
        ppPoolDefaults = NULL;
    }

    void readTheItems(SvStream & rStream, sal_uInt32 nCount, sal_uInt16 nVersion,
                      SfxPoolItem * pDefItem, SfxPoolItemArray_Impl ** pArr);
};

// -----------------------------------------------------------------------

// IBM-C-Set mag keine doppelten Defines
#ifdef DBG
#  undef DBG
#endif

#if defined(DBG_UTIL) && defined(MSC)
#define DBG(x) x
#else
#define DBG(x)
#endif

#define CHECK_FILEFORMAT( rStream, nTag ) \
    {   sal_uInt16 nFileTag; \
        rStream >> nFileTag; \
        if ( nTag != nFileTag ) \
        { \
            OSL_FAIL( #nTag ); /*! s.u. */ \
            /*! error-code setzen und auswerten! */ \
            (rStream).SetError(SVSTREAM_FILEFORMAT_ERROR); \
            pImp->bStreaming = sal_False; \
            return rStream; \
        } \
    }

#define CHECK_FILEFORMAT_RELEASE( rStream, nTag, pPointer ) \
   {   sal_uInt16 nFileTag; \
       rStream >> nFileTag; \
       if ( nTag != nFileTag ) \
        { \
           OSL_FAIL( #nTag ); /*! s.u. */ \
           /*! error-code setzen und auswerten! */ \
           (rStream).SetError(SVSTREAM_FILEFORMAT_ERROR); \
           pImp->bStreaming = sal_False; \
           delete pPointer; \
            return rStream; \
        } \
    }

#define CHECK_FILEFORMAT2( rStream, nTag1, nTag2 ) \
    {   sal_uInt16 nFileTag; \
        rStream >> nFileTag; \
        if ( nTag1 != nFileTag && nTag2 != nFileTag ) \
        { \
            OSL_FAIL( #nTag1 ); /*! s.u. */ \
            /*! error-code setzen und auswerten! */ \
            (rStream).SetError(SVSTREAM_FILEFORMAT_ERROR); \
            pImp->bStreaming = sal_False; \
            return rStream; \
        } \
    }

#define SFX_ITEMPOOL_VER_MAJOR          sal_uInt8(2)
#define SFX_ITEMPOOL_VER_MINOR          sal_uInt8(0)

#define SFX_ITEMPOOL_TAG_STARTPOOL_4    sal_uInt16(0x1111)
#define SFX_ITEMPOOL_TAG_STARTPOOL_5    sal_uInt16(0xBBBB)
#define SFX_ITEMPOOL_TAG_ITEMPOOL       sal_uInt16(0xAAAA)
#define SFX_ITEMPOOL_TAG_ITEMS          sal_uInt16(0x2222)
#define SFX_ITEMPOOL_TAG_ITEM           sal_uInt16(0x7777)
#define SFX_ITEMPOOL_TAG_SIZES          sal_uInt16(0x3333)
#define SFX_ITEMPOOL_TAG_DEFAULTS       sal_uInt16(0x4444)
#define SFX_ITEMPOOL_TAG_VERSIONMAP     sal_uInt16(0x5555)
#define SFX_ITEMPOOL_TAG_HEADER         sal_uInt16(0x6666)
#define SFX_ITEMPOOL_TAG_ENDPOOL        sal_uInt16(0xEEEE)
#define SFX_ITEMPOOL_TAG_TRICK4OLD      sal_uInt16(0xFFFF)

#define SFX_ITEMPOOL_REC                sal_uInt8(0x01)
#define SFX_ITEMPOOL_REC_HEADER         sal_uInt8(0x10)
#define SFX_ITEMPOOL_REC_VERSIONMAP     sal_uInt16(0x0020)
#define SFX_ITEMPOOL_REC_WHICHIDS       sal_uInt16(0x0030)
#define SFX_ITEMPOOL_REC_ITEMS          sal_uInt16(0x0040)
#define SFX_ITEMPOOL_REC_DEFAULTS       sal_uInt16(0x0050)

#define SFX_ITEMSET_REC                 sal_uInt8(0x02)

#define SFX_STYLES_REC                  sal_uInt8(0x03)
#define SFX_STYLES_REC_HEADER       sal_uInt16(0x0010)
#define SFX_STYLES_REC_STYLES       sal_uInt16(0x0020)

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
