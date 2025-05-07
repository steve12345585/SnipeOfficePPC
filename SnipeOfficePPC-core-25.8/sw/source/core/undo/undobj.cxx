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

#include <IShellCursorSupplier.hxx>
#include <txtftn.hxx>
#include <fmtanchr.hxx>
#include <ftnidx.hxx>
#include <frmfmt.hxx>
#include <doc.hxx>
#include <UndoManager.hxx>
#include <docary.hxx>
#include <swundo.hxx>
#include <pam.hxx>
#include <ndtxt.hxx>
#include <UndoCore.hxx>
#include <rolbck.hxx>
#include <ndnotxt.hxx>
#include <IMark.hxx>
#include <mvsave.hxx>
#include <redline.hxx>
#include <crossrefbookmark.hxx>
#include <undo.hrc>
#include <comcore.hrc>
#include <docsh.hxx>

class SwRedlineSaveData : public SwUndRng, public SwRedlineData,
                          private SwUndoSaveSection
{
public:
    SwRedlineSaveData( SwComparePosition eCmpPos,
                        const SwPosition& rSttPos, const SwPosition& rEndPos,
                        SwRedline& rRedl, sal_Bool bCopyNext );
    ~SwRedlineSaveData();
    void RedlineToDoc( SwPaM& rPam );
    SwNodeIndex* GetMvSttIdx() const
        { return SwUndoSaveSection::GetMvSttIdx(); }

#if OSL_DEBUG_LEVEL > 0
    sal_uInt16 nRedlineCount;
#endif
};

SV_IMPL_PTRARR( SwRedlineSaveDatas, SwRedlineSaveDataPtr )

// This class saves the Pam as sal_uInt16s and can recompose those into a PaM
SwUndRng::SwUndRng()
    : nSttNode( 0 ), nEndNode( 0 ), nSttCntnt( 0 ), nEndCntnt( 0 )
{
}

SwUndRng::SwUndRng( const SwPaM& rPam )
{
    SetValues( rPam );
}

void SwUndRng::SetValues( const SwPaM& rPam )
{
    const SwPosition *pStt = rPam.Start();
    if( rPam.HasMark() )
    {
        const SwPosition *pEnd = rPam.GetPoint() == pStt
                        ? rPam.GetMark()
                        : rPam.GetPoint();
        nEndNode = pEnd->nNode.GetIndex();
        nEndCntnt = pEnd->nContent.GetIndex();
    }
    else
        // no selection !!
        nEndNode = 0, nEndCntnt = STRING_MAXLEN;

    nSttNode = pStt->nNode.GetIndex();
    nSttCntnt = pStt->nContent.GetIndex();
}

void SwUndRng::SetPaM( SwPaM & rPam, sal_Bool bCorrToCntnt ) const
{
    rPam.DeleteMark();
    rPam.GetPoint()->nNode = nSttNode;
    SwNode* pNd = rPam.GetNode();
    if( pNd->IsCntntNode() )
        rPam.GetPoint()->nContent.Assign( pNd->GetCntntNode(), nSttCntnt );
    else if( bCorrToCntnt )
        rPam.Move( fnMoveForward, fnGoCntnt );
    else
        rPam.GetPoint()->nContent.Assign( 0, 0 );

    if( !nEndNode && STRING_MAXLEN == nEndCntnt )       // no selection
        return ;

    rPam.SetMark();
    if( nSttNode == nEndNode && nSttCntnt == nEndCntnt )
        return;                             // nothing left to do

    rPam.GetPoint()->nNode = nEndNode;
    if( (pNd = rPam.GetNode())->IsCntntNode() )
        rPam.GetPoint()->nContent.Assign( pNd->GetCntntNode(), nEndCntnt );
    else if( bCorrToCntnt )
        rPam.Move( fnMoveBackward, fnGoCntnt );
    else
        rPam.GetPoint()->nContent.Assign( 0, 0 );
}

SwPaM & SwUndRng::AddUndoRedoPaM(
        ::sw::UndoRedoContext & rContext, bool const bCorrToCntnt) const
{
    SwPaM & rPaM( rContext.GetCursorSupplier().CreateNewShellCursor() );
    SetPaM( rPaM, bCorrToCntnt );
    return rPaM;
}

void SwUndo::RemoveIdxFromSection( SwDoc& rDoc, sal_uLong nSttIdx,
                                    sal_uLong* pEndIdx )
{
    SwNodeIndex aIdx( rDoc.GetNodes(), nSttIdx );
    SwNodeIndex aEndIdx( rDoc.GetNodes(), pEndIdx ? *pEndIdx
                                    : aIdx.GetNode().EndOfSectionIndex() );
    SwPosition aPos( rDoc.GetNodes().GetEndOfPostIts() );
    rDoc.CorrAbs( aIdx, aEndIdx, aPos, sal_True );
}

void SwUndo::RemoveIdxFromRange( SwPaM& rPam, sal_Bool bMoveNext )
{
    const SwPosition* pEnd = rPam.End();
    if( bMoveNext )
    {
        if( pEnd != rPam.GetPoint() )
            rPam.Exchange();

        SwNodeIndex aStt( rPam.GetMark()->nNode );
        SwNodeIndex aEnd( rPam.GetPoint()->nNode );

        if( !rPam.Move( fnMoveForward ) )
        {
            rPam.Exchange();
            if( !rPam.Move( fnMoveBackward ) )
            {
                rPam.GetPoint()->nNode = rPam.GetDoc()->GetNodes().GetEndOfPostIts();
                rPam.GetPoint()->nContent.Assign( 0, 0 );
            }
        }

        rPam.GetDoc()->CorrAbs( aStt, aEnd, *rPam.GetPoint(), sal_True );
    }
    else
        rPam.GetDoc()->CorrAbs( rPam, *pEnd, sal_True );
}

