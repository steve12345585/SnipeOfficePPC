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
#include "vbasystem.hxx"

#include <ooo/vba/word/WdCursorType.hpp>
#include <tools/diagnose_ex.h>
#include <tools/config.hxx>
#include <osl/file.hxx>
#include <tools/urlobj.hxx>

#ifdef WNT
#include <windows.h>
#include <tchar.h>
#endif

using namespace ::ooo::vba;
using namespace ::com::sun::star;

PrivateProfileStringListener::~PrivateProfileStringListener()
{
}

void PrivateProfileStringListener::Initialize( const rtl::OUString& rFileName, const rtl::OString& rGroupName, const rtl::OString& rKey )
{
    maFileName = rFileName;
    maGroupName = rGroupName;
    maKey = rKey;
}
#ifdef WNT
void lcl_getRegKeyInfo( const rtl::OString& sKeyInfo, HKEY& hBaseKey, rtl::OString& sSubKey )
{
    sal_Int32 nBaseKeyIndex = sKeyInfo.indexOf('\\');
    if( nBaseKeyIndex > 0 )
    {
        rtl::OString sBaseKey = sKeyInfo.copy( 0, nBaseKeyIndex );
        sSubKey = sKeyInfo.copy( nBaseKeyIndex + 1 );
        if( sBaseKey.equalsL(RTL_CONSTASCII_STRINGPARAM("HKEY_CURRENT_USER")) )
        {
            hBaseKey = HKEY_CURRENT_USER;
        }
        else if( sBaseKey.equalsL(RTL_CONSTASCII_STRINGPARAM("HKEY_LOCAL_MACHINE")) )
        {
            hBaseKey = HKEY_LOCAL_MACHINE;
        }
        else if( sBaseKey.equalsL(RTL_CONSTASCII_STRINGPARAM("HKEY_CLASSES_ROOT")) )
        {
            hBaseKey = HKEY_CLASSES_ROOT;
        }
        else if( sBaseKey.equalsL(RTL_CONSTASCII_STRINGPARAM("HKEY_USERS")) )
        {
            hBaseKey = HKEY_USERS;
        }
        else if( sBaseKey.equalsL(RTL_CONSTASCII_STRINGPARAM("HKEY_CURRENT_CONFIG")) )
        {
            hBaseKey = HKEY_CURRENT_CONFIG;
        }
    }
}
#endif

uno::Any PrivateProfileStringListener::getValueEvent()
{
    // get the private profile string
    rtl::OUString sValue;
    if(!maFileName.isEmpty())
    {
        // get key/value from a file
        Config aCfg( maFileName );
        aCfg.SetGroup( maGroupName );
        sValue = rtl::OStringToOUString(aCfg.ReadKey(maKey), RTL_TEXTENCODING_DONTKNOW);
    }
    else
    {
        // get key/value from windows register
#ifdef WNT
        HKEY hBaseKey = NULL;
        rtl::OString sSubKey;
        lcl_getRegKeyInfo( maGroupName, hBaseKey, sSubKey );
        if( hBaseKey != NULL )
        {
            HKEY hKey = NULL;
            LONG lResult;
            LPCTSTR lpSubKey = TEXT( sSubKey.getStr());
            TCHAR szBuffer[1024];
            DWORD cbData = sizeof( szBuffer );
            lResult = RegOpenKeyEx( hBaseKey, lpSubKey, 0, KEY_QUERY_VALUE, &hKey );
            if( ERROR_SUCCESS == lResult )
            {
                LPCTSTR lpValueName = TEXT(maKey.getStr());
                lResult = RegQueryValueEx( hKey, lpValueName, NULL, NULL, (LPBYTE)szBuffer, &cbData );
                RegCloseKey( hKey );
                sValue = rtl::OUString::createFromAscii(szBuffer);
            }
        }

        return uno::makeAny( sValue );
#else
        throw uno::RuntimeException( rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM("Only support on Windows")), uno::Reference< uno::XInterface >() );
#endif
    }

    return uno::makeAny( sValue );
}

