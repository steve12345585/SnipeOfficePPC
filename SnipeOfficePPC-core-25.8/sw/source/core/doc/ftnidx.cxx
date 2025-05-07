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



#include <txtftn.hxx>
#include <fmtftn.hxx>
#include <ftninfo.hxx>
#include <doc.hxx>
#include <ftnidx.hxx>
#include <ndtxt.hxx>
#include <ndindex.hxx>
#include <section.hxx>
#include <fmtftntx.hxx>
#include <rootfrm.hxx>


_SV_IMPL_SORTAR_ALG( _SwFtnIdxs, SwTxtFtnPtr )
sal_Bool _SwFtnIdxs::Seek_Entry( const SwTxtFtnPtr rSrch, sal_uInt16* pFndPos ) const
{
    sal_uLong nIdx = _SwTxtFtn_GetIndex( rSrch );
    xub_StrLen nCntIdx = *rSrch->GetStart();

    sal_uInt16 nO = Count(), nM, nU = 0;
    if( nO > 0 )
    {
        nO--;
        while( nU <= nO )
        {
            nM = nU + ( nO - nU ) / 2;
            sal_uLong nFndIdx = _SwTxtFtn_GetIndex( (*this)[ nM ] );
            if( nFndIdx == nIdx && *(*this)[ nM ]->GetStart() == nCntIdx )
            {
                if( pFndPos )
                    *pFndPos = nM;
                return sal_True;
            }
            else if( nFndIdx < nIdx ||
                (nFndIdx == nIdx && *(*this)[ nM ]->GetStart() < nCntIdx ))
                nU = nM + 1;
            else if( nM == 0 )
            {
                if( pFndPos )
                    *pFndPos = nU;
                return sal_False;
            }
            else
                nO = nM - 1;
        }
    }
    if( pFndPos )
        *pFndPos = nU;
    return sal_False;
}


void SwFtnIdxs::UpdateFtn( const SwNodeIndex& rStt )
{
    if( !Count() )
        return;

    // Get the NodesArray using the first foot note's StartIndex
    SwDoc* pDoc = rStt.GetNode().GetDoc();
    if( pDoc->IsInReading() )
        return ;
    SwTxtFtn* pTxtFtn;

    const SwEndNoteInfo& rEndInfo = pDoc->GetEndNoteInfo();
    const SwFtnInfo& rFtnInfo = pDoc->GetFtnInfo();

    // For normal foot notes we treat per-chapter and per-document numbering
    // seperately. For Endnotes we only have per-document numbering.
    if( FTNNUM_CHAPTER == rFtnInfo.eNum )
    {
        const SwOutlineNodes& rOutlNds = pDoc->GetNodes().GetOutLineNds();
        const SwNode* pCapStt = &pDoc->GetNodes().GetEndOfExtras();
        sal_uLong nCapEnd = pDoc->GetNodes().GetEndOfContent().GetIndex();
        if( rOutlNds.Count() )
        {
            // Find the Chapter's start, which contains rStt
            sal_uInt16 n;

            for( n = 0; n < rOutlNds.Count(); ++n )
                if( rOutlNds[ n ]->GetIndex() > rStt.GetIndex() )
                    break;      // found it!
                //else if( !rOutlNds[ n ]->GetTxtNode()->GetTxtColl()->GetOutlineLevel() )  //#outline level,zhaojianwei
                else if ( rOutlNds[ n ]->GetTxtNode()->GetAttrOutlineLevel() == 1 )   //<-end,zhaojianwei
                    pCapStt = rOutlNds[ n ];    // Beginning of a new Chapter
            // now find the end of the range
            for( ; n < rOutlNds.Count(); ++n )
                //if( !rOutlNds[ n ]->GetTxtNode()->GetTxtColl()->GetOutlineLevel() )//#outline level,zhaojianwei
                if ( rOutlNds[ n ]->GetTxtNode()->GetAttrOutlineLevel() == 1 )//<-end,zhaojianwei
                {
                    nCapEnd = rOutlNds[ n ]->GetIndex();    // End of the found Chapter
                    break;
                }
        }

        sal_uInt16 nPos, nFtnNo = 1;
        if( SeekEntry( *pCapStt, &nPos ) && nPos )
        {
            // Step forward until the Index is not the same anymore
            const SwNode* pCmpNd = &rStt.GetNode();
            while( nPos && pCmpNd == &((*this)[ --nPos ]->GetTxtNode()) )
                ;
            ++nPos;
        }

        if( nPos == Count() )       // nothing found
            return;

        if( !rOutlNds.Count() )
            nFtnNo = nPos+1;

        for( ; nPos < Count(); ++nPos )
        {
            pTxtFtn = (*this)[ nPos ];
            if( pTxtFtn->GetTxtNode().GetIndex() >= nCapEnd )
                break;

            const SwFmtFtn &rFtn = pTxtFtn->GetFtn();
            if( !rFtn.GetNumStr().Len() && !rFtn.IsEndNote() &&
                !SwUpdFtnEndNtAtEnd::FindSectNdWithEndAttr( *pTxtFtn ))
                pTxtFtn->SetNumber( rFtnInfo.nFtnOffset + nFtnNo++,
                                    &rFtn.GetNumStr() );
        }
    }

    SwUpdFtnEndNtAtEnd aNumArr;

    // unless we have per-document numbering, only look at endnotes here
    const sal_Bool bEndNoteOnly = FTNNUM_DOC != rFtnInfo.eNum;

    sal_uInt16 nPos, nFtnNo = 1, nEndNo = 1;
    sal_uLong nUpdNdIdx = rStt.GetIndex();
    for( nPos = 0; nPos < Count(); ++nPos )
    {
        pTxtFtn = (*this)[ nPos ];
        if( nUpdNdIdx <= pTxtFtn->GetTxtNode().GetIndex() )
            break;

        const SwFmtFtn &rFtn = pTxtFtn->GetFtn();
        if( !rFtn.GetNumStr().Len() )
        {
            if( !aNumArr.ChkNumber( *pTxtFtn ) )
            {
                if( pTxtFtn->GetFtn().IsEndNote() )
                    nEndNo++;
                else
                    nFtnNo++;
            }
        }
    }

    // Set the array number for all footnotes starting from nPos
    for( ; nPos < Count(); ++nPos )
    {
        pTxtFtn = (*this)[ nPos ];
        const SwFmtFtn &rFtn = pTxtFtn->GetFtn();
        if( !rFtn.GetNumStr().Len() )
        {
            sal_uInt16 nSectNo = aNumArr.ChkNumber( *pTxtFtn );
            if( !nSectNo && ( rFtn.IsEndNote() || !bEndNoteOnly ))
                nSectNo = rFtn.IsEndNote()
                            ? rEndInfo.nFtnOffset + nEndNo++
                            : rFtnInfo.nFtnOffset + nFtnNo++;

            if( nSectNo )
            {
                pTxtFtn->SetNumber( nSectNo, &rFtn.GetNumStr() );
            }
        }
    }
}


