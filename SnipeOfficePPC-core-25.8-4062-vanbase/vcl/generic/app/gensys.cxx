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

#include <generic/gensys.h>

#include <vcl/msgbox.hxx>
#include <vcl/button.hxx>

#include <svdata.hxx>

#include <rtl/ustrbuf.hxx>
#include <rtl/strbuf.hxx>
#include <rtl/bootstrap.hxx>
#include <osl/process.h>
#include <osl/thread.h>

#include "vcl/unohelp.hxx"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

using namespace com::sun::star;

namespace {

OUString GetNativeMessageBoxButtonText( int nButtonId, bool bUseResources )
{
    OUString aText;
    if( bUseResources )
    {
        aText = Button::GetStandardText( nButtonId );
    }
    if( aText.isEmpty() )
    {
        switch( nButtonId )
        {
        case BUTTON_OK:
            aText = "OK";
            break;
        case BUTTON_CANCEL:
            aText = "Cancel";
            break;
        case BUTTON_ABORT:
            aText = "Abort";
            break;
        case BUTTON_RETRY:
            aText = "Retry";
            break;
        case BUTTON_IGNORE:
            aText = "Ignore";
            break;
        case BUTTON_YES:
            aText = "Yes";
            break;
        case BUTTON_NO:
            aText = "No";
            break;
        }
    }
    return aText;
}

}

SalGenericSystem::SalGenericSystem()
{
}

SalGenericSystem::~SalGenericSystem()
{
}

int SalGenericSystem::ShowNativeMessageBox( const rtl::OUString& rTitle, const rtl::OUString& rMessage,
                                            int nButtonCombination, int nDefaultButton,
                                            bool bUseResources )
{
    int nDefButton = 0;
    std::list< rtl::OUString > aButtons;
    int nButtonIds[5], nBut = 0;

    ImplHideSplash();

    if( nButtonCombination == SALSYSTEM_SHOWNATIVEMSGBOX_BTNCOMBI_OK ||
        nButtonCombination == SALSYSTEM_SHOWNATIVEMSGBOX_BTNCOMBI_OK_CANCEL )
    {
        aButtons.push_back( GetNativeMessageBoxButtonText( BUTTON_OK, bUseResources ) );
        nButtonIds[nBut++] = SALSYSTEM_SHOWNATIVEMSGBOX_BTN_OK;
    }
    if( nButtonCombination == SALSYSTEM_SHOWNATIVEMSGBOX_BTNCOMBI_YES_NO_CANCEL ||
        nButtonCombination == SALSYSTEM_SHOWNATIVEMSGBOX_BTNCOMBI_YES_NO )
    {
        aButtons.push_back( GetNativeMessageBoxButtonText( BUTTON_YES, bUseResources ) );
        nButtonIds[nBut++] = SALSYSTEM_SHOWNATIVEMSGBOX_BTN_YES;
        aButtons.push_back( GetNativeMessageBoxButtonText( BUTTON_NO, bUseResources ) );
        nButtonIds[nBut++] = SALSYSTEM_SHOWNATIVEMSGBOX_BTN_NO;
        if( nDefaultButton == SALSYSTEM_SHOWNATIVEMSGBOX_BTN_NO )
            nDefButton = 1;
    }
    if( nButtonCombination == SALSYSTEM_SHOWNATIVEMSGBOX_BTNCOMBI_OK_CANCEL ||
        nButtonCombination == SALSYSTEM_SHOWNATIVEMSGBOX_BTNCOMBI_YES_NO_CANCEL ||
        nButtonCombination == SALSYSTEM_SHOWNATIVEMSGBOX_BTNCOMBI_RETRY_CANCEL )
    {
        if( nButtonCombination == SALSYSTEM_SHOWNATIVEMSGBOX_BTNCOMBI_RETRY_CANCEL )
        {
            aButtons.push_back( GetNativeMessageBoxButtonText( BUTTON_RETRY, bUseResources ) );
            nButtonIds[nBut++] = SALSYSTEM_SHOWNATIVEMSGBOX_BTN_RETRY;
        }
        aButtons.push_back( GetNativeMessageBoxButtonText( BUTTON_CANCEL, bUseResources ) );
        nButtonIds[nBut++] = SALSYSTEM_SHOWNATIVEMSGBOX_BTN_CANCEL;
        if( nDefaultButton == SALSYSTEM_SHOWNATIVEMSGBOX_BTN_CANCEL )
            nDefButton = aButtons.size()-1;
    }
    if( nButtonCombination == SALSYSTEM_SHOWNATIVEMSGBOX_BTNCOMBI_ABORT_RETRY_IGNORE )
    {
        aButtons.push_back( GetNativeMessageBoxButtonText( BUTTON_ABORT, bUseResources ) );
        nButtonIds[nBut++] = SALSYSTEM_SHOWNATIVEMSGBOX_BTN_ABORT;
        aButtons.push_back( GetNativeMessageBoxButtonText( BUTTON_RETRY, bUseResources ) );
        nButtonIds[nBut++] = SALSYSTEM_SHOWNATIVEMSGBOX_BTN_RETRY;
        aButtons.push_back( GetNativeMessageBoxButtonText( BUTTON_IGNORE, bUseResources ) );
        nButtonIds[nBut++] = SALSYSTEM_SHOWNATIVEMSGBOX_BTN_IGNORE;
        switch( nDefaultButton )
        {
            case SALSYSTEM_SHOWNATIVEMSGBOX_BTN_RETRY: nDefButton = 1;break;
            case SALSYSTEM_SHOWNATIVEMSGBOX_BTN_IGNORE: nDefButton = 2;break;
        }
    }

    int nResult = ShowNativeDialog( rTitle, rMessage, aButtons, nDefButton );

    return nResult != -1 ? nButtonIds[ nResult ] : 0;
}