void PrivateProfileStringListener::setValueEvent( const css::uno::Any& value )
{
    // set the private profile string
    rtl::OUString aValue;
    value >>= aValue;
    if(!maFileName.isEmpty())
    {
        // set value into a file
        Config aCfg( maFileName );
        aCfg.SetGroup( maGroupName );
        aCfg.WriteKey( maKey, rtl::OUStringToOString(aValue, RTL_TEXTENCODING_DONTKNOW) );
    }
    else
    {
        //set value into windows register
#ifdef WNT
        HKEY hBaseKey = NULL;
        rtl::OString sSubKey;
        lcl_getRegKeyInfo( maGroupName, hBaseKey, sSubKey );
        if( hBaseKey != NULL )
        {
            HKEY hKey = NULL;
            LONG lResult;
            LPCTSTR lpSubKey = TEXT( sSubKey.getStr());
            lResult = RegCreateKeyEx( hBaseKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL );
            if( ERROR_SUCCESS == lResult )
            {
                LPCTSTR szValue = TEXT( rtl::OUStringToOString( aValue, RTL_TEXTENCODING_UTF8 ).getStr() );
                DWORD cbData = sizeof(TCHAR) * (_tcslen(szValue) + 1);
                LPCTSTR lpValueName = TEXT(maKey.getStr());
                lResult = RegSetValueEx( hKey, lpValueName, 0 /* Reserved */, REG_SZ, (LPBYTE)szValue, cbData );
                RegCloseKey( hKey );
            }
        }
        return;
#else
        throw uno::RuntimeException( rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM("Not implemented")), uno::Reference< uno::XInterface >() );
#endif
    }

}

SwVbaSystem::SwVbaSystem( uno::Reference<uno::XComponentContext >& xContext ): SwVbaSystem_BASE( uno::Reference< XHelperInterface >(), xContext )
{
}

SwVbaSystem::~SwVbaSystem()
{
}

sal_Int32 SAL_CALL
SwVbaSystem::getCursor() throw (uno::RuntimeException)
{
    sal_Int32 nPointerStyle =  getPointerStyle( getCurrentWordDoc(mxContext) );

    switch( nPointerStyle )
    {
        case POINTER_ARROW:
            return word::WdCursorType::wdCursorNorthwestArrow;
        case POINTER_NULL:
            return word::WdCursorType::wdCursorNormal;
        case POINTER_WAIT:
            return word::WdCursorType::wdCursorWait;
        case POINTER_TEXT:
            return word::WdCursorType::wdCursorIBeam;
        default:
            return word::WdCursorType::wdCursorNormal;
    }
}

void SAL_CALL
SwVbaSystem::setCursor( sal_Int32 _cursor ) throw (uno::RuntimeException)
{
    try
    {
        switch( _cursor )
        {
            case word::WdCursorType::wdCursorNorthwestArrow:
            {
                const Pointer& rPointer( POINTER_ARROW );
                setCursorHelper( getCurrentWordDoc(mxContext), rPointer, sal_False );
                break;
            }
            case word::WdCursorType::wdCursorWait:
            {
                const Pointer& rPointer( static_cast< PointerStyle >( POINTER_WAIT ) );
                //It will set the edit window, toobar and statusbar's mouse pointer.
                setCursorHelper( getCurrentWordDoc(mxContext), rPointer, sal_True );
                break;
            }
            case word::WdCursorType::wdCursorIBeam:
            {
                const Pointer& rPointer( static_cast< PointerStyle >( POINTER_TEXT ) );
                //It will set the edit window, toobar and statusbar's mouse pointer.
                setCursorHelper( getCurrentWordDoc( mxContext ), rPointer, sal_True );
                break;
            }
            case word::WdCursorType::wdCursorNormal:
            {
                const Pointer& rPointer( POINTER_NULL );
                setCursorHelper( getCurrentWordDoc( mxContext ), rPointer, sal_False );
                break;
            }
            default:
                throw uno::RuntimeException( rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM("Unknown value for Cursor pointer")), uno::Reference< uno::XInterface >() );
                // TODO: isn't this a flaw in the API? It should be allowed to throw an
                // IllegalArgumentException, or so
        }
    }
    catch( const uno::Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
}

uno::Any SAL_CALL
SwVbaSystem::PrivateProfileString( const rtl::OUString& rFilename, const rtl::OUString& rSection, const rtl::OUString& rKey ) throw ( uno::RuntimeException )
{
    // FIXME: need to detect whether it is a relative file path
    // we need to detect if this is a URL, if not then assume its a file path
    rtl::OUString sFileUrl;
    if( !rFilename.isEmpty() )
    {
        INetURLObject aObj;
        aObj.SetURL( rFilename );
        bool bIsURL = aObj.GetProtocol() != INET_PROT_NOT_VALID;
        if ( bIsURL )
            sFileUrl = rFilename;
        else
            osl::FileBase::getFileURLFromSystemPath( rFilename, sFileUrl);
    }

    rtl::OString aGroupName(rtl::OUStringToOString(rSection, RTL_TEXTENCODING_DONTKNOW));
    rtl::OString aKey(rtl::OUStringToOString(rKey, RTL_TEXTENCODING_DONTKNOW));
    maPrivateProfileStringListener.Initialize( sFileUrl, aGroupName, aKey );

    return uno::makeAny( uno::Reference< XPropValue > ( new ScVbaPropValue( &maPrivateProfileStringListener ) ) );
}

rtl::OUString
SwVbaSystem::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SwVbaSystem"));
}

uno::Sequence< rtl::OUString >
SwVbaSystem::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.System" ) );
    }
    return aServiceNames;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