void SwUndo::RemoveIdxRel( sal_uLong nIdx, const SwPosition& rPos )
{
    // Move only the Crsr. Bookmarks/TOXMarks/etc. are done by the corresponding
    // JoinNext/JoinPrev
    SwNodeIndex aIdx( rPos.nNode.GetNode().GetNodes(), nIdx );
    ::PaMCorrRel( aIdx, rPos );
}

SwUndo::SwUndo(SwUndoId const nId)
    : m_nId(nId), nOrigRedlineMode(nsRedlineMode_t::REDLINE_NONE),
      bCacheComment(true), pComment(NULL)
{
}

bool SwUndo::IsDelBox() const
{
    return GetId() == UNDO_COL_DELETE || GetId() == UNDO_ROW_DELETE ||
        GetId() == UNDO_TABLE_DELBOX;
}

SwUndo::~SwUndo()
{
    delete pComment;
}

class UndoRedoRedlineGuard
{
public:
    UndoRedoRedlineGuard(::sw::UndoRedoContext & rContext, SwUndo & rUndo)
        : m_rRedlineAccess(rContext.GetDoc())
        , m_eMode(m_rRedlineAccess.GetRedlineMode())
    {
        RedlineMode_t const eTmpMode =
            static_cast<RedlineMode_t>(rUndo.GetRedlineMode());
        if ((nsRedlineMode_t::REDLINE_SHOW_MASK & eTmpMode) !=
            (nsRedlineMode_t::REDLINE_SHOW_MASK & m_eMode))
        {
            m_rRedlineAccess.SetRedlineMode( eTmpMode );
        }
        m_rRedlineAccess.SetRedlineMode_intern( static_cast<RedlineMode_t>(
                eTmpMode | nsRedlineMode_t::REDLINE_IGNORE) );
    }
    ~UndoRedoRedlineGuard()
    {
        m_rRedlineAccess.SetRedlineMode(m_eMode);
    }
private:
    IDocumentRedlineAccess & m_rRedlineAccess;
    RedlineMode_t const m_eMode;
};

void SwUndo::Undo()
{
    assert(false); // SwUndo::Undo(): ERROR: must call UndoWithContext instead
}

void SwUndo::Redo()
{
    assert(false); // SwUndo::Redo(): ERROR: must call RedoWithContext instead
}

void SwUndo::UndoWithContext(SfxUndoContext & rContext)
{
    ::sw::UndoRedoContext *const pContext(
            dynamic_cast< ::sw::UndoRedoContext * >(& rContext));
    assert(pContext);
    if (!pContext) { return; }
    UndoRedoRedlineGuard const g(*pContext, *this);
    UndoImpl(*pContext);
}

void SwUndo::RedoWithContext(SfxUndoContext & rContext)
{
    ::sw::UndoRedoContext *const pContext(
            dynamic_cast< ::sw::UndoRedoContext * >(& rContext));
    assert(pContext);
    if (!pContext) { return; }
    UndoRedoRedlineGuard const g(*pContext, *this);
    RedoImpl(*pContext);
}

void SwUndo::Repeat(SfxRepeatTarget & rContext)
{
    ::sw::RepeatContext *const pRepeatContext(
            dynamic_cast< ::sw::RepeatContext * >(& rContext));
    assert(pRepeatContext);
    if (!pRepeatContext) { return; }
    RepeatImpl(*pRepeatContext);
}

sal_Bool SwUndo::CanRepeat(SfxRepeatTarget & rContext) const
{
    ::sw::RepeatContext *const pRepeatContext(
            dynamic_cast< ::sw::RepeatContext * >(& rContext));
    assert(pRepeatContext);
    if (!pRepeatContext) { return false; }
    return CanRepeatImpl(*pRepeatContext);
}

void SwUndo::RepeatImpl( ::sw::RepeatContext & )
{
}

bool SwUndo::CanRepeatImpl( ::sw::RepeatContext & ) const
{
    return ((REPEAT_START <= GetId()) && (GetId() < REPEAT_END));
}

rtl::OUString SwUndo::GetComment() const
{
    String aResult;

    if (bCacheComment)
    {
        if (! pComment)
        {
            pComment = new String(SW_RES(UNDO_BASE + GetId()));

            SwRewriter aRewriter = GetRewriter();

            *pComment = aRewriter.Apply(*pComment);
        }

        aResult = *pComment;
    }
    else
    {
        aResult = String(SW_RES(UNDO_BASE + GetId()));

        SwRewriter aRewriter = GetRewriter();

        aResult = aRewriter.Apply(aResult);
    }

    return aResult;
}

SwRewriter SwUndo::GetRewriter() const
{
    SwRewriter aResult;

    return aResult;
}

SwUndoSaveCntnt::SwUndoSaveCntnt()
    : pHistory( 0 )
{}

SwUndoSaveCntnt::~SwUndoSaveCntnt()
{
    delete pHistory;
}

