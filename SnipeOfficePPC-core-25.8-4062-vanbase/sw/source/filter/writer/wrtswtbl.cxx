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

#include <hintids.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/brshitem.hxx>
#include <tools/fract.hxx>
#include <wrtswtbl.hxx>
#include <swtable.hxx>
#include <frmfmt.hxx>
#include <fmtfsize.hxx>
#include <fmtornt.hxx>
#include <frmatr.hxx>
#include <htmltbl.hxx>

using ::editeng::SvxBorderLine;
using namespace ::com::sun::star;

//-----------------------------------------------------------------------

sal_Int16 SwWriteTableCell::GetVertOri() const
{
    sal_Int16 eCellVertOri = text::VertOrientation::TOP;
    if( pBox->GetSttNd() )
    {
        const SfxItemSet& rItemSet = pBox->GetFrmFmt()->GetAttrSet();
        const SfxPoolItem *pItem;
        if( SFX_ITEM_SET == rItemSet.GetItemState( RES_VERT_ORIENT, sal_False, &pItem ) )
        {
            sal_Int16 eBoxVertOri =
                ((const SwFmtVertOrient *)pItem)->GetVertOrient();
            if( text::VertOrientation::CENTER==eBoxVertOri || text::VertOrientation::BOTTOM==eBoxVertOri)
                eCellVertOri = eBoxVertOri;
        }
    }

    return eCellVertOri;
}

//-----------------------------------------------------------------------

SwWriteTableRow::SwWriteTableRow( long nPosition, sal_Bool bUseLayoutHeights )
    : pBackground(0), nPos(nPosition), mbUseLayoutHeights(bUseLayoutHeights),
    nTopBorder(USHRT_MAX), nBottomBorder(USHRT_MAX), bTopBorder(true),
    bBottomBorder(true)
{
}

SwWriteTableCell *SwWriteTableRow::AddCell( const SwTableBox *pBox,
                                sal_uInt16 nRow, sal_uInt16 nCol,
                                sal_uInt16 nRowSpan, sal_uInt16 nColSpan,
                                long nHeight,
                                const SvxBrushItem *pBackgroundBrush )
{
    SwWriteTableCell *pCell =
        new SwWriteTableCell( pBox, nRow, nCol, nRowSpan, nColSpan,
                                nHeight, pBackgroundBrush );
    aCells.push_back( pCell );

    return pCell;
}

//-----------------------------------------------------------------------

SwWriteTableCol::SwWriteTableCol(sal_uInt32 nPosition)
    : nPos(nPosition), nWidthOpt(0), bRelWidthOpt(false), bOutWidth(true),
    bLeftBorder(true), bRightBorder(true)
{
}

//-----------------------------------------------------------------------

sal_uInt32 SwWriteTable::GetBoxWidth( const SwTableBox *pBox )
{
    const SwFrmFmt *pFmt = pBox->GetFrmFmt();
    const SwFmtFrmSize& aFrmSize=
        (const SwFmtFrmSize&)pFmt->GetFmtAttr( RES_FRM_SIZE );

    return sal::static_int_cast<sal_uInt32>(aFrmSize.GetSize().Width());
}

long SwWriteTable::GetLineHeight( const SwTableLine *pLine )
{
#ifdef DBG_UTIL
    sal_Bool bOldGetLineHeightCalled = m_bGetLineHeightCalled;
    m_bGetLineHeightCalled = sal_True;
#endif

    long nHeight = 0;
    if( bUseLayoutHeights )
    {
        // Erstmal versuchen wir die Hoehe ueber das Layout zu bekommen
        bool bLayoutAvailable = false;
        nHeight = pLine->GetTableLineHeight(bLayoutAvailable);
        if( nHeight > 0 )
            return nHeight;

        // Wenn kein Layout gefunden wurde, gehen wir von festen Hoehen aus.
        // #i60390# - in some cases we still want to continue
        // to use the layout heights even if one of the rows has a height of 0
        // ('hidden' rows)
        bUseLayoutHeights = bLayoutAvailable; /*sal_False;*/

#ifdef DBG_UTIL
        SAL_WARN_IF( !bLayoutAvailable && bOldGetLineHeightCalled, "sw", "Layout ungueltig?" );
#endif
    }

    const SwTableBoxes& rBoxes = pLine->GetTabBoxes();
    sal_uInt16 nBoxes = rBoxes.size();

    for( sal_uInt16 nBox=0; nBox<nBoxes; nBox++ )
    {
        const SwTableBox* pBox = rBoxes[nBox];
        if( pBox->GetSttNd() )
        {
            if( nHeight < ROW_DFLT_HEIGHT )
                nHeight = ROW_DFLT_HEIGHT;
        }
        else
        {
            long nTmp = 0;
            const SwTableLines &rLines = pBox->GetTabLines();
            for( sal_uInt16 nLine=0; nLine<rLines.size(); nLine++ )
            {
                nTmp += GetLineHeight( rLines[nLine] );
            }
            if( nHeight < nTmp )
                nHeight = nTmp;
        }
    }

    return nHeight;
}

