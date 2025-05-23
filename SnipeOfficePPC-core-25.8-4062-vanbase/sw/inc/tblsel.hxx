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
#ifndef _TBLSEL_HXX
#define _TBLSEL_HXX

#include <swtable.hxx>
#include <swrect.hxx>
#include "swdllapi.h"

#include <deque>
#include <boost/ptr_container/ptr_vector.hpp>
#include <o3tl/sorted_vector.hxx>

class SwCrsrShell;
class SwCursor;
class SwTableCursor;
class SwFrm;
class SwTabFrm;
class SwTableBox;
class SwTableLine;
class SwLayoutFrm;
class SwPaM;
class SwNode;
class SwTable;
class SwUndoTblMerge;
class SwCellFrm;

typedef ::std::deque< SwCellFrm* > SwCellFrms;

struct CompareSwSelBoxes
{
    bool operator()(SwTableBox* const& lhs, SwTableBox* const& rhs) const
    {
        return lhs->GetSttIdx() < rhs->GetSttIdx();
    }
};
class SwSelBoxes : public o3tl::sorted_vector<SwTableBox*, CompareSwSelBoxes> {};

// Collects all boxes in table that are selected.
// Selection gets extended in given direction according to enum-parameter.
// Boxes are collected via the Layout; works correctly if tables are split.
// (Cf. MakeSelUnions().)
typedef sal_uInt16 SwTblSearchType;
namespace nsSwTblSearchType
{
    const SwTblSearchType TBLSEARCH_NONE = 0x1;       // No extension.
    const SwTblSearchType TBLSEARCH_ROW  = 0x2;       // Extend to rows.
    const SwTblSearchType TBLSEARCH_COL  = 0x3;       // Extend to columns.

    // As flag to the other values!
    const SwTblSearchType TBLSEARCH_PROTECT = 0x8;      // Collect protected boxes too.
    const SwTblSearchType TBLSEARCH_NO_UNION_CORRECT = 0x10; // Do not correct collected Union.
}

SW_DLLPUBLIC void GetTblSel( const SwCrsrShell& rShell, SwSelBoxes& rBoxes,
                const SwTblSearchType = nsSwTblSearchType::TBLSEARCH_NONE );

void GetTblSel( const SwCursor& rCrsr, SwSelBoxes& rBoxes,
                const SwTblSearchType = nsSwTblSearchType::TBLSEARCH_NONE );


// As before, but don't start from selection but from Start- EndFrms.
void GetTblSel( const SwLayoutFrm* pStart, const SwLayoutFrm* pEnd,
                SwSelBoxes& rBoxes, SwCellFrms* pCells,
                const SwTblSearchType = nsSwTblSearchType::TBLSEARCH_NONE );

// As before but directly via PaMs.
void GetTblSelCrs( const SwCrsrShell& rShell, SwSelBoxes& rBoxes );
void GetTblSelCrs( const SwTableCursor& rTblCrsr, SwSelBoxes& rBoxes );

// Collect boxes relevant for auto sum.
sal_Bool GetAutoSumSel( const SwCrsrShell&, SwCellFrms& );

// Check if the SelBoxes contains protected Boxes.
sal_Bool HasProtectedCells( const SwSelBoxes& rBoxes );

// Check if selection is balanced.
sal_Bool ChkChartSel( const SwNode& rSttNd, const SwNode& rEndNd);

// Check if cell is part of SSelection.
// (Became a function, in order to make sure that GetTblSel() and MakeTblCrsr()
// have always the same concept of the selection.
sal_Bool IsFrmInTblSel( const SwRect& rUnion, const SwFrm* pCell );

// Determine boxes to be merged.
// In this process the rectangle gets "adapted" on the base of the layout,
// i.e. boxes are added if some overlap at the sides.
// Additionally a new box is created and filled with the relevant content.
void GetMergeSel( const SwPaM& rPam, SwSelBoxes& rBoxes,
                  SwTableBox** ppMergeBox, SwUndoTblMerge* pUndo = 0 );

// Check if selected boxes allow for a valid merge.
sal_uInt16 CheckMergeSel( const SwPaM& rPam );
sal_uInt16 CheckMergeSel( const SwSelBoxes& rBoxes );

sal_Bool IsEmptyBox( const SwTableBox& rBox, SwPaM& rPam );

// Check if Split or InsertCol lead to a box becoming smaller than MINLAY.
sal_Bool CheckSplitCells( const SwCrsrShell& rShell, sal_uInt16 nDiv,
                        const SwTblSearchType = nsSwTblSearchType::TBLSEARCH_NONE );
