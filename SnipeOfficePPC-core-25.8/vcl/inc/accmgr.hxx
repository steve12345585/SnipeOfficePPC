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

#ifndef _SV_ACCMGR_HXX
#define _SV_ACCMGR_HXX

#include <vcl/sv.h>
#include <vector>

class Accelerator;
class KeyCode;

typedef ::std::vector< Accelerator* > ImplAccelList;

// --------------------
// - ImplAccelManager -
// --------------------

class ImplAccelManager
{
private:
    ImplAccelList*      mpAccelList;
    ImplAccelList*      mpSequenceList;

public:
                        ImplAccelManager()
                        {
                            mpAccelList    = NULL;
                            mpSequenceList = NULL;
                        }
                        ~ImplAccelManager();

    sal_Bool                InsertAccel( Accelerator* pAccel );
    void                RemoveAccel( Accelerator* pAccel );

    void                EndSequence( sal_Bool bCancel = sal_False );
    void                FlushAccel() { EndSequence( sal_True ); }

    sal_Bool                IsAccelKey( const KeyCode& rKeyCode, sal_uInt16 nRepeat );
};

#endif  // _SV_ACCMGR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
