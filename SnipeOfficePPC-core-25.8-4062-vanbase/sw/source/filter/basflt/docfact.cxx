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

#include <shellio.hxx>
#include <doc.hxx>
#include <node.hxx>

#include <cmdid.h>


/******************************************************************************
 *  Methode     :   SwDocFac::SwDocFac( SwDoc *pDoc )
 ******************************************************************************/

SwDocFac::SwDocFac( SwDoc *pDc )
    : pDoc( pDc )
{
    if( pDoc )
        pDoc->acquire();
}

/******************************************************************************
 *  Methode     :   SwDocFac::~SwDocFac()
 ******************************************************************************/


SwDocFac::~SwDocFac()
{
    if( pDoc && !pDoc->release() )
        delete pDoc;
}

/******************************************************************************
 *  Methode     :   SwDoc *SwDocFac::GetDoc()
 *  Beschreibung:   Diese Methode legt immer einen Drucker an.
 ******************************************************************************/

SwDoc *SwDocFac::GetDoc()
{
    if( !pDoc )
    {
        pDoc = new SwDoc;
        pDoc->acquire();
    }
    return pDoc;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