// This is needed when deleting content. For REDO all contents will be moved
// into the UndoNodesArray. These methods add a seperator for the attributes to
// the end of TextNodes. As a result, the attributes will not be expanded.
// - MoveTo   moves from NodesArray into UndoNodesArray
// - MoveFrom moves from UndoNodesArray into NodesArray
//
// If pEndNdIdx is given, Undo/Redo calls -Ins/DelFly. In that case the whole
// section should be moved.
void SwUndoSaveCntnt::MoveToUndoNds( SwPaM& rPaM, SwNodeIndex* pNodeIdx,
                    SwIndex* pCntIdx, sal_uLong* pEndNdIdx, xub_StrLen* pEndCntIdx )
{
    SwDoc& rDoc = *rPaM.GetDoc();
    ::sw::UndoGuard const undoGuard(rDoc.GetIDocumentUndoRedo());

    SwNoTxtNode* pCpyNd = rPaM.GetNode()->GetNoTxtNode();

    // here comes the actual delete (move)
    SwNodes & rNds = rDoc.GetUndoManager().GetUndoNodes();
    SwPosition aPos( pEndNdIdx ? rNds.GetEndOfPostIts()
                               : rNds.GetEndOfExtras() );
    aPos.nNode--;

    const SwPosition* pStt = rPaM.Start(), *pEnd = rPaM.End();

    if( pCpyNd || pEndNdIdx || !aPos.nNode.GetNode().GetCntntNode() ||
        (!pStt->nContent.GetIndex() && (pStt->nNode != pEnd->nNode ||
                (!pStt->nNode.GetNode().GetCntntNode() ||
                    pStt->nNode.GetNode().GetCntntNode()->Len() ==
                        pEnd->nContent.GetIndex() ) ) ) )
    {
        aPos.nNode++;
        aPos.nContent = 0;
    }
    else
        aPos.nNode.GetNode().GetCntntNode()->MakeEndIndex( &aPos.nContent );

    // keep as sal_uInt16; the indices shift!
    sal_uLong nTmpMvNode = aPos.nNode.GetIndex();
    xub_StrLen nTmpMvCntnt = aPos.nContent.GetIndex();

    if( pCpyNd || pEndNdIdx )
    {
        SwNodeRange aRg( pStt->nNode, 0, pEnd->nNode, 1 );
        rDoc.GetNodes()._MoveNodes( aRg, rNds, aPos.nNode, sal_False );
        aPos.nContent = 0;
        aPos.nNode--;
    }
    else
    {
        rDoc.GetNodes().MoveRange( rPaM, aPos, rNds );

        SwTxtNode* pTxtNd = aPos.nNode.GetNode().GetTxtNode();
        if( pTxtNd )        // add a seperator for the attributes
        {
            // But since all attributes will be touched at an insert (meaning
            // deleted from the array and re-added again), attributes might
            // disappear (e.g. "no bold" for 10-20,  "bold" for 12-15 -> when
            // inserting/deleting, the "bold" will be deleted, which is not
            // wanted here!)! Thus do not touch the hints but manipulate the
            // string directly.
            String& rStr = (String&)pTxtNd->GetTxt();
            // For safety reasons better only if positioned at the end
            if( rStr.Len() == aPos.nContent.GetIndex() )
            {
                rStr.Insert( ' ' );
                ++aPos.nContent;
            }
            else
            {
                pTxtNd->InsertText( sal_Unicode(' '), aPos.nContent,
                        IDocumentContentOperations::INS_NOHINTEXPAND );
            }
        }
    }
    if( pEndNdIdx )
        *pEndNdIdx = aPos.nNode.GetIndex();
    if( pEndCntIdx )
        *pEndCntIdx = aPos.nContent.GetIndex();

    // old position
    aPos.nNode = nTmpMvNode;
    if( pNodeIdx )
        *pNodeIdx = aPos.nNode;

    if( pCntIdx )
    {
        SwCntntNode* pCNd = aPos.nNode.GetNode().GetCntntNode();
        if( pCNd )
            pCntIdx->Assign( pCNd, nTmpMvCntnt );
        else
            pCntIdx->Assign( 0, 0 );
    }
}

void SwUndoSaveCntnt::MoveFromUndoNds( SwDoc& rDoc, sal_uLong nNodeIdx,
                            xub_StrLen nCntIdx, SwPosition& rInsPos,
                            sal_uLong* pEndNdIdx, xub_StrLen* pEndCntIdx )
{
    // here comes the recovery
    SwNodes & rNds = rDoc.GetUndoManager().GetUndoNodes();
    if( nNodeIdx == rNds.GetEndOfPostIts().GetIndex() )
        return;     // nothing saved

    ::sw::UndoGuard const undoGuard(rDoc.GetIDocumentUndoRedo());

    SwPaM aPaM( rInsPos );
    if( pEndNdIdx )         // than get the section from it
        aPaM.GetPoint()->nNode.Assign( rNds, *pEndNdIdx );
    else
    {
        aPaM.GetPoint()->nNode = rNds.GetEndOfExtras();
        GoInCntnt( aPaM, fnMoveBackward );
    }

    SwTxtNode* pTxtNd = aPaM.GetNode()->GetTxtNode();
    if( !pEndNdIdx && pTxtNd )  // delete the seperator again
    {
        if( pEndCntIdx )
            aPaM.GetPoint()->nContent.Assign( pTxtNd, *pEndCntIdx );
        if( pTxtNd->GetTxt().Len() )
        {
            GoInCntnt( aPaM, fnMoveBackward );
            pTxtNd->EraseText( aPaM.GetPoint()->nContent, 1 );
        }

        aPaM.SetMark();
        aPaM.GetPoint()->nNode = nNodeIdx;
        aPaM.GetPoint()->nContent.Assign( aPaM.GetCntntNode(), nCntIdx );

        _SaveRedlEndPosForRestore aRedlRest( rInsPos.nNode, rInsPos.nContent.GetIndex() );

        rNds.MoveRange( aPaM, rInsPos, rDoc.GetNodes() );

        // delete the last Node as well
        if( !aPaM.GetPoint()->nContent.GetIndex() ||
            ( aPaM.GetPoint()->nNode++ &&       // still empty Nodes at the end?
            &rNds.GetEndOfExtras() != &aPaM.GetPoint()->nNode.GetNode() ))
        {
            aPaM.GetPoint()->nContent.Assign( 0, 0 );
            aPaM.SetMark();
            rNds.Delete( aPaM.GetPoint()->nNode,
                        rNds.GetEndOfExtras().GetIndex() -
                        aPaM.GetPoint()->nNode.GetIndex() );
        }

        aRedlRest.Restore();
    }
    else if( pEndNdIdx || !pTxtNd )
    {
        SwNodeRange aRg( rNds, nNodeIdx, rNds, (pEndNdIdx
                        ? ((*pEndNdIdx) + 1)
                        : rNds.GetEndOfExtras().GetIndex() ) );
        rNds._MoveNodes( aRg, rDoc.GetNodes(), rInsPos.nNode, 0 == pEndNdIdx );

    }
    else {
        OSL_FAIL( "What happened now?" );
    }
}