void SwFtnIdxs::UpdateAllFtn()
{
    if( !Count() )
        return;

    // Get the NodesArray via the StartIndex of the first Footnote
    SwDoc* pDoc = (SwDoc*) (*this)[ 0 ]->GetTxtNode().GetDoc();
    SwTxtFtn* pTxtFtn;
    const SwEndNoteInfo& rEndInfo = pDoc->GetEndNoteInfo();
    const SwFtnInfo& rFtnInfo = pDoc->GetFtnInfo();

    SwUpdFtnEndNtAtEnd aNumArr;

    SwRootFrm* pTmpRoot = pDoc->GetCurrentLayout();//swmod 080305
    std::set<SwRootFrm*> aAllLayouts = pDoc->GetAllLayouts();
    // For normal Footnotes per-chapter and per-document numbering are treated separately.
    // For Endnotes we only have document-wise numbering.
    if( FTNNUM_CHAPTER == rFtnInfo.eNum )
    {
        const SwOutlineNodes& rOutlNds = pDoc->GetNodes().GetOutLineNds();
        sal_uInt16 nNo = 1,     // Number for the Footnotes
               nFtnIdx = 0;     // Index into theFtnIdx array
        for( sal_uInt16 n = 0; n < rOutlNds.Count(); ++n )
        {
            if ( rOutlNds[ n ]->GetTxtNode()->GetAttrOutlineLevel() == 1 )//<-end,zhaojianwei
            {
                sal_uLong nCapStt = rOutlNds[ n ]->GetIndex();  // Start of a new chapter
                for( ; nFtnIdx < Count(); ++nFtnIdx )
                {
                    pTxtFtn = (*this)[ nFtnIdx ];
                    if( pTxtFtn->GetTxtNode().GetIndex() >= nCapStt )
                        break;

                    // Endnotes are per-document only
                    const SwFmtFtn &rFtn = pTxtFtn->GetFtn();
                    if( !rFtn.IsEndNote() && !rFtn.GetNumStr().Len() &&
                        !SwUpdFtnEndNtAtEnd::FindSectNdWithEndAttr( *pTxtFtn ))
                        pTxtFtn->SetNumber( rFtnInfo.nFtnOffset + nNo++,
                                            &rFtn.GetNumStr() );
                }
                if( nFtnIdx >= Count() )
                    break;          // ok, everything is updated
                nNo = 1;
            }
        }

        for( nNo = 1; nFtnIdx < Count(); ++nFtnIdx )
        {
            // Endnotes are per-document
            pTxtFtn = (*this)[ nFtnIdx ];
            const SwFmtFtn &rFtn = pTxtFtn->GetFtn();
            if( !rFtn.IsEndNote() && !rFtn.GetNumStr().Len() &&
                !SwUpdFtnEndNtAtEnd::FindSectNdWithEndAttr( *pTxtFtn ))
                pTxtFtn->SetNumber( rFtnInfo.nFtnOffset + nNo++,
                                    &rFtn.GetNumStr() );
        }

    }

    // We use sal_Bool here, so that we also iterate through the Endnotes with a chapter setting.
    const sal_Bool bEndNoteOnly = FTNNUM_DOC != rFtnInfo.eNum;
    sal_uInt16 nFtnNo = 0, nEndNo = 0;
    for( sal_uInt16 nPos = 0; nPos < Count(); ++nPos )
    {
        pTxtFtn = (*this)[ nPos ];
        const SwFmtFtn &rFtn = pTxtFtn->GetFtn();
        if( !rFtn.GetNumStr().Len() )
        {
            sal_uInt16 nSectNo = aNumArr.ChkNumber( *pTxtFtn );
            if( !nSectNo && ( rFtn.IsEndNote() || !bEndNoteOnly ))
                nSectNo = rFtn.IsEndNote()
                                ? rEndInfo.nFtnOffset + (++nEndNo)
                                : rFtnInfo.nFtnOffset + (++nFtnNo);

            if( nSectNo )
            {
                pTxtFtn->SetNumber( nSectNo, &rFtn.GetNumStr() );
            }
        }
    }

    if( pTmpRoot && FTNNUM_PAGE == rFtnInfo.eNum )
        std::for_each( aAllLayouts.begin(), aAllLayouts.end(),std::mem_fun(&SwRootFrm::UpdateFtnNums));//swmod 0
}

