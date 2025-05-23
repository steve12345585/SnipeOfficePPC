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
#ifndef _BODYFRM_HXX
#define _BODYFRM_HXX

#include <tools/mempool.hxx>
#include "layfrm.hxx"

class SwBorderAttrs;

class SwBodyFrm: public SwLayoutFrm
{
protected:
    virtual void Format( const SwBorderAttrs *pAttrs = 0 );

#if defined ( GCC) && defined ( C272 )
    ~SwBodyFrm();
#endif

public:
    SwBodyFrm( SwFrmFmt*, SwFrm* );

    virtual void Paint( const SwRect&, const SwPrintData *pPrintData = NULL ) const;
    virtual void PaintSubsidiaryLines( const SwPageFrm*, const SwRect& ) const;

    DECL_FIXEDMEMPOOL_NEWDEL(SwBodyFrm)
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