long SwWriteTable::GetLineHeight( const SwTableBox *pBox ) const
{
    const SwTableLine *pLine = pBox->GetUpper();

    if( !pLine )
        return 0;

    const SwFrmFmt *pLineFrmFmt = pLine->GetFrmFmt();
    const SfxPoolItem* pItem;
    const SfxItemSet& rItemSet = pLineFrmFmt->GetAttrSet();

    long nHeight = 0;
    if( SFX_ITEM_SET == rItemSet.GetItemState( RES_FRM_SIZE, sal_True, &pItem ))
        nHeight = ((SwFmtFrmSize*)pItem)->GetHeight();

    return nHeight;
}

const SvxBrushItem *SwWriteTable::GetLineBrush( const SwTableBox *pBox,
                                                  SwWriteTableRow *pRow )
{
    const SwTableLine *pLine = pBox->GetUpper();

    while( pLine )
    {
        const SwFrmFmt *pLineFrmFmt = pLine->GetFrmFmt();
        const SfxPoolItem* pItem;
        const SfxItemSet& rItemSet = pLineFrmFmt->GetAttrSet();

        if( SFX_ITEM_SET == rItemSet.GetItemState( RES_BACKGROUND, sal_False,
                                                   &pItem ) )
        {
            if( !pLine->GetUpper() )
            {
                if( !pRow->GetBackground() )
                    pRow->SetBackground( (const SvxBrushItem *)pItem );
                pItem = 0;
            }

            return (const SvxBrushItem *)pItem;
        }

        pBox = pLine->GetUpper();
        pLine = pBox ? pBox->GetUpper() : 0;
    }

    return 0;
}


void SwWriteTable::MergeBorders( const SvxBorderLine* pBorderLine,
                                   bool bTable )
{
    if( (sal_uInt32)-1 == nBorderColor )
    {
        Color aGrayColor( COL_GRAY );
        if( !pBorderLine->GetColor().IsRGBEqual( aGrayColor ) )
            nBorderColor = pBorderLine->GetColor().GetColor();
    }

    if( !bCollectBorderWidth )
        return;

    sal_uInt16 nOutWidth = pBorderLine->GetOutWidth();
    if( bTable )
    {
        if( nOutWidth && (!nBorder || nOutWidth < nBorder) )
            nBorder = nOutWidth;
    }
    else
    {
        if( nOutWidth && (!nInnerBorder || nOutWidth < nInnerBorder) )
            nInnerBorder = nOutWidth;
    }

    sal_uInt16 nDist = pBorderLine->GetInWidth() ? pBorderLine->GetDistance()
                                                : 0;
    if( nDist && (!nCellSpacing || nDist < nCellSpacing) )
        nCellSpacing = nDist;
}