SwTxtFtn* SwFtnIdxs::SeekEntry( const SwNodeIndex& rPos, sal_uInt16* pFndPos ) const
{
    sal_uLong nIdx = rPos.GetIndex();

    sal_uInt16 nO = Count(), nM, nU = 0;
    if( nO > 0 )
    {
        nO--;
        while( nU <= nO )
        {
            nM = nU + ( nO - nU ) / 2;
            sal_uLong nNdIdx = _SwTxtFtn_GetIndex( (*this)[ nM ] );
            if( nNdIdx == nIdx )
            {
                if( pFndPos )
                    *pFndPos = nM;
                return (*this)[ nM ];
            }
            else if( nNdIdx < nIdx )
                nU = nM + 1;
            else if( nM == 0 )
            {
                if( pFndPos )
                    *pFndPos = nU;
                return 0;
            }
            else
                nO = nM - 1;
        }
    }
    if( pFndPos )
        *pFndPos = nU;
    return 0;
}


const SwSectionNode* SwUpdFtnEndNtAtEnd::FindSectNdWithEndAttr(
                const SwTxtFtn& rTxtFtn )
{
    sal_uInt16 nWh = static_cast<sal_uInt16>( rTxtFtn.GetFtn().IsEndNote() ?
                        RES_END_AT_TXTEND : RES_FTN_AT_TXTEND );
    sal_uInt16 nVal;
    const SwSectionNode* pNd = rTxtFtn.GetTxtNode().FindSectionNode();
    while( pNd && FTNEND_ATTXTEND_OWNNUMSEQ != ( nVal =
            ((const SwFmtFtnAtTxtEnd&)pNd->GetSection().GetFmt()->
            GetFmtAttr( nWh, sal_True )).GetValue() ) &&
            FTNEND_ATTXTEND_OWNNUMANDFMT != nVal )
        pNd = pNd->StartOfSectionNode()->FindSectionNode();

    return pNd;
}

sal_uInt16 SwUpdFtnEndNtAtEnd::GetNumber( const SwTxtFtn& rTxtFtn,
                                    const SwSectionNode& rNd )
{
    sal_uInt16 nRet = 0, nWh;
    SvPtrarr* pArr;
    std::vector<sal_uInt16> *pNum;
    if( rTxtFtn.GetFtn().IsEndNote() )
    {
        pArr = &aEndSects;
        pNum = &aEndNums;
        nWh = RES_END_AT_TXTEND;
    }
    else
    {
        pArr = &aFtnSects;
        pNum = &aFtnNums;
        nWh = RES_FTN_AT_TXTEND;
    }
    void* pNd = (void*)&rNd;

    for( sal_uInt16 n = pArr->Count(); n; )
        if( pArr->GetObject( --n ) == pNd )
        {
            nRet = ++((*pNum)[ n ]);
            break;
        }

    if( !nRet )
    {
        pArr->Insert( pNd, pArr->Count() );
        nRet = ((SwFmtFtnEndAtTxtEnd&)rNd.GetSection().GetFmt()->
                                GetFmtAttr( nWh )).GetOffset();
        ++nRet;
        pNum->push_back( nRet );
    }
    return nRet;
}

sal_uInt16 SwUpdFtnEndNtAtEnd::ChkNumber( const SwTxtFtn& rTxtFtn )
{
    const SwSectionNode* pSectNd = FindSectNdWithEndAttr( rTxtFtn );
    return pSectNd ? GetNumber( rTxtFtn, *pSectNd ) : 0;
}




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
