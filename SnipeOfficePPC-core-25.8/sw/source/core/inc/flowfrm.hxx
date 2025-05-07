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
#ifndef _FLOWFRM_HXX
#define _FLOWFRM_HXX

//Der FlowFrm gibt die Funktionalitaet fuer alle Frms vor, die fliessen und
//die sich aufspalten koennen (wie CntntFrm oder TabFrm).
//Teile der Funktionalitaet sind im FlowFrm implementiert, andere Teile werden
//von den spezifischen Frms implementiert.
//Der FlowFrm ist kein eigenstaender Frm, es kann also auch niemals eine
//eigenstaendige Instanz vom FlowFrm existieren.
//Der FlowFrm ist nicht einmal ein echter Frm. Die naheliegende Implementierung
//waere ein FlowFrm der virtual vom SwFrm abgeleitet ist und direkt auf den
//eigenen Instanzdaten arbeitet. Abgeleitete Klassen muessten sich
//vom FlowFrm und (ueber mehrere Basisklassen weil der Klassenbaum sich direkt
//vom SwFrm zu SwCntntFrm und zum SwLayoutFrm spaltet) virtual vom SwFrm
//ableiten.
//Leider entstehen dadurch - neben Problemen mit Compilern und Debuggern -
//erhebliche zusaetzliche Kosten, die wir uns heutzutage IMHO nicht erlauben
//koennen.
//Ich greife deshalb auf eine andere Technik zurueck: Der FlowFrm hat eine
//Referenz auf den SwFrm - der er genau betrachtet selbst ist - und ist mit
//diesem befreundet. So kann der FlowFrm anstelle des this-Pointer mit der
//Referenz auf den SwFrm arbeiten.

class SwPageFrm;
class SwRect;
class SwBorderAttrs;
class SwDoc;
class SwNodeIndex;
// #i44049#
class SwObjectFormatterTxtFrm;

void MakeFrms( SwDoc *, const SwNodeIndex &, const SwNodeIndex & );

/// Base class for frames that are allowed at page breaks and shall continue on the next page, e.g. paragraphs, tables.
class SwFlowFrm
{
    //PrepareMake darf Locken/Unlocken (Robustheit)
    friend inline void PrepareLock  ( SwFlowFrm * );
    friend inline void PrepareUnlock( SwFlowFrm * );
    friend inline void TableSplitRecalcLock( SwFlowFrm * );
    friend inline void TableSplitRecalcUnlock( SwFlowFrm * );
    // #i44049#
    friend class SwObjectFormatterTxtFrm;

    //TblSel darf das Follow-Bit zuruecksetzen.
    friend inline void UnsetFollow( SwFlowFrm *pFlow );

    friend void MakeFrms( SwDoc *, const SwNodeIndex &, const SwNodeIndex & );

    friend class SwNode2LayImpl;

    SwFrm &rThis;

    //Hilfsfunktionen fuer MoveSubTree()
    static SwLayoutFrm *CutTree( SwFrm* );
    static sal_Bool   PasteTree( SwFrm *, SwLayoutFrm *, SwFrm *, SwFrm* );

    //Wird fuer das Zusammenspiel von _GetPrevxxx und MoveBwd gebraucht, damit
    //mehrere Blaetter gleichzeitig uebersprungen werden koennen.
    //Wird auch vom MoveBwd des TabFrm ausgewertet!
    static sal_Bool bMoveBwdJump;

    /** helper method to determine previous frame for calculation of the
        upper space

        #i11860#

        @param _pProposedPrevFrm
        optional input parameter - pointer to frame, which should be used
        instead of the direct previous frame.
    */
    const SwFrm* _GetPrevFrmForUpperSpaceCalc( const SwFrm* _pProposedPrevFrm = 0L ) const;

    /** method to detemine the upper space amount, which is considered for
        the previous frame

        #i11860#
    */
    SwTwips _GetUpperSpaceAmountConsideredForPrevFrm() const;

    /** method to detemine the upper space amount, which is considered for
        the page grid

        #i11860#
    */
    SwTwips _GetUpperSpaceAmountConsideredForPageGrid(
                                const SwTwips _nUpperSpaceWithoutGrid ) const;

protected:

    SwFlowFrm *m_pFollow;
    SwFlowFrm *m_pPrecede;

    sal_Bool bLockJoin  :1; //Join (und damit deleten) verboten wenn sal_True!
    sal_Bool bUndersized:1; // wir sind kleiner als gewuenscht
    sal_Bool bFlyLock   :1; //  Stop positioning of at-character flyframes

    //Prueft ob Vorwaertsfluss noch Sinn macht Endloswanderschaften (unterbinden)
    inline sal_Bool IsFwdMoveAllowed();
    // #i44049# - method <CalcCntnt(..)> has to check this property.
    friend void CalcCntnt( SwLayoutFrm *pLay, bool bNoColl, bool bNoCalcFollow );
    sal_Bool IsKeepFwdMoveAllowed();    //Wie oben, Move fuer Keep.