sal_uInt16 SwWriteTable::MergeBoxBorders( const SwTableBox *pBox,
                                        size_t const nRow, size_t const nCol,
                                        sal_uInt16 nRowSpan, sal_uInt16 nColSpan,
                                        sal_uInt16& rTopBorder,
                                        sal_uInt16 &rBottomBorder )
{
    sal_uInt16 nBorderMask = 0;

    const SwFrmFmt *pFrmFmt = pBox->GetFrmFmt();
    const SvxBoxItem& rBoxItem = (const SvxBoxItem&)pFrmFmt->GetFmtAttr( RES_BOX );

    if( rBoxItem.GetTop() )
    {
        nBorderMask |= 1;
        MergeBorders( rBoxItem.GetTop(), nRow==0 );
        rTopBorder = rBoxItem.GetTop()->GetOutWidth();
    }

    if( rBoxItem.GetLeft() )
    {
        nBorderMask |= 4;
        MergeBorders( rBoxItem.GetLeft(), nCol==0 );
    }

    if( rBoxItem.GetBottom() )
    {
        nBorderMask |= 2;
        MergeBorders( rBoxItem.GetBottom(), nRow+nRowSpan==aRows.size() );
        rBottomBorder = rBoxItem.GetBottom()->GetOutWidth();
    }

    if( rBoxItem.GetRight() )
    {
        nBorderMask |= 8;
        MergeBorders( rBoxItem.GetRight(), nCol+nColSpan==aCols.size() );
    }

    // If any distance is set, the smallest one is used. This holds for
    // the four distance of a box as well as for the distances of different
    // boxes.
    if( bCollectBorderWidth )
    {
        sal_uInt16 nDist = rBoxItem.GetDistance( BOX_LINE_TOP );
        if( nDist && (!nCellPadding || nDist < nCellPadding) )
            nCellPadding = nDist;
        nDist = rBoxItem.GetDistance( BOX_LINE_BOTTOM );
        if( nDist && (!nCellPadding || nDist < nCellPadding) )
            nCellPadding = nDist;
        nDist = rBoxItem.GetDistance( BOX_LINE_LEFT );
        if( nDist && (!nCellPadding || nDist < nCellPadding) )
            nCellPadding = nDist;
        nDist = rBoxItem.GetDistance( BOX_LINE_RIGHT );
        if( nDist && (!nCellPadding || nDist < nCellPadding) )
            nCellPadding = nDist;
    }

    return nBorderMask;
}


sal_uInt32  SwWriteTable::GetRawWidth( sal_uInt16 nCol, sal_uInt16 nColSpan ) const
{
    sal_uInt32 nWidth = aCols[nCol+nColSpan-1]->GetPos();
    if( nCol > 0 )
        nWidth = nWidth - aCols[nCol-1]->GetPos();

    return nWidth;
}

sal_uInt16 SwWriteTable::GetLeftSpace( sal_uInt16 nCol ) const
{
    sal_uInt16 nSpace = nCellPadding + nCellSpacing;

    // In der ersten Spalte auch noch die Liniendicke abziehen
    if( nCol==0 )
    {
        nSpace = nSpace + nLeftSub;

        const SwWriteTableCol *pCol = aCols[nCol];
        if( pCol->HasLeftBorder() )
            nSpace = nSpace + nBorder;
    }

    return nSpace;
}

sal_uInt16
SwWriteTable::GetRightSpace(size_t const nCol, sal_uInt16 nColSpan) const
{
    sal_uInt16 nSpace = nCellPadding;

    // In der letzten Spalte noch einmal zusaetzlich CELLSPACING und
    // und die Liniendicke abziehen
    if( nCol+nColSpan==aCols.size() )
    {
        nSpace += (nCellSpacing + nRightSub);

        const SwWriteTableCol *pCol = aCols[nCol+nColSpan-1];
        if( pCol->HasRightBorder() )
            nSpace = nSpace + nBorder;
    }

    return nSpace;
}

sal_uInt16 SwWriteTable::GetAbsWidth( sal_uInt16 nCol, sal_uInt16 nColSpan ) const
{
    sal_uInt32 nWidth = GetRawWidth( nCol, nColSpan );
    if( nBaseWidth != nTabWidth )
    {
        nWidth *= nTabWidth;
        nWidth /= nBaseWidth;
    }

    nWidth -= GetLeftSpace( nCol ) + GetRightSpace( nCol, nColSpan );

    OSL_ENSURE( nWidth > 0, "Spaltenbreite <= 0. OK?" );
    return nWidth > 0 ? (sal_uInt16)nWidth : 0;
}

sal_uInt16 SwWriteTable::GetRelWidth( sal_uInt16 nCol, sal_uInt16 nColSpan ) const
{
    long nWidth = GetRawWidth( nCol, nColSpan );

    return (sal_uInt16)(long)Fraction( nWidth*256 + GetBaseWidth()/2,
                                   GetBaseWidth() );
}

sal_uInt16 SwWriteTable::GetPrcWidth( sal_uInt16 nCol, sal_uInt16 nColSpan ) const
{
    long nWidth = GetRawWidth( nCol, nColSpan );

    // sieht komisch aus, ist aber nichts anderes als
    //  [(100 * nWidth) + .5] ohne Rundungsfehler
    return (sal_uInt16)(long)Fraction( nWidth*100 + GetBaseWidth()/2,
                                   GetBaseWidth() );
}