sal_Bool CheckSplitCells( const SwCursor& rCrsr, sal_uInt16 nDiv,
                        const SwTblSearchType = nsSwTblSearchType::TBLSEARCH_NONE );

// For working on tab selection also for split tables.
class SwSelUnion
{
    SwRect   aUnion;        // The rectangle enclosing the selection.
    SwTabFrm *pTable;       // The (Follow-)Table for the Union.

public:
    SwSelUnion( const SwRect &rRect, SwTabFrm *pTab ) :
        aUnion( rRect ), pTable( pTab ) {}

    const SwRect&   GetUnion() const { return aUnion; }
          SwRect&   GetUnion()       { return aUnion; }
    const SwTabFrm *GetTable() const { return pTable; }
          SwTabFrm *GetTable()       { return pTable; }
};

// Determines tables affected by a table selection and union rectangles
// of the selection (also for split tables)
typedef boost::ptr_vector<SwSelUnion> SwSelUnions;

// Gets the tables involved in a table selection and the union-rectangles of the selections
// - also for split tables.
// If a parameter is passed that != nsSwTblSearchType::TBLSEARCH_NONE
// the selection is extended in the given direction.
void MakeSelUnions( SwSelUnions&, const SwLayoutFrm *pStart,
                    const SwLayoutFrm *pEnd,
                    const SwTblSearchType = nsSwTblSearchType::TBLSEARCH_NONE );



// These classes copy the current table selections (rBoxes) into a
// separate structure while keeping the table structure.

class _FndBox;
class _FndLine;

typedef boost::ptr_vector<_FndBox> _FndBoxes;
typedef boost::ptr_vector<_FndLine> _FndLines;

class _FndBox
{
    SwTableBox* pBox;
    _FndLines aLines;
    _FndLine* pUpper;

    SwTableLine *pLineBefore;   // For deleting/restoring the layout.
    SwTableLine *pLineBehind;

public:
    _FndBox( SwTableBox* pB, _FndLine* pFL ) :
        pBox(pB), pUpper(pFL), pLineBefore( 0 ), pLineBehind( 0 ) {}

    const _FndLines&    GetLines() const    { return aLines; }
        _FndLines&      GetLines()          { return aLines; }
    const SwTableBox*   GetBox() const      { return pBox; }
        SwTableBox*     GetBox()            { return pBox; }
    const _FndLine*     GetUpper() const    { return pUpper; }
        _FndLine*       GetUpper()          { return pUpper; }

    void SetTableLines( const SwSelBoxes &rBoxes, const SwTable &rTable );
    void SetTableLines( const SwTable &rTable );
    void DelFrms ( SwTable &rTable );
    void MakeFrms( SwTable &rTable );
    void MakeNewFrms( SwTable &rTable, const sal_uInt16 nNumber,
                                       const sal_Bool bBehind );
    sal_Bool AreLinesToRestore( const SwTable &rTable ) const;

    void ClearLineBehind() { pLineBehind = 0; }
};


class _FndLine
{
    SwTableLine* pLine;
    _FndBoxes aBoxes;
    _FndBox* pUpper;
public:
    _FndLine(SwTableLine* pL, _FndBox* pFB=0) : pLine(pL), pUpper(pFB) {}
    const _FndBoxes&    GetBoxes() const    { return aBoxes; }
        _FndBoxes&      GetBoxes()          { return aBoxes; }
    const SwTableLine*  GetLine() const     { return pLine; }
        SwTableLine*    GetLine()           { return pLine; }
    const _FndBox*      GetUpper() const    { return pUpper; }
        _FndBox*        GetUpper()          { return pUpper; }

    void SetUpper( _FndBox* pUp ) { pUpper = pUp; }
};


struct _FndPara
{
    const SwSelBoxes& rBoxes;
    _FndLine* pFndLine;
    _FndBox* pFndBox;

    _FndPara( const SwSelBoxes& rBxs, _FndBox* pFB )
        : rBoxes(rBxs), pFndLine(0), pFndBox(pFB) {}
    _FndPara( const _FndPara& rPara, _FndBox* pFB )
        : rBoxes(rPara.rBoxes), pFndLine(rPara.pFndLine), pFndBox(pFB) {}
    _FndPara( const _FndPara& rPara, _FndLine* pFL )
        : rBoxes(rPara.rBoxes), pFndLine(pFL), pFndBox(rPara.pFndBox) {}
};

SW_DLLPUBLIC void ForEach_FndLineCopyCol(SwTableLines& rLines, _FndPara* pFndPara );



#endif  //  _TBLSEL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
