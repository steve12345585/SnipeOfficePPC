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


#include "swcache.hxx"
#include "fntcache.hxx"     // pFntCache ( SwFont/ScrFont-PrtFont cache )
#include "swfntcch.hxx"     // pSwFontCache ( SwAttrSet/SwFont cache )
#include "txtfrm.hxx"
#include "txtcache.hxx"
#include "porlay.hxx"
#include "porglue.hxx"
#include "porexp.hxx"
#include "porrst.hxx"
#include "portab.hxx"
#include "porfly.hxx"
#include "portox.hxx"
#include "porref.hxx"
#include "porftn.hxx"
#include "porhyph.hxx"
#include "pordrop.hxx"
#include "blink.hxx"    // Blink manager
#include "init.hxx"     // Declarations for _TextInit() and _TextFinit()
#include "txtfly.hxx"   // SwContourCache
#include "dbg_lay.hxx"  // Layout Debug file output

SwCache *SwTxtFrm::pTxtCache = 0;
long SwTxtFrm::nMinPrtLine = 0;
SwContourCache *pContourCache = 0;
SwDropCapCache *pDropCapCache = 0;

IMPL_FIXEDMEMPOOL_NEWDEL( SwTxtLine )
IMPL_FIXEDMEMPOOL_NEWDEL( SwParaPortion ) // Paragraphs
IMPL_FIXEDMEMPOOL_NEWDEL( SwLineLayout ) // Lines
IMPL_FIXEDMEMPOOL_NEWDEL( SwHolePortion ) // e.g. Blanks at the line end
IMPL_FIXEDMEMPOOL_NEWDEL( SwTxtPortion ) // Attribute change

/*************************************************************************
 *                  _TextInit(), _TextFinit()
 *************************************************************************/

// Are ONLY used in init.cxx.
// There we have extern void _TextFinit()
// and extern void _TextInit(...)

void _TextInit()
{
    pFntCache = new SwFntCache; // Cache for SwSubFont -> SwFntObj = { Font aFont, Font* pScrFont, Font* pPrtFont, OutputDevice* pPrinter, ... }
    pSwFontCache = new SwFontCache; // Cache for SwTxtFmtColl -> SwFontObj = { SwFont aSwFont, SfxPoolItem* pDefaultArray }
    SwCache *pTxtCache = new SwCache( 250 // Cache for SwTxtFrm -> SwTxtLine = { SwParaPortion* pLine }
#ifdef DBG_UTIL
    , "static SwTxtFrm::pTxtCache"
#endif
    );
    SwTxtFrm::SetTxtCache( pTxtCache );
    pWaveCol = new Color( COL_GRAY );
    PROTOCOL_INIT
}

void _TextFinit()
{
    PROTOCOL_STOP
    delete SwTxtFrm::GetTxtCache();
    delete pSwFontCache;
    delete pFntCache;
    delete pBlink;
    delete pWaveCol;
    delete pContourCache;
    SwDropPortion::DeleteDropCapCache();
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