long SwWriteTable::GetAbsHeight(long nRawHeight, size_t const nRow,
                                   sal_uInt16 nRowSpan ) const
{
    nRawHeight -= (2*nCellPadding + nCellSpacing);

    // In der ersten Zeile noch einmal zusaetzlich CELLSPACING und
    // und die Liniendicke abziehen
    const SwWriteTableRow *pRow = 0;
    if( nRow==0 )
    {
        nRawHeight -= nCellSpacing;
        pRow = aRows[nRow];
        if( pRow->HasTopBorder() )
            nRawHeight -= nBorder;
    }

    // In der letzten Zeile noch die Liniendicke abziehen
    if( nRow+nRowSpan==aRows.size() )
    {
        if( !pRow || nRowSpan > 1 )
            pRow = aRows[nRow+nRowSpan-1];
        if( pRow->HasBottomBorder() )
            nRawHeight -= nBorder;
    }

    OSL_ENSURE( nRawHeight > 0, "Zeilenheohe <= 0. OK?" );
    return nRawHeight > 0 ? nRawHeight : 0;
}

sal_Bool SwWriteTable::ShouldExpandSub(const SwTableBox *pBox, sal_Bool /*bExpandedBefore*/,
    sal_uInt16 nDepth) const
{
    return !pBox->GetSttNd() && nDepth > 0;
}

// FIXME: the degree of coupling between this method and
// FillTableRowsCols which is called immediately afterwards
// is -extremely- unpleasant and potentially problematic.

void SwWriteTable::CollectTableRowsCols( long nStartRPos,
                                           sal_uInt32 nStartCPos,
                                           long nParentLineHeight,
                                           sal_uInt32 nParentLineWidth,
                                           const SwTableLines& rLines,
                                           sal_uInt16 nDepth )
{
    sal_Bool bSubExpanded = sal_False;
    sal_uInt16 nLines = rLines.size();

#if OSL_DEBUG_LEVEL > 0
    sal_uInt32 nEndCPos = 0;
#endif

    long nRPos = nStartRPos;
    for( sal_uInt16 nLine = 0; nLine < nLines; nLine++ )
    {
        /*const*/ SwTableLine *pLine = rLines[nLine];

        long nOldRPos = nRPos;

        if( nLine < nLines-1 || nParentLineHeight==0  )
        {
            long nLineHeight = GetLineHeight( pLine );
            nRPos += nLineHeight;
            if( nParentLineHeight && nStartRPos + nParentLineHeight <= nRPos )
            {
                /* If you have corrupt line height information, e.g. breaking rows in complex table
                layout, you may run into this robust code.
                It's not allowed that subrows leaves their parentrow. If this would happen the line
                height of subrow is reduced to a part of the remaining height */
                OSL_FAIL( "Corrupt line height I" );
                nRPos -= nLineHeight;
                nLineHeight = nStartRPos + nParentLineHeight - nRPos; // remaining parent height
                nLineHeight /= nLines - nLine; // divided through the number of remaining sub rows
                nRPos += nLineHeight;
            }
            SwWriteTableRow *pRow = new SwWriteTableRow( nRPos, bUseLayoutHeights);
            if( !aRows.insert( pRow ).second )
                delete pRow;
        }
        else
        {
#if OSL_DEBUG_LEVEL > 0
            long nCheckPos = nRPos + GetLineHeight( pLine );
#endif
            nRPos = nStartRPos + nParentLineHeight;
#if OSL_DEBUG_LEVEL > 0
            SwWriteTableRow aSrchRow( nRPos, bUseLayoutHeights );
            OSL_ENSURE( aRows.find( &aSrchRow ) != aRows.end(), "Parent-Zeile nicht gefunden" );
            SwWriteTableRow aRowCheckPos(nCheckPos,bUseLayoutHeights);
            SwWriteTableRow aRowRPos(nRPos,bUseLayoutHeights);
            OSL_ENSURE( !bUseLayoutHeights ||
                    aRowCheckPos == aRowRPos,
                    "Hoehe der Zeilen stimmt nicht mit Parent ueberein" );
#endif
        }

        // Fuer alle Boxen der Zeile ggf. eine Spalte einfuegen
        const SwTableBoxes& rBoxes = pLine->GetTabBoxes();
        sal_uInt16 nBoxes = rBoxes.size();

        sal_uInt32 nCPos = nStartCPos;
        for( sal_uInt16 nBox=0; nBox<nBoxes; nBox++ )
        {
            const SwTableBox *pBox = rBoxes[nBox];

            sal_uInt32 nOldCPos = nCPos;

            if( nBox < nBoxes-1 || (nParentLineWidth==0 && nLine==0)  )
            {
                nCPos = nCPos + GetBoxWidth( pBox );
                SwWriteTableCol *pCol = new SwWriteTableCol( nCPos );

                if( !aCols.insert( pCol ).second )
                    delete pCol;

                if( nBox==nBoxes-1 )
                {
                    OSL_ENSURE( nLine==0 && nParentLineWidth==0,
                            "Jetzt wird die Parent-Breite plattgemacht!" );
                    nParentLineWidth = nCPos-nStartCPos;
                }
            }
            else
            {
#if OSL_DEBUG_LEVEL > 0
                sal_uInt32 nCheckPos = nCPos + GetBoxWidth( pBox );
                if( !nEndCPos )
                {
                    nEndCPos = nCheckPos;
                }
                else
                {
                    OSL_ENSURE( SwWriteTableCol(nCheckPos) ==
                                                SwWriteTableCol(nEndCPos),
                    "Zelle enthaelt unterschiedlich breite Zeilen" );
                }
#endif
                nCPos = nStartCPos + nParentLineWidth;

#if OSL_DEBUG_LEVEL > 0
                SwWriteTableCol aSrchCol( nCPos );
                OSL_ENSURE( aCols.find( &aSrchCol ) != aCols.end(),
                        "Parent-Zelle nicht gefunden" );
                OSL_ENSURE( SwWriteTableCol(nCheckPos) ==
                                            SwWriteTableCol(nCPos),
                        "Breite der Zellen stimmt nicht mit Parent ueberein" );
#endif
            }

            if( ShouldExpandSub( pBox, bSubExpanded, nDepth ) )
            {
                CollectTableRowsCols( nOldRPos, nOldCPos,
                                        nRPos - nOldRPos,
                                        nCPos - nOldCPos,
                                        pBox->GetTabLines(),
                                        nDepth-1 );
                bSubExpanded = sal_True;
            }
        }
    }
}


