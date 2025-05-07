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

#ifndef _DDECTRL_HXX
#define _DDECTRL_HXX

#include <tools/link.hxx>
#include <basic/sberrors.hxx>
#include <tools/string.hxx>

class DdeConnection;
class DdeData;

class SbiDdeControl
{
private:
    DECL_LINK( Data, DdeData* );
    SbError GetLastErr( DdeConnection* );
    sal_Int16 GetFreeChannel();
    std::vector<DdeConnection*> aConvList;
    String aData;

public:

    SbiDdeControl();
    ~SbiDdeControl();

    SbError Initiate( const String& rService, const String& rTopic,
                     sal_Int16& rnHandle );
    SbError Terminate( sal_uInt16 nChannel );
    SbError TerminateAll();
    SbError Request( sal_uInt16 nChannel, const String& rItem, String& rResult );
    SbError Execute( sal_uInt16 nChannel, const String& rCommand );
    SbError Poke( sal_uInt16 nChannel, const String& rItem, const String& rData );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