// ------------------------------------------------------------------------
//           Helpers primarily for X Windowing derivatives
// ------------------------------------------------------------------------

const char* SalGenericSystem::getFrameResName()
{
    /*  according to ICCCM:
     *  first search command line for -name parameter
     *  then try RESOURCE_NAME environment variable
     *  then use argv[0] stripped by directories
     */
    static rtl::OStringBuffer aResName;
    if( !aResName.getLength() )
    {
        int nArgs = osl_getCommandArgCount();
        for( int n = 0; n < nArgs-1; n++ )
        {
            rtl::OUString aArg;
            if( ! osl_getCommandArg( n, &aArg.pData ) &&
                aArg.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("-name")) &&
                ! osl_getCommandArg( n+1, &aArg.pData ) )
            {
                aResName.append( rtl::OUStringToOString( aArg, osl_getThreadTextEncoding() ) );
                break;
            }
        }
        if( !aResName.getLength() )
        {
            const char* pEnv = getenv( "RESOURCE_NAME" );
            if( pEnv && *pEnv )
                aResName.append( pEnv );
        }
        if( !aResName.getLength() )
            aResName.append( "VCLSalFrame" );
    }
    return aResName.getStr();
}

const char* SalGenericSystem::getFrameClassName()
{
    static rtl::OStringBuffer aClassName;
    if( !aClassName.getLength() )
    {
        rtl::OUString aIni, aProduct;
        rtl::Bootstrap::get( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "BRAND_BASE_DIR" ) ), aIni );
        aIni += ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/program/" SAL_CONFIGFILE( "bootstrap" ) ) );
        rtl::Bootstrap aBootstrap( aIni );
        aBootstrap.getFrom( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ProductKey" ) ), aProduct );

        if( !aProduct.isEmpty() )
            aClassName.append( rtl::OUStringToOString( aProduct, osl_getThreadTextEncoding() ) );
        else
            aClassName.append( "VCLSalFrame" );
    }
    return aClassName.getStr();
}

rtl::OString SalGenericSystem::getFrameResName( SalExtStyle nStyle )
{
    rtl::OStringBuffer aBuf( 64 );
    aBuf.append( getFrameResName() );
    if( (nStyle & SAL_FRAME_EXT_STYLE_DOCUMENT) )
        aBuf.append( ".DocumentWindow" );

    return aBuf.makeStringAndClear();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
