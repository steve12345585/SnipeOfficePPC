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

#ifndef _BLINK_HXX
#define _BLINK_HXX

class SwLinePortion;
class SwRootFrm;
class SwTxtFrm;

#include <vcl/timer.hxx>
#include <tools/gen.hxx>
#include <boost/ptr_container/ptr_set.hpp>

class SwBlinkPortion
{
    Point               aPos;
    const SwLinePortion *pPor;
    const SwRootFrm     *pFrm;
    sal_uInt16              nDir;

public:
    SwBlinkPortion( const SwLinePortion* pPortion, sal_uInt16 nDirection )
            { pPor = pPortion; nDir = nDirection; }
    SwBlinkPortion( const SwBlinkPortion* pBlink, const SwLinePortion* pPort )
    {   pPor = pPort; pFrm = pBlink->pFrm; aPos = pBlink->aPos; nDir = pBlink->nDir; }
    void SetPos( const Point& aNew ){ aPos = aNew; }
    const Point& GetPos() const{ return aPos; }
    void SetRootFrm( const SwRootFrm* pNew ){ pFrm = pNew; }
    const SwRootFrm* GetRootFrm() const{ return pFrm; }
    const SwLinePortion *GetPortion() const{ return pPor; }
    sal_uInt16 GetDirection() const { return nDir; }
    sal_Bool operator<( const SwBlinkPortion& rBlinkPortion ) const
    { return (long)pPor < (long)rBlinkPortion.pPor; }
    sal_Bool operator==( const SwBlinkPortion& rBlinkPortion ) const
    { return (long)pPor == (long)rBlinkPortion.pPor; }
};

class SwBlinkList : public boost::ptr_set<SwBlinkPortion> {};

class SwBlink
{
    SwBlinkList     aList;
    AutoTimer       aTimer;
    sal_Bool            bVisible;

public:
    SwBlink();
    ~SwBlink();

    DECL_LINK( Blinker, void * );

    void Insert( const Point& rPoint, const SwLinePortion* pPor,
                 const SwTxtFrm *pTxtFrm, sal_uInt16 nDir );
    void Replace( const SwLinePortion* pOld, const SwLinePortion* pNew );
    void Delete( const SwLinePortion* pPor );
    void FrmDelete( const SwRootFrm* pRoot );
    inline sal_Bool IsVisible() const { return bVisible ; }
};

// Blink-Manager, global variable, see Blink.Cxx
extern SwBlink *pBlink;


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
