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
#ifndef _NDNOTXT_HXX
#define _NDNOTXT_HXX

#include "node.hxx"

class PolyPolygon;

// SwNoTxtNode

class SW_DLLPUBLIC SwNoTxtNode : public SwCntntNode
{
    friend class SwNodes;
    friend class SwNoTxtFrm;

    PolyPolygon *pContour;
    sal_Bool bAutomaticContour : 1; // automatic contour polygon, not manipulated
    sal_Bool bContourMapModeValid : 1; // contour map mode is not the graphics's
                                   // prefered map mode, but either
                                      // MM100 or or pixel
    sal_Bool bPixelContour : 1;     // contour map mode is invalid and pixel.

    // Creates for all derivations an AttrSet with ranges for frame- and
    // graphics-attributes (only called by SwCntntNode).
    virtual void NewAttrSet( SwAttrPool& );

    SwNoTxtNode( const SwNoTxtNode& );              // Not allowed.
    SwNoTxtNode &operator=( const SwNoTxtNode& );   // Not allowed.

protected:
    SwNoTxtNode( const SwNodeIndex &rWhere, const sal_uInt8 nNdType,
                SwGrfFmtColl *pGrColl, SwAttrSet* pAutoAttr = 0 );

public:
    ~SwNoTxtNode();

    virtual SwCntntFrm *MakeFrm( SwFrm* );

    inline SwGrfFmtColl *GetGrfColl() const { return (SwGrfFmtColl*)GetRegisteredIn(); }

    virtual Size GetTwipSize() const = 0;

    virtual sal_Bool SavePersistentData();
    virtual sal_Bool RestorePersistentData();

    const String GetTitle() const;
    void SetTitle( const String& rTitle,
                   bool bBroadcast = false );
    const String GetDescription() const;
    void SetDescription( const String& rDescription,
                         bool bBroadcast = false );

    void               SetContour( const PolyPolygon *pPoly,
                                   sal_Bool bAutomatic = sal_False );
    const PolyPolygon *HasContour() const;
    sal_Bool               _HasContour() const { return pContour!=0; };
    void               GetContour( PolyPolygon &rPoly ) const;
    void               CreateContour();

    void               SetAutomaticContour( sal_Bool bSet ) { bAutomaticContour = bSet; }
    sal_Bool               HasAutomaticContour() const { return bAutomaticContour; }

    // set either a MM100 or pixel contour
    void               SetContourAPI( const PolyPolygon *pPoly );

    // get either a MM100 or pixel contour, return sal_False if no contour is set.
    sal_Bool               GetContourAPI( PolyPolygon &rPoly ) const;

    void               SetPixelContour( sal_Bool bSet ) { bPixelContour = bSet; }
    sal_Bool               IsPixelContour() const;

    sal_Bool               IsContourMapModeValid() const { return bContourMapModeValid; }

    // Obtains the graphic with SwapIn for GrfNode via GetData for OLE.
    Graphic GetGraphic() const;
};

// Inline methods from Node.hxx - we know TxtNode only here!!
inline SwNoTxtNode *SwNode::GetNoTxtNode()
{
    return ND_NOTXTNODE & nNodeType ? (SwNoTxtNode*)this : 0;
}
inline const SwNoTxtNode *SwNode::GetNoTxtNode() const
{
    return ND_NOTXTNODE & nNodeType ? (const SwNoTxtNode*)this : 0;
}

#endif  // _NDNOTXT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