// These two methods move the Point of Pam backwards/forwards. With that, one
// can span an area for a Undo/Redo. (The Point is then positioned in front of
// the area to manipulate!)
// The flag indicates if there is still content in front of Point.
sal_Bool SwUndoSaveCntnt::MovePtBackward( SwPaM& rPam )
{
    rPam.SetMark();
    if( rPam.Move( fnMoveBackward ))
        return sal_True;

    // If there is no content onwards, set Point simply to the previous position
    // (Node and Content, so that Content will be detached!)
    rPam.GetPoint()->nNode--;
    rPam.GetPoint()->nContent.Assign( 0, 0 );
    return sal_False;
}

void SwUndoSaveCntnt::MovePtForward( SwPaM& rPam, sal_Bool bMvBkwrd )
{
    // Was there content before this position?
    if( bMvBkwrd )
        rPam.Move( fnMoveForward );
    else
    {
        rPam.GetPoint()->nNode++;
        SwCntntNode* pCNd = rPam.GetCntntNode();
        if( pCNd )
            pCNd->MakeStartIndex( &rPam.GetPoint()->nContent );
        else
            rPam.Move( fnMoveForward );
    }
}


// Delete all objects that have ContentIndices to the given area.
// Currently (1994) these exist:
//                  - Footnotes
//                  - Flys
//                  - Bookmarks
//                  - Directories
//
// #i81002# - extending method
// delete certain (not all) cross-reference bookmarks at text node of <rMark>
// and at text node of <rPoint>, if these text nodes aren't the same.
void SwUndoSaveCntnt::DelCntntIndex( const SwPosition& rMark,
                                     const SwPosition& rPoint,
                                     DelCntntType nDelCntntType )
{
    const SwPosition *pStt = rMark < rPoint ? &rMark : &rPoint,
                    *pEnd = &rMark == pStt ? &rPoint : &rMark;

    SwDoc* pDoc = rMark.nNode.GetNode().GetDoc();

    ::sw::UndoGuard const undoGuard(pDoc->GetIDocumentUndoRedo());

    // 1. Footnotes
    if( nsDelCntntType::DELCNT_FTN & nDelCntntType )
    {
        SwFtnIdxs& rFtnArr = pDoc->GetFtnIdxs();
        if( rFtnArr.Count() )
        {
            const SwNode* pFtnNd;
            sal_uInt16 nPos;
            rFtnArr.SeekEntry( pStt->nNode, &nPos );
            SwTxtFtn* pSrch;

            // for now delete all that come afterwards
            while( nPos < rFtnArr.Count() && ( pFtnNd =
                &( pSrch = rFtnArr[ nPos ] )->GetTxtNode())->GetIndex()
                        <= pEnd->nNode.GetIndex() )
            {
                xub_StrLen nFtnSttIdx = *pSrch->GetStart();
                if( (nsDelCntntType::DELCNT_CHKNOCNTNT & nDelCntntType )
                    ? (&pEnd->nNode.GetNode() == pFtnNd )
                    : (( &pStt->nNode.GetNode() == pFtnNd &&
                    pStt->nContent.GetIndex() > nFtnSttIdx) ||
                    ( &pEnd->nNode.GetNode() == pFtnNd &&
                    nFtnSttIdx >= pEnd->nContent.GetIndex() )) )
                {
                    ++nPos;     // continue searching
                    continue;
                }

// FIXME: duplicated code here and below -> refactor?
                // Unfortunately an index needs to be created. Otherwise there
                // will be problems with TextNode because the index will be
                // deleted in the DTOR of SwFtn!
                SwTxtNode* pTxtNd = (SwTxtNode*)pFtnNd;
                if( !pHistory )
                    pHistory = new SwHistory;
                SwTxtAttr* const pFtnHnt =
                    pTxtNd->GetTxtAttrForCharAt( nFtnSttIdx );
                assert(pFtnHnt);
                SwIndex aIdx( pTxtNd, nFtnSttIdx );
                pHistory->Add( pFtnHnt, pTxtNd->GetIndex(), false );
                pTxtNd->EraseText( aIdx, 1 );
            }

            while( nPos-- && ( pFtnNd = &( pSrch = rFtnArr[ nPos ] )->
                    GetTxtNode())->GetIndex() >= pStt->nNode.GetIndex() )
            {
                xub_StrLen nFtnSttIdx = *pSrch->GetStart();
                if( !(nsDelCntntType::DELCNT_CHKNOCNTNT & nDelCntntType) && (
                    ( &pStt->nNode.GetNode() == pFtnNd &&
                    pStt->nContent.GetIndex() > nFtnSttIdx ) ||
                    ( &pEnd->nNode.GetNode() == pFtnNd &&
                    nFtnSttIdx >= pEnd->nContent.GetIndex() )))
                    continue;               // continue searching

                // Unfortunately an index needs to be created. Otherwise there
                // will be problems with TextNode because the index will be
                // deleted in the DTOR of SwFtn!
                SwTxtNode* pTxtNd = (SwTxtNode*)pFtnNd;
                if( !pHistory )
                    pHistory = new SwHistory;
                SwTxtAttr* const pFtnHnt =
                    pTxtNd->GetTxtAttrForCharAt( nFtnSttIdx );
                assert(pFtnHnt);
                SwIndex aIdx( pTxtNd, nFtnSttIdx );
                pHistory->Add( pFtnHnt, pTxtNd->GetIndex(), false );
                pTxtNd->EraseText( aIdx, 1 );
            }
        }
    }

    // 2. Flys
    if( nsDelCntntType::DELCNT_FLY & nDelCntntType )
    {
        sal_uInt16 nChainInsPos = pHistory ? pHistory->Count() : 0;
        const SwSpzFrmFmts& rSpzArr = *pDoc->GetSpzFrmFmts();
        if( rSpzArr.Count() )
        {
            const sal_Bool bDelFwrd = rMark.nNode.GetIndex() <= rPoint.nNode.GetIndex();
            SwFlyFrmFmt* pFmt;
            const SwFmtAnchor* pAnchor;
            sal_uInt16 n = rSpzArr.Count();
            const SwPosition* pAPos;

            while( n && rSpzArr.Count() )
            {
                pFmt = (SwFlyFrmFmt*)rSpzArr[--n];
                pAnchor = &pFmt->GetAnchor();
                switch( pAnchor->GetAnchorId() )
                {
                case FLY_AS_CHAR:
                    if( 0 != (pAPos = pAnchor->GetCntntAnchor() ) &&
                        (( nsDelCntntType::DELCNT_CHKNOCNTNT & nDelCntntType )
                        ? ( pStt->nNode <= pAPos->nNode &&
                            pAPos->nNode < pEnd->nNode )
                        : ( *pStt <= *pAPos && *pAPos < *pEnd )) )
                    {
                        if( !pHistory )
                            pHistory = new SwHistory;
                        SwTxtNode *const pTxtNd =
                            pAPos->nNode.GetNode().GetTxtNode();
                        SwTxtAttr* const pFlyHnt = pTxtNd->GetTxtAttrForCharAt(
                            pAPos->nContent.GetIndex());
                        assert(pFlyHnt);
                        pHistory->Add( pFlyHnt, 0, false );
                        // reset n so that no Format is skipped
                        n = n >= rSpzArr.Count() ? rSpzArr.Count() : n+1;
                    }
                    break;
                case FLY_AT_PARA:
                    {
                        pAPos =  pAnchor->GetCntntAnchor();
                        if( pAPos )
                        {
                            bool bTmp;
                            if( nsDelCntntType::DELCNT_CHKNOCNTNT & nDelCntntType )
                                bTmp = pStt->nNode <= pAPos->nNode && pAPos->nNode < pEnd->nNode;
                            else
                            {
                                if (bDelFwrd)
                                    bTmp = rMark.nNode < pAPos->nNode &&
                                        pAPos->nNode <= rPoint.nNode;
                                else
                                    bTmp = rPoint.nNode <= pAPos->nNode &&
                                        pAPos->nNode < rMark.nNode;
                            }

                            if (bTmp)
                            {
                                if( !pHistory )
                                    pHistory = new SwHistory;

                                // Moving the anchor?
                                if( !( nsDelCntntType::DELCNT_CHKNOCNTNT & nDelCntntType ) &&
                                    ( rPoint.nNode.GetIndex() == pAPos->nNode.GetIndex() ) )
                                {
                                    // Do not try to move the anchor to a table!
                                    if( rMark.nNode.GetNode().GetTxtNode() )
                                    {
                                        pHistory->Add( *pFmt );
                                        SwFmtAnchor aAnch( *pAnchor );
                                        SwPosition aPos( rMark.nNode );
                                        aAnch.SetAnchor( &aPos );
                                        pFmt->SetFmtAttr( aAnch );
                                    }
                                }
                                else
                                {
                                    pHistory->Add( *pFmt, nChainInsPos );
                                    // reset n so that no Format is skipped
                                    n = n >= rSpzArr.Count() ?
                                        rSpzArr.Count() : n+1;
                                }
                            }
                        }
                    }
                    break;
                case FLY_AT_CHAR:
                    if( 0 != (pAPos = pAnchor->GetCntntAnchor() ) &&
                        ( pStt->nNode <= pAPos->nNode && pAPos->nNode <= pEnd->nNode ) )
                    {
                        if( !pHistory )
                            pHistory = new SwHistory;
                        if (IsDestroyFrameAnchoredAtChar(
                                *pAPos, *pStt, *pEnd, nDelCntntType))
                        {
                            pHistory->Add( *pFmt, nChainInsPos );
                            n = n >= rSpzArr.Count() ? rSpzArr.Count() : n+1;
                        }
                        else if( !( nsDelCntntType::DELCNT_CHKNOCNTNT & nDelCntntType ) )
                        {
                            if( *pStt <= *pAPos && *pAPos < *pEnd )
                            {
                                // These are the objects anchored
                                // between section start and end position
                                // Do not try to move the anchor to a table!
                                if( rMark.nNode.GetNode().GetTxtNode() )
                                {
                                    pHistory->Add( *pFmt );
                                    SwFmtAnchor aAnch( *pAnchor );
                                    aAnch.SetAnchor( &rMark );
                                    pFmt->SetFmtAttr( aAnch );
                                }
                            }
                        }
                    }
                    break;
                case FLY_AT_FLY:

                    if( 0 != (pAPos = pAnchor->GetCntntAnchor() ) &&
                        pStt->nNode == pAPos->nNode )
                    {
                        if( !pHistory )
                            pHistory = new SwHistory;

                        pHistory->Add( *pFmt, nChainInsPos );

                        // reset n so that no Format is skipped
                        n = n >= rSpzArr.Count() ? rSpzArr.Count() : n+1;
                    }
                    break;
                default: break;
                }
            }
        }
    }

    // 3. Bookmarks
    if( nsDelCntntType::DELCNT_BKM & nDelCntntType )
    {
        IDocumentMarkAccess* const pMarkAccess = pDoc->getIDocumentMarkAccess();
        if( pMarkAccess->getMarksCount() )
        {

            for( sal_uInt16 n = 0; n < pMarkAccess->getMarksCount(); ++n )
            {
                // #i81002#
                bool bSavePos = false;
                bool bSaveOtherPos = false;
                const ::sw::mark::IMark* pBkmk = (pMarkAccess->getMarksBegin() + n)->get();
                if( nsDelCntntType::DELCNT_CHKNOCNTNT & nDelCntntType )
                {
                    if( pStt->nNode <= pBkmk->GetMarkPos().nNode &&
                        pBkmk->GetMarkPos().nNode < pEnd->nNode )
                        bSavePos = true;
                    if( pBkmk->IsExpanded() &&
                        pStt->nNode <= pBkmk->GetOtherMarkPos().nNode &&
                        pBkmk->GetOtherMarkPos().nNode < pEnd->nNode )
                        bSaveOtherPos = true;
                }
                else
                {
                    // #i92125#
                    bool bKeepCrossRefBkmk( false );
                    {
                        if ( rMark.nNode == rPoint.nNode &&
                             ( IDocumentMarkAccess::GetType(*pBkmk) ==
                                IDocumentMarkAccess::CROSSREF_HEADING_BOOKMARK ||
                               IDocumentMarkAccess::GetType(*pBkmk) ==
                                IDocumentMarkAccess::CROSSREF_NUMITEM_BOOKMARK ) )
                        {
                            bKeepCrossRefBkmk = true;
                        }
                    }
                    if ( !bKeepCrossRefBkmk )
                    {
                        bool bMaybe = false;
                        if ( *pStt <= pBkmk->GetMarkPos() && pBkmk->GetMarkPos() <= *pEnd )
                        {
                            if( pBkmk->GetMarkPos() == *pEnd ||
                                ( *pStt == pBkmk->GetMarkPos() && pBkmk->IsExpanded() ) )
                                bMaybe = true;
                            else
                                bSavePos = true;
                        }
                        if( pBkmk->IsExpanded() &&
                            *pStt <= pBkmk->GetOtherMarkPos() && pBkmk->GetOtherMarkPos() <= *pEnd )
                        {
                            if( bSavePos || bSaveOtherPos ||
                                ( pBkmk->GetOtherMarkPos() < *pEnd && pBkmk->GetOtherMarkPos() > *pStt ) )
                            {
                                if( bMaybe )
                                    bSavePos = true;
                                bSaveOtherPos = true;
                            }
                        }
                    }

                    // #i81002#
                    const bool bDifferentTxtNodesAtMarkAndPoint(
                                        rMark.nNode != rPoint.nNode &&
                                        rMark.nNode.GetNode().GetTxtNode() &&
                                        rPoint.nNode.GetNode().GetTxtNode() );
                    if( !bSavePos && !bSaveOtherPos && bDifferentTxtNodesAtMarkAndPoint &&
                        dynamic_cast< const ::sw::mark::CrossRefBookmark* >(pBkmk))
                    {
                        // delete cross-reference bookmark at <pStt>, if only
                        // part of <pEnd> text node content is deleted.
                        if( pStt->nNode == pBkmk->GetMarkPos().nNode &&
                            pEnd->nContent.GetIndex() !=
                                pEnd->nNode.GetNode().GetTxtNode()->Len() )
                        {
                            bSavePos = true;
                            bSaveOtherPos = false;
                        }
                        // delete cross-reference bookmark at <pEnd>, if only
                        // part of <pStt> text node content is deleted.
                        else if( pEnd->nNode == pBkmk->GetMarkPos().nNode &&
                            pStt->nContent.GetIndex() != 0 )
                        {
                            bSavePos = true;
                            bSaveOtherPos = false;
                        }
                    }
                }
                if( bSavePos || bSaveOtherPos )
                {
                    if( !pHistory )
                        pHistory = new SwHistory;

                    pHistory->Add( *pBkmk, bSavePos, bSaveOtherPos );
                    if(bSavePos &&
                        (bSaveOtherPos || !pBkmk->IsExpanded()))
                    {
                        pMarkAccess->deleteMark(pMarkAccess->getMarksBegin()+n);
                        n--;
                    }
                }
            }
        }
    }
}

