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

#ifndef _FTNBOSS_HXX
#define _FTNBOSS_HXX

#include "layfrm.hxx"

class SwFtnBossFrm;
class SwFtnContFrm;
class SwFtnFrm;
class SwTxtFtn;

// Set max. footnote area.
// Restoration of the old value in DTor. Implementation in ftnfrm.cxx
class SwSaveFtnHeight
{
    SwFtnBossFrm *pBoss;
    const SwTwips nOldHeight;
    SwTwips nNewHeight;
public:
    SwSaveFtnHeight( SwFtnBossFrm *pBs, const SwTwips nDeadLine );
    ~SwSaveFtnHeight();
};

#define NA_ONLY_ADJUST 0
#define NA_GROW_SHRINK 1
#define NA_GROW_ADJUST 2
#define NA_ADJUST_GROW 3

class SwFtnBossFrm: public SwLayoutFrm
{
    // for private footnote operations
    friend class SwFrm;
    friend class SwSaveFtnHeight;
    friend class SwPageFrm; // for setting of MaxFtnHeight

    // max. height of the footnote container on this page
    SwTwips nMaxFtnHeight;

    SwFtnContFrm *MakeFtnCont();
    SwFtnFrm     *FindFirstFtn();
    sal_uInt8 _NeighbourhoodAdjustment( const SwFrm* pFrm ) const;

protected:
    void          InsertFtn( SwFtnFrm * );
    static void   ResetFtn( const SwFtnFrm *pAssumed );

public:
    inline SwFtnBossFrm( SwFrmFmt* pFmt, SwFrm* pSib ) : SwLayoutFrm( pFmt, pSib ) {}

                 SwLayoutFrm *FindBodyCont();
    inline const SwLayoutFrm *FindBodyCont() const;
    inline void SetMaxFtnHeight( const SwTwips nNewMax ) { nMaxFtnHeight = nNewMax; }

    // footnote interface
    void AppendFtn( SwCntntFrm *, SwTxtFtn * );
    void RemoveFtn( const SwCntntFrm *, const SwTxtFtn *, sal_Bool bPrep = sal_True );
    static       SwFtnFrm     *FindFtn( const SwCntntFrm *, const SwTxtFtn * );
                 SwFtnContFrm *FindFtnCont();
    inline const SwFtnContFrm *FindFtnCont() const;
           const SwFtnFrm     *FindFirstFtn( SwCntntFrm* ) const;
                 SwFtnContFrm *FindNearestFtnCont( sal_Bool bDontLeave = sal_False );

    void ChangeFtnRef( const SwCntntFrm *pOld, const SwTxtFtn *,
                       SwCntntFrm *pNew );
    void RearrangeFtns( const SwTwips nDeadLine, const sal_Bool bLock = sal_False,
                        const SwTxtFtn *pAttr = 0 );

    // Set DeadLine (in document coordinates) so that the text formatter can
    // temporarily limit footnote height.
    void    SetFtnDeadLine( const SwTwips nDeadLine );
    SwTwips GetMaxFtnHeight() const { return nMaxFtnHeight; }

    // returns value for remaining space until the body reaches minimal height
    SwTwips GetVarSpace() const;

    // methods needed for layouting
    // The parameters <_bCollectOnlyPreviousFtns> and <_pRefFtnBossFrm> control
    // if only footnotes that are positioned before the given reference
    // footnote boss-frame have to be collected.
    // Note: if parameter <_bCollectOnlyPreviousFtns> is true, then parameter
    // <_pRefFtnBossFrm> has to be referenced by an object.
    static void _CollectFtns( const SwCntntFrm*   _pRef,
                              SwFtnFrm*           _pFtn,
                              SvPtrarr&           _rFtnArr,
                              const sal_Bool      _bCollectOnlyPreviousFtns = sal_False,
                              const SwFtnBossFrm* _pRefFtnBossFrm = NULL);
    // The parameter <_bCollectOnlyPreviousFtns> controls if only footnotes
    // that are positioned before the footnote boss-frame <this> have to be
    // collected.
    void    CollectFtns( const SwCntntFrm* _pRef,
                         SwFtnBossFrm*     _pOld,
                         SvPtrarr&         _rFtnArr,
                         const sal_Bool    _bCollectOnlyPreviousFtns = sal_False );
    void    _MoveFtns( SvPtrarr &rFtnArr, sal_Bool bCalc = sal_False );
    void    MoveFtns( const SwCntntFrm *pSrc, SwCntntFrm *pDest,
                      SwTxtFtn *pAttr );

    // should AdjustNeighbourhood be called (or Grow/Shrink)?
    sal_uInt8 NeighbourhoodAdjustment( const SwFrm* pFrm ) const
        { return IsPageFrm() ? NA_ONLY_ADJUST : _NeighbourhoodAdjustment( pFrm ); }
};

inline const SwLayoutFrm *SwFtnBossFrm::FindBodyCont() const
{
    return ((SwFtnBossFrm*)this)->FindBodyCont();
}

inline const SwFtnContFrm *SwFtnBossFrm::FindFtnCont() const
{
    return ((SwFtnBossFrm*)this)->FindFtnCont();
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