void SwWriteTable::FillTableRowsCols( long nStartRPos, sal_uInt16 nStartRow,
                                        sal_uInt32 nStartCPos, sal_uInt16 nStartCol,
                                        long nParentLineHeight,
                                        sal_uInt32 nParentLineWidth,
                                        const SwTableLines& rLines,
                                        const SvxBrushItem* pParentBrush,
                                        sal_uInt16 nDepth,
                                        sal_uInt16 nNumOfHeaderRows )
{
    sal_uInt16 nLines = rLines.size();
    sal_Bool bSubExpanded = sal_False;

    // Festlegen der Umrandung
    long nRPos = nStartRPos;
    sal_uInt16 nRow = nStartRow;

    for( sal_uInt16 nLine = 0; nLine < nLines; nLine++ )
    {
        const SwTableLine *pLine = rLines[nLine];

        // Position der letzten ueberdeckten Zeile ermitteln
        long nOldRPos = nRPos;
        if( nLine < nLines-1 || nParentLineHeight==0 )
        {
            long nLineHeight = GetLineHeight( pLine );
            nRPos += nLineHeight;
            if( nParentLineHeight && nStartRPos + nParentLineHeight <= nRPos )
            {
                /* See comment in CollectTableRowCols */
                OSL_FAIL( "Corrupt line height II" );
                nRPos -= nLineHeight;
                nLineHeight = nStartRPos + nParentLineHeight - nRPos; // remaining parent height
                nLineHeight /= nLines - nLine; // divided through the number of remaining sub rows
                nRPos += nLineHeight;
            }
        }
        else
            nRPos = nStartRPos + nParentLineHeight;

        // Und ihren Index
        sal_uInt16 nOldRow = nRow;
        SwWriteTableRow aSrchRow( nRPos,bUseLayoutHeights );
        SwWriteTableRows::const_iterator it2 = aRows.find( &aSrchRow );
        OSL_ENSURE( it2 != aRows.end(), "missing row" );
        nRow = it2 - aRows.begin();

        OSL_ENSURE( nOldRow <= nRow, "Don't look back!" );
        if( nOldRow > nRow )
        {
            nOldRow = nRow;
            if( nOldRow )
                --nOldRow;
        }


        SwWriteTableRow *pRow = aRows[nOldRow];
        SwWriteTableRow *pEndRow = aRows[nRow];
        if( nLine+1==nNumOfHeaderRows && nParentLineHeight==0 )
            nHeadEndRow = nRow;

        const SwTableBoxes& rBoxes = pLine->GetTabBoxes();

        const SwFrmFmt *pLineFrmFmt = pLine->GetFrmFmt();
        const SfxPoolItem* pItem;
        const SfxItemSet& rItemSet = pLineFrmFmt->GetAttrSet();

        long nHeight = 0;
        if( SFX_ITEM_SET == rItemSet.GetItemState( RES_FRM_SIZE, sal_True, &pItem ))
            nHeight = ((SwFmtFrmSize*)pItem)->GetHeight();


        const SvxBrushItem *pBrushItem, *pLineBrush = pParentBrush;
        if( SFX_ITEM_SET == rItemSet.GetItemState( RES_BACKGROUND, sal_False,
                                                   &pItem ) )
        {
            pLineBrush = (const SvxBrushItem *)pItem;

            // Wenn die Zeile die gesamte Tabelle umspannt, koennen
            // Wir den Hintergrund an der Zeile ausgeben. Sonst muessen
            // wir in an den Zelle ausgeben.
            sal_Bool bOutAtRow = !nParentLineWidth;
            if( !bOutAtRow && nStartCPos==0 )
            {
                SwWriteTableCol aCol( nParentLineWidth );
                bOutAtRow = aCols.find( &aCol ) == (aCols.end() - 1);
            }
            if( bOutAtRow )
            {
                pRow->SetBackground( pLineBrush );
                pBrushItem = 0;
            }
            else
                pBrushItem = pLineBrush;
        }
        else
        {
            pRow->SetBackground( pLineBrush );
            pBrushItem = 0;
        }

        sal_uInt16 nBoxes = rBoxes.size();
        sal_uInt32 nCPos = nStartCPos;
        sal_uInt16 nCol = nStartCol;

        for( sal_uInt16 nBox=0; nBox<nBoxes; nBox++ )
        {
            const SwTableBox *pBox = rBoxes[nBox];

            // Position der letzten ueberdeckten Spalte ermitteln
            sal_uInt32 nOldCPos = nCPos;
            if( nBox < nBoxes-1 || (nParentLineWidth==0 && nLine==0) )
            {
                nCPos = nCPos + GetBoxWidth( pBox );
                if( nBox==nBoxes-1 )
                    nParentLineWidth = nCPos - nStartCPos;
            }
            else
                nCPos = nStartCPos + nParentLineWidth;

            // Und ihren Index
            sal_uInt16 nOldCol = nCol;
            SwWriteTableCol aSrchCol( nCPos );
            SwWriteTableCols::const_iterator it = aCols.find( &aSrchCol );
            OSL_ENSURE( it != aCols.end(), "missing column" );
            nCol = it - aCols.begin();

            if( !ShouldExpandSub( pBox, bSubExpanded, nDepth ) )
            {
                sal_uInt16 nRowSpan = nRow - nOldRow + 1;

                // The new table model may have true row span attributes
                const long nAttrRowSpan = pBox->getRowSpan();
                if ( 1 < nAttrRowSpan )
                    nRowSpan = (sal_uInt16)nAttrRowSpan;
                else if ( nAttrRowSpan < 1 )
                    nRowSpan = 0;

                sal_uInt16 nColSpan = nCol - nOldCol + 1;
                pRow->AddCell( pBox, nOldRow, nOldCol,
                               nRowSpan, nColSpan, nHeight,
                               pBrushItem );
                nHeight = 0; // Die Hoehe braucht nur einmal geschieben werden

                if( pBox->GetSttNd() )
                {
                    sal_uInt16 nTopBorder = USHRT_MAX, nBottomBorder = USHRT_MAX;
                    sal_uInt16 nBorderMask = MergeBoxBorders(pBox, nOldRow, nOldCol,
                        nRowSpan, nColSpan, nTopBorder, nBottomBorder);

                    // #i30094# add a sanity check here to ensure that
                    // we don't access an invalid aCols[] as &nCol
                    // above can be changed.
                    if (!(nBorderMask & 4) && nOldCol < aCols.size())
                    {
                        SwWriteTableCol *pCol = aCols[nOldCol];
                        OSL_ENSURE(pCol, "No TableCol found, panic!");
                        if (pCol)
                            pCol->bLeftBorder = false;
                    }

                    if (!(nBorderMask & 8))
                    {
                        SwWriteTableCol *pCol = aCols[nCol];
                        OSL_ENSURE(pCol, "No TableCol found, panic!");
                        if (pCol)
                            pCol->bRightBorder = false;
                    }

                    if (!(nBorderMask & 1))
                        pRow->bTopBorder = false;
                    else if (!pRow->nTopBorder || nTopBorder < pRow->nTopBorder)
                        pRow->nTopBorder = nTopBorder;

                    if (!(nBorderMask & 2))
                        pEndRow->bBottomBorder = false;
                    else if (
                                !pEndRow->nBottomBorder ||
                                nBottomBorder < pEndRow->nBottomBorder
                            )
                    {
                        pEndRow->nBottomBorder = nBottomBorder;
                    }
                }
            }
            else
            {
                FillTableRowsCols( nOldRPos, nOldRow, nOldCPos, nOldCol,
                                    nRPos-nOldRPos, nCPos-nOldCPos,
                                    pBox->GetTabLines(),
                                    pLineBrush, nDepth-1,
                                    nNumOfHeaderRows );
                bSubExpanded = sal_True;
            }

            nCol++; // Die naechste Zelle faengt in der nachten Spalte an
        }

        nRow++;
    }
}

