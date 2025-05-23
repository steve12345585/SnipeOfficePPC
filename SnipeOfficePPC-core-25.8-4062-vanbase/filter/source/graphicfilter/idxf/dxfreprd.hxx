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

#ifndef _DXFREPRD_HXX
#define _DXFREPRD_HXX

#include <dxfblkrd.hxx>
#include <dxftblrd.hxx>


//----------------------------------------------------------------------------
//--------------------Other stuff---------------------------------------------
//----------------------------------------------------------------------------

//-------------------A 3D-Min/Max-Box-----------------------------------------

class DXFBoundingBox {
public:
    sal_Bool bEmpty;
    double fMinX;
    double fMinY;
    double fMinZ;
    double fMaxX;
    double fMaxY;
    double fMaxZ;

    DXFBoundingBox() { bEmpty=sal_True; }
    void Union(const DXFVector & rVector);
};


//-------------------The (constant) palette for DXF-------------------------

class DXFPalette {

public:

    DXFPalette();
    ~DXFPalette();

    sal_uInt8 GetRed(sal_uInt8 nIndex) const;
    sal_uInt8 GetGreen(sal_uInt8 nIndex) const;
    sal_uInt8 GetBlue(sal_uInt8 nIndex) const;

private:
    sal_uInt8 * pRed;
    sal_uInt8 * pGreen;
    sal_uInt8 * pBlue;
    void SetColor(sal_uInt8 nIndex, sal_uInt8 nRed, sal_uInt8 nGreen, sal_uInt8 nBlue);
};


//----------------------------------------------------------------------------
//-----------------read and represent DXF file--------------------------------
//----------------------------------------------------------------------------

class DXFRepresentation {

public:

    DXFPalette aPalette;
        // The always equal DXF color palette

    DXFBoundingBox aBoundingBox;
        // is equal to the AutoCAD variables EXTMIN, EXTMAX if those exist
        // within the DXF file. Otherwise the BoundingBox gets calculated (in Read())

    DXFTables aTables;
        // the tables of the DXF file

    DXFBlocks aBlocks;
        // the blocks of the DXF file

    DXFEntities aEntities;
        // the entities (from the Entities-Section) of the DXF file

    rtl_TextEncoding mEnc;  // $DWGCODEPAGE

    double mfGlobalLineTypeScale; // $LTSCALE

    DXFRepresentation();
    ~DXFRepresentation();

        rtl_TextEncoding getTextEncoding() const;
        void setTextEncoding(rtl_TextEncoding aEnc);

        double getGlobalLineTypeScale() const;
        void setGlobalLineTypeScale(double fGlobalLineTypeScale);

    sal_Bool Read( SvStream & rIStream, sal_uInt16 nMinPercent, sal_uInt16 nMaxPercent);
        // Liesst die komplette DXF-Datei ein.

private:

    void ReadHeader(DXFGroupReader & rDGR);
    void CalcBoundingBox(const DXFEntities & rEntities,
                         DXFBoundingBox & rBox);
};

//----------------------------------------------------------------------------
//-------------------inlines--------------------------------------------------
//----------------------------------------------------------------------------

inline sal_uInt8 DXFPalette::GetRed(sal_uInt8 nIndex) const { return pRed[nIndex]; }
inline sal_uInt8 DXFPalette::GetGreen(sal_uInt8 nIndex) const { return pGreen[nIndex]; }
inline sal_uInt8 DXFPalette::GetBlue(sal_uInt8 nIndex) const { return pBlue[nIndex]; }
inline rtl_TextEncoding DXFRepresentation::getTextEncoding() const { return mEnc; }
inline void DXFRepresentation::setTextEncoding(rtl_TextEncoding aEnc) { mEnc = aEnc; }
inline double DXFRepresentation::getGlobalLineTypeScale() const { return mfGlobalLineTypeScale; }
inline void DXFRepresentation::setGlobalLineTypeScale(double fGlobalLineTypeScale) { mfGlobalLineTypeScale = fGlobalLineTypeScale; }

#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
