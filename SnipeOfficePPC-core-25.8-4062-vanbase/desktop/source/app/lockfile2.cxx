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


#include "vcl/msgbox.hxx"
#include "desktopresid.hxx"
#include "desktop.hrc"
#include "tools/config.hxx"
#include "lockfile.hxx"


namespace desktop {

bool Lockfile_execWarning( Lockfile * that )
{
    // read information from lock
    String aLockname = that->m_aLockname;
    Config aConfig(aLockname);
    aConfig.SetGroup( LOCKFILE_GROUP );
    rtl::OString aHost  = aConfig.ReadKey( LOCKFILE_HOSTKEY );
    rtl::OString aUser  = aConfig.ReadKey( LOCKFILE_USERKEY );
    rtl::OString aStamp = aConfig.ReadKey( LOCKFILE_STAMPKEY );
    rtl::OString aTime  = aConfig.ReadKey( LOCKFILE_TIMEKEY );

    // display warning and return response
    QueryBox aBox( NULL, DesktopResId( QBX_USERDATALOCKED ) );
    // set box title
    String aTitle = String( DesktopResId( STR_TITLE_USERDATALOCKED ));
    aBox.SetText( aTitle );
    // insert values...
    String aMsgText = aBox.GetMessText( );
    aMsgText.SearchAndReplaceAscii(
        "$u", rtl::OStringToOUString( aUser, RTL_TEXTENCODING_ASCII_US) );
    aMsgText.SearchAndReplaceAscii(
        "$h", rtl::OStringToOUString( aHost, RTL_TEXTENCODING_ASCII_US) );
    aMsgText.SearchAndReplaceAscii(
        "$t", rtl::OStringToOUString( aTime, RTL_TEXTENCODING_ASCII_US) );
    aBox.SetMessText(aMsgText);
    // do it
    return aBox.Execute( ) == RET_YES;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