SwWriteTable::SwWriteTable(const SwTableLines& rLines, long nWidth,
    sal_uInt32 nBWidth, bool bRel, sal_uInt16 nMaxDepth, sal_uInt16 nLSub, sal_uInt16 nRSub, sal_uInt32 nNumOfRowsToRepeat)
    : nBorderColor((sal_uInt32)-1), nCellSpacing(0), nCellPadding(0), nBorder(0),
    nInnerBorder(0), nBaseWidth(nBWidth), nHeadEndRow(USHRT_MAX),
     nLeftSub(nLSub), nRightSub(nRSub), nTabWidth(nWidth), bRelWidths(bRel),
    bUseLayoutHeights(true),
#ifdef DBG_UTIL
    m_bGetLineHeightCalled(false),
#endif
    bColsOption(false), bColTags(true), bLayoutExport(false),
    bCollectBorderWidth(true)
{
    sal_uInt32 nParentWidth = nBaseWidth + nLeftSub + nRightSub;

    // Erstmal die Tabellen-Struktur festlegen. Hinter der Tabelle ist in
    // jedem Fall eine Spalte zu Ende
    SwWriteTableCol *pCol = new SwWriteTableCol( nParentWidth );
    bUseLayoutHeights = true;
    aCols.insert( pCol );
    CollectTableRowsCols( 0, 0, 0, nParentWidth, rLines, nMaxDepth - 1 );

    // FIXME: awfully GetLineHeight writes to this in its first call
    // and proceeds to return a rather odd number fdo#62336, we have to
    // behave identically since the code in FillTableRowsCols duplicates
    // and is highly coupled to CollectTableRowsCols - sadly.
    bUseLayoutHeights = true;
    // And now fill with life
    FillTableRowsCols( 0, 0, 0, 0, 0, nParentWidth, rLines, 0, nMaxDepth - 1, static_cast< sal_uInt16 >(nNumOfRowsToRepeat) );

    // Einige Twip-Werte an Pixel-Grenzen anpassen
    if( !nBorder )
        nBorder = nInnerBorder;
}