// save a complete section into UndoNodes array
SwUndoSaveSection::SwUndoSaveSection()
    : pMvStt( 0 ), pRedlSaveData( 0 ), nMvLen( 0 ), nStartPos( ULONG_MAX )
{
}

SwUndoSaveSection::~SwUndoSaveSection()
{
    if( pMvStt )        // delete also the section from UndoNodes array
    {
        // SaveSection saves the content in the PostIt section.
        SwNodes& rUNds = pMvStt->GetNode().GetNodes();
        rUNds.Delete( *pMvStt, nMvLen );

        delete pMvStt;
    }
    delete pRedlSaveData;
}

void SwUndoSaveSection::SaveSection( SwDoc* pDoc, const SwNodeIndex& rSttIdx )
{
    SwNodeRange aRg( rSttIdx.GetNode(), *rSttIdx.GetNode().EndOfSectionNode() );
    SaveSection( pDoc, aRg );
}

void SwUndoSaveSection::SaveSection( SwDoc* , const SwNodeRange& rRange )
{
    SwPaM aPam( rRange.aStart, rRange.aEnd );

    // delete all Footnotes / FlyFrames / Bookmarks / Directories
    DelCntntIndex( *aPam.GetMark(), *aPam.GetPoint() );

    pRedlSaveData = new SwRedlineSaveDatas;
    if( !SwUndo::FillSaveData( aPam, *pRedlSaveData, sal_True, sal_True ))
        delete pRedlSaveData, pRedlSaveData = 0;

    nStartPos = rRange.aStart.GetIndex();

    aPam.GetPoint()->nNode--;
    aPam.GetMark()->nNode++;

    SwCntntNode* pCNd = aPam.GetCntntNode( sal_False );
    if( pCNd )
        aPam.GetMark()->nContent.Assign( pCNd, 0 );
    if( 0 != ( pCNd = aPam.GetCntntNode( sal_True )) )
        aPam.GetPoint()->nContent.Assign( pCNd, pCNd->Len() );

    // Keep positions as SwIndex so that this section can be deleted in DTOR
    sal_uLong nEnd;
    pMvStt = new SwNodeIndex( rRange.aStart );
    MoveToUndoNds( aPam, pMvStt, 0, &nEnd, 0 );
    nMvLen = nEnd - pMvStt->GetIndex() + 1;
}

