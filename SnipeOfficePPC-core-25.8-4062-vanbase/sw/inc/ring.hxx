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
#ifndef _RING_HXX
#define _RING_HXX

#include <swdllapi.h>
#include <swtypes.hxx>

class SW_DLLPUBLIC Ring
{
    Ring *pNext;
    Ring* pPrev;    ///< In order to speed up inserting and deleting.

protected:
    Ring()          { pNext = this; pPrev = this; }
    Ring( Ring * );
public:
    virtual ~Ring();
    void MoveTo( Ring *pDestRing );
    void MoveRingTo( Ring *pDestRing );

    Ring* GetNext() const       { return pNext; }
    Ring* GetPrev() const       { return pPrev; }

    sal_uInt32 numberOf() const;
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
