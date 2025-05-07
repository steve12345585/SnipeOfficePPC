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


#include <unotools/transliterationwrapper.hxx>
#include <tools/debug.hxx>
#include <i18npool/mslangid.hxx>

#include "instance.hxx"
#include <com/sun/star/i18n/TransliterationModulesExtra.hpp>

using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::i18n;
using namespace ::com::sun::star::uno;
using namespace ::utl;

TransliterationWrapper::TransliterationWrapper(
                    const Reference< XMultiServiceFactory > & xSF,
                    sal_uInt32 nTyp )
    : xSMgr( xSF ), nType( nTyp ), nLanguage( 0 ), bFirstCall( sal_True )
{
    xTrans = Reference< XExtendedTransliteration > (
        intl_createInstance( xSMgr, "com.sun.star.i18n.Transliteration",
                             "TransliterationWrapper" ), UNO_QUERY );
    DBG_ASSERT( xTrans.is(), "TransliterationWrapper: no Transliteraion available" );
}


TransliterationWrapper::~TransliterationWrapper()
{
}


String TransliterationWrapper::transliterate(
                                const String& rStr, sal_uInt16 nLang,
                                xub_StrLen nStart, xub_StrLen nLen,
                                Sequence <sal_Int32>* pOffset )
{
    String sRet;
    if( xTrans.is() )
    {
        try
        {
            loadModuleIfNeeded( nLang );

            if ( pOffset )
                sRet = xTrans->transliterate( rStr, nStart, nLen, *pOffset );
            else
                sRet = xTrans->transliterateString2String( rStr, nStart, nLen);
        }
        catch( Exception&  )
        {
            SAL_WARN( "unotools", "transliterate: Exception caught!" );
        }
    }
    return sRet;
}


String TransliterationWrapper::transliterate(
                                const String& rStr,
                                xub_StrLen nStart, xub_StrLen nLen,
                                Sequence <sal_Int32>* pOffset ) const
{
    String sRet( rStr );
    if( xTrans.is() )
    {
        try
        {
            if ( pOffset )
                sRet = xTrans->transliterate( rStr, nStart, nLen, *pOffset );
            else
                sRet = xTrans->transliterateString2String( rStr, nStart, nLen);
        }
        catch( Exception&  )
        {
            SAL_WARN( "unotools", "transliterate: Exception caught!" );
        }
    }
    return sRet;
}

sal_Bool TransliterationWrapper::needLanguageForTheMode() const
{
    return TransliterationModules_UPPERCASE_LOWERCASE == nType ||
           TransliterationModules_LOWERCASE_UPPERCASE == nType ||
           TransliterationModules_IGNORE_CASE == nType ||
           (sal_uInt32) TransliterationModulesExtra::SENTENCE_CASE == (sal_uInt32) nType ||
           (sal_uInt32) TransliterationModulesExtra::TITLE_CASE    == (sal_uInt32) nType ||
           (sal_uInt32) TransliterationModulesExtra::TOGGLE_CASE   == (sal_uInt32) nType;
}


void TransliterationWrapper::setLanguageLocaleImpl( sal_uInt16 nLang )
{
    nLanguage = nLang;
    if( LANGUAGE_NONE == nLanguage )
        nLanguage = LANGUAGE_SYSTEM;
    MsLangId::convertLanguageToLocale( nLanguage, aLocale);
}


void TransliterationWrapper::loadModuleIfNeeded( sal_uInt16 nLang )
{
    sal_Bool bLoad = bFirstCall;
    bFirstCall = sal_False;

    if( static_cast< sal_Int32 >(nType) == TransliterationModulesExtra::SENTENCE_CASE )
    {
        if( bLoad )
            loadModuleByImplName(String::CreateFromAscii("SENTENCE_CASE"), nLang);
    }
    else if( static_cast< sal_Int32 >(nType) == TransliterationModulesExtra::TITLE_CASE )
    {
        if( bLoad )
            loadModuleByImplName(String::CreateFromAscii("TITLE_CASE"), nLang);
    }
    else if( static_cast< sal_Int32 >(nType) == TransliterationModulesExtra::TOGGLE_CASE )
    {
        if( bLoad )
            loadModuleByImplName(String::CreateFromAscii("TOGGLE_CASE"), nLang);
    }
    else
    {
        if( nLanguage != nLang )
        {
            setLanguageLocaleImpl( nLang );
            if( !bLoad )
                bLoad = needLanguageForTheMode();
        }
        if( bLoad )
            loadModuleImpl();
    }
}


void TransliterationWrapper::loadModuleImpl() const
{
    if ( bFirstCall )
        ((TransliterationWrapper*)this)->setLanguageLocaleImpl( LANGUAGE_SYSTEM );

    try
    {
        if ( xTrans.is() )
            xTrans->loadModule( (TransliterationModules)nType, aLocale );
    }
    catch ( const Exception& e )
    {
        SAL_WARN( "unotools.i18n", "loadModuleImpl: Exception caught " << e.Message );
    }

    bFirstCall = sal_False;
}


void TransliterationWrapper::loadModuleByImplName(
        const String& rModuleName, sal_uInt16 nLang )
{
    try
    {
        setLanguageLocaleImpl( nLang );
        // Reset LanguageType, so the next call to loadModuleIfNeeded() forces
        // new settings.
        nLanguage = LANGUAGE_DONTKNOW;
        if ( xTrans.is() )
            xTrans->loadModuleByImplName( rModuleName, aLocale );
    }
    catch ( const Exception& e )
    {
        SAL_WARN( "unotools.i18n", "loadModuleByImplName: Exception caught " << e.Message );
    }

    bFirstCall = sal_False;
}


sal_Bool TransliterationWrapper::equals(
    const String& rStr1, sal_Int32 nPos1, sal_Int32 nCount1, sal_Int32& nMatch1,
    const String& rStr2, sal_Int32 nPos2, sal_Int32 nCount2, sal_Int32& nMatch2 ) const
{
    try
    {
        if( bFirstCall )
            loadModuleImpl();
        if ( xTrans.is() )
            return xTrans->equals( rStr1, nPos1, nCount1, nMatch1, rStr2, nPos2, nCount2, nMatch2 );
    }
    catch ( const Exception& e )
    {
        SAL_WARN( "unotools.i18n", "equals: Exception caught " << e.Message );
    }
    return sal_False;
}

sal_Int32 TransliterationWrapper::compareString( const String& rStr1, const String& rStr2 ) const
{
    try
    {
        if( bFirstCall )
            loadModuleImpl();
        if ( xTrans.is() )
            return xTrans->compareString( rStr1, rStr2 );
    }
    catch (const Exception& e)
    {
        SAL_WARN( "unotools.i18n", "compareString: Exception caught " << e.Message );
    }
    return 0;
}


// --- helpers --------------------------------------------------------

sal_Bool TransliterationWrapper::isEqual( const String& rStr1, const String& rStr2 ) const
{
    sal_Int32 nMatch1, nMatch2;
    sal_Bool bMatch = equals(
        rStr1, 0, rStr1.Len(), nMatch1,
        rStr2, 0, rStr2.Len(), nMatch2 );
    return bMatch;
}


sal_Bool TransliterationWrapper::isMatch( const String& rStr1, const String& rStr2 ) const
{
    sal_Int32 nMatch1, nMatch2;
    equals(
        rStr1, 0, rStr1.Len(), nMatch1,
        rStr2, 0, rStr2.Len(), nMatch2 );
    return (nMatch1 <= nMatch2) && (nMatch1 == rStr1.Len());
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
