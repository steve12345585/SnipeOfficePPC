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


#include <sot/storage.hxx>

/*
 * main.
 */
int main (int argc, sal_Char **argv)
{
     SotStorageRef xStor = new SotStorage( "c:\\temp\\65254.ppt" );
/*
    SotStorageRef xStor = new SotStorage( "c:\\temp\\video.sdb" );
    SotStorageRef xStor = new SotStorage( "c:\\temp\\video.sdb" );
    SotStorageRef x2Stor = xStor->OpenSotStorage( "1117" );

    SotStorageStreamRef xStm = x2Stor->OpenSotStream( "Genres", STREAM_STD_READWRITE | STREAM_TRUNC);
    //BYTE szData[100];
    //xStm->Write( szData, 100 );
    UINT32 nSize = xStm->GetSize();
    xStm->SetSize( 0 );
    nSize = xStm->GetSize();
    x2Stor->Commit();
    xStor->Commit();
*/
    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