void SwUndoSaveSection::RestoreSection( SwDoc* pDoc, SwNodeIndex* pIdx,
                                        sal_uInt16 nSectType )
{
    if( ULONG_MAX != nStartPos )        // was there any content?
    {
        // check if the content is at the old position
        SwNodeIndex aSttIdx( pDoc->GetNodes(), nStartPos );

        // move the content from UndoNodes array into Fly
        SwStartNode* pSttNd = pDoc->GetNodes().MakeEmptySection( aSttIdx,
                                                (SwStartNodeType)nSectType );

        RestoreSection( pDoc, SwNodeIndex( *pSttNd->EndOfSectionNode() ));

        if( pIdx )
            *pIdx = *pSttNd;
    }
}

void SwUndoSaveSection::RestoreSection( SwDoc* pDoc, const SwNodeIndex& rInsPos )
{
    if( ULONG_MAX != nStartPos )        // was there any content?
    {
        SwPosition aInsPos( rInsPos );
        sal_uLong nEnd = pMvStt->GetIndex() + nMvLen - 1;
        MoveFromUndoNds( *pDoc, pMvStt->GetIndex(), 0, aInsPos, &nEnd, 0 );

        // destroy indices again, content was deleted from UndoNodes array
        DELETEZ( pMvStt );
        nMvLen = 0;

        if( pRedlSaveData )
        {
            SwUndo::SetSaveData( *pDoc, *pRedlSaveData );
            delete pRedlSaveData, pRedlSaveData = 0;
        }
    }
}

