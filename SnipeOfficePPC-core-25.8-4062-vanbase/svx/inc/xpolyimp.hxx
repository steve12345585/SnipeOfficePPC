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

#ifndef _XPOLYIMP_HXX
#define _XPOLYIMP_HXX

#include <tools/gen.hxx>
#include <svx/xpoly.hxx>
#include <vector>

class ImpXPolygon
{
public:
    Point*          pPointAry;
    sal_uInt8*           pFlagAry;
    Point*          pOldPointAry;
    sal_Bool            bDeleteOldPoints;
    sal_uInt16          nSize;
    sal_uInt16          nResize;
    sal_uInt16          nPoints;
    sal_uInt16          nRefCount;

    ImpXPolygon( sal_uInt16 nInitSize = 16, sal_uInt16 nResize=16 );
    ImpXPolygon( const ImpXPolygon& rImpXPoly );
    ~ImpXPolygon();

    bool operator==(const ImpXPolygon& rImpXPoly) const;
    bool operator!=(const ImpXPolygon& rImpXPoly) const { return !operator==(rImpXPoly); }

    void CheckPointDelete()
    {
        if ( bDeleteOldPoints )
        {
            delete[] (char*)pOldPointAry;
            bDeleteOldPoints = sal_False;
        }
    }

    void Resize( sal_uInt16 nNewSize, sal_Bool bDeletePoints = sal_True );
    void InsertSpace( sal_uInt16 nPos, sal_uInt16 nCount );
    void Remove( sal_uInt16 nPos, sal_uInt16 nCount );
};

typedef ::std::vector< XPolygon* > XPolygonList;

class ImpXPolyPolygon
{
public:
    XPolygonList aXPolyList;
    sal_uInt16       nRefCount;

                ImpXPolyPolygon() { nRefCount = 1; }
                ImpXPolyPolygon( const ImpXPolyPolygon& rImpXPolyPoly );
                ~ImpXPolyPolygon();

    bool operator==(const ImpXPolyPolygon& rImpXPolyPoly) const;
    bool operator!=(const ImpXPolyPolygon& rImpXPolyPoly) const { return !operator==(rImpXPolyPoly); }
};



#endif      // _XPOLYIMP_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