SwWriteTable::SwWriteTable( const SwHTMLTableLayout *pLayoutInfo )
    : nBorderColor((sal_uInt32)-1), nCellSpacing(0), nCellPadding(0), nBorder(0),
    nInnerBorder(0), nBaseWidth(pLayoutInfo->GetWidthOption()), nHeadEndRow(0),
    nLeftSub(0), nRightSub(0), nTabWidth(pLayoutInfo->GetWidthOption()),
    bRelWidths(pLayoutInfo->HasPrcWidthOption()), bUseLayoutHeights(false),
#ifdef DBG_UTIL
    m_bGetLineHeightCalled(false),
#endif
    bColsOption(pLayoutInfo->HasColsOption()),
    bColTags(pLayoutInfo->HasColTags()), bLayoutExport(true),
    bCollectBorderWidth(pLayoutInfo->HaveBordersChanged())
{
    if( !bCollectBorderWidth )
    {
        nBorder = pLayoutInfo->GetBorder();
        nCellPadding = pLayoutInfo->GetCellPadding();
        nCellSpacing = pLayoutInfo->GetCellSpacing();
    }

    sal_uInt16 nRow, nCol;
    sal_uInt16 nCols = pLayoutInfo->GetColCount();
    sal_uInt16 nRows = pLayoutInfo->GetRowCount();

    // Erstmal die Tabellen-Struktur festlegen.
    for( nCol=0; nCol<nCols; nCol++ )
    {
        SwWriteTableCol *pCol =
            new SwWriteTableCol( (nCol+1)*COL_DFLT_WIDTH );

        if( bColTags )
        {
            const SwHTMLTableLayoutColumn *pLayoutCol =
                pLayoutInfo->GetColumn( nCol );
            pCol->SetWidthOpt( pLayoutCol->GetWidthOption(),
                               pLayoutCol->IsRelWidthOption() );
        }

        aCols.insert( pCol );
    }

    for( nRow=0; nRow<nRows; nRow++ )
    {
        SwWriteTableRow *pRow =
            new SwWriteTableRow( (nRow+1)*ROW_DFLT_HEIGHT, bUseLayoutHeights );
        pRow->nTopBorder = 0;
        pRow->nBottomBorder = 0;
        aRows.insert( pRow );
    }

    // Und jetzt mit leben fuellen
    for( nRow=0; nRow<nRows; nRow++ )
    {
        SwWriteTableRow *pRow = aRows[nRow];

        sal_Bool bHeightExported = sal_False;
        for( nCol=0; nCol<nCols; nCol++ )
        {
            const SwHTMLTableLayoutCell *pLayoutCell =
                pLayoutInfo->GetCell( nRow, nCol );

            const SwHTMLTableLayoutCnts *pLayoutCnts =
                pLayoutCell->GetContents();

            // Beginnt die Zelle eigentlich eine Zeile weiter oben oder
            // weiter vorne?
            if( ( nRow>0 && pLayoutCnts == pLayoutInfo->GetCell(nRow-1,nCol)
                                                      ->GetContents() ) ||
                ( nCol>0 && pLayoutCnts == pLayoutInfo->GetCell(nRow,nCol-1)
                                                      ->GetContents() ) )
            {
                continue;
            }

            sal_uInt16 nRowSpan = pLayoutCell->GetRowSpan();
            sal_uInt16 nColSpan = pLayoutCell->GetColSpan();
            const SwTableBox *pBox = pLayoutCnts->GetTableBox();
            OSL_ENSURE( pBox,
                    "Tabelle in Tabelle kann nicht ueber Layout exportiert werden" );

            long nHeight = bHeightExported ? 0 : GetLineHeight( pBox );
            const SvxBrushItem *pBrushItem = GetLineBrush( pBox, pRow );

            SwWriteTableCell *pCell =
                pRow->AddCell( pBox, nRow, nCol, nRowSpan, nColSpan,
                               nHeight, pBrushItem );
            pCell->SetWidthOpt( pLayoutCell->GetWidthOption(),
                                pLayoutCell->IsPrcWidthOption() );

            sal_uInt16 nTopBorder = USHRT_MAX, nBottomBorder = USHRT_MAX;
            sal_uInt16 nBorderMask =
            MergeBoxBorders( pBox, nRow, nCol, nRowSpan, nColSpan,
                                nTopBorder, nBottomBorder );

            SwWriteTableCol *pCol = aCols[nCol];
            if( !(nBorderMask & 4) )
                pCol->bLeftBorder = false;

            pCol = aCols[nCol+nColSpan-1];
            if( !(nBorderMask & 8) )
                pCol->bRightBorder = false;

            if( !(nBorderMask & 1) )
                pRow->bTopBorder = false;

            SwWriteTableRow *pEndRow = aRows[nRow+nRowSpan-1];
            if( !(nBorderMask & 2) )
                pEndRow->bBottomBorder = false;

            // Die Hoehe braucht nur einmal geschieben werden
            if( nHeight )
                bHeightExported = sal_True;
        }
    }

    // Einige Twip-Werte an Pixel-Grenzen anpassen
    if( bCollectBorderWidth && !nBorder )
        nBorder = nInnerBorder;
}

SwWriteTable::~SwWriteTable()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