// save and set the RedlineData
SwRedlineSaveData::SwRedlineSaveData( SwComparePosition eCmpPos,
                                        const SwPosition& rSttPos,
                                        const SwPosition& rEndPos,
                                        SwRedline& rRedl,
                                        sal_Bool bCopyNext )
    : SwUndRng( rRedl ),
    SwRedlineData( rRedl.GetRedlineData(), bCopyNext )
{
    assert( POS_OUTSIDE == eCmpPos ||
            !rRedl.GetContentIdx() ); // "Redline with Content"

    switch( eCmpPos )
    {
    case POS_OVERLAP_BEFORE:        // Pos1 overlaps Pos2 at the beginning
        nEndNode = rEndPos.nNode.GetIndex();
        nEndCntnt = rEndPos.nContent.GetIndex();
        break;
    case POS_OVERLAP_BEHIND:        // Pos1 overlaps Pos2 at the end
        nSttNode = rSttPos.nNode.GetIndex();
        nSttCntnt = rSttPos.nContent.GetIndex();
        break;

    case POS_INSIDE:                // Pos1 lays completely in Pos2
        nSttNode = rSttPos.nNode.GetIndex();
        nSttCntnt = rSttPos.nContent.GetIndex();
        nEndNode = rEndPos.nNode.GetIndex();
        nEndCntnt = rEndPos.nContent.GetIndex();
        break;

    case POS_OUTSIDE:               // Pos2 lays completely in Pos1
        if( rRedl.GetContentIdx() )
        {
            // than move section into UndoArray and memorize it
            SaveSection( rRedl.GetDoc(), *rRedl.GetContentIdx() );
            rRedl.SetContentIdx( 0 );
        }
        break;

    case POS_EQUAL:                 // Pos1 ist exactly as big as Pos2
        break;

    default:
        assert(false);
    }

#if OSL_DEBUG_LEVEL > 0
    nRedlineCount = rSttPos.nNode.GetNode().GetDoc()->GetRedlineTbl().Count();
#endif
}

SwRedlineSaveData::~SwRedlineSaveData()
{
}

void SwRedlineSaveData::RedlineToDoc( SwPaM& rPam )
{
    SwDoc& rDoc = *rPam.GetDoc();
    SwRedline* pRedl = new SwRedline( *this, rPam );

    if( GetMvSttIdx() )
    {
        SwNodeIndex aIdx( rDoc.GetNodes() );
        RestoreSection( &rDoc, &aIdx, SwNormalStartNode );
        if( GetHistory() )
            GetHistory()->Rollback( &rDoc );
        pRedl->SetContentIdx( &aIdx );
    }
    SetPaM( *pRedl );
    // First, delete the "old" so that in an Append no unexpected things will
    // happen, e.g. a delete in an insert. In the latter case the just restored
    // content will be deleted and not the one you originally wanted.
    rDoc.DeleteRedline( *pRedl, false, USHRT_MAX );

    RedlineMode_t eOld = rDoc.GetRedlineMode();
    rDoc.SetRedlineMode_intern((RedlineMode_t)(eOld | nsRedlineMode_t::REDLINE_DONTCOMBINE_REDLINES));
    //#i92154# let UI know about a new redline with comment
    if (rDoc.GetDocShell() && (pRedl->GetComment() != String()) )
        rDoc.GetDocShell()->Broadcast(SwRedlineHint(pRedl,SWREDLINE_INSERTED));

    bool const bSuccess = rDoc.AppendRedline( pRedl, true );
    assert(bSuccess); // SwRedlineSaveData::RedlineToDoc: insert redline failed
    (void) bSuccess; // unused in non-debug
    rDoc.SetRedlineMode_intern( eOld );
}

sal_Bool SwUndo::FillSaveData( const SwPaM& rRange, SwRedlineSaveDatas& rSData,
                            sal_Bool bDelRange, sal_Bool bCopyNext )
{
    if( rSData.Count() )
        rSData.DeleteAndDestroy( 0, rSData.Count() );

    SwRedlineSaveData* pNewData;
    const SwPosition *pStt = rRange.Start(), *pEnd = rRange.End();
    const SwRedlineTbl& rTbl = rRange.GetDoc()->GetRedlineTbl();
    sal_uInt16 n = 0;
    rRange.GetDoc()->GetRedline( *pStt, &n );
    for( ; n < rTbl.Count(); ++n )
    {
        SwRedline* pRedl = rTbl[ n ];
        const SwPosition *pRStt = pRedl->Start(), *pREnd = pRedl->End();

        SwComparePosition eCmpPos = ComparePosition( *pStt, *pEnd, *pRStt, *pREnd );
        if( POS_BEFORE != eCmpPos && POS_BEHIND != eCmpPos &&
            POS_COLLIDE_END != eCmpPos && POS_COLLIDE_START != eCmpPos )
        {
            pNewData = new SwRedlineSaveData( eCmpPos, *pStt, *pEnd,
                                                *pRedl, bCopyNext );
            rSData.Insert( pNewData, rSData.Count() );
        }
    }
    if( rSData.Count() && bDelRange )
        rRange.GetDoc()->DeleteRedline( rRange, false, USHRT_MAX );
    return 0 != rSData.Count();
}