    //Prueft ob ein Obj das Umlauf wuenscht ueberlappt.
    //eine Null bedeutet, kein Objekt ueberlappt,
    // 1 heisst, Objekte, die am FlowFrm selbst verankert sind, ueberlappen
    // 2 heisst, Objekte, die woanders verankert sind, ueberlappen
    // 3 heistt, beiderlei verankerte Objekte ueberlappen
    sal_uInt8 BwdMoveNecessary( const SwPageFrm *pPage, const SwRect &rRect );

    void LockJoin()   { bLockJoin = sal_True;  }
    void UnlockJoin() { bLockJoin = sal_False; }

            sal_Bool CheckMoveFwd( bool& rbMakePage, sal_Bool bKeep, sal_Bool bMovedBwd );
            sal_Bool MoveFwd( sal_Bool bMakePage, sal_Bool bPageBreak, sal_Bool bMoveAlways = sal_False );
    virtual sal_Bool ShouldBwdMoved( SwLayoutFrm *pNewUpper, sal_Bool bHead, sal_Bool &rReformat )=0;
            sal_Bool MoveBwd( sal_Bool &rbReformat );

public:
    SwFlowFrm( SwFrm &rFrm );
    virtual ~SwFlowFrm();

    const SwFrm *GetFrm() const            { return &rThis; }
          SwFrm *GetFrm()                  { return &rThis; }

    static sal_Bool IsMoveBwdJump()            { return bMoveBwdJump; }
    static void SetMoveBwdJump( sal_Bool bNew ){ bMoveBwdJump = bNew; }

    inline void SetUndersized( const sal_Bool bNew ) { bUndersized = bNew; }
    inline sal_Bool IsUndersized()  const { return bUndersized; }

    sal_Bool IsPrevObjMove() const;

    //Die Kette mit minimalen Operationen und Benachrichtigungen unter den
    //neuen Parent Moven.
    void MoveSubTree( SwLayoutFrm* pParent, SwFrm* pSibling = 0 );

           sal_Bool       HasFollow() const    { return m_pFollow ? sal_True : sal_False; }
           sal_Bool       IsFollow()     const { return 0 != m_pPrecede; }
    const  SwFlowFrm *GetFollow() const    { return m_pFollow;   }
           SwFlowFrm *GetFollow()          { return m_pFollow;   }
           sal_Bool       IsAnFollow( const SwFlowFrm *pFlow ) const;
           void       SetFollow(SwFlowFrm *const pFollow);

    const  SwFlowFrm *GetPrecede() const   { return m_pPrecede;   }
           SwFlowFrm *GetPrecede()         { return m_pPrecede;   }


    sal_Bool IsJoinLocked() const { return bLockJoin; }
    sal_Bool IsAnyJoinLocked() const { return bLockJoin || HasLockedFollow(); }

    sal_Bool IsPageBreak( sal_Bool bAct ) const;
    sal_Bool IsColBreak( sal_Bool bAct ) const;

    //Ist ein Keep zu beruecksichtigen (Breaks!)
    sal_Bool IsKeep( const SwAttrSet& rAttrs, bool bBreakCheck = false ) const;

    sal_Bool HasLockedFollow() const;

    sal_Bool HasParaSpaceAtPages( sal_Bool bSct ) const;

    /** method to determine the upper space hold by the frame

        #i11860# - add 3rd parameter <_bConsiderGrid> to get
        the upper space with and without considering the page grid
        (default value: <sal_True>)
    */
    SwTwips CalcUpperSpace( const SwBorderAttrs *pAttrs = NULL,
                            const SwFrm* pPr = NULL,
                            const bool _bConsiderGrid = true ) const;

    /** method to determine the upper space amount, which is considered for
        the previous frame and the page grid, if option 'Use former object
        positioning' is OFF

        #i11860#
    */
    SwTwips GetUpperSpaceAmountConsideredForPrevFrmAndPageGrid() const;

    /** calculation of lower space
    */
    SwTwips CalcLowerSpace( const SwBorderAttrs* _pAttrs = 0L ) const;

    /** calculation of the additional space to be considered, if flow frame
        is the last inside a table cell

        #i26250

        @param _pAttrs
        optional input parameter - border attributes of the flow frame.
        Used for optimization, if caller has already determined the border
        attributes.

        @return SwTwips
    */
    SwTwips CalcAddLowerSpaceAsLastInTableCell(
                                    const SwBorderAttrs* _pAttrs = 0L ) const;

    void CheckKeep();

    void SetFlyLock( sal_Bool bNew ){ bFlyLock = bNew; }
    sal_Bool IsFlyLock() const {    return bFlyLock; }

    //casten einen Frm auf einen FlowFrm - wenns denn einer ist, sonst 0
    //Diese Methoden muessen fuer neue Ableitungen geaendert werden!
    static       SwFlowFrm *CastFlowFrm( SwFrm *pFrm );
    static const SwFlowFrm *CastFlowFrm( const SwFrm *pFrm );
};

inline sal_Bool SwFlowFrm::IsFwdMoveAllowed()
{
    return rThis.GetIndPrev() != 0;
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
