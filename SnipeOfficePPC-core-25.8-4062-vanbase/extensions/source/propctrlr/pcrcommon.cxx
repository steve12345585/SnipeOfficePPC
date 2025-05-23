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

#include "pcrcommon.hxx"
#include "modulepcr.hxx"
#include "propresid.hrc"

#include <com/sun/star/util/MeasureUnit.hpp>
#include <rtl/ustrbuf.hxx>
#include <tools/urlobj.hxx>

//............................................................................
namespace pcr
{
//............................................................................

    using namespace ::com::sun::star::util;

    //========================================================================
    //= HelpIdUrl
    //========================================================================
    //------------------------------------------------------------------------
    rtl::OString HelpIdUrl::getHelpId( const ::rtl::OUString& _rHelpURL )
    {
        INetURLObject aHID( _rHelpURL );
        if ( aHID.GetProtocol() == INET_PROT_HID )
              return rtl::OUStringToOString( aHID.GetURLPath(), RTL_TEXTENCODING_UTF8 );
        else
            return rtl::OUStringToOString( _rHelpURL, RTL_TEXTENCODING_UTF8 );
    }

    //------------------------------------------------------------------------
    ::rtl::OUString HelpIdUrl::getHelpURL( const rtl::OString& sHelpId )
    {
        ::rtl::OUStringBuffer aBuffer;
        ::rtl::OUString aTmp( rtl::OStringToOUString(sHelpId, RTL_TEXTENCODING_UTF8) );
        INetURLObject aHID( aTmp );
        if ( aHID.GetProtocol() == INET_PROT_NOT_VALID )
            aBuffer.appendAscii( INET_HID_SCHEME );
        aBuffer.append( aTmp.getStr() );
        return aBuffer.makeStringAndClear();
    }
//............................................................................
} // namespace pcr
//............................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