sal_Bool SwUndo::FillSaveDataForFmt( const SwPaM& rRange, SwRedlineSaveDatas& rSData )
{
    if( rSData.Count() )
        rSData.DeleteAndDestroy( 0, rSData.Count() );

    SwRedlineSaveData* pNewData;
    const SwPosition *pStt = rRange.Start(), *pEnd = rRange.End();
    const SwRedlineTbl& rTbl = rRange.GetDoc()->GetRedlineTbl();
    sal_uInt16 n = 0;
    rRange.GetDoc()->GetRedline( *pStt, &n );
    for( ; n < rTbl.Count(); ++n )
    {
        SwRedline* pRedl = rTbl[ n ];
        if( nsRedlineType_t::REDLINE_FORMAT == pRedl->GetType() )
        {
            const SwPosition *pRStt = pRedl->Start(), *pREnd = pRedl->End();

            SwComparePosition eCmpPos = ComparePosition( *pStt, *pEnd, *pRStt, *pREnd );
            if( POS_BEFORE != eCmpPos && POS_BEHIND != eCmpPos &&
                POS_COLLIDE_END != eCmpPos && POS_COLLIDE_START != eCmpPos )
            {
                pNewData = new SwRedlineSaveData( eCmpPos, *pStt, *pEnd,
                                                    *pRedl, sal_True );
                rSData.Insert( pNewData, rSData.Count() );
            }


        }
    }
    return 0 != rSData.Count();
}

void SwUndo::SetSaveData( SwDoc& rDoc, const SwRedlineSaveDatas& rSData )
{
    RedlineMode_t eOld = rDoc.GetRedlineMode();
    rDoc.SetRedlineMode_intern( (RedlineMode_t)(( eOld & ~nsRedlineMode_t::REDLINE_IGNORE) | nsRedlineMode_t::REDLINE_ON ));
    SwPaM aPam( rDoc.GetNodes().GetEndOfContent() );

    for( sal_uInt16 n = rSData.Count(); n; )
        rSData[ --n ]->RedlineToDoc( aPam );

#if OSL_DEBUG_LEVEL > 0
    // check redline count against count saved in RedlineSaveData object
    assert((rSData.Count() == 0) ||
           (rSData[0]->nRedlineCount == rDoc.GetRedlineTbl().Count()));
            // "redline count not restored properly"
#endif

    rDoc.SetRedlineMode_intern( eOld );
}

sal_Bool SwUndo::HasHiddenRedlines( const SwRedlineSaveDatas& rSData )
{
    for( sal_uInt16 n = rSData.Count(); n; )
        if( rSData[ --n ]->GetMvSttIdx() )
            return sal_True;
    return sal_False;
}

sal_Bool SwUndo::CanRedlineGroup( SwRedlineSaveDatas& rCurr,
                        const SwRedlineSaveDatas& rCheck, sal_Bool bCurrIsEnd )
{
    sal_Bool bRet = sal_False;
    sal_uInt16 n;

    if( rCurr.Count() == rCheck.Count() )
    {
        bRet = sal_True;
        for( n = 0; n < rCurr.Count(); ++n )
        {
            const SwRedlineSaveData& rSet = *rCurr[ n ];
            const SwRedlineSaveData& rGet = *rCheck[ n ];
            if( rSet.nSttNode != rGet.nSttNode ||
                rSet.GetMvSttIdx() || rGet.GetMvSttIdx() ||
                ( bCurrIsEnd ? rSet.nSttCntnt != rGet.nEndCntnt
                             : rSet.nEndCntnt != rGet.nSttCntnt ) ||
                !rGet.CanCombine( rSet ) )
            {
                bRet = sal_False;
                break;
            }
        }

        if( bRet )
            for( n = 0; n < rCurr.Count(); ++n )
            {
                SwRedlineSaveData& rSet = *rCurr[ n ];
                const SwRedlineSaveData& rGet = *rCheck[ n ];
                if( bCurrIsEnd )
                    rSet.nSttCntnt = rGet.nSttCntnt;
                else
                    rSet.nEndCntnt = rGet.nEndCntnt;
            }
    }
    return bRet;
}

// #111827#
String ShortenString(const String & rStr, xub_StrLen nLength, const String & rFillStr)
{
    assert(nLength - rFillStr.Len() >= 2);

    String aResult;

    if (rStr.Len() <= nLength)
        aResult = rStr;
    else
    {
        long nTmpLength = nLength - rFillStr.Len();
        if ( nTmpLength < 2 )
            nTmpLength = 2;

        nLength = static_cast<xub_StrLen>(nTmpLength);

        const xub_StrLen nFrontLen = nLength - nLength / 2;
        const xub_StrLen nBackLen = nLength - nFrontLen;

        aResult += rStr.Copy(0, nFrontLen);
        aResult += rFillStr;
        aResult += rStr.Copy(rStr.Len() - nBackLen, nBackLen);
    }

    return aResult;
}

bool IsDestroyFrameAnchoredAtChar(SwPosition const & rAnchorPos,
        SwPosition const & rStart, SwPosition const & rEnd,
        DelCntntType const nDelCntntType)
{

    // Here we identified the objects to destroy:
    // - anchored between start and end of the selection
    // - anchored in start of the selection with "CheckNoContent"
    // - anchored in start of sel. and the selection start at pos 0
    return  (rAnchorPos.nNode < rEnd.nNode)
         && (   (nsDelCntntType::DELCNT_CHKNOCNTNT & nDelCntntType)
            ||  (rStart.nNode < rAnchorPos.nNode)
            ||  !rStart.nContent.GetIndex()
            );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
